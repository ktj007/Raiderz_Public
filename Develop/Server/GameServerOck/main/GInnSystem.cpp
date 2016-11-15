#include "StdAfx.h"
#include "GInnSystem.h"
#include "GEntityPlayer.h"
#include "CCommandResultTable.h"
#include "CCommandTable.h"
#include "GInteractionSystem.h"
#include "GFatigueSystem.h"
#include "GInnRoomInfoMgr.h"
#include "GInnRoomInfo.h"
#include "GPlayerInteraction.h"
#include "GDynamicField.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GServer.h"
#include "GCommand.h"
#include "GNPCInteractor.h"
#include "GItemHolder.h"
#include "GDBTaskCharData.h"
#include "GGuildMgr.h"


GInnSystem::GInnSystem()
{
}

GInnSystem::~GInnSystem()
{
}

bool GInnSystem::BeginSleep(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return false;

	pPlayer->GetInteraction().BeginSleep(SLEEP_ANI_TIME);

	MCommand* pNewCmd = MakeNewCommand(MC_INN_BEGIN_SLEEP, 0, NULL);
	pPlayer->RouteToMe(pNewCmd);

	return true;
}

bool GInnSystem::EndSleep(GEntityPlayer* pPlayer)
{
	if (NULL == pPlayer) return false;

	GInnRoomInfo* pInnRoomInfo = gmgr.pInnRoomInfoMgr->Get(pPlayer->GetPlayerInfo()->nInnRoomID);
	if (NULL == pInnRoomInfo) return NULL;
	
	gsys.pFatigueSystem->SetFatigueBySleep(pPlayer, pInnRoomInfo->m_nType);	

	MCommand* pNewCmd = MakeNewCommand(MC_INN_END_SLEEP, 0, NULL);
	pPlayer->RouteToMe(pNewCmd);

	return true;
}

void GInnSystem::SpawnInnRoomNPC(GEntityPlayer* pCreator, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField)
{
	switch (pInnRoomInfo->m_nType)
	{
	case IRT_NORMAL:
		{
			SpawnBedNPC(pCreator, pInnRoomInfo, pField);
			SpawnMuralNPC(pCreator, pInnRoomInfo, pField);
			SpawnStorageNPC(pCreator, pInnRoomInfo, pField);
			SpawnGuildStorageNPC(pCreator, pInnRoomInfo, pField);
		}
		break;
	case IRT_BASIC:
	case IRT_STORAGE:
		{
			SpawnCraftingNPC(pCreator, pInnRoomInfo, pField);
			SpawnBedNPC(pCreator, pInnRoomInfo, pField);
			SpawnCarpetNPC(pCreator, pInnRoomInfo, pField);
			SpawnFirePlaceNPC(pCreator, pInnRoomInfo, pField);
			SpawnMuralNPC(pCreator, pInnRoomInfo, pField);
			SpawnTableNPC(pCreator, pInnRoomInfo, pField);
			SpawnFunitureNPC(pCreator, pInnRoomInfo, pField);
			SpawnStorageNPC(pCreator, pInnRoomInfo, pField);
			SpawnGuildStorageNPC(pCreator, pInnRoomInfo, pField);
		}
		break;
	case IRT_STEWARD:
	case IRT_PREMIUM:
		{
			SpawnCraftingNPC(pCreator, pInnRoomInfo, pField);
			SpawnBedNPC(pCreator, pInnRoomInfo, pField);
			SpawnCarpetNPC(pCreator, pInnRoomInfo, pField);
			SpawnFirePlaceNPC(pCreator, pInnRoomInfo, pField);
			SpawnMuralNPC(pCreator, pInnRoomInfo, pField);
			SpawnTableNPC(pCreator, pInnRoomInfo, pField);
			SpawnFunitureNPC(pCreator, pInnRoomInfo, pField);
			SpawnStewardNPC(pCreator, pInnRoomInfo, pField);
			SpawnStorageNPC(pCreator, pInnRoomInfo, pField);
			SpawnGuildStorageNPC(pCreator, pInnRoomInfo, pField);
		}
		break;
	default:
		_ASSERT(0);
	}
}

