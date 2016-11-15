#include "stdafx.h"
#include "XCharacterParts.h"
#include "XModelParts.h"
#include "MockCharacter.h"
#include "MockMesh.h"
#include "MockPlayer.h"
#include "XModuleEntity.h"
#include "XCharacterSerializeStrategy.h"
#include "XPlayerSerializeStrategy.h"
#include "XNaming.h"
#include "XTestWrapper.h"
#include "XItemManager.h"
#include "MockItemInfo.h"

SUITE(CharacterParts)
{
	TEST(TestModelPart_Assign)
	{
		XPartsParam partsParam;
		partsParam.Make(1, L"SampleMeshName", NULL);
		partsParam.AddPart(wstring(L"nodeName1"), wstring(L""), wstring(L""), 0xFFFF0000);
		partsParam.AddPart(wstring(L"nodeName2"), wstring(L""), wstring(L""));

		XPartInfo part;
		part.Assign(&partsParam);

		CHECK(wstring(L"SampleMeshName") == part.strMeshName);
		CHECK_EQUAL(1, part.nPartsID);
	}

	TEST(TestModelPartsList_Reserve)
	{
		XModelPartsInfo partsList;

		const int nPartsID = PLAYER_PARTS_SLOT_HEAD;
		XPlayerPartsParam partsParam;
		partsParam.Make(nPartsID, L"SampleMeshName", NULL, L"SampleNodeName");

		CHECK_EQUAL(true, partsList.IsLoadingQueueEmpty());

		partsList.Reserve(&partsParam);

		CHECK_EQUAL(false, partsList.IsLoadingQueueEmpty());

		partsList.UpdateQueue();

		// 여전히 Part의 상태(State)가 PART_STATE_LOADING_COMPLETED가 아니라 큐에 남아있음
		CHECK_EQUAL(false, partsList.IsLoadingQueueEmpty());


		// 큐가 COMPLETED면 다음 UpdateQueue시 큐가 비워짐
		for (XModelPartsInfo::ModelPartsList::iterator itor = partsList.GetLoadingQueue().begin();
			itor != partsList.GetLoadingQueue().end(); ++itor)
		{
			XPartInfo* pPart = (*itor);
			pPart->SetState(PART_STATE_LOADING_COMPLETED);
		}

		partsList.UpdateQueue();

		CHECK_EQUAL(true, partsList.IsLoadingQueueEmpty());


		XPartInfo* pPart = partsList.Find(nPartsID);
		CHECK(pPart != NULL);

		CHECK_EQUAL(PART_STATE_LOADING_COMPLETED, pPart->GetState());
	}


	TEST(TestCharacterParts)
	{
		MockCharacter character;
		TestCharacterParts characterParts(&character);

		CHECK_EQUAL(true, characterParts.IsLoadingCompleted());
	}

	struct FSerializeFixture
	{
		FSerializeFixture() 
		{
			InitPlayerInfo();
			InitItemInfo();

			m_pPlayer = new MockPlayer(MUID::ZERO, SEX_FEMALE);
			m_pModuleEntity = m_pPlayer->GetModuleEntity();
			m_pCharacter = dynamic_cast<MockCharacter*>(m_pModuleEntity->GetActor());

			m_pMyPlayerSerializeStrategy = new XMyPlayerSerializeStrategy(m_pModuleEntity->GetActor(), true);
		}
		~FSerializeFixture()
		{
			SAFE_DELETE(m_pMyPlayerSerializeStrategy);
			SAFE_DELETE(m_pPlayer);
		}
		void InitPlayerInfo()
		{
			m_PlayerInfo.nSex = SEX_FEMALE;
			m_PlayerInfo.nFeatureHair = 1;
			m_PlayerInfo.nFeatureFace = 1;
		}
		void InitItemInfo()
		{

			info.item->CreateItemFromString(L"<ITEM id=\"1\" name=\"보급형 검\" mesh_name='1hd_03' mesh_path='weapons' parent_name='dummy_1hs_r' type='weapon' slot=\"rweapon\" weapon_type='1hs' node_name=\"1hd_03\" />");

			info.item->CreateItemFromString(L"<ITEM id=\"2\" name=\"보급형 방패\" mesh_name='shield_01' mesh_path='weapons' parent_name='dummy_shield' type='equipment' slot=\"lweapon\" equipment_type='shield' node_name='shield_01' />");

			info.item->CreateItemFromString(L"<ITEM id=\"3\" name=\"메이스\" mesh_name='1hd_03' mesh_path='weapons' parent_name='dummy_1hb_r' type='weapon' slot=\"rweapon\" weapon_type='1hb' node_name=\"1hd_03\" />");

		}
		MockPlayer*			m_pPlayer;
		XModuleEntity*		m_pModuleEntity;
		XMyPlayerSerializeStrategy* m_pMyPlayerSerializeStrategy;
		MockCharacter*		m_pCharacter;

		PLAYER_INFO			m_PlayerInfo;
		XMyEquipmentSlot	m_myEquipmentSlot;

		XTestInfoWrapper2<XItemManager, MockItemInfoMgr>	m_ItemInfoWrapper;

	};

	TEST_FIXTURE(FSerializeFixture, TestMyPlayerSerialize_Feature)
	{
		
		m_pMyPlayerSerializeStrategy->Serialize(m_PlayerInfo, &m_myEquipmentSlot);

		const XCharacterSerializeStrategy::_ActorFeature& feature = m_pMyPlayerSerializeStrategy->GetFeatureForTest();

		CHECK_EQUAL(feature.nSex, SEX_FEMALE);
		CHECK(feature.strMeshName == XNaming::GetPlayerModelName(SEX_FEMALE));

		wstring strHairModel, strFaceModel;
		XNaming::GetHairModelName(strHairModel, SEX_FEMALE, m_PlayerInfo.nFeatureHair);
		XNaming::GetFaceModelName(strFaceModel, SEX_FEMALE, m_PlayerInfo.nFeatureFace);

		CHECK(feature.strHairMeshName == strHairModel);
		CHECK(feature.strFaceMeshName == strFaceModel);
	}

	TEST_FIXTURE(FSerializeFixture, TestMyPlayerSerialize_Mesh)
	{
		m_pMyPlayerSerializeStrategy->Serialize(m_PlayerInfo, &m_myEquipmentSlot);

		CHECK_EQUAL(string("hf.elu"), m_pCharacter->GetMeshName());
	}

	TEST_FIXTURE(FSerializeFixture, TestMyPlayerSerialize_DefaultActorNode)
	{
		m_pMyPlayerSerializeStrategy->Serialize(m_PlayerInfo, &m_myEquipmentSlot);

		//m_pCharacter->Update();
		m_pCharacter->UpdateAllSceneNodes();

		CHECK(m_pCharacter->GetActorNode("hair") != NULL);
		CHECK(m_pCharacter->GetActorNode("face") != NULL);
		CHECK(m_pCharacter->GetActorNode("chest") != NULL);
		CHECK(m_pCharacter->GetActorNode("hands") != NULL);
		CHECK(m_pCharacter->GetActorNode("legs") != NULL);
		CHECK(m_pCharacter->GetActorNode("feet") != NULL);
	}

	TEST_FIXTURE(FSerializeFixture, TestMyPlayerSerialize_EquipWeapon)
	{
		XItem* pWeaponItem1 = info.item->MakeNewItem(1);

		// 무기 장착
		m_myEquipmentSlot.EquipItem(pWeaponItem1, ITEMSLOT_RWEAPON);

		m_pMyPlayerSerializeStrategy->Serialize(m_PlayerInfo, &m_myEquipmentSlot);

		TestCharacterParts* cp = m_pCharacter->GetCharacterParts();

		CHECK(cp->GetReservedSlotMeshName(PLAYER_PARTS_SLOT_RWEAPON) == wstring(L"data/model/weapons/1hd_03.elu"));

		CHECK_EQUAL(cp->IsLoadingCompleted(), false);

		m_pCharacter->UpdateAllSceneNodes();

		CHECK_EQUAL(cp->IsLoadingCompleted(), true);

		CHECK(m_pCharacter->GetActorNode("1hd_03") != NULL);
	}


	TEST_FIXTURE(FSerializeFixture, TestMyPlayerSerialize_CheckSword)
	{
		USES_CONVERSION_EX;

		XItem* pWeaponItem1 = info.item->MakeNewItem(1);
		XItem* pWeaponItem2 = info.item->MakeNewItem(3);
		XItem* pShield = info.item->MakeNewItem(2);

		// 무기 장착
		m_myEquipmentSlot.EquipItem(pWeaponItem1,	ITEMSLOT_RWEAPON);
		m_myEquipmentSlot.EquipItem(pWeaponItem2,		ITEMSLOT_RWEAPON2);

		m_pMyPlayerSerializeStrategy->Serialize(m_PlayerInfo, &m_myEquipmentSlot);
		m_pCharacter->UpdateAllSceneNodes();

		CHECK(m_pCharacter->GetActorNode("1hd_03") != NULL);
		CHECK(m_pCharacter->GetActorNode("1hd_03_b") != NULL);
		CHECK(m_pCharacter->GetActorNode("1hd_03_w") != NULL);
		CHECK(m_pCharacter->GetActorNode(W2A_EX(SWORDTRAIL_START_NODE_NAME, 0)) != NULL);
		CHECK(m_pCharacter->GetActorNode(W2A_EX(SWORDTRAIL_END_NODE_NAME, 0)) != NULL);

		// weapon set 변경 -> 1
		m_myEquipmentSlot.SwitchingWeaponSet(1);

		m_pMyPlayerSerializeStrategy->Serialize(m_PlayerInfo, &m_myEquipmentSlot);
		m_pCharacter->UpdateAllSceneNodes();

		CHECK(m_pCharacter->GetActorNode("1hd_03") != NULL);
		CHECK(m_pCharacter->GetActorNode("1hd_03_b") != NULL);
		CHECK(m_pCharacter->GetActorNode("1hd_03_w") != NULL);
		CHECK(m_pCharacter->GetActorNode(W2A_EX(SWORDTRAIL_START_NODE_NAME, 0)) != NULL);
		CHECK(m_pCharacter->GetActorNode(W2A_EX(SWORDTRAIL_END_NODE_NAME, 0)) != NULL);
		CHECK(m_pCharacter->GetActorNode("shield_01") == NULL);
		CHECK(m_pCharacter->GetActorNode("shield_01_b") == NULL);
		CHECK(m_pCharacter->GetActorNode("shield_01_w") == NULL);


		// 방패 장착
		m_myEquipmentSlot.EquipItem(pShield,		ITEMSLOT_LWEAPON2);

		m_pMyPlayerSerializeStrategy->Serialize(m_PlayerInfo, &m_myEquipmentSlot);
		m_pCharacter->UpdateAllSceneNodes();

		CHECK(m_pCharacter->GetActorNode("1hd_03") != NULL);
		CHECK(m_pCharacter->GetActorNode("1hd_03_b") != NULL);
		CHECK(m_pCharacter->GetActorNode("1hd_03_w") != NULL);
		CHECK(m_pCharacter->GetActorNode(W2A_EX(SWORDTRAIL_START_NODE_NAME, 0)) != NULL);
		CHECK(m_pCharacter->GetActorNode(W2A_EX(SWORDTRAIL_END_NODE_NAME, 0)) != NULL);
		CHECK(m_pCharacter->GetActorNode("shield_01") != NULL);
		CHECK(m_pCharacter->GetActorNode("shield_01_b") != NULL);
		CHECK(m_pCharacter->GetActorNode("shield_01_w") != NULL);

		//m_pCharacter->DumpSceneNode("char.xml");
	}


	TEST_FIXTURE(FSerializeFixture, TestMyPlayerSerialize_CancelLoadingQueue)
	{
		XItem* pWeaponItem1 = info.item->MakeNewItem(1);
		XItem* pWeaponItem2 = info.item->MakeNewItem(3);
		XItem* pShield = info.item->MakeNewItem(2);

		// 무기 장착
		m_myEquipmentSlot.EquipItem(pWeaponItem1,	ITEMSLOT_RWEAPON);
		m_myEquipmentSlot.EquipItem(pWeaponItem2,		ITEMSLOT_RWEAPON2);
		m_myEquipmentSlot.EquipItem(pShield,		ITEMSLOT_LWEAPON2);


		m_pMyPlayerSerializeStrategy->Serialize(m_PlayerInfo, &m_myEquipmentSlot);
		m_pCharacter->UpdateAllSceneNodes();

		// weapon set 변경 -> 1
		m_myEquipmentSlot.SwitchingWeaponSet(1);

		m_pMyPlayerSerializeStrategy->Serialize(m_PlayerInfo, &m_myEquipmentSlot);

		// 바로 다시 weapon set 변경 -> 0
		m_myEquipmentSlot.SwitchingWeaponSet(0);
		m_pMyPlayerSerializeStrategy->Serialize(m_PlayerInfo, &m_myEquipmentSlot);

		m_pCharacter->UpdateAllSceneNodes();

		// 방패를 착용했다가 다시 방패를 없앴으므로, CharacterParts에서 LoadingQueue에 있어도 취소되어야 한다.
		CHECK(m_pCharacter->GetActorNode("shield_01") == NULL);

	}

}