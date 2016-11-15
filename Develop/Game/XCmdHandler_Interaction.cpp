#include "StdAfx.h"
#include "XCmdHandler_Interaction.h"
#include "XController.h"
#include "XEventID.h"
#include "XStrings.h"
#include "XMMCode.h"
#include "XNonPlayer.h"
#include "XModuleActorControl.h"
#include "XModuleEffect.h"
#include "XMyPlayer.h"
#include "XPost_Interaction.h"
#include "XModuleMovable.h"
#include "XModuleNonControl.h"
#include "XInteractionInfo.h"
#include "XSoundNonPlayer.h"
#include "XUtil.h"

XCmdHandler_Interaction::XCmdHandler_Interaction(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_NPCINTERACTION_INTERACTION,	OnNPCInteraction);
	SetCmdHandler(MC_NPCINTERACTION_END,			OnNPCInteractionEnd);
	SetCmdHandler(MC_NPCINTERACTION_ICON,			OnNPCICon);	
	SetCmdHandler(MC_NPCINTERACTION_LOOT_START,		OnNPCInteractionLootStart);
}

MCommandResult XCmdHandler_Interaction::OnNPCInteraction(MCommand* pCommand, MCommandHandler* pHandler)
{
	gvar.Game.NPCInteractionInfo.Clear();

	MUID uidNPC;
	wstring& strOpeningTextID = gvar.Game.NPCInteractionInfo.GetOpeningTextID();
	vector<TD_INTERACTION_ELEMENT>& vecTDIElement = gvar.Game.NPCInteractionInfo.GetList();

	if (pCommand->GetParameter(&uidNPC,0, MPT_UID)==false)	return CR_ERROR;
	if (pCommand->GetParameter(strOpeningTextID,1, MPT_WSTR)==false)	return CR_ERROR;
	if (pCommand->GetBlob(vecTDIElement, 2)==false) return CR_ERROR;

	_ASSERT(0 != vecTDIElement.size());
	if (vecTDIElement.size() == 0) return CR_ERROR;

	for (vector<TD_INTERACTION_ELEMENT>::iterator itor = vecTDIElement.begin(); itor != vecTDIElement.end(); itor++)
	{
		// 문자열테이블 키값의 경우, 해당 문자열로 치환한다.
		wstring strTrans;
		XStringResMgr::GetInstance()->TransKeyStr(strTrans, (*itor).szIEText);
		wcsncpy_s((*itor).szIEText, strTrans.c_str(), _TRUNCATE);


		// IgnoreNonTranslationMark 제거
		if (XCONST::IGNORE_NON_TRANSLATION_MARK)
		{
			strTrans = ConvertIgnoreNonTranslationMark((*itor).szIEText);
			wcsncpy_s((*itor).szIEText, strTrans.c_str(), _TRUNCATE);
		}
	}

	//인터랙션 방향으로 돌리기
	XNonPlayer* pNonPlayer = gg.omgr->FindNPC(uidNPC);
	if (pNonPlayer == NULL) return CR_FALSE;

	XGetInteractionInfo().InteractionTargetUID = uidNPC;

	if((pNonPlayer->GetInfo(true) != NULL) &&
		pNonPlayer->GetInfo(true)->IsInteractionRotate())
	{
		UIID nUIID = pNonPlayer->GetUIID();
		vec3 vPos = pNonPlayer->GetPosition();
		vec3 vDir = gvar.Game.pMyPlayer->GetPosition() - vPos;
		XMath::NormalizeZ0( vDir);

		TD_NPC_MOVE move;
		move.vTarPos = vPos;
		move.svDir = vDir;
		move.nSpeed = 500;
		move.nUIID = nUIID;
		XModuleNonControl* pModuleNonControl = pNonPlayer->GetModuleNonControl();
		if (pModuleNonControl)
		{
			pModuleNonControl->OnNPCMove(&move, false);
		}			
	}

	if (vecTDIElement.size() == 1) //선택할수 있는 게 하나면 바로 선택해버리도록
	{
		XPostNPCInteraction_IE(uidNPC, vecTDIElement[0].nIElementID);
		return CR_TRUE;
	}

	XGetSoundNonPlayer()->PlayVoice( uidNPC);

	gvar.Game.NPCInteractionInfo.MenuType(MENU_NPC);
	if (global.script)
		global.script->GetGlueGameEvent().OnGameEvent( "INTERACTION_NPC", "BEGIN");

	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if (NULL == pModuleMyControl) return CR_FALSE;
	pModuleMyControl->DoActionInteraction(uidNPC);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Interaction::OnNPCInteractionEnd(MCommand* pCommand, MCommandHandler* pHandler)
{
	if(gvar.Game.pMyPlayer != NULL)
	{
		XModuleMyControl* pMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
		if (pMyControl) 
		{
			pMyControl->DoActionIdleWithSpecialState();

			XEvent msg(XEVTL_MYACTION_NPC_INTERACTION_END);
			pMyControl->Event(msg);
		}
	}

	if (global.ui)
	{
		int nVisibleWindowCount = global.script->GetGlueGameEvent().GetVisibledWindow();
		if (nVisibleWindowCount > 0)
			global.script->GetGlueGameEvent().OnInteractonWindowsClose();
	}

	if (XGetInteractionInfo().InteractionTargetUID != MUID::ZERO)
	{
		XNonPlayer* pNPC = gg.omgr->FindNPC(XGetInteractionInfo().InteractionTargetUID);
		if (pNPC && pNPC->GetInfo() && (pNPC->GetInfo()->eIT == IT_DIALOG))
		{
			wstring str = pNPC->GetCurrentAnimationName();
			if((pNPC->IsDead() == false) /*&& (MOTION_NAME_DIE != str)*/ && (L"none_die" != str))
			{
				XModuleNonControl* pModuleNonControl = pNPC->GetModuleNonControl();
				if (pModuleNonControl)
				{
					pModuleNonControl->DoActionIdle();
				}
			}
		}

		XGetSoundNonPlayer()->PlayVoice( XGetInteractionInfo().InteractionTargetUID, "BYE");

		XGetInteractionInfo().InteractionTargetUID = MUID::ZERO;
	}

	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "INTERACTION_NPC", "END");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Interaction::OnNPCICon(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_NPC_ICON> vecTDNPCIcon;
	vector<TD_ESSENTIAL_NPC_ICON> vecTDEssentialNPCIcon;

	if (pCommand->GetBlob(vecTDNPCIcon,			0)==false)	return CR_ERROR;
	if (pCommand->GetBlob(vecTDEssentialNPCIcon,1)==false)	return CR_ERROR;

	for each(TD_NPC_ICON tdNPCIcon in vecTDNPCIcon)
	{
		XNonPlayer* pNPC = gg.omgr->FindNPC_UIID(tdNPCIcon.m_nUIID);
		if (pNPC == NULL) continue;

		pNPC->SetNPCIConTypeToIconState(tdNPCIcon.m_nIcon);

		if (tdNPCIcon.m_nIcon == NIT_COMBAT) continue;

		map<UIID, NPC_ICON_INFO>::iterator itor = gvar.Game.vecNPCIcon.find(tdNPCIcon.m_nUIID);
		if (itor != gvar.Game.vecNPCIcon.end())
		{
			NPC_ICON_INFO& tdNPCIconSe = itor->second;
			tdNPCIconSe.m_nIcon = tdNPCIcon.m_nIcon;
			continue;
		}

		NPC_ICON_INFO tdFarTemp(tdNPCIcon.m_nUIID, tdNPCIcon.m_nIcon, pNPC->GetNPCID(), pNPC->GetPosition());
		gvar.Game.vecNPCIcon.insert(map<UIID, NPC_ICON_INFO>::value_type(tdNPCIcon.m_nUIID, tdFarTemp));
	}

	for each(TD_ESSENTIAL_NPC_ICON tdEssentialNPCIcon in vecTDEssentialNPCIcon)
	{
		map<UIID, NPC_ICON_INFO>::iterator itor = gvar.Game.vecNPCIcon.find(tdEssentialNPCIcon.m_nUIID);
		if (itor != gvar.Game.vecNPCIcon.end())
		{
			NPC_ICON_INFO& tdNPCIconSe = itor->second;
			tdNPCIconSe.m_nIcon = tdEssentialNPCIcon.m_nIcon;
			continue;
		}

		gvar.Game.vecNPCIcon.insert(map<UIID, NPC_ICON_INFO>::value_type(tdEssentialNPCIcon.m_nUIID, tdEssentialNPCIcon));
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Interaction::OnNPCInteractionLootStart(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidLooter;
	MUID uidNPC;
	if (pCommand->GetParameter(&uidLooter,	0,	MPT_UID)==false)	return CR_ERROR;
	if (pCommand->GetParameter(&uidNPC,		1,	MPT_UID)==false)	return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor(uidLooter);
	if (!pActor) return CR_TRUE;

	XModuleActorControl* pModuleActorControl = pActor->GetModuleActorControl();
	if(pModuleActorControl)
	{
		pModuleActorControl->DoActionLootingItem(uidNPC);
	}

	return CR_TRUE;
}
