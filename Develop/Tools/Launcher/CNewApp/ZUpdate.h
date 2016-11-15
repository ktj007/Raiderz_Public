/******************************************************************
   
   ZUpdate.h

     Corperation : MAIET entertainment
     Programmer  : Lim Dong Hwan
	 Date        : 22.June.2005

*******************************************************************/


#ifndef _ZUPDATE
#define _ZUPDATE


#include "ZFileTransfer.h"
#include "ZUpdateUIInfo.h"
#include "ZUpdatePatchNode.h"
#include <windows.h>
#include <list>
#include <string>
#include <vector>
using namespace std;


#define STR_TEMP_DOWNLOADFOLDER		"PATCH"

struct DOWNLOADPATCHVERSIONINFO
{
	int nVersion;
	std::vector< std::string > FileNames;
	std::vector< INT64 > FileSizes;
	std::vector< INT64 > FileCRCs;
};

class ZUpdatePatchList : public list<ZUpdatePatchNode*>{};


// class ZUpdate
class ZUpdate
{
protected:
	ZUpdatePatchList	m_pUpdatePatchList;

	ZFileTransfer		m_FileTransfer;

	ZUpdateUIInfo		m_UpdateInfo;

	bool				m_bInitialize;
	char				m_szAddress[ 256];
	unsigned long		m_nPort;
	char				m_szID[ 256];
	char				m_szPassword[ 256];
	bool				m_bStopUpdate;
	bool				m_bPatchComplete;
	CString				m_strErrorMsg;
	bool				m_bReboot;

	// 게임 프로세스 버젼
	int m_nClientVersion;
	// 런쳐 버젼
	int m_nLauncherVersion;


public:
	ZUpdate();
	virtual ~ZUpdate();

	bool Create( const char* pszAddress, unsigned long nPort, const char* pszDefDirectory, const char* pszID, const char* pszPassword);
	bool Destroy();

	bool UpdatePatchInfoFile( const char* pszPatchFileName, const char* pszLoaclFileName );
	bool StartUpdate();
	bool StopUpdate();

	ZUpdateUIInfo GetUpdateInfo()			{ return m_UpdateInfo;}
	bool IsPatchComplete()					{ return m_bPatchComplete; }

	CString* GetErrorMsg()					{ return &m_strErrorMsg; }
	char* GetFileTransferError()			{ return m_FileTransfer.GetLastError(); }

	bool IsCreated()						{ return m_bInitialize; }

	bool IsReboot()							{ return m_bReboot; }

	bool GetCurrentLauncherAndClientExeVersion( const char* pszFileName, int& nLauncherVersion, int& nClientVersion );
	void SetCurrentLauncherAndClientExeVersion( const char* pszFileName, const int nLauncherVersion );
	
	void GetFullPathFileName( const char* pszFileName, char* FullPathFileName );
	void DeleteTempDirectory();
	bool DeleteCurDirFile( const char* pszCaller, const char* pszDeleteFileName );
	void DeletePatchFiles();

protected:
	bool PrepareUpdatePatchInfoFile();
	bool DownloadPatchInfoFile( const char* pszPatchFileName, const char* pszLoaclFileName );
	bool GetUpdateInfo( const char* pszPatchFileName);
	bool CheckValidFromPatchList();
	bool PatchClientFiles();
	bool MovePatchFiles();
	bool IsHadPatchFiles( TCHAR* pszFullPath, INT64 nCheckSum, INT64 nFileSize );
	bool DownLoadPatchFile( const char* pszFileName, const char* pszFullPath, INT64 nFileSize );
	bool IsSamePatchFileInfo( const char* pszFullPath, const INT64 nCheckSumFromInfofile, const INT64 nFileSizeFromInfofile );
	bool CheckDownloadedPatchFile( const char* pszTmpFileFullPath, INT64 nCheckSum, INT64 nFileSize );
	bool CopyDownloadedPatchFile( const char* pszTmpFileFullPath, const char* pszFullPath );

	//void RemoveWasteStringInPatchInfoFile( const int nBufferSize, char* pBuffer );
	bool ParsePathcInfoFile( const char* pszFileName );
	bool DeCompressPatchFiles();
	void DeleteTempPatchFile( const char* pszFileName );
	
	int GetNewClientVersion();
	
	INT64 GetFileSize( const char* pszFileName );
	void GetDeCompressDirectory( char* pszFullPath );
	void GetTempFilePathName( const char* szFileName, char* szTmpFileName);
	void GetTempDirectory( char* szTmpDirectory );


	DOWNLOADPATCHVERSIONINFO* AddVersion( const int nVersion );
	std::vector< DOWNLOADPATCHVERSIONINFO > m_PatchFiles;


public:
	void Test();
};

#endif
