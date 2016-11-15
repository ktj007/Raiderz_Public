#include "StdAfx.h"
#include "GCmdHandler_Char.h"
#include "GGlobal.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GRebirthSystem.h"
#include "GPlayerTargetInfoSender.h"
#include "CCommandTable.h"
#include "CSDef_Rebirth.h"
#include "SConfig.h"

GCmdHandler_Char::GCmdHandler_Char(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_CHAR_REBIRTH_REQ,				OnRequestRebirth);
	SetCmdHandler(MC_CHAR_TARGET_INFO_REQ,			OnRequestTargetInfo);	
	SetCmdHandler(MC_CHAR_DIE_REQ,					OnRequestDie);
}

MCommandResult GCmdHandler_Char::OnRequestRebirth(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	REBIRTH_TYPE nRebirthType;
	if (pCmd->GetParameter(&nRebirthType, 0, MPT_INT)==false) return CR_FALSE;

	if (SConfig::IsExpoMode())
	{
		gsys.pRebirthSystem->RebirthToPlace(pPlayer, pPlayer, 1.0f);
		return CR_TRUE;
	}

	switch (nRebirthType)
	{
	case REBT_PLACE_IN_BATTLEGROUD	: gsys.pRebirthSystem->RebirthToPlaceInBattleArena(pPlayer);break;
	case REBT_SOULBINDING			: gsys.pRebirthSystem->RebirthToSoulBinding(pPlayer);		break;
	case REBT_NEAR_SOULBINDING		: gsys.pRebirthSystem->RebirthToNearSoulBinding(pPlayer);	break;
	case REBT_CHECKPOINT			: gsys.pRebirthSystem->RebirthToCheckPoint(pPlayer);		break;
	case REBT_QPVP					: gsys.pRebirthSystem->RebirthToQPVP(pPlayer);				break;
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Char::OnRequestTargetInfo(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	vector<UIID> vecUIID;
	if (pCmd->GetBlob(vecUIID, 0)==false) return CR_FALSE;

	pPlayer->GetTargetInfoSender().Send(vecUIID);
	
	return CR_TRUE;
}

MCommandResult GCmdHandler_Char::OnRequestDie(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidPlayer = pCmd->GetSenderUID();

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (pPlayer ==  NULL)	return CR_FALSE;


	// ÀÚ»ì
	pPlayer->SetKiller(uidPlayer);
	pPlayer->doDie();


	return CR_TRUE;	
}
