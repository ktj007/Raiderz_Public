#include "stdafx.h"
#include "XApplicationConfig.h"

#define CONFIG_TOKEN_APP_PMANG		L"SERVER"

vector<wstring> XApplicationConfig::m_vecIPString;

void XApplicationConfig::Load(const wchar_t* szFileName)
{
	int nIPCount = GetPrivateProfileInt(CONFIG_TOKEN_APP_PMANG, L"COUNT", 0, szFileName);

	// 1-base index
	for(int i = 1; i <= nIPCount; i++)
	{
		wchar_t szKeyName[256];
		wsprintf(szKeyName, L"IP%d", i);

		wchar_t szValue[256];
		if (0 < GetPrivateProfileString(CONFIG_TOKEN_APP_PMANG, szKeyName, L"localhost", szValue, 256, szFileName))
		{
			m_vecIPString.push_back(szValue);
		}
	}
}
