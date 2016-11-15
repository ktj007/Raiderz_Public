#include "stdafx.h"
#include "XCmdHandler_Quest.h"
#include "XPost_Local.h"
#include "XGame.h"
#include "XEventID.h"
#include "XMyPlayer.h"
#include "XGlueGameEvent.h"
#include "XWorld.h"
#include "XModuleEntity.h"
#include "XPost_Interaction.h"
#include "XMMCode.h"
#include "XNonPlayer.h"
#include "XModuleEffect.h"
#include "XInteractionInfo.h"
#include "CTransData.h"
#include "XStrings.h"

XCmdHandler_Quest::XCmdHandler_Quest(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_QUEST_SHARE_REQ_RELAY,		OnShareReqRelay);
	SetCmdHandler(MC_QUEST_SHARE,				OnShare);
	SetCmdHandler(MC_QUEST_SHOW_ACCPET_UI,		OnShowAcceptUI);
	SetCmdHandler(MC_QUEST_SHOW_REWARD_UI,		OnShowRewardUI);	
	SetCmdHandler(MC_QUEST_GIVE,				OnGive);
	SetCmdHandler(MC_QUEST_UPDATE_OBJECTIVE,	OnUpdateQObjective);
	SetCmdHandler(MC_QUEST_COMPLETE,			OnComplete);
	SetCmdHandler(MC_QUEST_INCOMPLETE,			OnIncomplete);
	SetCmdHandler(MC_QUEST_FAIL,				OnFail);
	SetCmdHandler(MC_QUEST_REWARD,				OnReward);
	SetCmdHandler(MC_QUEST_CANCEL,				OnCancel);
	SetCmdHandler(MC_QUEST_INTERACT_QOBJECTIVE_BEGIN,	OnInteractQObjective_Begin);
	SetCmdHandler(MC_QUEST_INTERACT_QOBJECTIVE_END,		OnInteractQObejctive_End);

	SetCmdHandler(MC_CHALLENGER_QUEST_LIST,		OnChallengerQuest);
	SetCmdHandler(MC_CHALLENGER_QUEST_COMPLETE,	OnChallengerQuestComplete);
	SetCmdHandler(MC_CHALLENGER_QUEST_LIST_REFRESH,		OnChallengerQuestRefresh);
}

MCommandResult XCmdHandler_Quest::OnShareReqRelay(MCommand* pCommand, MCommandHandler* pHandler)
{
	TD_QUEST_SHARE_RESULT result;
	if (pCommand->GetSingleBlob(result,	0)==false) return CR_ERROR;

	const wchar_t* szSharerName = gvar.MyInfo.Party.GetPartyMemberName(result.uidPlayer);
	if (NULL == szSharerName) return CR_ERROR;

	XQuestInfo* pQuestInfo = info.quest->Get(result.nQuestID);
	if (NULL == pQuestInfo) return CR_ERROR;

	switch (result.nResult)
	{
	case QSR_SUCCESS :
		{
			global.ui->OnSystemMsg( XGetStr( L"QUEST_SHAREDFROMOTHER"), FSParam(szSharerName, pQuestInfo->GetTitle()));

			XGetInteractionInfo().SetRequestPlayerUID(result.uidPlayer);
			global.script->GetGlueGameEvent().OnGameEvent( "QUEST", "SHARE", result.nQuestID);
		}
		break;
	case QSR_FAIL_DISTANCE :
		{
			global.ui->OnSystemMsg( XGetStr( L"QUEST_NOTRECIEVEBYTOOFARAWAY"), FSParam(szSharerName));
		}
		break;
	case QSR_FAIL_CONDITION :
		{
			global.ui->OnSystemMsg( XGetStr( L"QUEST_NOTRECIEVEBYREQUIREMENTS"), FSParam(szSharerName));
		}
		break;
	case QSR_FAIL_EXIST :
		{
			global.ui->OnSystemMsg( XGetStr( L"QUEST_NOTRECIEVEBYALREADYHAVE"), FSParam(szSharerName));
		}
		break;
	case QSR_FAIL_DONE :
		{
			global.ui->OnSystemMsg( XGetStr( L"QUEST_NOTRECIEVEBYALREADYCOMPLETED"), FSParam(szSharerName));
		}
		break;
	}	

	return CR_TRUE;
}


