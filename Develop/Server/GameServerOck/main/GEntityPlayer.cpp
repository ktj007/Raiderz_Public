#include "stdafx.h"
#include "GEntityPlayer.h"
#include "GField.h"
#include "GCommandTable.h"
#include "CTransData.h"
#include "GCalculator.h"
#include "GExpCalculator.h"
#include "GField.h"
#include "CCommandTable.h"
#include "GModuleCombat.h"
#include "GNPCInfo.h"
#include "GTalentInfo.h"
#include "GTalent.h"
#include "GCalculator.h"
#include "GItemSystem.h"
#include "GFieldSystem.h"
#include "GPartySystem.h"
#include "GTradeSystem.h"
#include "GNPCShopSystem.h"
#include "GQuestSystem.h"
#include "GLootSystem.h"
#include "GInteractionSystem.h"
#include "GBuffInfo.h"
#include "GModuleBuff.h"
#include "GCombatCalculator.h"
#include "GConditionsInfoMgr.h"
#include "GConditionsSystem.h"
#include "GNPCShopInfo.h"
#include "GConst.h"
#include "GDynamicFieldMaster.h"
#include "GSoulBindingInfoMgr.h"
#include "GSoulBindingInfo.h"
#include "GCommandCenter.h"
#include "GPlayerInteraction.h"
#include "GFatigueSystem.h"
#include "GInnRoomField.h"
#include "GGuildSystem.h"
#include "GDuelMgr.h"
#include "GDuel.h"
#include "GGuildMgr.h"
#include "GPalette.h"
#include "GPlayerTalent.h"
#include "GTalentInfoMgr.h"
#include "GNPCMgr.h"
#include "GModuleAI.h"
#include "GCommand.h"
#include "GSystem.h"
#include "GDBManager.h"
#include "GFieldMgr.h"
#include "GEnvManager.h"
#include "GTalentHelper.h"
#include "GScriptGroup_Quest.h"
#include "GScriptGroup_Field.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "GTrainingSystem.h"
#include "GGluePlayer.h"
#include "GSharedField.h"
#include "GItemExpirer.h"
#include "GFieldInfoMgr.h"
#include "GItemRemover.h"
#include "GPlayerQuests.h"
#include "GPlayerQuest.h"
#include "GPlayerFalling.h"
#include "GDynamicFieldPolicy_TrialField.h"
#include "GPlayerField.h"
#include "GQuestInfoMgr.h"
#include "GPlayerBattleArena.h"
#include "GMoneySystem.h"
#include "GStrings.h"
#include "GBattleArenaMgr.h"
#include "GPlayerEnemyInfoSender.h"
#include "GHateTable.h"
#include "GMasterServerFacade.h"
#include "GPlayerLoot.h"
#include "GPlayerFactions.h"
#include "GEntityPlayerDBAsyncMonitor.h"
#include "GPlayerPVPArea.h"
#include "GServer.h"
#include "GPlayerLevelUpdater.h"
#include "GTDMaker.h"
#include "GItemUser.h"
#include "GPlayerFieldDynamic.h"
#include "GServerPlayerRouter.h"
#include "GPlayerPVPChecker.h"
#include "GNPCInteractor.h"
#include "GConditionInfo.h"
#include "GPlayerObjectManager.h"
#include "GPlayerTDCacheInfoBuilder.h"
#include "GWarpInfoCollector.h"
#include "GActorHateTracker.h"
#include "GPlayerCombatChecker.h"
#include "GPlayerSensorCacheUpdater.h"
#include "GItemAdder.h"
#include "GItemDurabilityUpdater.h"
#include "GPlayerDoing.h"
#include "GPlayerCutscene.h"
#include "GPlayerRecipe.h"
#include "GPlayerTargetInfoSender.h"
#include "GMsgCommandFacade.h"
#include "GItemHolder.h"
#include "GEquipmentSlot.h"
#include "GInventory.h"
#include "GPlayerEmblem.h"
#include "CSBasicAttributeCalculator.h"
#include "CSMotionFactorHelper.h"
#include "GGuardCalculator.h"
#include "GPlayerSetterForTest.h"
#include "GPlayerSystem.h"
#include "GTradeCanceler.h"
#include "GPlayerTrade.h"
#include "CCommandResultTable.h"
#include "GCheatChecker.h"
#include "CSCharHelper.h"
#include "GQuestInfo.h"
#include "GChallengerQuestMgr.h"
#include "GDBCacheData.h"
#include "GAsyncTaskFactory.h"
#include "GClientFieldRouter.h"
#include "GActorCooltimeChecker.h"
#include "GActorInvisibility.h"
#include "GPlayerNPCIconSender.h"
#include "GDBTaskCharData.h"
#include "GQuestGiver.h"
#include "GQuestFailer.h"
#include "GQObjUpdater.h"
#include "GQuestVarUpdater.h"
#include "GPlayerObject.h"
#include "GPlayerMailBox.h"
#include "GPlayerEnchant.h"
#include "GPlayerMailReserver.h"
#include "GTestSystem.h"
#include "GPlayerDye.h"
#include "GConfig.h"
#include "GQPKillRewardOwner.h"
#include "GQPKillRewarder.h"
#include "GPlayerRole.h"
#include "GPlayerAFK.h"
#include "GPlayerSpawnedNPC.h"
#include "GEntityNPC.h"
#include "GDB_CODE.h"
#include "CSDef_QuestPVP.h"
#include "GStorage.h"
#include "GPlayerSit.h"
#include "CSDef_Fatigue.h"
#include "CSItemHelper.h"
#include "GPlayerStance.h"


GPlayerCRTLogger GEntityPlayer::m_PlayerCRTLogger;


GEntityPlayer* ToEntityPlayer( GEntity* pEntity )
{
	VALID_RET(pEntity, NULL);
	if (!pEntity->IsPlayer())			return NULL;
	return static_cast<GEntityPlayer*>(pEntity);
}

////////////////////////////////////////////////////////////////

GTimeSynchor::GTimeSynchor()
{
	m_rgrInvterval.SetTickTime((float)GConst::TIME_SYNCH_INTERVAL);
	m_rgrInvterval.Start();
}

void GTimeSynchor::RouteTimeSynch( GEntityPlayer* pPlayer )
{
	MCommand* pNewCmd = MakeNewCommand(MC_TIME_SYNCH, 1, NEW_UINT(gsys.pSystem->GetNowTime()));
	pPlayer->RouteToMe(pNewCmd);
}

void GTimeSynchor::Update( float fDelta, GEntityPlayer* pPlayer )
{
	PFC_THISFUNC;
	if (m_rgrInvterval.IsReady(fDelta))
	{
		RouteTimeSynch(pPlayer);
	}
}

void GTimeSynchor::SetActive( bool bActive )
{
	if (bActive)
	{
		m_rgrInvterval.Start();
	}
	else 
	{
		m_rgrInvterval.Stop();
	}
}

////////////////////////////////////////////////////////////////

GPlayTimeUpdater::GPlayTimeUpdater() : m_fPlayTimeDelta(0.0f)
{

}

void GPlayTimeUpdater::Update( const float fDelta, PLAYER_INFO* pPlayerInfo )
{
	PFC_THISFUNC;
	static const float fUpdatePlaytTimeSec = 1.0f;

	m_fPlayTimeDelta += fDelta;

	if (fUpdatePlaytTimeSec > m_fPlayTimeDelta)
		return;

	++pPlayerInfo->nPlayTimeSec;
	m_fPlayTimeDelta -= fUpdatePlaytTimeSec;
}

////////////////////////////////////////////////////////////////

GEntityPlayer::GEntityPlayer()
: GEntityActor()
, m_pPlayerInfo(NULL)
, m_nLastPrepareAttackTime(0)
, m_nCastingTalentID(-1)
, m_bPartyLeader(false)
, m_nTriggedSensorID(INVALID_ID)
, m_pQuests(NULL)
, m_pDuel(NULL)
, m_pPlayerField(NULL)
, m_pPlayerBattleArena(NULL)
, m_pKillTalentInfo(NULL)
, m_pPvPArea(NULL)
, m_isMoveServer(false)
, m_pWarpInfoCollector(NULL)
, m_nFieldGroupID(INVALID_ID)
, m_pSensorCacheUpdater(new GPlayerSensorCacheUpdater())
, m_pItemHolder(NULL)
, m_isMovedFromOtherGameServer(false)
, m_bInteractionCheck(true)
, m_nLastAuctionMsgTime(0)
{
	m_nType |= ET_PC;
	m_nTypeID = ETID_PLAYER;

	m_pPlayerInfo = new PLAYER_INFO();
	m_pChrInfo = m_pPlayerInfo;

	m_MeshInfo.SetForPlayer();

	m_pQuests = new GPlayerQuests(this);
	m_pTalent = new GPlayerTalent(this);
	m_pInteraction = new GPlayerInteraction(this);
	m_pPalette = new GPalette(this);
	m_pItemExpirer = new GItemExpirer(this);
	m_pPlayerFalling = new GPlayerFalling(this);
	m_pPlayerField = new GPlayerField(this);
	m_pPlayerField->SetLisener(this);
	m_pPlayerBattleArena = new GPlayerBattleArena(this);
	m_pEnemyInfoSender = new GPlayerEnemyInfoSender(this);	
	m_pPlayerLoot = new GPlayerLoot(this);
	m_pPlayerFactions = new GPlayerFactions();
	m_pDBAsyncMonitor = new GEntityPlayerDBAsyncMonitor();
	m_pPvPArea = new GPlayerPVPArea(this);
	m_pPvPChecker = new GPlayerPVPChecker(this);
	m_pCombatChecker = new GPlayerCombatChecker(this);
	m_pWarpInfoCollector = new GWarpInfoCollector();
	m_pCutscene = new GPlayerCutscene(this);
	m_pDoing = new GPlayerDoing(this);
	m_pStance = new GPlayerStance(this);
	m_pPlayerRecipe = new GPlayerRecipe(this);
	m_pTargetInfoSender = new GPlayerTargetInfoSender(this);
	m_pPlayerEmblem = new GPlayerEmblem(this);
	m_pPlayerSetterForTest = new GPlayerSetterForTest(this);
	m_pItemHolder = new GItemHolder(this, new GInventory(this), new GEquipmentSlot(this), new GStorage(this));
	m_pPlayerTrade = new GPlayerTrade(this);
	m_pCheatCheck = new GCheatChecker();
	m_pNPCIconSender = new GPlayerNPCIconSender(this);
	m_pMailbox = new GPlayerMailBox(this);
	m_pMailReserver = new GPlayerMailReserver(this);
	m_pPlayerEnchant = new GPlayerEnchant(this);
	m_pDye = new GPlayerDye();
	m_pQPKillRewardOwner = new GQPKillRewardOwner(this);
	m_pQPKillRewarder = new GQPKillRewarder(this);
	m_pRole = new GPlayerRole(this);
	m_pAFK = new GPlayerAFK(this);
	m_pSit = new GPlayerSit();
	m_pSpawnedNPC = new GPlayerSpawnedNPC(this);

	_FillRegenVar();
}


