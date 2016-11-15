#pragma once

#include "ServerCommonUnitTestLib.h"

class UTestDatabase;
class SUnitTestStageGameDB;
class SUnitTestDBBuilder;

/*
UTestDatabaseContainer클래스는 유닛테스트에서 Database객체를 사용할 수 있게 싱글턴으로 가지고 있기 위한 클래스.
Database객체 생성과 삭제 외에 다른 기능을 추가하면 안됨. - sunge.
*/
class SCOMMON_UNITTEST_API UTestDatabaseContainer
{
public :
	static UTestDatabaseContainer& GetUTestDatabaseContainer() 
	{ 
		return *(UTestDatabaseContainer::m_pTDBMgrContainer);
	}

	UTestDatabase& GetUTestDB();

	void Create();
	void Release();

private :
	friend SUnitTestDBBuilder;

	UTestDatabaseContainer();
	~UTestDatabaseContainer();	
	
private :
	static UTestDatabaseContainer*	m_pTDBMgrContainer;
	static int						m_nCrateCnt;

	UTestDatabase*					m_pUTestDB;
};


#define UTestDB UTestDatabaseContainer::GetUTestDatabaseContainer().GetUTestDB()