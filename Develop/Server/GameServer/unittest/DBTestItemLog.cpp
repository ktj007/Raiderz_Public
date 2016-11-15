#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GTimeCalculator.h"
#include "GConfig.h"


SUITE(DBTEST_ITEM_LOG)
{
	class MOCK_ITEM_LOG : public GLOG_DATA_ITEM
	{
	public :
		MOCK_ITEM_LOG()
		{
		}

		MOCK_ITEM_LOG(int64 nAID, int64 nCID, int nCode, int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nCurMoney, int64 nIUID, int nItemID, int16 nStackAmt, int nModStackAmt, const wstring& strRegDate, int nNpcID)
			:  GLOG_DATA_ITEM(nAID, nCID, nCode, nCharPtm, nDeltaCharPtm, nLevel, nCurMoney, nIUID, nItemID, nStackAmt, nModStackAmt, strRegDate, nNpcID)
		{

		}

		void Check()
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT ITEM_ID, CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_MONEY, CUR_STACK_AMT, DELTA_STACK_AMT, REG_DATE, NPC_ID\
								 FROM dbo.RZ_ITEM_LOG \
								 WHERE ACCN_ID = %I64d AND OWNER_ID = %I64d AND CODE = %d AND IUID = %I64d;"
								 , m_nAID, m_nCID, m_nCode, m_nIUID);
			CHECK_EQUAL(m_nCurMoney, rs.Field(L"CUR_MONEY").AsInt());
			CHECK_EQUAL(m_nItemID, rs.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(m_nCurStackAmt, rs.Field(L"CUR_STACK_AMT").AsInt());
			CHECK_EQUAL(m_nDeltaStackAmt, rs.Field(L"DELTA_STACK_AMT").AsInt());
			CHECK_EQUAL(m_nCharPtm, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nDeltaCharPtm, rs.Field(L"DELTA_CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nLevel, rs.Field(L"LEV").AsByte());
			CHECK(m_strRegDate == wstring(L"'") + rs.Field(L"REG_DATE").AsWString() + wstring(L"'"));
			CHECK_EQUAL(m_strNpcID, rs.Field(L"NPC_ID").AsWString().c_str());
			rs.Close();
		}	
	};

	LOGDB_TEST(USP_RZ_ITEM_LOG)
	{
		int nWorld = 1;
		MOCK_ITEM_LOG m_Log[GItemLogCache::MAX_LOG_COUNT];

		for (int i = 0; i < GItemLogCache::MAX_LOG_COUNT; ++i)
			m_Log[i] = MOCK_ITEM_LOG(i+1, i+2, i+3, i+4, i+5, i+6, i+7, i+8, i+9, i+10, i+11, GTimeCalculator::GetTimeAsString(GTimeSec(i)), i+12);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_LOG (%d\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s)}"
						 , GConfig::m_nMyWorldID
						 , m_Log[0].m_nAID, m_Log[0].m_nCID, m_Log[0].m_nCode, m_Log[0].m_strRegDate.c_str(), m_Log[0].m_nCharPtm, m_Log[0].m_nDeltaCharPtm, m_Log[0].m_nLevel, m_Log[0].m_nCurMoney, m_Log[0].m_nIUID, m_Log[0].m_nItemID, m_Log[0].m_nCurStackAmt, m_Log[0].m_nDeltaStackAmt, m_Log[0].m_strNpcID
						 , m_Log[1].m_nAID, m_Log[1].m_nCID, m_Log[1].m_nCode, m_Log[1].m_strRegDate.c_str(), m_Log[1].m_nCharPtm, m_Log[1].m_nDeltaCharPtm, m_Log[1].m_nLevel, m_Log[1].m_nCurMoney, m_Log[1].m_nIUID, m_Log[1].m_nItemID, m_Log[1].m_nCurStackAmt, m_Log[1].m_nDeltaStackAmt, m_Log[1].m_strNpcID
						 , m_Log[2].m_nAID, m_Log[2].m_nCID, m_Log[2].m_nCode, m_Log[2].m_strRegDate.c_str(), m_Log[2].m_nCharPtm, m_Log[2].m_nDeltaCharPtm, m_Log[2].m_nLevel, m_Log[2].m_nCurMoney, m_Log[2].m_nIUID, m_Log[2].m_nItemID, m_Log[2].m_nCurStackAmt, m_Log[2].m_nDeltaStackAmt, m_Log[2].m_strNpcID
						 , m_Log[3].m_nAID, m_Log[3].m_nCID, m_Log[3].m_nCode, m_Log[3].m_strRegDate.c_str(), m_Log[3].m_nCharPtm, m_Log[3].m_nDeltaCharPtm, m_Log[3].m_nLevel, m_Log[3].m_nCurMoney, m_Log[3].m_nIUID, m_Log[3].m_nItemID, m_Log[3].m_nCurStackAmt, m_Log[3].m_nDeltaStackAmt, m_Log[3].m_strNpcID
						 , m_Log[4].m_nAID, m_Log[4].m_nCID, m_Log[4].m_nCode, m_Log[4].m_strRegDate.c_str(), m_Log[4].m_nCharPtm, m_Log[4].m_nDeltaCharPtm, m_Log[4].m_nLevel, m_Log[4].m_nCurMoney, m_Log[4].m_nIUID, m_Log[4].m_nItemID, m_Log[4].m_nCurStackAmt, m_Log[4].m_nDeltaStackAmt, m_Log[4].m_strNpcID);

		for (int i = 0; i < GItemLogCache::MAX_LOG_COUNT; ++i)
			m_Log[i].Check();
	}
}