GEntityPlayer::~GEntityPlayer()
{
	GField* pField = GetField();
	if (NULL != pField)
	{
		pField->RemovePlayerCID(GetCID());
	}


	m_pChrInfo = NULL;

	SAFE_DELETE(m_pItemHolder);
	SAFE_DELETE(m_pSensorCacheUpdater);
	SAFE_DELETE(m_pPlayerFalling);
	SAFE_DELETE(m_pItemExpirer);
	SAFE_DELETE(m_pPlayerInfo);
	SAFE_DELETE(m_pQuests);
	SAFE_DELETE(m_pTalent);
	SAFE_DELETE(m_pInteraction);
	SAFE_DELETE(m_pPalette);
	SAFE_DELETE(m_pPlayerField);
	SAFE_DELETE(m_pPlayerBattleArena);
	SAFE_DELETE(m_pEnemyInfoSender);	
	SAFE_DELETE(m_pPlayerLoot);
	SAFE_DELETE(m_pPlayerFactions);
	SAFE_DELETE(m_pDBAsyncMonitor);
	SAFE_DELETE(m_pPvPArea);
	SAFE_DELETE(m_pPvPChecker);
	SAFE_DELETE(m_pCombatChecker);
	SAFE_DELETE(m_pWarpInfoCollector);
	SAFE_DELETE(m_pCutscene);
	SAFE_DELETE(m_pDoing);
	SAFE_DELETE(m_pStance);
	SAFE_DELETE(m_pPlayerRecipe);
	SAFE_DELETE(m_pTargetInfoSender);
	SAFE_DELETE(m_pPlayerEmblem);
	SAFE_DELETE(m_pPlayerSetterForTest);
	SAFE_DELETE(m_pPlayerTrade);
	SAFE_DELETE(m_pCheatCheck);
	SAFE_DELETE(m_pNPCIconSender);
	SAFE_DELETE(m_pMailbox);
	SAFE_DELETE(m_pPlayerEnchant);
	SAFE_DELETE(m_pMailReserver);
	SAFE_DELETE(m_pDye);
	SAFE_DELETE(m_pQPKillRewardOwner);
	SAFE_DELETE(m_pQPKillRewarder);
	SAFE_DELETE(m_pRole);
	SAFE_DELETE(m_pAFK);
	SAFE_DELETE(m_pSit);
	SAFE_DELETE(m_pSpawnedNPC);
}

bool GEntityPlayer::Create(MUID& uid)
{
	PFI_BLOCK_THISFUNC(105);

	GEntityActor::Create(uid);	
	m_pItemExpirer->Init();		
	
	m_DecFatigueRegulator.SetTickTime(DEC_FATIGUE_PERIOD);
	m_DecFatigueRegulator.Start();
	
	return true;
}

void GEntityPlayer::Destroy()
{
	PFI_BLOCK_THISFUNC(106);

	PFI_B(5751, "GEntityPlayer::Destroy() - 1");

	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(GetPlayerField().GetDynamicFieldGroupUID());
	if (pFieldGroup)
	{
		pFieldGroup->OnLeavePlayer(this);
	}

	GField* pField = gmgr.pFieldMgr->GetField(GetPlayerField().GetFieldEntry().GetReservedWarpField());
	if (pField)
	{
		pField->CancelPreservedEnter(this);
	}

	if (!IsMoveServer())
	{
		gsys.pGuildSystem->OffLine(this);
	}

	GetDoing().EndDoing();

	if (GetPlayerBattleArena().IsEntered())
	{
		GetPlayerBattleArena().OnDisconnect();
	}

	if (HasParty())
	{
		gsys.pPartySystem->DoOffline(this);
	}

	if (HasDuel() &&
		GetField())
	{
		GetField()->GetDuelMgr()->Disconnect(this); // 강제 종료
	}


	PFI_E(5751);

	PFI_B(5752, "GEntityPlayer::Destroy() - 2");
	__super::Destroy();
	PFI_E(5752);
	
}

void GEntityPlayer::OnEnter()
{
	GEntityActor::OnEnter();

	GetPlayerField().GetFieldEntry().ClearReserveWarp();

	SetDuel(NULL);
	GetDoing().ChangeDoing(CD_NORMAL);
	m_vReservedPosSync = vec3::ZERO;	

	GServerPlayerRouter* pServerRouter = gsys.pMasterServerFacade->GetPlayerRouter();
	pServerRouter->RoutePlayerInFieldNotify(this, m_pField->GetUID(), m_pField->GetID());

	m_pCheatCheck->Init(this);	// 필드에 진입한 경우에는 체크를 초기화한다.
	// 필드 진입에 치팅할만한 문제는 필드를 떠날때 확인한다.
	OnSynchQuest();

	if (m_pField->IsSharedField())
	{
		GDBT_CHAR_ENTER_FILED data(GetAID()
			, GetCID()
			, m_pPlayerInfo->nXP
			, m_pPlayerInfo->nLevel
			, m_pPlayerInfo->GetMoney()
			, m_pPlayerInfo->nPlayTimeSec
			, m_pField->GetID()
			, m_vPos.x
			, m_vPos.y
			, m_vPos.z);
		gsys.pDBManager->FieldEnter(data);
	}		
}

void GEntityPlayer::OnLeave()
{
	GetDoing().EndDoing();
	m_pTargetInfoSender->ClearSendTime();
	m_pDye->Clear();

	ClearEnteredSensorID();
	m_pSensorCacheUpdater->Invalidate();

	GetNPCIconSender().ClearNPCIconData();
	GetSpawnedNPC().DespawnAll();

	GEntityActor::OnLeave();
}

MCommand* GEntityPlayer::CreateFieldInCmd()
{
	TD_UPDATE_CACHE_PLAYER td_player_info;
	MakeTDCacheInfo(td_player_info);

	return MakeNewCommand(
		MC_FIELD_IN_PLAYER,
		1,
		NEW_SINGLE_BLOB(&td_player_info, sizeof(TD_UPDATE_CACHE_PLAYER))
	);
}

MCommand* GEntityPlayer::CreateFieldOutCmd()
{
	return MakeNewCommand(MC_FIELD_OUT_PLAYER, 1, NEW_USHORT(m_nUIID));
}

void GEntityPlayer::OnVisitedNewSectorPC(const MUID& newSightedPC,MCommand* const fieldInCmd)
{
	__super::OnVisitedNewSectorPC(newSightedPC, fieldInCmd);
// 	GFieldGridRouter sector_router;
// 	sector_router.SendToMe(this, vecNewSectors);
}

void GEntityPlayer::OnVisitedNewSectorNPC(const MUID& newSightedNPC,MCommand* const fieldInCmd)
{
	__super::OnVisitedNewSectorNPC(newSightedNPC, fieldInCmd);
// 	
// 	GetNPCIconSender().RemoveNPCIconData(vecOldNPC.Vector());
}

void GEntityPlayer::OnVisitedOldSectorNPC(const MUID& oldSIghtedNPC,MCommand* const fieldOutCmd)
{
	__super::OnVisitedOldSectorNPC(oldSIghtedNPC, fieldOutCmd);
//	GetNPCIconSender().SendByPlayerIn(vecNewNPC.Vector(), vecOldSectors.empty());
}

void GEntityPlayer::OnSectorChanged(const MPoint& oldSectorPt)
{
	__super::OnSectorChanged(oldSectorPt);

	VALID(NULL!=m_pField);
	GFieldGrid::DeltaSelector* const delta_selector = m_pField->GetGrid().GetDeltaSelector();
	VALID(NULL!=delta_selector);

	vector<GFieldGrid::Sector*> newSectors;
	delta_selector->GetDeltaNewSectors(newSectors, oldSectorPt, GetSectorPoint());
	vector<GFieldGrid::Sector*> newHugeSectors;
	delta_selector->GetDeltaNewHugeSectors(newHugeSectors, oldSectorPt, GetSectorPoint());
	
	newSectors.insert(newSectors.end(), newHugeSectors.begin(), newHugeSectors.end());

	GFieldGridRouter sector_router;
	sector_router.SendToMe(this, newSectors);

	UpdateUsableSensor(true);

	vector<MUID> npcs;
	class CollectNPCs : public GFieldGrid::Sector::IVisitor
	{
	public:
		CollectNPCs(vector<MUID>& npcs) : m_npcs(npcs) {
			m_npcs.clear();
		}

	private:
		void OnVisit(const MUID& npc)
		{
			m_npcs.push_back(npc);
		}

	private:
		vector<MUID>& m_npcs;
	};
	
	delta_selector->VisitOldSectors(CollectNPCs(npcs), ETID_NPC, oldSectorPt, GetSectorPoint(), 1);
	GetNPCIconSender().RemoveNPCIconData(npcs);

	delta_selector->VisitNewSectors(CollectNPCs(npcs), ETID_NPC, oldSectorPt, GetSectorPoint(), 1);
	GetNPCIconSender().SendByPlayerIn(npcs, npcs.empty());

	delta_selector->VisitNewHugeSectors(CollectNPCs(npcs), ETID_NPC, oldSectorPt, GetSectorPoint());
	GetNPCIconSender().SendByPlayerIn(npcs, npcs.empty());

// 	delta_selector->VisitNewHugeSectors(
// 		ProcessChangedSectorEvent(*this, &GEntitySync::OnVisitedNewSectorNPC, NULL), ETID_NPC, oldSectorPt, m_sectorPt
// 		);
// 
// 	delta_selector->VisitOldHugeSectors(
// 		ProcessChangedSectorEvent(*this, &GEntitySync::OnVisitedOldSectorNPC, NULL), ETID_NPC, oldSectorPt, m_sectorPt
// 		);
}

void GEntityPlayer::Warp(vec3 vecPos, vec3 vecDir, bool bRouteToMe)
{	
	m_pPlayerField->Warp(vecPos, vecDir, bRouteToMe);
}

