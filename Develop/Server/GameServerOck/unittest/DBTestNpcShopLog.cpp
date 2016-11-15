#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GNpcShopLogCache.h"
#include "GTimeCalculator.h"


SUITE(DBTEST_NPC_SHOP_LOG)
{
	const int WORLD = 1;

	class TEST_NPC_SHOP : public GLOG_DATA_NPC_SHOP
	{
	public :
		TEST_NPC_SHOP() : GLOG_DATA_NPC_SHOP() {}
		TEST_NPC_SHOP(int64 nAID, int64 nCID, int nCode, int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nCurMoney, int nDeltaMoney, int64 nIUID, int nItemID, int16 nCurStackAmt, int16 nDeltaStackAmt, const wstring& strRegDate)
			: GLOG_DATA_NPC_SHOP(nAID, nCID, nCode, nCharPtm, nDeltaCharPtm, nLevel, nCurMoney, nDeltaMoney, nIUID, nItemID, nCurStackAmt, nDeltaStackAmt, strRegDate) {}

		void Check()
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT WORLD_ID, ITEM_ID, CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_MONEY, DELTA_MONEY, CUR_STACK_AMT, DELTA_STACK_AMT, REG_DATE FROM dbo.RZ_ITEM_LOG \
								 WHERE ACCN_ID = %I64d AND OWNER_ID = %I64d AND CODE = %d AND IUID = %s;"
								 , m_nAID, m_nCID, m_nCode, m_strIUID);
			CHECK_EQUAL(WORLD, rs.Field(L"WORLD_ID").AsInt());
			CHECK(m_strItemID == rs.Field(L"ITEM_ID").AsWString().c_str());
			CHECK(m_strCurMoney == rs.Field(L"CUR_MONEY").AsWString().c_str());
			CHECK(m_strDeltaMoney == rs.Field(L"DELTA_MONEY").AsWString().c_str());
			CHECK(m_strCurStackAmt == rs.Field(L"CUR_STACK_AMT").AsWString().c_str());
			CHECK(m_strDeltaStackAmt == rs.Field(L"DELTA_STACK_AMT").AsWString().c_str());
			CHECK_EQUAL(m_nCharPtm, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nDeltaCharPtm, rs.Field(L"DELTA_CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nLevel, rs.Field(L"LEV").AsByte());
			CHECK(m_strRegDate == wstring(L"'") + rs.Field(L"REG_DATE").AsWString() + wstring(L"'"));
			rs.Close();
		}
	};

	LOGDB_TEST(USP_RZ_SELL_ITEM_LOG)
	{
		TEST_NPC_SHOP log[GNpcShopLogCache::MAX_LOG_COUNT];

		for (size_t i = 0; i < GNpcShopLogCache::MAX_LOG_COUNT; ++i)
			log[i] = TEST_NPC_SHOP(i + 1, i + 2, i + 3, i + 4, i + 5, i + 6, i + 7, i + 8, i + 9, i + 10, i+11, i+12, GTimeCalculator::GetTimeAsString(GTimeSec(1)));

		UTestDB.Execute(L"{CALL dbo.USP_RZ_NPC_SHOP_LOG (%d\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %s, %s, %s, %s, %s, %s)}"
						 , WORLD
						 , log[0].m_nAID, log[0].m_nCID, log[0].m_nCode, log[0].m_strRegDate.c_str(), log[0].m_nCharPtm, log[0].m_nDeltaCharPtm, log[0].m_nLevel, log[0].m_strCurMoney, log[0].m_strDeltaMoney, log[0].m_strIUID, log[0].m_strItemID, log[0].m_strCurStackAmt, log[0].m_strDeltaStackAmt
						 , log[1].m_nAID, log[1].m_nCID, log[1].m_nCode, log[1].m_strRegDate.c_str(), log[1].m_nCharPtm, log[1].m_nDeltaCharPtm, log[1].m_nLevel, log[1].m_strCurMoney, log[1].m_strDeltaMoney, log[1].m_strIUID, log[1].m_strItemID, log[1].m_strCurStackAmt, log[1].m_strDeltaStackAmt
						 , log[2].m_nAID, log[2].m_nCID, log[2].m_nCode, log[2].m_strRegDate.c_str(), log[2].m_nCharPtm, log[2].m_nDeltaCharPtm, log[2].m_nLevel, log[2].m_strCurMoney, log[2].m_strDeltaMoney, log[2].m_strIUID, log[2].m_strItemID, log[2].m_strCurStackAmt, log[2].m_strDeltaStackAmt
						 , log[3].m_nAID, log[3].m_nCID, log[3].m_nCode, log[3].m_strRegDate.c_str(), log[3].m_nCharPtm, log[3].m_nDeltaCharPtm, log[3].m_nLevel, log[3].m_strCurMoney, log[3].m_strDeltaMoney, log[3].m_strIUID, log[3].m_strItemID, log[3].m_strCurStackAmt, log[3].m_strDeltaStackAmt
						 , log[4].m_nAID, log[4].m_nCID, log[4].m_nCode, log[4].m_strRegDate.c_str(), log[4].m_nCharPtm, log[4].m_nDeltaCharPtm, log[4].m_nLevel, log[4].m_strCurMoney, log[4].m_strDeltaMoney, log[4].m_strIUID, log[4].m_strItemID, log[4].m_strCurStackAmt, log[4].m_strDeltaStackAmt);

		for (size_t i = 0; i < GNpcShopLogCache::MAX_LOG_COUNT; ++i)
			log[i].Check();
	}	
}