#include "StdAfx.h"
#include "GLog_Char.h"

string GLog_Char::m_strQueryForm = "{CALL spInsertLog_Char ('%s', %d, %d, '%s')}";

GLog_Char::GLog_Char(const string& strDate, int nCID, LOG_CHAR_TYPE nType, const string& strIP)
: GLog(strDate)
, m_nCID(nCID)
, m_nType(nType)
, m_strIP(strIP)
{
}

GLog_Char::~GLog_Char()
{
}

string GLog_Char::MakeQuery()
{
	CString strSQL;
	strSQL.Format(m_strQueryForm.c_str(), m_strDate, m_nCID, m_nType, m_strIP);

	return LPSTR(LPCTSTR(strSQL));
}