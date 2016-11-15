#include "StdAfx.h"
#include "GItemDurabilityUpdater.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GCalculator.h"
#include "GEntityActor.h"
#include "GItemHolder.h"
#include "CCommandResultTable.h"
#include "GDBTaskItemData.h"
#include "GConfig.h"
#include "GPlayerPVPArea.h"
#include "GPlayerObjectManager.h"
#include "GEquipmentSlot.h"


bool GItemDurabilityUpdater::LostDurabilityByCombat(GEntityActor* pAttackerActor, GEntityActor* pDefenderActor, bool bCritical, bool bTryGuard)
{
	if (NULL == pAttackerActor) return false;
	if (NULL == pDefenderActor) return false;
	if (GConfig::IsExpoMode()) return false;

	int nLevelFactor = GCalculator::CalcLevelFactor(pAttackerActor->GetChrInfo()->nLevel, pDefenderActor->GetChrInfo()->nLevel);

	if (true == pAttackerActor->IsPlayer())
	{
		GEntityPlayer* pAttackerPlayer = ToEntityPlayer(pAttackerActor);
		LostWeaponDurabilityByCombat(pAttackerPlayer, nLevelFactor);
	}

	if (true == pDefenderActor->IsPlayer())
	{
		GEntityPlayer* pDefenderPlayer = ToEntityPlayer(pDefenderActor);
		LostEquipmentDurabilityByCombat(pDefenderPlayer, nLevelFactor, bCritical, bTryGuard);
	}

	return true;
}

bool GItemDurabilityUpdater::LostWeaponDurabilityByCombat(GEntityPlayer* pAttacker, int nLevelFactor)
{
	if (NULL == pAttacker) return false;
	if (pAttacker->IsGod()) return false;

	GItemHolder* pItemHolder = pAttacker->GetItemHolder();	
	GItem* pWeaponItem = pItemHolder->GetEquipment().GetRightWeapon();	
	if (NULL == pWeaponItem) return true;

	if (0 >= pWeaponItem->m_nDurability)
	{
		pAttacker->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_EMPTY_DURABILITY);
		return true;
	}

	float fLostRate = min(MAX_DURABILITY_LOST_RATE_WEAPON, (nLevelFactor * 0.2f));
	if (fLostRate < RandomNumber(0.0f, 100.0f)) return true;

	if (false == Update(pAttacker, pWeaponItem, pWeaponItem->m_nDurability - 1)) return false;

	return true;
}

bool GItemDurabilityUpdater::LostEquipmentDurabilityByCombat(GEntityPlayer* pDefender, int nLevelFactor, bool bCritical, bool bTryGuard)
{
	if (NULL == pDefender) return false;

	GItem* pEquipmentItem = NULL;
	GItemHolder* pItemHolder = pDefender->GetItemHolder();

	if (true == bTryGuard)
	{
		pEquipmentItem = pItemHolder->GetEquipment().GetShield();
		if (NULL == pEquipmentItem)
		{
			// 무기로 방어했을때는 무기 내구도를 깍는다.
			pEquipmentItem = pItemHolder->GetEquipment().GetRightWeapon();
		}
	}
	else
	{
		vector<GItem*> vecEquipmentItem;
		pItemHolder->GetEquipment().CollectArmorSlotItem(vecEquipmentItem);
		if (true == vecEquipmentItem.empty()) return true;

		pEquipmentItem = vecEquipmentItem[RandomNumber(0, static_cast<int>(vecEquipmentItem.size() - 1))];
	}

	if (NULL == pEquipmentItem) return true;
	if (0 >= pEquipmentItem->m_nDurability)
	{
		pDefender->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_EMPTY_DURABILITY);
		return true;
	}

	const float fMaxLostRate = (true == bCritical) ? MAX_DURABILITY_LOST_RATE_EQUIPMENT_CRI : MAX_DURABILITY_LOST_RATE_EQUIPMENT;
	float fLostRate = min(fMaxLostRate, (max(0, nLevelFactor - 6) * 0.5f + 0.5f));
	if (fLostRate < RandomNumber(0.0f, 100.0f)) return true;

	if (false == Update(pDefender, pEquipmentItem, pEquipmentItem->m_nDurability - 1)) return false;

	return true;
}

