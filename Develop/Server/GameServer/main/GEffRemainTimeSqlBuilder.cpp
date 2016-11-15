#include "stdafx.h"
#include "GEffRemainTimeSqlBuilder.h"
#include "GConfig.h"


bool GEffRemainTimeSqlBuilder::Set(const int idx, const int ORD_NUM, const int EFF_TYPE, const int ID, const int EXPI_CHAR_PTM)
{
	if (0 > idx || MAX_EFF_REMAIN_COUNT <= idx)
		return false;

	m_EffRemainTime[idx].Set(ORD_NUM, EFF_TYPE, ID, EXPI_CHAR_PTM);

	return true;
}

CString& GEffRemainTimeSqlBuilder::BuildSQL(const int64 GSN, const int64 CSN)
{
	m_strSQL.ReleaseBuffer();

	m_strSQL.Format(L"{CALL dbo.USP_RZ_EFF_REMAIN_TIME_INSERT (%d, %I64d, %I64d\
					 , %d, %d, %d, %d\
					 , %d, %d, %d, %d\
					 , %d, %d, %d, %d\
					 , %d, %d, %d, %d\
					 , %d, %d, %d, %d\
					 , %d, %d, %d, %d\
					 , %d, %d, %d, %d\
					 , %d, %d, %d, %d\
					 , %d, %d, %d, %d\
					 , %d, %d, %d, %d)}"
					 , GConfig::m_nMyWorldID
					 , GSN, CSN
					 , m_EffRemainTime[0].m_ORD_NUM, m_EffRemainTime[0].m_EFF_TYPE, m_EffRemainTime[0].m_ID, m_EffRemainTime[0].m_EXPI_CHAR_PTM
					 , m_EffRemainTime[1].m_ORD_NUM, m_EffRemainTime[1].m_EFF_TYPE, m_EffRemainTime[1].m_ID, m_EffRemainTime[1].m_EXPI_CHAR_PTM
					 , m_EffRemainTime[2].m_ORD_NUM, m_EffRemainTime[2].m_EFF_TYPE, m_EffRemainTime[2].m_ID, m_EffRemainTime[2].m_EXPI_CHAR_PTM
					 , m_EffRemainTime[3].m_ORD_NUM, m_EffRemainTime[3].m_EFF_TYPE, m_EffRemainTime[3].m_ID, m_EffRemainTime[3].m_EXPI_CHAR_PTM
					 , m_EffRemainTime[4].m_ORD_NUM, m_EffRemainTime[4].m_EFF_TYPE, m_EffRemainTime[4].m_ID, m_EffRemainTime[4].m_EXPI_CHAR_PTM
					 , m_EffRemainTime[5].m_ORD_NUM, m_EffRemainTime[5].m_EFF_TYPE, m_EffRemainTime[5].m_ID, m_EffRemainTime[5].m_EXPI_CHAR_PTM
					 , m_EffRemainTime[6].m_ORD_NUM, m_EffRemainTime[6].m_EFF_TYPE, m_EffRemainTime[6].m_ID, m_EffRemainTime[6].m_EXPI_CHAR_PTM
					 , m_EffRemainTime[7].m_ORD_NUM, m_EffRemainTime[7].m_EFF_TYPE, m_EffRemainTime[7].m_ID, m_EffRemainTime[7].m_EXPI_CHAR_PTM
					 , m_EffRemainTime[8].m_ORD_NUM, m_EffRemainTime[8].m_EFF_TYPE, m_EffRemainTime[8].m_ID, m_EffRemainTime[8].m_EXPI_CHAR_PTM
					 , m_EffRemainTime[9].m_ORD_NUM, m_EffRemainTime[9].m_EFF_TYPE, m_EffRemainTime[9].m_ID, m_EffRemainTime[9].m_EXPI_CHAR_PTM);

	return m_strSQL;
}

void GEffRemainTimeSqlBuilder::Reset()
{
	for (int i = 0; i < MAX_EFF_REMAIN_COUNT; ++i)
	{
		m_EffRemainTime[i].m_ORD_NUM		= 0;
		m_EffRemainTime[i].m_EFF_TYPE		= 0;
		m_EffRemainTime[i].m_ID				= 0;
		m_EffRemainTime[i].m_EXPI_CHAR_PTM	= 0;
	}
}