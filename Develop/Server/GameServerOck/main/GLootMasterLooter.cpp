#include "StdAfx.h"
#include "GLootMasterLooter.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "CCommandResultTable.h"
#include "GGlobal.h"
#include "GNPCMgr.h"
#include "GDropItem.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GDropItems.h"
#include "AStlUtil.h"
#include "GLootSystem.h"
#include "GLootItemGetter.h"
#include "GLootModifyHandler.h"

bool GLootMasterLooter::MasterLootItem(GEntityPlayer* pPlayer, const MUID& uidNPC, const MUID& nDropItemUID, uint8 nSelectedIndex, bool bRandom)
{
	VALID_RET(pPlayer, false);

	GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(uidNPC);
	VALID_RET(pNPC, pPlayer->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_INVALID_NPC_UID));
	
	GDropItem* pDropItem = pNPC->GetNPCLoot().GetDropList().GetDropItems().GetDropItem(nDropItemUID);
	VALID_RET(pDropItem, pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_NOT_EXIST_DROPITEM));

	if (false == Check(pPlayer, pNPC, pDropItem, nSelectedIndex, bRandom)) return false;
	Apply(pNPC, pDropItem, nSelectedIndex, bRandom);

	return true;
}

bool GLootMasterLooter::Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC, GDropItem* pDropItem, uint8 nSelectedIndex, bool bRandom)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);
	VALID_RET(pDropItem, false);

	if (false == pDropItem->IsMasterLootable()) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_NOT_MASTERLOOTABLE_ITEM);
	if (pPlayer->GetCID() != pNPC->GetNPCLoot().GetDropList().GetMasterCID()) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_MASTERLOOT_POSSIBLE_ONLY_MASTER);

	if (false == bRandom)
	{
		const set<int>& setBeneficiaryCID = pNPC->GetNPCLoot().GetDropList().GetBeneficiaryCID();
		if (true == setBeneficiaryCID.empty()) return false;

		vector<int> vecBeneficiaryCID = SetToVector(setBeneficiaryCID);

		if (static_cast<int>(vecBeneficiaryCID.size()) <= nSelectedIndex)
			return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_NOT_MASTERLOOTABLE_PLAYER);

		if (false == pNPC->GetNPCLoot().GetDropList().IsBeneficiaryCID(vecBeneficiaryCID[nSelectedIndex]))
			return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_NOT_MASTERLOOTABLE_PLAYER);
	}

	return true;
}

void GLootMasterLooter::Apply(GEntityNPC* pNPC, GDropItem* pDropItem, uint8 nSelectedIndex, bool bRandom)
{
	VALID(pNPC);
	VALID(pDropItem);

	vector<GDropItem*> vecDropItem;
	vecDropItem.push_back(pDropItem);

	set<int> setPreViewableCID;
	pNPC->GetNPCLoot().GetDropList().GetViewableCID(setPreViewableCID);

	int nCID = SelectAssignCID(pNPC, nSelectedIndex, bRandom);
	gsys.pLootSystem->GetItemGetter().GetItem(nCID, pNPC, vecDropItem);		

	gsys.pLootSystem->GetModifyHandler().HandleDropListModification(pNPC, setPreViewableCID);
}

int GLootMasterLooter::SelectAssignCID(GEntityNPC* pNPC, uint8 nSelectedIndex, bool bRandom)
{
	VALID_RET(pNPC, false);

	const set<int>& setContriubtorCID = pNPC->GetNPCLoot().GetDropList().GetBeneficiaryCID();
	vector<int> vecBeneficiaryCID = SetToVector(setContriubtorCID);
	if (true == vecBeneficiaryCID.empty()) return 0;

	if (true == bRandom)
	{
		nSelectedIndex = RandomNumber(0, vecBeneficiaryCID.size() - 1);
	}

	return vecBeneficiaryCID[nSelectedIndex];
}