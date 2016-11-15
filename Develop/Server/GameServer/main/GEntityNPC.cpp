#include "stdafx.h"
#include "GEntityNPC.h"
#include "GField.h"
#include "GCommandTable.h"
#include "GModuleAI.h"
#include "GMessageID.h"
#include "GFieldInfo.h"
#include "GSpawnManager.h"
#include "GNPCMgr.h"
#include "GEntityPlayer.h"
#include "GCalculator.h"
#include "GExpCalculator.h"
#include "GQuestSystem.h"
#include "GModuleCombat.h"
#include "GNPCInfoMgr.h"
#include "GTask_Move.h"
#include "GConst.h"
#include "GTalent.h"
#include "GPartySystem.h"
#include "GPathFinder.h"
#include "GLootSystem.h"
#include "GBuffInfo.h"
#include "GInteractionSystem.h"
#include "GFieldSystem.h"
#include "GFatigueSystem.h"
#include "GNPCDebugger.h"
#include "GNPCPathFinder.h"
#include "GJobRunner.h"
#include "GColtMgr.h"
#include "GSoulSystem.h"
#include "GCommand.h"
#include "GTask_Talent.h"
#include "GJob.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "GScriptGroup_NPC.h"
#include "GNullGlueNPC.h"
#include "GGlueNPC.h"
#include "GPlayerQuest.h"
#include "GPlayerQuests.h"
#include "GModuleBuff.h"
#include "GJobMgr.h"
#include "GBehaviorStateMachine.h"
#include "GHateTable.h"
#include "GNPCDecay.h"
#include "GNPCInteraction.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GFactionSystem.h"
#include "GNPCSwallow.h"
#include "GFactionRelationInfo.h"
#include "GFactionRelationInfoMgr.h"
#include "CSFactionInfo.h"
#include "GNPCEventSender.h"
#include "GNPCQuest.h"
#include "GNPCSensorChecker.h"
#include "GNPCEscortQuests.h"
#include "GTestSystem.h"
#include "GRelationChecker.h"
#include "GNPCWaitFor.h"
#include "GNPCTarget.h"
#include "GNPCMinion.h"
#include "GNPCHitcapsuleGroup.h"
#include "GNPCRage.h"
#include "GPlayerNPCIconSender.h"
#include "GNPCInteractor.h"
#include "GNPCStress.h"
#include "CSCharHelper.h"
#include "GNPCPathContext.h"
#include "GNPCBParts.h"
#include "GNPCMove.h"
#include "GNPCAssist.h"
#include "GNPCFirstStrike.h"
#include "GAIState.h"
#include "GServer.h"
#include "GNPCShopInfoMgr.h"
#include "GFieldGrid.h"
#include "GCommandCenter.h"
#include "GPlayerObjectManager.h"
#include "GNPCInfoHelper.h"
#include "GNPCMode.h"
#include "GNPCRewardOwner.h"
#include "GExpSystem.h"
#include "GNPCRewarder.h"
#include "CSMotionFactorHelper.h"
#include "GNPCSetterForTest.h"
#include "GNPCLuaCallback.h"
#include "CCommandResultTable.h"
#include "GLootEnder.h"
#include "GScriptTimer.h"
#include "GModuleMovable.h"
#include "GDBManager.h"
#include "GNPCCombatAnticheat.h"
#include "GQObjUpdater.h"
#include "GAIMonitorPort.h"
#include "GQPNPCPosSender.h"
#include "GFieldNPCSession.h"
#include "GTimeCalculator.h"
#include "GLootInfoMgr.h"


#define NPC_MOVING_DELAY_TIME	(1.0f)
const int DEFAULT_YELL_RANGE = 800;

map<int,int> GEntityNPC::m_mapUserDataSize;


GEntityNPC* ToEntityNPC( GEntity* pEntity )
{
	VALID_RET(pEntity, NULL);
	return static_cast<GEntityNPC*>(pEntity);
}

//////////////////////////////////////////////////////////////////////////
GEntityNPC::GEntityNPC() 
: GEntityActor()
, m_pNPCInfo(NULL)
, m_pModuleAI(NULL)
, m_pModuleMovable(NULL)
, m_pSpawnInfo(NULL)
, m_nStaticID(0)
, m_pJobRunner(NULL)
, m_isThinkable(true)
, m_nGroupID(INVALID_ID)
, m_pNPCAssist(NULL)
, m_pScriptTimer(NULL)
, m_pNPCBParts(NULL)
, m_pNPCSensorChecker(NULL)
, m_nSpawnID(INVALID_ID)
, m_EXPRewardable(true)
, m_QuestRewardable(true)
{
	m_nType |= ET_NPC;
	m_nTypeID = ETID_NPC;

	m_pJobRunner = new GJobRunner();
	m_pModuleAI = new GModuleAI(this, m_pJobRunner);
	m_pModuleMovable = new GModuleMovable(this);
	m_pPathFinder = new GNPCPathFinder(this);
	m_pDebugger = new GNPCDebugger(this);
	m_pHateTable = new GHateTable(this);
	m_pNPCDecay = new GNPCDecay(this);
	m_pNPCWaitFor = new GNPCWaitFor(this);
	m_pNPCTarget = new GNPCTarget(this);
	m_pNPCInteraction = new GNPCInteraction(this);
	m_pNPCLoot = new GNPCLoot(this);
	m_pSwallow = new GNPCSwallow(this);
	m_pNPCQuest = new GNPCQuest(this);
	m_pNPCMinion = new GNPCMinion(this);
	m_pNPCHitcapsuleGroup = new GNPCHitcapsuleGroup(this);
	m_pNPCRage = new GNPCRage(this);
	m_pNPCStress = new GNPCStress(this);
	m_pNPCPathContext = new GNPCPathContext(this);
	m_pNPCMove = new GNPCMove(this);
	m_pNPCFirstStrike = new GNPCFirstStrike(this);
	m_pNPCMode = new GNPCMode(this);
	m_pNPCRewardOwner = new GNPCRewardOwner(this);
	m_pNPCRewarder = new GNPCRewarder();
	m_pNPCSetterForTest = new GNPCSetterForTest(this);
	m_pNPCLuaCallback = new GNPCLuaCallback(this);
	m_pNPCCombatAnticheat = new GNPCCombatAnticheat(this);
	m_pQPNPCPosSender = new GQPNPCPosSender(this);

	GetNPCStatusForced().nAA = NAAT_INVALID;
	GetNPCStatusForced().nAttackable = NAT_INVALID;
}

GEntityNPC::~GEntityNPC()
{
	SAFE_DELETE(m_pModuleAI);
	SAFE_DELETE(m_pModuleMovable);
	SAFE_DELETE(m_pDebugger);
	SAFE_DELETE(m_pPathFinder);
	SAFE_DELETE(m_pJobRunner);
	SAFE_DELETE(m_pHateTable);
	SAFE_DELETE(m_pNPCDecay);
	SAFE_DELETE(m_pNPCWaitFor);
	SAFE_DELETE(m_pNPCTarget);
	SAFE_DELETE(m_pNPCInteraction);
	SAFE_DELETE(m_pNPCLoot);
	SAFE_DELETE(m_pSwallow);
	SAFE_DELETE(m_pNPCQuest);
	SAFE_DELETE(m_pNPCSensorChecker);
	SAFE_DELETE(m_pNPCMinion);
	SAFE_DELETE(m_pNPCHitcapsuleGroup);
	SAFE_DELETE(m_pNPCRage);
	SAFE_DELETE(m_pNPCStress);
	SAFE_DELETE(m_pNPCPathContext);
	SAFE_DELETE(m_pNPCBParts);
	SAFE_DELETE(m_pNPCMove);
	SAFE_DELETE(m_pNPCAssist);
	SAFE_DELETE(m_pNPCFirstStrike);
	SAFE_DELETE(m_pNPCMode);
	SAFE_DELETE(m_pNPCRewardOwner);
	SAFE_DELETE(m_pNPCRewarder);
	SAFE_DELETE(m_pNPCSetterForTest);
	SAFE_DELETE(m_pNPCLuaCallback);
	SAFE_DELETE(m_pScriptTimer);
	SAFE_DELETE(m_pNPCCombatAnticheat);
	SAFE_DELETE(m_pQPNPCPosSender);
}

