#include "stdafx.h"
#include "GDBTaskNPCKillLogBuilder.h"
#include "GDBTaskSQL.h"
#include "GTimeCalculator.h"


class MockNPCKillLogBuilder : public GDBTaskNPCKillLogBuilder
{
public :
	MockNPCKillLogBuilder() : GDBTaskNPCKillLogBuilder() {}
	~MockNPCKillLogBuilder() 
	{
		for (size_t i = 0; i < m_vTask.size(); ++i)
			delete m_vTask[i];
	} 

protected :
	virtual void PostTask(GLogDBTaskQuery* pTask) override
	{
		m_vTask.push_back(pTask);
	}

public :
	vector<GLogDBTaskQuery*> m_vTask;

	using GDBTaskNPCKillLogBuilder::m_Player;
};

SUITE(TEST_NPC_KILL_LOG_BUILDER)
{
	TEST(DEFAULT_PALYER)
	{
		MockNPCKillLogBuilder nkb;

		for (size_t i = 0; i < GNPC_KILLER::MAX_COUNT; ++i)
		{
			CHECK_EQUAL(0, nkb.m_Player[i].m_nAID);
			CHECK_EQUAL(0, nkb.m_Player[i].m_nCID);
			CHECK_EQUAL(0, nkb.m_Player[i].m_nCharPtm);
			CHECK_EQUAL(0, nkb.m_Player[i].m_nDeltaCharPtm);
			CHECK_EQUAL(0, nkb.m_Player[i].m_nLevel);
		}
	}

	TEST(SET_KILL_PLAYER)
	{
		vector<GNPC_KILLER> vPlayer;

		for (size_t i = 0; i < GNPC_KILLER::MAX_COUNT; ++i)
			vPlayer.push_back(GNPC_KILLER(i+1, i+2, i+3, i+4, i+5));
		
		MockNPCKillLogBuilder nkb;

		for (size_t j = 0; j < vPlayer.size(); ++j)
		{
			nkb.SetPlayer(j, vPlayer[j]);
		}

		for (size_t k = 0; k < GNPC_KILLER::MAX_COUNT; ++k)
		{
			CHECK_EQUAL(nkb.m_Player[k].m_nAID, vPlayer[k].m_nAID);
			CHECK_EQUAL(nkb.m_Player[k].m_nCID, vPlayer[k].m_nCID);
			CHECK_EQUAL(nkb.m_Player[k].m_nCharPtm, vPlayer[k].m_nCharPtm);
			CHECK_EQUAL(nkb.m_Player[k].m_nDeltaCharPtm, vPlayer[k].m_nDeltaCharPtm);
			CHECK_EQUAL(nkb.m_Player[k].m_nLevel, vPlayer[k].m_nLevel);
		}
	}

	TEST(RESET_DATA)
	{
		MockNPCKillLogBuilder nkb;

		for (size_t i = 0; i < GNPC_KILLER::MAX_COUNT; ++i)
			nkb.SetPlayer(i, GNPC_KILLER(i+1, i+2, i+3, i+4, i+5));

		nkb.Reset();

		for (size_t i = 0; i < GNPC_KILLER::MAX_COUNT; ++i)
		{
			CHECK_EQUAL(0, nkb.m_Player[i].m_nAID);
			CHECK_EQUAL(0, nkb.m_Player[i].m_nCID);
			CHECK_EQUAL(0, nkb.m_Player[i].m_nCharPtm);
			CHECK_EQUAL(0, nkb.m_Player[i].m_nDeltaCharPtm);
			CHECK_EQUAL(0, nkb.m_Player[i].m_nLevel);
		}		
	}

	TEST(POST_TASK)
	{
		const int		WORLD		= 1;
		const int		NPC_ID		= 3;
		const wstring	strRegDate	= GTimeCalculator::GetTimeAsString(GTimeSec(0));

		MockNPCKillLogBuilder nkb;

		vector<GNPC_KILLER> vKiller;

		for (size_t i = 0; i < (GNPC_KILLER::MAX_COUNT + 1); ++i)
			vKiller.push_back(GNPC_KILLER(i+1, i+2, i+3, i+4, i+5));

		nkb.PostNpcKillLog(WORLD, strRegDate, NPC_ID, vKiller);

		CHECK_EQUAL(2, nkb.m_vTask.size());

		CString strSQL_1;
		strSQL_1.Format(L"{CALL dbo.USP_RZ_KILL_NPC_LOG (%d, %s, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d)}"
					   , WORLD, strRegDate.c_str(), NPC_ID
					   , vKiller[0].m_nAID, vKiller[0].m_nCID, vKiller[0].m_nCharPtm, vKiller[0].m_nDeltaCharPtm, vKiller[0].m_nLevel
					   , vKiller[1].m_nAID, vKiller[1].m_nCID, vKiller[1].m_nCharPtm, vKiller[1].m_nDeltaCharPtm, vKiller[1].m_nLevel
					   , vKiller[2].m_nAID, vKiller[2].m_nCID, vKiller[2].m_nCharPtm, vKiller[2].m_nDeltaCharPtm, vKiller[2].m_nLevel
					   , vKiller[3].m_nAID, vKiller[3].m_nCID, vKiller[3].m_nCharPtm, vKiller[3].m_nDeltaCharPtm, vKiller[3].m_nLevel
					   , vKiller[4].m_nAID, vKiller[4].m_nCID, vKiller[4].m_nCharPtm, vKiller[4].m_nDeltaCharPtm, vKiller[4].m_nLevel);

		const int64 nZero = 0;
		CString strSQL_2;
		strSQL_2.Format(L"{CALL dbo.USP_RZ_KILL_NPC_LOG (%d, %s, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d)}"
					   , WORLD, strRegDate.c_str(), NPC_ID
					   , vKiller[5].m_nAID, vKiller[5].m_nCID, vKiller[5].m_nCharPtm, vKiller[5].m_nDeltaCharPtm, vKiller[5].m_nLevel
					   , nZero, nZero, nZero, nZero, nZero
					   , nZero, nZero, nZero, nZero, nZero
					   , nZero, nZero, nZero, nZero, nZero
					   , nZero, nZero, nZero, nZero, nZero);

		GLogDBTaskQuery* pTask_1 = nkb.m_vTask[0];
		GLogDBTaskQuery* pTask_2 = nkb.m_vTask[1];

		const wstring strSql_1(strSQL_1.GetBuffer());
		const wstring strSql_2(strSQL_2.GetBuffer());

		const wstring strTaskSql_1 = pTask_1->GetSQLW(0);
		const wstring strTaskSql_2 = pTask_2->GetSQLW(0);

		CHECK(strSql_1 == strTaskSql_1);
		CHECK(strSql_2 == strTaskSql_2);
	}
}