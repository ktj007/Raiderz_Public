/******************************************************************
   
   ZUpdate.cpp

     Corperation : MAIET entertainment
     Programmer  : Lim Dong Hwan
	 Date        : 22.June.2005

*******************************************************************/


#include "stdafx.h"
#include "ZGlobal.h"
#include "ZUpdate.h"
#include "MXml.h"
#include "FileInfo.h"
#include <shlwapi.h>
#include <json/json.h>
#include "zlib/zip.h"
#include "zlib/unzip.h"

#pragma warning( disable : 4996 )


// 현재 폴더 이름이 필터링된 파일 이름을 구함
CString GetFilteredFileName( const char* szFileName)
{
	CString str = szFileName;

	char szDir[ _MAX_DIR];
	GetCurrentDirectory( sizeof( szDir), szDir);

	if ( strncmp( szFileName, szDir, strlen( szDir)) == 0)
	{
		str = ".";
		str += szFileName + (int)strlen( szDir);
	}

	return str;
}

// 파일의 CRC를 구함
DWORD GetCRC( const char* szFileName, CString* pstrErrorMsg)
{
	DWORD dwCRC = 0;
	dwCRC = GetFileCheckSum( (char*)szFileName);
	return dwCRC;
}

// 재부팅을 해야 하는 파일인지 검사
bool IsRebootingFile( const char* szFileName)
{
	bool bIsReboot = false;

// 	char *pFileName;
// 	_strlwr_s( pFileName = _strdup( szFileName), 256);
// 
// //	if ( strstr( pFileName, "launcher") != NULL)				bIsReboot = true;
// 
// 	delete pFileName;


	return bIsReboot;
}

// 생성자
ZUpdate::ZUpdate()
{
	// 변수 초기화
	m_bInitialize = false;
	m_szAddress[ 0] = 0;
	m_nPort = 0;
	m_szID[ 0] = 0;
	m_szPassword[ 0] = 0;
	m_pUpdatePatchList.clear();
	m_bStopUpdate = false;
	m_bPatchComplete = false;
	m_bReboot = false;

	//m_strClientVersion = "0";
	m_nClientVersion = 0;
	m_nLauncherVersion = 0;
}


// 소멸자
ZUpdate::~ZUpdate()
{
	if ( m_bInitialize)
		Destroy();
}


// Create
bool ZUpdate::Create( const char* pszAddress, unsigned long nPort, const char* pszDefDirectory, const char* pszID, const char* pszPassword)
{
	m_strErrorMsg = "";

	PutLog( "[ZUpdate] Create.");

	// Set variables
	strcpy( m_szAddress, pszAddress);
	m_nPort = nPort;
	strcpy( m_szID, pszID);
	strcpy( m_szPassword, pszPassword);

	// Create File Transfer
	if ( !m_FileTransfer.Create( pszAddress, nPort, pszID, pszPassword))
	{
		char szMsg[ 1024];
		sprintf( szMsg, "%s\n     [Tip] %s\n     [Tip] %s\n",	/*The update server is not responding or is not running right now.*/ ZGetString( "STR_128"),
																/*Please check your firewall.*/ ZGetString( "STR_129"),
																/*Please try again after a while.*/ ZGetString( "STR_130"));
		m_strErrorMsg += szMsg;

		PutLog( "[ZUpdate] ERROR : Cannot create file transfer.");
		PutLog( "[ZUpdate] ERROR : Create FAILED!!!");

		return false;
	}


	// Create Open Connection
	if ( !m_FileTransfer.OpenConnection())
	{
		char szMsg[ 1024];
		sprintf( szMsg, "%s\n     [Tip] %s\n     [Tip] %s\n",	/*Internet connection failed.*/ ZGetString( "STR_131"),
																/*Please check your firewall.*/ ZGetString( "STR_129"),
																/*Please check your internet connection.*/ ZGetString( "STR_132"));
		m_strErrorMsg += szMsg;

		PutLog( "[ZUpdate] ERROR : Cannot open file transfer.");
		PutLog( "[ZUpdate] ERROR : Create FAILED!!!");

		return false;
	}


	// Set default directory
	if ( !m_FileTransfer.ChangeDirectory( pszDefDirectory))
	{
		char szMsg[ 1024];
		sprintf( szMsg, "%s\n     [Tip] %s\n     [Tip] %s\n",	/*The update server is not responding or is not running right now.*/ ZGetString( "STR_128"),
																/*Please check your firewall.*/ ZGetString( "STR_129"),
																/*Please try again after a while.*/ ZGetString( "STR_130"));
		m_strErrorMsg += szMsg;

		PutLog( "[ZUpdate] ERROR : Cannot change default directory.");
		PutLog( "[ZUpdate] ERROR : Create FAILED!!!");

		return false;
	}

	// Success
	m_UpdateInfo.SetTransferUI( m_FileTransfer.GetFileTransferUI());
	m_bInitialize = true;
	PutLog( "[ZUpdate] Create successfully compete.");

	return true;
}


