#include "stdafx.h"
#include "GPlayerMailReserver.h"
#include "GUTHelper_Player.h"
#include "GTestWrapper.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"

SUITE(PlayerMailReserver)
{
	struct TestPlayerMailReserver
	{
		GTestMgrWrapper<GPlayerObjectManager>	m_PlayerObjectManagerWrapper;
		GEntityPlayer* m_pPlayer;
		GPlayerMailReserver* m_pMailReserver;

		TestPlayerMailReserver()
		{
			GUTHelper_Player helper;
			m_pPlayer = helper.NewPlayer();

			m_pMailReserver = &m_pPlayer->GetMailReserver();
		}
	};

	TEST_FIXTURE(TestPlayerMailReserver, PutUpItem)
	{	
		// arrange
		int nSlotID = 1;
		int nAmount = 10;

		// act, assert
		m_pMailReserver->PutUpItem(nSlotID, nAmount);
		CHECK_EQUAL(nAmount, m_pMailReserver->GetAmount(nSlotID));

		m_pMailReserver->PutUpItem(nSlotID, 1);
		CHECK_EQUAL(nAmount + 1, m_pMailReserver->GetAmount(nSlotID));
	}

	TEST_FIXTURE(TestPlayerMailReserver, PutDownItem)
	{
		// arrange
		int nSlotID = 1;
		int nAmount = 10;

		m_pMailReserver->PutUpItem(nSlotID, nAmount);

		
		// act, assert
		m_pMailReserver->PutDownItem(nSlotID, 1);
		CHECK_EQUAL(nAmount - 1, m_pMailReserver->GetAmount(nSlotID));

		m_pMailReserver->PutDownItem(nSlotID, nAmount - 1);
		CHECK_EQUAL(0, m_pMailReserver->GetAmount(nSlotID));
		CHECK(!m_pMailReserver->IsAppended(nSlotID));
	}

	TEST_FIXTURE(TestPlayerMailReserver, Clear)
	{
		// arrange
		int nSlotID = 1;
		int nSlotID2 = 2;
		int nAmount = 10;

		m_pMailReserver->PutUpItem(nSlotID, nAmount);
		m_pMailReserver->PutUpItem(nSlotID2, nAmount);

		CHECK_EQUAL(2, m_pMailReserver->GetAppendedSlotCount());


		// act
		m_pMailReserver->Clear();

		
		// assert
		CHECK_EQUAL(0, m_pMailReserver->GetAppendedSlotCount());
	}
}