void GEntityNPC::RegisterModules()
{
	GEntityActor::RegisterModules();

	m_pModuleAI->Initialize();
	m_Modules.AddModule(m_pModuleAI, true);

	m_pModuleMovable->Initialize();
	m_Modules.AddModule(m_pModuleMovable, true);
}

void GEntityNPC::UnregisterModlues()
{
	m_Modules.RemoveModule(m_pModuleAI);
	m_Modules.RemoveModule(m_pModuleMovable);
	GEntityActor::UnregisterModlues();
}

void GEntityNPC::OnEnter()
{
	VALID(m_pField);
	VALID(m_pField->GetInfo());

	if (m_pNPCInfo)
	{
		m_ChrStatus.nHP = GetMaxHP();
		m_ChrStatus.nEN = GetMaxEN();
		m_ChrStatus.nSTA = GetMaxSTA();

		// 동적 생성할 클래스 여기서 생성합니다.
		if (m_pNPCInfo->m_bAutoAssist)
		{
			m_pNPCAssist = new GNPCAssist(this);
		}

		if (m_pNPCInfo->IsUsingScriptTimer())
		{
			m_pScriptTimer = new GScriptTimer();
		}

		// 몬스터일 때만 브레이커블 파츠 사용함
		if (m_pNPCInfo->nNpcType == NPC_TYPE_MONSTER)
		{
			m_pNPCBParts = new GNPCBParts(this);
		}

		if (m_pNPCInfo->nNpcType == NPC_TYPE_MONSTER ||
			m_pNPCInfo->nNpcType == NPC_TYPE_NPC)
		{
			m_pNPCSensorChecker = new GNPCSensorChecker(this);
		}
	}
	else
	{
		OnHandle_NoNPCInfo();
	}

	// 진입지역을 홈포인트로 지정
	SetHomePoint(GetPos(), GetDir());

	m_pPathFinder->Create(m_pField->GetInfo()->m_nZoneID);
	
	GEntityActor::OnEnter();

	// AI 초기화 및 시작 LOD 결정
	m_pModuleAI->Init();
	
	if (m_pNPCInfo->m_isMajorLodNPC)
	{
		m_pModuleAI->OnMakeMajorLodNPC();
	}
	else if (0 == m_pField->GetPlayerQty())
	{
		m_pModuleAI->OnSpawnEmptyField();
	}
	else
	{
		GFieldGrid::EntitySelector* grid_selector = 
			m_pField->GetGrid().GetEntitySelector();
		
		if (grid_selector->IsEmptyPlayers(GetPos()))
		{	
			m_pModuleAI->OnSpawnEmptySector();
		}
		else
		{
			m_pModuleAI->OnSpawnPlayerSector();
		}		
	}
	

	GetDelegator()->RunSpawnScript(GetSpawnInfo());
}

void GEntityNPC::OnLeave()
{
	m_pPathFinder->Destroy();

	GEntityActor::OnLeave();
}

MCommand* GEntityNPC::CreateFieldInCmd()
{
	// 서버 준비 중 NPC가 추가될때는 라우팅하지 않는다.
	if (gsys.pServer == NULL || !gsys.pServer->IsCreated())	return NULL;

	TD_UPDATE_CACHE_NPC td_npc_info;
	MakeTDCacheInfo(td_npc_info);

	return MakeNewCommand(MC_FIELD_IN_NPC,
		1, 
		NEW_SINGLE_BLOB( &td_npc_info, sizeof(TD_UPDATE_CACHE_NPC) )
	);
}

MCommand* GEntityNPC::CreateFieldOutCmd()
{
	return MakeNewCommand(MC_FIELD_OUT_NPC, 1, NEW_USHORT(m_nUIID));
}

void GEntityNPC::OnVisitedNewSectorPC(const MUID& newSightedPC,MCommand* const fieldInCmd)
{
	__super::OnVisitedNewSectorPC(newSightedPC, fieldInCmd);

	if (!m_pNPCInfo->bInteract) return;

	GEntityPlayer* const pPlayer = gmgr.pPlayerObjectManager->GetEntity(newSightedPC);
	if (NULL == pPlayer)
		return;

	pPlayer->GetNPCIconSender().SendByNPC(this);
}

bool GEntityNPC::SetPos(const vec3& pos) 
{ 
	if (!GEntityActor::SetPos(pos))
		return false;

	return m_pPathFinder->MoveAgentTo(pos);
}

void GEntityNPC::OnHandle_NoNPCInfo()
{
	m_ChrStatus.nHP = 100;
	m_ChrStatus.nEN = 100;
	m_ChrStatus.nSTA = 100;

	_ASSERT(0);
}

void GEntityNPC::MakeTDCacheInfo(TD_UPDATE_CACHE_NPC& out)
{
	if (!GetNPCInfo())
	{
		return;
	}

	out.nStatusFlag = 0;
	out.uid = m_UID;
	out.nUIID = m_nUIID;
	out.nNPCID = (m_pNPCInfo) ? m_pNPCInfo->nID : -1;
	out.vPos = m_vPos;
	out.nSpeed = (uint16)GetNPCMove().GetSpeed();
	out.svDir = GetDir();
	out.nLevel = GetLevel();
	memset(out.Buffs, 0, sizeof(out.Buffs));
	GetBuffList(out.Buffs);

	if (GetNPCStatus().nAttackable == NAT_ALWAYS)
	{
		SetBitSet(out.nStatusFlag, UNS_ATTACKABLE_ALWAYS);
	}
	else if (GetNPCStatus().nAttackable == NAT_FACTION)
	{
		SetBitSet(out.nStatusFlag, UNS_ATTACKABLE_FACTION);
	}

	GEntityActor* pTargetActor = GetTargetActor();
	if (pTargetActor)
	{
		out.uidTarget = pTargetActor->GetUID();
	}	


	bool setup_tar = false;

	GJobMgr* pJobMgr = m_pModuleAI->GetJobMgr();
	GJob* pJob = pJobMgr->GetCurrJob();
	GTask* pTask = pJobMgr->GetCurrTask();

	if (pJob && pTask)
	{
		switch (pTask->GetID())
		{
		case GTASK_MOVE:
			{
				GTask_Move* pT = static_cast<GTask_Move*>(pTask);
				if ( pJob->IsSuspended())	out.vTarPos = m_vPos;
				else						out.vTarPos = pT->GetGoalPos();

				if (out.vTarPos != vec3::ZERO) //GetCurrentPathNodePos
				{
					out.svDir = pT->GetOwner()->GetDir();
					GetNPCMove().MakeTD(out);

					setup_tar = true;
				}
			}
			break;
		case GTASK_TALENT:
			{
				GTask_UseTalent* pT = static_cast<GTask_UseTalent*>(pTask);
				out.nTalentID = pT->GetTalentID();
			}break;
		}
	}

	if (!setup_tar)
	{
		out.vTarPos = m_vPos + m_vDir;
	}

	// npc 초기 상태
	{
		if (m_pNPCBParts) m_pNPCBParts->MakeTD(out);

		if (IsDead())
		{
			SetBitSet(out.nStatusFlag, UNS_DEAD);
		}

		switch(GetNPCMode().GetMode())
		{
		case 1: 
			SetBitSet(out.nStatusFlag, UNS_MODE1); break;
		case 2:
			SetBitSet(out.nStatusFlag, UNS_MODE2); break;
		case 3:
			SetBitSet(out.nStatusFlag, UNS_MODE3); break;
		case 4:
			SetBitSet(out.nStatusFlag, UNS_MODE4); break;
		case 5:
			SetBitSet(out.nStatusFlag, UNS_MODE5); break;

		}
	}
}

void GEntityNPC::MakeTDCacheInfo(GEntityPlayer* pReqPlayer, TD_UPDATE_CACHE_NPC& out)
{
	if (NULL == pReqPlayer) return;

	MakeTDCacheInfo(out);

	if (true == IsDead() && false == GetNPCLoot().GetDropList().IsEmpty())
	{
		if (true == GetNPCLoot().GetDropList().IsViewableCID(pReqPlayer->GetCID()))
		{
			SetBitSet(out.nStatusFlag, UNS_LOOTABLE);
		}
	}	
}

void GEntityNPC::OnUpdate(float fDelta)
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(104);

	m_Updater.UpdatePre(this, fDelta);

	GEntityActor::OnUpdate(fDelta);

	m_Updater.UpdatePost(this, fDelta);
}

