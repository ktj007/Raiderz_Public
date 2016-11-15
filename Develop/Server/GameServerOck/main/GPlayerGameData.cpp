#include "stdafx.h"
#include "GPlayerGameData.h"
#include "GEntityPlayer.h"
#include "GCommandTable.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"
#include "GPlayerTalent.h"
#include "GPalette.h"
#include "GPlayerBattleArena.h"
#include "GPaletteItem.h"
#include "GPlayerQuests.h"
#include "GPlayerQuest.h"
#include "GQuestInfo.h"
#include "GPlayerQObjective.h"
#include "GPlayerFactions.h"
#include "GPlayerFaction.h"
#include "GFactionInfo.h"
#include "GFieldInfoMgr.h"
#include "GBattleArenaMgr.h"
#include "GItemManager.h"
#include "GPaletteSystem.h"
#include "GQuestInfoMgr.h"
#include "GFactionInfoMgr.h"
#include "GPlayerFieldMasterServer.h"
#include "GPlayerFieldDynamic.h"
#include "GSharedField.h"
#include "GCalculator.h"
#include "GActorCooltimeChecker.h"
#include "GPlayerCutscene.h"
#include "GItemHolder.h"
#include "GPlayerRecipe.h"
#include "CSBasicAttributeCalculator.h"
#include "GPlayerDBCache.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GServer.h"
#include "GDBCacheSystem.h"
#include "GPlayerMailBox.h"
#include "GEquipmentSlot.h"


GPlayerGameData::GPlayerGameData() : m_bPacked(false)
{
	// do noting
}

GPlayerGameData::~GPlayerGameData()
{
	// do noting
}

bool GPlayerGameData::PackFromCommand(MCommand* pCommand)
{
	if (MMC_COMM_REQUEST_READY_ENTER_MOVE_PLAYER != pCommand->GetID())
	{
		_ASSERT(0);
		return false;
	}

	MUID ConnectionKey;
	MUID uidRequestGameServer;
	MUID uidRequestPlayer;

	int nParamIndex = 0;
	if (!pCommand->GetParameter(&ConnectionKey,				nParamIndex++,	MPT_UID))		return false;
	if (!pCommand->GetParameter(&uidRequestGameServer,		nParamIndex++,	MPT_UID))		return false;
	if (!pCommand->GetParameter(&uidRequestPlayer,			nParamIndex++,	MPT_UID))		return false;

	int nCount;
	TD_GATE_INFO* pGateInfo;
	TD_DYNAMIC_FIELD_ENTER_POS_INFO* pDynamicFieldEnterPosInfo;
	TD_MOVE_PLAYER_INFO* pPlayerInfoData;
	TD_PLAYER_GAME_DATA_CHARAINFO* pCharaInfoData;	

	if (!pCommand->GetBlob(pGateInfo, nCount,					nParamIndex++))			return false;
	if (!pCommand->GetBlob(pDynamicFieldEnterPosInfo, nCount,	nParamIndex++))			return false;
	if (!pCommand->GetBlob(pPlayerInfoData, nCount,				nParamIndex++))			return false;
	if (!pCommand->GetBlob(pCharaInfoData, nCount,				nParamIndex++))			return false;
	m_GateInfo = *pGateInfo;
	CopyMemory(&m_DynamicFieldEnterPosInfo, pDynamicFieldEnterPosInfo, sizeof(m_DynamicFieldEnterPosInfo));
	CopyMemory(&m_PlayerInfoData, pPlayerInfoData, sizeof(m_PlayerInfoData));
	CopyMemory(&m_CharaInfoData, pCharaInfoData, sizeof(m_CharaInfoData));

	if (!pCommand->GetBlob(m_vecEquipData,					nParamIndex++))			return false;
	if (!pCommand->GetBlob(m_vecItemInstanceData,			nParamIndex++))			return false;
	if (!pCommand->GetBlob(m_vecTalentData,					nParamIndex++))			return false;
	if (!pCommand->GetBlob(m_vecPaletteData,				nParamIndex++))			return false;
	if (!pCommand->GetBlob(m_vecTalentCollTimeSec,			nParamIndex++))			return false;
	if (!pCommand->GetBlob(m_vecBuffRemaindSec,				nParamIndex++))			return false;
	if (!pCommand->GetBlob(m_vecQuestInstanceData,			nParamIndex++))			return false;
	if (!pCommand->GetBlob(m_vecQuestDoneCount,				nParamIndex++))			return false;
	if (!pCommand->GetBlob(m_vecFactionData,				nParamIndex++))			return false;
	if (!pCommand->GetBlob(m_vecSawnCutscene,				nParamIndex++))			return false;
	if (!pCommand->GetBlob(m_vecRecipe,						nParamIndex++))			return false;

	m_bPacked = true;

	return true;
}

