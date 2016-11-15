#include "stdafx.h"
#include "GCmdHandler_GM.h"
#include "MCommand.h"
#include "GCommandTable.h"
#include "GField.h"
#include "GEntityPlayer.h"
#include "GConst.h"
#include "GEntityNPC.h"
#include "GBirdMonitor.h"
#include "GMessageID.h"
#include "GModuleAI.h"
#include "GReporter.h"
#include "GTrainingSystem.h"
#include "GNPCMgr.h"
#include "GCommandCenter.h"
#include "GQuestSystem.h"
#include "GReloader.h"
#include "GJobRunner.h"
#include "GBuffInfo.h"
#include "GModuleBuff.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GCommand.h"
#include "GEnvManager.h"
#include "GModuleCombat.h"
#include "GScriptManager.h"
#include "GTestSystem.h"
#include "GPlayerField.h"
#include "GMath.h"
#include "GPlayerSystem.h"
#include "GGmSetMeHandler.h"
#include "GItemDurabilityUpdater.h"
#include "CSCharHelper.h"
#include "GNPCLoot.h"
#include "GPlayerCRTLogger.h"
#include "GFactionSystem.h"
#include "GRelationChecker.h"
#include "GServer.h"
#include "GActorCooltimeChecker.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "GActorHateTracker.h"
#include "GNPCBParts.h"
#include "GItemAdder.h"
#include "GItemHolder.h"
#include "GHateTable.h"
#include "GRebirthSystem.h"
#include "GItemRemover.h"
#include "GMasterServerFacade.h"
#include "GServerPlayerRouter.h"
#include "GQuestGiver.h"
#include "GQuestRewarder.h"
#include "GQuestCompleter.h"
#include "GQuestFailer.h"
#include "GQuestReseter.h"
#include "GGMSetNPCHandler.h"
#include "GItemDyer.h"
#include "GPlayerObject.h"
#include "GFieldInfoMgr.h"
#include "GSharedField.h"
#include "GClientMoveServerRouter.h"
#include "CCommandResultTable.h"
#include "GServerModeChanger.h"
#include "GGMExpoHandler.h"


