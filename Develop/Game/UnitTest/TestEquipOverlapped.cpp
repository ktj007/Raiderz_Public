#include "stdafx.h"
#include "TestGame.h"
#include "XItemManager.h"
#include "XMyPlayer.h"
#include "XNetPlayer.h"
#include "XNetwork.h"
#include "TestModule.h"
#include "XGlueGame.h"
#include "FBaseNetClient.h"

SUITE(TestEquipOverlapped)
{
	struct FEquipOverlappedFixture
	{
		FEquipOverlappedFixture()
		{
			InitPlayerInfo();
			InitItemInfo();
		}
		~FEquipOverlappedFixture()
		{
			info.item->Clear();
		}
		void InitPlayerInfo()
		{
			m_pMyPlayer = tg.GetMyPlayer();
			if (m_pMyPlayer == NULL) return;

			minet::MTestNetAgent		netAgent(global.net->GetClient());
			TestModuleContext	testContext(&tg, &netAgent);
			testContext.MakeContextFieldLoadingCompleteIgnore();

			// net player
			m_pNetPlayer = testContext.CreateNetPlayerAndEnterToGmae(vec3(0.f, 0.f, 0.0f), vec3(1.f, 0.f, 0.f), SEX_FEMALE);
			CHECK(m_pNetPlayer != NULL);
		}
		void InitItemInfo()
		{

			info.item->CreateItemFromString(L"<ITEM id=\"1\" name=\"보급형 검\" mesh_name='1hd_03' mesh_path='weapons' parent_name='dummy_1hs_r' type='weapon' slot=\"rweapon\" weapon_type='1hs' node_name=\"1hd_03\" />");

			info.item->CreateItemFromString(L"<ITEM id=\"2\" name=\"보급형 방패\" mesh_name='shield_01' mesh_path='weapons' parent_name='dummy_shield' type='equipment' slot=\"lweapon\" equipment_type='shield' node_name='shield_01' />");

			info.item->CreateItemFromString(L"<ITEM id=\"3\" name=\"메이스\" mesh_name='1hd_03' mesh_path='weapons' parent_name='dummy_1hb_r' type='weapon' slot=\"rweapon\" weapon_type='1hb' node_name=\"1hd_03\" />");

		}

		TestGame		tg;
		XMyPlayer*		m_pMyPlayer;
		XNetPlayer*		m_pNetPlayer;

	};

	TEST_FIXTURE(FEquipOverlappedFixture, Test_MyPlayerEquipOverlapped)
	{
		m_pMyPlayer->EquipOverlapped(3);
		
		CHECK(gvar.MyInfo.EquipmentSlot.IsEquipItem(3));
	}

	TEST_FIXTURE(FEquipOverlappedFixture, Test_MyPlayerUnEquipOverlapped)
	{
		m_pMyPlayer->EquipOverlapped(3);

		m_pMyPlayer->UnEquipOverlapped(3);
		CHECK(gvar.MyInfo.EquipmentSlot.IsEquipItem(3) == false);
	}

	TEST_FIXTURE(FEquipOverlappedFixture, TestEquipOverlapped_MyPlayerSaveEquipItem)
	{
		XItem* pItem = info.item->MakeNewItem(1);
		m_pMyPlayer->EquipItem(pItem, ITEMSLOT_RWEAPON);

		CHECK(gvar.MyInfo.EquipmentSlot.IsEquipItem(1));

		m_pMyPlayer->EquipOverlapped(3);

		CHECK(gvar.MyInfo.EquipmentSlot.IsEquipOverlappedItem(1));
	}

	TEST_FIXTURE(FEquipOverlappedFixture, TestEquipOverlapped_MyPlayerSaveUnEquipItem)
	{
		XItem* pItem = info.item->MakeNewItem(1);
		m_pMyPlayer->EquipItem(pItem, ITEMSLOT_RWEAPON);

		CHECK(gvar.MyInfo.EquipmentSlot.IsEquipItem(1));

		m_pMyPlayer->EquipOverlapped(3);
		m_pMyPlayer->UnEquipOverlapped(3);

		CHECK(gvar.MyInfo.EquipmentSlot.IsEquipOverlappedItem(1) == false);
	}

	TEST_FIXTURE(FEquipOverlappedFixture, Test_NetPlayerEquipOverlapped)
	{
		m_pNetPlayer->EquipOverlapped(3);

		CHECK(m_pNetPlayer->GetFeature().nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON]);
	}

	TEST_FIXTURE(FEquipOverlappedFixture, Test_NetPlayerUnEquipOverlapped)
	{
		m_pNetPlayer->EquipOverlapped(3);

		m_pNetPlayer->UnEquipOverlapped(3);
		CHECK(m_pNetPlayer->GetFeature().nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON] == false);
	}

	TEST_FIXTURE(FEquipOverlappedFixture, TestEquipOverlapped_NetPlayerSaveEquipItem)
	{
		m_pNetPlayer->EquipItem(FEATURE_ITEMSLOT_RWEAPON, 1, FEATURE_ITEMSLOT_NONE, 0, 0);

		CHECK(m_pNetPlayer->GetFeature().nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON]);

		m_pNetPlayer->EquipOverlapped(3);

		CHECK(m_pNetPlayer->GetOverlappedFeature().nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON] == 1);
	}

	TEST_FIXTURE(FEquipOverlappedFixture, TestEquipOverlapped_NetPlayerSaveUnEquipItem)
	{
		m_pNetPlayer->EquipItem(FEATURE_ITEMSLOT_RWEAPON, 1, FEATURE_ITEMSLOT_NONE, 0, 0);

		CHECK(m_pNetPlayer->GetFeature().nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON]);

		m_pNetPlayer->EquipOverlapped(3);
		m_pNetPlayer->UnEquipOverlapped(3);

		CHECK(m_pNetPlayer->GetOverlappedFeature().nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON] == 0);
	}

	struct FBPartWeaponControllerFixture : public FBaseNetClient
	{
		FBPartWeaponControllerFixture()  : FBaseNetClient()
		{
			m_pMyPlayer = tg.GetMyPlayer();
			if (m_pMyPlayer == NULL) return;
		}
		~FBPartWeaponControllerFixture()
		{
		}

		TestGame		tg;
		XMyPlayer*		m_pMyPlayer;
	};

	TEST_FIXTURE(FBPartWeaponControllerFixture, Test_StartBPartWeaponController)
	{
		XModuleActorControl* pModuleActorControl = m_pMyPlayer->GetModuleActorControl();
		pModuleActorControl->StartOverlappedEquipmentController(0);

		CHECK(pModuleActorControl->IsOverlappedEquipmentControllerWork());
	}

	TEST_FIXTURE(FBPartWeaponControllerFixture, Test_EndBPartWeaponController)
	{
		XModuleActorControl* pModuleActorControl = m_pMyPlayer->GetModuleActorControl();
		pModuleActorControl->StartOverlappedEquipmentController(0);
		pModuleActorControl->EndOverlappedEquipmentController(0);

		CHECK(pModuleActorControl->IsOverlappedEquipmentControllerWork() == false);
	}

	TEST_FIXTURE(FBPartWeaponControllerFixture, Test_UsableActionStateBPartWeaponController)
	{
		XModuleActorControl* pModuleActorControl = m_pMyPlayer->GetModuleActorControl();
		pModuleActorControl->StartOverlappedEquipmentController(0);

		CHECK(pModuleActorControl->UsableActionState_OverlappedEquipmentItem(ACTION_STATE_NORMAL_ATTACK) == false);
		CHECK(pModuleActorControl->UsableActionState_OverlappedEquipmentItem(ACTION_STATE_CHANGE_STANCE) == false);
		CHECK(pModuleActorControl->UsableActionState_OverlappedEquipmentItem(ACTION_STATE_GUARD) == false);
		CHECK(pModuleActorControl->UsableActionState_OverlappedEquipmentItem(ACTION_STATE_WEAPON_CHANGE) == false);
		CHECK(pModuleActorControl->UsableActionState_OverlappedEquipmentItem(ACTION_STATE_SIT) == false);
		
		CHECK(pModuleActorControl->UsableActionState_OverlappedEquipmentItem(ACTION_STATE_HIT));
		CHECK(pModuleActorControl->UsableActionState_OverlappedEquipmentItem(ACTION_STATE_DIE));
		CHECK(pModuleActorControl->UsableActionState_OverlappedEquipmentItem(ACTION_STATE_GRAPPLED));
		CHECK(pModuleActorControl->UsableActionState_OverlappedEquipmentItem(ACTION_STATE_SWALLOWED));
	}

	TEST_FIXTURE(FBPartWeaponControllerFixture, Test_ExchangeEquippedItem)
	{
		// BPart 사용시 장착하면 안된다.
		XModuleActorControl* pModuleActorControl = m_pMyPlayer->GetModuleActorControl();
		pModuleActorControl->StartOverlappedEquipmentController(0);

		XGlueGame testGlueGame;
		testGlueGame.ExchangeEquippedItem( ITEMSLOT_LWEAPON, ITEMSLOT_LWEAPON2);

		int nSendCount = m_pNetAgent->GetSendCommandCount();
		for(int i = 0; i < nSendCount; ++i)
		{
			CHECK(m_pNetAgent->GetSendCommand(i).GetID() != MC_ITEM_EQUIP_REQ);
		}
	}

	TEST_FIXTURE(FBPartWeaponControllerFixture, Test_UnequipItem)
	{
		// BPart 사용시 장착이 벗기면 안된다.
		XModuleActorControl* pModuleActorControl = m_pMyPlayer->GetModuleActorControl();
		pModuleActorControl->StartOverlappedEquipmentController(0);

		XGlueGame testGlueGame;
		testGlueGame.UnequipItem( ITEMSLOT_LWEAPON);

		int nSendCount = m_pNetAgent->GetSendCommandCount();
		for(int i = 0; i < nSendCount; ++i)
		{
			CHECK(m_pNetAgent->GetSendCommand(i).GetID() != MC_ITEM_UNEQUIP_REQ);
		}
	}

	TEST_FIXTURE(FBPartWeaponControllerFixture, Test_EquipInvenItem)
	{
		info.item->CreateItemFromString(L"<ITEM id=\"1\" name=\"보급형 검\" mesh_name='1hd_03' mesh_path='weapons' parent_name='dummy_1hs_r' type='weapon' slot=\"rweapon\" weapon_type='1hs' node_name=\"1hd_03\" />");

		info.item->CreateItemFromString(L"<ITEM id=\"2\" name=\"보급형 방패\" mesh_name='shield_01' mesh_path='weapons' parent_name='dummy_shield' type='equipment' slot=\"lweapon\" equipment_type='shield' node_name='shield_01' />");

		info.item->CreateItemFromString(L"<ITEM id=\"3\" name=\"메이스\" mesh_name='1hd_03' mesh_path='weapons' parent_name='dummy_1hb_r' type='weapon' slot=\"rweapon\" weapon_type='1hb' node_name=\"1hd_03\" />");


		// BPart 사용시 장착하면 안된다.
		XModuleActorControl* pModuleActorControl = m_pMyPlayer->GetModuleActorControl();
		pModuleActorControl->StartOverlappedEquipmentController(0);

		XUnitTestHelper::CreateItemAndEquip(m_pMyPlayer, 1, ITEMSLOT_RWEAPON);
		XUnitTestHelper::CreateItemAndInven(m_pMyPlayer, 2, ITEMSLOT_RWEAPON);

		XGlueGame testGlueGame;
		testGlueGame.EquipInvenItem( ITEMSLOT_RWEAPON, 0);

		int nSendCount = m_pNetAgent->GetSendCommandCount();
		for(int i = 0; i < nSendCount; ++i)
		{
			CHECK(m_pNetAgent->GetSendCommand(i).GetID() != MC_ITEM_EQUIP_REQ);
		}

		info.item->Clear();

	}
}
