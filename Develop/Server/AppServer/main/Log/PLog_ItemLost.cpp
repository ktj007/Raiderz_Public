#include "StdAfx.h"
#include "PLog_ItemLost.h"

wstring PLog_ItemLost::m_strQueryForm = L"{CALL spInsertLog_ItemLost ('%s', %d, %d, %I64d, %d, %d)}";

PLog_ItemLost::PLog_ItemLost(const wstring& strDate, int nCID, LOG_ITEM_LOST_TYPE nType, const int64& nIUID, int nQuantity, int nGold )
: PLog(strDate)
, m_nCID(nCID)
, m_nType(nType)
, m_nIUID(nIUID)
, m_nQuantity(nQuantity)
, m_nGold(nGold)
{

}

PLog_ItemLost::~PLog_ItemLost(void)
{
}

wstring PLog_ItemLost::MakeQuery()
{
	CString strSQL;
	strSQL.Format(m_strQueryForm.c_str(), m_strDate, m_nCID, m_nType, m_nIUID, m_nQuantity, m_nGold);

	return strSQL.GetString();
}