#pragma once

class GNPCInteractionElementInfo;
class GEntityNPC;

enum INTERACTION_TYPE;

class GUTHelper_Interaction
{
public:
	static GNPCInteractionElementInfo* GiveNewIElementInfoForClick(GNPCInfo* pNPCInfo, INTERACTION_TYPE nIET = static_cast<INTERACTION_TYPE>(0), int nAct=0);
	static GNPCInteractionElementInfo* GiveNewIElementInfoForItem(GNPCInfo* pNPCInfo, int nItemID, INTERACTION_TYPE nIET = static_cast<INTERACTION_TYPE>(0), int nAct=0);

	static void MakeInteractionState(GEntityPlayer* pPlayer, GEntityNPC* pNPC, GNPCInteractionElementInfo* pIElementInfo);
	static void SetUninteractableDistacne(GEntityActor* pActor1, GEntityActor* pActor2);	
};