bool GEntityNPC::SetTimer( int nTimerID, float fIntervalTime, bool bRepeat/*=false*/ )
{
	if (!m_pScriptTimer) return false;

	return m_pScriptTimer->SetTimer(nTimerID, fIntervalTime, bRepeat);
}

bool GEntityNPC::KillTimer( int nTimerID )
{
	if (!m_pScriptTimer) return false;

	return m_pScriptTimer->KillTimer(nTimerID);
}

void GEntityNPC::Despawn(bool bSpawnEnable)
{
	m_pJobRunner->Despawn(m_pModuleAI, bSpawnEnable);	
}

void GEntityNPC::DespawnNow( bool bSpawnEnable )
{
	doDespawn(bSpawnEnable);
}

void GEntityNPC::Die(GEntityActor* pKiller)
{
	m_pJobRunner->Die(m_pModuleAI, pKiller?pKiller->GetUID():MUID::Invalid());
}

void GEntityNPC::Assist(GEntityNPC* pAlliedNPC)
{
	VALID(pAlliedNPC);
	m_pJobRunner->Assist(m_pModuleAI, pAlliedNPC->GetUID());
}

void GEntityNPC::RouteInEntity(const vector<MUID>& vecPlayers)
{
	// 서버 준비 중 NPC가 추가될때는 라우팅하지 않는다.
	if (gsys.pServer == NULL || !gsys.pServer->IsCreated())	return;

	TD_UPDATE_CACHE_NPC td_npc_info;
	MakeTDCacheInfo(td_npc_info);

	MCommand* pNewCmd = MakeNewCommand(MC_FIELD_IN_NPC, 
		1, 
		NEW_SINGLE_BLOB(&td_npc_info, sizeof(TD_UPDATE_CACHE_NPC)));
	pNewCmd->AddReceiver(vecPlayers);
	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GEntityNPC::RouteOutEntity( const vector<MUID>& vecPlayers )
{
	MCommand* pNewCmd = MakeNewCommand(MC_FIELD_OUT_NPC, 1, NEW_USHORT(m_nUIID));
	pNewCmd->AddReceiver(vecPlayers);
	gsys.pCommandCenter->PostCommand(pNewCmd);
}

MCommand* GEntityNPC::MakeCmd_Whois(GEntitySync* pReqSync)
{
	GEntityPlayer* pPlayer = ::ToEntityPlayer(pReqSync);
	if (pPlayer == NULL)	return NULL;
	
	TD_UPDATE_CACHE_NPC td_npc_info;
	MakeTDCacheInfo(pPlayer, td_npc_info);

	MCommand* pNewCmd = MakeNewCommand(MC_FIELD_IN_NPC, 1, NEW_SINGLE_BLOB(&td_npc_info, sizeof(TD_UPDATE_CACHE_NPC)));
	return pNewCmd;
}

bool GEntityNPC::IsEnemy(MUID uidEnemy)
{
	return GetHateTable().IsExist(uidEnemy);
}

void GEntityNPC::doMakePeace()
{
	__super::doMakePeace();

	// 헤이트 테이블 목록 초기화
	GetHateTable().Reset();

	// 전투 종료
	GMessage msg(GMSG_AI_EVT_ENDCOMBAT);
	GetModuleAI()->Message(msg);
}

void GEntityNPC::doMakePeaceForce()
{
	__super::doMakePeace();

	// 헤이트 테이블 목록 초기화
	GetHateTable().Reset();

	// 전투 종료
	GetModuleAI()->GetBehaviorFSM()->ChangeState(AI_BEHAVIOR_MISSION);
}

bool GEntityNPC::IsNowCombat() const
{
	if (IsNowReturn())
		return true; // 전투 종료후 복귀 때까지는 전투상태 (BehaviorJob Mode로 처리되야 함)

	GBehaviorStateMachine* pFSM = m_pModuleAI->GetBehaviorFSM();
	return pFSM->GetCurrentStateID() == AI_BEHAVIOR_COMBAT;
}


bool GEntityNPC::IsNowReturn() const
{
	GBehaviorStateMachine* pFSM = m_pModuleAI->GetBehaviorFSM();
	return pFSM->GetCurrentStateID() == AI_BEHAVIOR_RETURN;
}

MUID GEntityNPC::GetTarget()
{
	return GetHateTable().GetTarget();
}

GEntityActor* GEntityNPC::GetTargetActor()
{
	return FindActor(GetHateTable().GetTarget());
}

bool GEntityNPC::doDamage(MUID uidAttacker, DAMAGE_ATTRIB nDamageType, uint32 nDamage, EFFECT_SOURCE_TYPE nEffectSourceType, int nEffectSourceID)
{
	VALID_RET(GetNPCInfo(), false);
	
	m_LastDamagedInfo.nType = nDamageType;
	m_LastDamagedInfo.nAmount = nDamage;

	if (uidAttacker.IsValid())
	{
		// 이벤트 전송
		EVENT_ATTACKED_INFO info;
		info.uidAttacker =	uidAttacker;
		info.nType =	nDamageType;
		info.nDamage =		nDamage;
		info.nEffectSourceType = nEffectSourceType;
		info.nEffectSourceID = nEffectSourceID;

		GMessage msg(GMSG_AI_EVT_ATTACKED, &info);
		Message(msg);
	}

	return GEntityActor::doDamage(uidAttacker, nDamageType, nDamage, nEffectSourceType, nEffectSourceID);
}

void GEntityNPC::DoMotionFactor(MF_STATE nMF, int nWeight, GEntityActor* pOpponent, bool bKnockbackDirSyncToOpponentDir)
{
	GEntityActor::DoMotionFactor(nMF, nWeight, pOpponent, bKnockbackDirSyncToOpponentDir);

	if (nMF == MF_NONE || CSMotionFactorHelper::IsFakeMotionFactor(nMF))
	{
		return;
	}

	m_pModuleAI->GetJobMgr()->CancelCurrJob();
	m_pModuleAI->GetColt()->RunColtReaction(this, pOpponent);

	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(GetUID()))
	{
		GAIMonitorPort::Inst().SendEventMessageToViewer("ApplyMotionfactor: %s", MLocale::ConvUTF16ToAnsi(MF_STATE_NAME[nMF]).c_str());
	}
}

void GEntityNPC::doTryHit(GEntityActor* pAttacker, GTalentInfo* pTalentInfo, int nAddtiveDamage, uint16 nCheckTime, int nCapsuleGroup, int nCapsuleIndex)
{
	GRelationChecker relationCheker;

	if (!IsHittable(pAttacker, pTalentInfo)) 
		return;

	// 플레이어나 엔피씨로부터 공격을 받을수 있는 대상일때
	if (!IsNowInvincibility() &&
		true == relationCheker.IsEnemy(pAttacker, this))
	{
		EVENT_ATTACKED_INFO info;
		info.uidAttacker =	pAttacker->GetUID();
		info.nType =		DA_NONE;
		info.nDamage =		0;

		GMessage msg(GMSG_AI_EVT_TRY_ATTACKED, &info);
		Message(msg);
	}

	if((NULL != pAttacker) && (NULL != pTalentInfo))
		GetScriptNPC()->OnTryHit(dynamic_cast<GGlueNPC*>(GetDelegator()), pAttacker->GetDelegator(), pTalentInfo->m_nID);

	GEntityActor::doTryHit(pAttacker, pTalentInfo, nAddtiveDamage, nCheckTime, nCapsuleGroup, nCapsuleIndex);
}

void GEntityNPC::doDieByGather(GEntityPlayer* pPlayer, int nLootID)
{
	m_pNPCRewarder->RewardByGather(pPlayer, nLootID, this);
	doDie();
}

void GEntityNPC::doDie()
{	
	VALID(GetNPCInfo());

	if (GetNPCInfo()->IsBoss())
	{
		vector<pair<MUID, HatePoint>> vPlayerHatePoint;
		vPlayerHatePoint.reserve(GetHateTable().GetQty());
		GetHateTable().GetHighestTargets(GNPC_KILLER::MAX_COUNT, vPlayerHatePoint);

		wstring				strRegDate = GTimeCalculator::GetTimeAsString(GTimeSec(0));
		vector<GNPC_KILLER>	vKiller;
		GEntityPlayer*		pPlayer;

		vKiller.reserve(vPlayerHatePoint.size());

		for each (const pair<MUID, HatePoint>& PlayerHatePoint in vPlayerHatePoint)
		{
			pPlayer = gmgr.pPlayerObjectManager->GetEntity(PlayerHatePoint.first);
			if (NULL == pPlayer)
				continue;

			vKiller.push_back(GNPC_KILLER(pPlayer->GetAID()
				, pPlayer->GetCID()
				, pPlayer->GetPlayerInfo()->nPlayTimeSec
				, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
				, pPlayer->GetLevel()));
		}

		gsys.pDBManager->NPCKilledLog(strRegDate, GetID(), vKiller);
	}

	m_pNPCRewarder->RewardByDie(this);
	gsys.pTestSystem->DespawnLog(GetUID(), L"DecayByDie");	
	

	GMessage msg(GMSG_AI_EVT_DEAD);
	Message(msg);
}


