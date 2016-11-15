#include "StdAfx.h"
#include "GLog_ItemAcquire.h"

string GLog_ItemAcquire::m_strQueryForm = "{CALL spInsertLog_ItemAcquire ('%s', %d, %d, %I64d, %d, %d)}";

GLog_ItemAcquire::GLog_ItemAcquire(const string& strDate, int nCID, LOG_ITEM_ACQUIRE_TYPE nType, const int64& nIUID, int nQuantity, int nGold)
: GLog(strDate)
, m_nCID(nCID)
, m_nType(nType)
, m_nIUID(nIUID)
, m_nQuantity(nQuantity)
, m_nGold(nGold)
{

}

GLog_ItemAcquire::~GLog_ItemAcquire()
{
}

string GLog_ItemAcquire::MakeQuery()
{
	CString strSQL;
	strSQL.Format(m_strQueryForm.c_str(), m_strDate, m_nCID, m_nType, m_nIUID, m_nQuantity, m_nGold);

	return LPSTR(LPCTSTR(strSQL));
}