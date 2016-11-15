#pragma once

#include "GAIEvent.h"

class GAIEventHandler;
class GAIEventNotifier;

class GAIEventMgr : public MTestMemPool<GAIEventMgr>
{
public:
	GAIEventMgr();
	virtual ~GAIEventMgr();

	void Notify(GAIEvent aiEvent);
	void AddEventHandler(MUID uidNPC, GAIEventHandler* pHandler);
	void RemoveEventHandler(MUID uidNPC, GAIEventHandler* pHandler);

private:	
	GAIEventNotifier* m_pEventNotifier;
};