void GEntityNPC::doDespawn(bool bSpawnEnable)
{
	VALID(m_pField);

	OnDespawn();

	GDebugReporter::Record record;
	record.uidNPC = GetUID();
	record.strName = GetNPCInfo()->strName;
	gsys.pTestSystem->DespawnLog(GetUID(), __FUNCTIONW__);

	gsys.pInteractionSystem->GetNPCInteractor().End(this);
	gsys.pLootSystem->GetEnder().End(this);

	m_pModuleAI->GetJobMgr()->Clear();
	DeleteMe();	

	// 리스폰정보 초기화
	if (m_pSpawnInfo &&
		m_pField->GetSpawn()->IsSpawnedNPC(m_pSpawnInfo->nID, GetUID()))
	{
		m_pField->GetSpawn()->Enable(m_pSpawnInfo->nID, bSpawnEnable);
		m_pField->GetSpawn()->Reset(m_pSpawnInfo->nID);
	}
	
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_DESPAWN, 1, NEW_USHORT(m_nUIID));
	RouteToThisCell(pNewCommand);
}

void GEntityNPC::doAttack(MUID uidTarget, int nDamage)
{
	GEntityActor::doAttack(uidTarget, nDamage);

	GEntityActor* pTargetActor = FindActor(uidTarget);
	if (pTargetActor == NULL)		return;

	EVENT_START_COMBAT param;
	param.uidEnemy = uidTarget;
	GMessage msg(GMSG_AI_EVT_STARTCOMBAT, &param);
	Message(msg);
}

void GEntityNPC::doAssist(GEntityNPC* pAlliedNPC)
{
	_ASSERT(pAlliedNPC);
	if (!pAlliedNPC)	return;
	if (IsNowCombat()) return;

	GEntityActor* pTarget = pAlliedNPC->GetTargetActor();
	if (!pTarget)	return;

	m_pModuleAI->SetAssist();
	
	EVENT_START_COMBAT param;
	param.uidEnemy = pTarget->GetUID();
	GMessage msg(GMSG_AI_EVT_STARTCOMBAT, &param);
	Message(msg);
}


void GEntityNPC::doYell(int nRange)
{
	if (0 == nRange)
		nRange = DEFAULT_YELL_RANGE;

	MUID uidSender = GetUID();
	MUID uidTarget = GetHateTable().GetTarget();
	if (0 == uidTarget)	return;

	EVENT_YELL_INFO param;
	param.uidSender = uidSender;
	param.uidTarget = uidTarget;

	GMessage msg(GMSG_AI_EVT_YELL, &param);

	GNPCEventSender npcEventSender(this);
	npcEventSender.SendEvent(msg, nRange);
}

void GEntityNPC::_HandleQuestTrigger(const SENSOR_INFO& infoSensor)
{
	VALID(m_pField);

	GFieldGrid::EntitySelector* grid_selector = 
		m_pField->GetGrid().GetEntitySelector();

	GVectorMUID vecNeighborPlayers;
	grid_selector->GetPlayers(vecNeighborPlayers.Vector(), GetPos());

	for each (const MUID& nPlayerUID in vecNeighborPlayers.Vector())
	{
		GEntityPlayer* pPlayer = FindPlayer(nPlayerUID);
		if (NULL == pPlayer) continue;

		if (GConst::QUEST_SHARE_DISTANCE < DistanceTo(pPlayer)) continue;

		gsys.pQuestSystem->GetQObjUpdater().UpdEscortQObj(pPlayer, GetID(), infoSensor.nID);
	}
}

bool GEntityNPC::SetNPCInfo( GNPCInfo* pNPCInfo )
{
	VALID_RET(pNPCInfo, false);

	m_pNPCInfo = pNPCInfo;
	m_pChrInfo = m_pNPCInfo;

	// NPC모드 초기화
	GetNPCMode().Init(GetNPCInfo()->nSpawningMode);

	GNPCInfo* pNewInfo = GetNPCInfo();

	// 히트캡슐그룹 등록
	GetNPCHitcapsuleGroup().Init(pNPCInfo);

	// 공격 가능여부 설정
	ChangeAttackable(pNewInfo->nInitAttackable);
	m_NPCStatus.nAA = pNewInfo->nAA;

	// 면역 처리자 초기화
	m_ImmuneHandler.Init(pNewInfo->ImmuneInfo);

	// 헤이트테이블 초기화
	GetHateTable().Init(pNewInfo->m_fHateExpiredTime);

	return true;
}

bool GEntityNPC::InitFromNPCInfo( GNPCInfo* pNPCInfo, const SPAWN_INFO* pSpawnInfo, int nStaticID/*=0*/ )
{
	if (!SetNPCInfo(pNPCInfo))		return false;

	m_nStaticID = nStaticID;
	m_pSpawnInfo = pSpawnInfo;

	InitUserData();
	
	// 레벨을 랜덤으로 만든다.
	m_NPCStatus.nLevel = m_pNPCInfo->nLevel;
	if (m_NPCStatus.nLevel < m_pNPCInfo->nMaxLevel)
	{
		m_NPCStatus.nLevel = GMath::RandomNumber(m_pNPCInfo->nLevel, m_pNPCInfo->nMaxLevel);
	}

	if (GNPCInfoHelper::IsThinkableObject(m_pNPCInfo) == false)
	{
		SetThinkable(false);
	}

	_FillRegenVar();

	SetTauntable(pNPCInfo->bTauntable);

	return true;
}


void GEntityNPC::ReturnToHomePoint()
{
	if (m_HomePoint.vPoint == vec3::ZERO) 
		return; // 홈포인트 지정되지 않음

	if (m_HomePoint.vPoint.IsEqual(GetPos())) 
		return; // 홈포인트에 위치하고 있으면 무시

	// 홈포인트로 이동
	m_pJobRunner->Move(m_pModuleAI, m_HomePoint.vPoint, m_HomePoint.vDir);
}

void GEntityNPC::ReturnToSpawnPoint()
{
	GJobMgr* pJobMgr = m_pModuleAI->GetJobMgr();
	pJobMgr->Clear();

	if (!m_pJobRunner)	return;
	m_pJobRunner->Move(m_pModuleAI, GetSpawnPos());
}

bool GEntityNPC::doGuard(int nGuardTalentID)
{
	if (!GEntityActor::doGuard(nGuardTalentID))
		return false;

	SetDirForwardToEnemy();

	// 방어 메세지 전달
	MCommand* pNewCmd = MakeNewCommand(MC_ACTION_GUARD, m_UID, 3, NEW_USHORT(m_nUIID), NEW_SVEC2(GetDir()), NEW_VEC(GetPos()));
	RouteToThisCell(pNewCmd);

	return true;
}

bool GEntityNPC::doGuardReleased()
{
	if (!GEntityActor::doGuardReleased())
		return false;

	// 방어 해제 메세지 전달
	MCommand* pNewCmd = MakeNewCommand(MC_ACTION_GUARD_RELEASED, m_UID, 1, NEW_USHORT(m_nUIID));
	RouteToThisCell(pNewCmd);

	return true;
}

bool GEntityNPC::IsNowRunningCancelableTalent()
{
	GJobMgr* pJobMgr = m_pModuleAI->GetJobMgr();
	GTask* pCurTask = pJobMgr->GetCurrTask();
	
	if (!pCurTask || pCurTask->GetID() != GTASK_TALENT)	return true;

	GTask_UseTalent* pTaskTalent = static_cast<GTask_UseTalent*>(pCurTask);
	const GTalentInfo* pTalentInfo = pTaskTalent->GetTalentInfo();

	if (!pTalentInfo)	return true;

	return pTalentInfo->m_bCancelable;
}

