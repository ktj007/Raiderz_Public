#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GTimeCalculator.h"
#include "GCraftLogCache.h"

SUITE(DBTEST_CRAFT)
{
	const int WORLD = 1;

	class TEST_CRAFT : public GLOG_DATA_CRAFT
	{
	public :
		TEST_CRAFT() : GLOG_DATA_CRAFT() {}
		TEST_CRAFT(int64 nAID, int64 nCID, int nCode, int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nCurMoney, int nDeltaMoney, int64 nIUID, int nItemID, int16 nCurStackAmt, int16 nDeltaStackAmt, const wstring& strRegDate, int nRefNpcID, int64 nRefIUID)
			: GLOG_DATA_CRAFT(nAID, nCID, nCode, nCharPtm, nDeltaCharPtm, nLevel, nCurMoney, nDeltaMoney, nIUID, nItemID, nCurStackAmt, nDeltaStackAmt, strRegDate, nRefNpcID, nRefIUID) {}

		void Check()
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT WORLD_ID, ITEM_ID, CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_MONEY, DELTA_MONEY, CUR_STACK_AMT, DELTA_STACK_AMT, REG_DATE, NPC_ID FROM dbo.RZ_ITEM_LOG \
								 WHERE ACCN_ID = %I64d AND OWNER_ID = %I64d AND CODE = %d AND IUID = %I64d;"
								 , m_nAID, m_nCID, m_nCode, m_nIUID);
			CHECK_EQUAL(WORLD, rs.Field(L"WORLD_ID").AsInt());
			CHECK_EQUAL(m_nItemID, rs.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(m_nCurMoney, rs.Field(L"CUR_MONEY").AsInt());
			CHECK_EQUAL(m_nDeltaMoney, rs.Field(L"DELTA_MONEY").AsInt());
			CHECK_EQUAL(m_nCurStackAmt, rs.Field(L"CUR_STACK_AMT").AsInt());
			CHECK_EQUAL(m_nDeltaStackAmt, rs.Field(L"DELTA_STACK_AMT").AsInt());
			CHECK_EQUAL(m_nCharPtm, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nDeltaCharPtm, rs.Field(L"DELTA_CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nLevel, rs.Field(L"LEV").AsByte());
			CHECK(m_strRegDate == wstring(L"'") + rs.Field(L"REG_DATE").AsWString() + wstring(L"'"));
			CHECK(m_strRefNpcID == rs.Field(L"NPC_ID").AsWString().c_str());
			CString tmp =  rs.Field(L"NPC_ID").AsWString().c_str();
			rs.Close();
		}
	};

	LOGDB_TEST(USP_RZ_CRAFT_LOG)
	{
		TEST_CRAFT m_Log[GCraftLogCache::MAX_LOG_COUNT];
		for (size_t i = 1; i <= GCraftLogCache::MAX_LOG_COUNT; ++i)
			m_Log[i - 1] = TEST_CRAFT(i, i, i, i, i, i, i, i, i, i, i, i, GTimeCalculator::GetTimeAsString(GTimeSec(i)), i+1, i+2);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_CRAFT_LOG (%d\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s)}"
						 , WORLD
						 , m_Log[0].m_nAID, m_Log[0].m_nCID, m_Log[0].m_nCode, m_Log[0].m_strRegDate.c_str(), m_Log[0].m_nCharPtm, m_Log[0].m_nDeltaCharPtm, m_Log[0].m_nLevel, m_Log[0].m_nCurMoney, m_Log[0].m_nDeltaMoney, m_Log[0].m_nIUID, m_Log[0].m_nItemID, m_Log[0].m_nCurStackAmt, m_Log[0].m_nDeltaStackAmt, m_Log[0].m_strRefNpcID, m_Log[0].m_strRefIUID
						 , m_Log[1].m_nAID, m_Log[1].m_nCID, m_Log[1].m_nCode, m_Log[1].m_strRegDate.c_str(), m_Log[1].m_nCharPtm, m_Log[1].m_nDeltaCharPtm, m_Log[1].m_nLevel, m_Log[1].m_nCurMoney, m_Log[1].m_nDeltaMoney, m_Log[1].m_nIUID, m_Log[1].m_nItemID, m_Log[1].m_nCurStackAmt, m_Log[1].m_nDeltaStackAmt, m_Log[1].m_strRefNpcID, m_Log[1].m_strRefIUID
						 , m_Log[2].m_nAID, m_Log[2].m_nCID, m_Log[2].m_nCode, m_Log[2].m_strRegDate.c_str(), m_Log[2].m_nCharPtm, m_Log[2].m_nDeltaCharPtm, m_Log[2].m_nLevel, m_Log[2].m_nCurMoney, m_Log[2].m_nDeltaMoney, m_Log[2].m_nIUID, m_Log[2].m_nItemID, m_Log[2].m_nCurStackAmt, m_Log[2].m_nDeltaStackAmt, m_Log[2].m_strRefNpcID, m_Log[2].m_strRefIUID
						 , m_Log[3].m_nAID, m_Log[3].m_nCID, m_Log[3].m_nCode, m_Log[3].m_strRegDate.c_str(), m_Log[3].m_nCharPtm, m_Log[3].m_nDeltaCharPtm, m_Log[3].m_nLevel, m_Log[3].m_nCurMoney, m_Log[3].m_nDeltaMoney, m_Log[3].m_nIUID, m_Log[3].m_nItemID, m_Log[3].m_nCurStackAmt, m_Log[3].m_nDeltaStackAmt, m_Log[3].m_strRefNpcID, m_Log[3].m_strRefIUID
						 , m_Log[4].m_nAID, m_Log[4].m_nCID, m_Log[4].m_nCode, m_Log[4].m_strRegDate.c_str(), m_Log[4].m_nCharPtm, m_Log[4].m_nDeltaCharPtm, m_Log[4].m_nLevel, m_Log[4].m_nCurMoney, m_Log[4].m_nDeltaMoney, m_Log[4].m_nIUID, m_Log[4].m_nItemID, m_Log[4].m_nCurStackAmt, m_Log[4].m_nDeltaStackAmt, m_Log[4].m_strRefNpcID, m_Log[4].m_strRefIUID);

		for (size_t i = 0; i < GCraftLogCache::MAX_LOG_COUNT; ++i)
			m_Log[i].Check();
	}	
}