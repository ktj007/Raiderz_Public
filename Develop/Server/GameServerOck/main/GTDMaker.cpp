#include "StdAfx.h"
#include "GTDMaker.h"
#include "GPlayerObject.h"
#include "GCalculator.h"
#include "GExpCalculator.h"
#include "GPlayerTalent.h"
#include "GQuestSystem.h"
#include "GGuild.h"
#include "GGuildMgr.h"
#include "GPalette.h"
#include "GPaletteSystem.h"
#include "GPaletteItem.h"
#include "GPlayerFaction.h"
#include "GPlayerFactions.h"
#include "GActorCooltimeChecker.h"
#include "GNPCIconSelector.h"
#include "GItemHolder.h"
#include "GGlobal.h"
#include "GFatigueSystem.h"
#include "GSoulBindingInfo.h"
#include "GSoulBindingInfoMgr.h"
#include "GPlayerFieldDynamic.h"
#include "GPlayerSystem.h"
#include "GPlayerQuests.h"
#include "GPlayerQuest.h"
#include "GQuestInfo.h"
#include "GPlayerQObjective.h"
#include "GQObjectiveInfo.h"
#include "GPlayerMailBox.h"
#include "GEntityNPC.h"
#include "GEquipmentSlot.h"


GTDMaker::GTDMaker()
{
}

GTDMaker::~GTDMaker()
{
}

void GTDMaker::MakeTD_BUFF_REMAIN_TIME(GEntityPlayer* pPlayer, vector<TD_BUFF_REMAIN_TIME>& outvecTDBuffRemainTime)
{
	vector<REMAIN_BUFF_TIME> vecBuffRemainTime;
	pPlayer->GetBuffRemainTime(vecBuffRemainTime);

	for(vector<REMAIN_BUFF_TIME>::iterator it = vecBuffRemainTime.begin(); it != vecBuffRemainTime.end(); it++)
	{
		TD_BUFF_REMAIN_TIME tdBuffRemainTime;
		
		tdBuffRemainTime.nBuffID = it->nID;
		tdBuffRemainTime.fRemainTime = it->fRemainDurationSeconds;

		outvecTDBuffRemainTime.push_back(tdBuffRemainTime);
	}
}

void GTDMaker::MakeTD_TALENT_REMAIN_COOLTIME(GEntityPlayer* pPlayer, vector<TD_TALENT_REMAIN_COOLTIME>& outvecTDTalentRemainCoolTime)
{
	vector<pair<int,float>> vecTalentCoolTime;
	pPlayer->GetActorCooltimeChecker().GetTalentRemainCoolTime(vecTalentCoolTime);

	for each (const pair<int,float>& pair in vecTalentCoolTime)
	{
		TD_TALENT_REMAIN_COOLTIME tdTalentRemainCoolTime;
		tdTalentRemainCoolTime.nTalentID = pair.first;
		tdTalentRemainCoolTime.fRemainCoolTime = pair.second;

		outvecTDTalentRemainCoolTime.push_back(tdTalentRemainCoolTime);
	}
}