void GEntityPlayer::RouteToMe(MCommand* pCommand) const
{
	if (!pCommand)		return;

	pCommand->SetReceiverUID(m_UID);
	gsys.pCommandCenter->PostCommand(pCommand);
}

void GEntityPlayer::MakeTDCacheInfo(TD_UPDATE_CACHE_PLAYER& out)
{
	GPlayerTDCacheInfoBuilder tdCacheInfoBuilder;
	tdCacheInfoBuilder.Build(this, out);
}

void GEntityPlayer::RouteInEntity( const vector<MUID>& vecPlayers )
{
	if (vecPlayers.empty()) return;

	TD_UPDATE_CACHE_PLAYER td_player_info;
	MakeTDCacheInfo(td_player_info);

	MCommand* pNewCmd = MakeNewCommand(MC_FIELD_IN_PLAYER,
		1, 
		NEW_SINGLE_BLOB(&td_player_info, sizeof(TD_UPDATE_CACHE_PLAYER)));
	pNewCmd->AddReceiver(vecPlayers);
	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GEntityPlayer::RouteOutEntity( const vector<MUID>& vecPlayers )
{
	if (vecPlayers.empty()) return;

	MCommand* pNewCmd = MakeNewCommand(MC_FIELD_OUT_PLAYER, 1, NEW_USHORT(m_nUIID));
	pNewCmd->AddReceiver(vecPlayers);
	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GEntityPlayer::OnUpdate(float fDelta)
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(103);

	GEntityActor::OnUpdate(fDelta);

	m_pInvisibility->OnUpdate(fDelta);
	m_pCoolTimeChecker->Update(fDelta);

	if (true == m_DecFatigueRegulator.IsReady(fDelta))
	{
		gsys.pFatigueSystem->DecFatigueByTime(this);
	}

	m_pQuests->Update(fDelta);
	m_TimeSynchor.Update(fDelta, this);
	m_pInteraction->Update(fDelta);
	m_pEnemyInfoSender->Update(fDelta);
	m_pItemExpirer->Run(fDelta);
	m_PlayTimeUpdater.Update(fDelta, m_pPlayerInfo);
	m_pPlayerLoot->Update(fDelta);
	m_pPvPChecker->Update(fDelta);
	m_pAFK->Update(fDelta);
	m_pCheatCheck->Update(fDelta);
	m_pCombatChecker->Update(fDelta);
}

void GEntityPlayer::ApplyAllPassiveTalents()
{
	SET_TALENTID setTalentID = m_pTalent->GetContainer();
	for each(int nTalentID in setTalentID)
	{
		GTalentInfo* pTalentInfo = GTalentHelper::FindTalentInfo(nTalentID, this);
		if (pTalentInfo == NULL) continue;

		if (ST_EXTRA_PASSIVE == pTalentInfo->m_nSkillType || ST_PASSIVE == pTalentInfo->m_nSkillType)
		{
			GainPassiveTalentEffect(pTalentInfo);
		}
	}
}

void GEntityPlayer::RouteToParty(MCommand* pCommand, const MUID& uidExceptMember)
{
	if (!HasParty())	
	{
		SAFE_DELETE(pCommand);
		return;
	}

	GetPartySys()->RouteToAllMembers(GetPartyUID(), pCommand, uidExceptMember);
}

void GEntityPlayer::doMakePeace()
{
	__super::doMakePeace();

	// PC와 전투 해제
	m_pPvPChecker->doMakePeace();
}

bool GEntityPlayer::IsNowCombat() const
{
	if (!m_pHateTracker->IsEmpty())
		return true;

	if (m_pPvPChecker->IsNowPVP())
		return true;

	return false;
}

bool GEntityPlayer::doGuard(int nGuardTalentID)
{
	if (!GEntityActor::doGuard(nGuardTalentID))
	{
		RouteGuardFailed();
		return false;
	}
	
	// 방어 메세지 전달
	RouteGuard();

	return true;
}

bool GEntityPlayer::doGuardReleased()
{
	if (!GEntityActor::doGuardReleased())
		return false;

	// 방어 해제 메세지 전달
	MCommand* pNewCmd = MakeNewCommand(MC_ACTION_GUARD_RELEASED, 1, NEW_USHORT(m_nUIID));
	RouteToThisCellExceptMe(pNewCmd);

	return true;
}

void GEntityPlayer::doTryHit( GEntityActor* pAttacker, GTalentInfo* pTalentInfo, int nAddtiveDamage, uint16 nCheckTime, int nCapsuleGroup, int nCapsuleIndex )
{
	GEntityActor::doTryHit(pAttacker, pTalentInfo, nAddtiveDamage, nCheckTime, nCapsuleGroup, nCapsuleIndex);
}

bool GEntityPlayer::doDamage(MUID uidAttacker, DAMAGE_ATTRIB nDamageType, uint32 nDamage, EFFECT_SOURCE_TYPE nEffectSourceType, int nEffectSourceID)
{
	if (m_pPlayerInfo->IsGod())			return false;
	
	bool bRet = GEntityActor::doDamage(uidAttacker, nDamageType, nDamage, nEffectSourceType, nEffectSourceID);
	
	
	return bRet;
}

void GEntityPlayer::GetKillerIDAndBasicCode(PLAYER_DIE_INFO& dinfo)
{
	GEntityActor* pKiller = FindActor(GetKiller());
	if (pKiller)
	{
		if (pKiller->IsNPC())
		{
			dinfo.nNpcID = ((GEntityNPC*)pKiller)->GetNPCInfo()->nID;
			dinfo.nCode = GDB_CODE::CD_L_DIE_BY_NPC;
		}
		else if (pKiller->IsPlayer())
		{	
			dinfo.uidKiller = pKiller->GetUID();

			if (GetUID() == dinfo.uidKiller)
				dinfo.nCode = GDB_CODE::CD_L_SUICIDE;
			else
				dinfo.nCode = GDB_CODE::CD_L_DIE_BY_1ON1_PVP_PLAYER;

			dinfo.nKillerCode = GDB_CODE::CD_L_KILL_1ON1_PVP_PLAYER;
		}
	}
}

void GEntityPlayer::GetPVPCode(PLAYER_DIE_INFO& dinfo)
{
	GDuel* pDuel = GetDuel();
	if (pDuel)
	{
		if (pDuel->IsSingleDuel())
		{
			dinfo.nCode			= GDB_CODE::CD_L_DIE_BY_1ON1_PVP_PLAYER;
			dinfo.nKillerCode	= GDB_CODE::CD_L_KILL_1ON1_PVP_PLAYER;
		}
		else
		{
			dinfo.nCode			= GDB_CODE::CD_L_DIE_BY_TEAM_PVP_PLAYER;
			dinfo.nKillerCode	= GDB_CODE::CD_L_KILL_TEAM_PVP_PLAYER;
		}
	}
	else if (GetPlayerBattleArena().IsNowPlaying()) // 투기장
	{
		dinfo.nCode			= GDB_CODE::CD_L_DIE_BY_ARENA_PLAYER;
		dinfo.nKillerCode	= GDB_CODE::CD_L_KILL_ARENA_PLAYER;
	}
	else if (m_pPvPArea->IsLocatedPvPArea() && m_pPvPChecker->IsNowPVP() && QT_NONE !=  m_pPvPArea->GetTeam())
	{
		dinfo.nCode = GDB_CODE::CD_L_DIE_BY_QPVP_PLAYER;
		if (GDB_CODE::CD_L_DIE_BY_NPC != dinfo.nCode)
			dinfo.nKillerCode = GDB_CODE::CD_L_KILL_QPVP_PLAYER;
	}
}

void GEntityPlayer::PostDieLog()
{
	PLAYER_DIE_INFO dinfo;

	GetKillerIDAndBasicCode(dinfo);
	GetPVPCode(dinfo);	

	gsys.pPlayerSystem->WriteDieLogToDB(this, dinfo.nCode, dinfo.nNpcID, dinfo.uidKiller, dinfo.nKillerCode);	
}

void GEntityPlayer::doDie()
{
	PostDieLog();				

	// 결투 처리
	if (HasDuel() &&
		GetField() &&
		GetField()->GetDuelMgr()->DuelKilled(this, m_uidKiller))
	{
		return;		// 결투상대에게 죽은 경우는 죽이지 않는다.
	}

	GEntityActor::doDie();

	GetPartySys()->UpdateMember(this);

	GetDoing().EndDoing();	

	// 투기장 처리
	if (GetPlayerBattleArena().IsNowPlaying())
	{
		GetPlayerBattleArena().OnDie();
		return;
	}	

	if (GetPlayerPVPArea().IsLocateAndBelongTeam())
	{
		GetQPKillRewarder().Reward();
	}	

	gsys.pItemSystem->GetDurabilityUpdater().LostDurabilityByDie(this);
	
	// 마지막으로 죽은 시간 저장
	m_pPlayerInfo->nDeadTime = gsys.pSystem->GetNowTime();
	

	RouteDie();

	// 관련 정보 초기화
	SetKiller(MUID::Invalid());
	SetKillTalent(NULL);
}

void GEntityPlayer::GainPassiveTalentEffect(GTalentInfo* pTalentInfo)
{
	m_PassiveTalents.GainEffect(pTalentInfo);
}

void GEntityPlayer::LostPassiveTalentEffect( GTalentInfo* pTalentInfo )
{
	m_PassiveTalents.LostEffect(pTalentInfo);
}

void GEntityPlayer::GetBuffRemainTime(vector<REMAIN_BUFF_TIME>& outvecBuffRemainTime, bool bExceptFocusBuff, bool bExceptNoSaveDB)
{
	GetModuleBuff()->GetBuffRemainTimes(outvecBuffRemainTime, bExceptFocusBuff, bExceptNoSaveDB);
}

void GEntityPlayer::InsertBuffRemainTime(const vector<REMAIN_BUFF_TIME>& vecBuffRemainTime)
{
	GetModuleBuff()->InsertBuffRemainTimes(vecBuffRemainTime);
}

GActorMeshInfo* GEntityPlayer::GetMeshInfo()
{
	return &m_MeshInfo;
}


bool GEntityPlayer::HasParty()
{
	return m_uidParty.IsValid();
}

bool GEntityPlayer::IsPartyLeader()
{
	if (!HasParty())
		return false;

	return m_bPartyLeader;
}

bool GEntityPlayer::SetPartyLeader( bool bPartyLeader )
{
	if (!HasParty())
		return false;

	m_bPartyLeader = bPartyLeader;

	return true;
}

MUID GEntityPlayer::GetPartyUID() const
{
	return m_uidParty;
}

void GEntityPlayer::SetParty( MUID val )
{
	m_uidParty = val;
}

void GEntityPlayer::GameStart()
{
	if (GConfig::IsExpoMode())
	{
		gsys.pPartySystem->FixedPartyLogOn(m_UID, GetName());
	}
	else
	{
		gsys.pPartySystem->DoOnline(m_UID);
	}

	gsys.pPartySystem->DoOnline(m_UID);
	gsys.pGuildSystem->OnLine(this);
	
	gmgr.pBattleArenaMgr->TryRejoin(this);

	m_TimeSynchor.RouteTimeSynch(this);

	if (IsRunForTest())
	{
		RunDebugAction();
	}
}

bool GEntityPlayer::IsSameName( GEntityPlayer* pOtherPlayer )
{
	_ASSERT(pOtherPlayer);
	if (!pOtherPlayer)	return false;

	return IsSameName(pOtherPlayer->GetPlayerInfo()->szName);
}

bool GEntityPlayer::IsSameName( const wchar_t* szName )
{
	return (wcsncmp(GetPlayerInfo()->szName, szName, wcslen(GetPlayerInfo()->szName)) == 0);
}

void GEntityPlayer::GetSimpleStatus( int8& nHP, int8& nEN, int8& nSTA ) const
{
	PLAYER_INFO* pPlayerInfo = GetPlayerInfo();
	if (pPlayerInfo == NULL)	return;

	nHP  = (m_ChrStatus.nHP*100  / GetMaxHP());
	nEN	 = (m_ChrStatus.nEN*100  / GetMaxEN());
	nSTA = (m_ChrStatus.nSTA*100 / GetMaxSTA());
}

void GEntityPlayer::OnUpdatedStatus()
{
	RouteSimpleStatus(m_UID);

	GetPartySys()->UpdateMember(this);
}

void GEntityPlayer::RefreshStatusFromLevelup()
{
	PLAYER_INFO* pPlayerInfo = GetPlayerInfo();
	if (pPlayerInfo == NULL) return;

	pPlayerInfo->nMaxHP = CSBasicAttributeCalculator::CalcMaxHP(pPlayerInfo->nLevel, GetCON());
	pPlayerInfo->nMaxEN = CSBasicAttributeCalculator::CalcMaxEN(pPlayerInfo->nLevel, GetINT());
	pPlayerInfo->nMaxSTA = CSBasicAttributeCalculator::CalcMaxSP(pPlayerInfo->nLevel, GetDEX());

	SetHP(GetMaxHP());
	SetEN(GetMaxEN());
	SetSTA(GetMaxSTA());
}

GEntity* GEntityPlayer::FindEntity(const MUID& uidEntity)
{
	_ASSERT(NULL != m_pField);
	if (NULL == m_pField) return NULL;

	return m_pField->FindEntity(uidEntity);
}

GEntityPlayer* GEntityPlayer::FindPlayer(const MUID& uidPlayer)
{
	_ASSERT(NULL != m_pField);
	if (NULL == m_pField) return NULL;

	return m_pField->FindPlayer(uidPlayer);
}

GEntityNPC* GEntityPlayer::FindNPC(const MUID& uidNPC)
{
	_ASSERT(NULL != m_pField);
	if (NULL == m_pField) return NULL;

	return m_pField->FindNPC(uidNPC);
}

GEntityNPC* GEntityPlayer::FindNPC(const UIID nNPCUIID)
{
	_ASSERT(NULL != m_pField);
	if (NULL == m_pField) return NULL;

	return m_pField->FindNPC(nNPCUIID);
}

GEntityActor* GEntityPlayer::FindActor(const MUID& uidActor)
{
	_ASSERT(NULL != m_pField);
	if (NULL == m_pField) return NULL;

	return m_pField->FindActor(uidActor);
}


void GEntityPlayer::GetNeighborNPC(const float fRange, vector<GEntityNPC*>& vecOutNeighborNPC)
{
	_ASSERT(NULL != m_pField);
	if (NULL == m_pField) return;

	m_pField->GetNeighborNPC(m_vPos, fRange, vecOutNeighborNPC);
}

int GEntityPlayer::GetCID() const
{
//_ASSERT(NULL != m_pPlayerInfo);
	if (NULL == m_pPlayerInfo) return 0;

	return m_pPlayerInfo->nCID;
}


int GEntityPlayer::GetGID() const
{
	_ASSERT(NULL != m_pPlayerInfo);
	if (NULL == m_pPlayerInfo) return 0;

	return m_pPlayerInfo->nGID;
}

const wchar_t* GEntityPlayer::GetName() const
{
	if (!m_pPlayerInfo)
		return L"unknown_name";

	return m_pPlayerInfo->szName;
}

bool GEntityPlayer::IsStanceChangable()
{
	GModuleCombat* pCombatModule = this->GetModuleCombat();
	if (pCombatModule == NULL) 		return false;

	return !m_pItemHolder->GetEquipment().IsUnarmed();
}

int GEntityPlayer::GetAP() const
{
	int nAP = 0;

	if (m_GuardInfo.bGuarding)
	{
		int nGuardRank = GetPassiveValue(TEPT_GUARD);
		
		bool bEquipShield = IsEquipShield();
		uint32 nGuardTime = gsys.pSystem->GetNowTime() - m_GuardInfo.nGuardStartTime;
		float fAPFactor = GGuardCalculator::CalcAPFactor(nGuardRank, bEquipShield, nGuardTime);

		nAP = (int)(m_pItemHolder->GetEquipment().GetTotalAP() * fAPFactor);
	}
	else
	{
		nAP = m_pItemHolder->GetEquipment().GetTotalAP();
	}

	return m_ChrStatus.ActorModifier.nAP.Calc(m_pChrInfo->nAP+nAP);
}

bool GEntityPlayer::FailAndRouteSystemMsg( CCommandResultTable nRet )
{
#ifndef _DEBUG
	if (nRet >= CR_FAIL_DEBUG_MIN) return false;
#endif

	RouteSystemMsg(nRet);	

	return false;
}

void GEntityPlayer::RouteSystemMsg(CCommandResultTable nRet)
{
	gsys.pMsgCommandFacade->RouteSystemMsg(this, nRet);

	m_PlayerCRTLogger.Log(this, nRet);
}

bool GEntityPlayer::FailAndRouteAdviceMsg( CCommandResultTable nRet )
{
	RouteAdviceMsg(nRet);

	m_PlayerCRTLogger.Log(this, nRet);

	return false;
}

void GEntityPlayer::RouteAdviceMsg( CCommandResultTable nRet )
{
	gsys.pMsgCommandFacade->RouteAdviceMsg(this, nRet);
}

void GEntityPlayer::doSwimming()
{
	ChangeActionState(AS_SWIMMING);
}

void GEntityPlayer::doSwimmingRelease()
{
	ChangeActionState(AS_NORMAL);
}

bool GEntityPlayer::_GetMarkerFromSensorTrigger(const unsigned int nSensorID, const SENSOR_WARP_INFO& info, const MARKER_INFO*& pMarkerInfo)
{
	if (info.nFieldGroupID != INVALID_ID)
		return true;	// 동적 필드라면 무조건 유효하다 (파서에서 체크)

	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(info.nFieldID);
	if (!pFieldInfo)	
	{ 
		dlog("GEntityActor::_HandleWarpPortal - 해당 필드가 존재하지 않음 (SensorID:%d, FieldID:%d)\n", nSensorID, info.nFieldID); 
		return false;
	}
	pMarkerInfo = pFieldInfo->FindMarker(info.nMarkerID);
	if (!pMarkerInfo)
	{
		dlog("GEntityActor::_HandleWarpPortal - 해당 마커가 존재하지 않음 (SensorID:%d, FieldID:%d, MarkerID:%d)\n", nSensorID, info.nFieldID, info.nMarkerID); 
		return false;
	}

	return true;
}

void GEntityPlayer::_HandleMultipleWarpTrigger(void)
{
	// 기본 파라미터 생성
	SENSOR_WARP_INFO sample;
	int nTitleSize = sizeof(sample.szTitle);
	int nWarpCount = m_pWarpInfoCollector->GetWarpCount();

	MCommandParameterBlob* paramBlob = new MCommandParameterBlob(NULL, nTitleSize, nWarpCount);


	// 파라미터에 워프 타이틀 추가
	for(int i = 0; i < nWarpCount; i++)
	{
		const SENSOR_WARP_INFO* pWarpInfo = m_pWarpInfoCollector->GetSensorWarpInfo(i);
		VALID(pWarpInfo != NULL);

		paramBlob->SetBlobElement(i, pWarpInfo->szTitle, nTitleSize);
	}


	// 커맨드 전달
	MCommand* pNewCommand = MakeNewCommand(MC_TRIGGER_SENSOR_DESTINATION_SEL_REQ, 1, paramBlob);
	RouteToMe(pNewCommand);
}

bool GEntityPlayer::_HandleSingleWarpTrigger(int nWarpInfoCollectorIndex, int nSensorID, int nExitMarkerID)
{
	const SENSOR_WARP_INFO* pWarpInfo = m_pWarpInfoCollector->GetSensorWarpInfo(nWarpInfoCollectorIndex);
	VALID_RET(m_pWarpInfoCollector != NULL, false);

	if (pWarpInfo->nFieldID != INVALID_ID)
	{
		// 공유필드 워프
		const MARKER_INFO* pMarkerInfo=NULL;
		if (!_GetMarkerFromSensorTrigger(nSensorID, *pWarpInfo, pMarkerInfo))	return false;
		GetPlayerField().GateToMarker(*pMarkerInfo);
	}
	else
	{
		// 출구 설정 - EnterPos가 없을때면 설정한다. 기존 EnterPos를 덮어써선 안된다. ExitMarker는 워프 시도전에 설정해야한다.
		bool isSettedExitMarker = false;
		const MARKER_INFO& rEnterPos = GetPlayerField().GetFieldDynamic().GetEnterPos();

		if (rEnterPos.IsInvalid())
		{
			VALID_RET(m_pField != NULL, true);
			const GFieldInfo* pFieldInfo = GetField()->GetInfo();
			CHANNELID nChannelID = GetChannelID(GetField());
			
			if (SetSensorExitMarker(pFieldInfo, nExitMarkerID, nChannelID))
			{
				isSettedExitMarker = true;
			}
		}


		// 트라이얼필드 워프
		int nFieldGroupID = pWarpInfo->nFieldGroupID;
		bool bSingleEnter = m_pWarpInfoCollector->IsSingleQuestWarp(nWarpInfoCollectorIndex);		
				
		if (!GetPlayerField().GateToTrial(nFieldGroupID, bSingleEnter))
		{
			if (isSettedExitMarker)
			{
				GetPlayerField().GetFieldDynamic().ClearEnterPos();
			}

			VALID_RET(0 && L"동적 필드 초기화 실패", false);
		}
	}

	return true;
}

bool GEntityPlayer::_HandleAutoWarpTrigger(const SENSOR_INFO& infoSensor)
{
	// 조건에 맞는 Warp를 수집한다.
	VALID_RET(m_pWarpInfoCollector != NULL, false);
	GWarpInfoCollector::Result collectResult = m_pWarpInfoCollector->CollectAutoWarp(infoSensor, this);
	
	// 수집된 결과 처리
	switch(collectResult)
	{	
	case GWarpInfoCollector::SINGLE:
		{
			// 조건을 만족하는 한개의 워프가 존재한다.
			const int FIRST_WARP_INDEX = 0;
			int nSensorID = infoSensor.nID;
			int nExitMarkerID = infoSensor.nExitMarkerID;

			_HandleSingleWarpTrigger(FIRST_WARP_INDEX, nSensorID, nExitMarkerID);
		}
		break;
	case GWarpInfoCollector::MULTY:
		{
			// 둘 이상의 필드가 조건을 만족한다.
			m_nTriggedSensorID = infoSensor.nID;
			_HandleMultipleWarpTrigger();
		}
		break;
	case GWarpInfoCollector::FULLHOUSE:
		{
			// 입장 가능한 유일한 필드가 만원이다.
			RouteWarpFullHouseError();
		}
		break;	
	case GWarpInfoCollector::NOTHING:	
		{
			// 조건을 만족하는 워프 없다.
			return false;
		}
		break;
	default:
		VALID_RET(0, false);
		break;
	}

	return true;
}

bool GEntityPlayer::_HandleInnroomWarpTrigger( const SENSOR_INFO& infoSensor, GATE_TYPE eGateType )
{
	if (eGateType != infoSensor.vecWarpInfos.front().eGateType)
		return false;
	if (false == gsys.pConditionsSystem->Check(this, infoSensor.vecWarpInfos.front().nConditionID))
		return false;

	return GetPlayerField().GateToInnRoom();
}

bool GEntityPlayer::SetSensorExitMarker(const GFieldInfo* pFieldInfo, int nMarkerID, CHANNELID nChannelID)
{
	VALID_RET(pFieldInfo != NULL, false);

	if (nMarkerID != INVALID_ID)
	{
		const MARKER_INFO* pExitMarkerInfo = pFieldInfo->FindMarker(nMarkerID);
		if (pExitMarkerInfo == NULL)	return false;
		
		GetPlayerField().GetFieldDynamic().SetEnterPos(*pExitMarkerInfo, nChannelID);
	}

	return true;
}

bool GEntityPlayer::_HandleCheckpointTrigger( const SENSOR_INFO& infoSensor )
{
	assert(m_pField);
	if (!m_pField)					return true;
	const GFieldInfo* pFieldInfo = m_pField->GetInfo();
	assert(pFieldInfo);
	if (!pFieldInfo)		return true;

	if (infoSensor.nCheckpointMarkerID == INVALID_ID)
		return false;

	 const MARKER_INFO* pMarkerInfo = pFieldInfo->FindMarker(infoSensor.nCheckpointMarkerID);
	 if (!pMarkerInfo)
	 {
		 dlog("GEntityPlayer::_HandleCheckpointTrigger - 체크포인트 마커가 유효하지 않습니다. (SensorID:%d, FieldID:%d, MarkerID:%d)\n", infoSensor.nID, pFieldInfo->m_nFieldID, infoSensor.nCheckpointMarkerID);
		 return false;
	 }

	GetPlayerField().GetFieldDynamic().SetCheckpoint(*pMarkerInfo);

	MCommand* pNewCmd = MakeNewCommand(MC_CHAR_SAVE_CHECKPOINT, 
		1, 
		NEW_INT(pMarkerInfo->nFieldID));

	RouteToMe(pNewCmd);

	return false;
}

void GEntityPlayer::UpdateUsableSensor(bool bRoute)
{
	m_pSensorCacheUpdater->Update(this, m_pField, bRoute);
}

void GEntityPlayer::_ClearTriggerHistory()
{
	if (m_pWarpInfoCollector == NULL)	return;

	m_pWarpInfoCollector->Clear();
	m_nTriggedSensorID = INVALID_ID;
}

void GEntityPlayer::_HandleQuestTrigger(const SENSOR_INFO& infoSensor)
{
	gsys.pQuestSystem->GetQObjUpdater().UpdScoutQObj(this, this->GetFieldID(), infoSensor.nID);
}

bool GEntityPlayer::HandleManualWarpTrigger(const SENSOR_INFO& infoSensor)
{
	// 조건에 맞는 Warp를 수집한다.
	VALID_RET(m_pWarpInfoCollector != NULL, false);
	GWarpInfoCollector::Result collectResult = m_pWarpInfoCollector->CollectManualWarp(infoSensor, this);
	
	// 수집된 결과 처리
	switch(collectResult)
	{	
	case GWarpInfoCollector::SINGLE:
		{
			// 조건을 만족하는 한개의 워프가 존재한다.
			const int FIRST_WARP_INDEX = 0;
			int nSensorID = infoSensor.nID;
			int nExitMarkerID = infoSensor.nExitMarkerID;

			MUID uidStartField = GetFieldUID();
			int nCount = m_pWarpInfoCollector->GetAllowFollowWarpCount(FIRST_WARP_INDEX, this);
			int nFieldGroupID = m_pWarpInfoCollector->GetFieldGroupID(FIRST_WARP_INDEX);

			if (_HandleSingleWarpTrigger(FIRST_WARP_INDEX, nSensorID, nExitMarkerID))
			{				
				RouteFollowMeTrialField(uidStartField, nFieldGroupID, nCount);
			}
		}
		break;
	case GWarpInfoCollector::MULTY:
		{
			// 둘 이상의 필드가 조건을 만족한다.
			m_nTriggedSensorID = infoSensor.nID;
			_HandleMultipleWarpTrigger();
		}
		break;
	case GWarpInfoCollector::FULLHOUSE:
		{
			// 입장 가능한 유일한 필드가 만원이다.
			RouteWarpFullHouseError();
		}
		break;	
	case GWarpInfoCollector::NOTHING:	
		{
			// 조건을 만족하는 워프 없다.
			return false;
		}
		break;
	default:
		VALID_RET(0, false);
		break;
	}

	return true;
}

void GEntityPlayer::RouteFollowMeTrialField(MUID uidStartField, int nFieldGroupID, int nReceiveLimitCount)
{
	// 제한이 최소 1명 이상은 되야한다.
	if (nReceiveLimitCount < 1)	return;


	// 파티 찾기
	if (!HasParty())	return;

	MUID uidParty = GetPartyUID();
	GParty* pParty = gsys.pPartySystem->FindParty(uidParty);
	if (pParty == NULL)	return;


	// 커맨드 생성
	MCommand* pNewCmd = MakeNewCommand(MC_TRIGGER_SENSOR_FOLLOW_PARTY_REQ, 
										1,
										NEW_WSTR(GetName())
										);
	VALID(pNewCmd != NULL);
	

	// 퀘스트 공유 거리내 파티원들에게 따라올 것인지 통보
	bool isFirstReceiver = true;
	for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
	{
		MUID uidMember = it->first;

		// 자신은 제외
		if (uidMember == GetUID())	continue;

		// 같은 서버
		GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);		
		if (pMember == NULL)	continue;

		// 워프전 같은 필드
		if (uidStartField != pMember->GetFieldUID())	continue;

		// 퀘스트 공유거리 내
		if (GConst::QUEST_SHARE_DISTANCE < DistanceTo(pMember))	continue;
		
		// 리시버에 등록
		if (isFirstReceiver)
		{
			pNewCmd->SetReceiverUID(uidMember);		
			isFirstReceiver = false;
		}
		else
		{
			pNewCmd->AddReceiver(uidMember);
		}

		// 이동 필드그룹 기록
		pMember->SetFollowFieldGroupID(nFieldGroupID);

		// 전송제한 인원수까지만 추가
		if (nReceiveLimitCount <= pNewCmd->GetReceiverCount())	break;
	}

	
	// 커맨드 전송
	gsys.pCommandCenter->PostCommand(pNewCmd);
}

