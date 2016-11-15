#include "stdafx.h"
#include "RSystemInfo.h"
#include "REngine.h"
#include "RShaderCompositeManager.h"
#include "FileInfo.h"
#include <io.h>


namespace rs3 {


RSystemInfo::RSystemInfo()
{
}

bool RSystemInfo::GetFileList( const char* path, const char* filter, vector<string>& outvecFiles )
{
	struct _finddata_t file_t;
	long hFile;

	string pathFilter = path;
	pathFilter += filter;

	if( (hFile = _findfirst( pathFilter.c_str(), &file_t )) != -1L ) 
	{
		do
		{
			if(strcmp(file_t.name, "." )==0) continue;
			if(strcmp(file_t.name, "..")==0) continue;
			if(file_t.attrib & _A_SUBDIR )	 continue;

			string str = path;
			str += file_t.name;

			outvecFiles.push_back(str);
		} 
		while( _findnext( hFile, &file_t ) == 0 );

		_findclose(hFile);
	}

	return true;
}

void RSystemInfo::CheckAndDeleteCache()
{
	if (REngine::GetConfig().m_bUsingCompiledShaderCaching == false) return;

	const string& path = REngine::GetConfig().m_strCompiledShaderCachePath;

	string filter = "*";
	filter += COMPILED_SHADER_FILENAME_EXT;

	vector<string> cacheFiles;
	GetFileList(path.c_str(), filter.c_str(), cacheFiles);

	DWORD dwCacheShaderCRC = 0;		// 캐쉬가 저장될때의 CRC
	string strCheckSumFile = path + "checksum.bin";
	MFile file;
	file.SetAllowAccessUncompFile(true);
	if( file.Open( strCheckSumFile.c_str() ) )
	{
		file.Read( &dwCacheShaderCRC, sizeof(DWORD) );
		file.Close();
	}

	// 현재 실행파일의 쉐이더 CRC
	DWORD dwCurrentShaderCRC = REngine::GetSceneManager().GetShaderCompositeManager()->GetCheckSumComponentCRCs();

	if( dwCurrentShaderCRC != dwCacheShaderCRC )
	{
		mlog("캐쉬된 쉐이더를 사용하지 못합니다. \n");
		for (size_t i = 0; i < cacheFiles.size(); i++)
		{
			const string& str = cacheFiles[i];
			DeleteFile(str.c_str());
		}

		::MakePath( strCheckSumFile.c_str() );

		FILE* file = fopen( strCheckSumFile.c_str(), "wb+" );
		if( file )
		{
			fwrite( &dwCurrentShaderCRC, sizeof(DWORD), 1, file );
			fclose(file);
		}
	}
	else
	{
		mlog("캐쉬된 쉐이더를 사용합니다. \n");
	}
}

} // namespace rs3

