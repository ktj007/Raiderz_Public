#include "stdafx.h"
#include "SUnitTestStageLogDB.h"
#include "DBTestGlobal.h"
#include "UTestDatabase.h"
#include "MStringUtil.h"


void SUnitTestStageLogDB::Create( MUnitTestConfig& config )
{
	m_DBBuilder.LoadDropTableOrder(DB_UNIT_TEST_PATH L"RZ_LOGDB_DROP_TABLE_ORDER.xml");
	m_DBBuilder.LoadTableScript(DB_SCRIPT_PATH L"RZ_LOGDB_TABLE.sql");
	m_DBBuilder.LoadProcScript(DB_SCRIPT_PATH L"RZ_LOGDB_PROCEDURE.sql");
	m_DBBuilder.MakeTableSQL();
	
	m_DBBuilder.SetDesc(config.m_strOdbcDriver, config.m_strDBServer, config.m_strDBName, config.m_strDBUserName, config.m_strDBPassword);
	m_DBBuilder.CreateDBContainer();	
}

void SUnitTestStageLogDB::Destroy()
{
	m_DBBuilder.ReleaseDB();
	m_DBBuilder.ReleaseDBContainer();
}

void SUnitTestStageLogDB::OnTestStart()
{
	m_DBBuilder.CreateTestDB();	
}

void SUnitTestStageLogDB::OnTestFinish( UnitTest::TestDetails const& test, int const testTimeInMs )
{

}