void GEntityNPC::SetDirForwardToEnemy(bool bRoute)
{
	GEntityActor* pTarget = GetTargetActor();
	if (!pTarget)		return;

	SetDirForwardToActor(pTarget, bRoute);
}

void GEntityNPC::RouteMove(vec3& tarPos)
{
//	mlog("GEntityNPC::RouteMove - %.1f %.1f %.1f\n", tarPos.x, tarPos.y, tarPos.z);
	GetNPCMove().RouteMove(tarPos);
}

void GEntityNPC::RouteMoveStop()
{
	GetNPCMove().RouteMoveStop();
}

//////////////////////////////////////////////////////////////////////////
//	모션팩터 관련
//////////////////////////////////////////////////////////////////////////

void GEntityNPC::OnBeginMotionFactor()		
{
	GJobMgr* pJobMgr = m_pModuleAI->GetJobMgr();
	pJobMgr->Suspend();
}

void GEntityNPC::OnEndMotionFactor()		
{
	// 넉다운이 끝날때는 일어서는 애니메이션 시간동안 대기
	// -> GEntityActor::DoMotionFactor 으로 이동함

	GJobMgr* pJobMgr = m_pModuleAI->GetJobMgr();
	pJobMgr->Resume();
}

GNPCInfo* GEntityNPC::GetNPCInfo() const
{
	NPC_MODE nMode = GetMode();
	if (nMode > 0) return static_cast<GNPCInfo*>(m_pNPCInfo->vecModes[nMode-1]);
	return m_pNPCInfo;
}

GNPCInfo* GEntityNPC::GetNPCInfoMaster() const
{
	return m_pNPCInfo;
}

CHR_INFO* GEntityNPC::GetChrInfo() const
{
	return GetNPCInfo();
}

bool GEntityNPC::IsHittable( GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	if (GEntityActor::IsHittable(pAttacker, pAttackTalentInfo) == false) return false;

	if (!m_pNPCInfo)
		return true;

	ALTITUDE_TYPE nAltitude = GetNPCInfo()->nAltitude;

	if (CSTalentInfoHelper::IsCollidableSkillType(pAttackTalentInfo->m_nSkillType))
	{
		// 근접 공격시 공중,물속,땅속은 판정 불가
		if (CSAltitudeHelper::IsAreaHittable(nAltitude) == false) 
			return false;
	}

	if (pAttackTalentInfo->m_HitFilter.bAltitude[nAltitude] == false) 
	{
		//mlog("GEntityNPC::IsHittable() - HitFilter bAltitude(%d) TalentID(%d)\n", m_pNPCInfo->nAltitude, pAttackTalentInfo->m_nID);
		return false;
	}

	if (GetActionState() == AS_MF)
	{
		MF_STATE nMF = GetCurrentMotionFactor();
		if (nMF >= 0)
		{
			if (pAttackTalentInfo->m_HitFilter.bMotionFactor[nMF] == false) 
			{
				//mlog("GEntityNPC::IsHittable() - HitFilter MF(%d) TalentID(%d)\n", nMF, pAttackTalentInfo->m_nID);
				return false;
			}
		}
	}

	// 연출중 검사
	{
		if (IsNowPuppet())
			return false;
	}

	// 세션 검사
	GFieldNPCSession* pSession =
		GetSession();
	if (pSession)
	{
		if (!pSession->IsHittable(pAttacker, this))
			return false;
	}

	return true;
}

GEntity* GEntityNPC::FindEntity(const MUID& uidEntity)
{
	VALID_RET(m_pField, NULL);
	return m_pField->FindEntity(uidEntity);
}

GEntityPlayer* GEntityNPC::FindPlayer(const MUID& uidPlayer)
{
	VALID_RET(m_pField, NULL);
	return m_pField->FindPlayer(uidPlayer);
}

GEntityPlayer* GEntityNPC::FindPlayerByCID(int nCID)
{
	VALID_RET(m_pField, NULL);
	return m_pField->FindPlayerByCID(nCID);
}

GEntityNPC* GEntityNPC::FindNPC(const MUID& uidNPC) const
{
	VALID_RET(m_pField, NULL);
	return m_pField->FindNPC(uidNPC);
}

int GEntityNPC::GetStress()
{
	return GetNPCStress().GetStress();
}

vec3 GEntityNPC::GetSpawnPos()
{
	if (!m_pSpawnInfo)
	{
		return m_HomePoint.vPoint;
	}

	return m_pSpawnInfo->vPoint;
}

float GEntityNPC::GetCombatRadius()
{
	if (!m_pSpawnInfo)
	{
		return GConst::NPC_DEFAULT_COMBAT_RADIUS;
	}

	return m_pSpawnInfo->fCombatRadius;
}

void GEntityNPC::ResetTickVars()
{
	m_TickVar.bProceedReactionColt = false;
	m_TickVar.LazyAttack.uidTarget.SetInvalid();
}

void GEntityNPC::doSay( const wchar_t* szText, float fMaintainTime )
{	
	MCommand* pNewCommand = MakeNewCommand(	MC_NPC_SAY, 4, 
											NEW_UID(GetUID()),
											NEW_INT(GetID()),
											NEW_WSTR(szText),
											NEW_FLOAT(fMaintainTime));
	RouteToThisCell(pNewCommand);
}

void GEntityNPC::doBalloon( const wchar_t* szText, float fMaintainTime )
{	
	MCommand* pNewCommand = MakeNewCommand(	MC_NPC_BALLOON, 4, 
											NEW_UID(GetUID()),
											NEW_INT(GetID()),
											NEW_WSTR(szText),
											NEW_FLOAT(fMaintainTime));

	RouteToThisCell(pNewCommand);
}

void GEntityNPC::doShout( const wchar_t* szText, float fMaintainTime )
{	
	MCommand* pNewCommand = MakeNewCommand(	MC_NPC_SHOUT, 4, 
											NEW_UID(GetUID()),
											NEW_INT(GetID()),
											NEW_WSTR(szText),
											NEW_FLOAT(fMaintainTime));
	RouteToField(pNewCommand);
}

void GEntityNPC::doNarration(const wchar_t* szText)
{
	MCommand* pNewCommand = MakeNewCommand(	MC_NPC_NARRATION, 1, 
											NEW_WSTR(szText));

	RouteToThisCell(pNewCommand);
}

void GEntityNPC::doSayAndNarration(const wchar_t* szText, float fMaintainTime )
{
	MCommand* pNewCommand = MakeNewCommand(	MC_NPC_SAY_AND_NARRATION, 4, 
		NEW_UID(GetUID()),
		NEW_INT(GetID()),
		NEW_WSTR(szText),
		NEW_FLOAT(fMaintainTime));

	RouteToThisCell(pNewCommand);
}

float GEntityNPC::GetRotateSpeed() const
{
	GNPCInfo* pNPCInfo = GetNPCInfo();
	if (pNPCInfo) return pNPCInfo->fRotateSpeed;
	return 0.0f;
}

bool GEntityNPC::IsNoneAttackable() 
{
	if (NAT_NONE == GetNPCStatus().nAttackable) return true;

	return false;
}

bool GEntityNPC::IsAlwaysAttackable()
{
	if (NAT_ALWAYS == GetNPCStatus().nAttackable) return true;

	return false;
}

bool GEntityNPC::IsFactionAttackable()
{
	if (NAT_FACTION == GetNPCStatus().nAttackable) return true;

	return false;
}


void GEntityNPC::ChangeAttackable(NPC_ATTACKABLE_TYPE nAttckable)
{
	if (GetNPCStatus().nAttackable == nAttckable) 
		return;

	GetNPCStatus().nAttackable = nAttckable;

	MCommand* pNewCommand = MakeNewCommand(MC_NPC_CHANGE_ATTACKABLE,
		2, 
		NEW_UID(GetUID()),
		NEW_INT(nAttckable));

	RouteToThisCell(pNewCommand);
}

void GEntityNPC::EnableInteract( bool bEnable )
{
	GetNPCInteraction().Enable();
}

void GEntityNPC::Move(vec3 vTarPos)
{
	m_pJobRunner->Move(m_pModuleAI, vTarPos);
}

void GEntityNPC::MoveToMarker(int nMarkerID)
{
	if (NULL == GetFieldInfo())
		return;

	const MARKER_INFO* pMInfo = GetFieldInfo()->FindMarker(nMarkerID);
	if (NULL == pMInfo)
		return;

	m_pJobRunner->MoveToMarker(m_pModuleAI, *pMInfo);
}

