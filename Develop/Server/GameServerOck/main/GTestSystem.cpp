#include "stdafx.h"
#include "GTestSystem.h"
#include "GFieldInfo.h"
#include "GScriptManager.h"
#include "GGlobal.h"
#include "GFieldMgr.h"
#include "GConst.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GModuleAI.h"
#include "GPlayerObjectManager.h"
#include "GJobRunner.h"
#include "GBirdMonitor.h"
#include "GReloader.h"
#include "GCommandTable.h"
#include "GServer.h"
#include "GCommand.h"
#include "GCommandCenter.h"
#include "GConst.h"
#include "GAttackDamageCalculator.h"
#include "GCriticalCalculator.h"
#include "GDefenseFactorCalculator.h"
#include "GFieldFactory.h"
#include "GJobMgr.h"
#include "GBehaviorState.h"
#include "GNPCMgr.h"
#include "GMessageID.h"
#include "CSLogListener.h"
#include "GNPCShopSystem.h"
#include "GItemHolder.h"
#include "GDBManager.h"
#include "GPlayerFieldDynamic.h"
#include "GNPCShopRepairer.h"
#include "GChallengerQuestMgr.h"
#include "GStrings.h"
#include "GPathFinder.h"
#include "GItemEnchant.h"
#include "GPartySystem.h"
#include "GAIMonitorPort.h"
#include "GPlayerAFK.h"
#include "GFieldNPCSessionMgr.h"
#include "GFieldNPCSession.h"
#include "GScriptGroup_Field.h"
#include "GGluePlayer.h"
#include "GCombatMonitorPort.h"
#include "GModuleCombat.h"

bool GTestSystem::IsPublishDefined()
{
#ifdef _PUBLISH
	return true;
#endif

	return false;
}

#define IF_PUBLISH_RETURN					{ if (IsPublishDefined() && (!IsRunForTest())) return; }
#define IF_PUBLISH_RETURN_VALUE(ret)		{ if (IsPublishDefined() && (!IsRunForTest())) return ret; }


GTestSystem::GTestSystem()
: m_pListener(NULL)
{
	m_DespawnReporter.SetReportName(L"Despawn");
	m_DespawnReporter.SetRecordLimit(30);
	m_PickingReporter.SetReportName(L"Picking");
	m_PickingReporter.SetRecordLimit(30);
}

void GTestSystem::SetListener( CSLogListener* pListener )
{
	m_pListener = pListener;
}

void GTestSystem::OnRuntimeLog( const wchar_t* szLog )
{
	if (!m_pListener)
		return;

	m_pListener->OnLog(MLocale::ConvUTF16ToAnsi(szLog).c_str());
}

void GTestSystem::CreateMassSpawnTestFields(GFieldInfo* pFieldInfo)
{
	IF_PUBLISH_RETURN;

	if (!WLUA->IsOpened())
		return;

	// test code!
	int TEST_MASS_SPAWN_DYNAMIC_FIELD_AMOUNT = WLUA->GetVar<int>("TEST_MASS_SPAWN_DYNAMIC_FIELD_AMOUNT");

	for (int i=0; i<TEST_MASS_SPAWN_DYNAMIC_FIELD_AMOUNT; i++)
	{
		// 다이나믹 필드와 채널과 구조가 비슷하므로, 편의상 채널로 생성
		gmgr.pFieldMgr->GetFactory()->CreateSharedField(gsys.pServer->NewUID(), pFieldInfo->m_nFieldID, 1);
	}
}

void GTestSystem::SwitchAIRunning(GEntityPlayer* pEntityPlayer)
{
	// ai는 publish가 아니어도 끌 수 있도록 합니다.
	//IF_PUBLISH_RETURN;
	VALID(pEntityPlayer);
	VALID(pEntityPlayer->GetPlayerInfo());

	GConst::NPC_AI_RUNNING = !GConst::NPC_AI_RUNNING;

	wchar_t buff[512];
	swprintf_s(buff, L"NPC AI %s request by \"%s\"", 
		GConst::NPC_AI_RUNNING?L"enable":L"disable", 
		pEntityPlayer->GetPlayerInfo()->szName
		);

	// logging
	mlog("%s - %s\n", __FUNCTION__, MLocale::ConvUTF16ToAnsi(buff).c_str());
	
	// route
	MCommand* pNewCommand = MakeNewCommand(	MC_NPC_NARRATION, 
		1, 
		NEW_WSTR(buff));

	gsys.pCommandCenter->RouteToAll(pNewCommand);
}

void GTestSystem::RequestNPCAIRunning( MCommand* pCmd )
{
	IF_PUBLISH_RETURN;

	MUID uidNPC;
	if (pCmd->GetParameter(&uidNPC,		0, MPT_UID)==false) return;

	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (NULL == pEntityPlayer) return;

	GEntityNPC* pEntityNPC = pEntityPlayer->FindNPC(uidNPC);
	if (pEntityNPC == NULL) return;

	pEntityNPC->SetThinkable(!pEntityNPC->IsThinkable());
}