void GTDMaker::MakeTD_MyInfo(GPlayerObject* pPlayer, TD_MYINFO& out)
{
	GEntityPlayer* pEntityPlayer = pPlayer->GetEntity();
	PLAYER_INFO* pSrcPlayerInfo = pEntityPlayer->GetPlayerInfo();
	GItemHolder* pItemHolder = pEntityPlayer->GetItemHolder();

	wcsncpy_s(out.szName, pSrcPlayerInfo->szName, _TRUNCATE);
	wcsncpy_s(out.szSurname, pSrcPlayerInfo->szSurname, _TRUNCATE);

	out.nSex = pSrcPlayerInfo->nSex;
	out.nFeatureHair = pSrcPlayerInfo->nFeatureHair;
	out.nFeatureFace = pSrcPlayerInfo->nFeatureFace;
	out.nFeatureHairColor = pSrcPlayerInfo->nFeatureHairColor;
	out.nFeatureSkinColor = pSrcPlayerInfo->nFeatureSkinColor;
	out.nLevel = pSrcPlayerInfo->nLevel;
	out.fExpPercent = GExpCalculator::CalcExpPercent(pSrcPlayerInfo->nXP, pSrcPlayerInfo->nLevel);
	out.nMaxHP = pEntityPlayer->GetPlayerInfo()->nMaxHP;
	out.nMaxEN = pEntityPlayer->GetPlayerInfo()->nMaxEN;
	out.nMaxSTA = pEntityPlayer->GetPlayerInfo()->nMaxSTA;
	out.nCurHP = pEntityPlayer->GetHP();
	out.nCurEN = pEntityPlayer->GetEN();
	out.nCurSTA = pEntityPlayer->GetSTA();
	out.nMoney = pSrcPlayerInfo->GetMoney();
	out.nXP = pSrcPlayerInfo->nXP;
	out.nSTR = pEntityPlayer->GetSTRProto();
	out.nDEX = pEntityPlayer->GetDEXProto();
	out.nINT = pEntityPlayer->GetINTProto();
	out.nCHA = pEntityPlayer->GetCHAProto();
	out.nCON = pEntityPlayer->GetCONProto();
	out.nWeaponSet = pItemHolder->GetEquipment().GetWeaponSet();

	out.nRemainTP = pEntityPlayer->GetTalent().GetTP();	
	out.nFatigueType = gsys.pFatigueSystem->GetFatigueType(pEntityPlayer->GetPlayerInfo()->nFatigueQuantity);
	out.nPlayerGrade = (int8)pEntityPlayer->GetPlayerGrade();

	out.nSoulbindingFieldID = gsys.pPlayerSystem->GetSoulBindingFieldID(pEntityPlayer);;
	out.nCheckPointFieldID = pEntityPlayer->GetPlayerField().GetFieldDynamic().GetCheckpoint().nFieldID;

	out.bExistUnreadMail = pEntityPlayer->GetMailbox().IsExistUnreadMail();
	out.bIsFullMailbox = pEntityPlayer->GetMailbox().IsFullMailbox();
}


void GTDMaker::MakeTD_ItemFromInventory(GEntityPlayer* pPlayer, vector<TD_ITEM>& outvecTDItem)
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();	
	
	if (0 < pItemHolder->GetItemCount(SLOTTYPE_INVENTORY))
	{
		pItemHolder->MakeTD(SLOTTYPE_INVENTORY, outvecTDItem);
	}
}

void GTDMaker::MakeTD_ItemFromEquipment(GEntityPlayer* pPlayer, vector<TD_ITEM>& outvecTDItem)
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();

	if (0 < pItemHolder->GetItemCount(SLOTTYPE_EQUIP))
	{
		pItemHolder->MakeTD(SLOTTYPE_EQUIP, outvecTDItem);
	}
}

void GTDMaker::MakeTD_ItemFromStorage( GEntityPlayer* pPlayer, vector<TD_ITEM>& outvecTDItem )
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();

	if (0 < pItemHolder->GetItemCount(SLOTTYPE_STORAGE))
	{
		pItemHolder->MakeTD(SLOTTYPE_STORAGE, outvecTDItem);
	}
}

void GTDMaker::MakeTD_Faction(GEntityPlayer* pPlayer, vector<TD_FACTION>& outvecTDFaction)
{
	vector<GPlayerFaction*> vecPlayerFaction;
	pPlayer->GetPlayerFactions().GetAll(vecPlayerFaction);

	for each (GPlayerFaction* pPlayerFaction in vecPlayerFaction)
	{
		TD_FACTION tdFaction(pPlayerFaction->GetID(), pPlayerFaction->GetQuantity());
		outvecTDFaction.push_back(tdFaction);
	}
}