// Destroy
bool ZUpdate::Destroy()
{
	PutLog( "[ZUpdate] Destroy.");

	// Close Connection
	if ( !m_FileTransfer.Destroy()) { 
		PutLog( "[ZUpdate] WARNING : Destroy FAILED!!!");
	}

	// Clear varialbes
	m_szAddress[ 0] = 0;
	m_nPort = 0;
	m_szID[ 0] = 0;
	m_szPassword[ 0] = 0;
	m_bInitialize = false;

	for ( ZUpdatePatchList::iterator itr = m_pUpdatePatchList.begin();  itr != m_pUpdatePatchList.end();  itr++)
		delete (*itr);

	m_pUpdatePatchList.clear();
    
	// Success
	PutLog( "[ZUpdate] Destroy successfully complete.");

	return true;
}


// CheckUpdate
bool ZUpdate::UpdatePatchInfoFile( const char* pszPatchFileName, const char* pszLoaclFileName )
{
	if( false == PrepareUpdatePatchInfoFile() ) { 
		return false;
	}
		
	// Download patch info file
	if( false == DownloadPatchInfoFile( pszPatchFileName, pszLoaclFileName ) ) {
		return false;
	}
	
	// Get update info
	if ( !GetUpdateInfo(pszPatchFileName))
	{
		PutLog( "[ZUpdate] ERROR : GetUpdateInfo FAILED!!!");
		return false;
	}

	// Update files
	if ( !CheckValidFromPatchList())
	{
		PutLog( "[ZUpdate] ERROR : CheckValidFromPatchList FAILED!!!");
		return false;
	}

	CString strLog;
	strLog.Format("UpdatePatchInfoFile() | Latest Client Ver(%d)", GetNewClientVersion());
	PutLog(strLog);

	return true;
}

bool ZUpdate::DownloadPatchInfoFile( const char* pszPatchFileName, const char* pszLoaclFileName )
{
	if ( !m_FileTransfer.DownloadFile( pszPatchFileName, pszLoaclFileName, 0))
	{
		char szMsg[ 1024];
		sprintf( szMsg, "%s\n     [Tip] %s\n     [Tip] %s\n",	/*The update server is not responding or is not running right now.*/ ZGetString( "STR_128"),
			/*Please check your firewall.*/ ZGetString( "STR_129"),
			/*Please try again after a while.*/ ZGetString( "STR_130"));
		m_strErrorMsg += szMsg;

		PutLog( "[ZUpdate] ERROR : Start update FAILED!!!");
		return false;
	}

	return true;
}

// StartUpdate
bool ZUpdate::StartUpdate()
{
	m_strErrorMsg = "";

	// Patch files
	if ( false == PatchClientFiles() )
	{
		PutLog( "[ZUpdate] ERROR : Start update FAILED!!!");
		return false;
	}

	// 임시 폴더에 저장된 파일을 원래 디렉토리로 옮긴다
	if ( false == MovePatchFiles())
	{
		PutLog( "[ZUpdate] ERROR : Cannot move patch files.");
		return false;
	}

	// 패치 파일 압축을 풀어야한다.
	DeCompressPatchFiles();

	// 패치 파일을 삭제한다.
	DeletePatchFiles();

	// Update complete
	PutLog( "[ZUpdate] Start update successfully complete.");

	m_bPatchComplete = true;

	return true;
}


// StopUpdate
bool ZUpdate::StopUpdate()
{
	PutLog( "[ZUpdate] Stop update.");

	// Stop file transfer
	m_FileTransfer.StopDownload();
	m_bStopUpdate = true;

	// Update complete
	PutLog( "[ZUpdate] Stop update successfully complete.");

	// Log
	char szMsg[ 1024];
	sprintf( szMsg, "%s\n     [Tip] %s\n",	/*Update canceled.*/ ZGetString( "STR_133"),
											/*Please retry updating.*/ ZGetString( "STR_134"));
	m_strErrorMsg += szMsg;
	return true;
}



// GetUpdatePatchFileInfo
#define MPTOK_PATCHINFO		"PATCHINFO"
#define MPTOK_VERSION		"VERSION"
#define MPTOK_PATCHNODE		"PATCHNODE"
#define MPTOK_SIZE			"SIZE"
#define MPTOK_WRITETIMEHIGH	"WRITETIMEHIGH"
#define MPTOK_WRITETIMELOW	"WRITETIMELOW"
#define MPTOK_CHECKSUM		"CHECKSUM"
#define MPTOK_ATTR_FILE		"file"

bool ZUpdate::GetUpdateInfo( const char* pszPatchFileName)
{
	PutLog( "[ZUpdate] Get update info.");
	
	ParsePathcInfoFile( pszPatchFileName );
	
	// Success
	PutLog( "[ZUpdate] Get update info successfully complete.");
	
	return true;
}


