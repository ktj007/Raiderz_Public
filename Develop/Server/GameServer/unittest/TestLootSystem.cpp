#include "stdafx.h"
#include "GLootSystem.h"
#include "MockPartySystem.h"
#include "GLootInfoMgr.h"
#include "GLootInfo.h"
#include "GNPCInfoMgr.h"
#include "GNPCMgr.h"
#include "MockEntityNPC.h"
#include "MockEntityPlayer.h"
#include "SUnitTestUtil.h"
#include "MockField.h"
#include "MockDBManager.h"
#include "GItemManager.h"
#include "TestItemManager.h"
#include "MockPlayerObject.h"
#include "GGlobal.h"
#include "GTestWrapper.h"
#include "GLootInfo.h"
#include "GMessageID.h"
#include "GPlayerObjectManager.h"
#include "GQuestInfoMgr.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GUTHelper_Party.h"
#include "GParty.h"
#include "GInteractionSystem.h"
#include "GPlayerDoing.h"
#include "GDropItems.h"
#include "GDropSilver.h"
#include "GTestForward.h"
#include "GHateTable.h"
#include "GPlayerLoot.h"
#include "GItemHolder.h"
#include "GDropItem.h"
#include "GConfig.h"
#include "FBaseNetClient.h"
#include "CCommandTable_Master.h"
#include "GNPCRewardOwner.h"
#include "CCommandResultTable.h"
#include "GLootBeginner.h"
#include "GLootShower.h"
#include "GLootLooter.h"
#include "GLootMasterLooter.h"
#include "GLootEnder.h"
#include "GLootModifyHandler.h"
#include "GLootItemGetter.h"
#include "GLootItemGetter.h"
#include "GLootSystemSetterForTest.h"

using testing::_;
using testing::Return;
using testing::AnyOf;
using testing::Eq;
using testing::AtLeast;

class MockLootingNPC: public GEntityNPC
{
public:
	virtual ~MockLootingNPC()	{ Destroy();	}
	
	MOCK_METHOD2(doDieByGather, void(GEntityPlayer* pPlayer, int nLootID));
	MOCK_METHOD1(doDespawn,		void(bool bSpawnEnable));
};

