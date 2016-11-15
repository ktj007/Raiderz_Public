#include "stdafx.h"
#include "UTestDatabaseContainer.h"
#include "SDBManager.h"
#include "UTestDatabase.h"


UTestDatabaseContainer* UTestDatabaseContainer::m_pTDBMgrContainer = NULL;
int UTestDatabaseContainer::m_nCrateCnt = 0;


UTestDatabaseContainer::UTestDatabaseContainer()
{
	m_pUTestDB = NULL;
	++m_nCrateCnt;
	_ASSERT(0 <= m_nCrateCnt && 1 >= m_nCrateCnt);
	m_pTDBMgrContainer = this;
}


UTestDatabaseContainer::~UTestDatabaseContainer()
{
	m_pTDBMgrContainer = NULL;
	--m_nCrateCnt;
}


UTestDatabase& UTestDatabaseContainer::GetUTestDB()
{
	return *(m_pUTestDB);
}

void UTestDatabaseContainer::Create()
{
	m_pUTestDB = new UTestDatabase();
}

void UTestDatabaseContainer::Release()
{
	m_pUTestDB->Disconnect();
	m_pUTestDB->Release();

	SAFE_DELETE(m_pUTestDB);
}