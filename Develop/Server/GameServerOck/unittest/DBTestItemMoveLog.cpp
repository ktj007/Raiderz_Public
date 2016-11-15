#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GLogCacheData.h"
#include "GConfig.h"
#include "GItemMoveLogCache.h"

SUITE (DBTEST_ITEM_MOVE_LOG)
{
	class TEST_ITEM_MOVE : public GLOG_DATA_ITEM_MOVE
	{
	public :
		TEST_ITEM_MOVE() : GLOG_DATA_ITEM_MOVE() {}
		TEST_ITEM_MOVE(int64 nAID, int64 nOwnerID, int nCode, int nCharPtm, int nDeltaCharPtm, uint8 nLev, int nCurMoney, int64 nIUID, int nItemID, int16 nCurStackAmt
			, int64 nRefAID, int64 nRefOwnerID)
			: GLOG_DATA_ITEM_MOVE(nAID, nOwnerID, nCode, nCharPtm, nDeltaCharPtm, nLev, nCurMoney, nIUID, nItemID, nCurStackAmt, nRefAID, nRefOwnerID)
		{

		}

		void Check()
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_MONEY, ITEM_ID, CUR_STACK_AMT, DELTA_STACK_AMT\
								 , REF_ACCN_ID, REF_OWNER_ID\
								 FROM dbo.RZ_ITEM_LOG \
								 WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND OWNER_ID = %I64d AND CODE = %d;"
								 , m_nAID, GConfig::m_nMyWorldID, m_nOwnerID, m_nCode);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(m_nCharPtm, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nDeltaCharPtm, rs.Field(L"DELTA_CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nLev, rs.Field(L"LEV").AsByte());
			CHECK_EQUAL(m_nCurMoney, rs.Field(L"CUR_MONEY").AsInt());
			CHECK_EQUAL(m_nItemID, rs.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(m_nCurStackAmt, rs.Field(L"CUR_STACK_AMT").AsShort());
			CHECK_EQUAL(0, rs.Field(L"DELTA_STACK_AMT").AsShort());
			CHECK_EQUAL(m_nRefAID, rs.Field(L"REF_ACCN_ID").AsInt64());
			CHECK_EQUAL(m_nRefOwnerID, rs.Field(L"REF_OWNER_ID").AsInt64());
		}	
	};

	LOGDB_TEST(USP_RZ_ITEM_MOVE_LOG)
	{
		TEST_ITEM_MOVE m_Log[GItemMoveLogCache::MAX_LOG_COUNT];

		for (int i = 0; i < GItemMoveLogCache::MAX_LOG_COUNT; ++i)
			m_Log[i] = TEST_ITEM_MOVE(i+1, i+2, i+3, i+4, i+5, i+6, i+7, i+8, i+9, i+10, i+11, i+12);
		
		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_MOVE_LOG (%d\
						 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %I64d, %I64d\
						 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %I64d, %I64d\
						 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %I64d, %I64d\
						 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %I64d, %I64d\
						 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %I64d, %I64d)}"
						 , GConfig::m_nMyWorldID
						 , m_Log[0].m_nAID, m_Log[0].m_nOwnerID, m_Log[0].m_nCode, m_Log[0].m_nCharPtm, m_Log[0].m_nDeltaCharPtm, m_Log[0].m_nLev, m_Log[0].m_nCurMoney, m_Log[0].m_nIUID, m_Log[0].m_nItemID, m_Log[0].m_nCurStackAmt, m_Log[0].m_nRefAID, m_Log[0].m_nRefOwnerID
						 , m_Log[1].m_nAID, m_Log[1].m_nOwnerID, m_Log[1].m_nCode, m_Log[1].m_nCharPtm, m_Log[1].m_nDeltaCharPtm, m_Log[1].m_nLev, m_Log[1].m_nCurMoney, m_Log[1].m_nIUID, m_Log[1].m_nItemID, m_Log[1].m_nCurStackAmt, m_Log[1].m_nRefAID, m_Log[1].m_nRefOwnerID
						 , m_Log[2].m_nAID, m_Log[2].m_nOwnerID, m_Log[2].m_nCode, m_Log[2].m_nCharPtm, m_Log[2].m_nDeltaCharPtm, m_Log[2].m_nLev, m_Log[2].m_nCurMoney, m_Log[2].m_nIUID, m_Log[2].m_nItemID, m_Log[2].m_nCurStackAmt, m_Log[2].m_nRefAID, m_Log[2].m_nRefOwnerID
						 , m_Log[3].m_nAID, m_Log[3].m_nOwnerID, m_Log[3].m_nCode, m_Log[3].m_nCharPtm, m_Log[3].m_nDeltaCharPtm, m_Log[3].m_nLev, m_Log[3].m_nCurMoney, m_Log[3].m_nIUID, m_Log[3].m_nItemID, m_Log[3].m_nCurStackAmt, m_Log[3].m_nRefAID, m_Log[3].m_nRefOwnerID
						 , m_Log[4].m_nAID, m_Log[4].m_nOwnerID, m_Log[4].m_nCode, m_Log[4].m_nCharPtm, m_Log[4].m_nDeltaCharPtm, m_Log[4].m_nLev, m_Log[4].m_nCurMoney, m_Log[4].m_nIUID, m_Log[4].m_nItemID, m_Log[4].m_nCurStackAmt, m_Log[4].m_nRefAID, m_Log[4].m_nRefOwnerID);

		for (int i = 0; i < GItemMoveLogCache::MAX_LOG_COUNT; ++i)
			m_Log[i].Check();
	}
}
