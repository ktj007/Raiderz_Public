#include "StdAfx.h"
#include "GDropItemAuthorizer.h"
#include "GItemData.h"
#include "GGlobal.h"
#include "GItemManager.h"
#include "GDropItem.h"
#include "GParty.h"
#include "GPartySystem.h"

void GDropItemAuthorizer::AuthorizeNormalItem(vector<GDropItem*>& outvecDropItem, const vector<int>& vecBeneficiaryCID, MUID nPartyUID, const LOOTING_RULE_DATA& lootingRuleData)
{
	for each (GDropItem* pDropItem in outvecDropItem)
	{
		GItemData* pItemData = gmgr.pItemManager->GetItemData(pDropItem->GetID());
		if (NULL == pItemData) continue;

		if (false == IsRareItem(pDropItem->GetID(), lootingRuleData.m_nLRRF))
		{
			switch (lootingRuleData.m_nLRC)
			{
			case LRC_FREE_FOR_ALL:
				{
					Authorize(pDropItem, vecBeneficiaryCID, DIT_GETTABLE);
				}
				break;
			case LRC_ROUND_ROBIN:
				{
					int nCurrentOrderCID = GetRoundRobinCurrentOdrer(vecBeneficiaryCID, nPartyUID);
					SetRoundRobinLastOrder(nPartyUID, nCurrentOrderCID);

					Authorize(pDropItem, nCurrentOrderCID, DIT_GETTABLE);
				}
				break;
			case LRC_MASTER_LOOT:
				{
					Authorize(pDropItem, vecBeneficiaryCID, DIT_MASTERLOOTABLE);
				}
				break;
			}

		}
		else
		{
			switch (lootingRuleData.m_nLRR)
			{
			case LRR_FREE_FOR_ALL:
				{
					Authorize(pDropItem, vecBeneficiaryCID, DIT_GETTABLE);
				}
				break;
			case LRR_ROLL_DICE:
				{
					Authorize(pDropItem, vecBeneficiaryCID, DIT_ROLLABLE);				
				}
				break;
			case LRR_MASTER_LOOT:
				{
					Authorize(pDropItem, vecBeneficiaryCID, DIT_MASTERLOOTABLE);
				}
				break;
			}	
		}
	}
}

void GDropItemAuthorizer::AuthorizeQuestItem(vector<GDropItem*>& outvecDropItem, int nCID)
{
	for each (GDropItem* pDropItem in outvecDropItem)
	{
		Authorize(pDropItem, nCID, DIT_GETTABLE);
	}
}

void GDropItemAuthorizer::AuthorizeQuestPVPItem(vector<GDropItem*>& outvecDropItem, int nCID)
{
	for each (GDropItem* pDropItem in outvecDropItem)
	{
		Authorize(pDropItem, nCID, DIT_GETTABLE);
	}
}

void GDropItemAuthorizer::AuthorizeInteractItem(vector<GDropItem*>& outvecDropItem, int nCID )
{
	for each (GDropItem* pDropItem in outvecDropItem)
	{
		Authorize(pDropItem, nCID, DIT_GETTABLE);
	}
}

void GDropItemAuthorizer::Authorize(GDropItem* pDropItem, const vector<int>& vecBeneficiaryCID, DROP_ITEM_TYPE nDIT)
{
	for each (int nBeneficiaryCID in vecBeneficiaryCID)
	{
		Authorize(pDropItem, nBeneficiaryCID, nDIT);
	}
}

void GDropItemAuthorizer::Authorize(GDropItem* pDropItem, int nCurrentOrderCID, DROP_ITEM_TYPE nDIT)
{
	pDropItem->InsertAuthorizedCID(nCurrentOrderCID);
	pDropItem->SetType(nDIT);
}

int GDropItemAuthorizer::GetRoundRobinCurrentOdrer(const vector<int>& vecBeneficiaryCID, MUID nPartyUID)
{
	if (true == vecBeneficiaryCID.empty()) return 0;

	int nLastOrderCID = 0;
	GParty* pParty = gsys.pPartySystem->FindParty(nPartyUID);
	if (NULL != pParty)
	{
		nLastOrderCID = pParty->GetRoundRobinLastOrderCID();		
	}

	int nSize = vecBeneficiaryCID.size();
	int nCurrentOrder = vecBeneficiaryCID.front();

	for (int i = 0; i < nSize; i++)
	{
		int nCID = vecBeneficiaryCID[i];

		if (nLastOrderCID == nCID)
		{
			if (i+1 < nSize)
			{
				nCurrentOrder = vecBeneficiaryCID[i+1];
			}
			break;
		}
	}

	return nCurrentOrder;
}

void GDropItemAuthorizer::SetRoundRobinLastOrder(MUID nPartyUID, int nLastOrderCID)
{
	GParty* pParty = gsys.pPartySystem->FindParty(nPartyUID);
	if (NULL != pParty)
	{
		pParty->SetRoundRobinLastOrderCID(nLastOrderCID);
	}
}

bool GDropItemAuthorizer::IsRareItem(int nItemID, LOOTING_RULE_RARE_FOR nLRRF)
{
	GItemData* pItemData = gmgr.pItemManager->GetItemData(nItemID);
	if (NULL == pItemData) return false;

	switch(nLRRF)
	{
	case LRRF_RARE_OR_HIGHER:		return ITEM_TIER_RARE		<= pItemData->m_nTier;
	case LRRF_LEGENDARY_OR_HIGHER:	return ITEM_TIER_LEGENDARY	<= pItemData->m_nTier;	
	case LRRF_EPIC:					return ITEM_TIER_EPIC		== pItemData->m_nTier;
	}

	return false;
}