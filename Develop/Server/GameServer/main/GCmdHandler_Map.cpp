#include "StdAfx.h"
#include "GCmdHandler_Map.h"
#include "CCommandTable.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GPartySystem.h"

GCmdHandler_Map::GCmdHandler_Map(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_MINIMAP_MARK_POS_REQ,		OnRequestMiniMapMarkPos);	
}

MCommandResult GCmdHandler_Map::OnRequestMiniMapMarkPos(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	vec3 vPos;
	if (pCmd->GetParameter(&vPos,	0, MPT_VEC)==false) return CR_FALSE;

	if (!gsys.pPartySystem->IsPartyMember(pPlayer)) return CR_TRUE;

	MUID uidParty = pPlayer->GetPartyUID();
	GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
	_ASSERT(pParty != NULL);

	for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
	{
		MUID uidMember = it->first;

		GEntityPlayer* pMember = pPlayer->FindPlayer(uidMember);
		if (NULL == pMember) continue;

		MCommand* pNewCmd = MakeNewCommand(MC_MINIMAP_MARK_POS,
			1, 
			NEW_VEC(vPos));

		pMember->RouteToMe(pNewCmd);
	}

	return CR_TRUE;
}