void GTestSystem::RequestAICommand( MCommand* pCmd )
{
	IF_PUBLISH_RETURN;

	MUID uidNPC;
	wstring strCommand;
	wstring strParam;

	if (pCmd->GetParameter(&uidNPC,		0, MPT_UID)==false) return;
	if (pCmd->GetParameter(strCommand,	1, MPT_WSTR)==false) return;
	if (pCmd->GetParameter(strParam,	2, MPT_WSTR)==false) return;

	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (NULL == pEntityPlayer) return;

	GEntityNPC* pEntityNPC = pEntityPlayer->FindNPC(uidNPC);
	if (pEntityNPC == NULL) return;


	if (!_wcsicmp(strCommand.c_str(), L"talent"))	// use talent
	{
		int nTalentID = _wtoi(strParam.c_str());
		if (nTalentID != 0)
		{
			GJobRunner job_runner;
			job_runner.UseTalent(pEntityNPC, pEntityPlayer, nTalentID);
		}
	}
	else if (!_wcsicmp(strCommand.c_str(), L"move"))	// move
	{
		vec3 vTarPos = vec3::ZERO;
		swscanf(strParam.c_str(), L"%f %f %f", &vTarPos.x, &vTarPos.y, &vTarPos.z);

		GJobRunner job_runner;
		job_runner.Move(pEntityNPC->GetModuleAI(), vTarPos, false);
	}
	else if (!_wcsicmp(strCommand.c_str(), L"stop"))
	{		
		GJobMgr* pJobMgr = pEntityNPC->GetJobMgr();
		pJobMgr->Clear();
	}
	else if (!_wcsicmp(strCommand.c_str(), L"warp"))
	{
		vec3 vTarPos = vec3::ZERO;
		swscanf(strParam.c_str(), L"%f %f %f", &vTarPos.x, &vTarPos.y, &vTarPos.z);

		pEntityNPC->SetPos(vTarPos);
	}
}

void GTestSystem::RequestAISetMonitorNPC( MCommand* pCmd )
{
	IF_PUBLISH_RETURN;

	UIID nNPCUIID;
	int nTalentID = 0;

	if (pCmd->GetParameter(&nNPCUIID,		0, MPT_USHORT)==false) return;

	MUID uidPlayer = pCmd->GetSenderUID();
	if (uidPlayer.IsInvalid()) return;

	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (NULL == pEntityPlayer) return;

	GField* pField = pEntityPlayer->GetField();
	if (pField == NULL) return;

	if (BirdM().IsAttached(uidPlayer))
	{
		BirdM().DetachReportToClient(uidPlayer);
	}

	GBirdMonitor::Entry entry;
	entry.nPlayerUIID = pEntityPlayer->GetUIID();
	entry.uidField = pField->GetUID();
	entry.nTargetNPCUIID = nNPCUIID;
	BirdM().AttachReportToClient(entry);
}

