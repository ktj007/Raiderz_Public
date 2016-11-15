#ifndef _G_LOG_QUEST_H_
#define _G_LOG_QUEST_H_

#include "GLog.h"

enum LOG_QUEST_TYPE
{
	LQT_NONE = 0,

	LQT_ACQUIRE,
	LQT_DONE,
	LQT_CANCEL,

	LQT_MAX
};

class GLog_Quest : public GLog
{
private:
	int				m_nCID;
	LOG_QUEST_TYPE	m_nType;
	int				m_nQID;

	static string	m_strQueryForm;

public:
	GLog_Quest(const string& strDate, int nCID, LOG_QUEST_TYPE nType, int nQID);
	virtual ~GLog_Quest();

	virtual string MakeQuery();
};

#endif