// CheckValidFromPatchList
bool ZUpdate::CheckValidFromPatchList()
{
	PutLog( "[ZUpdate] Check valid from patch list.");

	size_t nVersionCount = m_PatchFiles.size();
	for( size_t nVer = 0; nVer < nVersionCount; ++nVer )
	{
		if( m_PatchFiles[nVer].nVersion <= m_nClientVersion ) { 
			continue;
		}

		size_t nFileCount = m_PatchFiles[nVer].FileNames.size();
		for( size_t nFile = 0; nFile < nFileCount; ++nFile )
		{
			m_UpdateInfo.AddTotalPatchFileSize( m_PatchFiles[nVer].FileSizes[nFile] );
			m_UpdateInfo.AddTotalPatchFileCount( 1 );

			m_pUpdatePatchList.push_back( new ZUpdatePatchNode( m_PatchFiles[nVer].FileNames[nFile].c_str(), m_PatchFiles[nVer].FileSizes[nFile], m_PatchFiles[nVer].FileCRCs[nFile])  );
		}
	}

	// Success
	PutLog( "[ZUpdate] Check valid from patch list successfully complete.");
	CString str;
	str.Format( "[ZUpdate] + Total patch file count : %u", m_UpdateInfo.GetTotalPatchFileCount());
	PutLog( str);
	str.Format( "[ZUpdate] + Total patch file size : %u bytes", m_UpdateInfo.GetTotalPatchFileSize());
	PutLog( str);

	return true;
}


// PatchFiles
bool ZUpdate::PatchClientFiles()
{
	PutLog( "[ZUpdate] Patch files.");
	
	// Get patch files information
	CString strErr;
	for ( ZUpdatePatchList::iterator itr = m_pUpdatePatchList.begin();  itr != m_pUpdatePatchList.end();  itr++)
	{
		if ( (*itr)->IsValid())
		{
			//// Get file name
			TCHAR szFullPath[ _MAX_DIR];
			GetTempFilePathName( (*itr)->GetFileName(), szFullPath);

			//// Check if exist file
			bool bExistFile = IsHadPatchFiles( szFullPath, (*itr)->GetChecksum(), (*itr)->GetSize() );
			
			// Start download
			if ( !bExistFile)
			{
				bool bRet = DownLoadPatchFile( (*itr)->GetFileName(), szFullPath, (*itr)->GetSize() );
				if( false == bRet ) { 
					(*itr)->SetPatchSuccess(false);
				}
			}

			// Update information
			m_UpdateInfo.AddCurrPatchedFileSize( (*itr)->GetSize() );
			m_UpdateInfo.AddCurrPatchedFileCount( 1);
		}
		else
		{
			strErr.Format("ZUpdate::PatchFiles() | Failed Valid. %s", (*itr)->GetFileName() );
			PutLog( strErr );
		}

		// Check stop
		if ( m_bStopUpdate)
		{
			PutLog( "[ZUpdate] Patch files STOP!!!");
			return false;
		}
	}

    // Success
	PutLog( "[ZUpdate] Patch files successfully complete.");
	return true;
}

bool ZUpdate::IsHadPatchFiles( TCHAR* pszFullPath, INT64 nCheckSum, INT64 nFileSize )
{
	bool bExistFile = false;
	char szMsg[ 1024];
	WIN32_FIND_DATA findfile;
	HANDLE hFind = FindFirstFile( _T( pszFullPath), &findfile);

	if ( hFind != INVALID_HANDLE_VALUE)
	{
		// CRC 검사를 해서 올바른 CRC이면 패스함
		//if ( nCheckSum == GetCRC( pszFullPath, &m_strErrorMsg))
		if( nFileSize == GetFileSize( pszFullPath) )
		{
			sprintf( szMsg, "[ZUpdate] Already exist patch file : %s", GetFilteredFileName(pszFullPath));
			PutLog( szMsg);

			bExistFile = true;
		}

		FindClose( hFind);
	}

	return bExistFile;
}