void GTestSystem::RequestDebugString( MCommand* pCmd )
{
	wstring strKey, strText1;
	int nParam = 0;
	if (pCmd->GetParameter(strKey,		0, MPT_WSTR)==false) return;
	if (pCmd->GetParameter(&nParam,		1, MPT_INT)==false) return;
	if (pCmd->GetParameter(strText1,	2, MPT_WSTR)==false) return;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(pCmd->GetSenderUID());
	if (NULL == pPlayer) return;

	
	if (!pPlayer->IsGM())	return;


	GField* pField = pPlayer->GetField();
	if (pField == NULL) return;

	if (strKey == L"tick")
	{

	}
	else if (strKey == L"fly")
	{
		vec3 vTarPos = vec3::ZERO;
		vTarPos.z = 5000.0f;
		swscanf(strText1.c_str(), L"%f %f", &vTarPos.x, &vTarPos.y);

		GEntityNPC* pEntityNPC = pField->FindNPC(BirdM().GetTargetNPC(pPlayer->GetUIID()));
		if (pEntityNPC)
		{
			GJobRunner job_runner;
			job_runner.Move(pEntityNPC->GetModuleAI(), vTarPos, false);
		}
	}
	else if (strKey == L"const") 
	{
		string strName = MLocale::ConvUTF16ToAnsi(strText1.c_str());
		WLUA->SetNull(strName.c_str());
		WLUA->SetVar<int>(strName.c_str(), nParam);

		GConst::Init(WLUA);
	}
	else if (strKey == L"re") 
	{
		if (pPlayer->IsDeveloper())
		{
			mmlog("req_reload: %s / %d\n", MLocale::ConvUTF16ToAnsi(strText1.c_str()).c_str(), nParam);

			GReloader reloader;
			
			if (strText1 == L"n" && nParam > 0)
			{
				reloader.Reload_NPC(nParam);
			}
			else if (strText1 == L"f")
			{
				if (nParam == 0)
				{
					nParam = pPlayer->GetFieldID();
				}
				reloader.Reload_Field(nParam);
			}
			else
			{
				reloader.Reload_All();
			}
		}
	}
	else if (strKey ==L"treport") 
	{
		mmlog("req_despawn: %s / %d\n", MLocale::ConvUTF16ToAnsi(strText1.c_str()).c_str(), nParam);
		if (strText1 == L"despawn")
		{
			m_DespawnReporter.FlushToFile();
			m_PickingReporter.FlushToFile();
		}
	}
	else if (strKey == L"pos_sync") 
	{
		GField* pField =pPlayer->GetField();
		if (pField == NULL) return;

		MCommandParameter* pParam = pCmd->GetParameter(3);
		if(pParam->GetType()!=MPT_BLOB) return;
		MUID* pUIDs = (MUID*)pParam->GetPointer();

		vector<TD_DEBUG_POS>		vectorPos;

		for (int i = 0; i < pParam->GetBlobCount(); i++)
		{
			MUID uid = pUIDs[i];

			GEntity* pEntity = pField->FindEntity(uid);
			if (pEntity)
			{
				TD_DEBUG_POS pos_info;
				pos_info.uidTarget = uid;
				pos_info.vPoint = pEntity->GetPos();
				pos_info.vDir = pEntity->GetDir();

				vectorPos.push_back(pos_info);
			}
		}

		if (!vectorPos.empty())
		{
			MCommand* pNewCommand = MakeNewCommand(MC_DEBUG_STRING, 4, NEW_WSTR(L"pos_sync"), NEW_INT(0), NEW_WSTR(L""), 
				NEW_BLOB(&(vectorPos[0]), sizeof(TD_DEBUG_POS), (unsigned short)vectorPos.size()));
			pPlayer->RouteToMe(pNewCommand);
		}
	}
	else if (strKey == L"show_actor_pos") 
	{
		map<MUID, PosSendInfo>::iterator it =	m_mapSubscriptors_ActorPos.find(pPlayer->GetUID());

		if (it != m_mapSubscriptors_ActorPos.end())
		{
			m_mapSubscriptors_ActorPos.erase(it);
		}
		else if (nParam == 1 && it == m_mapSubscriptors_ActorPos.end())
		{
			PosSendInfo info;
			info.uidSubscriptors = pPlayer->GetUID();
			info.rgrTick.SetTickTime(0.1f);
			info.rgrTick.Start();
			m_mapSubscriptors_ActorPos.insert(make_pair(pPlayer->GetUID(),info));
		}
	}
	else if (strKey == L"npc_monitor_show_type") 
	{
		GBirdMonitor::Entry* entry = BirdM().FindEntry(pPlayer->GetUID());
		if (!entry)
			return;

		for (int i=0; i<NPCLOG_ALL; ++i)
		{
			if (CheckBitSet(nParam, i))
			{
				entry->setTypes.insert((NPCLogRecordType)i);
			}
			else
			{
				entry->setTypes.erase((NPCLogRecordType)i);
			}
		}
	}
	else if (strKey == L"repair_all") 
	{
		gsys.pNPCShopSystem->GetRepairer().RepairAllForGM(pPlayer);
	}
	else if (strKey == L"hit")
	{
		map<MUID, PosSendInfo>::iterator it =	m_mapSubscriptors_ActorHitCapsule.find(pPlayer->GetUID());

		if (it != m_mapSubscriptors_ActorHitCapsule.end())
		{
			m_mapSubscriptors_ActorHitCapsule.erase(it);
			RouteToClientActorRenderCapsuleClear(pPlayer->GetUID());
		}
		else
		{
			PosSendInfo info;
			info.uidSubscriptors = pPlayer->GetUID();
			info.rgrTick.SetTickTime(0.1f);
			info.rgrTick.Start();
			m_mapSubscriptors_ActorHitCapsule.insert(make_pair(pPlayer->GetUID(),info));
		}
	}
	else if (strKey == L"remblem")
	{
		CString strSQL;
		strSQL.Format(L"DELETE dbo.EmblemInstance WHERE CID = %d;", pPlayer->GetCID());
		gsys.pDBManager->ExecuteAsyncForDev(pPlayer->GetUID(), strSQL.GetBuffer());
	}
	else if (strKey == L"leave")
	{
		pPlayer->GetPlayerField().GetFieldDynamic().GateToEnterPos(pPlayer->GetFieldID());
	}
	else if (strKey == L"creset")
	{
		gmgr.pChallengerQuestMgr->AllQuestFailForTest();
		gsys.pDBManager->Execute(L"{CALL spFailChallangerQuest}");
	}
	else if (strKey == L"nv_pick")
	{
		bool bValid = gsys.pPathFinder->IsValidPosition(gsys.pPathFinder->GetMesh(pPlayer->GetFieldInfo()->m_nZoneID), pPlayer->GetPos());
		if (bValid)
			pPlayer->Narration(L"현재위치 네비게이션메쉬 상태: 양호");
		else
			pPlayer->Narration(L"현재위치 네비게이션메쉬 상태: 유효하지 않음");
	}
	else if (strKey == L"raw_pick")
	{
		vec3 vPos = pPlayer->GetPos();
		pPlayer->GetField()->PickingZ_raw(vPos, vPos);
		char buff[512];
		sprintf_s(buff, "pickZ_raw: %.1f %.1f %.1f -> %.1f", vPos.x, vPos.y, pPlayer->GetPos().z, vPos.z);
		pPlayer->Narration(MLocale::ConvAnsiToUTF16(buff).c_str());
	}
	else if (strKey == L"nv_pos")
	{
		SPathMesh* pMesh = gsys.pPathFinder->GetMesh(pPlayer->GetFieldInfo()->m_nZoneID);

		vec3 vPos = pPlayer->GetPos();
		vPos.z += 50.f;
		cPosition cPos = pMesh->m_pMesh->positionFor3DPointF(vPos.v);
		char buff[512];
		sprintf_s(buff, "nevi_pos: %d %d %d", cPos.x, cPos.y, cPos.cell);
		pPlayer->Narration(MLocale::ConvAnsiToUTF16(buff).c_str());
	}
	else if (strKey == L"call")
	{
		WCALL1(WLUA, "test_handler", pPlayer->GetDelegator(), WNULL);
	}
	else if (strKey == L"nearnode")
	{
		SPathMesh* pMesh = gsys.pPathFinder->GetMesh(pPlayer->GetFieldInfo()->m_nZoneID);

		vec3 position = pPlayer->GetPos();
		tSigned32 iPos[3];
		iPos[0] = static_cast<int>(position.x);
		iPos[1] = static_cast<int>(position.y);
		iPos[2] = static_cast<int>(position.z);
		cPosition pathPos = pMesh->m_pMesh->positionNear3DPoint(iPos, 100, 400);
		if (!pMesh->m_pMesh->positionIsValid(pathPos))
		{
			pPlayer->Narration(L"nearnode 콘솔명령어 실패: 유효한 인접노드 검색 실패.");
			return;
		}

		// 인접 노드 위치 나레이션
		char buff[512];
		float fHeight = 0;
		GPathFinderImpl::GetHeightAtPosition(&fHeight, pMesh, pathPos);
		sprintf_s(buff, "nearnode: %d %d %d -> %.1f", pathPos.x, pathPos.y, position.z, fHeight);

		if (!pMesh->m_pMesh->positionIsValid(pathPos))
			pPlayer->Narration(MLocale::ConvAnsiToUTF16(buff).c_str());

		// 인접 노드 캡슐 그리기
		MCapsule c;
		c.bottom = vec3((float)pathPos.x, (float)pathPos.y, fHeight);
		c.top = vec3((float)pathPos.x, (float)pathPos.y, fHeight+100);
		c.height = 100;
		c.radius = 10;
		RouteToClientRenderCapsule(pPlayer->GetUID(), pPlayer, c);
	}
	else if (strKey == L"buff")
	{
		pPlayer->GainBuff(nParam);
	}
	else if (strKey == L"enchant")
	{
		GItemHolder* pItemHolder = pPlayer->GetItemHolder();
		GItem* pEStone = pItemHolder->GetItem(SLOTTYPE_INVENTORY, 0);
		if (!pEStone)
			return;

		GItem* pEAgent = pItemHolder->GetItem(SLOTTYPE_INVENTORY, 1);
		if (!pEAgent)
			return;

		gsys.pItemSystem->GetEnchant().Enchant(pPlayer, pEStone, SLOTTYPE_EQUIP, ITEMSLOT_RWEAPON, pEAgent);
	}
	else if (strKey == L"auto_party")
	{
		vector<MUID> vecUID;
		pPlayer->GetNeighborSectorPlayer(vecUID);

		gsys.pPartySystem->CreateAutoPartyReq(0, vecUID);
	}
	else if (strKey == L"aimon")
	{
		GEntityNPC* pEntityNPC = pField->FindNPC(UIID(nParam));
		if (!pEntityNPC)
			return;
		GAIMonitorPort::Inst().SetNPC(pEntityNPC->GetUID());
	}
	else if (strKey == L"afk")
	{
		pPlayer->GetAFK().Toggle();
	}
	else if (strKey == L"sstate")
	{
		TCHAR szSession[512];
		TCHAR szState[512];
		swscanf(strText1.c_str(), L"%s %s", &szSession, &szState);

		GFieldNPCSession* pSession = 
			pPlayer->GetField()->GetSession()->Find(MLocale::ConvTCHARToAnsi(szSession).c_str());
		
		if (!pSession)
			return;

		pSession->ChangeScene(MLocale::ConvTCHARToAnsi(szState).c_str());
	}
	else if (strKey == L"ssensor")
	{
		const SENSOR_INFO* pSensor = pField->GetInfo()->FindSensor(nParam);
		if (pSensor)														
			GetScriptField()->OnSensorEnter(pField->GetDelegator(), pSensor->nID, pPlayer->GetDelegator());
	}
	else if (strKey == L"cmon")
	{
		GCombatMonitorPort::Inst().LaunchMonitor();
		pField->AttachCombatMonitor(&GCombatMonitorPort::Inst());
	}
	else
	{
		wstring strMsg = L"알 수 없는 DS 명령어: ";
		strMsg += strKey;
		pPlayer->Narration(strMsg.c_str());
	}
}

