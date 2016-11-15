#include "StdAfx.h"
#include "GExpSystem.h"
#include "GNPCInfo.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GExpCalculator.h"
#include "GGlobal.h"
#include "GFatigueSystem.h"
#include "GPlayerLevelUpdater.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GDBTaskCharData.h"
#include "GDBManager.h"
#include "GPlayerLevelUpdater.h"
#include "GTimeCalculator.h"
#include "GDB_CODE.h"
#include "GConfig.h"


bool GExpSystem::RewardExpByNPCKill(const vector<GEntityPlayer*>& vecSameServerBeneficiary, int nAllBeneficiaryCount, GNPCInfo* pNPCInfo)
{
	if (NULL == pNPCInfo) return false;
	if (false == pNPCInfo->bGainEXP) return false;
	if (true == vecSameServerBeneficiary.empty()) return false;

	int nSameServerBeneficiaryCount = static_cast<int>(vecSameServerBeneficiary.size());
	if (0 >= nSameServerBeneficiaryCount) return false;

	int nSumLevel = CalculSumLevel(vecSameServerBeneficiary);

	for each (GEntityPlayer* pSameServerBeneficiary in vecSameServerBeneficiary)
	{
		//SoulHunterZ - Give XP only for killing BAMs (We give to parties, later we give to everyone for World BAMs)
		int nNPCExp = GExpCalculator::CalcXPFromNPC(pSameServerBeneficiary->GetLevel(), pNPCInfo->nLevel, pNPCInfo->nGrade);
		float fLevelNormalizeFactor = GExpCalculator::CalcLevelNormalizeFactor(pSameServerBeneficiary->GetLevel(), nSumLevel, nSameServerBeneficiaryCount, nAllBeneficiaryCount);

		float fContriubtorCountFactor = GExpCalculator::CalcBeneficiaryCountFactor(nAllBeneficiaryCount);		
		float fFatigueFactor = GExpCalculator::CalcFatigueFactor(pSameServerBeneficiary->GetPlayerInfo()->nFatigueQuantity);
		//if(pNPCInfo->nGrade >= 3)//Only BAMs give us normal XP
		AddExp(pSameServerBeneficiary, static_cast<int>(nNPCExp * fLevelNormalizeFactor * fContriubtorCountFactor * fFatigueFactor * GConfig::m_KillXPRate), pNPCInfo->nID);
		//AddItemExp(pSameServerBeneficiary, static_cast<int>(nNPCExp * fLevelNormalizeFactor * fContriubtorCountFactor * fFatigueFactor * GConfig::m_KillXPRate), pNPCInfo->nID);
	}

	return true;
}

int GExpSystem::CalculSumLevel(const vector<GEntityPlayer*>& vecSameServerBeneficiary)
{
	int nSumLevel = 0;
	for each(GEntityPlayer* pSameServerBeneficiary in vecSameServerBeneficiary)
	{
		if (NULL == pSameServerBeneficiary) continue;

		nSumLevel += pSameServerBeneficiary->GetLevel();
	}

	return nSumLevel;
}

void GExpSystem::AddExp(GEntityPlayer* pPlayer, int nEXP, int nNpcID)
{
	if (NULL == pPlayer) return;
	
	int nNewLevel = GExpCalculator::CalcNewLevel(pPlayer->GetPlayerInfo()->nLevel, pPlayer->GetPlayerInfo()->nXP + nEXP);
	if (nNewLevel < pPlayer->GetLevel()) return;
	
	if (nNewLevel == pPlayer->GetLevel())
	{
		GDBT_CHAR_XP_DATA data(pPlayer->GetAID()
			, pPlayer->GetUID()
			, pPlayer->GetCID()
			, GDB_CODE::CD_L_KILL_NPC_GAIN_XP
			, GTimeCalculator::GetTimeAsString(GTimeSec(0))
			, pPlayer->GetPlayerInfo()->nXP + nEXP
			, nEXP
			, pPlayer->GetLevel()
			, pPlayer->GetMoney()
			, pPlayer->GetPlayerInfo()->nPlayTimeSec
			, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
			, nNpcID);

		gsys.pDBManager->CharAddXP(data);	
	}
	else
	{
		GPlayerLevelUpdater levelUpdater(pPlayer);
		levelUpdater.LevelUp(nEXP, nNpcID);		
	}
}

