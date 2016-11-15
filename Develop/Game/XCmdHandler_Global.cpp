#include "stdafx.h"
#include "XCmdHandler_Global.h"
#include "XPost_Local.h"
#include "XGame.h"
#include "XEventID.h"
#include "XItemManager.h"
#include "XItem.h"
#include "XItemContainer.h"
#include "XMyPlayer.h"
#include "XNonPlayer.h"
#include "XGlueGameEvent.h"
#include "XWorld.h"
#include "XModuleEntity.h"
#include "XModuleMyControl.h"
#include "XModuleMotion.h"
#include "XModuleNetControl.h"
#include "XModuleNonControl.h"
#include "XModuleEffect.h"
#include "XItemContainer.h"
#include "XNonPlayer.h"
#include "XEffectManager.h"
#include "XMyPlayer.h"
#include "XNetDebuger.h"
#include "XGameFrameManager.h"
#include "XBaseApplication.h"
#include "XCharacter.h"
#include "XConnectionManager.h"
#include "XNetwork.h"
#include "XInteractionInfo.h"

#include "XPost_Interaction.h"
#include "XStrings.h"
#include "XMMCode.h"
#include "XGlueGameEvent.h"
#include "MCsvParser.h"
#include "XSystem.h"
#include "XTalentInfoMgr.h"
#include "CTransData.h"
#include "XWhoISRequester.h"
#include "XSensorManager.h"
#include "XSoundNonPlayer.h"


XCmdHandler_Global::XCmdHandler_Global(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_FIELD_SECTOR_ENTITY_INFO,		OnSectorEntityInfo);
	SetCmdHandler(MC_TRIGGER_UPDATE_USABLE_SENSOR,	OnUpdateUsableSensor);
	SetCmdHandler(MC_TRIGGER_SENSOR_DESTINATION_SEL_REQ,	OnSensorDestinationSelection);
	SetCmdHandler(MC_TRIGGER_SENSOR_ERROR_MSG,				OnSensorErrorMsg);
	SetCmdHandler(MC_TRIGGER_SENSOR_FOLLOW_PARTY_REQ,		OnSensorFollowPartyReq);

	SetCmdHandler(MC_ENV_PLAY_BGM,				OnPlayBGM);

	SetCmdHandler(MC_ENTITY_WARP,				OnEntityWarp);

	SetCmdHandler(MC_DIALOG_START,				OnDialogStart);
	SetCmdHandler(MC_DIALOG_SELECT,				OnDialogSelect);

	SetCmdHandler(MC_TIME_SYNCH,				OnTimeSynch);

	SetCmdHandler(MC_GM_GOD,					OnGMGod);
	SetCmdHandler(MC_GM_GHOST,					OnGMGhost);
	SetCmdHandler(MC_GM_GET_PLAYERUID,			OnGMGetUID);
	SetCmdHandler(MC_DEBUG_STRING,				OnTestDebug);
	SetCmdHandler(MC_AI_DEBUG_MESSAGE,			OnAIDebugMessage);
	SetCmdHandler(MC_DEBUG_COMBATCALC,			OnDebugCombatcalc);
	SetCmdHandler(MC_DEBUG_NPC_NETLOG,			OnDebugNPCNetLog);

	SetCmdHandler(MC_GG_AUTH_REQ,				OnGameGuardAuthReq);
}