void GEntityPlayer::RouteWarpFullHouseError(void)
{
	MCommand* pNewCmd = gsys.pCommandCenter->NewCommand(MC_TRIGGER_SENSOR_ERROR_MSG, 0);
	RouteToMe(pNewCmd);
}

void GEntityPlayer::SelectWarpTrigger(int nSelection)
{	
	VALID(m_pWarpInfoCollector != NULL);
	VALID(0 <= nSelection);
	VALID(nSelection < m_pWarpInfoCollector->GetWarpCount());


	// 센서 확인
	if (m_nTriggedSensorID == INVALID_ID)	return;


	// 센서와의 거리 판정
	const SENSOR_INFO* pSensor = m_pField->GetInfo()->FindSensor(m_nTriggedSensorID);
	if (!pSensor)	return;

	float fDistance = pSensor->vPoint.DistanceTo(GetPos());
	if (fDistance >= pSensor->fRadius)	return;


	// 해당 필드 입장 조건 재검사
	GWarpInfoCollector::Result checkResult = m_pWarpInfoCollector->RecheckWarp(nSelection, this);
	
	if (checkResult == GWarpInfoCollector::NOTHING)	return;
	if (checkResult == GWarpInfoCollector::FULLHOUSE)
	{
		RouteWarpFullHouseError();
		return;
	}
	
	
	// 이동
	int nSensorID = m_nTriggedSensorID;
	int nExitMarkerID = pSensor->nExitMarkerID;

	MUID uidStartField = GetFieldUID();
	int nCount = m_pWarpInfoCollector->GetAllowFollowWarpCount(nSelection, this);
	int nFieldGroupID = m_pWarpInfoCollector->GetFieldGroupID(nSelection);		

	if (_HandleSingleWarpTrigger(nSelection, nSensorID, nExitMarkerID))
	{		
		RouteFollowMeTrialField(uidStartField, nFieldGroupID, nCount);		
	}
}

