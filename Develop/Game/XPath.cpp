#include "stdafx.h"
#include "XPath.h"


bool XPath::CreateDirectoryInMyDocuments(const wchar_t* szSubPath)
{
	wchar_t raiderz_path[_MAX_PATH] = L"";
	char szPath[_MAX_PATH] = "";

	if(GetMyDocumentsPath(szPath)) 
	{
		wcscpy_s(raiderz_path, MLocale::ConvAnsiToUTF16(szPath).c_str());
		AddPathSeperator(raiderz_path);

		wcscat_s(raiderz_path, PATH_MYDOC_ROOT);

		CreatePath( raiderz_path );

		AddPathSeperator(raiderz_path);


		_CreatePath(raiderz_path, szSubPath);

		return true;
	}

	return false;
}

wstring XPath::GetMyDocumentsRaiderzSubPath( const wchar_t* szSubPath )
{
	wchar_t szRet[_MAX_PATH] = L"";	
	char szMyDocPath[_MAX_PATH] = "";

	if(GetMyDocumentsPath(szMyDocPath)) 
	{
		wcscpy_s(szRet, MLocale::ConvAnsiToUTF16(szMyDocPath).c_str());
		AddPathSeperator(szRet);

		wcscat_s(szRet, PATH_MYDOC_ROOT);
		AddPathSeperator(szRet);

		wcscat_s(szRet, szSubPath);
	}

	return szRet;
}

void XPath::AddPathSeperator( wchar_t* szPath )
{
	int len = wcslen(szPath);
	if (len > 0)
	{
		if (szPath[len-1] != L'\\' &&
			szPath[len-1] != L'/')
		{
			wcscat_s(szPath, _MAX_PATH, L"\\");
		}
	}
}

void XPath::_CreatePath( const wchar_t* szRootPath, const wchar_t* szSubPath )
{
	wchar_t szFullPath[_MAX_PATH] = L"";
	wcscpy_s(szFullPath, szRootPath);
	AddPathSeperator(szFullPath);
	wcscat_s(szFullPath, _MAX_PATH, szSubPath);
	CreatePath( szFullPath );
}

wstring XPath::GetConfigFileFullPath()
{
	wstring strConfigPath = GetMyDocumentsRaiderzSubPath(PATH_MYDOC_SAVE);
	strConfigPath = strConfigPath + FILENAME_CONFIG;
	return strConfigPath;
}

wstring XPath::GetConfigRelFileFullPath()
{
	wstring strConfigPath = GetMyDocumentsRaiderzSubPath(PATH_MYDOC_SAVE);
	strConfigPath = strConfigPath + FILENAME_CONFIG_REL;
	return strConfigPath;
}

wstring XPath::GetHistoryFileFullPath()
{
	wstring strHistoryPath = GetMyDocumentsRaiderzSubPath(PATH_MYDOC_SAVE);
	strHistoryPath = strHistoryPath + FILENAME_HISTORY;
	return strHistoryPath;
}

wstring XPath::GetAccountFileFullPath()
{
	wstring strAccountPath = GetMyDocumentsRaiderzSubPath(PATH_MYDOC_SAVE);
	strAccountPath = strAccountPath + FILENAME_ACCOUNT;
	return strAccountPath;
}

wstring XPath::GetShaderCachingFileFullPath()
{
	wstring strHistoryPath = GetMyDocumentsRaiderzSubPath(PATH_MYDOC_SHADER_CACHING);
	return strHistoryPath;
}