void GTestSystem::RequestGMGetPlayerUID( MCommand* pCmd )
{
	IF_PUBLISH_RETURN;
		
	wstring strPlayerID;
	if (pCmd->GetParameter(strPlayerID,		0, MPT_WSTR)==false) return;
	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityFromPlayerID(strPlayerID);
	if (NULL == pEntityPlayer) return;

	MUID uidPlayer = pEntityPlayer->GetUID();

	MCommand* pNewMyCommand = MakeNewCommand(MC_GM_GET_PLAYERUID, pCmd->GetSenderUID(), 1, NEW_UID(uidPlayer));
	gsys.pCommandCenter->PostCommand(pNewMyCommand);

}

void GTestSystem::RouteAIDebugMessage( GEntitySync* pEntity, const wchar_t* szMessage )
{
	IF_PUBLISH_RETURN;

	if (GConst::TEST_AI_DEBUG_MESSAGE == false) return;

	MCommand* pNewCommand = MakeNewCommand((MC_AI_DEBUG_MESSAGE), 2, NEW_UID(pEntity->GetUID()), NEW_WSTR(szMessage));
	pEntity->RouteToThisCell(pNewCommand);

}

void GTestSystem::RouteCombatCalcDebug( GEntityPlayer* pPlayer, const wchar_t* pszType, const wchar_t* pszText )
{
	IF_PUBLISH_RETURN;

	if (!pPlayer)
		return;

	MCommand* pNewCmd = MakeNewCommand(MC_DEBUG_COMBATCALC, 2, NEW_WSTR(pszType), NEW_WSTR(pszText));
	pPlayer->RouteToMe(pNewCmd);
}