bool ZUpdate::DownLoadPatchFile( const char* pszFileName, const char* pszFullPath, INT64 nFileSize )
{	
	bool bRet = false;
	char szMsg[ 1024] = {0,};
#define DOWNLOAD_RETRY_COUNT	2
	for ( int nRetry = 1;  nRetry <= DOWNLOAD_RETRY_COUNT;  nRetry++)
	{
		if ( m_bStopUpdate) {
			break;
		}

		if( m_FileTransfer.DownloadFile(pszFileName, pszFullPath, nFileSize) )
		{
			bRet = true;
			sprintf( szMsg, "[ZUpdate] Successfully download '%s'", GetFilteredFileName(pszFullPath));
			PutLog( szMsg);
			break;
		}
		// Fail
		else if ( nRetry == DOWNLOAD_RETRY_COUNT)
		{
			m_UpdateInfo.AddPatchFailedCount( 1);
			
			sprintf( szMsg, "[ZUpdate] ERROR : Fail to download file '%s'", GetFilteredFileName(pszFullPath));
			PutLog( szMsg);


			sprintf( szMsg, "%s : %s\n     [Tip] %s\n",	/*Internet connection closed or this file is read-only*/ ZGetString( "STR_135"), GetFilteredFileName(pszFullPath),
				/*Please check your internet connection or delete this file*/ ZGetString( "STR_136"));
			m_strErrorMsg += szMsg;
		}
		// Retry
		else
		{
			sprintf( szMsg, "[ZUpdate] WARNNING : Retry patch file : %s , %d retry", GetFilteredFileName(pszFullPath), nRetry);
			PutLog( szMsg);
		}
	}
	
	// Update information
	m_UpdateInfo.AddCurrPatchedFileSize(nFileSize);
	m_UpdateInfo.AddCurrPatchedFileCount(1);

	return true;
}

// 패치 파일을 원래 폴더로 옮김
bool ZUpdate::MovePatchFiles()
{
	PutLog( "[ZUpdate] Move patch files.");

	// Get patch files information
	char szMsg[ 1024];
	bool bFail = false;
	for ( ZUpdatePatchList::iterator itr = m_pUpdatePatchList.begin();  itr != m_pUpdatePatchList.end();  itr++)
	{
		if ( (*itr)->IsValid() && (*itr)->IsPatchSuccess())
		{
			// 원본 파일명을 구한다
			TCHAR szFullPath[512] = {0,};	GetFullPathFileName( (*itr)->GetFileName(), szFullPath );

			// 다운로드 받은 패치 파일의 파일명을 구한다
			TCHAR szTmpFullPath[ _MAX_DIR];		GetTempFilePathName( (*itr)->GetFileName(), szTmpFullPath);

			if( false == CheckDownloadedPatchFile( szTmpFullPath, (*itr)->GetChecksum(), (*itr)->GetSize() ) ) 
			{
				bFail = true;
				continue;
			}

			// 재부팅이 필요한 파일인지 검사한다
			if( IsRebootingFile(szTmpFullPath) )
			{
				m_bReboot = true;
				continue;
			}

			// 로그 출력
			sprintf( szMsg, "[ZUpdate] Copy patch file from '%s'  to  '%s'", GetFilteredFileName(szTmpFullPath), GetFilteredFileName(szFullPath));
			PutLog( szMsg);

			// Copy file
			if( false == CopyDownloadedPatchFile( szTmpFullPath, szFullPath ) ) {
				bFail = true;
				continue;
			}
			
			// 패치된 파일이 정상인지 다시 확인한다
			if( false == IsSamePatchFileInfo( szFullPath, (*itr)->GetChecksum(), (*itr)->GetSize() ) )
			{
				sprintf( szMsg, "[ZUpdate] ERROR : MovePatchFiles() - 2 '%s'", GetFilteredFileName( szFullPath));
				PutLog( szMsg);
				sprintf( szMsg, "%s : %s\n     [Tip] %s\n",	/*This file is currently cannot be modified*/ ZGetString( "STR_142"), GetFilteredFileName( szFullPath),
															/*Please check for file authorization*/ ZGetString( "STR_125"));
				m_strErrorMsg += szMsg;
				// 패치 실패
				bFail = true;
			}
		}
	}

	DeleteTempDirectory();

    // Success
	PutLog( "[ZUpdate] Move patch files successfully complete.");

	if( bFail ) {
		return false;
	}

	return true;
}

bool ZUpdate::CheckDownloadedPatchFile( const char* pszTmpFileFullPath, INT64 nCheckSum, INT64 nFileSize )
{
	char szMsg[ 1024];

	if( false == IsSamePatchFileInfo( pszTmpFileFullPath, nCheckSum, nFileSize ) )
	{
		sprintf( szMsg, "[ZUpdate] ERROR : MovePatchFiles() '%s'", GetFilteredFileName(pszTmpFileFullPath));
		PutLog( szMsg);


		sprintf( szMsg, "%s : %s\n     [Tip] %s\n",	/*Invalid CRC from the downloaded patch file.*/ ZGetString( "STR_137"), GetFilteredFileName(pszTmpFileFullPath),
			/*Please check for anti-virus program or firewall running on your system.*/ ZGetString( "STR_138"));
		m_strErrorMsg += szMsg;

		return false;
	}

	return true;
}

