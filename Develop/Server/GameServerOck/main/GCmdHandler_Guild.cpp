#include "StdAfx.h"
#include "GCmdHandler_Guild.h"
#include "CCommandTable.h"
#include "GEntityPlayer.h"
#include "GGuildSystem.h"
#include "GGuildMgr.h"
#include "GGuild.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"

GCmdHandler_Guild::GCmdHandler_Guild(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_GUILD_CREATE_REQ,			OnCreate);
	SetCmdHandler(MC_GUILD_INVITE_REQ,			OnInvite_Req);
	SetCmdHandler(MC_GUILD_INVITE_RES,			OnInvite_Res);
	SetCmdHandler(MC_GUILD_LEAVE_REQ,			OnLeave);
	SetCmdHandler(MC_GUILD_KICK_REQ,			OnKick);
	SetCmdHandler(MC_GUILD_DESTROY_REQ,			OnDestroy);	
	SetCmdHandler(MC_GUILD_CHANGE_MASTER_REQ,	OnChangeMaster);
	SetCmdHandler(MC_GUILD_DEPOSIT_STORAGEMONEY_REQ,	OnDepositStorageMoney);
	SetCmdHandler(MC_GUILD_WITHDRAW_STORAGEMONEY_REQ,	OnWithdrawStorageMoney);
}

MCommandResult GCmdHandler_Guild::OnCreate(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	wstring strName;
	if (pCmd->GetParameter(strName,	0, MPT_WSTR)==false) return CR_FALSE;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_GUILD_CREATE))
	{
		gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);
		return CR_FALSE;
	}

	gsys.pGuildSystem->Create(pPlayer, strName.c_str());

	return CR_TRUE;
}


MCommandResult GCmdHandler_Guild::OnInvite_Req(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	MUID uidGuest;
	if (pCmd->GetParameter(&uidGuest,	0, MPT_UID)==false) return CR_FALSE;

	GEntityPlayer* pGuest = gmgr.pPlayerObjectManager->GetEntity(uidGuest);
	if (NULL == pGuest) return CR_FALSE;

	gsys.pGuildSystem->Invite_Req(pPlayer, pGuest);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Guild::OnInvite_Res(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	bool bAccept;
	if (pCmd->GetParameter(&bAccept,	0, MPT_BOOL)==false) return CR_FALSE;

	gsys.pGuildSystem->Invite_Res(pPlayer, bAccept);
	gmgr.pGuildMgr->InviteEnd(pPlayer->GetCID());

	return CR_TRUE;
}

MCommandResult GCmdHandler_Guild::OnLeave(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	gsys.pGuildSystem->Leave(pPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Guild::OnKick(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	wstring strTargetName;
	if (pCmd->GetParameter(strTargetName,	0, MPT_WSTR)==false) return CR_FALSE;

	gsys.pGuildSystem->Kick(pPlayer, strTargetName.c_str());

	return CR_TRUE;
}

MCommandResult GCmdHandler_Guild::OnDestroy(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (false == gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_GUILD_DESTROY))
	{
		gsys.pInteractionSystem->GetNPCInteractor().End(pPlayer);
		return CR_FALSE;
	}

	gsys.pGuildSystem->Destroy(pPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Guild::OnChangeMaster(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	wstring strNewMasterName;
	if (pCmd->GetParameter(strNewMasterName,	0, MPT_WSTR)==false) return CR_FALSE;

	gsys.pGuildSystem->ChangeMaster(pPlayer, strNewMasterName.c_str());

	return CR_TRUE;
}


MCommandResult GCmdHandler_Guild::OnDepositStorageMoney(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nMoney;
	if (pCmd->GetParameter(&nMoney,	0, MPT_INT)==false) return CR_FALSE;

	if (!gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_GUILD_STORAGE))
		return CR_FALSE;

	gsys.pGuildSystem->DepositStorageMoney(pPlayer, nMoney);

	return CR_TRUE;
}


MCommandResult GCmdHandler_Guild::OnWithdrawStorageMoney(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nMoney;
	if (pCmd->GetParameter(&nMoney,	0, MPT_INT)==false) return CR_FALSE;

	if (!gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_GUILD_STORAGE))
		return CR_FALSE;

	gsys.pGuildSystem->WithdrawStorageMoney(pPlayer, nMoney);

	return CR_TRUE;
}