void GPlayerGameData::PackGateInfo(GEntityPlayer* pPlayer)
{
	pPlayer->GetPlayerField().GetFieldMasterServer().GetDestGateInfo().Export(m_GateInfo);
}

void GPlayerGameData::PackDynamicFieldEnterPosInfo(GEntityPlayer* pPlayer)
{
	MARKER_INFO marker;
	if (pPlayer->GetPlayerField().GetFieldMasterServer().GetDestGateInfo().bDynamicField)
	{
		if (pPlayer->IsStayDynamicField())
		{
			marker = pPlayer->GetPlayerField().GetFieldDynamic().GetEnterPos();
		}
		else
		{
			marker.nFieldID = pPlayer->GetFieldID();
			marker.vPoint = pPlayer->GetPos();
			marker.vDir = pPlayer->GetDir();
		}		
	}

	m_DynamicFieldEnterPosInfo.nFieldID = marker.nFieldID;
	m_DynamicFieldEnterPosInfo.vPos = marker.vPoint;
	m_DynamicFieldEnterPosInfo.vDir = marker.vDir;
	m_DynamicFieldEnterPosInfo.nChannelID = GetChannelID(pPlayer->GetField());
}

void GPlayerGameData::PackMovePlayerInfo(GEntityPlayer* pEntityPlayer)
{	
	MUID uidPlayer = pEntityPlayer->GetUID();
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	uint32 nAID = 0;
	
	if (pPlayerObject)
	{
		nAID = (int)pPlayerObject->GetAccountInfo().nAID;
	}

	m_PlayerInfoData.nEnterKey = 0;
	m_PlayerInfoData.nPlayerAID = nAID;
}

void GPlayerGameData::PackCharaInfo(GEntityPlayer* pEntityPlayer)
{
	m_CharaInfoData.uidParty = pEntityPlayer->GetPartyUID();


	PLAYER_INFO* pPlayerInfo = pEntityPlayer->GetPlayerInfo();
	m_CharaInfoData.nSex = pPlayerInfo->nSex;
	m_CharaInfoData.nCID = pPlayerInfo->nCID;
	m_CharaInfoData.nLevel = pPlayerInfo->nLevel;
	m_CharaInfoData.nXP = pPlayerInfo->nXP;
	m_CharaInfoData.nMoney = pPlayerInfo->GetMoney();
	m_CharaInfoData.nStorageMoney = pPlayerInfo->GetStorageMoney();
	m_CharaInfoData.nGID = pPlayerInfo->nGID;
	m_CharaInfoData.nFeatureHair = pPlayerInfo->nFeatureHair;
	m_CharaInfoData.nFeatureFace = pPlayerInfo->nFeatureFace;
	m_CharaInfoData.nFeatureHairColor = pPlayerInfo->nFeatureHairColor;
	m_CharaInfoData.nFeatureSkinColor = pPlayerInfo->nFeatureSkinColor;
	m_CharaInfoData.nSoulBindingID = pPlayerInfo->nSoulBindingID;	
	m_CharaInfoData.nInnRoomID = pPlayerInfo->nInnRoomID;
	wcsncpy_s(m_CharaInfoData.szName, pPlayerInfo->szName, _TRUNCATE);
	m_CharaInfoData.nFatigueQuantity = pPlayerInfo->nFatigueQuantity;
	m_CharaInfoData.nBlankTimeSec = pPlayerInfo->nBlankTimeSec;	
	m_CharaInfoData.nPlayerGrade = pPlayerInfo->nPlayerGrade;
	m_CharaInfoData.nPlayTimeSec = pPlayerInfo->nPlayTimeSec;
	m_CharaInfoData.nConnPlayTimeSec = pPlayerInfo->nConnPlayTimeSec;
	m_CharaInfoData.nElapsedDeadTimeSec = pPlayerInfo->nElapsedDeadTimeSec;
	m_CharaInfoData.nGID = pPlayerInfo->nGID;


	GPlayerTalent& PlayerTalent = pEntityPlayer->GetTalent();
	m_CharaInfoData.nRemaindTP = PlayerTalent.GetTP();


	CHR_STATUS* pChrStatus = pEntityPlayer->GetChrStatus();		
	m_CharaInfoData.nHP = pChrStatus->nHP;
	m_CharaInfoData.nEN = pChrStatus->nEN;
	m_CharaInfoData.nSTA = pChrStatus->nSTA;
		

	GItemHolder* pItemHolder = pEntityPlayer->GetItemHolder();
	m_CharaInfoData.nWeaponSet = pItemHolder->GetEquipment().GetWeaponSet();
	
	
	GPalette& Palette = pEntityPlayer->GetPalette();
	m_CharaInfoData.ActivePaletteID = Palette.GetSelectedPalette();
	m_CharaInfoData.PrimaryPaletteID = Palette.GetPrimary();
	m_CharaInfoData.SecondaryPaletteID = Palette.GetSecondary();
	
	
	GPlayerBattleArena& BattelArena = pEntityPlayer->GetPlayerBattleArena();
	m_CharaInfoData.uidArena = BattelArena.GetBattleArenaUID();
	m_CharaInfoData.uidArenaTeam;	// 설정 없음.

	// 편지함 정보
	m_CharaInfoData.nMailTopUID = pEntityPlayer->GetMailbox().GetDBMailUID().GetTop();
	m_CharaInfoData.nMailBottomUID = pEntityPlayer->GetMailbox().GetDBMailUID().GetBottom();
}