void GExpSystem::AddItemExp(GEntityPlayer* pPlayer, int nEXP, int nNpcID)
{
	//SoulHunterZ - Item XP
	vector <GItem*> vecItem;
	pPlayer->GetItemHolder()->CollectAllItem(SLOTTYPE_EQUIP, vecItem);

	for(vector<GItem*>::iterator itr = vecItem.begin(); itr != vecItem.end(); itr++)
	{
		//int nextAttuneXP = 2000*(pPlayer->GetLevel()*pPlayer->GetLevel())+(1000*(GMath::Pow((*itr)->m_nAttuneLvl, (*itr)->m_nAttuneLvl)));
		if((*itr)->m_nNextAttuneXP == 0 || (*itr)->m_nNextAttuneXP == -1) //Initial
		{
			int nextAttuneXP = 2000*(pPlayer->GetLevel()*pPlayer->GetLevel())+(1000*(GMath::Pow(2, (((*itr)->m_nAttuneLvl+1)+((*itr)->m_nAttuneLvl+1)))));
			GDBT_ITEM_XP_DATA itemData(pPlayer->GetAID()
				, pPlayer->GetUID()
				, (*itr)->m_nIUID
				, pPlayer->GetCID()
				, GTimeCalculator::GetTimeAsString(GTimeSec(0))
				, (*itr)->m_nXP + nEXP
				, nextAttuneXP
				, nEXP
				, nNpcID);
			gsys.pDBManager->ItemAddXP(itemData);
		}
		else if((*itr)->m_nNextAttuneXP > (*itr)->m_nXP + nEXP) //Normal Case
		{
			GDBT_ITEM_XP_DATA itemData(pPlayer->GetAID()
				, pPlayer->GetUID()
				, (*itr)->m_nIUID
				, pPlayer->GetCID()
				, GTimeCalculator::GetTimeAsString(GTimeSec(0))
				, (*itr)->m_nXP + nEXP
				, (*itr)->m_nNextAttuneXP
				, nEXP
				, nNpcID);
			gsys.pDBManager->ItemAddXP(itemData);
		}
		else if((*itr)->m_nNextAttuneXP < (*itr)->m_nXP + nEXP) //Hit New Attunement
		{
			int diff = (*itr)->m_nNextAttuneXP - (*itr)->m_nXP;
			GDBT_ITEM_XP_DATA itemData(pPlayer->GetAID()
				, pPlayer->GetUID()
				, (*itr)->m_nIUID
				, pPlayer->GetCID()
				, GTimeCalculator::GetTimeAsString(GTimeSec(0))
				, (*itr)->m_nNextAttuneXP
				, (*itr)->m_nNextAttuneXP
				, diff
				, nNpcID);
			gsys.pDBManager->ItemAddXP(itemData);
		}
	}
}

void GExpSystem::AddExpForDB( GEntityPlayer* pPlayer, int nEXP )
{
	if (NULL == pPlayer) return;

	pPlayer->GetPlayerInfo()->nXP += nEXP;	
	RouteAddExp(pPlayer, nEXP);
}

void GExpSystem::AddItemExpForDB( GEntityPlayer* pPlayer, int nEXP, int nNextAttuneXP )
{
	if (NULL == pPlayer) return;
	vector <GItem*> vecItem;
	pPlayer->GetItemHolder()->CollectAllItem(SLOTTYPE_EQUIP, vecItem);
	for(vector<GItem*>::iterator itr = vecItem.begin(); itr != vecItem.end(); itr++)
	{
		((*itr))->m_nXP += nEXP;
		((*itr))->m_nNextAttuneXP = nNextAttuneXP;
	}
	RouteItemAddExp(pPlayer, nEXP, nNextAttuneXP);
}

void GExpSystem::RouteAddExp( GEntityPlayer* pPlayer, int nEXP )
{
	if (NULL == pPlayer) return;

	float fExpPercent = GExpCalculator::CalcExpPercent(pPlayer->GetPlayerInfo()->nXP, pPlayer->GetLevel());

	MCommand* pNewCommand = MakeNewCommand(MC_CHAR_REWARD_EXP,
		3, 
		NEW_INT(nEXP), 
		NEW_FLOAT(fExpPercent), 
		NEW_INT(pPlayer->GetPlayerInfo()->nXP));

	pPlayer->RouteToMe(pNewCommand);
}

void GExpSystem::RouteItemAddExp(GEntityPlayer *pPlayer, int nEXP, int nNextAttuneXP)
{
	float fExpPercent = GExpCalculator::CalcExpPercent(pPlayer->GetPlayerInfo()->nXP, pPlayer->GetLevel());

	//SoulHunterZ - Item XP Routing
	vector <GItem*> vecItem;
	pPlayer->GetItemHolder()->CollectAllItem(SLOTTYPE_EQUIP, vecItem);
	for(vector<GItem*>::iterator itr = vecItem.begin(); itr != vecItem.end(); itr++)
	{
		MCommand* pItemXPCommand = MakeNewCommand(MC_SH_ITEM_XP, 
			4,
			NEW_INT((*itr)->m_nSlotID),
			NEW_INT(nEXP), 
			NEW_INT(nNextAttuneXP),
			NEW_INT((*itr)->m_nXP));
		pPlayer->RouteToMe(pItemXPCommand);
	}
}