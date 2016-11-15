#include "stdafx.h"
#include "XCmdHandler_Field.h"
#include "XPost_Local.h"
#include "XGame.h"
#include "XWorld.h"
#include "XGameFrameID.h"
#include "XModuleNetControl.h"
#include "XStrings.h"
#include "XBaseApplication.h"
#include "XMyPlayer.h"
#include "XWhoISRequester.h"
#include "XNetwork.h"
#include "XInteractionInfo.h"
#include "XFieldInfoHelper.h"
#include "XFieldChanger.h"


XCmdHandler_Field::XCmdHandler_Field(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_FIELD_CHANGE_FIELD,				OnChangeField);
	SetCmdHandler(MC_DYNAMIC_FIELD_EXPIRING,			OnDynamicFieldExpiring);
	SetCmdHandler(MC_DYNAMIC_FIELD_CANCEL_EXPIRE,		OnDynamicFieldCancelExpire);

	SetCmdHandler(MC_FIELD_CHANNEL_LIST,				OnChannelList);
	SetCmdHandler(MC_FIELD_PREPARE_CHANGE_CHANNEL_RES,	OnChannelChangePrepareRes);
	SetCmdHandler(MC_FIELD_CANCEL_CHANGE_FIELD,			OnCancelChangeField);
	

	SetCmdHandler(MC_FIELD_PREPARING_TO_LOAD,			OnPreparingToLoad);
	SetCmdHandler(MC_FIELD_START_GAME,					OnStartGame);
	
	SetCmdHandler(MC_FIELD_SET_TIME,					OnSetTime);
	SetCmdHandler(MC_FIELD_SET_WEATHER,					OnSetWeather);
	SetCmdHandler(MC_FIELD_SET_TIME_WEATHER,			OnSetTimeWeather);
	SetCmdHandler(MC_LOCAL_TEST_CHANGE_ENV,				OnSetTimeWeather);		// 로컬 테스트 커맨드이다.

	SetCmdHandler(MC_FIELD_IN_PLAYER,					OnInPlayer);
	SetCmdHandler(MC_FIELD_OUT_PLAYER,					OnOutPlayer);
	SetCmdHandler(MC_FIELD_IN_NPC,						OnInNPC);
	SetCmdHandler(MC_FIELD_OUT_NPC,						OnOutNPC);
	SetCmdHandler(MC_FIELD_IN_BPART,					OnInBPart);
	SetCmdHandler(MC_FIELD_OUT_BPART,					OnOutBPart);
	SetCmdHandler(MC_FIELD_IN_BUFFENTITY,				OnInMagicArea);
	SetCmdHandler(MC_FIELD_OUT_BUFFENTITY,				OnOutMagicArea);
	SetCmdHandler(MC_FIELD_TRAP_TRIGGERED,				OnTrapTriggered);
}