GCmdHandler_GM::GCmdHandler_GM(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	// 개발전용
	SetCmdHandler(MC_GM_REQUEST_DESPAWN,		OnRequestDespawn);
	SetCmdHandler(MC_GM_DESPAWNBYID_REQ,		OnRequestDespawnByID);
	SetCmdHandler(MC_GM_AI_RUNNING_REQ,			OnRequestGMAIRunning);
	SetCmdHandler(MC_GM_NPC_AI_RUNNING_REQ,		OnRequestNPCAIRunning);
	SetCmdHandler(MC_GM_AI_USE_TALENT_REQ,		OnRequestGMAICommand);
	SetCmdHandler(MC_GM_AI_SET_MONITOR_TARGET,	OnRequestGMAISetMonitorNPC);
	SetCmdHandler(MC_REQUEST_DEBUG_STRING,		OnRequestDebugString);
	SetCmdHandler(MC_GM_GET_PLAYERUID_REQ,		OnRequestGMGetPlayerUID);
	SetCmdHandler(MC_GM_REPORT_TARGET_ENTITY,	OnRequestReportTargetEntity);

	SetCmdHandler(MC_GM_MOVE_TO_MYSPOT_REQ,		OnRequestGMMoveToMySpot);

	SetCmdHandler(MC_GM_ENABLE_ICHECK_REQ,		OnRequestGMEnableICheck);
	SetCmdHandler(MC_GM_DISABLE_ICHECK_REQ,		OnRequestGMDisableICheck);

	SetCmdHandler(MC_GM_CLEAR_INVENTORY_REQ,	OnRequestGMClearInventory);

	SetCmdHandler(MC_GM_SET_ITEM_REQ,			OnRequestGMSetItem);
	SetCmdHandler(MC_GM_INSERT_BUFF_REQ,		OnRequestGMInsertBuff);
	SetCmdHandler(MC_GM_DELETE_BUFF_REQ,		OnRequestGMDeleteBuff);

	SetCmdHandler(MC_GM_QUEST_GIVE_REQ,			OnRequestGMQuestGive);
	SetCmdHandler(MC_GM_QUEST_COMPLETE_REQ,		OnRequestGMQuestComplete);
	SetCmdHandler(MC_GM_QUEST_RESET_REQ,		OnRequestGMQuestReset);	
	SetCmdHandler(MC_GM_QUEST_RESET_ALL_REQ,	OnRequestGMQuestResetAll);
	SetCmdHandler(MC_GM_QUEST_VAR_REQ,			OnRequestGMQuestVar);	
	SetCmdHandler(MC_GM_QUEST_FAIL_REQ,			OnRequestGMQuestFail);
	SetCmdHandler(MC_GM_QUEST_REWARD_REQ,		OnRequestGMQuestReward);	

	SetCmdHandler(MC_GM_SHOW_ENEMY_UID_REQ,		OnRequestGMShowEnemyUID);

	SetCmdHandler(MC_GM_BREAK_PART_REQ,			OnRequestGMBreakPart);
	SetCmdHandler(MC_GM_RANGE_BREAK_PART_REQ,	OnRequestGMRangeBreakPart);

	SetCmdHandler(MC_GM_LOG_CRT_INSERT_REQ,		OnRequestGMLogCRTInsert);
	SetCmdHandler(MC_GM_LOG_CRT_DELETE_REQ,		OnRequestGMLogCRTDelete);
	SetCmdHandler(MC_GM_QUERY_MULTILOGIN_REQ,	OnRequestGMQueryMultiLogin);
	

	SetCmdHandler(MC_GM_FACTION_INCREASE_REQ,	OnRequestGMFactionIncrease);
	SetCmdHandler(MC_GM_FACTION_DECREASE_REQ,	OnRequestGMFactionDecrease);
	SetCmdHandler(MC_GM_FACTION_RESET_REQ,		OnRequestGMFactionReset);

	SetCmdHandler(MC_GM_CHANGE_SERVER_MODE_REQ,	OnRequestGMChangeServerMode);
	SetCmdHandler(MC_GM_RESET_EXPO_CHARACTERS_REQ,	OnRequestGMResetExpoCharacters);

	SetCmdHandler(MC_GM_DYE_REQ,				OnRequestGMDye);

	SetCmdHandler(MC_DEBUG_START_COMMAND_PROFILE,	OnRequestStartCommandProfile);
	SetCmdHandler(MC_DEBUG_DUMP_COMMAND_PROFILE,	OnRequestDumpCommandProfile);
	SetCmdHandler(MC_DEBUG_END_COMMAND_PROFILE,		OnRequestEndCommandProfile);

	SetCmdHandler(MC_GM_SERVER_DUMP_REQ,			OnRequestServerDump);

	


	// GM용
	SetCmdHandler(MC_GM_REBIRTH_REQ,			OnRequestGMRebirth);
	SetCmdHandler(MC_GM_REQUEST_SPAWN,			OnRequestSpawn);
	SetCmdHandler(MC_GM_ITEM_GIVE_REQ,			OnRequestGMItemGive);
	SetCmdHandler(MC_GM_RESET_COOLTIME_REQ,		OnRequestGMResetCoolTime);

	SetCmdHandler(MC_GM_MOVE_REQ,				OnRequestGMMove);
	SetCmdHandler(MC_GM_MOVE_TO_PLAYER_REQ,		OnRequestGMMoveToPlayer);
	SetCmdHandler(MC_GM_MOVE_TO_NPC_REQ,		OnRequestGMMoveToNPC);

	SetCmdHandler(MC_GM_SUMMON_REQ,				OnRequestGMSummon);

	SetCmdHandler(MC_GM_GOD_REQ,				OnRequestGMGod);
	SetCmdHandler(MC_GM_CHANGE_WEATHER_REQ,		OnRequestGMChangeWeather);
	SetCmdHandler(MC_GM_CHANGE_TIME_REQ,		OnRequestGMChangeTime);	
	SetCmdHandler(MC_GM_SET_ME_REQ,				OnRequestGMSetMe);	
	SetCmdHandler(MC_GM_SET_NPC_REQ,			OnRequestGMSetNPC);

	SetCmdHandler(MC_GM_GHOST_REQ,				OnRequestGMGhost);	
	SetCmdHandler(MC_GM_RANGE_KILL_NPC_REQ,		OnRequestGMRangeKillEntity);
	SetCmdHandler(MC_GM_KILL_ENTITY_REQ,		OnRequestGMKillEntity);
	SetCmdHandler(MC_GM_REGEN_REQ,				OnRequestGMRegen);

	// GameGuard - Auth	

	// test
	SetCmdHandler(MC_DEBUG_ECHO_REQ,			OnRequestDebugEchoReq);
}