void GEntityNPC::MoveToSensor( unsigned long nSensorID )
{
	if (NULL == GetFieldInfo())
		return;

	const SENSOR_INFO* pSensorInfo = GetFieldInfo()->FindSensor(nSensorID);
	if (NULL == pSensorInfo)
		return;

	m_pJobRunner->MoveToSensor(m_pModuleAI, *pSensorInfo);
}

void GEntityNPC::Follow(GEntityActor* pActor, float fLeastDistance)
{
	m_pJobRunner->Follow(m_pModuleAI, pActor, fLeastDistance, 0.0f);	// Inifnity Follow
}

void GEntityNPC::Attack( GEntityActor* pTarget )
{
	m_pJobRunner->Attack(m_pModuleAI, pTarget->GetUID(), 0);
}

void GEntityNPC::Yell(int nRange)
{
	m_pJobRunner->Yell(m_pModuleAI, nRange);
}

bool GEntityNPC::UseTalent(int nTalentID, MUID uidOpponent, bool bCheckValidate)
{
	GEntityActor* pOpponent = FindActor(uidOpponent);
	if (!pOpponent)		return false;
	USETALENT_PARAM param;
	param.bCheckValidate = bCheckValidate;
	return m_pJobRunner->UseTalent(this, pOpponent, nTalentID, param);
}

bool GEntityNPC::UseTalentSelf(int nTalentID)
{
	return m_pJobRunner->UseTalent(this, this, nTalentID);
}

bool GEntityNPC::MoveToUseTalent(GEntityActor* pTargetActor, int nTalentID)
{
	return m_pJobRunner->MoveToTargetCloser(m_pModuleAI, pTargetActor, nTalentID);
}

bool GEntityNPC::MoveToUseTalentCloserWithDuration(GEntityActor* pTargetActor, int nTalentID, float fDuration)
{
	return m_pJobRunner->MoveToTargetCloser(m_pModuleAI, pTargetActor, nTalentID, fDuration);
}

bool GEntityNPC::IsRepairer()
{
	GNPCInteractionInfo& interactionInfo = GetNPCInfo()->m_InteractionInfo;

	for each (GNPCInteractionElementInfo* pIElementInfo in interactionInfo.m_vecNPCIEInfo)
	{
		if(	IT_NPCSHOP == pIElementInfo->m_nType &&
			true == gmgr.pNPCShopInfoMgr->IsRepairable(pIElementInfo->GetSinlgeAct()))
		{
			return true;
		}
	}

	return false;
}

void GEntityNPC::Warp(vec3 vecPos, vec3 vecDir, bool bRouteToMe)
{
	VALID(m_pModuleAI);

	if (!m_pJobRunner)
		return;

	m_pJobRunner->Warp(m_pModuleAI, vecPos, vecDir, bRouteToMe);
}

bool GEntityNPC::PushJob_GainBuff(int nBuffID)
{
	if (!m_pJobRunner)
		return false;

	m_pJobRunner->GainBuff(m_pModuleAI, nBuffID);		

	return true;
}

const int MAX_USER_DATA = 100;

void GEntityNPC::InitUserData()
{
	map<int,int>::iterator iter = m_mapUserDataSize.find(GetNPCInfo()->nID);
	if (iter == m_mapUserDataSize.end())		return;
	const int nUsrDataSize = iter->second;
	
	if (nUsrDataSize <= 0 || nUsrDataSize > MAX_USER_DATA) return;

	m_vecUserData.resize(nUsrDataSize, 0);
}

void GEntityNPC::OnHit( GHitInfo infoHit, bool bRouteHit )
{
	VALID(infoHit.pAttacker);
	VALID(infoHit.pVictim);

	__super::OnHit(infoHit, bRouteHit);

}

int GEntityNPC::GetID() const
{
	if (!GetNPCInfo()) return INVALID_ID;

	return GetNPCInfo()->nID;
}

void GEntityNPC::RouteFaceTo( vec3 vDir )
{
	assert(GetNPCInfo());
	if (!GetNPCInfo())	return;

	MCommand* pNewCommand = MakeNewCommand((MC_NPC_FACE_TO), 3, NEW_USHORT(GetUIID()), NEW_SVEC2(vDir), NEW_VEC(GetPos()));
	RouteToThisCell(pNewCommand);
}

void GEntityNPC::RouteChangeCombatTarget(MUID uidTarget)
{
	MCommand* pNewCommand = MakeNewCommand(MC_NPC_CHANGE_COMBAT_TARGET, 2, 
		NEW_UID(GetUID()), 
		NEW_UID(uidTarget));
	RouteToThisCell(pNewCommand);
}	

void GEntityNPC::UpdateBuff(float fDelta)
{
	// AI LOD에 의해 JobMgr가 업데이트되지 않는다면, 버프도 업데이트하지 않는다;
	if (GConst::AI_OPTIMIZATION && IsAILodEnableFiled())
	{
		if (AI_LOD_LEVEL_5 == m_pModuleAI->GetLodLevel()) return;
	}

	GEntityActor::UpdateBuff(fDelta);
}

bool GEntityNPC::IsThinkable( void )
{
	if (!m_isThinkable)
		return false; // 명시적으로 사고하지 않음
	
	if (!GConst::NPC_AI_RUNNING) 
		return false; // AI 작동이 허락되어있지 않음

	if (IsDead()) 
		return false; // 이미 죽어있음
	
	if (IsDeleteMe()) 
		return false; // 삭제 예정
	
	if (GetActionState() != AS_NORMAL) 
		return false; // 유효하지 않은 액션상태

	if (IsBeginUnableAction()) 
		return false; // 이동 불가능한 상태

	//GFieldNPCSession* pSession = 
	//	GetModuleAI()->GetJobMgr()->GetSession()->GetFieldSession();
	//if (pSession)
	//{
	//	if (!pSession->IsThinkable())
	//		return false; // NPC 세션으로 인해 불가능한 상태
	//}

	return true;
}

void GEntityNPC::UpdateAI( float fDelta )
{
	if (!IsThinkable()) 
		return;

	// AI 작동가능하면 업데이트	
	m_pModuleAI->Update(fDelta);
}

void GEntityNPC::UpdateMovable( float fDelta )
{
	if (!IsThinkable()) 
		return;
	
	// AI 작동가능하면 업데이트	
	m_pModuleMovable->Update(fDelta);
}

void GEntityNPC::Roam( float fRadius, float fDurationTime/*=0.0f*/ )
{
	m_pJobRunner->Roam(m_pModuleAI, fRadius, fDurationTime);
}

void GEntityNPC::Patrol( MWLua::table tableMarkers, int nPatrolType )
{
	MARKER_LIST MarkerList;
	tableMarkers.iteration();
	int nMarkerID;
	while (tableMarkers.next<int>(&nMarkerID))  
	{ 
		MarkerList.push_back(nMarkerID);
	} 

	m_pJobRunner->Patrol(m_pModuleAI, MarkerList, nPatrolType);
}

void GEntityNPC::PatrolInstant( MWLua::table tableMarkers, int nPatrolType )
{
	vector<MARKER_INFO> MarkerList;
	tableMarkers.iteration();
	vec3 vPoint;
	while (tableMarkers.next<vec3>(&vPoint))  
	{ 
		MarkerList.push_back(MARKER_INFO(vPoint));
	} 

	m_pJobRunner->Patrol(m_pModuleAI, MarkerList, nPatrolType);
}

void GEntityNPC::ClearJob()
{
	m_pJobRunner->ClearJob(m_pModuleAI);
}

void GEntityNPC::Say(const wchar_t* szText, float fMaintainTime)
{
	m_pJobRunner->Speak(m_pModuleAI, L"say", szText, fMaintainTime);
}

void GEntityNPC::Ballon(const wchar_t* szText, float fMaintainTime)
{
	m_pJobRunner->Speak(m_pModuleAI, L"balloon", szText, fMaintainTime);
}

void GEntityNPC::Shout(const wchar_t* szText, float fMaintainTime)
{
	m_pJobRunner->Speak(m_pModuleAI, L"shout", szText, fMaintainTime);
}

void GEntityNPC::Narration( const wchar_t* szText)
{
	m_pJobRunner->Speak(m_pModuleAI, L"narration", szText, 0.0f);	// 나레이션은 항상 대기하지 않음
}

