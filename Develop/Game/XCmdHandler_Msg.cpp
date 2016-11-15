#include "stdafx.h"
#include "XCmdHandler_Msg.h"
#include "XController.h"
#include "XStrings.h"
#include "XModuleActorControl.h"
#include "XMMCode.h"
#include "XPvPAreaLogic.h"
#include "XGame.h"
#include "XMsgCommandFacade.h"
#include "XNPCInfo.h"

XCmdHandler_Msg::XCmdHandler_Msg(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{		
	SetCmdHandler(MC_NPC_SAY,				OnNpcSay);
	SetCmdHandler(MC_NPC_BALLOON,			OnNpcBalloon);
	SetCmdHandler(MC_NPC_SHOUT,				OnNpcShout);
	SetCmdHandler(MC_NPC_NARRATION,			OnNpcNarration);
	SetCmdHandler(MC_NPC_SAY_AND_NARRATION,	OnNpcSayAndNarration);

	SetCmdHandler(MC_MSG_RES,				OnMsgRes);
}

MCommandResult XCmdHandler_Msg::OnNpcSay(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID	uidSpeaker;
	int		nSpeakerID;
	wstring	strKeyWithParam;
	float	fMaintainTime;

	if (pCommand->GetParameter(&uidSpeaker,	0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSpeakerID,	1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(strKeyWithParam,	2, MPT_WSTR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fMaintainTime,	3, MPT_FLOAT)==false) return CR_ERROR;


	wstring strSpeaker = info.npc->GetName(nSpeakerID);
	wstring strMessage = XStringResMgr::GetInstance()->TransKeyStrWithParam(strKeyWithParam);
	
	if ( XGETCFG_GAME_SHOWCHATMSG)
	{
		if (gg.controller)
		{
			mmcode::MControlCodeTransResult result = XMMCodeMgr::GetInstance().TransControlCodeAndReplaceChars(strMessage.c_str(), XGetMyUID());
			wstring szChat = MLocale::ConvAnsiToUTF16(result.m_strOutput.c_str());

			wstring strText = strSpeaker + L" : " + szChat;
			gg.controller->OnChat(MT_SAY, strText.c_str());
		}
	}

	if ( XGETCFG_GAME_SHOWCHATBUBBLE)
	{
		XActor* pActor = gg.omgr->FindActor(uidSpeaker);
		if (!pActor) return CR_FALSE;

		XModuleActorControl* pActorControl = pActor->GetModuleActorControl();
		if (pActorControl)
			pActorControl->OnBallonText(strSpeaker, strMessage, fMaintainTime);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Msg::OnNpcBalloon(MCommand* pCommand, MCommandHandler* pHandler)
{
	if ( XGETCFG_GAME_SHOWCHATBUBBLE == false)
		return CR_TRUE;


	int		nSpeakerID;
	MUID	uidSpeaker;
	wstring	strKeyWithParam;
	float	fMaintainTime;

	if (pCommand->GetParameter(&uidSpeaker,		0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSpeakerID,		1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(strKeyWithParam, 2, MPT_WSTR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fMaintainTime,	3, MPT_FLOAT)==false) return CR_ERROR;

	wstring strSpeaker = info.npc->GetName(nSpeakerID);
	wstring strMessage = XStringResMgr::GetInstance()->TransKeyStrWithParam(strKeyWithParam);

	XActor* pActor = gg.omgr->FindActor(uidSpeaker);
	if (!pActor) return CR_FALSE;

	XModuleActorControl* pActorControl = pActor->GetModuleActorControl();
	if (pActorControl)
	{
		pActorControl->OnBallonText(strSpeaker, strMessage, fMaintainTime);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Msg::OnNpcShout(MCommand* pCommand, MCommandHandler* pHandler)
{
	int		nSpeakerID;
	MUID	uidSpeaker;
	wstring	strKeyWithParam;
	float 	fMaintainTime;

	if (pCommand->GetParameter(&uidSpeaker,		0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSpeakerID,		1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(strKeyWithParam,	2, MPT_WSTR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fMaintainTime,	3, MPT_FLOAT)==false) return CR_ERROR;


	wstring strSpeaker = info.npc->GetName(nSpeakerID);
	wstring strMessage = XStringResMgr::GetInstance()->TransKeyStrWithParam(strKeyWithParam);

	if ( XGETCFG_GAME_SHOWCHATMSG)
	{
		if (gg.controller)
		{
			mmcode::MControlCodeTransResult result = XMMCodeMgr::GetInstance().TransControlCodeAndReplaceChars(strKeyWithParam.c_str(), XGetMyUID());
			wstring szChat = MLocale::ConvAnsiToUTF16(result.m_strOutput.c_str());

			wstring strText = L"[외침] " + strSpeaker + L" : " + szChat;
			gg.controller->OnChat(MT_SAY, strText.c_str());
		}
	}

	if ( XGETCFG_GAME_SHOWCHATBUBBLE)
	{
		XActor* pActor = gg.omgr->FindActor(uidSpeaker);
		if (!pActor) return CR_FALSE;

		XModuleActorControl* pActorControl = pActor->GetModuleActorControl();
		if (pActorControl)
			pActorControl->OnBallonText(strSpeaker, strKeyWithParam, fMaintainTime);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Msg::OnNpcNarration(MCommand* pCommand, MCommandHandler* pHandler)
{
	if ( XGETCFG_GAME_SHOWCHATMSG == false)
		return CR_TRUE;

	wstring	strKeyWithParam;
	if (pCommand->GetParameter(strKeyWithParam, 0, MPT_WSTR)==false)	return CR_ERROR;	
		

	wstring strMessage = XStringResMgr::GetInstance()->TransKeyStrWithParam(strKeyWithParam);

	MUID uidOwner = MUID::ZERO;
	mmcode::MControlCodeTransResult result = XMMCodeMgr::GetInstance().TransControlCodeAndReplaceChars(strMessage.c_str(), XGetMyUID());
	result.Proc(&uidOwner);

	wstring strText = MLocale::ConvAnsiToUTF16(result.m_strOutput.c_str());
	global.ui->OnPresentationUpper(strText);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Msg::OnNpcSayAndNarration(MCommand* pCommand, MCommandHandler* pHandler)
{
	int		nSpeakerID;
	MUID	uidSpeaker;
	wstring	strKeyWithParam;
	float	fMaintainTime;

	if (pCommand->GetParameter(&uidSpeaker,	0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSpeakerID,	1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(strKeyWithParam, 2, MPT_WSTR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fMaintainTime,	3, MPT_FLOAT)==false) return CR_ERROR;


	wstring strSpeaker = info.npc->GetName(nSpeakerID);
	wstring strMessage = XStringResMgr::GetInstance()->TransKeyStrWithParam(strKeyWithParam);

	if ( XGETCFG_GAME_SHOWCHATMSG)
	{
		if (gg.controller)
		{
			mmcode::MControlCodeTransResult result = XMMCodeMgr::GetInstance().TransControlCodeAndReplaceChars(strMessage.c_str(), XGetMyUID());
			wstring szChat = MLocale::ConvAnsiToUTF16(result.m_strOutput.c_str());

			wstring strText = strSpeaker + L" : " + szChat;
			gg.controller->OnChat(MT_SAY, strText.c_str());
		}
	}

	if ( XGETCFG_GAME_SHOWCHATBUBBLE)
	{
		XActor* pActor = gg.omgr->FindActor(uidSpeaker);
		if (!pActor) return CR_FALSE;

		XModuleActorControl* pActorControl = pActor->GetModuleActorControl();
		if (pActorControl)
			pActorControl->OnBallonText(strSpeaker, strMessage, fMaintainTime);

		wstring strString = strSpeaker + L" : " + strMessage;
		global.ui->OnPresentationUpper(strString);
	}


	return CR_TRUE;
}

MCommandResult XCmdHandler_Msg::OnMsgRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 커멘드 파싱 및 처리 모두 담당
	if (!global.msg->ResponseMessage(pCommand))	return CR_ERROR;
	
	return CR_TRUE;
}

wstring XCmdHandler_Msg::FilterPvPArea( wchar_t* szPlayerID, wchar_t* szMsg )
{
	wstring strMsg = szMsg;

	if (gg.currentgamestate)
	{
		XPvPAreaLogic* pPvPAreaLogic = gg.currentgamestate->GetPvPArea();
		if (pPvPAreaLogic->IsInPvPArea())
		{
			XPlayer* pPlayer = gg.omgr->FindPlayerInPvP(szPlayerID);
			if (pPlayer && pPvPAreaLogic->IsEnemy(pPlayer))
			{
				strMsg = pPvPAreaLogic->MakeEnemyChatMsg(szMsg);
			}

		}
	}

	return strMsg;
}
