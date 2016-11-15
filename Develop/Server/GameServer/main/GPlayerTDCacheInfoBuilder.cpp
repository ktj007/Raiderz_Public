#include "stdafx.h"
#include "GPlayerTDCacheInfoBuilder.h"
#include "GEntityPlayer.h"
#include "GPlayerDoing.h"
#include "GGlobal.h"
#include "GGuildMgr.h"
#include "GPlayerPVPArea.h"
#include "GItemHolder.h"
#include "CSDef_QuestPVP.h"
#include "GPlayerAFK.h"
#include "GEquipmentSlot.h"
#include "GPlayerSit.h"
#include "GPlayerStance.h"

void GPlayerTDCacheInfoBuilder::Build( GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& cache, TD_PLAYER_FEATURE_TATTOO& TattooCache )
{
	PLAYER_INFO* pPlayerInfo = pEntityPlayer->m_pPlayerInfo;	

	BuildBaseInfo(pEntityPlayer, cache);
	BuildFeature(pEntityPlayer, cache, TattooCache);
	BuildEquipment(pEntityPlayer, cache);
	BuildStatusFlag(pEntityPlayer, cache);
}

void GPlayerTDCacheInfoBuilder::BuildBaseInfo( GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& cache )
{
	PLAYER_INFO* pPlayerInfo = pEntityPlayer->m_pPlayerInfo;

	cache.uid = pEntityPlayer->m_UID;
	cache.nUIID = pEntityPlayer->m_nUIID;
	cache.vPos = pEntityPlayer->m_vPos;
	cache.svDir = pEntityPlayer->m_vDir;	
	cache.nStance = pEntityPlayer->GetStance().Get();
	memset(cache.szName, 0, sizeof(cache.szName));
	wcsncpy_s(cache.szName, pPlayerInfo->szName, _TRUNCATE);		
	memset(cache.Buffs, 0, sizeof(cache.Buffs));
	pEntityPlayer->GetBuffList(cache.Buffs);

	cache.nMF = pEntityPlayer->m_MotionFactor.GetCurrMF();
	cache.nMFWeight = (uint16)pEntityPlayer->m_MotionFactor.GetCurrWeight();
}

void GPlayerTDCacheInfoBuilder::BuildFeature( GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& cache, TD_PLAYER_FEATURE_TATTOO& TattooCache )
{
	PLAYER_INFO* pPlayerInfo = pEntityPlayer->m_pPlayerInfo;
	GItemHolder* pItemHolder = pEntityPlayer->GetItemHolder();

	// feature	
	cache.Feature.nSex = pPlayerInfo->nSex;
	cache.Feature.nHair = (int8)pPlayerInfo->nFeatureHair;
	cache.Feature.nFace = (int8)pPlayerInfo->nFeatureFace;
	cache.Feature.nHairColor = pPlayerInfo->nFeatureHairColor;
	cache.Feature.nSkinColor = pPlayerInfo->nFeatureSkinColor;
	cache.Feature.nEyeColor = pPlayerInfo->nEyeColor;
	cache.Feature.nMakeUp = pPlayerInfo->nMakeUp;

	if (0 != pPlayerInfo->nTattooType)
	{
		TattooCache.nUIID = pEntityPlayer->GetUIID();
		TattooCache.nTattooType = pPlayerInfo->nTattooType;
		TattooCache.nTattooPosX = pPlayerInfo->nTattooPosX;
		TattooCache.nTattooPosY = pPlayerInfo->nTattooPosY;
		TattooCache.nTattooScale = pPlayerInfo->nTattooScale;
	}

	cache.Feature.nWeaponSet = pItemHolder->GetEquipment().GetWeaponSet();
	memset(cache.Feature.szGuildName, 0, sizeof(cache.Feature.szGuildName));	
	const wchar_t* szGuildName = gmgr.pGuildMgr->GetName(pPlayerInfo->nGID);
	if (NULL != szGuildName)
	{
		wcsncpy_s(cache.Feature.szGuildName, szGuildName, _TRUNCATE);
	}
}