MCommandResult XCmdHandler_Quest::OnShare(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_QUEST_SHARE_RESULT> vecResult;
	if (pCommand->GetBlob(vecResult,	0)==false) return CR_ERROR;

	for each (const TD_QUEST_SHARE_RESULT& result in vecResult)
	{
		const wchar_t* szMemberName = gvar.MyInfo.Party.GetPartyMemberName(result.uidPlayer);
		if (NULL == szMemberName) continue;

		switch (result.nResult)
		{
		case QSR_SUCCESS :
			{
				global.ui->OnSystemMsg( XGetStr( L"QUEST_SHAREDTOOTHER"), FSParam(szMemberName));
			}
			break;
		case QSR_FAIL_DISTANCE :
			{
				global.ui->OnSystemMsg( XGetStr( L"QUEST_NOTSHAREBYTOOFARAWAY"), FSParam(szMemberName));
			}
			break;
		case QSR_FAIL_CONDITION :
			{
				global.ui->OnSystemMsg( XGetStr( L"QUEST_NOTSHAREBYREQUIREMENTS"), FSParam(szMemberName));
			}
			break;
		case QSR_FAIL_EXIST :
			{
				global.ui->OnSystemMsg( XGetStr( L"QUEST_NOTSHAREBYALREADYHAVE"), FSParam(szMemberName));
			}
			break;
		case QSR_FAIL_DONE :
			{
				global.ui->OnSystemMsg( XGetStr( L"QUEST_NOTSHAREBYALREADYCOMPLETED"), FSParam(szMemberName));
			}
			break;
		}	
	}


	return CR_TRUE;
}



