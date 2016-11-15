#pragma once

#include <set>
#include "MCommand.h"
#include "ServerCommonLib.h"


const int MAX_IGNORE_COMMAND_ID = 10000;

class SCOMMON_API SCommandLogger
{
protected:
	std::set<int>		m_IgnoredCmdSet;

public:
	SCommandLogger() {}
	virtual ~SCommandLogger() {}
	void IgnoreCommand(int nID, bool bIgnore=true);
	void IgnoreCommandAll(bool bIgnore=true, int nMaxIgnoreID=MAX_IGNORE_COMMAND_ID);

	virtual void Log(minet::MCommand* pCmd);
};