bool ZUpdate::CopyDownloadedPatchFile( const char* pszTmpFileFullPath, const char* pszFullPath )
{
	char szMsg[ 1024];

	if ( !MakePath(pszFullPath))
	{
		sprintf( szMsg, "[ZUpdate] WARNNING : This file cannot made path : %s", GetFilteredFileName(pszFullPath));
		PutLog( szMsg);

		sprintf( szMsg, "%s : %s\n     [Tip] %s\n",	/*WARNNING : This file cannot made path.*/ ZGetString( "STR_139"), GetFilteredFileName(pszFullPath),
			/*Please check for directory authorization*/ ZGetString( "STR_140"));
		m_strErrorMsg += szMsg;
	}

	BOOL bRetVal = CopyFile( pszTmpFileFullPath, pszFullPath, FALSE);

	// 복사 실패면...
	if ( bRetVal == FALSE)
	{
		LPVOID lpMsgBuf;
		DWORD error = GetLastError();
		FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, error, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL);

		// 에러 메시지 작성
		sprintf( szMsg, "[ZUpdate] ERROR : File copy error : %s : %s", GetFilteredFileName(pszFullPath), lpMsgBuf);
		PutLog( szMsg);

		sprintf( szMsg, "%s : %s : %s\n     [Tip] %s\n",	/*File copy error*/ ZGetString( "STR_141"), GetFilteredFileName(pszFullPath), lpMsgBuf,
			/*Please check for file authorization*/ ZGetString( "STR_125"));
		m_strErrorMsg += szMsg;


		LocalFree( lpMsgBuf);

		return false;
	}

	return true;
}

bool ZUpdate::DeCompressPatchFiles()
{
	PutLog( "[ZUpdate] DeCompress Patch Files.");
	char szFullFilePath[512] = {0, };
	
	char szDeCompressDirectory[512] = {0,};
	GetDeCompressDirectory( szDeCompressDirectory );

	// Get patch files information
	char szMsg[ 1024];
	bool bFail = false;
	for ( ZUpdatePatchList::iterator itr = m_pUpdatePatchList.begin();  itr != m_pUpdatePatchList.end();  itr++)
	{
		GetFullPathFileName( (*itr)->GetFileName(), szFullFilePath  );

		WIN32_FIND_DATA findfile;
		HANDLE hFind = FindFirstFile( _T(szFullFilePath), &findfile);

		if ( hFind != INVALID_HANDLE_VALUE)
		{
			// http://www.codeproject.com/KB/files/zip_utils.aspx
			HZIP hz; 
			hz = OpenZip(_T(szFullFilePath),0); 
			SetUnzipBaseDir(hz,_T(szDeCompressDirectory));
			ZIPENTRY ze; 
			GetZipItem(hz,-1,&ze); 
			int numitems=ze.index;
			for (int zi=0; zi<numitems; zi++)
			{ 
				GetZipItem(hz,zi,&ze); 
				UnzipItem(hz,zi,ze.name);
			}
			CloseZip(hz);

			FindClose(hFind);
		}
		else
		{
			sprintf( szMsg, "[ZUpdate] DeCompressPatchFiles(). Not exist patch file : %s", szFullFilePath );
			PutLog( szMsg);
			return false;
		}
	}

	return true;
}

void ZUpdate::DeletePatchFiles()
{
	PutLog( "[ZUpdate] Delete Patch Files.");
	
	// Get patch files information
	/*for ( ZUpdatePatchList::iterator itr = m_pUpdatePatchList.begin();  itr != m_pUpdatePatchList.end();  itr++)
	{
		DeleteCurDirFile( "ZUpdate::DeletePatchFiles()", (*itr)->GetFileName() );
	}*/

	size_t nPatchCount = m_PatchFiles.size();
	for( size_t i = 0; i < nPatchCount; ++i )
	{
		size_t nCount = m_PatchFiles[i].FileNames.size();
		for( size_t n = 0; n < nCount; ++n )
		{
			DeleteCurDirFile( "ZUpdate::DeletePatchFiles()", m_PatchFiles[i].FileNames[n].c_str() );
		}
	}
}

bool ZUpdate::DeleteCurDirFile( const char* pszCaller, const char* pszDeleteFileName )
{
	char szFullPathFileName[512] = {0,};
	GetFullPathFileName(pszDeleteFileName, szFullPathFileName);

	WIN32_FIND_DATA findfile;
	HANDLE hFind = FindFirstFile( _T(szFullPathFileName), &findfile);

	if ( hFind != INVALID_HANDLE_VALUE)
	{
		DeleteFile(szFullPathFileName);
		FindClose(hFind);
	}
	else
	{
		//char szMsg[ 1024];
		//sprintf( szMsg, "[ZUpdate] %s. Not exist patch file : %s", pszCaller, szFullPathFileName );
		//PutLog(szMsg);
		return false;
	}

	return true;
}

void ZUpdate::DeleteTempDirectory()
{
	//char szMsg[ 1024];

	char szTmpDirectory[512] = {0, };
	GetTempDirectory( szTmpDirectory );	
	
	SHFILEOPSTRUCT FileOp = {0};//쉘 구조체

	FileOp.hwnd = NULL;
	FileOp.wFunc = FO_DELETE;//지우고
	FileOp.pFrom = szTmpDirectory;//인자로 받은 폴더

	FileOp.pTo = NULL;//복사할때 사용
	FileOp.fFlags = FOF_NO_UI;//화면에 메세지 표시 않함

	//뭔지 알고 싶으시면 지우고 해보세요
	FileOp.fAnyOperationsAborted = false;//나머진 뭔지 모르겠네요
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = NULL;

	SHFileOperation(&FileOp);//구조체를 실행
}

