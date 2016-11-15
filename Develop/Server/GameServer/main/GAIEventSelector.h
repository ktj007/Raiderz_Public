#pragma once

#include "GAIEventHandler.h"

class GAIEventSelector: public GAIEventHandler
{
public:
	virtual bool IsSelectEvent(GAIEvent aiEvent) = 0;

	void OnNotify(GAIEvent aiEvent);
	void RegistHandler(GAIEventHandler* pHandler);
	void UnregistHandler(void);

private:
	GAIEventHandler* m_pHandler;
};
