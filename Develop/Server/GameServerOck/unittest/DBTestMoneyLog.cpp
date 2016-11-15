#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GTimeCalculator.h"
#include "GMoneyLogCache.h"

class DT_MONEY_LOG
{
public :
	DT_MONEY_LOG() {}

	void Set(int i )
	{
		m_GSN				= i + 1;
		m_CSN				= i + 2;
		m_CODE				= i + 3;
		m_REG_DATE			= GTimeCalculator::GetTimeAsString(GTimeSec(i));
		m_CHAR_PTM			= i + 4;
		m_DELTA_CHAR_PTM	= i + 5;
		m_LEV				= i + 6;
		m_CUR_MONEY			= i + 7;
		m_DELTA_MONEY		= i + 8;	

		m_strNpcID.Format(L"%d", i + 9);
	}

	void CHECK_MONEY_LOOT()
	{
		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_MONEY, DELTA_MONEY, NPC_ID FROM dbo.RZ_ITEM_LOG WHERE ACCN_ID = %I64d AND OWNER_ID = %I64d AND CODE = %d AND REG_DATE = %s"
			, m_GSN, m_CSN, m_CODE, m_REG_DATE.c_str());
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(m_CHAR_PTM, rs.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(m_DELTA_CHAR_PTM, rs.Field(L"DELTA_CHAR_PTM").AsInt());
		CHECK_EQUAL(m_LEV, rs.Field(L"LEV").AsInt());
		CHECK_EQUAL(m_CUR_MONEY, rs.Field(L"CUR_MONEY").AsInt());
		CHECK_EQUAL(m_DELTA_MONEY, rs.Field(L"DELTA_MONEY").AsInt());
		CHECK_EQUAL(m_strNpcID, rs.Field(L"NPC_ID").AsWString().c_str());
		rs.Close();
	}

	int64	m_GSN;
	int64	m_CSN;
	int		m_CODE;
	wstring	m_REG_DATE;
	int		m_CHAR_PTM;
	int		m_DELTA_CHAR_PTM;
	int		m_LEV;
	int		m_CUR_MONEY;
	int		m_DELTA_MONEY;
	CString	m_strNpcID;
};

SUITE(USP_RZ_MONEY_LOG)
{
	const int nWORLD = 1;

	LOGDB_TEST(USP_RZ_MONEY_LOG)
	{
		DT_MONEY_LOG log[GMoneyLogCache::MAX_LOG_COUNT];
		for (int i = 0; i < GMoneyLogCache::MAX_LOG_COUNT; ++i)
			log[i].Set(i);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_MONEY_LOG (%d\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s\
						 , %I64d, %I64d, %d, %s, %d, %d, %d, %d, %d, %s)}"
						 , nWORLD
						 , log[0].m_GSN, log[0].m_CSN, log[0].m_CODE, log[0].m_REG_DATE.c_str(), log[0].m_CHAR_PTM, log[0].m_DELTA_CHAR_PTM, log[0].m_LEV, log[0].m_CUR_MONEY, log[0].m_DELTA_MONEY, log[0].m_strNpcID
						 , log[1].m_GSN, log[1].m_CSN, log[1].m_CODE, log[1].m_REG_DATE.c_str(), log[1].m_CHAR_PTM, log[1].m_DELTA_CHAR_PTM, log[1].m_LEV, log[1].m_CUR_MONEY, log[1].m_DELTA_MONEY, log[1].m_strNpcID
						 , log[2].m_GSN, log[2].m_CSN, log[2].m_CODE, log[2].m_REG_DATE.c_str(), log[2].m_CHAR_PTM, log[2].m_DELTA_CHAR_PTM, log[2].m_LEV, log[2].m_CUR_MONEY, log[2].m_DELTA_MONEY, log[2].m_strNpcID
						 , log[3].m_GSN, log[3].m_CSN, log[3].m_CODE, log[3].m_REG_DATE.c_str(), log[3].m_CHAR_PTM, log[3].m_DELTA_CHAR_PTM, log[3].m_LEV, log[3].m_CUR_MONEY, log[3].m_DELTA_MONEY, log[3].m_strNpcID
						 , log[4].m_GSN, log[4].m_CSN, log[4].m_CODE, log[4].m_REG_DATE.c_str(), log[4].m_CHAR_PTM, log[4].m_DELTA_CHAR_PTM, log[4].m_LEV, log[4].m_CUR_MONEY, log[4].m_DELTA_MONEY, log[4].m_strNpcID);

		for (int i = 0; i < GMoneyLogCache::MAX_LOG_COUNT; ++i)
			log[i].CHECK_MONEY_LOOT();
	}	
}