void GPlayerGameData::PackItemList(GEntityPlayer* pEntityPlayer)
{
	vector<GItem*> vecItem;
	GItemHolder* pItemHolder = pEntityPlayer->GetItemHolder();		
	pItemHolder->CollectAllItem(SLOTTYPE_EQUIP, vecItem);
	pItemHolder->CollectAllItem(SLOTTYPE_INVENTORY, vecItem);
	pItemHolder->CollectAllItem(SLOTTYPE_STORAGE, vecItem);

	for each(GItem* pItem in vecItem)
	{
		_ASSERT(pItem->m_pItemData != NULL);
		if (MAX_INVENTORY_SIZE <= m_vecItemInstanceData.size())
		{
			dlog("Overflow item pack as GPlayerGameData::PackEquipAndItem(), Total = %d\n", vecItem.size());
			break;
		}

		m_vecItemInstanceData.push_back(pItem->ToTD_PLAYER_GAME_DATA_ITEM_INSTANCE());
	}
}

void GPlayerGameData::PackTalent(GEntityPlayer* pEntityPlayer)
{
	GPlayerTalent& PlayerTalent = pEntityPlayer->GetTalent();	
	SET_TALENTID& setTalentID = PlayerTalent.GetContainer();

	for each(int nTalentID in setTalentID)
	{
		m_vecTalentData.push_back(nTalentID);
	}
}

void GPlayerGameData::PackPaletteList(GEntityPlayer* pEntityPlayer)
{
	GPalette& Palette = pEntityPlayer->GetPalette();
	VEC_PALETTE_ITEM& vecPaletteItem = Palette.GetContainer();

	for(size_t i = 0; i < vecPaletteItem.size(); i++)
	{
		TD_PLAYER_GAME_DATA_PALETTE PaletteData;

		PaletteData.nOrderID = i;
		PaletteData.nType = vecPaletteItem[i].m_nType;
		PaletteData.nID = vecPaletteItem[i].m_nItemIDorTalentID;

		m_vecPaletteData.push_back(PaletteData);		
	}
}

void GPlayerGameData::PackTalentRemainTime(GEntityPlayer* pEntityPlayer)
{
	// 탤런트 쿨타임
	pEntityPlayer->GetActorCooltimeChecker().GetTalentRemainCoolTime(m_vecTalentCollTimeSec);
	

	// 버프 남은 시간
	vector<REMAIN_BUFF_TIME> vecRemainBuffTime;
	pEntityPlayer->GetBuffRemainTime(vecRemainBuffTime, true);

	for(vector<REMAIN_BUFF_TIME>::iterator it = vecRemainBuffTime.begin(); it != vecRemainBuffTime.end(); it++)
	{
		TD_REMAIN_BUFF_TIME tdRemainBuffTime;

		tdRemainBuffTime.nID = it->nID;
		tdRemainBuffTime.fRemainDurationSeconds = it->fRemainDurationSeconds;
		tdRemainBuffTime.fRemainNextPeriodSeconds = it->fRemainNextPeriodSeconds;
		tdRemainBuffTime.fRemainBuffOffsetSeconds = it->fRemainBuffOffsetSeconds;
		tdRemainBuffTime.nStackedCount = it->nStackedCount;

		m_vecBuffRemaindSec.push_back(tdRemainBuffTime);
	}
}

