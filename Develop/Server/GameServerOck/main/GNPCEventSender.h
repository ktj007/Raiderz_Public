#pragma once

#include "GMessage.h"

class GModuleAI;
class GEntityNPC;

class GNPCEventSender
{
private:
	GEntityNPC* m_pOwner;

	bool CheckFaction(GEntityNPC* pSender, GEntityNPC* pTarget);
	bool CheckRange(GEntityNPC* pSender, GEntityNPC* pTarget, int nRange);

	void MakeTargetNPCList_FriendlyFaction(vector<GEntityNPC*>& refVecTargetList, int nRange);

public:
	GNPCEventSender(GEntityNPC* pOwner);
	~GNPCEventSender();

	void SendEvent(GMessage& refMessage, int nRange);
};