vector<TD_NPC_ICON> GTDMaker::MakeTD_NPCIcon(const vector<NPC_ICON>& vecNPCICon)
{
	vector<TD_NPC_ICON> vecTDNPCIcon;

	for each (NPC_ICON npcIcon in vecNPCICon)
	{
		vecTDNPCIcon.push_back(TD_NPC_ICON(npcIcon.m_pNPC->GetUIID(), npcIcon.m_nIcon));
	}

	return vecTDNPCIcon;
}

vector<TD_ESSENTIAL_NPC_ICON> GTDMaker::MakeTD_EssentialNPCIcon( const vector<NPC_ICON>& vecNPCICon )
{
	vector<TD_ESSENTIAL_NPC_ICON> vecTDNPCIcon;

	for each (NPC_ICON npcIcon in vecNPCICon)
	{
		vecTDNPCIcon.push_back(TD_ESSENTIAL_NPC_ICON(npcIcon.m_pNPC->GetUIID(), npcIcon.m_nIcon, npcIcon.m_pNPC->GetID(), npcIcon.m_pNPC->GetPos()));
	}

	return vecTDNPCIcon;
}


void GTDMaker::MakeTD_ReservedDummySlot(GEntityPlayer* pPlayer, vector<TD_ITEM_SLOT_INFO>& outvecTDSlotInfo)
{
	vector<ITEM_SLOT_INFO> vecSlotInfo;
	pPlayer->GetItemHolder()->CollectReservedDummySlotInfo(vecSlotInfo);

	for each (const vector<ITEM_SLOT_INFO>::value_type& data in vecSlotInfo)
	{
		TD_ITEM_SLOT_INFO tdItemSlotInfo;
		tdItemSlotInfo.nSlotType = data.nSlotType;
		tdItemSlotInfo.nSlotID = data.nSlotID;
		outvecTDSlotInfo.push_back(tdItemSlotInfo);
	}
}

void GTDMaker::MakeTD_PLAYERQUEST(GEntityPlayer* pEntityPlayer, vector<TD_PLAYERQUEST>& outvecTDPlayerQuest)
{
	GPlayerQuests& playerQuests = pEntityPlayer->GetQuests();
	MAP_PLAYER_QUEST& mapPlayerQuest = playerQuests.GetQuestContainer();
	for each (pair<int, GPlayerQuest*> pairPlayerQuest in mapPlayerQuest)
	{
		GPlayerQuest* pPlayerQuest = pairPlayerQuest.second;
		if (NULL == pPlayerQuest) continue;

		TD_PLAYERQUEST tdPlayerQuest;
		tdPlayerQuest.nQuestID = pPlayerQuest->GetInfo()->nID;
		tdPlayerQuest.nState = pPlayerQuest->GetState();
		tdPlayerQuest.nAcceptTime = pPlayerQuest->GetAcceptTime();
		tdPlayerQuest.nLimitTime = pPlayerQuest->GetLimitTime();
		tdPlayerQuest.bChallengerQuest = pPlayerQuest->GetInfo()->bChallengerQuest;
		tdPlayerQuest.vRewardPos = pPlayerQuest->GetInfo()->vRewardPos;

		size_t i = 0;
		for each (pair<int, GPlayerQObjective*> pairPlayerQObjective in pPlayerQuest->GetQObjectiveContainer())
		{
			GPlayerQObjective* pPlayerQObjective = pairPlayerQObjective.second;
			if (NULL == pPlayerQObjective) continue;

			tdPlayerQuest.playerQObjectives[i].nQObjectiveID = pPlayerQObjective->GetInfo()->nID;
			tdPlayerQuest.playerQObjectives[i].nQOT = pPlayerQObjective->GetInfo()->nType;
			tdPlayerQuest.playerQObjectives[i].nProgress = pPlayerQObjective->GetProgress();
			tdPlayerQuest.playerQObjectives[i].bComplete = pPlayerQObjective->IsComplete();

			i++;
			if (MAX_PLAYERQOBJECTIVE < i) break;
		}

		outvecTDPlayerQuest.push_back(tdPlayerQuest);
	}
}