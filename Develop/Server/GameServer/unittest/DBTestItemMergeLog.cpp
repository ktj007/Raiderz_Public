#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GLogCacheData.h"
#include "GItemMergeLogCache.h"
#include "GTimeCalculator.h"

SUITE(DBTET_ITEM_MERGE_AND_SPLIT_LOG)
{
	const int WORLD = 1;

	class TEST_ITEM_MERGE : public GLOG_DATA_ITEM_MERGE_AND_SPLIT
	{
	public :
		TEST_ITEM_MERGE() : GLOG_DATA_ITEM_MERGE_AND_SPLIT() {}
		TEST_ITEM_MERGE(int64 nAID, int64 nCID, int nCode, int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nMoney, int64 nIUID, int nItemID, int16 nStackAmt, int nModStackAmt, int64 nRefAID, int64 nRefOwnerID, int64 nRefIUID, int16 nRefCurStackAmt)
			: GLOG_DATA_ITEM_MERGE_AND_SPLIT(nAID, nCID, nCode, nCharPtm, nDeltaCharPtm, nLevel, nMoney, nIUID, nItemID, nStackAmt, nModStackAmt, nRefAID, nRefOwnerID, nRefIUID, nRefCurStackAmt)
		{

		}

		void Check(int64 ACCN_ID, int64 OWNER_ID, int64 IUID, int16 CUR_STACK_AMT, int16 DELTA_STACK_AMT, int64 REF_ACCN_ID, int64 REF_OWNER_ID, int64 REF_IUID)
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT WORLD_ID, CODE, ITEM_ID, CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_MONEY, CUR_STACK_AMT, DELTA_STACK_AMT\
								 , REF_ACCN_ID, REF_OWNER_ID, REF_IUID \
								 FROM dbo.RZ_ITEM_LOG \
								 WHERE ACCN_ID = %I64d AND OWNER_ID = %I64d AND CODE = %d AND IUID = %I64d;"
								 , ACCN_ID, OWNER_ID, m_nCode, IUID);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(WORLD, rs.Field(L"WORLD_ID").AsInt());
			CHECK_EQUAL(m_nCode, rs.Field(L"CODE").AsInt());
			CHECK_EQUAL(m_nItemID, rs.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(CUR_STACK_AMT, rs.Field(L"CUR_STACK_AMT").AsInt());
			CHECK_EQUAL(DELTA_STACK_AMT, rs.Field(L"DELTA_STACK_AMT").AsInt());
			CHECK_EQUAL(m_nCharPtm, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nDeltaCharPtm, rs.Field(L"DELTA_CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nLevel, rs.Field(L"LEV").AsByte());
			CHECK_EQUAL(m_nMoney, rs.Field(L"CUR_MONEY").AsInt());
			CHECK_EQUAL(REF_ACCN_ID, rs.Field(L"REF_ACCN_ID").AsInt64());
			CHECK_EQUAL(REF_OWNER_ID, rs.Field(L"REF_OWNER_ID").AsInt64());
			CHECK_EQUAL(REF_IUID, rs.Field(L"REF_IUID").AsInt64());			
			rs.Close();
		}
	};

	LOGDB_TEST(USP_RZ_ITEM_MERGE_LOG)
	{
		TEST_ITEM_MERGE log[GItemMergeAndSplitLogCache::MAX_LOG_COUNT];
		for (size_t i = 0; i < GItemMergeAndSplitLogCache::MAX_LOG_COUNT; ++i)
			log[i] = TEST_ITEM_MERGE(i + 1, i + 2, i + 3, i + 4, i + 4, i + 5, i + 6, i+7, i+8, i+9, i + 10, i + 11, i + 12, i + 13, i + 14);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_MERGE_AND_SPLIT_LOG (%d\
						 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %I64d, %I64d, %I64d, %d\
						 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %I64d, %I64d, %I64d, %d\
						 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %I64d, %I64d, %I64d, %d\
						 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %I64d, %I64d, %I64d, %d\
						 , %I64d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %I64d, %I64d, %I64d, %d)}"
						 , WORLD
						 , log[0].m_nAID, log[0].m_nOwnerID, log[0].m_nCode, log[0].m_nCharPtm, log[0].m_nDeltaCharPtm, log[0].m_nLevel, log[0].m_nMoney, log[0].m_nIUID, log[0].m_nItemID, log[0].m_nCurStackAmt, log[0].m_nDeltaStackAmt, log[0].m_nRefAID, log[0].m_nRefOwnerID, log[0].m_nRefIUID, log[0].m_nRefCurStackAmt
						 , log[1].m_nAID, log[1].m_nOwnerID, log[1].m_nCode, log[1].m_nCharPtm, log[1].m_nDeltaCharPtm, log[1].m_nLevel, log[1].m_nMoney, log[1].m_nIUID, log[1].m_nItemID, log[1].m_nCurStackAmt, log[1].m_nDeltaStackAmt, log[1].m_nRefAID, log[1].m_nRefOwnerID, log[1].m_nRefIUID, log[1].m_nRefCurStackAmt
						 , log[2].m_nAID, log[2].m_nOwnerID, log[2].m_nCode, log[2].m_nCharPtm, log[2].m_nDeltaCharPtm, log[2].m_nLevel, log[2].m_nMoney, log[2].m_nIUID, log[2].m_nItemID, log[2].m_nCurStackAmt, log[2].m_nDeltaStackAmt, log[2].m_nRefAID, log[2].m_nRefOwnerID, log[2].m_nRefIUID, log[2].m_nRefCurStackAmt
						 , log[3].m_nAID, log[3].m_nOwnerID, log[3].m_nCode, log[3].m_nCharPtm, log[3].m_nDeltaCharPtm, log[3].m_nLevel, log[3].m_nMoney, log[3].m_nIUID, log[3].m_nItemID, log[3].m_nCurStackAmt, log[3].m_nDeltaStackAmt, log[3].m_nRefAID, log[3].m_nRefOwnerID, log[3].m_nRefIUID, log[3].m_nRefCurStackAmt
						 , log[4].m_nAID, log[4].m_nOwnerID, log[4].m_nCode, log[4].m_nCharPtm, log[4].m_nDeltaCharPtm, log[4].m_nLevel, log[4].m_nMoney, log[4].m_nIUID, log[4].m_nItemID, log[4].m_nCurStackAmt, log[4].m_nDeltaStackAmt, log[4].m_nRefAID, log[4].m_nRefOwnerID, log[4].m_nRefIUID, log[4].m_nRefCurStackAmt);

		for (size_t i = 0; i < GItemMergeAndSplitLogCache::MAX_LOG_COUNT; ++i)
		{
			log[i].Check(log[i].m_nAID, log[i].m_nOwnerID, log[i].m_nIUID, log[i].m_nCurStackAmt, -log[i].m_nDeltaStackAmt, log[i].m_nRefAID, log[i].m_nRefOwnerID, log[i].m_nRefIUID);
			log[i].Check(log[i].m_nRefAID, log[i].m_nRefOwnerID, log[i].m_nRefIUID, log[i].m_nRefCurStackAmt, log[i].m_nDeltaStackAmt, log[i].m_nAID, log[i].m_nOwnerID, log[i].m_nIUID);
		}
	}
}