MCommandResult XCmdHandler_Global::OnSectorEntityInfo(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	// player
	vector<TD_UPDATE_CACHE_PLAYER> vecPlayerInfo;
	if (pCommand->GetBlob(vecPlayerInfo,		0)==false) return CR_ERROR;
	vector<TD_PLAYER_FEATURE_TATTOO> vecPlayerFeatureTattoo;
	if (pCommand->GetBlob(vecPlayerFeatureTattoo,	1)==false) return CR_ERROR;
	
	int nSize = vecPlayerInfo.size();
	for (int i=0; i < nSize; i++)
	{
		TD_PLAYER_FEATURE_TATTOO* _tattoo = NULL;
		for ( vector<TD_PLAYER_FEATURE_TATTOO>::iterator itr = vecPlayerFeatureTattoo.begin();  itr != vecPlayerFeatureTattoo.end();  itr++)
		{
			if ( vecPlayerInfo[i].nUIID == (*itr).nUIID)
			{
				_tattoo = &(*itr);
				break;
			}
		}
		gg.currentgamestate->InPlayer(vecPlayerInfo[i].uid, &vecPlayerInfo[i], _tattoo, false);
	}

	//----------------------------------------------------------------------------------------------------
	// npc
	MCommandParameter* pParam = pCommand->GetParameter(2);
	if(pParam->GetType() != MPT_BLOB) return CR_ERROR;

	TD_UPDATE_CACHE_NPC* pNpcNode = (TD_UPDATE_CACHE_NPC*)pParam->GetPointer();
	int nNpcCount = pParam->GetBlobCount();

	gg.currentgamestate->InNPCs(pNpcNode, nNpcCount);

	//----------------------------------------------------------------------------------------------------
	// bpart
	pParam = pCommand->GetParameter(3);
	if(pParam->GetType() != MPT_BLOB) return CR_ERROR;

	TD_UPDATE_CACHE_PBART* pBPartNode = (TD_UPDATE_CACHE_PBART*)pParam->GetPointer();
	int nBPartCount = pParam->GetBlobCount();
	
	gg.currentgamestate->InBPartNPCs(pBPartNode, nBPartCount);

	//----------------------------------------------------------------------------------------------------
	// MagicArea
	pParam = pCommand->GetParameter(4);
	if(pParam->GetType() != MPT_BLOB) return CR_ERROR;

	TD_UPDATE_CACHE_BUFFENTITY* pMagicAreaNode = (TD_UPDATE_CACHE_BUFFENTITY*)pParam->GetPointer();
	int nMagicAreaCount = pParam->GetBlobCount();

	gg.currentgamestate->InMagicAreas(pMagicAreaNode, nMagicAreaCount);

	//----------------------------------------------------------------------------------------------------
	// usable markers

	vector<TD_TRIGGER_SENSOR_INFO> vecSensors;
	if (false == pCommand->GetBlob(vecSensors, 5)) return CR_ERROR;

	XSensorManager* pSensorManager = gg.currentgamestate->GetSensorManager();
	pSensorManager->Init(gg.currentgamestate->GetWorld()->GetInfo(), vecSensors);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Global::OnUpdateUsableSensor(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;
	if (gg.currentgamestate == NULL) return CR_ERROR;

	// 해당 필드의 field.xml이 없어 필드 초기화에 실패한 경우, pMyPlayer가 NULL일 수 있음.	
	if (gvar.Game.pMyPlayer == NULL) return CR_ERROR;

	vector<TD_TRIGGER_SENSOR_INFO> vecUsableSensorID;	
	if (false == pCommand->GetBlob(vecUsableSensorID, 0)) return CR_ERROR;

	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if (pFieldInfo==NULL) return CR_FALSE;	

	XSensorManager* pSensorManager = gg.currentgamestate->GetSensorManager();
	pSensorManager->Init(pFieldInfo, vecUsableSensorID);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Global::OnSensorDestinationSelection(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	// usable markers
	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType() != MPT_BLOB) return CR_ERROR;

	int nTitleCounter = pParam->GetBlobCount();
	mlog("*OnSensorDestinationSelection* | nTitleCounter = %d\n", nTitleCounter);

	//wstring szTextID;
	//char szChat[1024] = {0,};

	//for (int i=0; i<nTitleCounter; i++)
	//{
	//	char pTest[80];
	//	strncpy(pTest, (const char*)pParam->GetBlobElement(i), pParam->GetBlobSize());
	//	mlog("[%d] %s\n", i, pTest);

	//	sprintf_s(szChat, "%d,%s", i, pTest);

	//	if( i != 0)
	//		szTextID += ",";

	//	szTextID += szChat;
	//}

	// TODO : 넘어오는 패킷을 NPCInteraction 에 맞게 보내주면 좋겠어요
	// TD_INTERACTION_ELEMENT

	gvar.Game.NPCInteractionInfo.Clear();
	wstring& strMsg = gvar.Game.NPCInteractionInfo.GetOpeningTextID();
	strMsg = L"센서로 이동하시겠습니까?\n";

	vector<TD_INTERACTION_ELEMENT>& vecTDIElement = gvar.Game.NPCInteractionInfo.GetList();

	for (int i=0; i<nTitleCounter; i++)
	{
		TD_INTERACTION_ELEMENT td;
		td.nIElementID = i;
		wcsncpy_s(td.szIEText, (wchar_t*)pParam->GetBlobElement(i), _TRUNCATE);
		vecTDIElement.push_back(td);
	}

	gvar.Game.pMyPlayer->GetModuleMyControl()->StopRun();

	gvar.Game.NPCInteractionInfo.MenuType(MENU_SECTOR);
	if (global.script)
		global.script->GetGlueGameEvent().OnGameEvent( "INTERACTION_NPC", "BEGIN");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Global::OnSensorErrorMsg(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	// no parameter command

	// 트라이얼필드가 만원이되어 워프 실패
	if (global.script)
	{
		wstring strMsg = L"이동하려는 필드의 인원제한에 걸렸습니다.\n퀘스트를 확인해주세요.\n";
//		global.script->GetGlueGameEvent().OnNoticeDialogShow(strMsg.c_str());
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Global::OnSensorFollowPartyReq(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	// 커멘드 확인
	wstring strMemberName;
	if (!pCommand->GetParameter(strMemberName, 0, MPT_WSTR))		return CR_FALSE;	


	// 트라이얼필드 파티원 따라가기 선택 창
	if (global.script)
	{
		gvar.Game.NPCInteractionInfo.Clear();
		wstring& strMsg = gvar.Game.NPCInteractionInfo.GetOpeningTextID();
		strMsg = strMemberName + L"님이 트라이얼 필드로 이동합니다.\n따라가시겠습니까?\n";

		vector<TD_INTERACTION_ELEMENT>& vecTDIElement = gvar.Game.NPCInteractionInfo.GetList();
		TD_INTERACTION_ELEMENT td;
		td.nIElementID = 0;
		wcsncpy_s(td.szIEText, L"따라가기", _TRUNCATE);
		vecTDIElement.push_back(td);

		gvar.Game.NPCInteractionInfo.MenuType(MENU_FOLLOW_WARP);
		if (global.script)
			global.script->GetGlueGameEvent().OnGameEvent( "INTERACTION_NPC", "BEGIN");
	}

	return CR_TRUE;
}


MCommandResult XCmdHandler_Global::OnEntityWarp(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	MUID uidEntity;
	vec3 vPos, vDir;
	if (pCommand->GetParameter(&uidEntity,	0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vPos,		1, MPT_VECTOR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vDir,		2, MPT_VECTOR)==false) return CR_ERROR;

	XActor* pActor = gg.omgr->FindActor(uidEntity);
	if (pActor)
	{
		XModuleActorControl* pModuleActionControl = pActor->GetModuleActorControl();
		if (pModuleActionControl)
		{
			pModuleActionControl->OnWarp(vPos, vDir);
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Global::OnDialogStart(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	gvar.Game.DialogInfo.Clear();

	int& nSayTextID = gvar.Game.DialogInfo.GetSayTextID();
	vector<TD_DIALOG_SELECT_NODE>& vecTDDialogSelectNode = gvar.Game.DialogInfo.GetList();	
	if (pCommand->GetParameter(&nSayTextID, 0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDDialogSelectNode, 1)==false) return CR_ERROR;

	if (global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "DIALOG_NPC", "BEGIN");
	}

	XGetSoundNonPlayer()->PlayVoice( XGetInteractionInfo().InteractionTargetUID);

	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if (NULL == pModuleMyControl) return CR_FALSE;
	pModuleMyControl->DoActionInteraction(XGetInteractionInfo().InteractionTargetUID);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Global::OnDialogSelect(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	gvar.Game.DialogInfo.Clear();

	int& nSayTextID = gvar.Game.DialogInfo.GetSayTextID();
	vector<TD_DIALOG_SELECT_NODE>& vecTDDialogSelectNode = gvar.Game.DialogInfo.GetList();
	if (pCommand->GetParameter(&nSayTextID,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDDialogSelectNode, 1)==false) return CR_ERROR;

	_ASSERT(0 != vecTDDialogSelectNode.size());

	XNonPlayer* pNPC = gg.omgr->FindNPC(XGetInteractionInfo().InteractionTargetUID);
	if (NULL == pNPC) return CR_ERROR;
	XNPCInfo* pNPCInfo = pNPC->GetInfo();
	if (NULL == pNPCInfo) return CR_ERROR;
	if (NULL == global.script) return CR_ERROR;

	if (global.script)
		global.script->GetGlueGameEvent().OnGameEvent( "DIALOG_NPC", "BEGIN");

	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if (NULL == pModuleMyControl) return CR_FALSE;
	pModuleMyControl->DoActionInteraction(XGetInteractionInfo().InteractionTargetUID);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Global::OnTimeSynch(MCommand* pCommand, MCommandHandler* pHandler)
{
	uint32 nGlobalTime=0;
	if (pCommand->GetParameter(&nGlobalTime,		0, MPT_UINT)==false)	return CR_ERROR;

	global.system->GetTimer().SynchGlobalTime(nGlobalTime);

	return CR_TRUE;
}


MCommandResult XCmdHandler_Global::OnGMGod(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	bool bGod = false;
	if (pCommand->GetParameter(&bGod,		0, MPT_BOOL)==false)	return CR_ERROR;
	gvar.MyInfo.bGod = bGod;

	wstring strMessage;
	if (gvar.MyInfo.bGod)	strMessage = XGetStr( L"MSG_SETGODMODE");
	else					strMessage = XGetStr( L"MSG_RELEASEGODMODE");

	global.ui->OutputToConsole( strMessage.c_str());

	return CR_TRUE;
}

MCommandResult XCmdHandler_Global::OnGMGhost(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	bool bEnable = false;
	if (pCommand->GetParameter(&bEnable,		0, MPT_BOOL)==false)	return CR_ERROR;

	wstring strMessage;
	if (bEnable)			strMessage = XGetStr( L"MSG_SETGHOSTMODE");
	else					strMessage = XGetStr( L"MSG_RELEASEGHOSTMODE");

	global.ui->OutputToConsole( strMessage.c_str());

	return CR_TRUE;
}


MCommandResult XCmdHandler_Global::OnTestDebug(MCommand* pCommand, MCommandHandler* pHandler)
{
#ifdef _PUBLISH
	return CR_TRUE;
#endif

	int nParam = -1;
	wstring strKey, strText;

	if (pCommand->GetParameter(strKey,		0, MPT_WSTR)==false)	return CR_ERROR;
	if (pCommand->GetParameter(&nParam,		1, MPT_INT)==false)	return CR_ERROR;
	if (pCommand->GetParameter(strText,		2, MPT_WSTR)==false)	return CR_ERROR;
	
	XNetDebuger::Handle(strKey, nParam, strText, pCommand);

	return CR_TRUE;
}

bool IsPublishDefined()
{
#ifdef _PUBLISH
	return true;
#endif
	return false;
}

MCommandResult XCmdHandler_Global::OnAIDebugMessage(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (IsPublishDefined() || !XCONST::USE_NET_DEBUGGER)
	{
		return CR_TRUE;
	}

	MUID uidNPC;
	wstring strMessage;
	if (pCommand->GetParameter(&uidNPC,			0, MPT_UID)==false)	return CR_ERROR;
	if (pCommand->GetParameter(strMessage,		1, MPT_WSTR)==false) return CR_ERROR;

	XObject* pObject = gg.omgr->Find(uidNPC);
	if (pObject)
	{
		XModuleEffect* pModuleEffect = pObject->GetModuleEffect();
		if (pModuleEffect)
		{
			XCaptionEffectEventData * pEventData = new XCaptionEffectEventData;
			pEventData->m_strMessage = strMessage;
			pEventData->m_color = 0xFFFF00FF;
			pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_CAPTION_DEBUG, pEventData);
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Global::OnGMGetUID(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	if (pCommand->GetParameter(&uidPlayer,		0, MPT_UID)==false)	return CR_ERROR;

	wchar_t text[1024]=L"";
	swprintf_s(text, L"요청하신 플레이어의 UID는 %I64u", uidPlayer);
	global.ui->OutputToConsole(text);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Global::OnDebugCombatcalc(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strType, strMessage;

	if (pCommand->GetParameter(strType,		0, MPT_WSTR)==false)	return CR_ERROR;
	if (pCommand->GetParameter(strMessage,	1, MPT_WSTR)==false)	return CR_ERROR;

	XNetDebuger::DebugCombatCalc(strType, strMessage);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Global::OnDebugNPCNetLog(MCommand* pCommand, MCommandHandler* pHandler)
{
#ifdef _PUBLISH
	return CR_TRUE;
#endif

	MUID nNPCUID;
	vector<TD_DEBUG_NPCLOG> vecTDDebugNPCLog;
	if (pCommand->GetParameter(&nNPCUID,			0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDDebugNPCLog, 1)==false) return CR_ERROR;

	if (global.ui)
	{
		global.ui->NPCLog(vecTDDebugNPCLog);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Global::OnPlayBGM(MCommand* pCommand, MCommandHandler* pHandler)
{
	wstring strSoundName;
	if (pCommand->GetParameter( strSoundName, 0, MPT_WSTR)==false)		return CR_ERROR;

	if ( global.sound)
		global.sound->SetDefalutBGM( MLocale::ConvUTF16ToAnsi(strSoundName.c_str()).c_str());

	return CR_TRUE;
}

MCommandResult XCmdHandler_Global::OnGameGuardAuthReq(MCommand* pCommand, MCommandHandler* pHandler)
{
#if defined( XANTIHACK)  &&  defined( _KOREA)

	XAntiHackAuthInfoGameGuardKor _auth_info;
	if ( pCommand->GetParameter( &_auth_info.m_AuthInfo, 0, MPT_SINGLE_BLOB) == false)		return CR_ERROR;

	XGetAntiHack()->CheckAuthority( &_auth_info);

#endif

	return CR_TRUE;
}