void GEntityPlayer::SensorInteraction(vector<uint32>& vecSensorID)	
{
	VALID(NULL != m_pField);
	VALID(NULL != m_pField->GetInfo());

	if (MAX_INTERACTABLE_SENSOR_COUNT < vecSensorID.size())
	{
		// 중첩된 센서들에 대한 인터랙션 요청에서 센서의 개수가 한도를 초과했을 경우 처리하지 않습니다.
		mlog3("There Are Many Overlapped Sensor! FieldID=%d, FirstSensorID=%d, CID=%d\n"
			, m_pField->GetInfo()->m_nFieldID, vecSensorID[0], this->GetCID());
		return;
	}

	// 클라이언트가 보내준 순서대로 검사해서 가장먼저 만족하는 센서를 처리한다.
	for each(uint32 nSensorID in vecSensorID)
	{
		const SENSOR_INFO* pSensor = m_pField->GetInfo()->FindSensor(nSensorID);
		if (!pSensor)																continue;

		// 사용가능한 센서인지 확인
		float fDistance = pSensor->vPoint.DistanceTo(GetPos());
		if (fDistance >= pSensor->fRadius)											continue;
		if (!IsEnteredSensorID(nSensorID))											continue;
		if (!m_pField->IsEnableSensor(nSensorID))									continue;
		
		// 워프 - 워프가 일어나면, 이하 센서는 무시한다.
		if (pSensor->HasInnroomWarp())
		{
			if(_HandleInnroomWarpTrigger(*pSensor, TGT_MANUAL))						break;
		}
		else
		{			
			if (HandleManualWarpTrigger(*pSensor))									break;
		}		
		
		// 워프 트리거가 없을 경우, 스크립트로 제어권 이양			
		GetScriptField()->OnSensorInteract(m_pField->GetDelegator(), pSensor->nID, GetDelegator());					
	}
}