MCommandResult XCmdHandler_Quest::OnShowAcceptUI(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nQuestID;
	if (pCommand->GetParameter(&nQuestID,		0, MPT_INT)==false) return CR_ERROR;

	// 퀘스트 설명 문자열에 있는 제어코드를 실행한다.
	MUID uidInteractionNPC = XGetInteractionInfo().InteractionTargetUID;
	XQuestInfo* pQuestInfo = info.quest->Get(nQuestID);
	if (pQuestInfo)
	{
		XMMCodeMgr::GetInstance().ExecControlCode(pQuestInfo->GetDesc(), uidInteractionNPC);
	}

	if (global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "ACCEPT_QUEST", "BEGIN", nQuestID);
	}

	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if (NULL == pModuleMyControl) return CR_FALSE;

	pModuleMyControl->DoActionInteraction(uidInteractionNPC);

	if(gg.omgr)
	{
		XNonPlayer* pPlayer = gg.omgr->FindNPC(XGetInteractionInfo().InteractionTargetUID);
		if(pPlayer)
		{
			pPlayer->SetNPCIConState(NIS_QUEST_INTERACTION);
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Quest::OnShowRewardUI(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nQuestID;
	if (pCommand->GetParameter(&nQuestID,		0, MPT_INT)==false) return CR_ERROR;

	if (global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "REWARD_QUEST", "BEGIN", nQuestID);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Quest::OnGive(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	int nQuestID;
	uint32 nAcceptTime;
	uint32 nLimitTime;
	vec3 vRewardPos;
	bool bChallengerQuest = false;
	if (pCommand->GetParameter(&nQuestID,			0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nAcceptTime,		1, MPT_UINT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nLimitTime,			2, MPT_UINT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vRewardPos,			3, MPT_VEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&bChallengerQuest,	4, MPT_BOOL)==false) return CR_ERROR;

	XPlayerQuest* pNewQuest = gvar.MyInfo.Quests.Insert(nQuestID);
	if (pNewQuest == NULL)	return CR_TRUE;

	pNewQuest->m_nAcceptTime = nAcceptTime;
	pNewQuest->m_nLimitTime = nLimitTime;
	pNewQuest->bChallengerQuest = bChallengerQuest;
	pNewQuest->m_pQuestInfo->vRewardPos = vRewardPos;

	gvar.MyInfo.nRecievedQuest++;
	gvar.MyInfo.Party.QuestGive(nQuestID);

	global.ui->OnSystemMsg( XGetStr( L"QUEST_RECIEVED"));

	if ( global.sound)
		global.sound->Play( MLocale::ConvUTF16ToAnsi(SOUND_NAME_QUEST_GIVE).c_str());

	global.script->GetGlueGameEvent().OnGameEvent( "QUEST", "RECIEVE", nQuestID);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Quest::OnFail(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	int nQuestID;
	if (pCommand->GetParameter(&nQuestID,		0, MPT_INT)==false) return CR_ERROR;

	XPlayerQuest* pPlayerQuest = gvar.MyInfo.Quests.Get(nQuestID);
	if (NULL == pPlayerQuest) return CR_ERROR;

	pPlayerQuest->m_nState = QST_FAIL;

	global.ui->OnSystemMsg( XGetStr( L"QUEST_FAILED"));

	global.script->GetGlueGameEvent().OnGameEvent( "QUEST", "FAIL", nQuestID);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Quest::OnUpdateQObjective(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	TD_UPDATE_PLAYERQOBJECTIVE tdUpdatePlayerQObjective;
	if (pCommand->GetSingleBlob(tdUpdatePlayerQObjective, 0)==false) return CR_ERROR;

	if (gg.controller == NULL) return CR_ERROR;

	XPlayerQuest* pPlayerQuest = gvar.MyInfo.Quests.Get(tdUpdatePlayerQObjective.nQuestID);
	if (NULL == pPlayerQuest) return CR_ERROR;

	pPlayerQuest->UpdateObjective(tdUpdatePlayerQObjective.nQObjectiveID, tdUpdatePlayerQObjective.nProgress, tdUpdatePlayerQObjective.bComplete);
	
#ifdef _DEBUG
	if (true == tdUpdatePlayerQObjective.bComplete)
	{
		global.ui->OnSystemMsg(L"오브젝트 완료[퀘스트 ID={0} , 오브젝트 ID={1}, 상태값 = {2}]",
			FSParam(tdUpdatePlayerQObjective.nQuestID, tdUpdatePlayerQObjective.nQObjectiveID, tdUpdatePlayerQObjective.nProgress));
	}
	else
	{
		global.ui->OnSystemMsg(L"오브젝트 상태 변경[퀘스트 ID={0} , 오브젝트 ID={1}, 상태값 = {2}]",
			FSParam(tdUpdatePlayerQObjective.nQuestID, tdUpdatePlayerQObjective.nQObjectiveID, tdUpdatePlayerQObjective.nProgress));
	}
#endif

	global.script->GetGlueGameEvent().OnGameEvent( "QUEST", "OBJECTIVE", tdUpdatePlayerQObjective.nQuestID, tdUpdatePlayerQObjective.nQObjectiveID);
	

	return CR_TRUE;
}

MCommandResult XCmdHandler_Quest::OnComplete(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;
	if (gg.controller == NULL) return CR_ERROR;

	int nQuestID;
	if (pCommand->GetParameter(&nQuestID,	0, MPT_INT)==false) return CR_ERROR;

	XPlayerQuest* pPlayerQuest = gvar.MyInfo.Quests.Get(nQuestID);
	if (NULL == pPlayerQuest) return CR_ERROR;

	pPlayerQuest->m_nState = QST_COMPLETE;

	global.script->GetGlueGameEvent().OnGameEvent( "QUEST", "COMPLETE", nQuestID);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Quest::OnIncomplete(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;
	if (gg.controller == NULL) return CR_ERROR;

	int nQuestID;
	if (pCommand->GetParameter(&nQuestID,	0, MPT_INT)==false) return CR_ERROR;

	XPlayerQuest* pPlayerQuest = gvar.MyInfo.Quests.Get(nQuestID);
	if (NULL == pPlayerQuest) return CR_ERROR;

	pPlayerQuest->m_nState = QST_EXIST;

	return CR_TRUE;
}


MCommandResult XCmdHandler_Quest::OnReward(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MUID nNPCUID;
	int nQuestID;
	if (pCommand->GetParameter(&nNPCUID,	0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nQuestID,	1, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.nCompletedQuest++;

	gvar.MyInfo.Quests.Reward(nQuestID);

	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "QUEST", "REWARD", nQuestID);

	if ( global.sound)
		global.sound->Play( MLocale::ConvUTF16ToAnsi(SOUND_NAME_QUEST_COMPLETE).c_str());

	if(gg.omgr)
	{
		XNonPlayer* pNonPlayer = gg.omgr->FindNPC(nNPCUID);
		if(pNonPlayer)
		{
			pNonPlayer->SetNPCIConState(NIS_QUEST_REWARD);
			pNonPlayer->SetPreviousNPCIcon();
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Quest::OnCancel(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	int nQuestID;
	if (pCommand->GetParameter(&nQuestID,	0, MPT_INT)==false) return CR_ERROR;

	gvar.MyInfo.Quests.Cancel(nQuestID);
	gvar.MyInfo.Party.QuestCancel(nQuestID);

	global.script->GetGlueGameEvent().OnGameEvent( "QUEST", "DISMISS", nQuestID);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Quest::OnInteractQObjective_Begin(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	MUID uidNPC;
	if (pCommand->GetParameter(&uidPlayer,	0,	MPT_UID)==false)	return CR_ERROR;
	if (pCommand->GetParameter(&uidNPC,		1,	MPT_UID)==false)	return CR_ERROR;

	XPlayer* pPlayer = gg.omgr->FindPlayer(uidPlayer);
	if (NULL == pPlayer) return CR_ERROR;

	XNonPlayer* pNPC = gg.omgr->FindNPC(uidNPC);
	if (NULL == pNPC) return CR_ERROR;

	// TODO : 프로그래스바, 애니메이션
	XModuleActorControl* pActorControl = pPlayer->GetModuleActorControl();
	if (pActorControl == NULL) return CR_ERROR;

	pActorControl->DoActionQuestInteraction();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Quest::OnInteractQObejctive_End(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	if (pCommand->GetParameter(&uidPlayer,	0,	MPT_UID)==false)	return CR_ERROR;
	
	XObject* pObject = gg.omgr->Find(uidPlayer);
	if (NULL == pObject) return CR_ERROR;

	XModuleActorControl* pActorControl = pObject->GetModuleActorControl();
	if (pActorControl == NULL) return CR_ERROR;

	pActorControl->DoActionIdle();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Quest::OnChallengerQuest(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_CHALLENGER_QUEST>& vecQuestList	= gvar.Game.vecQuestList;
	vecQuestList.clear();

	if (pCommand->GetBlob(vecQuestList,			0)==false) return CR_ERROR;

	if (global.script)
	{
		global.script->GetGlueGameEvent().OnChallengerQuestBegin();
		global.script->GetGlueGameEvent().OnChallengerQuestRefresh();
	}

	gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionInteraction(XGetInteractionInfo().InteractionTargetUID);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Quest::OnChallengerQuestComplete(MCommand* pCommand, MCommandHandler* pHandler)
{
	gvar.Game.pMyPlayer->GetModuleMyControl()->StartChallengerQuestExitTimer();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Quest::OnChallengerQuestRefresh(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_CHALLENGER_QUEST>& vecQuestList	= gvar.Game.vecQuestList;
	vecQuestList.clear();

	if (pCommand->GetBlob(vecQuestList,			0)==false) return CR_ERROR;

	if (global.script)
	{
		global.script->GetGlueGameEvent().OnChallengerQuestRefresh();
	}

	return CR_TRUE;
}
