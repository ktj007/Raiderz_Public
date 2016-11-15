#ifndef _XPOST_QUEST_H
#define _XPOST_QUEST_H

#include "XNetClient.h"
#include "XCommandTable.h"

inline void XPostQuest_RequestGive(int nQuestID)
{
	XPOSTCMD1(MC_QUEST_GIVE_REQ, 	
		MCommandParameterInt(nQuestID));
}

inline void XPostQuest_RequestReward(const MUID& uidNPC, int nQuestID, int nSelectedReward)
{
	XPOSTCMD3(MC_QUEST_REWARD_REQ, 
		MCommandParameterUID(uidNPC), 
		MCommandParameterInt(nQuestID), 
		MCommandParameterInt(nSelectedReward));
}

inline void XPostQuest_RequestCancel(int nQuestID)
{
	XPOSTCMD1(MC_QUEST_CANCEL_REQ, MCommandParameterInt(nQuestID));
}

inline void XPostQuest_RequestShare(int nQuestID)
{
	XPOSTCMD1(MC_QUEST_SHARE_REQ, MCommandParameterInt(nQuestID));
}

inline void XPostQuest_RequestShareGive(int nQuestID)
{
	XPOSTCMD1(MC_QUEST_SHARE_GIVE_REQ, MCommandParameterInt(nQuestID));
}


inline void XPostQuest_RequestInteractCancel()
{
	XPOSTCMD0(MC_QUEST_INTERACT_QOBJECTIVE_CANCEL_REQ);
}

#endif//_XPOST_QUEST_H

