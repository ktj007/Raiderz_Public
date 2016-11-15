#pragma once

#include "GAIEvent.h"

class GAIEventHandler;

class GAIEventNotifier : public MTestMemPool<GAIEventNotifier>
{
public:
	GAIEventNotifier();
	virtual ~GAIEventNotifier();

	virtual void Notify(GAIEvent aiEvent);
	void AddEventHandler(MUID uidNPC, GAIEventHandler* pHandler);
	void RemoveEventHandler(MUID uidNPC, GAIEventHandler* pHandler);
	
	void NotifyField(GAIEvent aiEvent);
	void NotifyNeighborSector(GAIEvent aiEvent);
	void NotifyNPC(GAIEvent aiEvent);
	void NotifyAll(GAIEvent aiEvent);
	
	void NotifyToUID(MUID uidNPC, GAIEvent aiEvent);

private:
	map<MUID, GAIEventHandler*> m_mapNPCHandler;
};
