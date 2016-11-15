#pragma once

#include "PLog.h"

enum LOG_QUEST_TYPE
{
	LQT_NONE = 0,

	LQT_ACQUIRE,
	LQT_DONE,
	LQT_CANCEL,

	LQT_MAX
};

class PLog_Quest : public PLog
{
private:
	int				m_nCID;
	LOG_QUEST_TYPE	m_nType;
	int				m_nQID;

	static wstring	m_strQueryForm;

public:
	PLog_Quest(const wstring& strDate, int nCID, LOG_QUEST_TYPE nType, int nQID);
	virtual ~PLog_Quest();

	virtual wstring MakeQuery();
};

