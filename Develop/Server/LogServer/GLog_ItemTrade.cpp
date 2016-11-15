#include "StdAfx.h"
#include "GLog_ItemTrade.h"
#include "CTransData_Log.h"

GLog_ItemTrade::GLog_ItemTrade(const string& strDate,	int nCID1, const vector<TDL_LOG_ITEM>& vecTDItem1, int nGold1, 
														int nCID2, const vector<TDL_LOG_ITEM>& vecTDItem2, int nGold2)
: GLog(strDate)
, m_nCID1(nCID1), m_vecTDItem1(vecTDItem1), m_nGold1(nGold1)
, m_nCID2(nCID2), m_vecTDItem2(vecTDItem2), m_nGold2(nGold2)
{

}

GLog_ItemTrade::~GLog_ItemTrade()
{

}

string GLog_ItemTrade::MakeQuery()
{
	size_t nTDItemSize1 = m_vecTDItem1.size();
	size_t nTDItemSize2 = m_vecTDItem2.size();

	_ASSERT(MAX_TRADE_ITEM_SPACE < nTDItemSize1 || MAX_TRADE_ITEM_SPACE < nTDItemSize2);

	CString strSQL;
	strSQL.Format("{CALL spInsertLog_ItemTrade (%d", m_nCID1);

	for(size_t i = 0; i < nTDItemSize1; ++i)
	{
		strSQL.AppendFormat(", %I64d, %d", m_vecTDItem1[i].nIUID, m_vecTDItem1[i].nQuantity);
	}
	for(size_t i = 0; i < MAX_TRADE_ITEM_SPACE - nTDItemSize1; ++i)
	{
		strSQL.AppendFormat(", %I64d, %d", (INT64)0, 0);
	}

	strSQL.AppendFormat(", %d ", m_nGold1);
	strSQL.AppendFormat(", %d", m_nCID2);

	for(size_t i = 0; i < nTDItemSize2; ++i)
	{
		strSQL.AppendFormat(", %I64d, %d", (int64) m_vecTDItem2[i].nIUID, m_vecTDItem2[i].nQuantity);
	}
	for(size_t i = 0; i < MAX_TRADE_ITEM_SPACE - nTDItemSize2; ++i)
	{
		strSQL.AppendFormat(", %I64d, %d", (INT64)0, 0);
	}

	strSQL.AppendFormat(", %d)}", m_nGold2);

	return LPSTR(LPCTSTR(strSQL));
}