void GPlayerGameData::PackQuestInstance(GEntityPlayer* pEntityPlayer)
{
	GPlayerQuests& PlayerQuests = pEntityPlayer->GetQuests();
	MAP_PLAYER_QUEST& mapPlayerQuest = PlayerQuests.GetQuestContainer();	

	for each(const MAP_PLAYER_QUEST::value_type& quest in mapPlayerQuest)
	{
		// QuestInstance
		TD_PLAYER_GAME_DATA_QUEST_INSTANCE QuestData;

		GPlayerQuest* pQuest = quest.second;
		QuestData.nSlotID = pQuest->GetSlotID();
		QuestData.nVar = pQuest->GetQuestVar();
		QuestData.nDone = pQuest->GetState();
		QuestData.nAcceptTime = pQuest->GetAcceptTime();

		wcsncpy_s(QuestData.szAcceptDt, pQuest->GetAcceptDt().c_str(), _TRUNCATE);
		wcsncpy_s(QuestData.szExpiDt, pQuest->GetExpiDt().c_str(), _TRUNCATE);
		
		GQuestInfo* pQuestInfo = pQuest->GetInfo();		
		QuestData.nQID = pQuestInfo->nID;

		QuestData.nDoneCount = PlayerQuests.GetDoneCount(pQuestInfo->nID);
		

		GPlayerQObjective* pPlayerQObjective; 
		
		pPlayerQObjective = pQuest->Get(1);
		if (pPlayerQObjective != NULL)
		{
			QuestData.nObj1Progress = pPlayerQObjective->GetProgress();
			QuestData.bObj1Complete = pPlayerQObjective->IsComplete();
		}

		pPlayerQObjective = pQuest->Get(2);
		if (pPlayerQObjective != NULL)
		{
			QuestData.nObj2Progress = pPlayerQObjective->GetProgress();
			QuestData.bObj2Complete = pPlayerQObjective->IsComplete();
		}

		pPlayerQObjective = pQuest->Get(3);
		if (pPlayerQObjective != NULL)
		{
			QuestData.nObj3Progress = pPlayerQObjective->GetProgress();
			QuestData.bObj3Complete = pPlayerQObjective->IsComplete();
		}

		pPlayerQObjective = pQuest->Get(4);
		if (pPlayerQObjective != NULL)
		{
			QuestData.nObj4Progress = pPlayerQObjective->GetProgress();
			QuestData.bObj4Complete = pPlayerQObjective->IsComplete();
		}

		pPlayerQObjective = pQuest->Get(5);
		if (pPlayerQObjective != NULL)
		{
			QuestData.nObj5Progress = pPlayerQObjective->GetProgress();
			QuestData.bObj5Complete = pPlayerQObjective->IsComplete();
		}

		m_vecQuestInstanceData.push_back(QuestData);
	}


	// Done
	MAP_DONE_COUNT& mapQuestDoneCount = PlayerQuests.GetDoneCountContainer();
	
	for each(const MAP_DONE_COUNT::value_type& doneCount in mapQuestDoneCount)
	{
		TD_PLAYER_GAME_DATA_QUEST_DONE_COUNT QuestDoneCount;

		QuestDoneCount.nQID = doneCount.first;
		QuestDoneCount.nDoneCount = doneCount.second;

		m_vecQuestDoneCount.push_back(QuestDoneCount);
	}
}

void GPlayerGameData::PackFaction(GEntityPlayer* pEntityPlayer)
{
	GPlayerFactions& PlayerFactions = pEntityPlayer->GetPlayerFactions();	
	MAP_PLAYERFACTION& mapPlayerFaction = PlayerFactions.GetContainer();

	for each(const MAP_PLAYERFACTION::value_type& Faction in mapPlayerFaction)
	{
		TD_PLAYER_GAME_DATA_FACTION FactionData;
		
		GFactionInfo* pFactionInfo = Faction.second->GetInfo();
		FactionData.nFactionID = pFactionInfo->m_nID;
		FactionData.nFaction = Faction.second->GetQuantity();

		m_vecFactionData.push_back(FactionData);
	}
}

void GPlayerGameData::PackSawnCutscene(GEntityPlayer* pEntityPlayer)
{
	GPlayerCutscene& PlayerCutscene = pEntityPlayer->GetCutscene();	
	SET_SAWN_CUTSCENE& setSawnCutscene = PlayerCutscene.GetContainer();

	for each(int nCutsceneID in setSawnCutscene)
	{
		m_vecSawnCutscene.push_back(nCutsceneID);
	}
}

