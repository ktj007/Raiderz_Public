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
		int nNPCExp = GExpCalculator::CalcXPFromNPC(pSameServerBeneficiary->GetLevel(), pNPCInfo->nLevel, pNPCInfo->nGrade);
		float fLevelNormalizeFactor = GExpCalculator::CalcLevelNormalizeFactor(pSameServerBeneficiary->GetLevel(), nSumLevel, nSameServerBeneficiaryCount, nAllBeneficiaryCount);

		float fContriubtorCountFactor = GExpCalculator::CalcBeneficiaryCountFactor(nAllBeneficiaryCount);		
		float fFatigueFactor = GExpCalculator::CalcFatigueFactor(pSameServerBeneficiary->GetPlayerInfo()->nFatigueQuantity);
		AddExp(pSameServerBeneficiary, static_cast<int>(nNPCExp * fLevelNormalizeFactor * fContriubtorCountFactor * fFatigueFactor), pNPCInfo->nID);
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

void GExpSystem::AddExpForDB( GEntityPlayer* pPlayer, int nEXP )
{
	if (NULL == pPlayer) return;

	pPlayer->GetPlayerInfo()->nXP += nEXP;	
	RouteAddExp(pPlayer, nEXP);
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