SUITE(LootSystem)
{
	struct FLootSystem : public FBaseNetClient
	{
		FLootSystem()
		{
			m_pNetAgent->IgnoreCmd(MMC_PLAYER_IN_FIELD_NOTIFY);
			m_pNetAgent->IgnoreCmd(MMC_PARTY_DO_OFFLINE);

			m_pField = GUTHelper_Field::DefaultMockField();

			m_pPlayer = test.player.NewPlayer(m_pField);			
			m_pMember = test.player.NewPlayer(m_pField);

			m_pNPC = test.npc.SpawnNPC(m_pField);

			m_pItemData = test.item.NewItemData();

			m_pLootInfo = test.loot.NewLootInfo();
			m_pLootItemInfo = test.loot.NewLootItemInfo(m_pLootInfo, 100, 10, 10, m_pItemData->m_nID);
			LOOT_MONEY_INFO* pLootMoneyInfo = test.loot.NewLootMoneyInfo(m_pLootInfo, 100, 10, 10);			
			m_pNPC->GetNPCInfo()->m_pLootInfo = m_pLootInfo;
			m_pLootIEInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_LOOT_IMMORTAL, m_pLootInfo->m_nID);

			m_pParty = GUTHelper_Party::CreateParty(m_pPlayer, m_pMember);
			m_pParty->SetLootingRuleData(LOOTING_RULE_DATA(LRC_FREE_FOR_ALL, LRR_FREE_FOR_ALL, LRRF_RARE_OR_HIGHER));
		}

		virtual ~FLootSystem()
		{
			m_pField->Destroy();
		}

		void ProvideBeginByKillNPC()
		{
			m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
			m_pNPC->doDie();
		}

		void ProvideBeginByInteractNPC()
		{
			test.interaction.MakeInteractionState(m_pPlayer, m_pNPC, m_pLootIEInfo);
		}

		void ProvideShow()
		{
			m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
			m_pNPC->doDie();
			gsys.pLootSystem->GetBeginner().KillNPC(m_pPlayer, m_pNPC->GetUID());
		}

		void ProvideLootItem()
		{
			m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
			m_pNPC->doDie();
			gsys.pLootSystem->GetBeginner().KillNPC(m_pPlayer, m_pNPC->GetUID());
			gsys.pLootSystem->GetShower().Show(m_pPlayer, m_pNPC->GetUID());
		}

		void ProvideGetItem()
		{
			m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
			m_pNPC->doDie();
			gsys.pLootSystem->GetBeginner().KillNPC(m_pPlayer, m_pNPC->GetUID());
			gsys.pLootSystem->GetShower().Show(m_pPlayer, m_pNPC->GetUID());
		}

		void ProvidePrepareMasterLootItem()
		{
			m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);			
			m_pNPC->doDie();
			gsys.pLootSystem->GetBeginner().KillNPC(m_pPlayer, m_pNPC->GetUID());
			gsys.pLootSystem->GetShower().Show(m_pPlayer, m_pNPC->GetUID());
		}

		void ProvideMasterLootItem()
		{
			m_pParty->SetLootingRuleData(LOOTING_RULE_DATA(LRC_MASTER_LOOT, LRR_MASTER_LOOT, LRRF_RARE_OR_HIGHER));

			m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
			m_pNPC->doDie();
			gsys.pLootSystem->GetBeginner().KillNPC(m_pPlayer, m_pNPC->GetUID());
			gsys.pLootSystem->GetShower().Show(m_pPlayer, m_pNPC->GetUID());
		}

		void ProvideRollItem()
		{
			m_pItemData->m_nTier = ITEM_TIER_TREASURE;
			m_pParty->SetLootingRuleData(LOOTING_RULE_DATA(LRC_MASTER_LOOT, LRR_ROLL_DICE, LRRF_RARE_OR_HIGHER));

			m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
			m_pNPC->doDie();
			gsys.pLootSystem->GetBeginner().KillNPC(m_pPlayer, m_pNPC->GetUID());
			gsys.pLootSystem->GetShower().Show(m_pPlayer, m_pNPC->GetUID());
		}

		void ProvideEnd_StartByKill()
		{
			m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
			m_pNPC->doDie();
			gsys.pLootSystem->GetBeginner().KillNPC(m_pPlayer, m_pNPC->GetUID());
			gsys.pLootSystem->GetShower().Show(m_pPlayer, m_pNPC->GetUID());
		}

		void ProvideEnd_StartByInteract()
		{
			test.interaction.MakeInteractionState(m_pPlayer, m_pNPC, m_pLootIEInfo);
		}

		MockLootingNPC* NewMockLootingNPC(GATHER_TYPE gatherType)
		{
			MUID uidNPC = GUnitTestUtil::NewUID();
			GNPCInfo* pNPCInfo = m_Helper.NewNPCInfo();
			pNPCInfo->nNpcType = NPC_TYPE_GATHER;
			pNPCInfo->nGatherType = gatherType;			

			MockLootingNPC* pNPC = new MockLootingNPC;
			pNPC->Create(uidNPC);
			pNPC->InitFromNPCInfo(pNPCInfo, NULL);
			pNPC->EnterToField(m_pField, vec3(0.0f, 0.0f, 0.0f));

			return pNPC;
		}


		GUTHelper			m_Helper;
		MockField*			m_pField;
		GEntityPlayer*		m_pPlayer;
		GEntityPlayer*		m_pMember;
		GEntityNPC*			m_pNPC;
		GItemData*			m_pItemData;
		GLootInfo*			m_pLootInfo;
		LOOT_ITEM_INFO*		m_pLootItemInfo;
		GParty*				m_pParty;
		GNPCInteractionElementInfo*	m_pLootIEInfo;

		GTestSysWrapper<GLootSystem>	m_LootSystem;
	};

	// 이미 다른 플레이어가 인터랙션 루팅중인 NPC에게 인터랙션 루팅을 시도한 경우
	TEST_FIXTURE(FLootSystem, BeginByInteractNPC_AlreadyExistLootingPlayer)
	{
		ProvideBeginByInteractNPC();

		GEntityPlayer* pInteractPlayer = test.player.NewPlayer(m_pField);
		m_pNPC->GetNPCLoot().Begin(pInteractPlayer->GetCID());
		
		CHECK_EQUAL(false, gsys.pLootSystem->GetBeginner().InteractNPC(m_pPlayer, m_pNPC, m_pLootInfo->m_nID, true));
	}

	TEST_FIXTURE(FLootSystem, BeginByInteractNPC)
	{
		ProvideBeginByInteractNPC();

		MockLink* pLink = test.network.NewLink(m_pPlayer);
		pLink->AddIgnoreID(MC_NPCINTERACTION_END);

		CHECK_EQUAL(true, gsys.pLootSystem->GetBeginner().InteractNPC(m_pPlayer, m_pNPC, m_pLootInfo->m_nID, true));
		CHECK_EQUAL(true, m_pPlayer->GetPlayerLoot().IsLootingNPC(m_pNPC->GetUID()));
		CHECK_EQUAL(true, m_pNPC->GetNPCLoot().IsLootingPlayer(m_pPlayer->GetCID()));
		CHECK_EQUAL(true, m_pNPC->GetNPCLoot().GetDropList().IsDropByInteract_Immortal());

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_LOOT_BEGIN, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pPlayer->GetUID(), pLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(m_pNPC->GetUID(), pLink->GetParam<MUID>(0, 1));
	}	

	// 루팅중이지 않은 플레이어가 아이템을 보려 한경우
	TEST_FIXTURE(FLootSystem, Show_NotLootingPlayer)
	{
		ProvideShow();

		m_pPlayer->GetDoing().ChangeDoing(CD_NORMAL);
		CHECK_EQUAL(false, gsys.pLootSystem->GetShower().Show(m_pPlayer, m_pNPC->GetUID()));
	}

	// 볼수 있는NPC의 아이템이 없는 경우
	TEST_FIXTURE(FLootSystem, Show_Empty)
	{
		ProvideShow();
		m_pNPC->GetNPCLoot().GetDropList().Clear();

		CHECK_EQUAL(false, gsys.pLootSystem->GetShower().Show(m_pPlayer, m_pNPC->GetUID()));
	}

	// 너무 먼 거리에 있는 NPC의 아이템을 보려한경우
	TEST_FIXTURE(FLootSystem, Show_FarDistance)
	{
		ProvideShow();

		test.interaction.SetUninteractableDistacne(m_pNPC, m_pPlayer);
		CHECK_EQUAL(false, gsys.pLootSystem->GetShower().Show(m_pPlayer, m_pNPC->GetUID()));
	}

	// 획득 권한이 없는 NPC의 아이템을 보려한 경우
	TEST_FIXTURE(FLootSystem, Show_NotAuthorizedCID)
	{		
		ProvideShow();

		GEntityPlayer* pNotBeneficiary = test.player.NewPlayer(m_pField);
		pNotBeneficiary->GetDoing().ChangeDoing(CD_LOOTING);

		CHECK_EQUAL(false, gsys.pLootSystem->GetShower().Show(pNotBeneficiary, m_pNPC->GetUID()));
	}	

	TEST_FIXTURE(FLootSystem, Show)
	{
		ProvideShow();

		int nBeforeLooterMoney = m_pPlayer->GetPlayerInfo()->GetMoney();
		int nBeforeMemberMoney = m_pMember->GetPlayerInfo()->GetMoney();
		int nMemberCount = m_pParty->GetMemberCount();
		int nDistributedMoney = m_pNPC->GetNPCLoot().GetDropList().GetDropMoney().GetQuantity() / nMemberCount;

		vector<int> vecAuthorizedCID;
		vecAuthorizedCID.push_back(m_pPlayer->GetCID());
		vecAuthorizedCID.push_back(m_pMember->GetCID());

		class MockModifyHandler : public GLootModifyHandler
		{
		public:
			MOCK_METHOD3(HandleUnviewablePlayer,	void(set<int> setPreViewableCID, set<int> setPostViewableCID, GEntityNPC* pNPC));			
			MOCK_METHOD1(HandleEmptyNPC,		void(GEntityNPC* pNPC));
		};

		MockModifyHandler* pMockModifyHandler = new MockModifyHandler;
		gsys.pLootSystem->GetSetterForTest().SetModifyHandler(pMockModifyHandler);

		set<int> setPreViewableCID;
		set<int> setPostViewableCID;
		setPreViewableCID.insert(m_pPlayer->GetCID());
		setPreViewableCID.insert(m_pMember->GetCID());
		setPostViewableCID = setPreViewableCID;		
		EXPECT_CALL(*pMockModifyHandler, HandleUnviewablePlayer(setPreViewableCID, setPostViewableCID, m_pNPC));
		EXPECT_CALL(*pMockModifyHandler, HandleEmptyNPC(m_pNPC));

		MockLink* pLink = test.network.NewLink(m_pPlayer);
		pLink->AddIgnoreID(MC_CHAR_UPDATE_MONEY);

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());

 		CHECK_EQUAL(true, gsys.pLootSystem->GetShower().Show(m_pPlayer, m_pNPC->GetUID()));

		// 실버 획득 체크
		CHECK_EQUAL(nBeforeLooterMoney + nDistributedMoney, m_pPlayer->GetPlayerInfo()->GetMoney());
		CHECK_EQUAL(nBeforeMemberMoney + nDistributedMoney, m_pMember->GetPlayerInfo()->GetMoney());
		CHECK_EQUAL(0, m_pNPC->GetNPCLoot().GetDropList().GetDropMoney().GetQuantity());

		// Route 체크
		CHECK_EQUAL(1, pLink->GetCommandCount());
		ASSERT_EQUAL(MC_LOOT_SEE_ITEM, pLink->GetCommand(0).GetID());

		vector<TD_ITEM_DROP> vecTDDropItem;
		ASSERT_CHECK(pLink->GetCommand(0).GetBlob(vecTDDropItem, 0));

		CHECK(vecDropItem.front()->GetUID() == vecTDDropItem.front().m_nDropItemUID);
		CHECK(vecDropItem.front()->GetType() == vecTDDropItem.front().m_nDropItemType);
		CHECK(vecDropItem.front()->GetID() == vecTDDropItem.front().m_nItemID);
		CHECK(vecDropItem.front()->GetAmount() == vecTDDropItem.front().m_nAmount);
	}	


	TEST_FIXTURE(FLootSystem, LootItem_NotNowLooting)
	{
		ProvideLootItem();

		m_pPlayer->GetDoing().ChangeDoing(CD_NORMAL);

		vector<MUID> vecDropItemUID = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemUIDByCID(m_pPlayer->GetCID());

		CHECK_EQUAL(false, gsys.pLootSystem->GetLooter().LootItem(m_pPlayer, m_pNPC->GetUID(), vecDropItemUID));
	}

	TEST_FIXTURE(FLootSystem, LootItem_FarDistance)
	{
		ProvideLootItem();

		test.interaction.SetUninteractableDistacne(m_pNPC, m_pPlayer);

		vector<MUID> vecDropItemUID = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemUIDByCID(m_pPlayer->GetCID());

		CHECK_EQUAL(false, gsys.pLootSystem->GetLooter().LootItem(m_pPlayer, m_pNPC->GetUID(), vecDropItemUID));
	}

	TEST_FIXTURE(FLootSystem, LootItem_UnVisibleItem)
	{
		ProvideLootItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());
		vecDropItem[0]->Unvisible();

		vector<MUID> vecDropItemUID;
		vecDropItemUID.push_back(vecDropItem[0]->GetUID());

		CHECK_EQUAL(false, gsys.pLootSystem->GetLooter().LootItem(m_pPlayer, m_pNPC->GetUID(), vecDropItemUID));
	}

	TEST_FIXTURE(FLootSystem, LootItem_NotAuthorizedPlayer)
	{
		ProvideLootItem();

		GEntityPlayer* pNotBeneficiary = test.player.NewPlayer(m_pField);
		pNotBeneficiary->GetDoing().ChangeDoing(CD_LOOTING);

		vector<MUID> vecDropItemUID = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemUIDByCID(m_pPlayer->GetCID());

		CHECK_EQUAL(false, gsys.pLootSystem->GetLooter().LootItem(pNotBeneficiary, m_pNPC->GetUID(), vecDropItemUID));
	}

	TEST_FIXTURE(FLootSystem, LootItem)
	{	
		LOOT_ITEM_INFO* pLootItemInfo2 = test.loot.NewLootItemInfo(m_pLootInfo, 100, 10, 10, m_pItemData->m_nID);
		LOOT_ITEM_INFO* pLootItemInfo3 = test.loot.NewLootItemInfo(m_pLootInfo, 100, 10, 10, m_pItemData->m_nID);
		ProvideLootItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());

		// 테스트를 위해 차례대로 루팅룰로 드랍된 아이템을 루터루팅룰과 주사위루팅룰로 드랍된것 처럼 수정
		GDropItem* pGettableItem = vecDropItem[0];
		GDropItem* pMasterLootableItem = vecDropItem[1];
		pMasterLootableItem->SetMasterLootable();
		pMasterLootableItem->InsertAuthorizedCID(m_pMember->GetCID());
		GDropItem* pRollableItem = vecDropItem[2];		
		pRollableItem->SetRollable();
		pRollableItem->InsertAuthorizedCID(m_pMember->GetCID());	

		vector<MUID> vecDropItemUID;
		for each (GDropItem* pDropItem in vecDropItem)
		{
			vecDropItemUID.push_back(pDropItem->GetUID());
		}

		class MockLooter : public GLootLooter
		{
		public:


			MOCK_METHOD3(PrepareMasterLootItem, void(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem));			
			MOCK_METHOD3(RollItem, void(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem));
		};

		class MockItemGetter : public GLootItemGetter
		{
		public:
			MOCK_METHOD3(GetItem, UNGETTABLE_ITEM_REASON(int nCID, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem));
		};
		
		class MockModifyHandler : public GLootModifyHandler
		{
		public:
			MOCK_METHOD2(HandleDropListModification, void(GEntityNPC* pNPC, set<int> setPreViewableCID));
		};	
		
		MockLooter* pMockLooter = new MockLooter;
		MockItemGetter* pMockItemGetter = new MockItemGetter;		
		MockModifyHandler* pMockModifyHandler = new MockModifyHandler;
		gsys.pLootSystem->GetSetterForTest().SetLooter(pMockLooter);
		gsys.pLootSystem->GetSetterForTest().SetItemGetter(pMockItemGetter);		
		gsys.pLootSystem->GetSetterForTest().SetModifyHandler(pMockModifyHandler);

		set<int> setPreViewableCID;
		setPreViewableCID.insert(m_pPlayer->GetCID());
		setPreViewableCID.insert(m_pMember->GetCID());
		vector<GDropItem*> vecGettableItem;
		vecGettableItem.push_back(pGettableItem);
		vector<GDropItem*> vecMasterLootableItem;
		vecMasterLootableItem.push_back(pMasterLootableItem);
		vector<GDropItem*> vecRollableItem;
		vecRollableItem.push_back(pRollableItem);
		
		EXPECT_CALL(*pMockItemGetter, GetItem(m_pPlayer->GetCID(), m_pNPC, vecGettableItem)).WillRepeatedly(Return(UIR_NONE)); ;
		EXPECT_CALL(*pMockLooter, PrepareMasterLootItem(m_pPlayer, m_pNPC, vecMasterLootableItem));
		EXPECT_CALL(*pMockLooter, RollItem(m_pPlayer, m_pNPC, vecRollableItem));		
		EXPECT_CALL(*pMockModifyHandler, HandleDropListModification(m_pNPC, setPreViewableCID));
			
		CHECK_EQUAL(true, gsys.pLootSystem->GetLooter().LootItem(m_pPlayer, m_pNPC->GetUID(), vecDropItemUID));		
	}

	TEST_FIXTURE(FLootSystem, GetItem)
	{
		ProvideGetItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());
		GDropItem* pGettableItem = vecDropItem.front();
		int nGettableItemID = pGettableItem->GetID();
		int nGettableItemAmount = pGettableItem->GetAmount();

		// 다른 서버에 있는 보상자 설정
		int nOtherServerBeneficiaryCID = GUnitTestUtil::NewCID();
		pGettableItem->InsertAuthorizedCID(nOtherServerBeneficiaryCID);
		m_pNPC->GetNPCLoot().GetDropList().InsertBeneficiaryCID(nOtherServerBeneficiaryCID);

		MockLink* pPlayerLink = test.network.NewLink(m_pPlayer);
		MockLink* pMemberLink = test.network.NewLink(m_pMember);

		gsys.pLootSystem->GetItemGetter().GetItem(m_pPlayer->GetCID(), m_pNPC, vecDropItem);

		CHECK_EQUAL(true, m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID()).empty());

		CHECK_EQUAL(nGettableItemAmount, m_pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, nGettableItemID));
		CHECK_EQUAL(1, pPlayerLink->GetCommandCount());
		CHECK_EQUAL(MC_ITEM_ADD, pPlayerLink->GetCommand(0).GetID());
				
		// 서버에 있는 보상자들에게 아이템 획득 전달
		{
			CHECK_EQUAL(1, pMemberLink->GetCommandCount());
			CHECK_EQUAL(MC_LOOT_OTHER_GAIN_ITEM, pMemberLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pPlayer->GetUID(), pMemberLink->GetParam<MUID>(0, 0));
			vector<TD_LOOT_OTHERGAIN_ITEM> vecTDOtherGainItem;
			ASSERT_CHECK(pMemberLink->GetCommand(0).GetBlob(vecTDOtherGainItem, 1));
			CHECK_EQUAL(nGettableItemID,  vecTDOtherGainItem.front().m_nItemID);
			CHECK_EQUAL(nGettableItemAmount,  vecTDOtherGainItem.front().m_nAmount);
		}		

		// 서버에 없는 보상자들에게 마스터서버를 통해 아이템 획득 전달
		{
			CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());

			vector<int> vecOtherServerBeneficiaryCID;
			vector<TD_LOOT_OTHERGAIN_ITEM>	vecTDOtherGainItem;
			ASSERT_EQUAL(MMC_LOOT_OTHER_GAIN_ITEM_REQ, m_pNetAgent->GetSendCommandID(0));
			m_pNetAgent->GetSendCommand(0).GetBlob(vecOtherServerBeneficiaryCID, 0);
			CHECK_EQUAL(nOtherServerBeneficiaryCID,		vecOtherServerBeneficiaryCID.front());

			CHECK_EQUAL(m_pPlayer->GetUID(), m_pNetAgent->GetSendParam<MUID>(0, 1));

			m_pNetAgent->GetSendCommand(0).GetBlob(vecTDOtherGainItem, 2);
			CHECK_EQUAL(nGettableItemID,	vecTDOtherGainItem.front().m_nItemID);
			CHECK_EQUAL(nGettableItemAmount,vecTDOtherGainItem.front().m_nAmount);
		}
	}

	TEST_FIXTURE(FLootSystem, RollItem_RouteForOtherServer)
	{
		ProvideRollItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());
		GDropItem* pRollabeItem = vecDropItem.front();
		int nItemID = pRollabeItem->GetID();
		int nAmount = pRollabeItem->GetAmount();

		// 다른 서버에 있는 보상자 설정
		int nOtherServerBeneficiaryCID = GUnitTestUtil::NewCID();
		pRollabeItem->InsertAuthorizedCID(nOtherServerBeneficiaryCID);
		m_pNPC->GetNPCLoot().GetDropList().InsertBeneficiaryCID(nOtherServerBeneficiaryCID);

		m_pNetAgent->IgnoreCmd(MMC_LOOT_OTHER_GAIN_ITEM_REQ);
		m_pNetAgent->IgnoreCmd(MMC_LOOT_GETTABLE_ITEM_ADD_REQ);

		gsys.pLootSystem->GetLooter().RollItem(m_pPlayer, m_pNPC, vecDropItem);

		// 서버에 없는 보상자들에게 마스터서버를 통해 RollResult 전달
		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		ASSERT_EQUAL(MMC_LOOT_ROLLRESULT_REQ, m_pNetAgent->GetSendCommandID(0));

		vector<int> vecOtherServerBeneficiaryCID;
		m_pNetAgent->GetSendCommand(0).GetBlob(vecOtherServerBeneficiaryCID, 0);
		CHECK_EQUAL(nOtherServerBeneficiaryCID,	vecOtherServerBeneficiaryCID.front());

		vector<TD_LOOT_ROLL_ITEM> vecTDRollItem;
		m_pNetAgent->GetSendCommand(0).GetBlob(vecTDRollItem, 1);
		CHECK_EQUAL(nItemID,	vecTDRollItem.front().m_nItemID);
		CHECK_EQUAL(nAmount,	vecTDRollItem.front().m_nItemAmount);

		vector<TD_LOOT_ROLL_RESULT> vecTDRollResult;
		m_pNetAgent->GetSendCommand(0).GetBlob(vecTDRollResult, 2);
		for (vector<TD_LOOT_ROLL_RESULT>::iterator itor=vecTDRollResult.begin(); itor!=vecTDRollResult.end(); itor++)
		{
			CHECK(m_pPlayer->GetUID() == itor->m_nPlayerUID || 
				m_pMember->GetUID() == itor->m_nPlayerUID || 
				MUID::ZERO == itor->m_nPlayerUID);
		}
	}

	TEST_FIXTURE(FLootSystem, RollItem)
	{
		ProvideRollItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());
		GDropItem* pRollabeItem = vecDropItem.front();

		class MockItemGetter : public GLootItemGetter
		{
		public:			
			MOCK_METHOD3(GetItem, UNGETTABLE_ITEM_REASON(int nCID, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem));
		};

		MockItemGetter* pMockItemGetter = new MockItemGetter;
		gsys.pLootSystem->GetSetterForTest().SetItemGetter(pMockItemGetter);

		EXPECT_CALL(*pMockItemGetter, GetItem(AnyOf(m_pPlayer->GetCID(), m_pMember->GetCID()), m_pNPC, vecDropItem)).WillRepeatedly(Return(UIR_NONE));		

		MockLink* pPlayerLink = test.network.NewLink(m_pPlayer);
		MockLink* pMemberLink = test.network.NewLink(m_pMember);

		gsys.pLootSystem->GetLooter().RollItem(m_pPlayer, m_pNPC, vecDropItem);
		
		CHECK_EQUAL(1, pPlayerLink->GetCommandCount());		
		CHECK_EQUAL(MC_LOOT_ROLLRESULT, pPlayerLink->GetCommand(0).GetID());
		vector<TD_LOOT_ROLL_ITEM> vecTDRollItem;
		vector<TD_LOOT_ROLL_RESULT> vecTDRollResult;
		pPlayerLink->GetCommand(0).GetBlob(vecTDRollItem, 0);
		pPlayerLink->GetCommand(0).GetBlob(vecTDRollResult, 1);

		for (vector<TD_LOOT_ROLL_ITEM>::iterator itor=vecTDRollItem.begin(); itor!=vecTDRollItem.end(); itor++)
		{
			CHECK_EQUAL(pRollabeItem->GetID(), itor->m_nItemID);
			CHECK_EQUAL(pRollabeItem->GetAmount(), itor->m_nItemAmount);
		}

		for (vector<TD_LOOT_ROLL_RESULT>::iterator itor=vecTDRollResult.begin(); itor!=vecTDRollResult.end(); itor++)
		{
			CHECK(m_pPlayer->GetUID() == itor->m_nPlayerUID || 
				m_pMember->GetUID() == itor->m_nPlayerUID ||
				MUID::ZERO == itor->m_nPlayerUID);
		}

		CHECK_EQUAL(1, pMemberLink->GetCommandCount());
		CHECK_EQUAL(MC_LOOT_ROLLRESULT, pMemberLink->GetCommand(0).GetID());
		// 똑같은 패킷을 보상자들에게 보내기때문에 member의 패킷내용 체크는 생략				
	}	

	// 마스터가 같은 필드에 없는 경우
	TEST_FIXTURE(FLootSystem, PrepareMasterLootItem_NotExistMasterInSameField)
	{
		ProvidePrepareMasterLootItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());
		GDropItem* pMasterLootableItem = vecDropItem.front();

		m_pPlayer->LeaveFromField();

		MockLink* pPlayerLink = test.network.NewLink(m_pPlayer);
		pPlayerLink->AddIgnoreID(MC_MSG_RES);
		MockLink* pMemberLink = test.network.NewLink(m_pMember);

		gsys.pLootSystem->GetLooter().PrepareMasterLootItem(m_pPlayer, m_pNPC, vecDropItem);

		CHECK_EQUAL(true, pMasterLootableItem->IsVisible());

		CHECK_EQUAL(0, pPlayerLink->GetCommandCount());
		CHECK_EQUAL(0, pMemberLink->GetCommandCount());
		CHECK_EQUAL(0, m_pNetAgent->GetSendCommandCount());
	}

	TEST_FIXTURE(FLootSystem, PrepareMasterLootItem)
	{
		ProvidePrepareMasterLootItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());
		GDropItem* pMasterLootableItem = vecDropItem.front();		

		// 다른 서버에 있는 보상자 설정
		int nOtherServerBeneficiaryCID = GUnitTestUtil::NewCID();
		pMasterLootableItem->InsertAuthorizedCID(nOtherServerBeneficiaryCID);
		m_pNPC->GetNPCLoot().GetDropList().InsertBeneficiaryCID(nOtherServerBeneficiaryCID);

		MockLink* pPlayerLink = test.network.NewLink(m_pPlayer);
		MockLink* pMemberLink = test.network.NewLink(m_pMember);

		gsys.pLootSystem->GetLooter().PrepareMasterLootItem(m_pPlayer, m_pNPC, vecDropItem);

		CHECK_EQUAL(false, pMasterLootableItem->IsVisible());

		// Master에게 MasterLootUI 전달
		{
			CHECK_EQUAL(1, pPlayerLink->GetCommandCount());

			vector<TD_LOOT_MASTERLOOT_UI> vecTDLootMasterLootUI;			
			ASSERT_EQUAL(MC_LOOT_MASTERLOOT_UI, pPlayerLink->GetCommand(0).GetID());
			CHECK_EQUAL(GConfig::m_nMyServerID, pPlayerLink->GetParam<int>(0, 0));
			CHECK_EQUAL(m_pNPC->GetFieldUID(), pPlayerLink->GetParam<MUID>(0, 1));
			CHECK_EQUAL(m_pNPC->GetUID(), pPlayerLink->GetParam<MUID>(0, 2));
			pPlayerLink->GetCommand(0).GetBlob(vecTDLootMasterLootUI, 3);
			CHECK_EQUAL(pMasterLootableItem->GetUID(), vecTDLootMasterLootUI[0].m_nDropItemUID);
			CHECK_EQUAL(pMasterLootableItem->GetID(), vecTDLootMasterLootUI[0].m_nItemID);
			CHECK_EQUAL(pMasterLootableItem->GetAmount(), vecTDLootMasterLootUI[0].m_nItemAmount);
		}

		// Master가 아닌 보상자에게 MasterLootNotify 전달
		{
			CHECK_EQUAL(1, pMemberLink->GetCommandCount());

			vector<TD_LOOT_MASTERLOOT_NOTIFY> vecTDLootMasterLootNotify;
			ASSERT_EQUAL(MC_LOOT_MASTERLOOT_NOTIFY, pMemberLink->GetCommand(0).GetID());
			pMemberLink->GetCommand(0).GetBlob(vecTDLootMasterLootNotify, 0);
			CHECK_EQUAL(pMasterLootableItem->GetID(), vecTDLootMasterLootNotify[0].m_nItemID);
			CHECK_EQUAL(pMasterLootableItem->GetAmount(), vecTDLootMasterLootNotify[0].m_nItemAmount);
		}

		// 서버에 없는 보상자들에게 마스터서버를 통해 MasterLootNotify 전달
		{
			CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());

			vector<int> vecOtherServerBeneficiaryCID;
			vector<TD_LOOT_MASTERLOOT_NOTIFY>	vecTDLootMasterLootNotify;
			ASSERT_EQUAL(MMC_LOOT_MASTERLOOT_NOTIFY_REQ, m_pNetAgent->GetSendCommandID(0));
			m_pNetAgent->GetSendCommand(0).GetBlob(vecOtherServerBeneficiaryCID, 0);
			CHECK_EQUAL(nOtherServerBeneficiaryCID,		vecOtherServerBeneficiaryCID.front());

			m_pNetAgent->GetSendCommand(0).GetBlob(vecTDLootMasterLootNotify, 1);
			CHECK_EQUAL(pMasterLootableItem->GetID(),		vecTDLootMasterLootNotify.front().m_nItemID);
			CHECK_EQUAL(pMasterLootableItem->GetAmount(),	vecTDLootMasterLootNotify.front().m_nItemAmount);
		}
	}

	// 수동 지급된 이유가 다른 서버에 있어서 일때
	TEST_FIXTURE(FLootSystem, HandleUngettableItem_OtherServer)
	{
		ProvideMasterLootItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());
		GDropItem* pDropItem = vecDropItem.front();

		int nPlayerCID = m_pPlayer->GetCID();
		gmgr.pPlayerObjectManager->DeletePlayer(m_pPlayer->GetUID());

		gsys.pLootSystem->GetItemGetter().HandleUngettableItem(UIR_OTHERSERVER, nPlayerCID, vecDropItem);

		CHECK_EQUAL(true, pDropItem->IsGettable());
		CHECK_EQUAL(true, pDropItem->IsAuthorizedCID(nPlayerCID));
		CHECK_EQUAL(false, pDropItem->IsAuthorizedCID(m_pMember->GetCID()));

		CHECK_EQUAL(1, m_pNetAgent->GetSendCommandCount());
		ASSERT_EQUAL(MMC_LOOT_GETTABLE_ITEM_ADD_REQ, m_pNetAgent->GetSendCommandID(0));

		vector<TD_LOOT_GETTABLE_ITEM_ADD> vecGettableItemAdd;
		m_pNetAgent->GetSendCommand(0).GetBlob(vecGettableItemAdd, 1);
		CHECK_EQUAL(pDropItem->GetID(),		vecGettableItemAdd.front().m_nItemID);
		CHECK_EQUAL(pDropItem->GetAmount(),	vecGettableItemAdd.front().m_nItemAmount);
		CHECK_EQUAL(UIR_OTHERSERVER,		vecGettableItemAdd.front().m_nCGR);
	}

	// 수동 지급된 이유가 다른 필드에 있어서 일때
	TEST_FIXTURE(FLootSystem, HandleUngettableItem_OtherField)
	{
		ProvideMasterLootItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());
		GDropItem* pDropItem = vecDropItem.front();
		
		m_pPlayer->LeaveFromField();

		MockLink* pPlayerMockLink = test.network.NewLink(m_pPlayer);

		gsys.pLootSystem->GetItemGetter().HandleUngettableItem(UIR_OTHERFIELD, m_pPlayer->GetCID(), vecDropItem);

		CHECK_EQUAL(true, pDropItem->IsGettable());
		CHECK_EQUAL(true, pDropItem->IsAuthorizedCID(m_pPlayer->GetCID()));
		CHECK_EQUAL(false, pDropItem->IsAuthorizedCID(m_pMember->GetCID()));

		CHECK_EQUAL(1, pPlayerMockLink->GetCommandCount());
		ASSERT_EQUAL(MC_LOOT_GETTABLE_ITEM_ADD, pPlayerMockLink->GetCommandID(0));

		vector<TD_LOOT_GETTABLE_ITEM_ADD> vecGettableItemAdd;
		pPlayerMockLink->GetCommand(0).GetBlob(vecGettableItemAdd, 0);
		CHECK_EQUAL(pDropItem->GetID(),		vecGettableItemAdd.front().m_nItemID);
		CHECK_EQUAL(pDropItem->GetAmount(),	vecGettableItemAdd.front().m_nItemAmount);
		CHECK_EQUAL(UIR_OTHERFIELD,			vecGettableItemAdd.front().m_nCGR);
	}

	TEST_FIXTURE(FLootSystem, MasterLootItem_NotMasterLootableDropItem)
	{
		ProvideMasterLootItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());
		vecDropItem[0]->SetGettable();

		CHECK_EQUAL(false, gsys.pLootSystem->GetMasterLooter().MasterLootItem(m_pPlayer, m_pNPC->GetUID(), vecDropItem[0]->GetUID(), 1, false));
	}

	TEST_FIXTURE(FLootSystem, MasterLootItem_RequesterIsNotLooter)
	{
		ProvideMasterLootItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());

		CHECK_EQUAL(false, gsys.pLootSystem->GetMasterLooter().MasterLootItem(m_pMember, m_pNPC->GetUID(), vecDropItem[0]->GetUID(), 1, false));
	}

	TEST_FIXTURE(FLootSystem, MasterLootItem_NotMasterLootablePlayer)
	{
		ProvideMasterLootItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());
		CHECK_EQUAL(false, gsys.pLootSystem->GetMasterLooter().MasterLootItem(m_pPlayer, m_pNPC->GetUID(), vecDropItem[0]->GetUID(), 2, false));
	}

	TEST_FIXTURE(FLootSystem, MasterLootItem_Random)
	{
		ProvideMasterLootItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());
		GDropItem* pMasterLootableItem = vecDropItem.front();

		class MockModifyHandler : public GLootModifyHandler
		{
		public:
			MOCK_METHOD2(HandleDropListModification, void(GEntityNPC* pNPC, set<int> setPreViewableCID));			
		};
		class MockItemGetter : public GLootItemGetter
		{
		public:
			MOCK_METHOD3(GetItem, UNGETTABLE_ITEM_REASON(int nCID, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem));
		};

		MockModifyHandler* pMockModifyHandler = new MockModifyHandler;
		MockItemGetter* pMockItemGetter = new MockItemGetter;
		gsys.pLootSystem->GetSetterForTest().SetModifyHandler(pMockModifyHandler);
		gsys.pLootSystem->GetSetterForTest().SetItemGetter(pMockItemGetter);

		set<int> setPreViewableCID;
		setPreViewableCID.insert(m_pPlayer->GetCID());
		setPreViewableCID.insert(m_pMember->GetCID());

		vector<GDropItem*> vecMasterLootableItem;
		vecMasterLootableItem.push_back(pMasterLootableItem);
		EXPECT_CALL(*pMockModifyHandler, HandleDropListModification(m_pNPC, setPreViewableCID));
		EXPECT_CALL(*pMockItemGetter, GetItem(AnyOf(m_pPlayer->GetCID(), m_pMember->GetCID()), m_pNPC, vecMasterLootableItem)).WillRepeatedly(Return(UIR_NONE));		

		CHECK_EQUAL(true, gsys.pLootSystem->GetMasterLooter().MasterLootItem(m_pPlayer, m_pNPC->GetUID(), pMasterLootableItem->GetUID(), 1, true));
	}
	
	TEST_FIXTURE(FLootSystem, MasterLootItem)
	{
		ProvideMasterLootItem();

		vector<GDropItem*> vecDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().CollectDropItemByCID(m_pPlayer->GetCID());
		GDropItem* pMasterLootableItem = vecDropItem.front();
		
		class MockModifyHandler : public GLootModifyHandler
		{
		public:
			MOCK_METHOD2(HandleDropListModification, void(GEntityNPC* pNPC, set<int> setPreViewableCID));			
		};
		class MockItemGetter : public GLootItemGetter
		{
		public:
			MOCK_METHOD3(GetItem, UNGETTABLE_ITEM_REASON(int nCID, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem));
		};

		MockModifyHandler* pMockModifyHandler = new MockModifyHandler;
		MockItemGetter* pMockItemGetter = new MockItemGetter;
		gsys.pLootSystem->GetSetterForTest().SetModifyHandler(pMockModifyHandler);
		gsys.pLootSystem->GetSetterForTest().SetItemGetter(pMockItemGetter);

		set<int> setPreViewableCID;
		setPreViewableCID.insert(m_pPlayer->GetCID());
		setPreViewableCID.insert(m_pMember->GetCID());

		vector<GDropItem*> vecMasterLootableItem;
		vecMasterLootableItem.push_back(pMasterLootableItem);
		EXPECT_CALL(*pMockModifyHandler, HandleDropListModification(m_pNPC, setPreViewableCID));
		EXPECT_CALL(*pMockItemGetter, GetItem(m_pMember->GetCID(), m_pNPC, vecMasterLootableItem)).WillRepeatedly(Return(UIR_NONE));

		CHECK_EQUAL(true, gsys.pLootSystem->GetMasterLooter().MasterLootItem(m_pPlayer, m_pNPC->GetUID(), pMasterLootableItem->GetUID(), 1, false));
	}
				
	TEST_FIXTURE(FLootSystem, HandleDropListModification)
	{
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
		m_pNPC->doDie();
		gsys.pLootSystem->GetBeginner().KillNPC(m_pPlayer, m_pNPC->GetUID());

		set<int> setPreViewableCID;
		m_pNPC->GetNPCLoot().GetDropList().GetViewableCID(setPreViewableCID);

		m_pNPC->GetNPCLoot().GetDropList().GetDropItems().Clear();

		set<int> setPostViewableCID;
		m_pNPC->GetNPCLoot().GetDropList().GetViewableCID(setPostViewableCID);

		class MockModifyHandler : public GLootModifyHandler
		{
		public:
			MOCK_METHOD3(HandleUnviewablePlayer,	void(set<int> setPreViewableCID, set<int> setPostViewableCID, GEntityNPC* pNPC));
			MOCK_METHOD3(HandleViewablePlayer,	void(set<int> setPreViewableCID, set<int> setPostViewableCID, GEntityNPC* pNPC ));
			MOCK_METHOD1(RefreshItem,			void(GEntityNPC* pNPC));
			MOCK_METHOD1(HandleEmptyNPC,		void(GEntityNPC* pNPC));
		};

		MockModifyHandler* pMockModifyHandler = new MockModifyHandler;
		gsys.pLootSystem->GetSetterForTest().SetModifyHandler(pMockModifyHandler);

		EXPECT_CALL(*pMockModifyHandler, HandleUnviewablePlayer(setPreViewableCID, setPostViewableCID, m_pNPC));	
		EXPECT_CALL(*pMockModifyHandler, HandleViewablePlayer(setPreViewableCID, setPostViewableCID, m_pNPC));
		EXPECT_CALL(*pMockModifyHandler, RefreshItem(m_pNPC));
		EXPECT_CALL(*pMockModifyHandler, HandleEmptyNPC(m_pNPC));

		gsys.pLootSystem->GetModifyHandler().HandleDropListModification(m_pNPC, setPreViewableCID);		
	}

	TEST_FIXTURE(FLootSystem, HandleUnvisblePlayer_Looting)
	{
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
		m_pNPC->doDie();
		gsys.pLootSystem->GetBeginner().KillNPC(m_pPlayer, m_pNPC->GetUID());

		set<int> setPreAuthorizedCID;
		set<int> setPostAuthorizedCID;
		setPreAuthorizedCID.insert(m_pPlayer->GetCID());

		MockLink* pLink = test.network.NewLink(m_pPlayer);
		gsys.pLootSystem->GetModifyHandler().HandleUnviewablePlayer(setPreAuthorizedCID, setPostAuthorizedCID, m_pNPC);

		CHECK_EQUAL(false, m_pPlayer->GetPlayerLoot().IsLootingNPC(m_pNPC->GetUID()));
		CHECK_EQUAL(false, m_pNPC->GetNPCLoot().IsLootingPlayer(m_pPlayer->GetCID()));

		CHECK_EQUAL(2, pLink->GetCommandCount());
		CHECK_EQUAL(MC_LOOT_REMOVE_LOOTABLE_NPC, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pNPC->GetUID(), pLink->GetParam<MUID>(0, 0));
		CHECK_EQUAL(MC_LOOT_END, pLink->GetCommand(1).GetID());
		CHECK_EQUAL(m_pPlayer->GetUID(), pLink->GetParam<MUID>(1, 0));
	}

	TEST_FIXTURE(FLootSystem, HandleUnvisblePlayer_NotLooting)
	{
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
		m_pNPC->doDie();

		set<int> setPreAuthorizedCID;
		set<int> setPostAuthorizedCID;
		setPreAuthorizedCID.insert(m_pPlayer->GetCID());

		MockLink* pLink = test.network.NewLink(m_pPlayer);
		gsys.pLootSystem->GetModifyHandler().HandleUnviewablePlayer(setPreAuthorizedCID, setPostAuthorizedCID, m_pNPC);

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_LOOT_REMOVE_LOOTABLE_NPC, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pNPC->GetUID(), pLink->GetParam<MUID>(0, 0));
	}

	TEST_FIXTURE(FLootSystem, HandleViewablePlayer)
	{
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
		m_pNPC->doDie();

		set<int> setPreAuthorizedCID;
		set<int> setPostAuthorizedCID;
		setPostAuthorizedCID.insert(m_pPlayer->GetCID());

		MockLink* pLink = test.network.NewLink(m_pPlayer);
		gsys.pLootSystem->GetModifyHandler().HandleViewablePlayer(setPreAuthorizedCID, setPostAuthorizedCID, m_pNPC);

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_LOOT_INSERT_LOOTABLE_NPC, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pNPC->GetUID(), pLink->GetParam<MUID>(0, 0));
	}

	TEST_FIXTURE(FLootSystem, RefreshItem)
	{
		m_pNPC->GetHateTable().AddPoint_FoundEnemy(m_pPlayer);
		m_pNPC->doDie();
		gsys.pLootSystem->GetBeginner().KillNPC(m_pPlayer, m_pNPC->GetUID());

		MockLink* pLink = test.network.NewLink(m_pPlayer);
		gsys.pLootSystem->GetModifyHandler().RefreshItem(m_pNPC);

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_LOOT_REFRESH_ITEM, pLink->GetCommand(0).GetID());

		vector<TD_ITEM_DROP> vecCommandTDDropItem;
		pLink->GetCommand(0).GetBlob(vecCommandTDDropItem, 0);
		vector<TD_ITEM_DROP> vecMemoryTDDropItem = m_pNPC->GetNPCLoot().GetDropList().GetDropItems().MakeVisbleTDDropItem(m_pPlayer->GetCID());

		int nSize = vecCommandTDDropItem.size();

		for (int i=0; i<nSize; i++)
		{
			CHECK(vecCommandTDDropItem[i] == vecMemoryTDDropItem[i]);
		}
	}

	TEST_FIXTURE(FLootSystem, HandleEmptyNPC)
	{
		MockLootingNPC* pNPC = NewMockLootingNPC(GATHER_LUMBER);		

		EXPECT_CALL(*pNPC, doDespawn(false));

		pNPC->GetNPCLoot().GetDropList().SetDropCause(DC_GATHER);


		gsys.pLootSystem->GetModifyHandler().HandleEmptyNPC(pNPC);
	}

	TEST_FIXTURE(FLootSystem, StartByGatherNPC)
	{
		m_pNPC->GetNPCInfo()->nNpcType = NPC_TYPE_GATHER;		
		m_pNPC->GetNPCInfo()->nGatherType = GATHER_LUMBER;

		int nGatherTier = 1;
		int nLootID = GUnitTestUtil::NewID();

		GTalentInfo* pTalentInfo = test.talent.NewTalentInfo();
		pTalentInfo->m_nTalentType = TT_GATHER;
		pTalentInfo->m_nRank = nGatherTier;
		test.talent.Train(m_pPlayer, pTalentInfo);		

		CHECK_EQUAL(true, gsys.pLootSystem->GetBeginner().GatherNPC(m_pPlayer, m_pNPC, nGatherTier, nLootID));

		CHECK(m_pPlayer->GetPlayerLoot().IsLootingNPC(m_pNPC->GetUID()));
		CHECK(m_pPlayer->GetPlayerLoot().IsILootRegulatorActive());
		CHECK(m_pPlayer->GetDoing().IsNowLooting());		
	}


	// 채집준비를 할때 NPC를 죽인다.
	TEST_FIXTURE(FLootSystem, PrepareGather_DeadNPC)
	{	
		MockLootingNPC* pNPC = NewMockLootingNPC(GATHER_LUMBER);
		int nLootID = GUnitTestUtil::NewID();

		EXPECT_CALL(*pNPC, doDieByGather(m_pPlayer, nLootID));
		
		CHECK_EQUAL(true, gsys.pLootSystem->PrepareGather(m_pPlayer, pNPC, nLootID));
	}

	// 렐릭을 채집준비할때 소모성 채집 도구가 소모된다.
	TEST_FIXTURE(FLootSystem, PrepareGather_ConsumeToolItemForRelic)
	{
		m_pNPC->GetNPCInfo()->nNpcType = NPC_TYPE_GATHER;		
		m_pNPC->GetNPCInfo()->nGatherType = GATHER_RELIC;

		// 아이템이 없을때
		CHECK_EQUAL(false, gsys.pLootSystem->PrepareGather(m_pPlayer, m_pNPC, GUnitTestUtil::NewID()));


		// 아이템이 있을때
		GItemData* pItemData = test.item.NewItemData();
		pItemData->m_ItemType = ITEMTYPE_GATHER;
		pItemData->m_nGatherType = GATHER_RELIC;
		test.item.GiveNewItem(m_pPlayer, pItemData->m_nID, 2);

		MockLink* pLink = test.network.NewLink(m_pPlayer);		
		CHECK_EQUAL(true, gsys.pLootSystem->PrepareGather(m_pPlayer, m_pNPC, GUnitTestUtil::NewID()));

		CHECK_EQUAL(1, m_pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pItemData->m_nID));
		CHECK_EQUAL(MC_ITEM_REMOVE, pLink->GetCommandID(0));
	}

	// NPC를 죽여서 시작한 루팅이 끝난 경우
	TEST_FIXTURE(FLootSystem, End_StartByKillNPC)
	{
		ProvideEnd_StartByKill();

		MockLink* pLink = test.network.NewLink(m_pPlayer);

		CHECK_EQUAL(true, gsys.pLootSystem->GetEnder().End(m_pPlayer));

		CHECK_EQUAL(false, m_pPlayer->GetPlayerLoot().IsLootingNPC(m_pNPC->GetUID()));
		CHECK_EQUAL(false, m_pNPC->GetNPCLoot().IsLootingPlayer(m_pPlayer->GetCID()));
		
		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_LOOT_END, pLink->GetCommandID(0));
		CHECK_EQUAL(m_pPlayer->GetUID(), pLink->GetParam<MUID>(0, 0));
	}

	// NPC와의 인터랙션으로 시작한 루팅이 끝난 경우
	TEST_FIXTURE(FLootSystem, End_StratByInteracNPC)
	{
		ProvideEnd_StartByInteract();

		MockLink* pLink = test.network.NewLink(m_pPlayer);

		CHECK_EQUAL(true, gsys.pLootSystem->GetEnder().End(m_pPlayer));

		CHECK_EQUAL(false, m_pPlayer->GetPlayerLoot().IsLootingNPC(m_pNPC->GetUID()));
		CHECK_EQUAL(false, m_pNPC->GetNPCLoot().IsLootingPlayer(m_pPlayer->GetCID()));

		CHECK_EQUAL(false, m_pPlayer->GetPlayerLoot().IsILootRegulatorActive());		

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_LOOT_END, pLink->GetCommandID(0));
		CHECK_EQUAL(m_pPlayer->GetUID(), pLink->GetParam<MUID>(0, 0));
	}
}
