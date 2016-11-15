#include "StdAfx.h"
#include "GLog_Quest.h"

string GLog_Quest::m_strQueryForm = "{CALL spInsertLog_Quest ('%s', %d, %d, %d)}";

GLog_Quest::GLog_Quest(const string& strDate, int nCID, LOG_QUEST_TYPE nType, int nQID)
: GLog(strDate)
, m_nCID(nCID)
, m_nType(nType)
, m_nQID(nQID)
{
}

GLog_Quest::~GLog_Quest()
{
}

string GLog_Quest::MakeQuery()
{
	CString strSQL;
	strSQL.Format(m_strQueryForm.c_str(), m_strDate, m_nCID, m_nType, m_nQID);

	return LPSTR(LPCTSTR(strSQL));
}