void GPlayerGameData::PackRecipe(GEntityPlayer* pPlayer)
{
	const SET_RECIPEID& setRecipeID = pPlayer->GetPlayerRecipe().GetContainer();

	m_vecRecipe.resize(setRecipeID.size());
	copy(setRecipeID.begin(), setRecipeID.end(), m_vecRecipe.begin());
}

void GPlayerGameData::PackFromEntityPlayer(GEntityPlayer* pPlayer)
{
	_ASSERT(pPlayer != NULL);

	// 주요 정보
	PackGateInfo(pPlayer);
	PackDynamicFieldEnterPosInfo(pPlayer);
	PackMovePlayerInfo(pPlayer);
	PackCharaInfo(pPlayer);

	// 나머지 정보
	PackItemList(pPlayer);
	PackTalent(pPlayer);
	PackPaletteList(pPlayer);
	PackTalentRemainTime(pPlayer);
	PackQuestInstance(pPlayer);
	PackFaction(pPlayer);
	PackSawnCutscene(pPlayer);
	PackRecipe(pPlayer);

	m_bPacked = true;
}

void GPlayerGameData::UnpackToEntityPlayer(GEntityPlayer* pPlayer)
{
	// 주요 정보
	UnpackGateInfo(pPlayer);
	UnpackDynamicFieldEnterPosInfo(pPlayer);
	UnpackMovePlayerInfo(pPlayer);
	UnpackCharaInfo_Pre(pPlayer);

	// 나머지 정보 - 일부 설정 주요정보에 종속
	UnpackItemList(pPlayer);
	UnpackTalent(pPlayer);
	UnpackPaletteList(pPlayer);
	UnpackTalentRemainTime(pPlayer);
	UnpackQuestInstance(pPlayer);
	UnpackFaction(pPlayer);
	UnpackSawnCutscene(pPlayer);
	UnpackRecipe(pPlayer);

	UnpackCharaInfo_Post(pPlayer);
}

void GPlayerGameData::UnpackGateInfo(GEntityPlayer* pPlayer)
{
	GGateInfo gateInfo;
	gateInfo.Assign(m_GateInfo);
	pPlayer->GetPlayerField().SetGateInfo(gateInfo);
}

void GPlayerGameData::UnpackDynamicFieldEnterPosInfo(GEntityPlayer* pPlayer)
{
	if (0 >= m_DynamicFieldEnterPosInfo.nFieldID) return;

	MARKER_INFO marker;
	marker.nFieldID = m_DynamicFieldEnterPosInfo.nFieldID;
	marker.vPoint = m_DynamicFieldEnterPosInfo.vPos;
	marker.vDir = m_DynamicFieldEnterPosInfo.vDir;

	CHANNELID nChannelID = m_DynamicFieldEnterPosInfo.nChannelID;

	pPlayer->GetPlayerField().GetFieldDynamic().SetEnterPos(marker, nChannelID);
}

void GPlayerGameData::UnpackMovePlayerInfo(GEntityPlayer* pPlayer)
{
	m_PlayerInfoData.nEnterKey;		// 검사용.
	m_PlayerInfoData.nPlayerAID;	// 검사용.
}

