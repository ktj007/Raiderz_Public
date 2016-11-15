#include "StdAfx.h"
#include "GDynamicFieldFactory.h"
#include "SDef.h"
#include "GEntityPlayer.h"
#include "GInnRoomInfo.h"
#include "GInnRoomInfoMgr.h"
#include "GGlobal.h"
#include "GFieldMgr.h"
#include "GBattleArena.h"
#include "GBattleArenaMgr.h"
#include "GPlayerBattleArena.h"



GDynamicFieldMaster* GDynamicFieldFactory::CreateDynamicField(DYNAMIC_FIELD_TYPE eType, GEntityPlayer* pPlayer, int nFieldGroupID)
{
	GDynamicFieldMaster* pFieldGroup = NULL;
	switch(eType)
	{
	case DFT_INNROOM:		pFieldGroup = CreateInnRoom(pPlayer);						break;
	case DFT_BATTLEARENA:	pFieldGroup = CreateBattleArena(pPlayer, nFieldGroupID);	break;
	case DFT_TRIAL:			pFieldGroup = CreateTrialField(pPlayer, nFieldGroupID);		break;
	}
	return pFieldGroup;
}


GDynamicFieldMaster* GDynamicFieldFactory::CreateInnRoom(GEntityPlayer* pPlayer)
{
	GInnRoomInfo* pInnRoomInfo = gmgr.pInnRoomInfoMgr->Get(pPlayer->GetPlayerInfo()->nInnRoomID);
	if (!pInnRoomInfo)
		return NULL;

	int nFieldGroupID = pInnRoomInfo->m_nFieldGroupID;

	GDynamicFieldMaster* pFieldGroup = pFieldGroup = gmgr.pFieldMgr->CreateDynamicFieldMaster(nFieldGroupID, DFT_INNROOM, pPlayer->GetUID());
	VALID_RET(pFieldGroup, NULL);
	if (NULL == pFieldGroup)
	{
		dlog("GDynamicFieldFactory::CreateInnRoom() - failed create field group (id: %d)\n", nFieldGroupID);
		return NULL;
	}

	return pFieldGroup;
}

GDynamicFieldMaster* GDynamicFieldFactory::CreateBattleArena(GEntityPlayer* pPlayer, int nFieldGroupID)
{
	MUID uidBattleArena = pPlayer->GetPlayerBattleArena().GetBattleArenaUID();

	GBattleArena* pBattleArena = gmgr.pBattleArenaMgr->FindBattleArena(uidBattleArena);
	if (NULL == pBattleArena)
		return NULL;

	if (GBattleArena::COUNTING != pBattleArena->GetStatus())
		return NULL;

	GDynamicFieldMaster* pDynamicFieldGroup = gmgr.pFieldMgr->CreateDynamicFieldMaster(nFieldGroupID, DFT_BATTLEARENA, pPlayer->GetUID());
	VALID_RET(pDynamicFieldGroup, NULL);
	if (NULL == pDynamicFieldGroup)
	{
		dlog("GDynamicFieldFactory::CreateBattleArena() - failed create field group (id: %d)\n", nFieldGroupID);
		return NULL;
	}

	return pDynamicFieldGroup;
}

GDynamicFieldMaster* GDynamicFieldFactory::CreateTrialField(GEntityPlayer* pPlayer, int nFieldGroupID)
{
	GDynamicFieldMaster* pFieldGroup = pFieldGroup = gmgr.pFieldMgr->CreateDynamicFieldMaster(nFieldGroupID, DFT_TRIAL, pPlayer->GetUID());
	VALID_RET(pFieldGroup, NULL);
	if (NULL == pFieldGroup)
	{
		dlog("GDynamicFieldFactory::CreateTrialField() - failed create field group (id: %d)\n", nFieldGroupID);
		return NULL;
	}

	return pFieldGroup;
}