MCommandResult XCmdHandler_Field::OnChangeField(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nFieldID;
	int nChannelID;
	int nIntroCutsceneID;
	vec3 vPos, vDir;
	int8 nGameTime = 0;
	int8 nWeatherType = 0;
	if (pCommand->GetParameter(&nFieldID,			0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nChannelID,			1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vPos,				2, MPT_VEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&vDir,				3, MPT_VEC)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nIntroCutsceneID,	4, MPT_INT)==false) return CR_ERROR;

	XFieldChanger fieldChanger;
	fieldChanger.Change(nFieldID, nChannelID, vPos, vDir, nIntroCutsceneID);

	gvar.Game.Init_ForChangeField();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnDynamicFieldExpiring(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_TRUE;

	uint32 nServerLocalTime = 0;
	uint32 nExpiredTime = 0;

	if (pCommand->GetParameter(&nExpiredTime,		0, MPT_UINT)==false) return CR_ERROR;

	gvar.World.nDynamicFieldExpiredTime = nExpiredTime;

	if ( global.script)
	{
		uint32 nCurr = global.system->GetTimer().GetNowGlobalTime();
		if ( nCurr > nExpiredTime)
		{
			global.script->GetGlueGameEvent().OnGameEvent( "MISSIONPROG", "END");
		}
		else
		{
			uint32 nRemained = nExpiredTime - nCurr;
			if ( nRemained > 1000)	global.script->GetGlueGameEvent().OnGameEvent( "MISSIONPROG", "TIMELIMIT", nRemained);
			else					global.script->GetGlueGameEvent().OnGameEvent( "MISSIONPROG", "END");
		}
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnDynamicFieldCancelExpire(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_TRUE;

	gvar.World.nDynamicFieldExpiredTime = 0;

	if(global.script)
		global.script->GetGlueGameEvent().OnGameEvent( "MISSIONPROG", "END");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnSetTime(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_TRUE;

	int8 nGameTime = 0;

	if (pCommand->GetParameter(&nGameTime,		0, MPT_CHAR)==false) return CR_ERROR;

	GAME_TIME_TYPE nType = (GAME_TIME_TYPE)nGameTime;


	if ((gg.currentgamestate)
		&& (gg.currentgamestate->GetWorld()))
	{
		// fixed time이면 무시
		if (gg.currentgamestate->GetWorld()->GetInfo())
		{
			if (gg.currentgamestate->GetWorld()->GetInfo()->m_TimeInfo.bFixed)
			{
				return CR_TRUE;
			}
		}

		if (gvar.Game.nLocalTime == TIME_INVALID)
		{
			gg.currentgamestate->GetWorld()->OnCmdSetTime(nType, true, GAME_ENVIRONMENT_FADING_TIME);
		}

	}

	global.ui->OnSystemMsg( XGetStr( L"MSG_CHANGEDTIME"), FSParam(XStrings::GameTime(GAME_TIME_TYPE(nGameTime))));

	gvar.World.nGameTime = (GAME_TIME_TYPE)nGameTime;
//	global.ui->MiniMapInfoRefresh();

	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "DAYTIME");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnSetWeather(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.currentgamestate == NULL) return CR_TRUE;

	int8 nWeather = 0;

	if (pCommand->GetParameter(&nWeather,		0, MPT_CHAR)==false) return CR_ERROR;

	if ((gg.currentgamestate) 
		&& (gg.currentgamestate->GetWorld())
		&& (gvar.Game.nLocalWeather == WEATHER_INVALID))
	{
		gg.currentgamestate->GetWorld()->OnCmdSetWeather(GAME_WEATHER_TYPE(nWeather), true, GAME_ENVIRONMENT_FADING_TIME);
	}

	global.ui->OnSystemMsg( XGetStr( L"MSG_CHANGEDWEATHER"), FSParam(XStrings::Weather(GAME_WEATHER_TYPE(nWeather))));

	gvar.World.nWeather = (GAME_WEATHER_TYPE)nWeather;
//	global.ui->MiniMapInfoRefresh();

	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "WEATHER");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnSetTimeWeather(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.currentgamestate == NULL) return CR_TRUE;

	int8 nGameTime = 0;
	int8 nWeather = 0;

	if (pCommand->GetParameter(&nGameTime,		0, MPT_CHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nWeather,		1, MPT_CHAR)==false) return CR_ERROR;

	gg.currentgamestate->ChangeTimeWeather((GAME_TIME_TYPE)nGameTime, (GAME_WEATHER_TYPE)nWeather);

	global.ui->OnSystemMsg( XGetStr( L"MSG_CHANGEDWEATHER"),
		FSParam(XStrings::Weather(GAME_WEATHER_TYPE(nWeather)), XStrings::GameTime(GAME_TIME_TYPE(nGameTime))));

	gvar.World.nGameTime = (GAME_TIME_TYPE)nGameTime;
	gvar.World.nWeather = (GAME_WEATHER_TYPE)nWeather;
//	global.ui->MiniMapInfoRefresh();

	if ( global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "DAYTIME");
		global.script->GetGlueGameEvent().OnGameEvent( "ENVIRONMENT", "WEATHER");
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnChannelList(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_TRUE;

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_BLOB) return CR_ERROR;

	
	vector< int>	vChannelList;
	vector< int>	vChannelStateList;
	if ( pCommand->GetBlob( vChannelList, 0) == false)		return CR_ERROR;
	if ( pCommand->GetBlob( vChannelStateList, 1) == false)		return CR_ERROR;

	gvar.World.ChannelList.clear();

	int _size = (int)vChannelList.size();
	for ( int i = 0;  i < _size;  i++)
	{
		int nID = vChannelList[ i];
		int nState = vChannelStateList[i];

		gvar.World.ChannelList.push_back( pair< int, int>( nID, nState));
	}

	if(global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "CHANNEL", "RECIEVED_LIST", 0);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnChannelChangePrepareRes(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nResult;
	if (pCommand->GetParameter(&nResult,		0, MPT_INT)==false) return CR_ERROR;

	if (gg.game == NULL) return CR_TRUE;

	if (nResult == CR_SUCCESS)
	{
		gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionChangeChannel();
	}
	else
	{
		if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "CHANNEL", "CHANGED", -1);

		const wchar_t* szMsg = XCmdResultStr(nResult);
		if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "ERROR_MESSAGEBOX", MLocale::ConvUTF16ToAnsi(szMsg));
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnCancelChangeField(MCommand* pCommand, MCommandHandler* pHandler)
{
	// 인자 없음
	 
	if (gg.game == NULL)	return CR_TRUE;

	gg.currentgamestate->PlayFieldBGMSound();

	if (global.app) global.app->MessageToFrame(L"CancelChangeField");
	
	if (global.script)
	{
		global.ui->OnPresentationUpper( XGetStr( L"MSG_FAILEDMOVEFIELD"));
	}

	gvar.Game.bGameStart = true;

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnPreparingToLoad(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nNextFieldID;
	if (pCommand->GetParameter(&nNextFieldID,		0, MPT_INT)==false) return CR_ERROR;

	//만약 0이나 -1 같이 Invalid 값이 오면 창현씨께 알려주세요.
	_ASSERT(nNextFieldID != 0 || nNextFieldID != -1);

	gvar.Temp.nNextFieldID = nNextFieldID;

	if (global.app) global.app->MessageToFrame(L"Prepare");

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnStartGame(MCommand* pCommand, MCommandHandler* pHandler)
{
	// TODO : 로딩화면 끝내고 게임 활성화 시작
	if ( global.script)	global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "EXPERIENCE");

	gvar.Game.pMyPlayer->CheckDie();

	gg.currentgamestate->PlayFieldBGMSound();

	if (global.app) global.app->MessageToFrame(L"Field_StartGame");

	gg.currentgamestate->FieldStartMessage();

	gvar.Game.bGameStart = true;

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnInPlayer(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.currentgamestate == NULL) return CR_TRUE;

	TD_UPDATE_CACHE_PLAYER playerInfo;
	TD_PLAYER_FEATURE_TATTOO playerFeatrueTattoo;
	if (false == pCommand->GetSingleBlob(playerInfo, 0)) return CR_ERROR;
	if (false == pCommand->GetSingleBlob(playerFeatrueTattoo, 1)) return CR_ERROR;

	MUID uidPlayer = playerInfo.uid;
	if (uidPlayer == XGetMyUID()) return CR_TRUE;	

	gg.currentgamestate->InPlayer(uidPlayer, &playerInfo, playerFeatrueTattoo.IsValid() ? &playerFeatrueTattoo : NULL);

	if (global.net && global.net->GetNetClient())
	{
		UIID nPlayerUIID = playerInfo.nUIID;

		XWhoIsRequester* pWhoIsRequester = global.net->GetNetClient()->GetWhoIsRequester();
		pWhoIsRequester->Erase(nPlayerUIID);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnOutPlayer(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.currentgamestate == NULL) return CR_TRUE;

	UIID nPlayerUIID;
	if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (nPlayerUIID == XGetMyUIID()) return CR_TRUE;	

	gg.currentgamestate->OutPlayer(nPlayerUIID);

	if(XGetInteractionInfo().GetTargetUIID() == nPlayerUIID)
		XGetInteractionInfo().stTargetActorInfo.bLockOnTarget = false;

	gvar.Game.EnemyInfo.ClearEnemyInfo(nPlayerUIID);
	if(global.script)	global.script->GetGlueGameEvent().OnEnemyInfoRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnInNPC(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.currentgamestate == NULL) return CR_TRUE;

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_ERROR;
	TD_UPDATE_CACHE_NPC* pNPCInfo = (TD_UPDATE_CACHE_NPC*)pParam->GetPointer();

	mmlog("XCmdHandler_Field::OnInNPC - %.1f %.1f %.1f\n", pNPCInfo->vPos.x, pNPCInfo->vPos.y, pNPCInfo->vPos.z);

	gg.currentgamestate->InNPCs(pNPCInfo, 1);


	if (global.net && global.net->GetNetClient())
	{
		XWhoIsRequester* pWhoIsRequester = global.net->GetNetClient()->GetWhoIsRequester();
		pWhoIsRequester->Erase(pNPCInfo->nUIID);
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnOutNPC(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.currentgamestate == NULL) return CR_TRUE;

	UIID nNPCUIID;
	if (pCommand->GetParameter(&nNPCUIID,	0, MPT_USHORT)==false) return CR_ERROR;

	gg.currentgamestate->OutNPC(nNPCUIID);

	gvar.Game.vecNPCIcon.erase(nNPCUIID);

	if(XGetInteractionInfo().GetTargetUIID() == nNPCUIID)
		XGetInteractionInfo().stTargetActorInfo.bLockOnTarget = false;

	gvar.Game.EnemyInfo.ClearEnemyInfo(nNPCUIID);
	if(global.script)	global.script->GetGlueGameEvent().OnEnemyInfoRefresh();

	return CR_TRUE;
}


MCommandResult XCmdHandler_Field::OnInBPart(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.currentgamestate == NULL) return CR_TRUE;

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_ERROR;
	TD_UPDATE_CACHE_PBART* pBPartInfo = (TD_UPDATE_CACHE_PBART*)pParam->GetPointer();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnOutBPart(MCommand* pCommand, MCommandHandler* pHandler)
{
	return OnOutNPC(pCommand, pHandler);
}

MCommandResult XCmdHandler_Field::OnInMagicArea(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.currentgamestate == NULL) return CR_TRUE;

	MCommandParameter* pParam = pCommand->GetParameter(0);
	if(pParam->GetType()!=MPT_SINGLE_BLOB) return CR_ERROR;
	TD_UPDATE_CACHE_BUFFENTITY* pMagicAreaInfo = (TD_UPDATE_CACHE_BUFFENTITY*)pParam->GetPointer();

	gg.currentgamestate->InMagicArea(pMagicAreaInfo);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnOutMagicArea(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.currentgamestate == NULL) return CR_TRUE;

	MUID uidMagicArea;
	if (pCommand->GetParameter(&uidMagicArea,	0, MPT_UID)==false) return CR_ERROR;

	gg.currentgamestate->OutMagicArea(uidMagicArea);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Field::OnTrapTriggered(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.currentgamestate == NULL) return CR_TRUE;

	MUID uidMagicArea;
	if (pCommand->GetParameter(&uidMagicArea,	0, MPT_UID)==false) return CR_ERROR;

	gg.currentgamestate->TrapTriggered(uidMagicArea);

	return CR_TRUE;
}