void ZUpdate::DeleteTempPatchFile( const char* pszFileName )
{
	char szMsg[ 1024];

	char szFullFilePath[512] = {0, };
	GetTempFilePathName( pszFileName, szFullFilePath);

	WIN32_FIND_DATA findfile;
	HANDLE hFind = FindFirstFile( _T(szFullFilePath), &findfile);

	if ( hFind != INVALID_HANDLE_VALUE)
	{
		DeleteFile(szFullFilePath);
		FindClose(hFind);

		sprintf( szMsg, "[ZUpdate] Delete Patch Files. %s", szFullFilePath );
	}
	else
	{
		sprintf( szMsg, "[ZUpdate] DeleteTempPatchFile(). Not exist patch file : %s", szFullFilePath );
	}

	PutLog( szMsg);
}

bool ZUpdate::GetCurrentLauncherAndClientExeVersion( const char* pszFileName, int& nLauncherVersion, int& nClientVersion )
{
	char szFullFilePath[MAX_PATH] = {0, };
	GetFullPathFileName( pszFileName, szFullFilePath );

	CString strErrMsg;
	bool bRet = false;
	CFile file;
	CFileException ex;
	if ( file.Open( szFullFilePath, CFile::modeRead, &ex) )
	{
		int nFileSize = (int)file.GetLength();
		char* szBuff = new char[ nFileSize ];
		ZeroMemory( szBuff, nFileSize );
		file.Read( szBuff, nFileSize );
			
		Json::Value root;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse( szBuff, root );
		if( parsingSuccessful )
		{
			nLauncherVersion = root["Version"].get("Launcher", 0).asInt();
			nClientVersion = root["Version"].get("ClientExe", 0).asInt();
			bRet = true;
		}
		else
		{
			strErrMsg.Format( "ZUpdate::GetCurrentLauncherAndClientExeVersion() | Failed parse json File. %s", szFullFilePath);
			PutLog( strErrMsg );
		}

		// 메모리, 파일 해제
		delete [] szBuff;
		file.Close();
	}
	else
	{
		strErrMsg.Format( "ZUpdate::GetCurrentLauncherAndClientExeVersion() | Failed Open File. %s", szFullFilePath);
		PutLog( strErrMsg );
	}

	if( bRet )
	{
		m_nClientVersion = nClientVersion;
		m_nLauncherVersion = nLauncherVersion;
	}

	return bRet;
}

void ZUpdate::SetCurrentLauncherAndClientExeVersion( const char* pszFileName, const int nLauncherVersion )
{
	const int nClientVersion = GetNewClientVersion();

	Json::Value root;
	Json::Value version;
	version["Launcher"] = nLauncherVersion;
	version["ClientExe"] = nClientVersion;
	root["Version"] = version;
	Json::StyledWriter writer;
	std::string outputConfig = writer.write( root );


	char szFullFilePath[MAX_PATH] = {0, };
	GetFullPathFileName( pszFileName, szFullFilePath );

	char szError[1024] = {0,};
	CString strErrMsg;
	bool bRet = false;
	CFile LuncherInfoFile;
	CFileException ex;
	if( LuncherInfoFile.Open(szFullFilePath, CFile::modeCreate | CFile::modeWrite, &ex) )
	{
		try
		{
			LuncherInfoFile.Write( outputConfig.c_str(), outputConfig.length() );

			CString strMsg;
			strMsg.Format("Success Update %s", szFullFilePath);
			PutLog(strMsg);
		}
		catch ( CInternetException* pEx)
		{
			pEx->GetErrorMessage(szError, 1024);
			strErrMsg.Format( "ZUpdate::SetCurrentLauncherAndClientExeVersion() | ERROR : %s", szError);
			PutLog(strErrMsg);

			LuncherInfoFile.Close();
		}
	}
	else
	{
		ex.GetErrorMessage(szError, 1024);
		strErrMsg.Format( "[ZFileTransfer] ERROR : %s. filepath:%s", szError, szFullFilePath);
		PutLog(strErrMsg);
	}
}

