#ifndef _GCOMMAND_LOGGER_H
#define _GCOMMAND_LOGGER_H

#include <set>
#include "GNetServer.h"
#include "SCommandLogger.h"

class ICommandLogListener
{
public:
	virtual void OnLog(const wchar_t* szLog) {}
};

class GCommandLogger : public SCommandLogger
{
private:
	MLogger					m_FileLogger;
	ICommandLogListener*	m_pListener;
public:
	GCommandLogger(ICommandLogListener* pListener);
	virtual ~GCommandLogger();

	void Log(MCommand* pCmd) override;

	void IgnoreDebugCommand();
};

#endif