int GEntityPlayer::GetWarpTriggerHistoryQty(void)
{
	if (m_pWarpInfoCollector == NULL)	return 0;
	return m_pWarpInfoCollector->GetWarpCount();
}

void GEntityPlayer::SetFollowFieldGroupID(int nFieldGroupID)
{
	m_nFieldGroupID = nFieldGroupID;
}

int GEntityPlayer::GetFollowFieldGroupID(void) const
{
	return m_nFieldGroupID;
}

void GEntityPlayer::OnHit( GHitInfo infoHit, bool bRouteHit )
{
	PFI_BLOCK_THISFUNC(9020);

	if (CheckBitSet(infoHit.nCombatResultFlags,CTR_CRITICAL))
	{
		// 크리티컬 타격인 경우
		m_PassiveTalents.ApplyEffect(TC_CRITICAL_HIT_ME, this, infoHit.pAttacker);
	}
	
	m_PassiveTalents.ApplyEffect(TC_HIT_ME, this, infoHit.pAttacker);

	__super::OnHit(infoHit, bRouteHit);

	if (true == GetSit().IsNowSit())
	{
		GetSit().Rise();
	}	
}

void GEntityPlayer::OnHitAll( GTalentInfo* pTalentInfo )
{
	m_PassiveTalents.ApplyEffect(TC_HIT_ALL_TALENT, this, NULL);

	__super::OnHitAll(pTalentInfo);
}

void GEntityPlayer::OnHitSuccess( GTalentInfo* pTalentInfo )
{
	m_PassiveTalents.ApplyEffect(TC_HIT_SUCCESS, this, NULL);

	__super::OnHitSuccess(pTalentInfo);
}


void GEntityPlayer::OnHitEnemy( uint32 nCombatResultFalg, GEntityActor* pVictim, GTalentInfo* pTalentInfo )
{
	VALID(pVictim);
	if (CheckBitSet(nCombatResultFalg,CTR_CRITICAL))
	{
		// 크리티컬 타격인 경우
		m_PassiveTalents.ApplyEffect(TC_CRITICAL_HIT_ENEMY, this, pVictim);
	}

	m_PassiveTalents.ApplyEffect(TC_HIT_ENEMY, this, pVictim);	

	__super::OnHitEnemy(nCombatResultFalg, pVictim, pTalentInfo);
}

void GEntityPlayer::OnMiss( uint32 nCombatResultFalg, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	VALID(pAttacker);
	m_PassiveTalents.ApplyEffect(TC_MISS_ME, this, pAttacker);

	__super::OnMiss(nCombatResultFalg, pAttacker, pAttackTalentInfo);
}

void GEntityPlayer::OnMissEnemy( uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo )
{
	VALID(pTarget);
	m_PassiveTalents.ApplyEffect(TC_MISS_ENEMY, this, pTarget);

	__super::OnMissEnemy(nCombatResultFalg, pTarget, pAttackTalentInfo);
}

void GEntityPlayer::OnGuard( GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	m_PassiveTalents.ApplyEffect(TC_GUARD_ME, this, pAttacker);

	__super::OnGuard(pAttacker, pAttackTalentInfo);
}


void GEntityPlayer::OnPerfectGuard( GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	m_PassiveTalents.ApplyEffect(TC_PERFECT_GUARD_ME, this, pAttacker);

	__super::OnPerfectGuard(pAttacker, pAttackTalentInfo);
}

void GEntityPlayer::OnAbsoluteGuard( GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	m_PassiveTalents.ApplyEffect(TC_PERFECT_GUARD_ME, this, pAttacker);

	__super::OnAbsoluteGuard(pAttacker, pAttackTalentInfo);
}

void GEntityPlayer::OnGuardEnemy( GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	VALID(pAttacker);
	m_PassiveTalents.ApplyEffect(TC_GUARD_ENEMY, this, pAttacker);

	__super::OnGuardEnemy(pAttacker, pAttackTalentInfo);
}

void GEntityPlayer::OnMagicHit( GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	m_PassiveTalents.ApplyEffect(TC_MAGIC_HIT_ME, this, pAttacker);

	__super::OnMagicHit(pAttacker, pTalentInfo);
}

void GEntityPlayer::OnMagicHitEnemy( GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	m_PassiveTalents.ApplyEffect(TC_MAGIC_HIT_ENEMY, this, pTarget);

	__super::OnMagicHitEnemy(pTarget, pTalentInfo);
}

void GEntityPlayer::OnMagicActTalent( GTalentInfo* pTalentInfo )
{
	m_PassiveTalents.ApplyEffect(TC_MAGIC_ACT, this, NULL);

	__super::OnMagicActTalent(pTalentInfo);
}

void GEntityPlayer::OnMeleeHit( GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	m_PassiveTalents.ApplyEffect(TC_MELEE_HIT_ME, this, pAttacker);

	__super::OnMeleeHit(pAttacker, pTalentInfo);
}

void GEntityPlayer::OnMeleeHitEnemy( GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	m_PassiveTalents.ApplyEffect(TC_MELEE_HIT_ENEMY, this, pTarget);

	__super::OnMeleeHitEnemy(pTarget, pTalentInfo);
}



void GEntityPlayer::OnSynchQuest()
{
	if (gsys.pPartySystem->IsPartyMember(this))
	{
		gsys.pPartySystem->UpdateQuestRunner(m_uidParty, this);
	}
	else
	{
		MUID uidPlayer = GetUID();
		MUID uidField = GetFieldUID();
		vector<int> vecQuestID;
		GetQuests().MakeActiveQuestID(&vecQuestID);

		for each(MUID uidField in m_setTrialField)
		{
			GDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(uidField);
			if (pDynamicFieldMaster == NULL)	continue;

			GDynamicFieldPolicy* pPolicy = pDynamicFieldMaster->GetPolicy();
			if (pPolicy == NULL) continue;

			pPolicy->OnQuestSynch(uidPlayer, uidField, &vecQuestID);			
		}		
	}
}

void GEntityPlayer::OnQuestComplete(int nQuestID)
{
	if (NULL == m_pField)
	{
		return;
	}
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	if (NULL == pQuestInfo) return;	

	if (pQuestInfo->IsTrialQuest())
	{
		for each(MUID uidField in m_setTrialField)
		{
			GDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(uidField);
			if (pDynamicFieldMaster == NULL)	continue;

			GDynamicFieldPolicy* pPolicy = pDynamicFieldMaster->GetPolicy();
			if (pPolicy == NULL) continue;

			pPolicy->OnQuestComplete(this, nQuestID);			
		}	
	}

	if (pQuestInfo->bChallengerQuest)
	{
		gmgr.pChallengerQuestMgr->OnChallengerQuestComplete(this);
	}
}

void GEntityPlayer::OnQuestDrop(int nQuestID)
{
	//assert(m_pField);
	if (!m_pField)					return;

	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);
	if (NULL == pQuestInfo) return;	

	if (pQuestInfo->IsTrialQuest())
	{
		for each(MUID uidField in m_setTrialField)
		{
			GDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(uidField);
			if (pDynamicFieldMaster == NULL)	continue;

			GDynamicFieldPolicy* pPolicy = pDynamicFieldMaster->GetPolicy();
			if (pPolicy == NULL) continue;

			pPolicy->OnQuestDrop(this, nQuestID);			
		}	
	}	
}

bool GEntityPlayer::IsStayDynamicField()
{
	if (!m_pField)	return false;

	return m_pField->IsDynamicField();
}

bool GEntityPlayer::IsHittable( GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo )
{
	if (GetPlayerInfo()->m_bGhost)
		return false; // 고스트 모드일때 때릴 수 없음

	if (!GEntityActor::IsHittable(pAttacker, pAttackTalentInfo)) 
		return false;

	// 잡기 진행중
	if (GetActionState() == AS_MF)
	{		 
		if (GetCurrentMotionFactor() == MF_GRAPPLED || GetCurrentMotionFactor() == MF_SWALLOWED)
		{
			return false;
		}
	}

	if (GetDoing().IsNowCutscening()) 
		return false; // 컷씬 진행중

	return true;
}

bool GEntityPlayer::DoScheduleMove()
{
	if (!__super::DoScheduleMove())
		return false;	// 스케쥴이동을 하지 않았음

	CheatCheck_MotionFactorMove();

	return true;
}

void GEntityPlayer::CheatCheck_Walk()
{
	m_pCheatCheck->CheckPosition(PCT_WALK, this);
}

void GEntityPlayer::CheatCheck_Warp()
{
	m_pCheatCheck->CheckPosition(PCT_WARP, this);
}

void GEntityPlayer::CheatCheck_MotionFactorMove()
{
	m_pCheatCheck->CheckPosition(PCT_MOTIONFACTOR, this);
}

bool GEntityPlayer::HasMotionfactor()
{
	MF_STATE nMF = m_MotionFactor.GetCurrMF();
	if (CSMotionFactorHelper::HasMotionFactor(nMF)) return true;

	return false;
}