void GTestSystem::RouteDamageDebug( GEntityActor* pAttacker, GEntityActor* pVictim, const GTalentInfo* pTalentInfo, bool bCritical, const GDamageRangedInfo& DamageInfo, GCombatTurnResult &ret, float fResistRatePercent )
{
	IF_PUBLISH_RETURN;

	if (!GConst::TEST_SHOW_CLIENT_FORMULA_DAMAGE)
		return;

	wchar_t text[256];

	// 탤런트 데미지
	int nTalentDamageMin = DamageInfo.nMinDamage, nTalentDamageMax = DamageInfo.nMaxDamage;

	int nWeaponDamage = 0;
	float fWeaponFactor = 1.0f;
	float fTalentFactor = 1.0f;
	float fCharacterFactor = 1.0f;
	float fBuffFactor = 1.0f;
	float fCriticalFactor = 1.0f;

	if (pAttacker->IsPlayer())
	{
		GEntityPlayer* pPlayerAttacker = static_cast<GEntityPlayer*>(pAttacker);

		// 스탯 보정율
		fCharacterFactor = gsys.pAttackDamageCalculator->CalcCharacterFactor(pPlayerAttacker, pTalentInfo);
	}
	fWeaponFactor = pTalentInfo->m_fWeaponApplyRate;

	swprintf_s(text, L"]TalentDmg(%d~%d), 스탯보정율(%.2f), 무기데미지율(%.2f)",
		nTalentDamageMin, nTalentDamageMax,
		fCharacterFactor,
		fWeaponFactor);

	gsys.pCommandCenter->NetDebugString(L"damage", 0, text);

	// ---------------------------

	GCriticalCalculator criticalCalculator;
	GDefenseFactorCalculator defenseFactorCalculator;

	// 저항율
	float fCriticalPercent = criticalCalculator.CalcCriticalPercent(pAttacker, pVictim, pTalentInfo);

	// 공격 데미지
	float fAttackDamage = gsys.pAttackDamageCalculator->CalcAttackDamage(pAttacker, pVictim, pTalentInfo, bCritical, DamageInfo);

	// 아머에 의한 데미지 수정치
	float fDefenseFactor = defenseFactorCalculator.CalcDefenseFactor(pAttacker, pVictim, pTalentInfo);

	wchar_t szFlag[256] = L"";

	if (CheckBitSet(ret.nResultFlags, CTR_CRITICAL)) wcscat_s(szFlag, L"[크리]");
	if (CheckBitSet(ret.nResultFlags, CTR_DRAIN)) wcscat_s(szFlag, L"[흡수]");
	if (CheckBitSet(ret.nResultFlags, CTR_RESIST_PERFECT)) wcscat_s(szFlag, L"[완전저항]");
	if (CheckBitSet(ret.nResultFlags, CTR_RESIST_PARTIAL)) wcscat_s(szFlag, L"[부분저항]");

	swprintf_s(text, L"]]dmg(%d),크리(%.2f),저항(%.2f),공데(%.0f),아머수정치(%.2f) %s", 
		ret.nDamage,
		fCriticalPercent, 
		fResistRatePercent,
		fAttackDamage,
		fDefenseFactor,
		szFlag);

	if (pAttacker->IsPlayer())
	{
		gsys.pTestSystem->RouteCombatCalcDebug(ToEntityPlayer(pAttacker), L"공격 피해", text);
	}

	if (pVictim->IsPlayer())
	{
		gsys.pTestSystem->RouteCombatCalcDebug(ToEntityPlayer(pVictim), L"피격 피해", text);
	}
}



