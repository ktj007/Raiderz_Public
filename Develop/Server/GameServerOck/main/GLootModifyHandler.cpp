#include "StdAfx.h"
#include "GLootModifyHandler.h"
#include "GEntityNPC.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GField.h"
#include "GGlobal.h"
#include "GLootSystem.h"
#include "GLootEnder.h"
#include "GNPCDecay.h"
#include "GDropItems.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"

void GLootModifyHandler::HandleDropListModification(GEntityNPC* pNPC, set<int> setPreViewableCID)
{
	VALID(pNPC);

	set<int> setPostViewableCID;
	pNPC->GetNPCLoot().GetDropList().GetViewableCID(setPostViewableCID);

	HandleUnviewablePlayer(setPreViewableCID, setPostViewableCID, pNPC);
	HandleViewablePlayer(setPreViewableCID, setPostViewableCID, pNPC);	
	RefreshItem(pNPC);
	HandleEmptyNPC(pNPC);
}

void GLootModifyHandler::HandleUnviewablePlayer(set<int> setPreViewableCID, set<int> setPostViewableCID, GEntityNPC* pNPC)
{
	for each (int nPreViewableCID in setPreViewableCID)
	{
		if (setPostViewableCID.end() == setPostViewableCID.find(nPreViewableCID))
		{
			int nUnviewableCID = nPreViewableCID;

			GEntityPlayer* pUnviewablePlayer = pNPC->GetField()->FindPlayerByCID(nUnviewableCID);
			if (NULL == pUnviewablePlayer) continue;

			RouteRemoveLootableNPC(pUnviewablePlayer, pNPC->GetUID());

			if (true == pNPC->GetNPCLoot().IsLootingPlayer(nUnviewableCID))
			{
				gsys.pLootSystem->GetEnder().End(pUnviewablePlayer);
			}
		}		
	}

	if (true == pNPC->GetNPCLoot().GetDropList().IsDropByInteract_Mortal() ||
		true == pNPC->GetNPCLoot().GetDropList().IsDropByInteract_Immortal())
	{
		if (true == pNPC->GetNPCLoot().GetDropList().IsEmpty())
		{
			set<int> setLootingPlayerCID = pNPC->GetNPCLoot().GetLootingPlayerCID();
			for each (int nLootingPlayerCID in setLootingPlayerCID)
			{
				GEntityPlayer* pLootingPlayer = pNPC->GetField()->FindPlayerByCID(nLootingPlayerCID);
				if (NULL == pLootingPlayer) continue;

				gsys.pLootSystem->GetEnder().End(pLootingPlayer);				
			}
		}
	}
}

void GLootModifyHandler::HandleViewablePlayer( set<int> setPreViewableCID, set<int> setPostViewableCID, GEntityNPC* pNPC )
{
	for each (int nPostVisibleCID in setPostViewableCID)
	{
		if (setPreViewableCID.end() == setPreViewableCID.find(nPostVisibleCID))
		{
			int nViewableCID = nPostVisibleCID;

			GEntityPlayer* pAuthorityGainPlayer = pNPC->GetField()->FindPlayerByCID(nViewableCID);
			if (NULL == pAuthorityGainPlayer) continue;

			RouteInsertLootableNPC(pAuthorityGainPlayer, pNPC->GetUID());
		}		
	}
}

void GLootModifyHandler::HandleEmptyNPC(GEntityNPC* pNPC)
{
	VALID(pNPC);
	if (false == pNPC->GetNPCLoot().GetDropList().IsEmpty()) return;

	pNPC->SendNPCIcon();
	
	if (true == pNPC->GetNPCLoot().GetDropList().IsDropByKill())
	{
		pNPC->GetNPCDecay().Decay();
	}
	else if (true == pNPC->GetNPCLoot().GetDropList().IsDropByGather())
	{
		pNPC->doDespawn(false);
	}
	else if (true == pNPC->GetNPCLoot().GetDropList().IsDropByInteract_Mortal())
	{
		pNPC->doDespawn(true);
	}
}

void GLootModifyHandler::RouteInsertLootableNPC(GEntityNPC* pNPC)
{
	VALID(pNPC);

	set<int> setVisibleCID;
	pNPC->GetNPCLoot().GetDropList().GetViewableCID(setVisibleCID);
	for each (int nViewableCID in setVisibleCID)
	{
		GEntityPlayer* pViewablePlayer = gmgr.pPlayerObjectManager->GetEntity(nViewableCID);
		if (NULL == pViewablePlayer) continue;

		gsys.pLootSystem->GetModifyHandler().RouteInsertLootableNPC(pViewablePlayer, pNPC->GetUID());		
	}
}

void GLootModifyHandler::RefreshItem( GEntityNPC* pNPC )
{
	VALID(pNPC);

	set<int> setLootingPlayerCID = pNPC->GetNPCLoot().GetLootingPlayerCID();
	for each (int nLootingPlayerCID in setLootingPlayerCID)
	{
		GEntityPlayer* pLootingPlayer = pNPC->GetField()->FindPlayerByCID(nLootingPlayerCID);
		if (NULL == pLootingPlayer) continue;

		vector<TD_ITEM_DROP> vecTDDropItem = pNPC->GetNPCLoot().GetDropList().GetDropItems().MakeVisbleTDDropItem(nLootingPlayerCID);
		RouteRefreshItem(pLootingPlayer, vecTDDropItem);
	}
}
void GLootModifyHandler::RouteRefreshItem( GEntityPlayer* pPlayer, const vector<TD_ITEM_DROP>& vecTDDropItem )
{
	VALID(pPlayer);

	MCommand* pNewCmd = MakeNewCommand(MC_LOOT_REFRESH_ITEM, 
		1, 
		NEW_BLOB(vecTDDropItem));
	pPlayer->RouteToMe(pNewCmd);
}

void GLootModifyHandler::RouteInsertLootableNPC(GEntityPlayer* pPlayer, const MUID& uidNPC)
{
	VALID(pPlayer);

	MCommand* pNewCmd = MakeNewCommand(MC_LOOT_INSERT_LOOTABLE_NPC, 
		1, 
		NEW_UID(uidNPC));
	pPlayer->RouteToMe(pNewCmd);
}

void GLootModifyHandler::RouteRemoveLootableNPC(GEntityPlayer* pPlayer, const MUID& uidNPC)
{
	VALID(pPlayer);

	MCommand* pNewCmd = MakeNewCommand(MC_LOOT_REMOVE_LOOTABLE_NPC, 
		1, 
		NEW_UID(uidNPC));
	pPlayer->RouteToMe(pNewCmd);
}