bool GEntityPlayer::HasMovingMotionfactor()
{
	MF_STATE nMF = m_MotionFactor.GetCurrMF();
	if (CSMotionFactorHelper::IsMovingMotionFactor(nMF)) return true;

	return false;
}

bool GEntityPlayer::HasFocus(TALENT_FOCUS_TYPE nFocus)
{
	GModuleCombat* pmCombat = GetModuleCombat();
	VALID_RET(pmCombat, false);

	return pmCombat->HasFocus(nFocus);
}

bool GEntityPlayer::HasFocus()
{
	GModuleCombat* pmCombat = GetModuleCombat();
	VALID_RET(pmCombat, false);

	return pmCombat->HasFocus();
}

int GEntityPlayer::GetBerserkBubble()
{
	GModuleCombat* pmCombat = GetModuleCombat();
	VALID_RET(pmCombat, false);

	return pmCombat->GetBerserkBubble();
}

bool GEntityPlayer::IsEquipShield() const
{
	return m_pItemHolder->GetEquipment().IsEquipShield();	
}

void GEntityPlayer::OnEndMotionFactor()
{
	if (m_vReservedPosSync != vec3::ZERO)
	{
		vec3 vCurrentPos = GetPos();

		// 차이가 30 이하이면 무시
		if (m_vReservedPosSync.DistanceToSq(vCurrentPos) <= 900.0f)	
		{
			SyncMovingMotionFactorFinishPos(m_vReservedPosSync);
		}
		m_vReservedPosSync = vec3::ZERO;
	}
}

void GEntityPlayer::SyncMovingMotionFactorFinishPos( vec3& vPos )
{
	SetPos(vPos);
	CheatCheck_Walk();

	const UIID nPlayerUIID = GetUIID();

	// 위치 차이가 나므로 플레이어에게 이동 정보를 보내준다.
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_STOP_MOVE, 2, NEW_USHORT(nPlayerUIID), NEW_VEC(vPos));
	RouteToThisCellExceptMe(pNewCommand);
}

float GEntityPlayer::GetStandupTime()
{
	WEAPON_TYPE weaponType = m_pItemHolder->GetEquipment().GetWeaponType();
	return GConst::PLAYER_STANDUPTIME.GetStandupTime(weaponType);
}

void GEntityPlayer::AttachFieldObserver(MUID uidTrialField)
{
	VALID(uidTrialField.IsValid());
	m_setTrialField.insert(uidTrialField);
}

void GEntityPlayer::DetachFieldObserver(MUID uidTrialField)
{
	VALID(uidTrialField.IsValid());	
	m_setTrialField.erase(uidTrialField);
}

bool GEntityPlayer::UpdateQuestVar(int nQuestID, int nVar)
{
	return gsys.pQuestSystem->GetQuestVarUpdater().Update(this, nQuestID, nVar);
}

int	GEntityPlayer::GetQuestVar(int nQuestID)
{
	GPlayerQuest* pPlayerQuest = GetQuests().Get(nQuestID);
	if (NULL == pPlayerQuest) return 0;

	return pPlayerQuest->GetQuestVar();
}

bool GEntityPlayer::AddQuest(int nQuestID)
{
	return gsys.pQuestSystem->GetGiver().Give(this, nQuestID, true);
}

void GEntityPlayer::RuinQuest(int nQuestID)
{
	gsys.pQuestSystem->GetFailer().Fail(this, nQuestID);
}

bool GEntityPlayer::BindSoul(uint8 nSoulBindingID)
{
	return gsys.pPlayerSystem->SaveSoulBinding(this, nSoulBindingID);
}

bool GEntityPlayer::CheckCondition(int nConditionID)
{
	return gsys.pConditionsSystem->Check(this, nConditionID);
}

void GEntityPlayer::SetFatigue(int nPercent)
{
	gsys.pFatigueSystem->SetFatigueByCommand(this, nPercent);	
}

bool GEntityPlayer::AddMoney(int nMoney)
{
	return gsys.pMoneySystem->Increase(this, nMoney);
}

bool GEntityPlayer::RemoveMoney(int nMoney)
{
	return gsys.pMoneySystem->Decrease(this, nMoney);
}

int GEntityPlayer::GetMoney()
{
	if (!GetPlayerInfo())	return 0;
	return GetPlayerInfo()->GetMoney();
}

int GEntityPlayer::GetStorageMoney()
{
	if (!GetPlayerInfo())	return 0;
	return GetPlayerInfo()->GetStorageMoney();
}

GGlueActor* GEntityPlayer::NewDelegator()
{
	return new GGluePlayer(this);
}

GGluePlayer* GEntityPlayer::GetDelegator()
{
	VALID_RET(m_pDelegator, NULL);
	return static_cast<GGluePlayer*>(m_pDelegator);
}

void GEntityPlayer::Narration(const wchar_t* szText)
{
	MCommand* pNewCommand = MakeNewCommand(	MC_NPC_NARRATION, 
		1, 
		NEW_WSTR(szText));

	RouteToThisCell(pNewCommand);
}

void GEntityPlayer::NarrationNow(const wchar_t* szText)
{
	Narration(szText);
}

void GEntityPlayer::Tip(const wchar_t* szText)
{
	MCommand* pNewCommand = MakeNewCommand(	MC_CHAR_TIP,
		1, 
		NEW_WSTR(szText));

	RouteToMe(pNewCommand);
}

void GEntityPlayer::OnGateExecute( GField* pField, vec3 vPos, vec3 vDir )
{
	RVALID(pField);

	if (IsRunForTest())
	{
		mlog_filter("mizelan", "공유필드이동: %I64u -> %d:%d\n", GetUID(), pField->GetID(), GetChannelID(pField));
	}

	ClearEnteredSensorID();
	pField->ReservedEnter(this, vPos, vDir);

	int nIntroScene = pField->GetIntroCutsceneID();
	if (GetCutscene().IsSawn(nIntroScene))
	{
		nIntroScene = INVALID_ID;
	}

	// 메세지 전송
	GClientFieldRouter router;
	router.ChangeField(GetUID(), pField, vPos, vDir, nIntroScene);
}

void GEntityPlayer::OnGateComplete( GField* pField, vec3 vPos, vec3 vDir )
{
	EnterToField(pField, vPos, vDir);
	gsys.pGuildSystem->MoveField(this);

	if (IsRunForTest())
	{
		if (GConst::TEST_SHOW_NAVIGATION_MESH)
		{
			const GFieldInfo* pFieldInfo = pField->GetInfo();
			if (pFieldInfo)
			{
				MCommand* pNewCommand = MakeNewCommand(MC_DEBUG_STRING, 4, 
					NEW_WSTR(L"LoadNaviMesh"), 
					NEW_INT(0), 
					NEW_WSTR(pFieldInfo->m_strZoneFile.c_str()), 
					NEW_BLOB());

				RouteToMe(pNewCommand);
			}
		}
	}

	OnChangeField();
	
}

void GEntityPlayer::OnEnterDynamicField( GDynamicField* pField, vec3 vPos, vec3 vDir )
{
	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(GetPlayerField().GetDynamicFieldGroupUID());
	if (NULL == pFieldGroup) return;
	if (false == pFieldGroup->OnEnterPlayer(this)) return;

	if (gsys.pPartySystem->IsPartyMember(this))
	{
		gsys.pPartySystem->AddField(m_uidParty, this);
	}

	VALID(GetPlayerField().GetFieldDynamic().GetEnterPos().IsValid());
}

void GEntityPlayer::OnEnterSharedField( GSharedField* pField, vec3 vPos, vec3 vDir )
{
	GetPlayerField().GetFieldDynamic().ClearCheckpoint();	// 공유필드에 들어오면 동적필드에서 쓰이는 체크포인트는 지운다.

	// 이전 필드는 동적필드였음
	if (GetPlayerField().GetDynamicFieldGroupUID().IsValid())
	{
		MUID uidLeavedFieldGroup = GetPlayerField().GetDynamicFieldGroupUID();
		GetPlayerField().SetDynamicFieldGroupUID(MUID::Invalid());

		GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(uidLeavedFieldGroup);
		if (NULL != pFieldGroup)
		{
			pFieldGroup->OnLeavePlayer(this);
		}

	}
}

void GEntityPlayer::OnWarp(vec3 vPos, vec3 vDir)
{
	OnWarpSameField();
}


// 테스트용 행동을 수행
void GEntityPlayer::RunDebugAction()
{
	if (IsRunForTest())
	{
		if (GConst::TEST_MAKE_SOLO_PARTY)
		{
			// 파티가 없다면, 혼자 파티를 만든다.
			if (!HasParty())
			{
				gsys.pPartySystem->CreateSinglePartyReq(m_UID);
			}
		}
	}
}

void GEntityPlayer::OnRebirth()
{
	__super::OnRebirth();

	m_pKillTalentInfo = NULL;
	m_uidKiller.SetInvalid();
	m_pPlayerInfo->nElapsedDeadTimeSec = 0;

	ApplyAllPassiveTalents();

	if (m_pField)
	{
		GetPartySys()->UpdateMember(this);
		GetModuleBuff()->GainInvincibleBuff_Rebirth();
	}
}

void GEntityPlayer::OnRebirthBySelf()
{
	__super::OnRebirthBySelf();

	// 투기장 처리
	if (GetPlayerBattleArena().IsNowPlaying())
	{
		GetPlayerBattleArena().OnRebirthBySelf();
		return;
	}
}


void GEntityPlayer::OnRebirthByOther()
{
	__super::OnRebirthByOther();

	// 투기장 처리
	if (GetPlayerBattleArena().IsNowPlaying())
	{
		GetPlayerBattleArena().OnRebirthByOther();
		return;
	}
}

bool GEntityPlayer::IsBeginUnableAction()
{
	if (__super::IsBeginUnableAction())
		return true;

	// 투기장 점수판 볼때는 행동 금지
	if (GetPlayerBattleArena().IsNowDisplaySocreboard())
		return true;

	return false;
}