void GTestSystem::SetNPCMonitor(UIID nPlayerUIID, UIID nNPCUIID)
{
	IF_PUBLISH_RETURN;

	BirdM().SetTargetNPC(nPlayerUIID, nNPCUIID);
	
}

void GTestSystem::ShowGuardFormula(GEntityActor* pGuarder, int nDamage, float fReduce, GUARD_TYPE nGuardType)
{
	IF_PUBLISH_RETURN;

	if (!GConst::TEST_SHOW_CLIENT_FORMULA_GUARD)
		return;

	wchar_t buff[512];
	swprintf_s(buff, L"성공, 원래_피해 = %d, 보정된_피해 = %d (보정률: %.1f%%)", 
		nDamage,
		(int)(nDamage * fReduce), fReduce);

	std::wstring strGuardType = L"정의되지않은_방어";
	switch(nGuardType)
	{
	case GT_PERFECT:	strGuardType = L"완전방어";	break;
	case GT_PARTIAL:	strGuardType = L"부분방어";	break;
	}
	RouteCombatCalcDebug(ToEntityPlayer(pGuarder), strGuardType.c_str(), buff);
}

void GTestSystem::DespawnLog( MUID uidNPC, wstring strText )
{
	IF_PUBLISH_RETURN;

	if (!m_DespawnReporter.FindRecord(uidNPC))
	{
		GDebugReporter::Record record;
		record.uidNPC = uidNPC;
		GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(uidNPC);
		if (pNPC)
		{
			VALID(pNPC->GetNPCInfo());
			record.strName = pNPC->GetNPCInfo()->strName;
		}

		m_DespawnReporter.Push(record);
	}

	m_DespawnReporter.Log(uidNPC, strText);
}

void GTestSystem::PickingLog( MUID uidNPC, wstring strText )
{
	IF_PUBLISH_RETURN;

	if (!m_PickingReporter.FindRecord(uidNPC))
	{
		GDebugReporter::Record record;
		record.uidNPC = uidNPC;
		GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(uidNPC);
		if (pNPC)
		{
			VALID(pNPC->GetNPCInfo());
			record.strName = pNPC->GetNPCInfo()->strName;
		}
		
		m_PickingReporter.Push(record);
	}

	m_PickingReporter.Log(uidNPC, strText);
}

void GTestSystem::CheckNaN( const wchar_t* pszLocation, vec3 v )
{
	IF_PUBLISH_RETURN;

	if (_isnan(v.x) ||
		_isnan(v.y) ||
		_isnan(v.z))
	{
		const char* pszDumpName = "NaN_Error.dmp";
		mlog3("CheckNaN failed at %s: vector = %.1f %.1f %.1f\n", MLocale::ConvUTF16ToAnsi(pszLocation).c_str(), v.x, v.y, v.z);
		mlog3("Building crash dump (%s)...\n", pszDumpName);
		OnRuntimeLog(L"CheckNaN failed, check the crash dump.\n");
		MCrashDump::BuildCrashDump(pszDumpName);
		exit(-1);
	}
}

void GTestSystem::RouteToClientActorRenderCapsuleClear(MUID uidRequester)
{
	IF_PUBLISH_RETURN;

	MCommand* pNewCommand = MakeNewCommand(MC_DEBUG_STRING, uidRequester, 4, 
		NEW_WSTR(L"Clear_DrawActorCapsules"), 
		NEW_INT(0), 
		NEW_WSTR(L""),
		NEW_BLOB());
	gsys.pCommandCenter->PostCommand(pNewCommand);
}

void GTestSystem::RouteToClientRenderCapsule( MUID uidRequester, GEntityActor* pOwner, const MCapsule& capsule )
{
	IF_PUBLISH_RETURN;

	VALID(pOwner);

	wchar_t buff[128];
	swprintf_s(buff, 128, L"%u", pOwner->GetUID().High);

	TD_CAPSULE tdCapsule;
	tdCapsule.bottom = capsule.bottom;
	tdCapsule.top = capsule.top;
	tdCapsule.height = capsule.height;
	tdCapsule.radius = capsule.radius;

	MCommand* pNewCommand = MakeNewCommand(MC_DEBUG_STRING, uidRequester, 4, 
		NEW_WSTR(L"TraceProjectileHitCapsule"), 
		NEW_INT(pOwner->GetUID().Low), 
		NEW_WSTR(buff),
		NEW_BLOB(&tdCapsule, sizeof(TD_CAPSULE), static_cast<unsigned short>(1)));
	gsys.pCommandCenter->PostCommand(pNewCommand);
}

