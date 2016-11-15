#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"
#include "GTimeCalculator.h"
#include "GItemDeleteLogCache.h"


SUITE(DBTestItemDeleteLog)
{
	class MOCK_LOG_DATA_ITEM_DELETE : public GLOG_DATA_ITEM_DELETE
	{
	public :
		MOCK_LOG_DATA_ITEM_DELETE() : GLOG_DATA_ITEM_DELETE() {}
		MOCK_LOG_DATA_ITEM_DELETE(int64 nAID, int64 nCID, int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nCurMoney, int64 nIUID, int nItemID, int16 nDeltaStackAmt, uint8 nMaxDura, int nUsagePeriod, const wstring& strEffEndDate, const wstring& strRegDate)
			: GLOG_DATA_ITEM_DELETE(nAID, nCID, nCharPtm, nDeltaCharPtm, nLevel, nCurMoney, nIUID, nItemID, nDeltaStackAmt, nMaxDura, nUsagePeriod, strEffEndDate, strRegDate) {}

		void Check()
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT WORLD_ID, ITEM_ID, CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_MONEY, CUR_STACK_AMT, DELTA_STACK_AMT, MAX_DURA, EFF_END_DATE, REG_DATE FROM dbo.RZ_ITEM_LOG \
								 WHERE ACCN_ID = %I64d AND OWNER_ID = %I64d AND CODE = 801 AND IUID = %I64d;"
								 , m_nAID, m_nCID, m_nIUID);
			CHECK_EQUAL(GConfig::m_nMyWorldID, rs.Field(L"WORLD_ID").AsInt());
			CHECK_EQUAL(m_nItemID, rs.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(0, rs.Field(L"CUR_STACK_AMT").AsInt());
			CHECK_EQUAL(m_nDeltaStackAmt, rs.Field(L"DELTA_STACK_AMT").AsInt());
			CHECK_EQUAL(m_nCharPtm, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nLevel, rs.Field(L"LEV").AsByte());
			CHECK_EQUAL(m_nCurMoney, rs.Field(L"CUR_MONEY").AsInt());
			CHECK_EQUAL(m_nDeltaCharPtm, rs.Field(L"DELTA_CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nLevel, rs.Field(L"LEV").AsByte());
			CHECK_EQUAL(m_nMaxDura, rs.Field(L"MAX_DURA").AsInt());
			CHECK(m_strEffEndDate == wstring(L"'") + rs.Field(L"EFF_END_DATE").AsWString() + wstring(L"'"));
			CHECK(m_strRegDate == wstring(L"'") + rs.Field(L"REG_DATE").AsWString() + wstring(L"'"));
			rs.Close();
		}
	};

	LOGDB_TEST(USP_RZ_ITEM_DELETE_LOG)
	{
		MOCK_LOG_DATA_ITEM_DELETE m_Log[GItemDeleteLogCache::MAX_LOG_COUNT];

		for (int i = 0; i < GItemDeleteLogCache::MAX_LOG_COUNT; ++i)
			m_Log[i] = MOCK_LOG_DATA_ITEM_DELETE(i+1, i+2, i+3, i+4, i+5, i+6, i+7, i+8, i+9, i+10, i+11, GTimeCalculator::GetTimeAsString(GTimeSec(i)), GTimeCalculator::GetTimeAsString(GTimeSec(i + 1)));

		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_DELETE_LOG (%d\
						 , %I64d, %I64d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s\
						 , %I64d, %I64d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s\
						 , %I64d, %I64d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s\
						 , %I64d, %I64d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s\
						 , %I64d, %I64d, %s, %d, %d, %d, %d, %I64d, %d, %d, %d, %s, %s)}"
						 , GConfig::m_nMyWorldID
						 , m_Log[0].m_nAID, m_Log[0].m_nCID, m_Log[0].m_strRegDate.c_str(), m_Log[0].m_nCharPtm, m_Log[0].m_nDeltaCharPtm, m_Log[0].m_nLevel, m_Log[0].m_nCurMoney, m_Log[0].m_nIUID, m_Log[0].m_nItemID, m_Log[0].m_nDeltaStackAmt, m_Log[0].m_nMaxDura, m_Log[0].m_strUsagePeriod, m_Log[0].m_strEffEndDate.c_str()
						 , m_Log[1].m_nAID, m_Log[1].m_nCID, m_Log[1].m_strRegDate.c_str(), m_Log[1].m_nCharPtm, m_Log[1].m_nDeltaCharPtm, m_Log[1].m_nLevel, m_Log[1].m_nCurMoney, m_Log[1].m_nIUID, m_Log[1].m_nItemID, m_Log[1].m_nDeltaStackAmt, m_Log[1].m_nMaxDura, m_Log[1].m_strUsagePeriod, m_Log[1].m_strEffEndDate.c_str()
						 , m_Log[2].m_nAID, m_Log[2].m_nCID, m_Log[2].m_strRegDate.c_str(), m_Log[2].m_nCharPtm, m_Log[2].m_nDeltaCharPtm, m_Log[2].m_nLevel, m_Log[2].m_nCurMoney, m_Log[2].m_nIUID, m_Log[2].m_nItemID, m_Log[2].m_nDeltaStackAmt, m_Log[2].m_nMaxDura, m_Log[2].m_strUsagePeriod, m_Log[2].m_strEffEndDate.c_str()
						 , m_Log[3].m_nAID, m_Log[3].m_nCID, m_Log[3].m_strRegDate.c_str(), m_Log[3].m_nCharPtm, m_Log[3].m_nDeltaCharPtm, m_Log[3].m_nLevel, m_Log[3].m_nCurMoney, m_Log[3].m_nIUID, m_Log[3].m_nItemID, m_Log[3].m_nDeltaStackAmt, m_Log[3].m_nMaxDura, m_Log[3].m_strUsagePeriod, m_Log[3].m_strEffEndDate.c_str()
						 , m_Log[4].m_nAID, m_Log[4].m_nCID, m_Log[4].m_strRegDate.c_str(), m_Log[4].m_nCharPtm, m_Log[4].m_nDeltaCharPtm, m_Log[4].m_nLevel, m_Log[4].m_nCurMoney, m_Log[4].m_nIUID, m_Log[4].m_nItemID, m_Log[4].m_nDeltaStackAmt, m_Log[4].m_nMaxDura, m_Log[4].m_strUsagePeriod, m_Log[4].m_strEffEndDate.c_str());

		for (int i = 0; i < GItemDeleteLogCache::MAX_LOG_COUNT; ++i)
			m_Log[i].Check();
	}
}