void GPlayerGameData::UnpackCharaInfo_Pre(GEntityPlayer* pPlayer)
{
	// PlayerInfo
	PLAYER_INFO* pPlayerInfo = pPlayer->GetPlayerInfo();

	pPlayerInfo->nCID				= m_CharaInfoData.nCID;
	pPlayerInfo->nSex				= (SEX)m_CharaInfoData.nSex;
	pPlayerInfo->nFeatureHair		= m_CharaInfoData.nFeatureHair;
	pPlayerInfo->nFeatureFace		= m_CharaInfoData.nFeatureFace;
	pPlayerInfo->nFeatureHairColor	= m_CharaInfoData.nFeatureHairColor;
	pPlayerInfo->nFeatureSkinColor	= m_CharaInfoData.nFeatureSkinColor;
	pPlayerInfo->nLevel				= m_CharaInfoData.nLevel;
	pPlayerInfo->nXP				= m_CharaInfoData.nXP;
	pPlayerInfo->SetMoney(m_CharaInfoData.nMoney);
	pPlayerInfo->SetStorageMoney(m_CharaInfoData.nStorageMoney);
	pPlayerInfo->nSoulBindingID = m_CharaInfoData.nSoulBindingID;
	pPlayerInfo->nFatigueQuantity = m_CharaInfoData.nFatigueQuantity;
	pPlayerInfo->nInnRoomID = m_CharaInfoData.nInnRoomID;
	pPlayerInfo->nGID = m_CharaInfoData.nGID;
	pPlayerInfo->m_nWeaponSet = (SH_ITEM_SWITCH_WEAPON)m_CharaInfoData.nWeaponSet;
	pPlayerInfo->nBlankTimeSec = m_CharaInfoData.nBlankTimeSec;	
	pPlayerInfo->nPlayTimeSec = m_CharaInfoData.nPlayTimeSec;
	pPlayerInfo->nConnPlayTimeSec = m_CharaInfoData.nConnPlayTimeSec;
	pPlayerInfo->nElapsedDeadTimeSec = m_CharaInfoData.nElapsedDeadTimeSec;

	pPlayerInfo->nPlayerGrade = (GPlayerGrade)m_CharaInfoData.nPlayerGrade;

	wcsncpy_s(pPlayerInfo->szName, m_CharaInfoData.szName, _TRUNCATE);

	// TP
	GPlayerTalent& PlayerTalent = pPlayer->GetTalent();
	PlayerTalent.SetTP(m_CharaInfoData.nRemaindTP);

	// Palette
	GPalette& Palette = pPlayer->GetPalette();
	Palette.SelectPalette((PALETTE_NUM)m_CharaInfoData.ActivePaletteID);
	Palette.SetPrimary((PALETTE_NUM)m_CharaInfoData.PrimaryPaletteID);
	Palette.SetSecondary((PALETTE_NUM)m_CharaInfoData.SecondaryPaletteID);

	// Party and Arena
	pPlayer->SetParty(m_CharaInfoData.uidParty);	
	if (gmgr.pBattleArenaMgr->FindBattleArena(m_CharaInfoData.uidArena))
	{
		pPlayer->GetPlayerBattleArena().SetBattleArenaUID(m_CharaInfoData.uidArena);
	}

	m_CharaInfoData.uidArenaTeam;	// 설정 안함


	// 편지함 속성
	pPlayer->GetMailbox().GetDBMailUID().Init(m_CharaInfoData.nMailTopUID, m_CharaInfoData.nMailBottomUID);
}

void GPlayerGameData::UnpackCharaInfo_Post(GEntityPlayer* pPlayer)
{
	PLAYER_INFO* pPlayerInfo = pPlayer->GetPlayerInfo();

	// 계산값 설정
	pPlayerInfo->nSTR		= PLAYER_DEFAULT_STR;
	pPlayerInfo->nDEX		= PLAYER_DEFAULT_DEX;
	pPlayerInfo->nINT		= PLAYER_DEFAULT_INT;
	pPlayerInfo->nCHA		= PLAYER_DEFAULT_CHA;
	pPlayerInfo->nCON		= PLAYER_DEFAULT_CON;

	pPlayer->ApplyAllPassiveTalents();

	pPlayerInfo->nMaxHP		= CSBasicAttributeCalculator::CalcMaxHP(pPlayerInfo->nLevel, pPlayer->GetCON());
	pPlayerInfo->nMaxEN		= CSBasicAttributeCalculator::CalcMaxEN(pPlayerInfo->nLevel, pPlayer->GetINT());
	pPlayerInfo->nMaxSTA	= CSBasicAttributeCalculator::CalcMaxSP(pPlayerInfo->nLevel, pPlayer->GetDEX());

	// CharacterStatus
	pPlayer->SetHP(m_CharaInfoData.nHP);
	pPlayer->SetEN(m_CharaInfoData.nEN);
	pPlayer->SetSTA(m_CharaInfoData.nSTA);

	if (pPlayer->IsDie())
	{
		pPlayer->SetDie();
	}

	// WeaponSet
	SH_ITEM_SWITCH_WEAPON nWeaponSet = pPlayerInfo->m_nWeaponSet;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	if (nWeaponSet == ITEMSWITCH_SECONDARY)
	{
		if (pItemHolder->GetEquipment().IsEquipItem(ITEMSLOT_RWEAPON2) == false &&
			pItemHolder->GetEquipment().IsEquipItem(ITEMSLOT_LWEAPON2) == false)
		{
			nWeaponSet = ITEMSWITCH_PRIMARY;
		}
	}
	pItemHolder->GetEquipment().SwitchingWeaponSet(nWeaponSet);
}

