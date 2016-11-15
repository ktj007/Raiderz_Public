#include "StdAfx.h"
#include "PLog_ItemTrade.h"
#include "CTransData_Log.h"

PLog_ItemTrade::PLog_ItemTrade(const wstring& strDate,	int nCID1, const vector<TDL_LOG_ITEM>& vecTDItem1, int nGold1, 
														int nCID2, const vector<TDL_LOG_ITEM>& vecTDItem2, int nGold2)
: PLog(strDate)
, m_nCID1(nCID1), m_vecTDItem1(vecTDItem1), m_nGold1(nGold1)
, m_nCID2(nCID2), m_vecTDItem2(vecTDItem2), m_nGold2(nGold2)
{

}

PLog_ItemTrade::~PLog_ItemTrade()
{

}

wstring PLog_ItemTrade::MakeQuery()
{
	size_t nTDItemSize1 = m_vecTDItem1.size();
	size_t nTDItemSize2 = m_vecTDItem2.size();

	_ASSERT(MAX_TRADE_ITEM_SPACE < nTDItemSize1 || MAX_TRADE_ITEM_SPACE < nTDItemSize2);

	CString strSQL;
	strSQL.Format(L"{CALL spInsertLog_ItemTrade (%d", m_nCID1);

	for(size_t i = 0; i < nTDItemSize1; ++i)
	{
		strSQL.AppendFormat(L", %I64d, %d", m_vecTDItem1[i].nIUID, m_vecTDItem1[i].nQuantity);
	}
	for(size_t i = 0; i < MAX_TRADE_ITEM_SPACE - nTDItemSize1; ++i)
	{
		strSQL.AppendFormat(L", %I64d, %d", (INT64)0, 0);
	}

	strSQL.AppendFormat(L", %d ", m_nGold1);
	strSQL.AppendFormat(L", %d", m_nCID2);

	for(size_t i = 0; i < nTDItemSize2; ++i)
	{
		strSQL.AppendFormat(L", %I64d, %d", (int64) m_vecTDItem2[i].nIUID, m_vecTDItem2[i].nQuantity);
	}
	for(size_t i = 0; i < MAX_TRADE_ITEM_SPACE - nTDItemSize2; ++i)
	{
		strSQL.AppendFormat(L", %I64d, %d", (INT64)0, 0);
	}

	strSQL.AppendFormat(L", %d)}", m_nGold2);

	return strSQL.GetString();
}
