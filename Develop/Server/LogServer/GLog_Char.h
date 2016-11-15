#ifndef _G_CHAR_LOG_H_
#define _G_CHAR_LOG_H_

#include "GLog.h"

enum LOG_CHAR_TYPE
{
	CLT_NONE = 0,

	CLT_CREATE,
	CLT_DELETE,
	CLT_CONNECT,
	CLT_DISCONNECT,

	CLT_MAX
};

class GLog_Char : public GLog
{
private:
	int				m_nCID;
	LOG_CHAR_TYPE	m_nType;
	string			m_strIP;	

	static string	m_strQueryForm;

public:
	GLog_Char(const string& strDate, int nCID, LOG_CHAR_TYPE nType, const string& strIP);
	virtual ~GLog_Char();

	virtual string MakeQuery();
};

#endif
