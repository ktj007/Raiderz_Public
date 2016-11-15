#include "stdafx.h"
#include "XCmdHandler_NPCAction.h"
#include "XGlobal.h"
#include "XNonPlayer.h"
#include "XModuleEntity.h"
#include "XModuleNonControl.h"
#include "XNetwork.h"
#include "XWhoISRequester.h"

XCmdHandler_NPCAction::XCmdHandler_NPCAction(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_NPC_MOVE,					OnNPCMove);
	SetCmdHandler(MC_NPC_RUN,					OnNPCRun);
	SetCmdHandler(MC_NPC_MOVE_STOP,				OnNPCMoveStop);
	SetCmdHandler(MC_NPC_FACE_TO,				OnNPCFaceTo);
	SetCmdHandler(MC_NPC_CHANGE_CAPSULE,		OnNPCChangeCapsule);
	SetCmdHandler(MC_NPC_CHANGE_ATTACKABLE,		OnChangeAttackable);
	SetCmdHandler(MC_NPC_CHANGE_COMBAT_TARGET,	OnChangetCombatTarget);
	SetCmdHandler(MC_NPC_SPEWUP,				OnSpewUp);
	SetCmdHandler(MC_NPC_SPEWUP_INTERRUPT,		OnSpewUpInterrupt);
}


MCommandResult XCmdHandler_NPCAction::OnNPCMove(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;
	TD_NPC_MOVE* pNM = (TD_NPC_MOVE*)pParam->GetPointer();

	XActor* pNPC = gg.omgr->FindActor_UIID(pNM->nUIID);
	if (pNPC)
	{
		XModuleNonControl* pModuleNonControl = pNPC->GetModuleNonControl();
		if (pModuleNonControl)
		{
			pModuleNonControl->OnNPCMove(pNM, false);
		}
	}
	else
	{
		// 없으면 이놈 NPC가 누군지 요청
		if (global.net && global.net->GetNetClient())
		{
			XWhoIsRequester* pWhoIsRequester = global.net->GetNetClient()->GetWhoIsRequester();
			pWhoIsRequester->Request(pNM->nUIID);
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_NPCAction::OnNPCRun(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_FALSE;
	TD_NPC_MOVE* pNM = (TD_NPC_MOVE*)pParam->GetPointer();

	XActor* pNPC = gg.omgr->FindActor_UIID(pNM->nUIID);
	if (pNPC)
	{
		XModuleNonControl* pModuleNonControl = pNPC->GetModuleNonControl();
		if (pModuleNonControl)
		{
			pModuleNonControl->OnNPCMove(pNM, true);
		}
	}
	else
	{
		// 없으면 이놈 NPC가 누군지 요청
		if (global.net && global.net->GetNetClient())
		{
			XWhoIsRequester* pWhoIsRequester = global.net->GetNetClient()->GetWhoIsRequester();
			pWhoIsRequester->Request(pNM->nUIID);
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_NPCAction::OnNPCMoveStop(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nUIID;
	vec3 pos;

	if (pCommand->GetParameter(&nUIID,		0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&pos,		1, MPT_VECTOR)==false) return CR_ERROR;


	XObject* pObject = gg.omgr->FindActor_UIID(nUIID);
	if (pObject == NULL) return CR_TRUE;

	XModuleNonControl* pModuleNonControl = pObject->GetModuleNonControl();
	if (pModuleNonControl)
	{
		pModuleNonControl->OnNPCStop(pos);
	}

	return CR_TRUE;
}


MCommandResult XCmdHandler_NPCAction::OnNPCFaceTo(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	UIID nUIID;
	svec2 svDir;
	vec3 vPos;

	if (pCommand->GetParameter(&nUIID,		0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&svDir,		1, MPT_SVEC2)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vPos,		2, MPT_VEC)==false) return CR_ERROR;

	XActor* pNPC = gg.omgr->FindActor_UIID(nUIID);
	if (pNPC)
	{
		if (pNPC->GetModuleNonControl())
		{
			vec3 vDir; vDir = svDir;

			pNPC->GetModuleNonControl()->OnNPCFaceTo(vPos, vDir);
		}
	}

	return CR_TRUE;
}


MCommandResult XCmdHandler_NPCAction::OnNPCChangeCapsule(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MUID uidNPC;
	int8 nCapsuleGroupIndex;
	if (pCommand->GetParameter(&uidNPC,					0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nCapsuleGroupIndex,		1, MPT_CHAR)==false) return CR_ERROR;

	XNonPlayer* pNPC = gg.omgr->FindNPC(uidNPC);
	if (pNPC)
	{
		pNPC->SetOpenHitGroupID(nCapsuleGroupIndex);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_NPCAction::OnChangeAttackable(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MUID uidNPC;
	NPC_ATTACKABLE_TYPE nAttackableType;
	if (pCommand->GetParameter(&uidNPC,				0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nAttackableType,	1, MPT_INT)==false) return CR_ERROR;

	// bEnable = true (공격), false (비공격)
	XNonPlayer* pNPC = gg.omgr->FindNPC(uidNPC);
	if (pNPC)
	{
//		bool bAttackable = pNPC->IsEnemy(nAttackableType);
		pNPC->SetAttackableType(nAttackableType);
		pNPC->SetNPCNameColor(pNPC->IsAttackable(), true);

		//vector<MUID> vecInteractionNPC;
		//vecInteractionNPC.push_back(uidNPC);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_NPCAction::OnChangetCombatTarget(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MUID uidNPC;
	MUID uidTarget;
	if (pCommand->GetParameter(&uidNPC,			0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidTarget,		1, MPT_UID)==false) return CR_ERROR;

	XNonPlayer* pNPC = gg.omgr->FindNPC( uidNPC);
	if (pNPC)
		pNPC->SetTarget( uidTarget);

	return CR_TRUE;
}

MCommandResult XCmdHandler_NPCAction::OnSpewUp(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID nNPCUID;
	if (pCommand->GetParameter(&nNPCUID,			0, MPT_UID)==false) return CR_ERROR;

	XNonPlayer* pNPC = gg.omgr->FindNPC(nNPCUID);
	if (pNPC)
	{
		XModuleActorControl* pActorControl = pNPC->GetModuleActorControl();
		if(pActorControl)
		{
			pActorControl->OnSpew();
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_NPCAction::OnSpewUpInterrupt(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID nNPCUID;
	if (pCommand->GetParameter(&nNPCUID,			0, MPT_UID)==false) return CR_ERROR;

	XNonPlayer* pNPC = gg.omgr->FindNPC(nNPCUID);
	if (pNPC)
	{
		XModuleActorControl* pActorControl = pNPC->GetModuleActorControl();
		if(pActorControl)
		{
			pActorControl->OnSpewInterrupt();
		}
	}

	return CR_TRUE;
}

