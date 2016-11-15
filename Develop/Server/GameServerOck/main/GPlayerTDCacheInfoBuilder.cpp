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

void GPlayerTDCacheInfoBuilder::Build( GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& out )
{
	PLAYER_INFO* pPlayerInfo = pEntityPlayer->m_pPlayerInfo;	

	BuildBaseInfo(pEntityPlayer, out);
	BuildFeature(pEntityPlayer, out);
	BuildEquipment(pEntityPlayer, out);
	BuildStatusFlag(pEntityPlayer, out);
}

void GPlayerTDCacheInfoBuilder::BuildBaseInfo( GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& out )
{
	PLAYER_INFO* pPlayerInfo = pEntityPlayer->m_pPlayerInfo;

	out.uid = pEntityPlayer->m_UID;
	out.nUIID = pEntityPlayer->m_nUIID;
	out.vPos = pEntityPlayer->m_vPos;
	out.svDir = pEntityPlayer->m_vDir;	
	out.nStance = pEntityPlayer->GetStance().Get();
	memset(out.szName, 0, sizeof(out.szName));
	wcsncpy_s(out.szName, pPlayerInfo->szName, _TRUNCATE);		
	memset(out.Buffs, 0, sizeof(out.Buffs));
	pEntityPlayer->GetBuffList(out.Buffs);

	out.nMF = pEntityPlayer->m_MotionFactor.GetCurrMF();
	out.nMFWeight = (uint16)pEntityPlayer->m_MotionFactor.GetCurrWeight();
}

void GPlayerTDCacheInfoBuilder::BuildFeature( GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& out )
{
	PLAYER_INFO* pPlayerInfo = pEntityPlayer->m_pPlayerInfo;
	GItemHolder* pItemHolder = pEntityPlayer->GetItemHolder();

	// feature	
	out.Feature.nSex = pPlayerInfo->nSex;
	out.Feature.nHair = (int8)pPlayerInfo->nFeatureHair;
	out.Feature.nFace = (int8)pPlayerInfo->nFeatureFace;
	out.Feature.nHairColor = pPlayerInfo->nFeatureHairColor;
	out.Feature.nSkinColor = pPlayerInfo->nFeatureSkinColor;
	out.Feature.nWeaponSet = pItemHolder->GetEquipment().GetWeaponSet();
	memset(out.Feature.szGuildName, 0, sizeof(out.Feature.szGuildName));	
	const wchar_t* szGuildName = gmgr.pGuildMgr->GetName(pPlayerInfo->nGID);
	if (NULL != szGuildName)
	{
		wcsncpy_s(out.Feature.szGuildName, szGuildName, _TRUNCATE);
	}
}

void GPlayerTDCacheInfoBuilder::BuildEquipment( GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& out )
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

		out.Feature.nItemID[nFeatureSlot] = pItem->m_pItemData->m_nID;
		out.Feature.nItemID_DyedColor[nFeatureSlot] = pItem->m_nDyedColor;
		out.Feature.nItemID_EnchantBuff[nFeatureSlot] = pItem->GetActiveEnchantBuffID();
	}
}

void GPlayerTDCacheInfoBuilder::BuildStatusFlag( GEntityPlayer* pEntityPlayer, TD_UPDATE_CACHE_PLAYER& out )
{
	out.nStatusFlag = 0;

	if (pEntityPlayer->GetActionState() == AS_SWIMMING) 
	{
		SetBitSet(out.nStatusFlag, UPS_SWIMMING);
	}

	if (pEntityPlayer->GetAFK().IsNowAFK())
	{
		SetBitSet(out.nStatusFlag, UPS_AFK);
	}

	if (pEntityPlayer->IsPartyLeader())
	{
		SetBitSet(out.nStatusFlag, UPS_PARTYLEADER);
	}

	if (true == pEntityPlayer->IsDead())
	{
		SetBitSet(out.nStatusFlag, UPS_DEAD);
	}
	else
	{
		if (true == pEntityPlayer->GetDoing().IsNowLooting())
		{
			SetBitSet(out.nStatusFlag, UPS_LOOTING);
		}
		else if (true == pEntityPlayer->GetSit().IsNowSit())
		{
			SetBitSet(out.nStatusFlag, UPS_SITTING);
		}
		else if (true == pEntityPlayer->GetDoing().IsNowCutscening())
		{
			SetBitSet(out.nStatusFlag, UPS_CUTSCENING);
		}
	}	

	// 필드 PVP 상태
	if (pEntityPlayer->GetPlayerPVPArea().IsLocatedPvPArea())
	{
		switch (pEntityPlayer->GetPlayerPVPArea().GetTeam())
		{
		case QT_ONE: { SetBitSet(out.nStatusFlag, UPS_FIELDPVP_TEAM1); }break;
		case QT_TWO: { SetBitSet(out.nStatusFlag, UPS_FIELDPVP_TEAM2); }break;
		}
	}
}