void GEntityNPC::SayAndNarration(const wchar_t* szText, float fMaintainTime)
{
	m_pJobRunner->Speak(m_pModuleAI, L"say_and_narration", szText, fMaintainTime);
}

void GEntityNPC::NarrationNow(const wchar_t* szText)
{	
	doNarration(szText);
}

void GEntityNPC::SayAndNarrationNow(const wchar_t* szText)
{
	doSayAndNarration(szText, 0.0f);
}

void GEntityNPC::ScriptSelf(const wchar_t* szText)
{
	if (!szText)	return;
	m_pJobRunner->RunScript(m_pModuleAI, MUID::Invalid(), L"", szText);
}

void GEntityNPC::Wait(float fSecond)
{
	m_pJobRunner->Idle(m_pModuleAI, fSecond);
}

GEntityActor* GEntityNPC::Aggro(const wchar_t* pszType, float fDuration)
{
	USES_CONVERSION_EX;

	wstring strType = pszType;

	HATE_TARGET_SWAP_EVENT nEventType = HATESWAP_NONE;
	if		(strType == NPC_XML_ATTR_AGGRO_FLIP)		nEventType = HATESWAP_FLIP;
	else if (strType == NPC_XML_ATTR_AGGRO_RANDOM)		nEventType = HATESWAP_RANDOM;
	else if (strType == NPC_XML_ATTR_AGGRO_FAR)			nEventType = HATESWAP_FAR;
	else if (strType == NPC_XML_ATTR_AGGRO_NEAR)		nEventType = HATESWAP_NEAR;
	else if (strType == NPC_XML_ATTR_AGGRO_SHORT)		nEventType = HATESWAP_SHORT;
	else	
	{
		dlog("루아함수 'aggro'에 잘못된 타입이 입력되었습니다. (type:%s)\n", pszType ? MLocale::ConvUTF16ToAnsi(pszType).c_str() : "null");
		return NULL;
	}

	GetHateTable().ApplyEvent(nEventType, fDuration);

	return GetTargetActor();
}

int GEntityNPC::GetCurrentTalent() const
{
	GModuleCombat* pmCombat = GetModuleCombat();
	VALID_RET(pmCombat,0);
	return pmCombat->GetUsingTalentID();
}

void GEntityNPC::BreakPartJob(uint8 nPartID, MUID uidOpponent)
{
	m_pJobRunner->BreakPart(m_pModuleAI, nPartID, uidOpponent);
}

void GEntityNPC::MoveToPos(GEntityActor* pTargetActor, float fDistance, float fDurationTime)
{
	VALID(GetField());
	VALID(pTargetActor);

	m_pJobRunner->MoveToEntity(GetModuleAI(), pTargetActor, fDistance, false, fDurationTime);
}

void GEntityNPC::Summon(int nNPCID, vec3 vStartPos)
{
	m_pJobRunner->Summon(GetModuleAI(), nNPCID, vStartPos);
}

int GEntityNPC::GetSightRange(int nEnemyLevel)
{
	if (!GetNPCInfo())	return -1;

	int nLF = GCalculator::CalcLevelFactor(GetLevel(), nEnemyLevel);

	return GetNPCInfo()->nSightRange + ((5-nLF)*30);
}

int GEntityNPC::GetMaxSightRange()
{
	if (!GetNPCInfo())	return -1;

	int nLF = 0;

	return GetNPCInfo()->nSightRange + ((5-nLF)*30);
}


float GEntityNPC::GetStandupTime()
{
	if (!m_pNPCInfo)
		return -1.0f;

	return m_pNPCInfo->fStandupTime;
}

bool GEntityNPC::HasJob()
{
	GJobMgr* pJobMgr = m_pModuleAI->GetJobMgr();
	return !pJobMgr->IsEmpty();
}

bool GEntityNPC::HasMissionJob()
{
	return !GetMissionJobs().empty();
}

void GEntityNPC::ReserveUserdata( int nNPCID, int nSize )
{
	if (nSize <= 0 || nSize > MAX_USER_DATA) return;
	m_mapUserDataSize.insert(map<int,int>::value_type(nNPCID, nSize));
}

size_t GEntityNPC::GetUserdataSize( int nNPCID )
{
	map<int,int>::iterator it = m_mapUserDataSize.find(nNPCID);
	if (it == m_mapUserDataSize.end())
		return 0;

	return it->second;
}

int GEntityNPC::GetUserdata( int index )
{
	index -= 1;	// 스크립트의 index는 1부터 시작
	if (index < 0 || index >= (int)m_vecUserData.size())
	{
		dlog("NPC:GetUserdata(): index가 사용가능 범위를 초과하였습니다. (NPCID: %d, 유효범위: %d to %d)\n", GetID(), 0, (int)m_vecUserData.size());
		return 0;
	}
	return m_vecUserData[index];
}

void GEntityNPC::SetUserdata( int index, int value )
{
	index -= 1;	// 스크립트의 index는 1부터 시작
	if (index < 0 || index >= (int)m_vecUserData.size())
	{
		dlog("NPC:SetUserdata(): index가 사용가능 범위를 초과하였습니다. (NPCID: %d, 유효범위: %d to %d)\n", GetID(), 0, (int)m_vecUserData.size());
		return;
	}
	m_vecUserData[index] = value;

	// 유저데이터 변경 콜백 루아함수 호출
	GetScriptNPC()->OnUserdataChanged(GetDelegator(), index+1, value);
}

void GEntityNPC::Flee(FleeType nFleeType, float fDurationTime)
{
	EVENT_FLEE_INFO param;
	param.nFleeType = nFleeType;
	param.fDurationTime = fDurationTime;
	GMessage msg(GMSG_AI_EVT_FLEE, &param);
	Message(msg);
}

void GEntityNPC::FaceTo(GEntityActor* pTargetActor)
{
	VALID(pTargetActor);
	m_pJobRunner->FaceTo(m_pModuleAI, pTargetActor->GetUID());
}

GGlueActor* GEntityNPC::NewDelegator()
{
	return new GGlueNPC(this);
}

GGlueNPC* GEntityNPC::GetDelegator()
{
	if (!m_pDelegator)
		return NULL;

	return static_cast<GGlueNPC*>(m_pDelegator);
}

void GEntityNPC::OnBeginUnableAction()
{
	__super::OnBeginUnableAction();

	GJobMgr* pJobMgr = m_pModuleAI->GetJobMgr();
	pJobMgr->Suspend();
}

void GEntityNPC::OnEndUnableAction()
{
	__super::OnEndUnableAction();

	GJobMgr* pJobMgr = m_pModuleAI->GetJobMgr();
	pJobMgr->Resume();
}

void GEntityNPC::UpdateScriptTimer( float fDelta )
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(4008);

	if (!m_pScriptTimer) return;

	if (m_pScriptTimer->Update(fDelta))
	{
		for each (int nTimerID in m_pScriptTimer->GetUpdatedTimers())
			GetScriptNPC()->OnTimer(GetDelegator(), nTimerID);
	}
}

GBehaviorStateMachine* GEntityNPC::GetBehaviorFSM(void)
{
	return m_pModuleAI->GetBehaviorFSM();
}

GJobMgr* GEntityNPC::GetJobMgr(void)
{
	return m_pModuleAI->GetJobMgr();
}

void GEntityNPC::OnFinishTalent( GTalentInfo* pTalentInfo )
{
	if (NULL == pTalentInfo) return;

	__super::OnFinishTalent(pTalentInfo);

	GetScriptNPC()->OnTalentEnd(GetDelegator(), pTalentInfo->m_nID);
}

void GEntityNPC::SetTauntable( bool bEnable )
{
	GetHateTable().SetTauntable(bEnable);
}

const wchar_t* GEntityNPC::GetName() const
{
	if (!m_pNPCInfo)
		return L"unknown_name";

	return GetNPCInfo()->strName.c_str();
}

void GEntityNPC::OnDie()
{
	__super::OnDie();
	
	GetNPCMinion().OnDead();
	m_pQPNPCPosSender->OnDead();	
}

void GEntityNPC::OnEndCombat()
{
	GetNPCMinion().OnEndCombat();
}

void GEntityNPC::OnVictory()
{
	GetScriptNPC()->OnVictory(GetDelegator());
}

