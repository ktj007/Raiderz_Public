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
}

bool MUnitTestConfig::CheckStageFilter( const char* szStageName )
{
	return true;
}

bool MUnitTestConfig::IsExistFile( const char *filename )
{
	return false;
}