void GPlayerGameData::UnpackItemList(GEntityPlayer* pPlayer)
{
	wstring strExpiDt;
	for(vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>::iterator it = m_vecItemInstanceData.begin();
		it != m_vecItemInstanceData.end();
		it++)
	{
		GItem* pItem = gmgr.pItemManager->MakeNewItem(*it);		
		if (pItem == NULL)	continue;

		pPlayer->GetItemHolder()->PushItem((SH_ITEM_SLOT_TYPE)it->nSlotType, it->nSlotID, pItem);
	}
}

void GPlayerGameData::UnpackTalent(GEntityPlayer* pPlayer)
{
	GPlayerTalent& PlayerTalent = pPlayer->GetTalent();

	for each(int nTalentID in m_vecTalentData)
	{
		PlayerTalent.Insert(nTalentID, false);
	}
}

void GPlayerGameData::UnpackPaletteList(GEntityPlayer* pPlayer)
{
	for(vector<TD_PLAYER_GAME_DATA_PALETTE>::iterator it = m_vecPaletteData.begin();
		it != m_vecPaletteData.end();
		it++)
	{
		PALETTE_NUM		nNum;
		PALETTE_SLOT	nSlot;

		gsys.pPaletteSystem->IndexToNumAndSlot(it->nOrderID, nNum, nSlot);

		GPalette& Palette = pPlayer->GetPalette();
		Palette.Set(nNum, nSlot, (PALETTE_ITEM_TYPE)it->nType, it->nID);
	}
}

void GPlayerGameData::UnpackTalentRemainTime(GEntityPlayer* pPlayer)
{
	// 탤런트 쿨타임
	pPlayer->GetActorCooltimeChecker().InsertTalentRemainCoolTime(m_vecTalentCollTimeSec);
	

	// 버프 남은 시간
	vector<REMAIN_BUFF_TIME> vecRemainBuffTime;
	for(vector<TD_REMAIN_BUFF_TIME>::iterator it = m_vecBuffRemaindSec.begin(); it != m_vecBuffRemaindSec.end(); it++)
	{
		REMAIN_BUFF_TIME remainBuffTime;

		remainBuffTime.nID = it->nID;
		remainBuffTime.fRemainDurationSeconds = it->fRemainDurationSeconds;
		remainBuffTime.fRemainNextPeriodSeconds = it->fRemainNextPeriodSeconds;
		remainBuffTime.fRemainBuffOffsetSeconds = it->fRemainBuffOffsetSeconds;
		remainBuffTime.nStackedCount = it->nStackedCount;

		vecRemainBuffTime.push_back(remainBuffTime);
	}
	
	pPlayer->InsertBuffRemainTime(vecRemainBuffTime);
}

void GPlayerGameData::UnpackQuestInstance(GEntityPlayer* pPlayer)
{
	GPlayerQuests& PlayerQuests = pPlayer->GetQuests();	


	for(vector<TD_PLAYER_GAME_DATA_QUEST_INSTANCE>::iterator it = m_vecQuestInstanceData.begin();
		it != m_vecQuestInstanceData.end();
		it++)
	{	
		GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(it->nQID);
		if (pQuestInfo == NULL) continue;

		GPlayerQuest* pNewPlayerQuest = new GPlayerQuest(pQuestInfo, pPlayer, it->nSlotID, it->nAcceptTime, it->szAcceptDt, it->szExpiDt);

		pNewPlayerQuest->UpdateQuestVar(it->nVar);
		
		if (it->nDone == QST_COMPLETE)
		{
			pNewPlayerQuest->Complete();
		}
		
		if (it->nDone == QST_FAIL)
		{
			pNewPlayerQuest->Fail();
		}
		

		pair<MAP_PLAYER_QUEST::iterator, bool> ret;
		ret = PlayerQuests.GetQuestContainer().insert(MAP_PLAYER_QUEST::value_type(pQuestInfo->nID, pNewPlayerQuest));
		_ASSERT(ret.second);
		if (!ret.second)
		{
			delete pNewPlayerQuest;
			continue;
		}


		GPlayerQObjective* pPlayerQObjective = pNewPlayerQuest->Get(1);
		if (NULL != pPlayerQObjective) 
		{
			pPlayerQObjective->UpdateProgress(it->nObj1Progress);
		}

		pPlayerQObjective = pNewPlayerQuest->Get(2);
		if (NULL != pPlayerQObjective) 
		{
			pPlayerQObjective->UpdateProgress(it->nObj2Progress);
		}

		pPlayerQObjective = pNewPlayerQuest->Get(3);
		if (NULL != pPlayerQObjective) 
		{
			pPlayerQObjective->UpdateProgress(it->nObj3Progress);
		}

		pPlayerQObjective = pNewPlayerQuest->Get(4);
		if (NULL != pPlayerQObjective) 
		{
			pPlayerQObjective->UpdateProgress(it->nObj4Progress);
		}

		pPlayerQObjective = pNewPlayerQuest->Get(5);
		if (NULL != pPlayerQObjective) 
		{
			pPlayerQObjective->UpdateProgress(it->nObj5Progress);
		}

		PlayerQuests.SlotMarking(pNewPlayerQuest->GetSlotID());
	}	


	for(vector<TD_PLAYER_GAME_DATA_QUEST_DONE_COUNT>::iterator it = m_vecQuestDoneCount.begin();
		it != m_vecQuestDoneCount.end();
		it++)
	{
		if (!gmgr.pQuestInfoMgr->IsExist(it->nQID))	continue;

		PlayerQuests.GetDoneCountContainer().insert(MAP_DONE_COUNT::value_type(it->nQID, it->nDoneCount));
	}
}

