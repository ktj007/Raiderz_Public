#include "StdAfx.h"
#include "PLog_Char.h"

wstring PLog_Char::m_strQueryForm = L"{CALL spInsertLog_Char ('%s', %d, %d, '%s')}";

PLog_Char::PLog_Char(const wstring& strDate, int nCID, LOG_CHAR_TYPE nType, const wstring& strIP)
: PLog(strDate)
, m_nCID(nCID)
, m_nType(nType)
, m_strIP(strIP)
{
}

PLog_Char::~PLog_Char()
{
}

wstring PLog_Char::MakeQuery()
{
	CString strSQL;
	strSQL.Format(m_strQueryForm.c_str(), m_strDate, m_nCID, m_nType, m_strIP);

	return strSQL.GetString();
}