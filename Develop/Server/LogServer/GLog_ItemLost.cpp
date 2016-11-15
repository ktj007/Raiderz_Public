#include "StdAfx.h"
#include "GLog_ItemLost.h"

string GLog_ItemLost::m_strQueryForm = "{CALL spInsertLog_ItemLost ('%s', %d, %d, %I64d, %d, %d)}";

GLog_ItemLost::GLog_ItemLost(const string& strDate, int nCID, LOG_ITEM_LOST_TYPE nType, const int64& nIUID, int nQuantity, int nGold )
: GLog(strDate)
, m_nCID(nCID)
, m_nType(nType)
, m_nIUID(nIUID)
, m_nQuantity(nQuantity)
, m_nGold(nGold)
{

}

GLog_ItemLost::~GLog_ItemLost(void)
{
}

string GLog_ItemLost::MakeQuery()
{
	CString strSQL;
	strSQL.Format(m_strQueryForm.c_str(), m_strDate, m_nCID, m_nType, m_nIUID, m_nQuantity, m_nGold);

	return LPSTR(LPCTSTR(strSQL));
}