void GTestSystem::RouteToClientActorRenderCapsule( MUID uidRequester, GEntityActor* pOwner, const vector<MCapsule>& vecCapsules )
{
	IF_PUBLISH_RETURN;

	VALID(pOwner);

	wchar_t buff[128];
	swprintf_s(buff, 128, L"%u", pOwner->GetUID().High);

	vector<TD_CAPSULE> vecTDCapsules;
	for each (const MCapsule& capsule in vecCapsules)
	{
		TD_CAPSULE tdCapsule;
		tdCapsule.bottom = capsule.bottom;
		tdCapsule.top = capsule.top;
		tdCapsule.height = capsule.height;
		tdCapsule.radius = capsule.radius;
		vecTDCapsules.push_back(tdCapsule);
	}
	

	MCommand* pNewCommand = MakeNewCommand(MC_DEBUG_STRING, uidRequester, 4, 
		NEW_WSTR(L"DrawActorCapsules"), 
		NEW_INT(pOwner->GetUID().Low), 
		NEW_WSTR(buff),
		NEW_BLOB(vecTDCapsules));
	gsys.pCommandCenter->PostCommand(pNewCommand);
}


void GTestSystem::ClientRenderHitCapsule( GEntityActor* pOwner, const MCapsule& capsule )
{
	IF_PUBLISH_RETURN;

	if (!pOwner)
		return;

	if (pOwner->IsDead())
		return;

	GField* pField = pOwner->GetField();
	if (!pField)
		return;

	class DoClientRenderHitCapsule : public GFieldGrid::Sector::IVisitor
	{
	public:
		DoClientRenderHitCapsule(GTestSystem& testsystem,GField& field,GEntityActor& owner,const MCapsule& capsule,map<MUID, PosSendInfo>& subscriptors_ActorHitCapsule)
			: m_testsystem(testsystem)
			, m_field(field)
			, m_owner(owner)
			, m_capsule(capsule)
			, m_subscriptors_ActorHitCapsule(subscriptors_ActorHitCapsule) {}

	private:
		void OnVisit(const MUID& uid)
		{
			GEntityPlayer* const pPlayer = m_field.FindPlayer(uid);
			if (!pPlayer)
				return;

			if (m_subscriptors_ActorHitCapsule.find(pPlayer->GetUID()) == m_subscriptors_ActorHitCapsule.end())
				return; // 구독하지 않음

			m_testsystem.RouteToClientRenderCapsule(pPlayer->GetUID(), &m_owner, m_capsule);
		}
	private:
		GTestSystem&			m_testsystem;
		GField&					m_field;
		GEntityActor&			m_owner;
		const MCapsule&			m_capsule;
		map<MUID, PosSendInfo>&	m_subscriptors_ActorHitCapsule;
	};
	pField->GetGrid().GetEntitySelector()->VisitByPos(
		DoClientRenderHitCapsule( *this, *pField, *pOwner, capsule, m_mapSubscriptors_ActorHitCapsule ),
		ETID_PLAYER,
		pOwner->GetPos(),
		1
	);
}

void GTestSystem::ClientRenderActorCapsule( MUID uidPlayer, GEntityActor* pThisEntity )
{
	IF_PUBLISH_RETURN;

	if (pThisEntity->IsDead()) 
		return;

	GActorMeshInfo* pMeshInfo = pThisEntity->GetMeshInfo();
	if (!pMeshInfo) 
		return;

	vector<MCapsule> vecCapsules;
	if (pMeshInfo->IsSimple())
	{
		MCapsule thisCapsule = pMeshInfo->GetSimpleCapsule();
		thisCapsule.bottom += pThisEntity->GetPos();
		thisCapsule.top += pThisEntity->GetPos();
		vecCapsules.push_back(thisCapsule);
	}
	else if (pThisEntity->IsNPC())
	{
		GEntityNPC* pThisEntityNPC = ToEntityNPC(pThisEntity);
		int nCurCapsuleGroupIndex = pThisEntityNPC->GetCapsuleGroupIndex();
		if (!pMeshInfo->HasHitGroup(nCurCapsuleGroupIndex))
		{
			//ASSERT(0);  히트캡슐그룹이 바뀌는 동안 피격하면 해당될 수 있음
			return;
		}

		const CSHitGroup& CapsuleGroup = pMeshInfo->GetHitGroup(nCurCapsuleGroupIndex);
		float fHitCapsuleDistance = 0.0f;
		for (size_t i=0; i<CapsuleGroup.m_vecHitCapsules.size(); i++)
		{
			const CSHitCapsule& hitCapsule = CapsuleGroup.m_vecHitCapsules[i];

			if (hitCapsule.IsAlwaysActive() == false)
			{
				float fTalentElapsedTime = pThisEntity->GetModuleCombat()->GetTalentActElapsedTime();
				if (hitCapsule.CheckActiveTime(fTalentElapsedTime) == false) 
					continue;
			}

			MCapsule thisCapsule = hitCapsule.m_Capsule;
			vec3 dir = pThisEntity->GetDir();
			GMath::NormalizeZ0(dir);

			mat4 matTM;
			matTM.SetLocalMatrix(vec3::ZERO, -dir, vec3::AXISZ);

			thisCapsule.top = pThisEntity->GetPos() + thisCapsule.top * matTM;
			thisCapsule.bottom = pThisEntity->GetPos() + thisCapsule.bottom * matTM;

			vecCapsules.push_back(thisCapsule);
		}
	}
	
	RouteToClientActorRenderCapsule(uidPlayer, pThisEntity, vecCapsules);
}