DAMAGE_ATTRIB GEntityPlayer::GetDamageType( const GTalentInfo* pTalentInfo/*=NULL*/ )
{
	DAMAGE_ATTRIB nDamageAttrib = __super::GetDamageType(pTalentInfo);
	if (nDamageAttrib != DA_NONE)
		return nDamageAttrib;

	const GItem* pItem = NULL;
	if (pTalentInfo)
	{
		switch (pTalentInfo->m_WeaponReference)
		{
		case WR_LEFT:	{ pItem = m_pItemHolder->GetEquipment().GetLeftWeapon();  }break;
		case WR_RIGHT:	{ pItem = m_pItemHolder->GetEquipment().GetRightWeapon(); }break;
		// 양손일때는 오른쪽만 적용
		case WR_BOTH:	{ pItem = m_pItemHolder->GetEquipment().GetRightWeapon(); }break;
			default:		
			{ 
				_ASSERT(0); 
				return DA_NONE;
			}
		}
	}

	// 무기가 없는 경우엔 BLUNT로 처리
	if (!pItem)
	{
		return DA_BLUNT;
	}

	VALID_RET(pItem->m_pItemData, DA_NONE);

	return pItem->m_pItemData->m_nDamageAttrib;
}

void GEntityPlayer::SetDie()
{
	m_bDead = true;
}

wstring GEntityPlayer::GetMotionFactorString()
{
	return m_MotionFactor.GetDebugString();
}

bool GEntityPlayer::IsNowInvincibility()
{
	if (true == __super::IsNowInvincibility()) return true;
	if (true == GetDoing().IsNowCutscening()) return true;

	return false;
}

bool GEntityPlayer::IsNowInvisibilityToNPC()
{
	if (true == __super::IsNowInvisibilityToNPC()) return true;
	if (true == GetDoing().IsNowCutscening()) return true;

	return false;
}

bool GEntityPlayer::HasPassiveTalent( TALENT_EXTRA_PASSIVE_TYPE nType )
{
	if (false == CSItemHelper::IsMustCheckPassiveType(nType)) return true;

	return m_PassiveTalents.HasEffect(nType);
}

bool GEntityPlayer::HasPassiveTalent( TALENT_EXTRA_PASSIVE_TYPE nType, int nExtraPassiveLevel )
{
	if (false == CSItemHelper::IsMustCheckPassiveType(nType)) return true;

	return m_PassiveTalents.HasEffect(nType, nExtraPassiveLevel);
}

void GEntityPlayer::RouteGuard()
{
	MCommand* pNewCmd = MakeNewCommand(MC_ACTION_GUARD, 3, NEW_USHORT(m_nUIID), NEW_SVEC2(GetDir()), NEW_VEC(GetPos()));
	RouteToThisCellExceptMe(pNewCmd);
}

void GEntityPlayer::RouteGuardFailed()
{
	MCommand* pNewCmd = MakeNewCommand(MC_ACTION_GUARD_FAILED, 1, NEW_USHORT(m_nUIID));
	RouteToThisCellExceptMe(pNewCmd);
}

bool GEntityPlayer::doUseTalent( GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target /*= TALENT_TARGET_INFO::Invalid()*/, bool bCheckEnabled/*=true*/, bool bGainStress/*=true*/ )
{
	VALID_RET(pTalentInfo, false);
	if (pTalentInfo->m_bUseOnlyGM &&
		(!IsGM() && !IsDeveloper()))
	{
		OnUseTalentFailed(pTalentInfo->m_nID, CR_FAIL_USE_TALENT_GM_ONLY);
		return false; // 개발자만 사용가능한 탤런트를 사용함
	}

	bool bRet = 
		__super::doUseTalent(pTalentInfo, Target, bCheckEnabled, bGainStress);

	if (!bRet)
	{
		RouteUseTalentRequestFailed(pTalentInfo->m_nID);
	}	

	return bRet;
}

bool GEntityPlayer::doUseTalent( int nTalentID, TALENT_TARGET_INFO Target /*= TALENT_TARGET_INFO::Invalid()*/, bool bCheckEnabled/*=true*/, bool bGainStress/*=true*/ )
{
	return __super::doUseTalent(nTalentID, Target, bCheckEnabled, bGainStress);
}

void GEntityPlayer::OnUseTalentFailed(int nTalentID, CCommandResultTable nFailCause)
{
	if (nFailCause == CR_FAIL_SYSTEM_INVALID_TALENT_ID)
	{
		dlog("%s - invalid talent (talentid: %d)\n", __FUNCTION__, nTalentID);
	}

	FailAndRouteSystemMsg(nFailCause);
}

void GEntityPlayer::OnFinishTalent(GTalentInfo* pTalentInfo)
{
	if (true == GetTalent().IsEnableUseItemTalent(pTalentInfo->m_nID))
	{
		gsys.pItemSystem->GetUser().ConsumeUsableItem(this, this->GetTalent().GetUsedTalentItemUID());
		this->GetTalent().DisableUseItemTalent();
	}	

	__super::OnFinishTalent(pTalentInfo);
}

bool GEntityPlayer::IsMovedFromOtherGameServer( void )
{
	return m_isMovedFromOtherGameServer;
}

void GEntityPlayer::OnMovedFromOtherGameServer( void )
{
	m_isMovedFromOtherGameServer = true;
}

float GEntityPlayer::GetSpellPower() const 
{
	const GItemHolder* pItemHolder = GetItemHolder();
	VALID_RET(pItemHolder, 1.0f);
	float fSpellPower = 0.0f;

	int nEquipQty = 0;
	{
		const GItem* pItem = pItemHolder->GetEquipment().GetLeftWeapon();
		if (pItem)
		{
			++nEquipQty;
			VALID_RET(pItem->m_pItemData, 0.0f);
			fSpellPower += pItem->m_pItemData->m_fSpellPower;
		}
	}
	
	{
		const GItem* pItem = pItemHolder->GetEquipment().GetRightWeapon();
		if (pItem)
		{
			++nEquipQty;
			VALID_RET(pItem->m_pItemData, 0.0f);
			fSpellPower += pItem->m_pItemData->m_fSpellPower;
		}
	}

	if (nEquipQty == 0)
		return 1.0f; // 기본값

	return fSpellPower / nEquipQty;
}

bool GEntityPlayer::IsDeveloper() const
{
	return CSCharHelper::IsDeveloperGrade(GetPlayerGrade());
}

bool GEntityPlayer::IsGM() const
{
	return CSCharHelper::IsGMGrade(GetPlayerGrade());
}

bool GEntityPlayer::IsGod() const
{
	return GetPlayerInfo()->IsGod();
}

void GEntityPlayer::OnLeaveFromParty()
{
	GDynamicFieldMaster* pDynamicFieldMaster = gmgr.pFieldMgr->GetDynamicFieldMaster(GetPlayerField().GetDynamicFieldGroupUID());
	if (pDynamicFieldMaster == NULL)		
		return; // 동적 필드가 아님

	if (pDynamicFieldMaster->GetPolicy()->GetType() != DPT_TRIAL)
		return; // 트라이얼 필드가 아님

	// 시작 위치로 보냄
	GetPlayerField().GetFieldDynamic().GateToEnterPos(GetFieldID());
}

float GEntityPlayer::DefaultWalkSpeed() const 
{
	WEAPON_TYPE nWeaponType = GetItemHolder()->GetEquipment().GetWeaponType();
	VALID_RET(nWeaponType >= WEAPON_NONE, 0.0f);
	VALID_RET(nWeaponType < WEAPON_MAX, 0.0f);
	return (float)MoveSpeedByWeapon[nWeaponType];
}

void GEntityPlayer::MakeDBCacheData(GDBCACHEDATA_CHARACTER& data)
{
	data.nXP = GetPlayerInfo()->nXP;
	data.nMoney = GetMoney();
}

bool GEntityPlayer::IsNowInvisibility() const 
{
	if (GetPlayerInfo()->m_bGhost)
		return true;

	return __super::IsNowInvisibility();
}

bool GEntityPlayer::IsNowGhost() const 
{
	return GetPlayerInfo()->m_bGhost;
}

MCommand* GEntityPlayer::MakeCmd_Whois( GEntitySync* pReqSync )
{
	GEntityPlayer* pPlayer = ::ToEntityPlayer(pReqSync);
	if (pPlayer == NULL)	return NULL;

	TD_UPDATE_CACHE_PLAYER td_player_info;
	MakeTDCacheInfo(td_player_info);

	return MakeNewCommand(MC_FIELD_IN_PLAYER, 1, NEW_SINGLE_BLOB(&td_player_info, sizeof(TD_UPDATE_CACHE_PLAYER)));
}

int GEntityPlayer::GetMaxHPProto() const 
{
	return CSBasicAttributeCalculator::CalcMaxHP(GetLevel(), GetCON());
}

int GEntityPlayer::GetMaxENProto() const 
{
	return CSBasicAttributeCalculator::CalcMaxEN(GetLevel(), GetINT());
}

int GEntityPlayer::GetMaxSTAProto() const 
{
	return CSBasicAttributeCalculator::CalcMaxSP(GetLevel(), GetDEX());
}

bool GEntityPlayer::CheckAllowAuctionMsgTime(uint32 nNowTime)
{
	// 최초 거래 메시지 - time이 한바퀴 돌면, 다시 0이 나올 수도 있지만, 무시한다.
	if (m_nLastAuctionMsgTime == 0)
	{
		m_nLastAuctionMsgTime = nNowTime;
		return true;
	}


	// 시간차를 구한다
	uint32 nDifference = 0;

	if (m_nLastAuctionMsgTime <= nNowTime)
	{
		nDifference = nNowTime - m_nLastAuctionMsgTime;
	}
	else
	{
		nDifference = UINT_MAX - m_nLastAuctionMsgTime + nNowTime;
	}


	// 1분제한 확인
	if (60000 <= nDifference)
	{
		m_nLastAuctionMsgTime = nNowTime;
		return true;
	}
	else
	{
		return false;
	}
}

int64 GEntityPlayer::GetAID() const
{
	GPlayerObject* pPlayerObj = gmgr.pPlayerObjectManager->GetPlayer(GetUID());
	if (NULL == pPlayerObj)
		return 0;

	return pPlayerObj->GetAccountInfo().nAID;
}

void GEntityPlayer::CollectEssentialReceiver( vector<MUID>& vecReceiverUID )
{
	GField* pField = GetField();
	if (NULL == pField) return;

	vector<MUID> vecMemberUID = gsys.pPartySystem->CollectMemberUID(this);
	for each (MUID uidMember in vecMemberUID)
	{
		GEntityPlayer* pMember = pField->FindPlayer(uidMember);
		if (NULL == pMember) continue;

		if (1 >= abs(GetSectorPoint().x - pMember->GetSectorPoint().x) &&
			1 >= abs(GetSectorPoint().y - pMember->GetSectorPoint().y))
		{
			vecReceiverUID.push_back(pMember->GetUID());
		}		
	}
}