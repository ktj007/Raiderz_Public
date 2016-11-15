#include "StdAfx.h"
#include "PLog_ItemAcquire.h"

wstring PLog_ItemAcquire::m_strQueryForm = L"{CALL spInsertLog_ItemAcquire ('%s', %d, %d, %I64d, %d, %d)}";

PLog_ItemAcquire::PLog_ItemAcquire(const wstring& strDate, int nCID, LOG_ITEM_ACQUIRE_TYPE nType, const int64& nIUID, int nQuantity, int nGold)
: PLog(strDate)
, m_nCID(nCID)
, m_nType(nType)
, m_nIUID(nIUID)
, m_nQuantity(nQuantity)
, m_nGold(nGold)
{

}

PLog_ItemAcquire::~PLog_ItemAcquire()
{
}

wstring PLog_ItemAcquire::MakeQuery()
{
	CString strSQL;
	strSQL.Format(m_strQueryForm.c_str(), m_strDate, m_nCID, m_nType, m_nIUID, m_nQuantity, m_nGold);

	return strSQL.GetString();
}