#ifndef _G_LOG_H_
#define _G_LOG_H_

class GLog
{
protected:
	string	m_strDate;

public:
	GLog(const string& strDate);
	virtual ~GLog();

	virtual string MakeQuery() = 0;
};

typedef vector<GLog*> VEC_LOG;

#endif
