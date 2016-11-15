#include "StdAfx.h"
#include "GLootEnder.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GEntityPlayer.h"
#include "GPlayerLoot.h"
#include "GEntityNPC.h"
#include "GNPCLoot.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GLootSystem.h"
#include "GDropList.h"
#include "GLootModifyHandler.h"
#include "GDropItems.h"

bool GLootEnder::End(GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer, false);

	GEntityNPC* pNPC = pPlayer->GetPlayerLoot().GetLootingNPC();
	if (NULL != pNPC)
	{
		pNPC->GetNPCLoot().End(pPlayer->GetCID());		
		RemoveGarbageDropItem(pPlayer, pNPC);
	}

	pPlayer->GetPlayerLoot().EndLoot();

	MCommand* pNewCmd = MakeNewCommand(MC_LOOT_END, 1, NEW_UID(pPlayer->GetUID()));

	pPlayer->RouteToThisCell(pNewCmd);

	return true;
}

bool GLootEnder::End(GEntityNPC* pNPC)
{
	VALID_RET(pNPC, false);

	set<int> setLootingPlayerCID = pNPC->GetNPCLoot().GetLootingPlayerCID();
	for each (int nLootingPlayerCID in setLootingPlayerCID)
	{
		GEntityPlayer* pLootingPlayer = gmgr.pPlayerObjectManager->GetEntity(nLootingPlayerCID);
		if (NULL == pLootingPlayer) continue;

		End(pLootingPlayer);
	}

	return true;
}

// 인터랙션 루팅은 NPC를 죽여서 드랍시키지 않기 때문에 아이템을 먹지않고 루팅을 종료하면 NPC가 영원이 디스폰되지 않을수 있다.
// 그래서 루팅 종료시에 아이템을 제거시킨다.
void GLootEnder::RemoveGarbageDropItem(GEntityPlayer* pPlayer, GEntityNPC* pNPC)
{
	if (NULL == pPlayer) return;
	if (NULL == pNPC) return;

	if (pNPC->GetNPCLoot().GetDropList().IsDropByInteract_Mortal() || 
		pNPC->GetNPCLoot().GetDropList().IsDropByInteract_Immortal())
	{
		set<int> setPreViewableCID;
		pNPC->GetNPCLoot().GetDropList().GetDropItems().Clear();

		if (!pPlayer->GetPlayerLoot().IsILootRegulatorActive())
		{
			gsys.pLootSystem->GetModifyHandler().HandleDropListModification(pNPC, setPreViewableCID);
		}
	}
}
