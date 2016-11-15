#include "stdafx.h"
#include "GUTHelper.h"
#include "GTestWrapper.h"
#include "MockField.h"
#include "MockLink.h"
#include "GFieldMgr.h"
#include "GItemData.h"
#include "GItemManager.h"
#include "MockDBManager.h"
#include "GPlayerObjectManager.h"
#include "GItemExpirer.h"
#include "CCommandTable.h"
#include "FBaseMockLink.h"
#include "GItemEquiper.h"
#include "GUTHelper_Field.h"
#include "GItemHolder.h"
#include "GDBTaskItemDelete.h"
#include "GDBTaskDataItemEquip.h"
#include "GTestForward.h"
#include "FBaseGame.h"

SUITE(ItemExpirer)
{
	struct Fixture : public FBaseGame
	{
		Fixture() : ONE_MIN_BY_TICK(60.f)
		{
			m_pItemHolder = m_pPlayer->GetItemHolder();
			m_pItemExpirer = m_pPlayer->GetItemExpirer();

			m_pInvenItem = test.item.GiveNewItem(m_pPlayer);
			m_pEquipItem = test.item.EquipNewItem(m_pPlayer, INVALID_ID,  ITEMSLOT_FEET);

			m_pLink->ResetCommands();
		}

		void SetPlayTime(const int nPlayTime)
		{
			m_pPlayer->GetPlayerInfo()->nPlayTimeSec = nPlayTime;
		}		

		static const int ONE_SECOND = 1;
		const float ONE_MIN_BY_TICK;

		GItemHolder* m_pItemHolder;
		GItemExpirer* m_pItemExpirer;

		GItem* m_pInvenItem;
		GItem* m_pEquipItem;
	};

	class MockDB : public MockDBManager
	{
	public:
		MockDB()
		{
			bIsCall_UpdateEquipedItem_Equip = false;
			bIsCall_DeleteItem = false;
		}

		bool IsCall_UpdateEquipedItem_Equip() { return bIsCall_UpdateEquipedItem_Equip; }
		bool IsCall_DeleteItem() { return bIsCall_DeleteItem; }

		virtual bool ItemEquip(GDBT_ITEM_EQUIP& data) override
		{
			bIsCall_UpdateEquipedItem_Equip = true;
			return __super::ItemEquip(data);
		}
		virtual bool ItemDelete(const GDBT_ITEM_DATA& data) override
		{
			bIsCall_DeleteItem = true;

			return __super::ItemDelete(data);
		}

	private:
		bool bIsCall_UpdateEquipedItem_Equip;
		bool bIsCall_DeleteItem;
	};

	TEST_FIXTURE(Fixture, OneItemIsExpiring)
	{
		SetPlayTime(60);

		GTestSysWrapper2<GDBManager, MockDB> mockDBManager;
		m_pInvenItem->m_UsagePeriod.Set(true, true, 10, 0);

		int nBeforeItemCount = m_pItemHolder->GetItemCount(SLOTTYPE_INVENTORY);
		m_pItemExpirer->Run(ONE_MIN_BY_TICK);

		CHECK_EQUAL(nBeforeItemCount-1, m_pItemHolder->GetItemCount(SLOTTYPE_INVENTORY));

		CHECK_EQUAL(1,				m_pLink->GetCommandCount());
		CHECK_EQUAL(MC_ITEM_REMOVE,	m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(true,			mockDBManager.Get()->IsCall_DeleteItem());
	}

	TEST_FIXTURE(Fixture, OneItemIsExpiringDirectly)
	{
		SetPlayTime(100);

		GTestSysWrapper2<GDBManager, MockDB> mockDBManager;
		m_pInvenItem->m_UsagePeriod.Set(true, true, 10, 0);		

		int nBeforeItemCount = m_pItemHolder->GetItemCount(SLOTTYPE_INVENTORY);
		m_pItemExpirer->RunImmediately();

		CHECK_EQUAL(true,	mockDBManager.Get()->IsCall_DeleteItem());
		CHECK_EQUAL(nBeforeItemCount-1, m_pItemHolder->GetItemCount(SLOTTYPE_INVENTORY));
	}

	TEST_FIXTURE(Fixture, AllItemIsNonExpiring)
	{
		int nBeforeItemCount = m_pItemHolder->GetItemCount(SLOTTYPE_INVENTORY);

		m_pItemExpirer->Run(ONE_MIN_BY_TICK);

		CHECK_EQUAL(nBeforeItemCount, m_pItemHolder->GetItemCount(SLOTTYPE_INVENTORY));
		CHECK_EQUAL(0,	m_pLink->GetCommandCount());
	}

	TEST_FIXTURE(Fixture, AllItemIsExpiring)
	{
		SetPlayTime(100);

		GTestSysWrapper2<GDBManager, MockDB> mockDBManager;
		m_pEquipItem->m_UsagePeriod.Set(true, true, 10, 0);
		m_pInvenItem->m_UsagePeriod.Set(true, true, 10, 0);

		m_pItemExpirer->Run(ONE_MIN_BY_TICK);

		CHECK_EQUAL(0,				m_pItemHolder->GetItemCount(SLOTTYPE_EQUIP));
		CHECK_EQUAL(0,				m_pItemHolder->GetItemCount(SLOTTYPE_INVENTORY));
		CHECK_EQUAL(true,			mockDBManager.Get()->IsCall_DeleteItem());

		CHECK_EQUAL(2,				m_pLink->GetCommandCount());
		CHECK_EQUAL(MC_ITEM_REMOVE,	m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(MC_ITEM_REMOVE,	m_pLink->GetCommand(1).GetID());
	}

	TEST_FIXTURE(Fixture, AllItemIsExpiring_InOrder)
	{
		m_pEquipItem->m_UsagePeriod.Set(true, true, ONE_SECOND, 0);		
		m_pInvenItem->m_UsagePeriod.Set(true, true, ONE_SECOND+1, 0);			
		
		int nBeforeItemCount = 0;
		
		SetPlayTime(ONE_SECOND);
		nBeforeItemCount = m_pItemHolder->GetItemCount(SLOTTYPE_EQUIP);
		m_pItemExpirer->Run(ONE_MIN_BY_TICK);
		CHECK_EQUAL(nBeforeItemCount-1, m_pItemHolder->GetItemCount(SLOTTYPE_EQUIP));

		SetPlayTime(ONE_SECOND + 1);
		nBeforeItemCount = m_pItemHolder->GetItemCount(SLOTTYPE_INVENTORY);
		m_pItemExpirer->Run(ONE_MIN_BY_TICK);
		CHECK_EQUAL(nBeforeItemCount-1, m_pItemHolder->GetItemCount(SLOTTYPE_INVENTORY));
	}	
}