#include "stdafx.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"
#include "GDBTaskEmblemInsert.h"
#include "GDBTaskEmblemDelete.h"
#include "GDBTaskDataEmblem.h"
#include "GConfig.h"

SUITE(DBTestEmblem)
{
	GAMEDB_TEST(USP_RZ_CHAR_GET_EMBLEM_NOT_SELECTED)
	{
		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int nEmblID	= DBTestHelper::InsertEmblemInfo(1);

		DBTestHelper::InsertEmblem(GConfig::m_nMyWorldID, nAID, nCID, nEmblID, false);

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_CHAR_GET_EMBLEM (%d, %I64d, %I64d)}",GConfig::m_nMyWorldID, nAID, nCID));
		CHECK_EQUAL(nEmblID, rs.Field(L"EMBLEM_ID").AsInt());
		CHECK_EQUAL(false, rs.Field(L"SELECTED").AsBool());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_CHAR_GET_EMBLEM_SELECTED)
	{
		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int nEmblID	= DBTestHelper::InsertEmblemInfo(1);

		DBTestHelper::InsertEmblem(GConfig::m_nMyWorldID, nAID, nCID, nEmblID, true);

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_CHAR_GET_EMBLEM (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, nAID, nCID));
		CHECK_EQUAL(nEmblID, rs.Field(L"EMBLEM_ID").AsInt());
		CHECK_EQUAL(true, rs.Field(L"SELECTED").AsBool());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_EMBLEM_INSERT)
	{
		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int nEmblID	= DBTestHelper::InsertEmblemInfo(1);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_EMBLEM_INSERT (%d, %I64d, %I64d, %d)}", GConfig::m_nMyWorldID, nAID, nCID, nEmblID));

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT EMBLEM_ID, SELECTED FROM dbo.RZ_EMBLEM WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d;", nAID, nCID);
		CHECK_EQUAL(nEmblID, rs.Field(L"EMBLEM_ID").AsInt());
		CHECK_EQUAL(false, rs.Field(L"SELECTED").AsBool());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_EMBLEM_DELETE)
	{
		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int nEmblID	= DBTestHelper::InsertEmblemInfo(1);

		DBTestHelper::InsertEmblem(GConfig::m_nMyWorldID, nAID, nCID, nEmblID, true);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_EMBLEM_DELETE (%d, %I64d, %I64d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nEmblID));

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT COUNT(*) cnt FROM dbo.RZ_EMBLEM WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d AND EMBLEM_ID = %d;", nAID, nCID, nEmblID);
		CHECK_EQUAL(0, rs.Field(L"cnt").AsInt());
		rs.Close();
	}

	class FakeDBManager : public GDBManager
	{
	public :
		FakeDBManager() : m_bCalledPostFunc(false), m_pTask(NULL) {}
		~FakeDBManager() { if (NULL != m_pTask) SAFE_DELETE(m_pTask); }
		bool IsCalledPostFunc() { return m_bCalledPostFunc; }
		GDBAsyncTask* GetTask() { return m_pTask; }

		bool Post(GDBAsyncTask* pTask) 
		{ 
			m_pTask = (GDBAsyncTask*)pTask;
			m_bCalledPostFunc = true; 
			return true; 
		}

	protected :
		bool			m_bCalledPostFunc;
		GDBAsyncTask*	m_pTask;
	};

	class FFakePost
	{
	public :
		FFakePost() : uidPlayer(1), nCID(2), nQualifyID(3) {}
		bool SetupFixture()
		{
			if (1 != uidPlayer || 2 != nCID || 3 != nQualifyID)
				return false;

			return true;
		}

	public :
		FakeDBManager	fdb;
		const MUID		uidPlayer;
		const int		nCID;
		const int		nQualifyID;
	};

	TEST_FIXTURE(FFakePost, PostQualifyInsert)
	{
		CHECK(SetupFixture());

		GDBT_EMBLEM data(1, uidPlayer, nCID, nQualifyID);

		CHECK(fdb.EmblemInsert(data));
		CHECK(fdb.IsCalledPostFunc());

		GDBAsyncTask* pTask = fdb.GetTask();
		CHECK_EQUAL(SDBT_DBTYPE_GAMEDB, pTask->GetDBType());
		CHECK_EQUAL(SDBTID_QUALIFYINSERT, pTask->GetID());
		CHECK_EQUAL(uidPlayer, pTask->GetReqPlayer());

		GDBTaskEmblemInsert* pQualifyTask = (GDBTaskEmblemInsert*)pTask;
		CHECK_EQUAL(uidPlayer, pQualifyTask->GetData().uidPlayer);
		CHECK_EQUAL(nQualifyID, pQualifyTask->GetData().nQualifyID);
	}

	TEST_FIXTURE(FFakePost, PostQualifyDelete)
	{
		CHECK(SetupFixture());

		GDBT_EMBLEM data(1, uidPlayer, nCID, nQualifyID);

		CHECK(fdb.EmblemDelete(data));
		CHECK(fdb.IsCalledPostFunc());

		GDBAsyncTask* pTask = fdb.GetTask();
		CHECK_EQUAL(SDBT_DBTYPE_GAMEDB, pTask->GetDBType());
		CHECK_EQUAL(SDBTID_QUALIFYDELETE, pTask->GetID());
		CHECK_EQUAL(uidPlayer, pTask->GetReqPlayer());

		GDBTaskEmblemDelete* pQualifyTask = (GDBTaskEmblemDelete*)pTask;
		CHECK_EQUAL(uidPlayer, pQualifyTask->GetData().uidPlayer);
		CHECK_EQUAL(nQualifyID, pQualifyTask->GetData().nQualifyID);
	}	
}