void GPlayerTDCacheInfoBuilder::BuildEquipment( GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& cache )
{
	GItemHolder* pItemHolder = pEntityPlayer->GetItemHolder();

	static map<int,int> s_ItemLookSlotMapper;
	if (s_ItemLookSlotMapper.empty())
	{
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_HEAD, ITEMSLOT_LOOK_HEAD));
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_FACE, ITEMSLOT_NONE));
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_HANDS, ITEMSLOT_LOOK_HANDS));
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_FEET, ITEMSLOT_LOOK_FEET));
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_BODY, ITEMSLOT_LOOK_BODY));
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_LEG, ITEMSLOT_LOOK_LEG));
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_LFINGER, ITEMSLOT_NONE));
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_RFINGER, ITEMSLOT_NONE));
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_NECK, ITEMSLOT_NONE));
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_CHARM, ITEMSLOT_NONE));
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_LWEAPON, ITEMSLOT_NONE));
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_RWEAPON, ITEMSLOT_NONE));
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_LWEAPON2, ITEMSLOT_NONE));
		s_ItemLookSlotMapper.insert(make_pair(ITEMSLOT_RWEAPON2, ITEMSLOT_NONE));
	}
	
	static map<int,int> s_FeatureSlotMapper;
	if (s_FeatureSlotMapper.empty())
	{
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_HEAD, FEATURE_ITEMSLOT_HEAD));
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_FACE, FEATURE_ITEMSLOT_FACE));
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_HANDS, FEATURE_ITEMSLOT_HANDS));
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_FEET, FEATURE_ITEMSLOT_FEET));
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_BODY, FEATURE_ITEMSLOT_BODY));
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_LEG, FEATURE_ITEMSLOT_LEG));
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_LFINGER, FEATURE_ITEMSLOT_NONE));
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_RFINGER, FEATURE_ITEMSLOT_NONE));
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_NECK, FEATURE_ITEMSLOT_NONE));
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_CHARM, FEATURE_ITEMSLOT_NONE));
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_LWEAPON, FEATURE_ITEMSLOT_LWEAPON));
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_RWEAPON, FEATURE_ITEMSLOT_RWEAPON));
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_LWEAPON2, FEATURE_ITEMSLOT_LWEAPON2));
		s_FeatureSlotMapper.insert(make_pair(ITEMSLOT_RWEAPON2, FEATURE_ITEMSLOT_RWEAPON2));
	}

	for (int i=ITEMSLOT_DEFAULT_BEGIN; i<=ITEMSLOT_DEFAULT_MAX; ++i)
	{
		VALID(s_ItemLookSlotMapper.find(i) != s_ItemLookSlotMapper.end());

		map<int,int>::iterator itFeatureSlot = s_FeatureSlotMapper.find(i);
		if (itFeatureSlot == s_FeatureSlotMapper.end())
			continue; // 보여지지 않는 슬롯

		SH_FEATURE_ITEMSLOT nFeatureSlot = (SH_FEATURE_ITEMSLOT)itFeatureSlot->second;
		if (nFeatureSlot == FEATURE_ITEMSLOT_NONE)
			continue; // 보여지지 않는 슬롯

		SH_ITEM_SLOT nLookSlot = (SH_ITEM_SLOT)s_ItemLookSlotMapper[i];

		GItem* pItem = 
			pItemHolder->GetItem(SLOTTYPE_EQUIP, nLookSlot);

		if (!pItem) 
			pItem = pItemHolder->GetItem(SLOTTYPE_EQUIP, (SH_ITEM_SLOT)i);

		if (!pItem)
			continue; // 장비하지 않은 아이템

		cache.Feature.nItemID[nFeatureSlot] = pItem->m_pItemData->m_nID;
		cache.Feature.nItemID_DyedColor[nFeatureSlot] = pItem->m_nDyedColor;
		cache.Feature.nItemID_EnchantBuff[nFeatureSlot] = pItem->GetActiveEnchantBuffID();
	}
}

void GPlayerTDCacheInfoBuilder::BuildStatusFlag( GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& cache )
{
	cache.nStatusFlag = 0;

	if (pEntityPlayer->GetActionState() == AS_SWIMMING) 
	{
		SetBitSet(cache.nStatusFlag, UPS_SWIMMING);
	}

	if (pEntityPlayer->GetAFK().IsNowAFK())
	{
		SetBitSet(cache.nStatusFlag, UPS_AFK);
	}

	if (pEntityPlayer->IsPartyLeader())
	{
		SetBitSet(cache.nStatusFlag, UPS_PARTYLEADER);
	}

	if (true == pEntityPlayer->IsDead())
	{
		SetBitSet(cache.nStatusFlag, UPS_DEAD);
	}
	else
	{
		if (true == pEntityPlayer->GetDoing().IsNowLooting())
		{
			SetBitSet(cache.nStatusFlag, UPS_LOOTING);
		}
		else if (true == pEntityPlayer->GetSit().IsNowSit())
		{
			SetBitSet(cache.nStatusFlag, UPS_SITTING);
		}
		else if (true == pEntityPlayer->GetDoing().IsNowCutscening())
		{
			SetBitSet(cache.nStatusFlag, UPS_CUTSCENING);
		}
	}	

	// 필드 PVP 상태
	if (pEntityPlayer->GetPlayerPVPArea().IsLocatedPvPArea())
	{
		switch (pEntityPlayer->GetPlayerPVPArea().GetTeam())
		{
		case QT_ONE: { SetBitSet(cache.nStatusFlag, UPS_FIELDPVP_TEAM1); }break;
		case QT_TWO: { SetBitSet(cache.nStatusFlag, UPS_FIELDPVP_TEAM2); }break;
		}
	}
}