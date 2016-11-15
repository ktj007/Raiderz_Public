#include "stdafx.h"
//#include "SUnitTestStageDB.h"
//#include "DBTestGlobal.h"
//#include "UTestDatabase.h"
//#include "MStringUtil.h"
//
//
//SUnitTestStageGameDB::SUnitTestStageGameDB()
//{
//	
//}
//
//void SUnitTestStageGameDB::Create(MUnitTestConfig& config)
//{
//	m_DBBuilder.LoadDropTableOrder(DBSCRIPT_PATH"RZ_GAEMDB_DROP_TABLE_ORDER.xml");
//	m_DBBuilder.LoadTableScript(DBSCRIPT_PATH"RZ_GAMEDB_TABLE.sql");
//	m_DBBuilder.LoadProcScript(DBSCRIPT_PATH"RZ_GAMEDB_PROCEDURE.sql");
//	m_DBBuilder.MakeTableSQL();
//	m_DBBuilder.SetDesc(config.m_strDBServer, config.m_strDBName, config.m_strDBUserName, config.m_strDBPassword);
//	m_DBBuilder.CreateDBContainer();	
//}
//
//void SUnitTestStageGameDB::Destroy()
//{
//	m_DBBuilder.ReleaseDB();
//	m_DBBuilder.ReleaseDBContainer();
//}
//
//void SUnitTestStageGameDB::OnTestStart()
//{
//	m_DBBuilder.CreateTestDB();	
//}
//
//void SUnitTestStageGameDB::OnTestFinish(UnitTest::TestDetails const& test, int const testTimeInMs)
//{	
//}
//
