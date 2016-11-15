#include "stdafx.h"
#include "XGame.h"
#include "XController.h"
#include "XModuleEntity.h"
#include "XModuleMotion.h"
#include "XGlobal.h"
#include "XObjectManager.h"
#include "TestGame.h"
#include "XMyPlayer.h"
#include "XNetPlayer.h"
#include "XNonPlayer.h"
#include "XSystem.h"
#include "CTransData.h"
#include "MockNetClient.h"
#include "XNPCInfo.h"
#include "XItemManager.h"
#include "XCharacterSerializer.h"
#include "XUnitTestHelper.h"
#include "XGameTransformControllerManager.h"

SUITE(ModuleEntity)
{
	TEST(TestSimpleMyPlayerModuleEntity)
	{
		TestGame		tg;

		// MyPlayer는 이미 생성되어 있다.
		CHECK_EQUAL(gg.omgr->GetCount(), 1);
		XObject* pMyObject = gg.omgr->Find(XGetMyUID());
		CHECK(pMyObject != NULL);

		if (pMyObject == NULL) return;

		CHECK_EQUAL(MIsExactlyClass(XMyPlayer, pMyObject), true);
		if (MIsExactlyClass(XMyPlayer, pMyObject) == false) return;

		XMyPlayer* pMyPlayer = pMyObject->AsMyPlayer();
		CHECK_EQUAL(pMyPlayer->IsVisible(), false);

		XModuleEntity* pMyModuleEntity = pMyPlayer->GetModuleEntity();
		CHECK(pMyModuleEntity->GetAnimationController()->GetAnimationName() == wstring(L"none_idle"));


		tg.Update(0.1f);
	}

	TEST(TestSimpleInPlayer)
	{
		TestGame		tg;

		XGame* pGame = tg.GetGame();

		MUID uidPlayer = global.system->GenerateLocalUID();
	
		TD_UPDATE_CACHE_PLAYER td_update_cache_player = XUnitTestHelper::MakeSamle_TD_UPDATE_CACHE_PLAYER(uidPlayer);
		td_update_cache_player.svDir = vec3(1.f, 0.f, 0.f);

		XPlayer* pPlayer = pGame->GetCurrentGameState()->InPlayer(uidPlayer, &td_update_cache_player);

		CHECK_EQUAL(gg.omgr->GetCount(), 2);


		CHECK_EQUAL(MIsExactlyClass(XNetPlayer, pPlayer), true);
		if (MIsExactlyClass(XNetPlayer, pPlayer) == false) return;

		XNetPlayer* pNetPlayer = AsNetPlayer(pPlayer);
		CHECK(!wcscmp(L"bird", pNetPlayer->GetName()));
		CHECK_EQUAL(pNetPlayer->GetSex(), SEX_FEMALE);
		
		// feature 체크
		XPlayerInfoFeature& netplayer_feature = pNetPlayer->GetFeature();
		CHECK_EQUAL(netplayer_feature.nHair, td_update_cache_player.Feature.nHair);
		CHECK_EQUAL(netplayer_feature.nFace, td_update_cache_player.Feature.nFace);		
		CHECK_EQUAL(netplayer_feature.nHairColor, td_update_cache_player.Feature.nHairColor);
		CHECK_EQUAL(netplayer_feature.nSkinColor, td_update_cache_player.Feature.nSkinColor);
		CHECK_EQUAL(netplayer_feature.nWeaponSet, td_update_cache_player.Feature.nWeaponSet);

		for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
		{
			CHECK_EQUAL(netplayer_feature.nEquipedItemID[i], td_update_cache_player.Feature.nItemID[i]);
			CHECK_EQUAL(netplayer_feature.nEquipedItemColor[i], td_update_cache_player.Feature.nItemID_DyedColor[i]);
		}

		// 위치, 방향 체크
		CHECK_CLOSE(vec3(1000.0f, 1000.0f, 0.0f).DistanceTo(pNetPlayer->GetPosition()), 0.0f, 0.0001f);
		CHECK_CLOSE(vec3(1.0f, 0.0f, 0.0f).DistanceTo(pNetPlayer->GetDirection()), 0.0f, 0.0001f);

		// 모션 체크
		XModuleMotion* pModuleMotion = pNetPlayer->GetModuleMotion();
		CHECK(pModuleMotion->IsCurrentMotion(L"idle"));
	}


	TEST(TestHumanNPCEntityFeatures)
	{
		XUnitTestHelper helper;
		TestGame		tg;

		XGame* pGame = tg.GetGame();

		// npc 정보 추가
		XUnitTestHelper::NewNPCInfo(1, L"노아", 3);

		int NPC_ID = 1;
		CHECK(info.npc->Get(NPC_ID) != NULL);

		XNPCInfo* pNPCInfo = info.npc->Get(NPC_ID);
		CHECK(pNPCInfo != NULL);
		if (pNPCInfo == NULL) return;

		// NPC 입장
		const MUID uidNPC = global.system->GenerateLocalUID();
		helper.InNPC(uidNPC, NPC_ID, vec3(1000.0f, 1000.0f, 0.0f));

		CHECK_EQUAL(gg.omgr->GetCount(), 2);

		XNonPlayer* pNPC = gg.omgr->FindNPC(uidNPC);
		CHECK(pNPC != NULL);
		if (pNPC == NULL) return;

		XCharacterSerializer* pSerializer = pNPC->GetModuleEntity()->GetSerializer();

		// item 정보 추가 --------------------

		// npc hair
		info.item->CreateItemFromString(L"<ITEM id=\"1\" name=\"인간형 NPC 머리\" mesh_name=\"hair01\" node_name=\"hair\" />");
		XItemData* pItemData = info.item->GetItemData(1);
		if (pItemData)
		{
			pItemData->m_bMeshNameBySex = false;
			pItemData->m_ItemType = ITEMTYPE_ARMOR;
		}

		// npc face
		info.item->CreateItemFromString(L"<ITEM id=\"2\" name=\"인간형 NPC 얼굴\" mesh_name=\"face01\" node_name=\"face\"/>");
		pItemData = info.item->GetItemData(2);
		if (pItemData)
		{
			pItemData->m_bMeshNameBySex = false;
			pItemData->m_ItemType = ITEMTYPE_ARMOR;
		}

		// npc head
		info.item->CreateItemFromString(L"<ITEM id=\"3\" name=\"인간형 NPC head\" mesh_name=\"head01\" slot=\"head\" node_name=\"head\" />");
		pItemData = info.item->GetItemData(3);
		if (pItemData)
		{
			pItemData->m_bMeshNameBySex = false;
			pItemData->m_ItemType = ITEMTYPE_ARMOR;
		}

		// npc chest
		info.item->CreateItemFromString(L"<ITEM id=\"4\" name=\"인간형 NPC chest\" mesh_name=\"chest01\" slot=\"body\" node_name=\"chest\" />");
		pItemData = info.item->GetItemData(4);
		if (pItemData)
		{
			pItemData->m_bMeshNameBySex = false;
			pItemData->m_ItemType = ITEMTYPE_ARMOR;
		}

		// npc legs
		info.item->CreateItemFromString(L"<ITEM id=\"5\" name=\"인간형 NPC legs\" mesh_name=\"legs01\" slot=\"leg\" node_name=\"legs\" />");
		pItemData = info.item->GetItemData(5);
		if (pItemData)
		{
			pItemData->m_bMeshNameBySex = false;
			pItemData->m_ItemType = ITEMTYPE_ARMOR;
		}

		// npc hands
		info.item->CreateItemFromString(L"<ITEM id=\"6\" name=\"인간형 NPC hands\" mesh_name=\"hands01\" slot=\"hands\" node_name=\"hands\" />");
		pItemData = info.item->GetItemData(6);
		if (pItemData)
		{
			pItemData->m_bMeshNameBySex = false;
			pItemData->m_ItemType = ITEMTYPE_ARMOR;
		}

		// npc feet
		info.item->CreateItemFromString(L"<ITEM id=\"7\" name=\"인간형 NPC feet\" mesh_name=\"feet01\" slot=\"feet\" node_name=\"feet\" />");
		pItemData = info.item->GetItemData(7);
		if (pItemData)
		{
			pItemData->m_bMeshNameBySex = false;
			pItemData->m_ItemType = ITEMTYPE_ARMOR;
		}

		// NPC에 옷 장착된 NPC 정보 추가
		info.npc->CreateNPCFromString(L"<NPC id=\"2\" Name=\"노아 옷 입음\" Grade=\"3\" Type=\"npc\" MeshPath=\"NPC/hm_adult\" MeshName=\"hm_adult\"		\
									  PartsHair=\"1\" PartsFace=\"2\" PartsHead=\"3\" PartsChest=\"4\" PartsLeg=\"5\" PartsHands=\"6\" PartsFeet=\"7\"	\
									   />");

		NPC_ID = 2;
		const MUID uidNPC2 = global.system->GenerateLocalUID();
		helper.InNPC(uidNPC2, NPC_ID, vec3(1000.0f, 1000.0f, 0.0f));

		CHECK_EQUAL(gg.omgr->GetCount(), 3);

		pNPC = gg.omgr->FindNPC(uidNPC2);
		CHECK(pNPC != NULL);
		if (pNPC == NULL) return;

		pSerializer = pNPC->GetModuleEntity()->GetSerializer();

		XCharacter* pActor = pNPC->GetModuleEntity()->GetActor();

		CHECK(pActor->GetReservedSlotMeshName(NPC_PARTS_SLOT_HAIR) == wstring(L"hair01.elu"));
		CHECK(pActor->GetReservedSlotMeshName(NPC_PARTS_SLOT_FACE) == wstring(L"face01.elu"));
		CHECK(pActor->GetReservedSlotMeshName(NPC_PARTS_SLOT_HEAD) == wstring(L"head01.elu"));
		CHECK(pActor->GetReservedSlotMeshName(NPC_PARTS_SLOT_CHEST) == wstring(L"chest01.elu"));
		CHECK(pActor->GetReservedSlotMeshName(NPC_PARTS_SLOT_LEG) == wstring(L"legs01.elu"));
		CHECK(pActor->GetReservedSlotMeshName(NPC_PARTS_SLOT_HANDS) == wstring(L"hands01.elu"));
		CHECK(pActor->GetReservedSlotMeshName(NPC_PARTS_SLOT_FEET) == wstring(L"feet01.elu"));
	}
}