void GPlayerGameData::UnpackFaction(GEntityPlayer* pPlayer)
{
	for(vector<TD_PLAYER_GAME_DATA_FACTION>::iterator it = m_vecFactionData.begin();	
		it != m_vecFactionData.end();
		it++)
	{
		GFactionInfo* pFactionInfo = gmgr.pFactionInfoMgr->Get(it->nFactionID);
		if (NULL == pFactionInfo) continue;

		GPlayerFactions& PlayerFactions = pPlayer->GetPlayerFactions();
		PlayerFactions.Insert(pFactionInfo, it->nFaction);
	}
}

void GPlayerGameData::UnpackSawnCutscene(GEntityPlayer* pPlayer)
{
	GPlayerCutscene& PlayerCutscene = pPlayer->GetCutscene();

	PlayerCutscene.AssignSawnCutscene(m_vecSawnCutscene);
}

void GPlayerGameData::UnpackRecipe(GEntityPlayer* pPlayer)
{
	for (size_t i = 0; i < m_vecRecipe.size(); ++i)
	{
		pPlayer->GetPlayerRecipe().Insert(m_vecRecipe[i]);
	}
}

const TD_GATE_INFO* GPlayerGameData::GetGateInfo(void) const
{
	return &m_GateInfo;
}

const TD_DYNAMIC_FIELD_ENTER_POS_INFO* GPlayerGameData::GetDynamicFieldEnterPosInfo(void) const
{
	return &m_DynamicFieldEnterPosInfo;
}

const TD_MOVE_PLAYER_INFO* GPlayerGameData::GetMovePlayerInfo(void) const
{
	return &m_PlayerInfoData;
}

const TD_PLAYER_GAME_DATA_CHARAINFO* GPlayerGameData::GetCharaInfo(void) const
{
	return &m_CharaInfoData;
}

const vector<TD_PLAYER_GAME_DATA_EQUIP>& GPlayerGameData::GetEquip(void) const
{
	return m_vecEquipData;
}

const vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE>& GPlayerGameData::GetItemInstance(void) const
{
	return m_vecItemInstanceData;
}

const vector<int>& GPlayerGameData::GetTalent(void) const
{
	return m_vecTalentData;
}

const vector<TD_PLAYER_GAME_DATA_PALETTE>& GPlayerGameData::GetPalette(void) const
{
	return m_vecPaletteData;
}

const vector<pair<int, float> >& GPlayerGameData::GetTalentCollTime(void) const
{
	return m_vecTalentCollTimeSec;
}

const vector<TD_REMAIN_BUFF_TIME>& GPlayerGameData::GetBuffRemain(void) const
{
	return m_vecBuffRemaindSec;
}

const vector<TD_PLAYER_GAME_DATA_QUEST_INSTANCE>& GPlayerGameData::GetQuestInstance(void) const
{
	return m_vecQuestInstanceData;
}

const vector<TD_PLAYER_GAME_DATA_QUEST_DONE_COUNT>& GPlayerGameData::GetQuestInstanceDontDoing(void) const
{
	return m_vecQuestDoneCount;
}

const vector<TD_PLAYER_GAME_DATA_FACTION>& GPlayerGameData::GetFactoin(void) const
{
	return m_vecFactionData;
}

const vector<int>& GPlayerGameData::GetSawnCutscene(void) const
{
	return m_vecSawnCutscene;
}

const vector<int>& GPlayerGameData::GetRecipe(void) const
{
	return m_vecRecipe;
}