bool GTestSystem::RouteAllNearActorCapsule( const MUID& uidPlayer )
{
	IF_PUBLISH_RETURN_VALUE(false);

	GEntityPlayer* const pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (!pPlayer)
		return false;

	GField* const pField = pPlayer->GetField();
	if (!pField)
		return false;

	class DoRouteAllNearActorCapsule : public GFieldGrid::Sector::IVisitor
	{
	public:
		DoRouteAllNearActorCapsule(GTestSystem&	testSystem, GField& field, const MUID& player)
			: m_testSystem(testSystem)
			, m_field(field)
			, m_player(player) {}
	private:
		void OnVisit(const MUID& uid)
		{
			GEntityNPC* const pActor = m_field.FindNPC(uid);
			if (!pActor)
				return;

			m_testSystem.ClientRenderActorCapsule(m_player, pActor);
		}
	private:
		GTestSystem&	m_testSystem;
		GField&			m_field;
		const MUID&		m_player;
	};
	pField->GetGrid().GetEntitySelector()->VisitByPos(
		DoRouteAllNearActorCapsule(*this, *pField, uidPlayer), ETID_NPC, pPlayer->GetPos(), 1);

	return true;
}

bool GTestSystem::RouteActorPos( MUID uidPlayer )
{
	IF_PUBLISH_RETURN_VALUE(false);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (!pPlayer)
		return false;

	GField* pField = pPlayer->GetField();
	if (!pField)
		return false;

	vector<TD_DEBUG_POS> vecTDPositions;
	class MakeTDPositions : public GFieldGrid::Sector::IVisitor
	{
	public:
		MakeTDPositions(GField& field,vector<TD_DEBUG_POS>& positions) : m_field(field), m_positions(positions) {}

	private:
		void OnVisit(const MUID& uid)
		{
			GEntityActor* const pActor = m_field.FindActor(uid);
			if (!pActor)
				return;

			TD_DEBUG_POS infoDebug;
			infoDebug.vPoint = pActor->GetPos();
			infoDebug.vDir = pActor->GetDir();
			infoDebug.uidTarget = MUID::ZERO;
			infoDebug.uidOwner = uid;

			GEntityNPC* const toNPC = dcast<GEntityNPC*>(pActor);
			if (NULL != toNPC)
			{
				GEntityActor* const pTarget = toNPC->GetTargetActor();

				if (pTarget)
					infoDebug.uidTarget = pTarget->GetUID();
			}

			m_positions.push_back(infoDebug);
		}

	private:
		GField&					m_field;
		vector<TD_DEBUG_POS>&	m_positions;
	};
	pField->GetGrid().GetEntitySelector()->VisitByPos(MakeTDPositions(*pField, vecTDPositions), ETID_ACTOR,  pPlayer->GetPos(), 1);

	MCommand* pNewCommand = MakeNewCommand(MC_DEBUG_STRING, uidPlayer, 4, 
		NEW_WSTR(L"UpdatePosTrace"), 
		NEW_INT(0), 
		NEW_WSTR(L""), 
		NEW_BLOB(vecTDPositions));
	gsys.pCommandCenter->PostCommand(pNewCommand);

	return true;
}

void GTestSystem::Update( float fDelta )
{
	for (map<MUID, PosSendInfo>::iterator it = m_mapSubscriptors_ActorPos.begin();
		 it != m_mapSubscriptors_ActorPos.end();)
	{
		PosSendInfo& info = it->second;
		if (!info.rgrTick.IsReady(fDelta))
			continue;

		if (!RouteActorPos(info.uidSubscriptors))
		{
			it = m_mapSubscriptors_ActorPos.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (map<MUID, PosSendInfo>::iterator it = m_mapSubscriptors_ActorHitCapsule.begin();
		it != m_mapSubscriptors_ActorHitCapsule.end();)
	{
		PosSendInfo& info = it->second;
		if (!info.rgrTick.IsReady(fDelta))
			continue;

		if (!RouteAllNearActorCapsule(info.uidSubscriptors))
		{
			it = m_mapSubscriptors_ActorHitCapsule.erase(it);
		}
		else
		{
			++it;
		}
	}
	
}
