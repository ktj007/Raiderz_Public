#include "StdAfx.h"
#include "TestCallCounter.h"

int TestCallCounter::GetCallCounter( wstring strFuncName )
{
	MAP_CALLCOUNTER::iterator iter = m_mapTestCallCounter.find(strFuncName);
	if (iter==m_mapTestCallCounter.end())	return 0;

	return iter->second;
}

void TestCallCounter::IncreaseCallCounter( wstring strFuncName )
{
	MAP_CALLCOUNTER::iterator iter = m_mapTestCallCounter.find(strFuncName);
	if (iter == m_mapTestCallCounter.end())
	{
		m_mapTestCallCounter.insert(MAP_CALLCOUNTER::value_type(strFuncName, 1));
	}
	else
	{
		iter->second++;
	}
}