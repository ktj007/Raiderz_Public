#pragma once

#include "FileInfo.h"

class XPath
{
private:
	static void AddPathSeperator(wchar_t* szPath);
	static void _CreatePath( const wchar_t* szRootPath, const wchar_t* szSubPath  );
public:
	static bool CreateDirectoryInMyDocuments(const wchar_t* szSubPath);
	static wstring GetMyDocumentsRaiderzSubPath(const wchar_t* szSubPath);

	static wstring GetConfigFileFullPath();
	static wstring GetConfigRelFileFullPath();
	static wstring GetHistoryFileFullPath();
	static wstring GetAccountFileFullPath();
	static wstring GetShaderCachingFileFullPath();
};