void GInnSystem::SpawnCraftingNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField)
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	const GItem* pItem = pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_INN_CRAFTING);
	if (NULL == pItem) return;

	pField->SpawnNPC(pItem->m_pItemData->m_nHousingNPC, pInnRoomInfo->m_nCraftingMarkerID);
}

void GInnSystem::SpawnBedNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField)
{
	int nNPCID = 0;
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	const GItem* pItem = pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_INN_BED);	
	if (NULL == pItem)
	{
		nNPCID = pInnRoomInfo->m_nDefaultBedNPCID;
	}
	else
	{
		nNPCID = pItem->m_pItemData->m_nHousingNPC;
	}
	pField->SpawnNPC(nNPCID, pInnRoomInfo->m_nBedMarkerID);
}

void GInnSystem::SpawnStorageNPC( GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField )
{
	pField->SpawnNPC(pInnRoomInfo->m_nDefaultStorageNPCID , pInnRoomInfo->m_nStorageMarkerID);
}

void GInnSystem::SpawnGuildStorageNPC( GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField )
{
	if (false == gmgr.pGuildMgr->IsExist(pPlayer->GetGID())) return;

	pField->SpawnNPC(pInnRoomInfo->m_nDefaultGuildStorageNPCID , pInnRoomInfo->m_nGuildStorageMarkerID);
}

void GInnSystem::SpawnCarpetNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField)
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	const GItem* pItem = pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_INN_CARPET);
	if (NULL == pItem) return;

	pField->SpawnNPC(pItem->m_pItemData->m_nHousingNPC, pInnRoomInfo->m_nCarpetMarkerID);
}

void GInnSystem::SpawnFirePlaceNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField)
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	const GItem* pItem = pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_INN_FIREPLACE);
	if (NULL == pItem) return;

	pField->SpawnNPC(pItem->m_pItemData->m_nHousingNPC, pInnRoomInfo->m_nFirePlaceMarkerID);
}

void GInnSystem::SpawnMuralNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField)
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	const GItem* pItem = pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_INN_MURAL);
	if (NULL == pItem) return;

	pField->SpawnNPC(pItem->m_pItemData->m_nHousingNPC, pInnRoomInfo->m_nMuralMarkerID);
}

void GInnSystem::SpawnTableNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField)
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	const GItem* pItem = pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_INN_TABLE);
	if (NULL == pItem) return;

	pField->SpawnNPC(pItem->m_pItemData->m_nHousingNPC, pInnRoomInfo->m_nTableMarkerID);
}

void GInnSystem::SpawnFunitureNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField)
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	const GItem* pItem = pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_INN_FUNITURE);
	if (NULL == pItem) return;

	pField->SpawnNPC(pItem->m_pItemData->m_nHousingNPC, pInnRoomInfo->m_nFunitureMarkerID);
}

void GInnSystem::SpawnStewardNPC(GEntityPlayer* pPlayer, GInnRoomInfo* pInnRoomInfo, GDynamicField* pField)
{
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	const GItem* pItem = pItemHolder->GetItem(SLOTTYPE_EQUIP, ITEMSLOT_INN_STEWARD);
	if (NULL == pItem) return;

	pField->SpawnNPC(pItem->m_pItemData->m_nHousingNPC, pInnRoomInfo->m_nStewardMarkerID);
}

void GInnSystem::SetInnRoom(GEntityPlayer* pPlayer, int nID)
{
	pPlayer->GetPlayerInfo()->nInnRoomID = nID;

	GDBT_CHAR_REGIST_INN_ROOM data(pPlayer->GetAID()
		, pPlayer->GetUID()
		, pPlayer->GetCID()
		, nID
		, pPlayer->GetPlayerInfo()->nPlayTimeSec);

	gsys.pDBManager->CharRegistInnRoom(data);
}