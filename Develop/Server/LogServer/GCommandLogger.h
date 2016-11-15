#ifndef _GCOMMAND_LOGGER_H
#define _GCOMMAND_LOGGER_H

#include <set>
#include "GNetServer.h"

class GCommandLogger
{
private:
	std::set<int>		m_IgnoredCmdSet;
public:
	GCommandLogger() {}
	void IgnoreCommand(int nID, bool bIgnore=true);
	void IgnoreCommandAll(bool bIgnore=true);
	void Log(MCommand* pCmd);
};

#endif