bool GItemDurabilityUpdater::LostDurabilityByDie(GEntityPlayer* pVictim)
{
	if (NULL == pVictim) return false;
	if (pVictim->IsGod()) return false;
	if (GConfig::IsExpoMode()) return false;
	if (pVictim->GetPlayerPVPArea().IsLocateAndBelongTeam())
	{
		if (true == gmgr.pPlayerObjectManager->IsExist(pVictim->GetKiller()))
		{
			return false;
		}
	}

	vector<GItem*> vecItem;
	GItemHolder* pItemHolder = pVictim->GetItemHolder();
	pItemHolder->CollectAllItem(SLOTTYPE_EQUIP, vecItem);

	for each (GItem* pItem in vecItem)
	{
		if (NULL == pItem) continue;
		if (0 >= pItem->m_nDurability) continue;

		int nLostDurability = min(pItem->m_nDurability, GMath::TruncateToInt(pItem->m_pItemData->m_nMaxDurability * 0.10f));
		if (false == Update(pVictim, pItem, pItem->m_nDurability - nLostDurability)) continue;
	}	

	return true;
}

bool GItemDurabilityUpdater::Update(GEntityPlayer* pPlayer, GItem* pItem, int nDurability)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	vector<GItem*> vecItem;
	vecItem.push_back(pItem);
	vector<int> vecDurability;
	vecDurability.push_back(nDurability);

	if (false == Update(pPlayer, vecItem, vecDurability)) return false;
	
	return true;
}

bool GItemDurabilityUpdater::Update(GEntityPlayer* pPlayer, const vector<GItem*>& vecItem, const vector<int>& vecDurability)
{
	if (NULL == pPlayer) return false;

	if (false == Check(pPlayer, vecItem, vecDurability)) return false;
	if (false == ApplyDB(pPlayer, vecItem, vecDurability)) return false;
	//if (false == ApplyServer(vecItem, vecDurability)) return false;

	//Route(pPlayer, vecItem);

	return true;
}

bool GItemDurabilityUpdater::Check(GEntityPlayer* pPlayer, const vector<GItem*>& vecItem, const vector<int>& vecDurability)
{
	if (NULL == pPlayer) return false;
	if (vecItem.size() != vecDurability.size()) return false;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();

	int i = 0;
	for each (GItem* pItem in vecItem)
	{
		int nDurability = vecDurability[i];
		i++;

		if (nDurability < 0) return false;
		if (pItem->m_pItemData->m_nMaxDurability <= 0) return false;		
		if (pItem->m_pItemData->m_nMaxDurability < nDurability) return false;;		
	}

	return true;
}

bool GItemDurabilityUpdater::ApplyDB(GEntityPlayer* pPlayer, const vector<GItem*>& vecItem, const vector<int>& vecDurability)
{
	if (vecItem.size() != vecDurability.size()) return false;

	int nSize = vecItem.size();

	for (int i = 0; i < nSize; i++)
	{
		GItem* pItem = vecItem[i];		
		int nDurability = vecDurability[i];
		if (NULL == pItem) continue;

		GDBT_ITEM_DEC_DURA_DATA data(pPlayer->GetUID()
			, pPlayer->GetCID()
			, pItem->m_nSlotType
			, pItem->m_nSlotID
			, pItem->m_nIUID
			, nDurability
			, pItem->m_nDurability - nDurability
			, pItem->GetAmount());

		if (pItem->m_nDurability > nDurability)
		{
			if (!gsys.pDBManager->ItemDescDurability(data))
				return false;
		}
		else
		{
			_ASSERT(0 && "need to create increase durability task.");
		}	
	}

	return true;
}

bool GItemDurabilityUpdater::ApplyServer(const vector<GItem*>& vecItem, const vector<int>& vecDurability)
{
	if (vecItem.size() != vecDurability.size()) return false;

	int nSize = vecItem.size();
	
	for (int i = 0; i < nSize; i++)
	{
		GItem* pItem = vecItem[i];		
		int nDurability = vecDurability[i];
		if (NULL == pItem) continue;

		pItem->m_nDurability = nDurability;
	}	

	return true;
}

void GItemDurabilityUpdater::Route(GEntityPlayer* pPlayer, const vector<GItem*>& vecItem)
{
	if (NULL == pPlayer) return;

	vector<TD_UPDATE_DURABILITY> vecTDUpdateDurability;
	for each (GItem* pItem in vecItem)
	{
		if (NULL == pItem) continue;

		TD_UPDATE_DURABILITY tdUpdateDurability = pItem->ToTD_UPDATE_DURABILITY();
		vecTDUpdateDurability.push_back(tdUpdateDurability);
	}	

	MCommand* pNewCmd = MakeNewCommand(MC_ITEM_UPDATE_DURABILITY, 
		1, 
		NEW_BLOB(vecTDUpdateDurability));

	pPlayer->RouteToMe(pNewCmd);
}

bool GItemDurabilityUpdater::ApplyServerAndRoute(GEntityPlayer* pPlayer, const vector<GItem*>& vecItem, const vector<int>& vecDurability)
{
	if (NULL == pPlayer) return false;	

	if (false == ApplyServer(vecItem, vecDurability)) return false;

	Route(pPlayer, vecItem);

	return true;
}