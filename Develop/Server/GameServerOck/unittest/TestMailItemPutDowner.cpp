#include "stdafx.h"
#include "GMailItemPutDowner.h"
#include "FBaseMail.h"
#include "GUTHelper_Player.h"
#include "GPlayerMailReserver.h"
#include "MockField.h"
#include "GUTHelper_Field.h"
#include "GUTHelper_NPC.h"
#include "GUTHelper_Interaction.h"
#include "GUTHelper_Item.h"
#include "GEntityPlayer.h"
#include "GPlayerInteraction.h"

SUITE(MailItemPutDowner)
{
	struct TestMailItemPutDowner: public FBaseMail
	{
		GMailItemPutDowner m_PutDowner;
		GEntityPlayer* m_pPlayer;
		GEntityNPC* m_pNPC;
		GItem* m_pItem;

		TestMailItemPutDowner()
		{
			MockField* pField = GUTHelper_Field::NewMockField();

			GUTHelper_Player helper;
			m_pPlayer = helper.NewPlayer(pField);
			m_pNPC = GUTHelper_NPC::SpawnNPC(pField);

			GNPCInteractionElementInfo* pInteractionElementInfo = GUTHelper_Interaction::GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_MAILBOX);
			GUTHelper_Interaction::MakeInteractionState(m_pPlayer, m_pNPC, pInteractionElementInfo);			


			m_pItem = GUTHelper_Item::GiveNewItem(m_pPlayer, 100, 10);
			GPlayerMailReserver& reserver = m_pPlayer->GetMailReserver();
			reserver.PutUpItem(0, 5);
		}
	};

	TEST_FIXTURE(TestMailItemPutDowner, Downer_Success)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);

		// act, assert
		CHECK(m_PutDowner.Do(m_pPlayer, &item));
	}

	TEST_FIXTURE(TestMailItemPutDowner, Downer_InteractionEnd)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		InitReserveItem(item);
		
		m_pPlayer->GetInteraction().EndInteraction();


		// act, assert
		CHECK(!m_PutDowner.Do(m_pPlayer, &item));
		CHECK_EQUAL(CR_FAIL_MAIL_APPEND_ITEM_NOT_INTERATION, m_PutDowner.GetLastFailure());
	}
	
	TEST_FIXTURE(TestMailItemPutDowner, Downer_Amount)
	{
		// arrange
		TD_MAIL_RESERVE_ITEM item;
		item.nInventorySlotID = 0;
		item.nItemAmount = 10;

		// act, assert
		CHECK(!m_PutDowner.Do(m_pPlayer, &item));
		CHECK_EQUAL(CR_FAIL_MAIL_APPEND_ITEM_NOT_ENOUGH_AMOUNT_DOWN, m_PutDowner.GetLastFailure());
	}
}