bool ZUpdate::ParsePathcInfoFile( const char* pszFileName )
{
	for ( ZUpdatePatchList::iterator itr = m_pUpdatePatchList.begin();  itr != m_pUpdatePatchList.end();  itr++) {
		delete (*itr);
	}
	m_pUpdatePatchList.clear();
	m_PatchFiles.clear();

	char szFullFilePath[MAX_PATH] = {0, };
	GetFullPathFileName( pszFileName, szFullFilePath );

	CString strErrMsg;
	bool bRet = false;
	CFile file;
	CFileException ex;
	if ( file.Open( szFullFilePath, CFile::modeRead, &ex) )
	{
		int nFileSize = (int)file.GetLength();
		int nBuffSize = nFileSize * 2;
		char* szBuff = new char[ nBuffSize ];
		ZeroMemory( szBuff, nBuffSize );
		file.Read( szBuff, nFileSize );
			
//		RemoveWasteStringInPatchInfoFile( nBuffSize, szBuff );

		Json::Value root;
		Json::Reader reader; 
		bool parsingSuccessful = reader.parse( szBuff, root );
		if( parsingSuccessful )
		{
			const Json::Value VersionList = root["VERSION_LIST"];
			for( size_t i = 0; i < VersionList.size(); ++i )
			{
				int nVersion = VersionList[i]["Version"].asInt();
				DOWNLOADPATCHVERSIONINFO* pInfo = AddVersion( nVersion );
				if( NULL != pInfo )
				{
					const Json::Value FileList = VersionList[i]["Files"];
					for( size_t n = 0; n < FileList.size(); ++n )
					{
						std::string strFileName = FileList[n]["filename"].asString();
						INT64 nSize = (INT64)FileList[n]["size"].asDouble();
						INT64 nCRC = (INT64)FileList[n]["CRC"].asDouble();

						pInfo->FileNames.push_back( strFileName.c_str() );
						pInfo->FileSizes.push_back( nSize );
						pInfo->FileCRCs.push_back( nCRC );
					}
				}
				else
				{
					strErrMsg.Format( "ZUpdate::ParsePathcInfoFile() | Failed Add PatchInfo. Version : %d", nVersion);
					PutLog( strErrMsg );
				}
			}

			bRet = true;
		}
		else
		{
			strErrMsg.Format( "ZUpdate::ParsePathcInfoFile() | Failed parse json File. %s", szFullFilePath);
			PutLog( strErrMsg );
		}

		// 메모리, 파일 해제
		delete [] szBuff;
		file.Close();
	}
	else
	{
		strErrMsg.Format( "ZUpdate::ParsePathcInfoFile() | Failed Open File. %s", szFullFilePath);
		PutLog( strErrMsg );
	}

	return bRet;
}

DOWNLOADPATCHVERSIONINFO* ZUpdate::AddVersion( const int nVersion )
{
	DOWNLOADPATCHVERSIONINFO Info;
	Info.nVersion = nVersion;
	m_PatchFiles.push_back( Info );

	size_t nCount = m_PatchFiles.size();
	return &m_PatchFiles[ nCount-1 ];
}

void ZUpdate::GetTempDirectory( char* szTmpDirectory )
{
	char szDir[ _MAX_DIR];
	GetCurrentDirectory( sizeof( szDir), szDir);

	char drive[ _MAX_PATH], dir[ _MAX_PATH], fname[ _MAX_PATH], ext[ _MAX_PATH];
	_splitpath( szTmpDirectory, drive, dir, fname, ext);

#ifdef _DEBUG
	sprintf( szTmpDirectory, "%s\\Runtime\\%s", szDir, STR_TEMP_DOWNLOADFOLDER); /// 디버깅 시
#else
	sprintf( szTmpDirectory, "%s\\%s", szDir, STR_TEMP_DOWNLOADFOLDER);
#endif
}

void ZUpdate::GetTempFilePathName( const char* szFileName, char* szTmpFileName)
{
	char szDir[ _MAX_DIR];
	GetCurrentDirectory( sizeof( szDir), szDir);

	char drive[ _MAX_PATH], dir[ _MAX_PATH], fname[ _MAX_PATH], ext[ _MAX_PATH];
	_splitpath( szFileName, drive, dir, fname, ext);

#ifdef _DEBUG
	sprintf( szTmpFileName, "%s\\Runtime\\%s\\%s%s_", szDir, STR_TEMP_DOWNLOADFOLDER, fname, ext); /// 디버깅 시
#else
	sprintf( szTmpFileName, "%s\\%s\\%s%s_", szDir, STR_TEMP_DOWNLOADFOLDER, fname, ext);
#endif
}

void ZUpdate::GetFullPathFileName( const char* pszFileName, char* FullPathFileName )
{
	char szCurDir[MAX_PATH] = {0, };
	GetCurrentDirectory(MAX_PATH, szCurDir);
	
#ifdef _DEBUG
	sprintf_s( FullPathFileName, MAX_PATH, "%s\\Runtime\\%s", szCurDir, pszFileName ); //디버깅 시
#else
	sprintf_s( FullPathFileName, MAX_PATH, "%s\\%s", szCurDir, pszFileName );
#endif
}

void ZUpdate::GetDeCompressDirectory( char* pszFullPath )
{
	char szCurDir[MAX_PATH] = {0, };
	GetCurrentDirectory(MAX_PATH, szCurDir);
	
#ifdef _DEBUG
	sprintf_s( pszFullPath, MAX_PATH, "%s\\Runtime", szCurDir ); //디버깅 시
#else
	sprintf_s( pszFullPath, MAX_PATH, "%s", szCurDir );
#endif
}