int8 GEntityNPC::GetCapsuleGroupIndex()
{
	return GetNPCHitcapsuleGroup().GetCapsuleGroupIndex();
}

float GEntityNPC::GetValidInteractionDistance()
{
	return CSCharHelper::CalcValidInteractionDistance(GetColRadius(), m_pNPCInfo->nInteractionDistance);
}

void GEntityNPC::SetHomePoint( const vec3& vPoint, const vec3& vDir )
{
	m_HomePoint.vPoint = vPoint;
	m_HomePoint.vDir = vDir;
}

const SimpleMarker& GEntityNPC::GetHomePoint() const
{
	return m_HomePoint;
}

bool GEntityNPC::IsNowUsingTalent() const
{
	return GetModuleCombat()->IsUsingTalent();
}

bool GEntityNPC::IsNowUsingRotateTalent() const
{
	int nUsingTalentID = GetModuleCombat()->GetUsingTalentID();
	if (nUsingTalentID == GetNPCInfo()->m_nLRotationID)
		return true;

	if (nUsingTalentID == GetNPCInfo()->m_nRRotationID)
		return true;

	return false;
}

bool GEntityNPC::IsMovable()
{
	if (!__super::IsMovable())
		return false;

	if (GetNPCStatus().bRooted)
		return false;

	return true;
}

bool GEntityNPC::IsHuge() const
{
	GNPCInfo* pNPCInfo = GetNPCInfo();
	if (!pNPCInfo)
		return false;

	return pNPCInfo->bHuge;
}

void GEntityNPC::Reset()
{
	// 모든 버프 잃음
	GetModuleBuff()->LazyLostAll();
	GetModuleBuff()->Update(0.0f);

	// 기본 스탯 회복
	SetHP(GetMaxHP());
	SetEN(GetMaxEN());
	SetSTA(GetMaxSTA());

	// 브레이커블파츠 복구
	if (m_pNPCBParts) m_pNPCBParts->Recovery();

	// 전투 종료
//	doMakePeace();
}

NPC_MODE GEntityNPC::GetMode() const
{
	return m_pNPCMode->GetMode();
}

void GEntityNPC::ChangeMode( NPC_MODE nNexMode )
{
	return m_pNPCMode->ChangeMode(nNexMode);
}

int GEntityNPC::GetGroupID()
{
	return m_nGroupID;
}

void GEntityNPC::SetGroupID(int nGroupID)
{	
	if (0 <= nGroupID)
	{
		m_nGroupID = nGroupID;
	}
}

void GEntityNPC::OnUseTalentFailed( int nTalentID, CCommandResultTable nFailCause )
{
	if (nFailCause == CR_FAIL_SYSTEM_INVALID_TALENT_ID)
	{
		dlog("%s - invalid talent (npcid: %d, talentid: %d)\n", __FUNCTION__, GetID(), nTalentID);
	}

	__super::OnUseTalentFailed(nTalentID, nFailCause);
}

float GEntityNPC::GetSpellPower() const 
{
	VALID_RET(GetNPCInfo(), 1.0f);
	
	return GetNPCInfo()->m_fSpellPower;
}

NPC_AA_TYPE GEntityNPC::GetAAType() const
{
	return m_NPCStatus.nAA;
}

SPAWN_ID GEntityNPC::GetSpawnID() const
{
	return m_nSpawnID;
}

void GEntityNPC::SetSpawnID( SPAWN_ID nID )
{
	m_nSpawnID = nID;
}

bool GEntityNPC::IsNowInvincibility()
{
	if (m_pNPCCombatAnticheat->IsCheating())
		return true; // 치팅중

	if (m_pModuleAI->IsNowInvincibility())		
		return true;

	return __super::IsNowInvincibility();
}

void GEntityNPC::EnableQPMarkPos()
{
	m_pQPNPCPosSender->Enable();
}

void GEntityNPC::DisableQPMarkPos()
{
	m_pQPNPCPosSender->Disable();
}

void GEntityNPC::doDieForced()
{
	if (GetField())
	{
		GetField()->GetSpawn()->OnNPCDie(GetSpawnID());
	}		


	GEntityActor::doDie();	

	GJobMgr* pJobMgr = GetModuleAI()->GetJobMgr();
	pJobMgr->Clear();

	GetDelegator()->RunDieScript(GetSpawnInfo());
	GetNPCDecay().Decay();	
	RouteDie();

	SendNPCIcon();
}

bool GEntityNPC::IsNowPuppet()
{
	if (GetModuleAI()->GetBehaviorFSM()->GetCurrentCombatStateID() == AI_COMBAT_PROLOGUE)
		return true; // 프롤로그 중

	if (GetModuleAI()->GetBehaviorFSM()->GetCurrentStateID() == AI_BEHAVIOR_DEAD)
		return true; // 죽은 상태	

	return false;
}

void GEntityNPC::Destroy()
{
	m_pQPNPCPosSender->OnDestroy();

	__super::Destroy();	
}

void GEntityNPC::CollectEssentialReceiver( vector<MUID>& vecReceiverUID )
{
	GetHateTable().CollectTarget(vecReceiverUID);
}

void GEntityNPC::doAttackLazy( MUID uidTarget, int nDamage/*=0*/ )
{
	GetTickVars().LazyAttack.uidTarget = uidTarget;
	GetTickVars().LazyAttack.nDamage = nDamage;
}

deque<GJob*> GEntityNPC::GetMissionJobs()
{
	return GetModuleAI()->GetMissionJobs();
}

bool GEntityNPC::HasSession()
{
	if (GetSession() == NULL)
		return false;

	return true;
	//return GetModuleAI()->HasNPCSession();
}

GFieldNPCSession* GEntityNPC::GetSession()
{
	return GetModuleAI()->GetBehaviorFSM()->GetNPCSession();
}

void GEntityNPC::SendNPCIcon()
{
	if (!m_pNPCInfo->bInteract) return;

	GVectorMUID vecPlayerUID;
	GetNeighborSectorPlayer(vecPlayerUID.Vector());
	for each (const MUID& nPlayerUID in vecPlayerUID.Vector())
	{
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nPlayerUID);
		if (NULL == pPlayer) continue;

		pPlayer->GetNPCIconSender().SendByNPC(this);
	}
}

void GEntityNPC::SendNPCIconBySectorChange( const vector<GFieldGrid::Sector*>& vecNewSectors )
{
	if (!m_pNPCInfo->bInteract) return;

	GFieldGrid::EntitySelector* entity_selector = m_pField->GetGrid().GetEntitySelector();
	
	class NPCIconSender : public GFieldGrid::Sector::IVisitor
	{
	public:
		NPCIconSender(GEntityNPC& npc) : m_npc(npc) {}

	private:
		void OnVisit(const MUID& uid)
		{
			GEntityPlayer* const player = gmgr.pPlayerObjectManager->GetEntity(uid);
			if (NULL == player) return;

			player->GetNPCIconSender().SendByNPC(&m_npc);
		}
	private:
		GEntityNPC& m_npc;
	};

	for each (GFieldGrid::Sector* const sector in vecNewSectors)
	{
		entity_selector->Visit(NPCIconSender(*this), ETID_PLAYER, sector->Position(), 0);
	}
}

bool GEntityNPC::ExpRewardable() const
{
	if (!GetNPCInfo())
		return false;

	if (!m_EXPRewardable)
		return false;

	return GetNPCInfo()->bGainEXP;
}

void GEntityNPC::DontRewardExp()
{
	m_EXPRewardable = false;
}

bool GEntityNPC::QuestRewardable() const
{
	return m_QuestRewardable;
}

void GEntityNPC::DontRewardQuest()
{
	m_QuestRewardable = false;
}

GLootInfo* GEntityNPC::GetLootInfo()
{
	int nBPartLootID = GetNPCLoot().GetBPartLootID();

	if (INVALID_ID != nBPartLootID)
	{
		GLootInfo* pBPartLootInfo = gmgr.pLootInfoMgr->Get(nBPartLootID);
		if (NULL != pBPartLootInfo)
			return pBPartLootInfo;
	}

	return GetNPCInfo()->m_pLootInfo;
}

void GEntityNPC::OnSectorChanged( const vector<GFieldGrid::Sector*>& vecNewSectors, const vector<GFieldGrid::Sector*>& vecOldSectors )
{
	__super::OnSectorChanged(vecNewSectors, vecOldSectors);

	SendNPCIconBySectorChange(vecNewSectors);
}