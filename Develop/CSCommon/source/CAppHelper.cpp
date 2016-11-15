#include "stdafx.h"
#include "CAppHelper.h"


std::tstring CAppHelper::MakeTitleCaption( const TCHAR* szProgName, const TCHAR* szVersion, const TCHAR* szBuildNumber, const TCHAR* szDate )
{
	TCHAR szRet[256];
	TCHAR szDebug[64] = _T("");

#ifdef _PUBLISH
	_tcscpy(szDebug, _T(""));
#elif _DEBUG
	_tcscpy(szDebug, _T("Debug "));
#elif NDEBUG
	_tcscpy(szDebug, _T(""));
#else
	_tcscpy(szDebug, _T("Unknown "));
#endif

	_stprintf_s(szRet, _T("%s %s- Ver %s (Ver %s, %s)"),
		szProgName,
		szDebug,
		szVersion,
		szBuildNumber,
		szDate);
	return std::tstring(szRet);
}

std::tstring CAppHelper::MakeServerVersion( const TCHAR* szVersion, const TCHAR* szBuildNumber, const TCHAR* szDate )
{
	TCHAR szRet[256];
	TCHAR szDebug[64] = _T("");

#ifdef _PUBLISH
	_tcscpy(szDebug, _T(""));
#elif _DEBUG
	_tcscpy(szDebug, _T("Debug "));
#elif NDEBUG
	_tcscpy(szDebug, _T(""));
#else
	_tcscpy(szDebug, _T("Unknown "));
#endif

	_stprintf_s(szRet, _T("%s- Ver %s (Ver %s, %s)"),
		szDebug,
		szVersion,
		szBuildNumber,
		szDate);
	return std::tstring(szRet);
}