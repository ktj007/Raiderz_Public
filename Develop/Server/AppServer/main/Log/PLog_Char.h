#ifndef _G_CHAR_LOG_H_
#define _G_CHAR_LOG_H_

#include "PLog.h"

enum LOG_CHAR_TYPE
{
	CLT_NONE = 0,

	CLT_CREATE,
	CLT_DELETE,
	CLT_CONNECT,
	CLT_DISCONNECT,

	CLT_MAX
};

class PLog_Char : public PLog
{
private:
	int				m_nCID;
	LOG_CHAR_TYPE	m_nType;
	wstring			m_strIP;	

	static wstring	m_strQueryForm;

public:
	PLog_Char(const wstring& strDate, int nCID, LOG_CHAR_TYPE nType, const wstring& strIP);
	virtual ~PLog_Char();

	virtual wstring MakeQuery();
};

#endif
