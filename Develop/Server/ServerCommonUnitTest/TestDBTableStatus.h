#pragma once

#include "ServerCommonUnitTestLib.h"
#include "TestDBTable.h"

class SCOMMON_UNITTEST_API TestDBTableStatus
{
private:
	void Release();
public :
	TestDBTableStatus();
	~TestDBTableStatus();


	// xml에서 테스트 데이터를 읽어와서 구성한다.
	bool Load(const char* szFileName);

	TDBTableDef* GetTable(const string& strName);


	DBTableVec m_DBTableVec;
};


