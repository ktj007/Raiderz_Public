#include "stdafx.h"
#include "MUnitTestConfig.h"
#include <algorithm>
#include <cctype>	// std::tolower

#define UNITTEST_CONFIG_TOKEN_APP_UNITTEST				"UNITTEST"
#define UNITTEST_CONFIG_TOKEN_APP_DB_TEST				"DB_TEST"
#define UNITTEST_CONFIG_TOKEN_APP_FILTER				"FILTER"

MUnitTestConfig::MUnitTestConfig() : m_bExecuteUnitTest(true), m_bBreakOnFailure(false), m_nRepeat(1), m_bLogSuiteName(false), m_bCheckCodeCoverage(false), m_bLogTimeElapsed(false)
{
}

bool MUnitTestConfig::Load()
{
	char filename[_MAX_PATH] = "./unittest.ini";
	char rel_filename[_MAX_PATH] = "./unittest_rel.ini";
	char szValue[256] = "";

	if (!IsExistFile(filename))
	{
		if (IsExistFile(rel_filename)) strcpy_s(filename, rel_filename);
		else return false;
	}

	// [UNITTEST] ---------
	m_bExecuteUnitTest = GetPrivateProfileBool(UNITTEST_CONFIG_TOKEN_APP_UNITTEST,	"EXECUTE",	true, filename);
	m_bBreakOnFailure = GetPrivateProfileBool(UNITTEST_CONFIG_TOKEN_APP_UNITTEST,	"BREAK_ON_FAILURE",	false, filename);
	m_nRepeat = GetPrivateProfileInt(UNITTEST_CONFIG_TOKEN_APP_UNITTEST,	"REPEAT", 1,	filename);
	m_bLogSuiteName = GetPrivateProfileBool(UNITTEST_CONFIG_TOKEN_APP_UNITTEST,	"LOG_SUITE_NAME",	false, filename);
	m_bCheckCodeCoverage = GetPrivateProfileBool(UNITTEST_CONFIG_TOKEN_APP_UNITTEST,	"CHECK_CODE_COVERAGE",	false, filename);
	m_bLogTimeElapsed = GetPrivateProfileBool(UNITTEST_CONFIG_TOKEN_APP_UNITTEST,	"LOG_TIME_ELAPSED",	false, filename);
	
	// [DB_TEST] ----------
	if (GetPrivateProfileString(UNITTEST_CONFIG_TOKEN_APP_DB_TEST, "SERVER", "", szValue, 256, filename))
	{
		m_strDBServer = szValue;
	}
	if (GetPrivateProfileString(UNITTEST_CONFIG_TOKEN_APP_DB_TEST, "DATABASE", "", szValue, 256, filename))
	{
		m_strDBName = szValue;
	}
	if (GetPrivateProfileString(UNITTEST_CONFIG_TOKEN_APP_DB_TEST, "USERNAME", "", szValue, 256, filename))
	{
		m_strDBUserName = szValue;
	}
	if (GetPrivateProfileString(UNITTEST_CONFIG_TOKEN_APP_DB_TEST, "PASSWORD", "", szValue, 256, filename))
	{
		m_strDBPassword = szValue;
	}
	if (GetPrivateProfileString(UNITTEST_CONFIG_TOKEN_APP_DB_TEST, "ODBC", "SQL Server", szValue, 256, filename))
	{
		m_strOdbcDriver = szValue;
	}
	
	m_bExecuteDB = GetPrivateProfileBool(UNITTEST_CONFIG_TOKEN_APP_DB_TEST, "EXECUTE", false, filename);
	
	// [FILTER] ---------
	if (GetPrivateProfileString(UNITTEST_CONFIG_TOKEN_APP_FILTER, "STAGE", "all", szValue, 256, filename))
	{
		m_strStageFilter = szValue;
		std::transform(m_strStageFilter.begin(), m_strStageFilter.end(), m_strStageFilter.begin(), std::tolower);
	}

	GetFilter("TEST_SUITE",				m_strSuite, filename);
	GetFilter("DISABLED_SUITE",			m_strDisabledSuite, filename);
	GetFilter("TEST",					m_strTest, filename);

	if (GetPrivateProfileString(UNITTEST_CONFIG_TOKEN_APP_FILTER, "TEST_SUITE_ANY_WORDS", "", szValue, 256, filename))
	{
		m_strSuiteAnyWords = szValue;
	}

	if (GetPrivateProfileString(UNITTEST_CONFIG_TOKEN_APP_FILTER, "CODE_COVERAGE", "", szValue, 256, filename))
	{
		m_strCodeCoverageFilter = szValue;
	}

	return true;
}


bool MUnitTestConfig::GetPrivateProfileBool(const char* szAppName, const char* szKeyName, 
						   bool bDefault, const char* szFileName)
{
	int nDefault = bDefault ? 1 : 0;

	int n;
	n = GetPrivateProfileInt(szAppName, szKeyName, nDefault, szFileName);
	if (n == 0) return false;
	return true;
}

void MUnitTestConfig::GetFilter( const char* szName, string (&outString)[UNITTEST_CONFIG_FILTER_ITEM_COUNT], const char* filename )
{
	char szValue[256];
	char szCountedName[256];

	szValue[0] = 0;
	if (!GetPrivateProfileString(UNITTEST_CONFIG_TOKEN_APP_FILTER, szName,		"", szValue, 256, filename))
	{
		sprintf_s(szCountedName, "%s%d", szName, 1);
		GetPrivateProfileString(UNITTEST_CONFIG_TOKEN_APP_FILTER, szCountedName,"", szValue, 256, filename);
	}
	outString[0] = szValue;

	for (int i = 1; i < 4; i++)
	{
		sprintf_s(szCountedName, "%s%d", szName, i+1);
		if (GetPrivateProfileString(UNITTEST_CONFIG_TOKEN_APP_FILTER, szCountedName,"", szValue, 256, filename))
		{
			outString[i] = szValue;
		}
	}
}

bool MUnitTestConfig::CheckStageFilter( const char* szStageName )
{
	string strName = szStageName;
	std::transform(strName.begin(), strName.end(), strName.begin(), std::tolower);

	if (m_strStageFilter.empty()) return true;
	if (m_strStageFilter.find("all") != std::string::npos) return true;
	if (m_strStageFilter.find(strName) != std::string::npos) return true;
	return false;
}

bool MUnitTestConfig::IsExistFile( const char *filename )
{
	FILE *file=fopen(filename,"r");
	bool breturn=(file!=NULL);
	if(file) fclose(file);
	return breturn;
}