// void ZUpdate::RemoveWasteStringInPatchInfoFile( const int nBufferSize, char* pBuffer )
// {
// 	if( nBufferSize < 1 || NULL == pBuffer ) { 
// 		return;
// 	}
// 
// 	// UTF_8 -> Ansi
// 	BSTR    bstrWide;  
// 	char*   pszAnsi;  
// 	int     nLength;      
// 	// Get nLength of the Wide Char buffer  
// 	nLength = MultiByteToWideChar(CP_UTF8, 0, pBuffer, lstrlen(pBuffer) + 1,NULL, NULL);  
// 	bstrWide = SysAllocStringLen(NULL, nLength);  
// 
// 	// Change UTF-8 to Unicode (UTF-16)  
// 	MultiByteToWideChar(CP_UTF8, 0, pBuffer, lstrlen(pBuffer) + 1, bstrWide,nLength);  
// 
// 
// 	// Get nLength of the multi byte buffer   
// 	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);  
// 	pszAnsi = new char[nLength];  
// 
// 
// 	// Change from unicode to mult byte  
// 	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);  
// 
// 	SysFreeString(bstrWide);
// 
// 	wsprintfA( pBuffer, "%S", bstrWide );
// 
// 	/// 앞의 불필요한 문자 제거
// 	int nNewBuffSize = nBufferSize;
// 	char* pNewBuffer = new char[nNewBuffSize];
// 
// 	ZeroMemory( pNewBuffer, nNewBuffSize );
// 	CopyMemory( pNewBuffer, &pBuffer[1], nBufferSize-1 );
// 
// 	ZeroMemory( pBuffer, nBufferSize );
// 	CopyMemory( pBuffer, pNewBuffer, nBufferSize );
// 
// 	delete[] pNewBuffer;
// }

INT64 ZUpdate::GetFileSize( const char* pszFileName )
{
	HANDLE hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	if ((hFile == NULL) || (hFile == INVALID_HANDLE_VALUE) ) {
		return 0;
	}

	BY_HANDLE_FILE_INFORMATION fileInfo;
	GetFileInformationByHandle(hFile, &fileInfo);
	CloseHandle(hFile);

	return fileInfo.nFileSizeLow;
}

bool ZUpdate::IsSamePatchFileInfo( const char* pszFullPath, const INT64 nCheckSumFromInfofile, const INT64 nFileSizeFromInfofile )
{
	INT64 nFileSize = GetFileSize(pszFullPath);
	if ( nFileSizeFromInfofile != nFileSize) {
		return false;
	}
	/*DWORD dwCRC = GetCRC( szTmpFullPath, &m_strErrorMsg);
	if ( nCheckSumFromInfofile != dwCRC) { 
		return false;
	}
	*/
	return true;
}

int ZUpdate::GetNewClientVersion()
{
	int nMaxVersion = 0;
	size_t nCount = m_PatchFiles.size();
	for( size_t i = 0; i < nCount; ++i )
	{
		if( nMaxVersion < m_PatchFiles[i].nVersion ) { 
			nMaxVersion = m_PatchFiles[i].nVersion;
		}
	}

	return nMaxVersion;
}

bool ZUpdate::PrepareUpdatePatchInfoFile()
{
	m_strErrorMsg = "";

	// Check Initialized
	if ( !m_bInitialize)
	{
		PutLog( "[ZUpdate] ERROR : Do not Created.");
		PutLog( "[ZUpdate] ERROR : Start update FAILED!!!");

		return false;
	}
	PutLog( "[ZUpdate] Start update.");


	// Set variables
	m_bStopUpdate = false;
	m_bPatchComplete = false;
	m_bReboot = false;
	m_UpdateInfo.ClearTotalPatchFileSize();
	m_UpdateInfo.ClearCurrPatchedFileSize();
	m_UpdateInfo.ClearTotalPatchFileCount();
	m_UpdateInfo.ClearCurrPatchedFileCount();
	m_UpdateInfo.ClearPatchFailedCount();

	return true;
}



void ZUpdate::Test()
{
	// http://www.codeproject.com/KB/files/zip_utils.aspx
	//HZIP hz; DWORD writ;

	//// EXAMPLE 2 - unzip it with the names suggested in the zip
	//hz = OpenZip(_T("\\simple1.zip"),0);
	//SetUnzipBaseDir(hz,_T("\\"));
	//ZIPENTRY ze; GetZipItem(hz,-1,&ze); int numitems=ze.index;
	//for (int zi=0; zi<numitems; zi++)
	//{ 
	//	GetZipItem(hz,zi,&ze);
	//	UnzipItem(hz,zi,ze.name);
	//}
	//CloseZip(hz);
}