#include "StdAfx.h"
#include "GLootBPart.h"
#include "GPlayerDoing.h"
#include "CCommandResultTable.h"
#include "GNPCMgr.h"
#include "GInteractionSystem.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GDropItem.h"
#include "GDropItems.h"
#include "GLootInfo.h"
#include "GGlobal.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GLootSystem.h"
#include "GLootLooter.h"
#include "GLootInfoMgr.h"

GLootBPart::GLootBPart(void)
{
}

GLootBPart::~GLootBPart(void)
{
}

bool GLootBPart::IsBPartLoot( MUID uidNPC )
{
	GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(uidNPC);
	VALID_RET(pNPC, false);

	return pNPC->IsBPart();
}

void GLootBPart::TakeAll( GEntityPlayer* pPlayer, MUID uidNPC )
{
	VALID(pPlayer);

	GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(uidNPC);
	VALID(pNPC);

	if (!pNPC->GetNPCLoot().GetDropList().IsAuthorizedCID(pPlayer->GetCID()))
		return;

	GLootInfo* pLootInfo = 
		gmgr.pLootInfoMgr->Get(pNPC->GetNPCLoot().GetBPartLootID());
	
	if (!pLootInfo)
	{
		pLootInfo = pNPC->GetNPCInfo()->m_pLootInfo;
	}

	if (!pLootInfo)
	{
		pNPC->DeleteMe();
		return;
	}

	if (!pPlayer->GetDoing().IsNowLooting()) 
	{
		pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_NOT_LOOTING);	
		return;
	}

	if (pPlayer->GetPos().XYDistanceTo(pNPC->GetPos()) > GAME_INTERACTION_BPART_LOOTING_DISTANCE + GAME_INTERACTION_SERVER_TOLER_DISTANCE_XY)
	{
		pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_TARGET_DISTANCE);	
		return;
	}

	// 아이템 얻기
	vector<GDropItem*> vecDropItems = 
		pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(pPlayer->GetCID());
	gsys.pLootSystem->GetLooter().GetItem(pPlayer, pNPC, vecDropItems);
	
	// 버프 얻기
	if (!pLootInfo->m_vecLootBuffs.empty())
	{
		for each (int nBuffID in pLootInfo->m_vecLootBuffs)
		{
			pPlayer->GainBuff(nBuffID);
		}
	}
	
	// 시체 소멸
	pNPC->DeleteMe();
	//pNPC->DespawnNow(false);
}
