#include "stdafx.h"
#include "GMailItemPutUper.h"
#include "FBaseMail.h"
#include "GUTHelper_Player.h"
#include "GUTHelper_Item.h"
#include "GEntityPlayer.h"
#include "GUTHelper_NPC.h"
#include "GUTHelper_Field.h"
#include "MockField.h"
#include "GNPCInteractionInfo.h"
#include "GUTHelper_Interaction.h"
#include "GPlayerInteraction.h"

SUITE(MailItemPutUper)
{
	struct TestMailItemPutUper: public FBaseMail
	{
		GMailItemPutUper m_PutUper;
		GEntityPlayer* m_pPlayer;
		GEntityNPC* m_pNPC;

		TestMailItemPutUper()
		{
			MockField* pField = GUTHelper_Field::NewMockField();

			GUTHelper_Player helper;
			m_pPlayer = helper.NewPlayer(pField);
			m_pNPC = GUTHelper_NPC::SpawnNPC(pField);

			GNPCInteractionElementInfo* pInteractionElementInfo = GUTHelper_Interaction::GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_MAILBOX);
			GUTHelper_Interaction::MakeInteractionState(m_pPlayer, m_pNPC, pInteractionElementInfo);
		}
	};
	
	TEST_FIXTURE(TestMailItemPutUper, Uper_Success)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);

		GItem* pItem = GUTHelper_Item::GiveNewItem(m_pPlayer, 100, 10);


		// act, assert
		CHECK(m_PutUper.Do(m_pPlayer, &item));
	}

	TEST_FIXTURE(TestMailItemPutUper, Uper_EmptySlot)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);


		// act, assert
		CHECK(!m_PutUper.Do(m_pPlayer, &item));
		CHECK_EQUAL(CR_FAIL_MAIL_APPEND_ITEM_NO_INVENTORY, m_PutUper.GetLastFailure());
	}

	TEST_FIXTURE(TestMailItemPutUper, Uper_InterationEnd)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);

		GItem* pItem = GUTHelper_Item::GiveNewItem(m_pPlayer, 100, 10);

		m_pPlayer->GetInteraction().EndInteraction();


		// act, assert
		CHECK(!m_PutUper.Do(m_pPlayer, &item));
		CHECK_EQUAL(CR_FAIL_MAIL_APPEND_ITEM_NOT_INTERATION, m_PutUper.GetLastFailure());
	}

	TEST_FIXTURE(TestMailItemPutUper, Uper_Binded)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);

		GItem* pItem = GUTHelper_Item::GiveNewItem(m_pPlayer, 100, 10);
		pItem->m_bBind = true;


		// act, assert
		CHECK(!m_PutUper.Do(m_pPlayer, &item));
		CHECK_EQUAL(CR_FAIL_MAIL_APPEND_ITEM_BIND, m_PutUper.GetLastFailure());
	}

	TEST_FIXTURE(TestMailItemPutUper, Uper_Period)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);

		GItem* pItem = GUTHelper_Item::GiveNewItem(m_pPlayer, 100, 10);
		
		
		// act, assert
		pItem->m_ExpirationPeriod.Set(true, false, 0, 0);
		CHECK(!m_PutUper.Do(m_pPlayer, &item));
		CHECK_EQUAL(CR_FAIL_MAIL_APPEND_ITEM_PERIOD, m_PutUper.GetLastFailure());
		pItem->m_ExpirationPeriod.Set(false, false, 0, 0);


		pItem->m_UsagePeriod.Set(true, false, 0, 0);
		CHECK(!m_PutUper.Do(m_pPlayer, &item));		
		CHECK_EQUAL(CR_FAIL_MAIL_APPEND_ITEM_PERIOD, m_PutUper.GetLastFailure());
		pItem->m_UsagePeriod.Set(false, false, 0, 0);

		
		pItem->m_ExpirationPeriod.Set(true, false, 0, 0);
		pItem->m_UsagePeriod.Set(true, false, 0, 0);
		CHECK(!m_PutUper.Do(m_pPlayer, &item));			
		CHECK_EQUAL(CR_FAIL_MAIL_APPEND_ITEM_PERIOD, m_PutUper.GetLastFailure());
	}

	TEST_FIXTURE(TestMailItemPutUper, Uper_Amount)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);

		GItem* pItem = GUTHelper_Item::GiveNewItem(m_pPlayer, 100, 0);

		// act, assert
		CHECK(!m_PutUper.Do(m_pPlayer, &item));
		CHECK_EQUAL(CR_FAIL_MAIL_APPEND_ITEM_NOT_ENOUGH_AMOUNT_UP, m_PutUper.GetLastFailure());
	}
}