MCommandResult GCmdHandler_GM::OnRequestSpawn(MCommand* pCmd, MCommandHandler* pHandler)
{
	int nNPCID = 0;
	int nCount = 0;
	vec3 vSpawnPos;

	if (pCmd->GetParameter(&nNPCID,0, MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nCount,1, MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&vSpawnPos,2, MPT_VEC)==false) return CR_FALSE;

	MUID uidPlayer = pCmd->GetSenderUID();
	if (uidPlayer.IsInvalid()) return CR_FALSE;

	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsGM(pEntityPlayer) == false) return CR_TRUE;

	GField* pField = pEntityPlayer->GetField();
	if (pField==NULL) return CR_FALSE;

	nCount = min(nCount, 100);	// 최대 100개

	GVectorMUID vecNPCUID;
	for (int i = 0; i < nCount; i++)
	{
		vec3 pos;
		if (vSpawnPos == vec3::ZERO)
		{
			pos = pEntityPlayer->GetPos();
			pos.x += GMath::RandomNumber(-100.0f, 100.0f);
			pos.y += GMath::RandomNumber(-100.0f, 100.0f);
		}
		else
		{
			pos = vSpawnPos;
			pos.z = pEntityPlayer->GetPos().z;
		}

		GEntityNPC* pNPC = pField->SpawnNPC(nNPCID, NULL, pos, vec3(0.0f,1.0f,0.0f));
		if (NULL == pNPC) continue;

		vecNPCUID.Vector().push_back(pNPC->GetUID());		
	}

	MCommand* pNewCmd = MakeNewCommand(MC_GM_SPAWN, 1, NEW_BLOB(vecNPCUID.Vector()));
	pEntityPlayer->RouteToMe(pNewCmd);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestDespawn(MCommand* pCmd, MCommandHandler* pHandler)
{
	float fRange;
	if (pCmd->GetParameter(&fRange, 0, MPT_FLOAT) == false) return CR_FALSE;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;
	if (IsGM(pPlayer) == false) return CR_TRUE;

	vector<GEntityNPC*> vecNeighborNPC;
	pPlayer->GetNeighborNPC(fRange, vecNeighborNPC);

	for each (GEntityNPC* pNeighborNPC in vecNeighborNPC)
	{
		if (true == pNeighborNPC->IsStaticSpawned()) continue;

		pNeighborNPC->DespawnNow(true);
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestDespawnByID(MCommand* pCmd, MCommandHandler* pHandler)
{
	int nNPCID;
	float fRange;

	if (pCmd->GetParameter(&nNPCID,0, MPT_INT)==false) return CR_FALSE;	
	if (pCmd->GetParameter(&fRange, 1, MPT_FLOAT) == false) return CR_FALSE;

	MUID uidPlayer = pCmd->GetSenderUID();
	if (uidPlayer.IsInvalid()) return CR_FALSE;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (NULL == pPlayer) return CR_FALSE;
	if (IsGM(pPlayer) == false) return CR_TRUE;

	vector<GEntityNPC*> vecNeighborNPC;
	pPlayer->GetNeighborNPC(fRange, vecNeighborNPC);
	for each (GEntityNPC* pNPC in vecNeighborNPC)
	{
		if (nNPCID != pNPC->GetID()) continue;

		pNPC->DespawnNow(true);
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMAIRunning(MCommand* pCmd, MCommandHandler* pHandler)
{
	if (IsDeveloper(pCmd) == false) return CR_TRUE;

	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (!pEntityPlayer)
		return CR_TRUE;

	gsys.pTestSystem->SwitchAIRunning(pEntityPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestNPCAIRunning(MCommand* pCmd, MCommandHandler* pHandler)
{
	if (IsDeveloper(pCmd) == false) return CR_TRUE;

	gsys.pTestSystem->RequestNPCAIRunning(pCmd);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMAICommand(MCommand* pCmd, MCommandHandler* pHandler)
{
	if (IsDeveloper(pCmd) == false) return CR_TRUE;

	gsys.pTestSystem->RequestAICommand(pCmd);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMAISetMonitorNPC(MCommand* pCmd, MCommandHandler* pHandler)
{
	if (IsDeveloper(pCmd) == false) return CR_TRUE;

	gsys.pTestSystem->RequestAISetMonitorNPC(pCmd);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestDebugString(MCommand* pCmd, MCommandHandler* pHandler)
{
	if (IsGM(pCmd) == false) return CR_TRUE;

	gsys.pTestSystem->RequestDebugString(pCmd);

	return CR_TRUE;
}


MCommandResult GCmdHandler_GM::OnRequestGMGetPlayerUID(MCommand* pCmd, MCommandHandler* pHandler)
{
	if (IsDeveloper(pCmd) == false) return CR_TRUE;

	gsys.pTestSystem->RequestGMGetPlayerUID(pCmd);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestReportTargetEntity(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsDeveloper(pEntityPlayer) == false) return CR_TRUE;

	MUID uidTarget;
	int nTalentID = 0;

	if (pCmd->GetParameter(&uidTarget,		0, MPT_UID)==false) return CR_FALSE;


	GReporter	reporter;
	bool bRet = 
		reporter.ReportTargetEntity(pEntityPlayer->GetFieldUID(), uidTarget);

	dlog("Report %s (Field: %I64d, NPC: %I64d)\n", bRet?"Complete":"Failed", pEntityPlayer->GetFieldUID(), uidTarget);


	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMMove(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsGM(pEntityPlayer) == false) return CR_TRUE;

	int nFieldID=0;
	vec3 vPosition, vDir(0,1,0);

	if (pCmd->GetParameter(&nFieldID,	0, MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&vPosition,	1, MPT_VEC)==false) return CR_FALSE;

	if (NULL == gmgr.pFieldInfoMgr->Find(nFieldID))
	{
		pEntityPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_INVALID_FIELD_ID);
		return CR_TRUE;
	}

	pEntityPlayer->GetPlayerField().Gate(nFieldID, vPosition, vDir);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMMoveToMySpot(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsGM(pEntityPlayer) == false) return CR_TRUE;

	//int nFieldID = m_pPlayerInfo->nLastField;
	//vec3 vPosition = m_pPlayerInfo->vLastPos, vDir(0,1,0);

	// 서버에 세팅된 값으로 이동
	const MARKER_INFO* pMarkerInfo = gsys.pPlayerSystem->GetSoulBindingMarker(pEntityPlayer->GetPlayerInfo()->nSoulBindingID);
	if (NULL == pMarkerInfo) return CR_FALSE;

	pEntityPlayer->GetPlayerField().Gate(pMarkerInfo->nFieldID, pMarkerInfo->vPoint, pMarkerInfo->vDir);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMMoveToPlayer(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (IsGM(pPlayer) == false) return CR_TRUE;

	wstring strTarPlayerName;
	if (pCmd->GetParameter(strTarPlayerName, 0, MPT_WSTR)==false) return CR_FALSE;

	pPlayer->GetPlayerField().GateToPlayer(strTarPlayerName);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMMoveToNPC(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsGM(pEntityPlayer) == false) return CR_TRUE;

	int nNPCID;
	int nFieldID;
	if (pCmd->GetParameter(&nNPCID, 0, MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nFieldID, 1, MPT_INT)==false) return CR_FALSE;

	vec3 vPos = gmgr.pFieldInfoMgr->FindNPCPos(nNPCID, nFieldID); 
	if (vec3::ZERO == vPos) return CR_FALSE;

	pEntityPlayer->GetPlayerField().Gate(nFieldID, vPos, vec3(0,1,0));

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMSummon(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (IsGM(pPlayer) == false) return CR_TRUE;

	wstring strTarPlayerName;	
	if (pCmd->GetParameter(strTarPlayerName, 0, MPT_WSTR)==false) return CR_FALSE;

	pPlayer->GetPlayerField().Summon(strTarPlayerName);

	
	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMRebirth(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsGM(pEntityPlayer) == false) return CR_TRUE;

	vec3 vPos;
	if (pCmd->GetParameter(&vPos, 0, MPT_VEC)==false) return CR_FALSE;

	gsys.pRebirthSystem->RebirthForGM(pEntityPlayer);

	return CR_TRUE;	
}

MCommandResult GCmdHandler_GM::OnRequestGMGod(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsGM(pEntityPlayer) == false) return CR_TRUE;

	pEntityPlayer->GetPlayerInfo()->SetGod(!pEntityPlayer->GetPlayerInfo()->IsGod());

	if (GConst::GODMODE_WITH_INVINCIBLE)
	{
		if (pEntityPlayer->GetPlayerInfo()->IsGod())
		{
			pEntityPlayer->GetModuleBuff()->GainInvincibleBuff_Infinity();
		}
		else
		{
			pEntityPlayer->GetModuleBuff()->LostInvincibleBuff();
		}
	}
	
	MCommand* pNewCommand = MakeNewCommand(MC_GM_GOD, pEntityPlayer->GetUID(), 1, NEW_BOOL(pEntityPlayer->GetPlayerInfo()->IsGod()));
	gsys.pCommandCenter->PostCommand(pNewCommand);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMEnableICheck(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsDeveloper(pEntityPlayer) == false) return CR_TRUE;

	pEntityPlayer->EnableICheck();

	
	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMDisableICheck(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsDeveloper(pEntityPlayer) == false) return CR_TRUE;

	pEntityPlayer->DisableICheck();
	
	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMGhost(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsGM(pEntityPlayer) == false) return CR_TRUE;

	{
		if (!pEntityPlayer->GetPlayerInfo()->m_bGhost)
		{
			MCommand* pNewCmd = MakeNewCommand(MC_ACTION_HIDE, 1, 
				NEW_UID(pEntityPlayer->GetUID()));
			pEntityPlayer->RouteToThisCellExceptMe(pNewCmd);

			pEntityPlayer->GetPlayerInfo()->m_bGhost = true;
		}
		else
		{
			pEntityPlayer->GetPlayerInfo()->m_bGhost = false;
			TD_UPDATE_CACHE_PLAYER td_player_info;
			pEntityPlayer->MakeTDCacheInfo(td_player_info);

			MCommand* pNewCmd = MakeNewCommand(MC_FIELD_IN_PLAYER, 1, 
				NEW_SINGLE_BLOB(&td_player_info, sizeof(TD_UPDATE_CACHE_PLAYER)));
			pEntityPlayer->RouteToThisCellExceptMe(pNewCmd);
		}		
	}

	MCommand* pNewCommand = MakeNewCommand(MC_GM_GHOST, pEntityPlayer->GetUID(), 1, NEW_BOOL(pEntityPlayer->GetPlayerInfo()->m_bGhost));
	gsys.pCommandCenter->PostCommand(pNewCommand);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMChangeWeather(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsGM(pEntityPlayer) == false) return CR_TRUE;

	int weather_type=0;
	if (pCmd->GetParameter(&weather_type,	0, MPT_INT)==false) return CR_FALSE;

	if (weather_type < 0 || weather_type >= MAX_WEATHER_TYPE) return CR_FALSE;
	GAME_WEATHER_TYPE nWeatherType = (GAME_WEATHER_TYPE)weather_type;

	GField* pField = pEntityPlayer->GetField();
	if (NULL == pField) return CR_FALSE;

	pField->ChangeWeather(nWeatherType);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMChangeTime(MCommand* pCmd, MCommandHandler* pHandler)
{
	if (IsGM(pCmd) == false) return CR_TRUE;

	int time_type=0;
	if (pCmd->GetParameter(&time_type,	0, MPT_INT)==false) return CR_FALSE;

	GAME_TIME_TYPE nTimeType;
	if (time_type >= 0 && time_type < MAX_TIME_TYPE)
	{
		nTimeType = (GAME_TIME_TYPE)time_type;
	}
	else
	{
		GAME_TIME_TYPE nCurrTime = gmgr.pEnvManager->GetCurrentTime();
		if (nCurrTime == TIME_NIGHT) nTimeType = TIME_DAWN;
		else nTimeType = (GAME_TIME_TYPE)(nCurrTime+1);
	}

	gmgr.pEnvManager->ChangeTimeForced(nTimeType);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMItemGive(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsGM(pEntityPlayer) == false) return CR_TRUE;

	int nItemID = 0;
	int nQuantity = 0;

	if (pCmd->GetParameter(&nItemID, 0, MPT_INT) == false) return CR_FALSE;
	if (pCmd->GetParameter(&nQuantity, 1, MPT_INT) == false) return CR_FALSE;

	gsys.pItemSystem->GetAdder().Add(pEntityPlayer->GetUID(), nItemID, nQuantity);

	return CR_TRUE;
}


MCommandResult GCmdHandler_GM::OnRequestGMClearInventory(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsDeveloper(pEntityPlayer) == false) return CR_TRUE;

	vector <GItem*> vecItem;
	pEntityPlayer->GetItemHolder()->CollectAllItem(SLOTTYPE_INVENTORY, vecItem);

	for each (GItem* pItem in vecItem)
	{
		gsys.pItemSystem->GetRemover().Remove(pEntityPlayer, SLOTTYPE_INVENTORY, pItem->m_nSlotID, pItem->GetAmount());
	}	

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMKillEntity(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (IsGM(pPlayer) == false) return CR_TRUE;

	UIID nUIID;
	if (pCmd->GetParameter(&nUIID, 0, MPT_USHORT) == false) return CR_FALSE;

	GEntityPlayer* pTarPlayer = gmgr.pPlayerObjectManager->GetEntity(nUIID);
	if (NULL != pTarPlayer)
	{
		pTarPlayer->SetKiller(pPlayer->GetUID());
		pTarPlayer->doDie();
	}

	GEntityNPC* pTarNPC = pPlayer->FindNPC(nUIID);
	if (NULL != pTarNPC)
	{
		pTarNPC->GetHateTable().AddPoint_FoundEnemy(pPlayer);
		pTarNPC->SetKiller(pPlayer->GetUID());
		pTarNPC->doDie();
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMRegen(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (IsGM(pPlayer) == false) return CR_TRUE;

	if (pPlayer->IsDisableRegen())
	{
		pPlayer->EnableRegen();		
	}
	else
	{
		pPlayer->DisableRegen();
	}

	return CR_TRUE;
}


MCommandResult GCmdHandler_GM::OnRequestGMRangeKillEntity(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	if (IsGM(pPlayer) == false) return CR_TRUE;

	float fRange;

	if (pCmd->GetParameter(&fRange, 0, MPT_FLOAT) == false) return CR_FALSE;

	vector<GEntityNPC*> vecNeighborNPC;
	pPlayer->GetNeighborNPC(fRange, vecNeighborNPC);

	GRelationChecker relationChecker;

	for each (GEntityNPC* pNeighborNPC in vecNeighborNPC)
	{
		if (true == pNeighborNPC->IsDead()) continue;
		if (false == relationChecker.IsEnemy(pPlayer, pNeighborNPC)) continue;

		pNeighborNPC->GetHateTable().AddPoint_FoundEnemy(pPlayer);
		pNeighborNPC->SetKiller(pPlayer->GetUID());
		pNeighborNPC->doDie();
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMSetMe(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (!IsRunForTest())
	{
		if (IsGM(pCmd) == false) return CR_TRUE;
	}

	wstring strType;
	wstring strValue;
	if (pCmd->GetParameter(strType,		0, MPT_WSTR) == false) return CR_FALSE;
	if (pCmd->GetParameter(strValue,	1, MPT_WSTR) == false) return CR_FALSE;

	GGmSetMeHandler gmSetMeHandler;

	gmSetMeHandler.Handle(pPlayer, strType, strValue);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMSetNPC(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (!IsRunForTest())
	{
		if (IsGM(pCmd) == false) return CR_TRUE;
	}

	wstring strType;
	wstring strValue;
	if (pCmd->GetParameter(strType,		0, MPT_WSTR) == false) return CR_FALSE;
	if (pCmd->GetParameter(strValue,	1, MPT_WSTR) == false) return CR_FALSE;

	GGMSetNPCHandler gmSetNPCHandler;	

	gmSetNPCHandler.Handle(pPlayer, strType, strValue);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMQuestGive(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer) return CR_FALSE;

	if (IsDeveloper(pEntityPlayer) == false) return CR_TRUE;

	int nQuestID;
	if (pCmd->GetParameter(&nQuestID, 0, MPT_INT) == false) return CR_FALSE;

	gsys.pQuestSystem->GetGiver().GiveForGM(pEntityPlayer, nQuestID);

	return CR_TRUE;	
};

MCommandResult GCmdHandler_GM::OnRequestGMQuestComplete(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	int nQuestID;
	if (pCmd->GetParameter(&nQuestID,		0, MPT_INT) == false) return CR_FALSE;

	gsys.pQuestSystem->GetCompleter().CompleteForGM(pPlayer, nQuestID);	

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMQuestReward(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	int nQuestID;
	if (pCmd->GetParameter(&nQuestID, 0, MPT_INT) == false) return CR_FALSE;

	gsys.pQuestSystem->GetRewarder().RewardForGM(pPlayer, nQuestID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMQuestReset(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	gsys.pQuestSystem->GetReseter().ResetDoneQuestForGM(pPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMQuestResetAll(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	gsys.pQuestSystem->GetReseter().ResetAllQuestForGM(pPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMQuestVar(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	int nQuestID;
	if (pCmd->GetParameter(&nQuestID, 0, MPT_INT) == false) return CR_FALSE;

	gsys.pQuestSystem->GetFailer().Fail(pPlayer, nQuestID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMQuestFail(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	int nQuestID;
	if (pCmd->GetParameter(&nQuestID, 0, MPT_INT) == false) return CR_FALSE;

	GPlayerQuest* pPlayerQuest = pPlayer->GetQuests().Get(nQuestID);
	if (NULL == pPlayerQuest) return CR_FALSE;	

	MCommand* pNewCmd = MakeNewCommand(MC_GM_QUEST_VAR,
		1,
		NEW_INT(pPlayerQuest->GetQuestVar()));

	pPlayer->RouteToMe(pNewCmd);


	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMSetItem(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	wstring strType;
	int nInvenSlotID;
	int nValue;
	if (pCmd->GetParameter(strType, 0, MPT_WSTR) == false) return CR_FALSE;
	if (pCmd->GetParameter(&nInvenSlotID, 1, MPT_INT) == false) return CR_FALSE;	
	if (pCmd->GetParameter(&nValue,	2, MPT_INT) == false) return CR_FALSE;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	GItem* pItem = pItemHolder->GetItem(SLOTTYPE_INVENTORY, nInvenSlotID);
	if (NULL == pItem) return CR_FALSE;

	// 소문자로 변환
	for (wstring::size_type i = 0 ; i < strType.length() ; ++i)
	{
		strType[i] = tolower(strType[i]);
	}	

	if (strType == L"durability")
	{
		gsys.pItemSystem->GetDurabilityUpdater().Update(pPlayer, pItem, nValue);
	}
	else
	{
		return CR_TRUE;
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMInsertBuff(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	int nBuffID;
	float fDurationTime;
	float fPeriodTime;
	if (pCmd->GetParameter(&nBuffID,		0, MPT_INT) == false) return CR_FALSE;
	if (pCmd->GetParameter(&fDurationTime,	1, MPT_FLOAT) == false) return CR_FALSE;
	if (pCmd->GetParameter(&fPeriodTime,	2, MPT_FLOAT) == false) return CR_FALSE;

	GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
	if (NULL == pBuffInfo) return CR_FALSE;

	pPlayer->GetModuleBuff()->GainBuffForced(pBuffInfo, fDurationTime, fPeriodTime);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMDeleteBuff(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	int nBuffID;
	if (pCmd->GetParameter(&nBuffID,		0, MPT_INT) == false) return CR_FALSE;

	pPlayer->GetModuleBuff()->CancelBuff(nBuffID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMResetCoolTime(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsGM(pPlayer) == false) return CR_TRUE;

	pPlayer->GetActorCooltimeChecker().Clear();

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMShowEnemyUID(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;
	
	GVectorMUID vecEnemyUID;
	pPlayer->GetActorHateTracker().GetHateNPCs(vecEnemyUID.Vector());

	MCommand* pNewCmd = MakeNewCommand(MC_GM_SHOW_ENEMY_UID,
		1,
		NEW_BLOB(vecEnemyUID.Vector()));

	pPlayer->RouteToMe(pNewCmd);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMBreakPart(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	MUID uidNPC;
	uint8 nBPartID;
	if (pCmd->GetParameter(&uidNPC,		0, MPT_UID) == false) return CR_FALSE;
	if (pCmd->GetParameter(&nBPartID,	1, MPT_UCHAR) == false) return CR_FALSE;

	GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(uidNPC);
	if (NULL == pNPC) return CR_FALSE;

	if (pNPC->GetNPCBParts())
		pNPC->GetNPCBParts()->Break(nBPartID, pPlayer->GetUID());

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMRangeBreakPart(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	float fRange;
	if (pCmd->GetParameter(&fRange,		0, MPT_FLOAT) == false) return CR_FALSE;

	vector<GEntityNPC*> vecNeighborNPC;
	pPlayer->GetNeighborNPC(fRange, vecNeighborNPC);

	GRelationChecker relationChecker;

	for each (GEntityNPC* pNeighborNPC in vecNeighborNPC)
	{
		if (true == pNeighborNPC->IsDead()) continue;
		if (false == relationChecker.IsEnemy(pPlayer, pNeighborNPC)) continue;

		for (int nBPartID=1; nBPartID<=MAX_BREAKABLE_PARTS; nBPartID++)
		{
			if (pNeighborNPC->GetNPCBParts())
				pNeighborNPC->GetNPCBParts()->Break(nBPartID, pPlayer->GetUID());
		}
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMLogCRTInsert(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;	

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	int nCRT;
	if (pCmd->GetParameter(&nCRT,		0, MPT_INT) == false) return CR_FALSE;

	GEntityPlayer::GetPlayerCRTLogger().Insert(pPlayer, static_cast<CCommandResultTable>(nCRT));

	return CR_TRUE;
}


MCommandResult GCmdHandler_GM::OnRequestGMLogCRTDelete(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;	

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	int nCRT;
	if (pCmd->GetParameter(&nCRT,		0, MPT_INT) == false) return CR_FALSE;

	GEntityPlayer::GetPlayerCRTLogger().Delete(pPlayer, static_cast<CCommandResultTable>(nCRT));

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMQueryMultiLogin(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;	

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;
	
	wstring strPlayerName = pPlayer->GetName();
	bool bMultiLogin =gmgr.pPlayerObjectManager->IsMultiLoginPlayerID(strPlayerName);

	MCommand* pNewCmd = MakeNewCommand(MC_GM_QUERY_MULTILOGIN, 1, NEW_BOOL(bMultiLogin));
	pPlayer->RouteToMe(pNewCmd);	

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMFactionIncrease(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;	

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	uint8 nFactionID;
	uint16 nQuantity;
	if (pCmd->GetParameter(&nFactionID,		0, MPT_UCHAR) == false) return CR_FALSE;
	if (pCmd->GetParameter(&nQuantity,		1, MPT_USHORT) == false) return CR_FALSE;

	gsys.pFactionSystem->Increase(pPlayer, nFactionID, nQuantity);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMFactionDecrease(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;	

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	uint8 nFactionID;
	uint16 nQuantity;
	if (pCmd->GetParameter(&nFactionID,		0, MPT_UCHAR) == false) return CR_FALSE;
	if (pCmd->GetParameter(&nQuantity,		1, MPT_USHORT) == false) return CR_FALSE;

	gsys.pFactionSystem->Decrease(pPlayer, nFactionID, nQuantity);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMFactionReset(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;	

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	gsys.pFactionSystem->Reset(pPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMChangeServerMode(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsGM(pCmd) == false) return CR_TRUE;

	int nServerMode;
	if (pCmd->GetParameter(&nServerMode, 0, MPT_INT) == false) return CR_FALSE;

	GServerModeChanger serverModeChanger;
	serverModeChanger.ChangeServerMode(nServerMode);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMResetExpoCharacters(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	if (IsGM(pCmd) == false) return CR_TRUE;

	GGMExpoHandler gmExpoHandler;
	gmExpoHandler.ResetCharacters();
	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestGMDye(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;	

	if (IsDeveloper(pPlayer) == false) return CR_TRUE;

	int nSlotType;
	int nSlotID;
	int nColor;

	if (pCmd->GetParameter(&nSlotType,		0, MPT_INT) == false) return CR_FALSE;
	if (pCmd->GetParameter(&nSlotID,		1, MPT_INT) == false) return CR_FALSE;
	if (pCmd->GetParameter(&nColor,			2, MPT_INT) == false) return CR_FALSE;

	gsys.pItemSystem->GetDyer().DyeForGM(pPlayer, static_cast<SH_ITEM_SLOT_TYPE>(nSlotType), nSlotID, nColor);

	return CR_TRUE;
}

bool GCmdHandler_GM::IsDeveloper( MCommand* pCmd )
{
	MUID uidPlayer = pCmd->GetSenderUID();
	VALID_RET(uidPlayer.IsValid(), false);

	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	VALID_RET(pEntityPlayer, false);

	return IsDeveloper(pEntityPlayer);
}

bool GCmdHandler_GM::IsDeveloper( GEntityPlayer* pEntityPlayer )
{
	VALID_RET(pEntityPlayer, false);
	return pEntityPlayer->IsDeveloper();
}

bool GCmdHandler_GM::IsGM(MCommand* pCmd)
{
	MUID uidPlayer = pCmd->GetSenderUID();
	VALID_RET(uidPlayer.IsValid(), false);

	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	VALID_RET(pEntityPlayer, false);

	return IsGM(pEntityPlayer);
}

bool GCmdHandler_GM::IsGM( GEntityPlayer* pEntityPlayer )
{
	VALID_RET(pEntityPlayer, false);
	return pEntityPlayer->IsGM();
}

MCommandResult GCmdHandler_GM::OnRequestStartCommandProfile(MCommand* pCmd, MCommandHandler* pHandler)
{
	if (IsDeveloper(pCmd) == false) return CR_TRUE;

	gsys.pServer->StartCommandProfile();

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestDumpCommandProfile(MCommand* pCmd, MCommandHandler* pHandler)
{
	if (IsDeveloper(pCmd) == false) return CR_TRUE;

	gsys.pServer->DumpCommandProfile();

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestEndCommandProfile(MCommand* pCmd, MCommandHandler* pHandler)
{
	if (IsDeveloper(pCmd) == false) return CR_TRUE;

	gsys.pServer->FinishCommandProfile();

	return CR_TRUE;
}


MCommandResult GCmdHandler_GM::OnRequestServerDump(MCommand* pCmd, MCommandHandler* pHandler)
{
	if (IsDeveloper(pCmd) == false) return CR_TRUE;

	wstring strID;
	if (pCmd->GetParameter(strID, 0, MPT_WSTR) == false) return CR_FALSE;

	gsys.pServer->Dump(strID);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_TRUE;

	MCommand* pNewCmd = MakeNewCommand(MC_GM_SERVER_DUMP_RESP, 0, NULL);
	pPlayer->RouteToMe(pNewCmd);

	// MasterServer 에도 보냄.
	MCommand* pNewCmdToMaster = gsys.pMasterServerFacade->MakeNewCommand(MMC_DEBUG_DUMP, 1, NEW_INT((AID)pPlayer->GetAID()));
	gsys.pMasterServerFacade->Route(pNewCmdToMaster);

	return CR_TRUE;
}

MCommandResult GCmdHandler_GM::OnRequestDebugEchoReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	int nType = 0;
	if (pCmd->GetParameter(&nType, 0, MPT_INT) == false) return CR_FALSE;

	int nDataSize = 0;
	if (pCmd->GetParameter(&nDataSize, 1, MPT_INT) == false) return CR_FALSE;

	MUID uidPlayer = pCmd->GetSenderUID();

	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (!pEntityPlayer) return CR_FALSE;

	vector<int> vecData;

	vecData.resize(nDataSize);

	for (int i = 0; i < nDataSize; i++)
	{
		vecData[i] = i;
	}

	MCommand* pNewCommand = MakeNewCommand(MC_DEBUG_ECHO, 2, NEW_UID(uidPlayer), NEW_BLOB(vecData));

	if (nType == 0) pEntityPlayer->RouteToMe(pNewCommand);
	else if (nType == 1) pEntityPlayer->RouteToThisCellExceptMe(pNewCommand);
	else if (nType == 2) pEntityPlayer->RouteToThisCell(pNewCommand);
	else if (nType == 3) pEntityPlayer->RouteToField(pNewCommand);

	return CR_TRUE;
}