#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GLogCacheData.h"
#include "GConfig.h"
#include "GPartyLogCache.h"
#include "GTimeCalculator.h"

SUITE(DBTEST_PARTY_LOG)
{
	class MOCK_LOG_DATA_PARTY : public GLOG_DATA_PARTY
	{
	public :
		MOCK_LOG_DATA_PARTY() : GLOG_DATA_PARTY() {}
		MOCK_LOG_DATA_PARTY(const MUID& uidParty, int64 nAID, int64 nCID, int nCode, const wstring& strRegDate, uint8 nType)
			: GLOG_DATA_PARTY(uidParty, nAID, nCID, nCode, strRegDate, nType) {}

		void Check()
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT PARTY_UID, REG_DATE, [TYPE] FROM dbo.RZ_PARTY_LOG WHERE WORLD_ID = %d AND CODE = %d AND ACCN_ID = %I64d AND CHAR_ID = %I64d;"
				, GConfig::m_nMyWorldID, m_nCode, m_nAID, m_nCID);
			CHECK_EQUAL(m_uidParty, rs.Field(L"PARTY_UID").AsInt64());
			CHECK(m_strRegDate == wstring(L"'") + rs.Field(L"REG_DATE").AsWString() + wstring(L"'"));
			CHECK_EQUAL(m_nType, rs.Field(L"TYPE").AsByte());
			rs.Close();
		}
	};

	LOGDB_TEST(USP_RZ_PARTY_LOG)
	{
		MOCK_LOG_DATA_PARTY m_Log[GPartyLogCache::MAX_LOG_COUNT];
		for (int i = 0; i < GPartyLogCache::MAX_LOG_COUNT; ++i)
			m_Log[i] = MOCK_LOG_DATA_PARTY(i+1, i+2, i+3, i+4, GTimeCalculator::GetTimeAsString(GTimeSec(i)), i+5);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_PARTY_LOG (%d\
						 , %I64d, %d, %s, %d, %I64d, %I64d\
						 , %I64d, %d, %s, %d, %I64d, %I64d\
						 , %I64d, %d, %s, %d, %I64d, %I64d\
						 , %I64d, %d, %s, %d, %I64d, %I64d\
						 , %I64d, %d, %s, %d, %I64d, %I64d)}"
						 , GConfig::m_nMyWorldID
						 , m_Log[0].m_uidParty, m_Log[0].m_nCode, m_Log[0].m_strRegDate.c_str(), m_Log[0].m_nType, m_Log[0].m_nAID, m_Log[0].m_nCID
						 , m_Log[1].m_uidParty, m_Log[1].m_nCode, m_Log[1].m_strRegDate.c_str(), m_Log[1].m_nType, m_Log[1].m_nAID, m_Log[1].m_nCID
						 , m_Log[2].m_uidParty, m_Log[2].m_nCode, m_Log[2].m_strRegDate.c_str(), m_Log[2].m_nType, m_Log[2].m_nAID, m_Log[2].m_nCID
						 , m_Log[3].m_uidParty, m_Log[3].m_nCode, m_Log[3].m_strRegDate.c_str(), m_Log[3].m_nType, m_Log[3].m_nAID, m_Log[3].m_nCID
						 , m_Log[4].m_uidParty, m_Log[4].m_nCode, m_Log[4].m_strRegDate.c_str(), m_Log[4].m_nType, m_Log[4].m_nAID, m_Log[4].m_nCID);

		for (int i = 0; i < GPartyLogCache::MAX_LOG_COUNT; ++i)
			m_Log[i].Check();
	}	
}