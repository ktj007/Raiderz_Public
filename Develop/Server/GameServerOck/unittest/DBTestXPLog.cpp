#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GXPLogCache.h"
#include "GTimeCalculator.h"

SUITE(DBTEST_XP_LOG)
{
	const int WORLD = 1;

	class MOCK_LOG_DATA_XP : public GLOG_DATA_XP
	{
	public :
		MOCK_LOG_DATA_XP() : GLOG_DATA_XP() {}
		MOCK_LOG_DATA_XP(int64 nAID, int64 nCID, int nCode, int nCharPtm, int nDeltaCharPtm, uint8 nLevel, int nCurXP, int nDeltaXP, const wstring& strRegDate, int nNpcID)
			: GLOG_DATA_XP(nAID, nCID, nCode, nCharPtm, nDeltaCharPtm, nLevel, nCurXP, nDeltaXP, strRegDate, nNpcID) {}

		void Check()
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT WORLD_ID, REG_DATE, CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_EXP, DELTA_EXP FROM dbo.RZ_CHAR_LOG \
								 WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d AND CODE = %d"
								 , m_nAID, m_nCID, m_nCode);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(WORLD, rs.Field(L"WORLD_ID").AsInt());
			CHECK_EQUAL(m_nCharPtm, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nDeltaCharPtm, rs.Field(L"DELTA_CHAR_PTM").AsInt());
			CHECK_EQUAL(m_nLevel, rs.Field(L"LEV").AsByte());
			CHECK_EQUAL(m_nCurXP, rs.Field(L"CUR_EXP").AsInt());
			CHECK_EQUAL(m_nDeltaXP, rs.Field(L"DELTA_EXP").AsInt());
			CHECK(m_strRegDate == wstring(L"'") + rs.Field(L"REG_DATE").AsWString() + wstring(L"'"));
			rs.Close();
		}
	};

	LOGDB_TEST(USP_RZ_XP_LOG)
	{
		MOCK_LOG_DATA_XP m_Log[GXPLogCache::MAX_LOG_COUNT];

		for (int i = 1; i <= GXPLogCache::MAX_LOG_COUNT; ++i)
			m_Log[i - 1] = MOCK_LOG_DATA_XP(i+1, i+2, i+3, i+4, i+5, i+6, i+7, i+8, GTimeCalculator::GetTimeAsString(GTimeSec(i)), i+9);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_XP_LOG (%d\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s)}"
						 , WORLD
						 , m_Log[0].m_nAID, m_Log[0].m_nCID, m_Log[0].m_nCode, m_Log[0].m_strRegDate.c_str(), m_Log[0].m_nCharPtm, m_Log[0].m_nDeltaCharPtm, m_Log[0].m_nLevel, m_Log[0].m_nCurXP, m_Log[0].m_nDeltaXP, m_Log[0].m_strNpcID
						 , m_Log[1].m_nAID, m_Log[1].m_nCID, m_Log[1].m_nCode, m_Log[1].m_strRegDate.c_str(), m_Log[1].m_nCharPtm, m_Log[1].m_nDeltaCharPtm, m_Log[1].m_nLevel, m_Log[1].m_nCurXP, m_Log[1].m_nDeltaXP, m_Log[1].m_strNpcID
						 , m_Log[2].m_nAID, m_Log[2].m_nCID, m_Log[2].m_nCode, m_Log[2].m_strRegDate.c_str(), m_Log[2].m_nCharPtm, m_Log[2].m_nDeltaCharPtm, m_Log[2].m_nLevel, m_Log[2].m_nCurXP, m_Log[2].m_nDeltaXP, m_Log[2].m_strNpcID
						 , m_Log[3].m_nAID, m_Log[3].m_nCID, m_Log[3].m_nCode, m_Log[3].m_strRegDate.c_str(), m_Log[3].m_nCharPtm, m_Log[3].m_nDeltaCharPtm, m_Log[3].m_nLevel, m_Log[3].m_nCurXP, m_Log[3].m_nDeltaXP, m_Log[3].m_strNpcID
						 , m_Log[4].m_nAID, m_Log[4].m_nCID, m_Log[4].m_nCode, m_Log[4].m_strRegDate.c_str(), m_Log[4].m_nCharPtm, m_Log[4].m_nDeltaCharPtm, m_Log[4].m_nLevel, m_Log[4].m_nCurXP, m_Log[4].m_nDeltaXP, m_Log[4].m_strNpcID);

		for (int i = 0; i < GXPLogCache::MAX_LOG_COUNT; ++i)
			m_Log[i].Check();
	}
}