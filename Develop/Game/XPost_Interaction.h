#ifndef _XPOST_INTERACTION_H
#define _XPOST_INTERACTION_H

#include "XNetClient.h"
#include "XCommandTable.h"

enum CONDITION_TYPE;

inline void XPostInteraction_Interaction(const MUID& uidTarget, vec3 vTalkerPos, vec3 vTalkerDir)
{
	XPOSTCMD3(MC_INTERACTION_INTERACTION_REQ, MCommandParameterUID(uidTarget), MCommandParameterVector(vTalkerPos), MCommandParameterVector(vTalkerDir));
}

inline void XPostNPCInteraction_IE(const MUID& uidItem, int nID)
{
	XPOSTCMD2(MC_NPCINTERACTION_IELEMENT_REQ, MCommandParameterUID(uidItem), MCommandParameterInt(nID));
}

inline void XPostNPCInteraction_End()
{
	XPOSTCMD0(MC_NPCINTERACTION_END_REQ);
}

#endif//_XPOST_INTERACTION_H