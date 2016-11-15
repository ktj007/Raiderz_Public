#include "stdafx.h"
#include "GPlayerObject.h"
#include "GFieldInfoMgr.h"
#include "MockDBManager.h"
#include "GTestWrapper.h"
#include "GUTHelper.h"
#include "GCommandTable.h"
#include "MockLink.h"
#include "MockField.h"
#include "GItemManager.h"
#include "GPlayerTalent.h"
#include "GPlayerObjectManager.h"
#include "GFieldMgr.h"
#include "GDBTaskCharSerialize.h"
#include "GTalentInfoMgr.h"
#include "FBaseFieldInfo.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"
#include "GItemHolder.h"
#include "GDBTaskCharData.h"
#include "GTestForward.h"

SUITE(PlayerObject)
{
	class MockDBManagerForPlayerObject : public MockDBManager
	{
	public:
		virtual bool GetCharInfo(const int nCID, GEntityPlayer* pPlayer, int& nBlankTime) 
		{ 
			PLAYER_INFO* pPlayerInfo = pPlayer->GetPlayerInfo();

			pPlayerInfo->nCID = 1;

			pPlayerInfo->nSex = SEX_FEMALE;
			pPlayerInfo->nFeatureHair = 1;
			pPlayerInfo->nFeatureFace = 1;
			pPlayerInfo->nLevel = 1;
			pPlayerInfo->nXP = 0;
			pPlayerInfo->SetMoney(1);

			pPlayer->GetTalent().SetTP(2);

			wcscpy_s(pPlayerInfo->szName, L"test");

			return true; 
		}
	};

	struct FPlayerObject : public FBaseFieldInfo, FBaseMockLink
	{
		FPlayerObject()
		{
			// 필드 등록
			GFieldInfo* pFieldInfo = AddFieldInfo();
			pFieldInfo->m_strFieldFile = L"test";
			MARKER_INFO* pMarkerInfo = GUTHelper_Field::InsertNewMarkerInfo(pFieldInfo);
			pMarkerInfo->vDir = vec3(0, 1, 0);
			pMarkerInfo->vPoint = vec3(0, 0, 0);


			// 아이템 등록
			GItemData* pItemData = helper.NewItemData(ITEMSLOT_RWEAPON, 1);
			pItemData->m_ItemType = ITEMTYPE_WEAPON;
			pItemData->m_nWeaponType = WEAPON_1H_SLASH;

			m_pPlayerObject = gmgr.pPlayerObjectManager->NewPlayerObject(SUnitTestUtil::NewUID());
			gmgr.pPlayerObjectManager->AddPlayer(m_pPlayerObject);

			m_pPlayerObject->GetAccountInfo().nAID = 1;

			m_pLink = NewLink(m_pPlayerObject->GetEntity());
		}
		~FPlayerObject()
		{
			gmgr.pPlayerObjectManager->DeleteAllPlayer();

			gmgr.pFieldMgr->Clear();
		}

		GTestMgrWrapper<GItemManager>	m_ItemManager;
		GPlayerObject* m_pPlayerObject;
		MockLink* m_pLink;
		GUTHelper helper;
	};


	class MockDBManagerForPlayerObjectWeaponSetTest : public MockDBManagerForPlayerObject
	{
	public:
		virtual bool GetCharInfo(const int nCID, GEntityPlayer* pPlayer, int& nBlankTime) 
		{ 
			MockDBManagerForPlayerObject::GetCharInfo(nCID, pPlayer, nBlankTime);

			// weaponset이 secondary이다
			pPlayer->GetPlayerInfo()->m_nWeaponSet = ITEMSWITCH_SECONDARY;


			return true; 
		}
	};


	class MockDBManagerForSerializeCharacter : public MockDBManager
	{
	public :
		class MockDBTaskSerializeSelectCharacter : public GDBTaskCharSerialize
		{
		public :
			friend class MockDBManagerForSerializeCharacter;

			MockDBTaskSerializeSelectCharacter(const MUID& uidReqPlayer) : GDBTaskCharSerialize(uidReqPlayer) {}

			using GDBTaskCharSerialize::m_Result;
		};

		
		virtual bool CharSerialize(const GDBT_CHAR_SERIALIZE& data)
		{
			MockDBTaskSerializeSelectCharacter Task(data.m_uidPlayer);

			Task.Input(data.m_nCID, data.m_nUIID, data.m_bReload);
			Task.m_Result = SDBTR_SUCCESS;

			InitTestTalent(Task);
			SetTestCharInfo(Task);
			SetTestEquipSlot(Task);
			SetTestInvenItem(Task);
			SetTestPalette(Task);

			Task.IncreaseRefCount(data.m_uidPlayer);
			Task.OnCompleted();

			return true;
		}

		void InitTestTalent(MockDBTaskSerializeSelectCharacter& Task) 
		{
			CSTalentInfo* pTInfo = new CSTalentInfo;
			pTInfo->m_nID = 6;
			gmgr.pTalentInfoMgr->Insert(pTInfo);

			Task.m_Data.qTalent.push_back(6);
		}

		void SetTestCharInfo(MockDBTaskSerializeSelectCharacter& Task)
		{
			memset(&Task.m_Data.CharInfo, 0, sizeof(Task.m_Data.CharInfo));

			Task.m_Data.CharInfo.nCID = Task.m_Data.nCID;
		}

		void SetTestEquipSlot(MockDBTaskSerializeSelectCharacter& Task)
		{
			GDBT_ITEMINSTANCE ii;
			
			ii.nItemID				= test.item.NewItemData()->m_nID;
			ii.nIUID				= 1;
			ii.nSlotType			= SLOTTYPE_EQUIP;
			ii.nSlotID				= 1;

			Task.m_Data.qItemInst.push_back(ii);
		}

		void SetTestInvenItem(MockDBTaskSerializeSelectCharacter& Task)
		{
			GDBT_ITEMINSTANCE ii;

			ii.nItemID				= test.item.NewItemData()->m_nID;
			ii.nIUID				= 2;
			ii.nSlotType			= SLOTTYPE_INVENTORY;
			ii.nSlotID				= 1;

			Task.m_Data.qItemInst.push_back(ii);
		}

		void SetTestPalette(MockDBTaskSerializeSelectCharacter& Task)
		{
			GDBT_PALETTE pl;
			pl.nID = 6;
			pl.nOrderID = 22;
			pl.nType = PIT_TALENT;
			Task.m_Data.qPalette.push_back(pl);
		}
	};

	TEST_FIXTURE(FPlayerObject, TestSendSerializeSelectCharacterCommand)
	{
		GTestSysWrapper2<GDBManager, MockDBManagerForSerializeCharacter> m_DBManager;
		GDBT_CHAR_SERIALIZE data(m_pPlayerObject->GetAccountInfo().nAID
			, m_pPlayerObject->GetUID()
			, m_pPlayerObject->GetUIID()
			, 2
			, false);
		m_DBManager.Get()->CharSerialize(data);

		GEntityPlayer* pEntityPlayer = m_pPlayerObject->GetEntity();
		GItemHolder* pItemHolder = pEntityPlayer->GetItemHolder();

		CHECK_EQUAL(MC_COMM_RESPONSE_LOGIN_GAME_SERVER, m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(2, pEntityPlayer->GetPlayerInfo()->nCID);
		CHECK_EQUAL(1, pItemHolder->GetItemCount(SLOTTYPE_EQUIP));
		CHECK_EQUAL(1, pItemHolder->GetItemCount(SLOTTYPE_INVENTORY));

		{
			GItem* pItem = pItemHolder->GetItem(SLOTTYPE_EQUIP, 1);
			CHECK(NULL != pItem);
		}		

		{
			GItem* pItem = pItemHolder->GetItem(SLOTTYPE_INVENTORY, 1);
			CHECK(NULL != pItem);
		}
	}
}
