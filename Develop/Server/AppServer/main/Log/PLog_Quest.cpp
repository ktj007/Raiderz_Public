#include "StdAfx.h"
#include "PLog_Quest.h"

wstring PLog_Quest::m_strQueryForm = L"{CALL spInsertLog_Quest ('%s', %d, %d, %d)}";

PLog_Quest::PLog_Quest(const wstring& strDate, int nCID, LOG_QUEST_TYPE nType, int nQID)
: PLog(strDate)
, m_nCID(nCID)
, m_nType(nType)
, m_nQID(nQID)
{
}

PLog_Quest::~PLog_Quest()
{
}

wstring PLog_Quest::MakeQuery()
{
	CString strSQL;
	strSQL.Format(m_strQueryForm.c_str(), m_strDate, m_nCID, m_nType, m_nQID);

	return strSQL.GetString();
}
