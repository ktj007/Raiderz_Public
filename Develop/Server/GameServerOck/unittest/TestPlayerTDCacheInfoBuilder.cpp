#include "stdafx.h"
#include "GPlayerTDCacheInfoBuilder.h"
#include "GEntityPlayer.h"
#include "GPlayerDoing.h"
#include "MockEntityPlayer.h"
#include "SUnitTestUtil.h"
#include "FBaseItem.h"
#include "FBaseTalent.h"
#include "GUTHelper_Item.h"
#include "GItemHolder.h"
#include "FBaseGame.h"
#include "GEquipmentSlot.h"
#include "GPlayerSit.h"
#include "GPlayerStance.h"

SUITE(PlayerTDCacheInfoBuilder)
{
	class FPlayerTDCacheInfoBuilder : public FBaseItem, public FBaseGame
	{
	public:
		FPlayerTDCacheInfoBuilder()
		{
			PLAYER_INFO* pPlayerInfo = m_pPlayer->GetPlayerInfo();
			GItemHolder* pItemHolder = m_pPlayer->GetItemHolder();

			// base
			m_pPlayer->SetPos(vec3(1000.0f, 50.0f, 0.0f)) ;
			m_pPlayer->SetDir(vec3(0.5f, 0.5f, 0.0f));
			m_pPlayer->GetStance().Change(CS_BATTLE);
			wcscpy_s(pPlayerInfo->szName, L"birdkr");

			// feature
			pPlayerInfo->nSex = SEX_FEMALE;
			pPlayerInfo->nFeatureHair = 5;
			pPlayerInfo->nFeatureFace = 2;
			pPlayerInfo->nFeatureHairColor = 101;
			pPlayerInfo->nFeatureSkinColor = 50;
			pItemHolder->GetEquipment().SwitchingWeaponSet(ITEMSWITCH_SECONDARY);

			// item
			GUTHelper_Item::NewItemData(1, ITEMSLOT_HEAD);
			GUTHelper_Item::EquipNewItem(m_pPlayer, 1, ITEMSLOT_HEAD);
		}

		virtual ~FPlayerTDCacheInfoBuilder()
		{
			m_pPlayer->GetDoing().EndDoing();
		}


		GPlayerTDCacheInfoBuilder		m_Builder;
	};

	TEST_FIXTURE(FPlayerTDCacheInfoBuilder, TestPlayerTDCacheInfoBuilder_BuildBaseInfo)
	{
		TD_UPDATE_CACHE_PLAYER td;

		m_Builder.BuildBaseInfo(m_pPlayer, td);

		CHECK_EQUAL(td.uid, m_pPlayer->GetUID());
		CHECK_EQUAL(td.nUIID, m_pPlayer->GetUIID());
		CHECK(0 == wcscmp(td.szName, m_pPlayer->GetPlayerInfo()->szName));

		CHECK_CLOSE(td.vPos.x, m_pPlayer->GetPos().x, 0.001f);
		CHECK_CLOSE(td.vPos.y, m_pPlayer->GetPos().y, 0.001f);
		CHECK_CLOSE(td.vPos.z, m_pPlayer->GetPos().z, 0.001f);

		CHECK_CLOSE(td.svDir.X(), m_pPlayer->GetDir().x, 0.001f);
		CHECK_CLOSE(td.svDir.Y(), m_pPlayer->GetDir().y, 0.001f);

		CHECK_EQUAL(td.nStance, m_pPlayer->GetStance().Get());

		CHECK_EQUAL(td.nMF, m_pPlayer->GetCurrMF());
		CHECK_EQUAL(td.nMFWeight, (uint16)m_pPlayer->GetCurrWeight());
	}
	
	TEST_FIXTURE(FPlayerTDCacheInfoBuilder, TestPlayerTDCacheInfoBuilder_BuildFeature)
	{
		PLAYER_INFO* pPlayerInfo = m_pPlayer->GetPlayerInfo();
		GItemHolder* pItemHolder = m_pPlayer->GetItemHolder();

		TD_UPDATE_CACHE_PLAYER td;

		m_Builder.BuildFeature(m_pPlayer, td);

		CHECK_EQUAL(td.Feature.nSex, pPlayerInfo->nSex);
		CHECK_EQUAL(td.Feature.nHair, (int8)pPlayerInfo->nFeatureHair);
		CHECK_EQUAL(td.Feature.nFace, (int8)pPlayerInfo->nFeatureFace);
		CHECK_EQUAL(td.Feature.nHairColor, pPlayerInfo->nFeatureHairColor);
		CHECK_EQUAL(td.Feature.nSkinColor, pPlayerInfo->nFeatureSkinColor);
		CHECK_EQUAL(td.Feature.nWeaponSet, (int8)pItemHolder->GetEquipment().GetWeaponSet());
	}
	

	TEST_FIXTURE(FPlayerTDCacheInfoBuilder, TestPlayerTDCacheInfoBuilder_BuildEquipment)
	{
		PLAYER_INFO* pPlayerInfo = m_pPlayer->GetPlayerInfo();

		TD_UPDATE_CACHE_PLAYER td;

		m_Builder.BuildEquipment(m_pPlayer, td);

		CHECK_EQUAL(1, td.Feature.nItemID[FEATURE_ITEMSLOT_HEAD]);
		CHECK_EQUAL(0, td.Feature.nItemID[FEATURE_ITEMSLOT_FACE]);
		CHECK_EQUAL(0, td.Feature.nItemID[FEATURE_ITEMSLOT_HANDS]);
		CHECK_EQUAL(0, td.Feature.nItemID[FEATURE_ITEMSLOT_FEET]);
		CHECK_EQUAL(0, td.Feature.nItemID[FEATURE_ITEMSLOT_BODY]);
		CHECK_EQUAL(0, td.Feature.nItemID[FEATURE_ITEMSLOT_LEG]);
		CHECK_EQUAL(0, td.Feature.nItemID[FEATURE_ITEMSLOT_LWEAPON]);
		CHECK_EQUAL(0, td.Feature.nItemID[FEATURE_ITEMSLOT_RWEAPON]);
		CHECK_EQUAL(0, td.Feature.nItemID[FEATURE_ITEMSLOT_LWEAPON2]);
		CHECK_EQUAL(0, td.Feature.nItemID[FEATURE_ITEMSLOT_RWEAPON2]);
	}
	

	TEST_FIXTURE(FPlayerTDCacheInfoBuilder, TestPlayerTDCacheInfoBuilder_BuildStatusFlag)
	{
		TD_UPDATE_CACHE_PLAYER td;

		m_Builder.BuildStatusFlag(m_pPlayer, td);

		CHECK_EQUAL(0, td.nStatusFlag);
	}


	TEST_FIXTURE(FPlayerTDCacheInfoBuilder, TestPlayerTDCacheInfoBuilder_BuildStatusFlag_SWIMMING)
	{
		m_pPlayer->ChangeActionState(AS_SWIMMING);

		TD_UPDATE_CACHE_PLAYER td;

		m_Builder.BuildStatusFlag(m_pPlayer, td);

		CHECK(CheckBitSet(td.nStatusFlag, UPS_SWIMMING) > 0);
	}

	TEST_FIXTURE(FPlayerTDCacheInfoBuilder, TestPlayerTDCacheInfoBuilder_BuildStatusFlag_DEAD)
	{
		m_pPlayer->SetDie();

		TD_UPDATE_CACHE_PLAYER td;

		m_Builder.BuildStatusFlag(m_pPlayer, td);

		CHECK(CheckBitSet(td.nStatusFlag, UPS_DEAD) > 0);
	}


	TEST_FIXTURE(FPlayerTDCacheInfoBuilder, TestPlayerTDCacheInfoBuilder_BuildStatusFlag_LOOTING)
	{
		m_pPlayer->GetDoing().ChangeDoing(CD_LOOTING);

		TD_UPDATE_CACHE_PLAYER td;
		m_Builder.BuildStatusFlag(m_pPlayer, td);

		CHECK(CheckBitSet(td.nStatusFlag, UPS_LOOTING) > 0);
	}

	TEST_FIXTURE(FPlayerTDCacheInfoBuilder, TestPlayerTDCacheInfoBuilder_BuildStatusFlag_SITTING)
	{
		m_pPlayer->GetSit().Sit();

		TD_UPDATE_CACHE_PLAYER td;
		m_Builder.BuildStatusFlag(m_pPlayer, td);

		CHECK(CheckBitSet(td.nStatusFlag, UPS_SITTING) > 0);
	}

	TEST_FIXTURE(FPlayerTDCacheInfoBuilder, TestPlayerTDCacheInfoBuilder_BuildStatusFlag_CUTSCENING)
	{
		m_pPlayer->GetDoing().ChangeDoing(CD_CUTSCENING);

		TD_UPDATE_CACHE_PLAYER td;
		m_Builder.BuildStatusFlag(m_pPlayer, td);

		CHECK(CheckBitSet(td.nStatusFlag, UPS_CUTSCENING) > 0);
	}

}