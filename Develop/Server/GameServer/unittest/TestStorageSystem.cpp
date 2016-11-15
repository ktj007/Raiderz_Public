#include "stdafx.h"
#include "FBaseGame.h"
#include "GTestForward.h"
#include "GItemHolder.h"
#include "GStorageSystem.h"
#include "GStorage.h"

SUITE(StorageSytem)
{
	struct Fixture : public FBaseGame
	{
		Fixture()
		{

		}

	};

	namespace Serialize
	{
		TEST_FIXTURE(Fixture, AlreadySerialized)
		{
			m_pPlayer->GetPlayerInfo()->SetStorageMoney(1000);

			GItem* pItem = test.item.GiveNewItemToStorage(m_pPlayer);
			m_pPlayer->GetItemHolder()->GetStorage().SetSerialized();

			gsys.pStorageSystem->Show(m_pPlayer);

			CHECK_EQUAL(1, m_pLink->GetCommandCount());
			CHECK_EQUAL(MC_STORAGE_SHOW_UI, m_pLink->GetCommand(0).GetID());
			CHECK_EQUAL(0, m_pLink->GetParam<int>(0, 0));
			vector<TD_ITEM> vecTDItem;
			ASSERT_EQUAL(true, m_pLink->GetCommand(0).GetBlob(vecTDItem, 1));
			CHECK_EQUAL(true, vecTDItem.empty());
		}

		TEST_FIXTURE(Fixture, FirstSerialize)
		{
			m_pPlayer->GetPlayerInfo()->SetStorageMoney(1000);

			GItem* pItem = test.item.GiveNewItemToStorage(m_pPlayer);
			gsys.pStorageSystem->Show(m_pPlayer);

			CHECK_EQUAL(1, m_pLink->GetCommandCount());
			CHECK_EQUAL(MC_STORAGE_SHOW_UI, m_pLink->GetCommand(0).GetID());
			CHECK_EQUAL(1000, m_pLink->GetParam<int>(0, 0));
			vector<TD_ITEM> vecTDItem;
			ASSERT_EQUAL(true, m_pLink->GetCommand(0).GetBlob(vecTDItem, 1));
			CHECK(pItem->ToTD_ITEM() == vecTDItem.front());
		}
	}

	namespace DepositMoney
	{
		TEST_FIXTURE(Fixture, NotEnoughMoney)
		{
			m_pPlayer->GetPlayerInfo()->SetMoney(5000);
			m_pPlayer->GetPlayerInfo()->SetStorageMoney(1000);

			CHECK_EQUAL(false, gsys.pStorageSystem->DepositMoney(m_pPlayer, 5001));
		}

		TEST_FIXTURE(Fixture, OverDepositMoney)
		{
			m_pPlayer->GetPlayerInfo()->SetMoney(MAX_MONEY);
			m_pPlayer->GetPlayerInfo()->SetStorageMoney(1000);

			CHECK_EQUAL(false, gsys.pStorageSystem->DepositMoney(m_pPlayer, MAX_MONEY));
		}

		TEST_FIXTURE(Fixture, DepositMoney)
		{
			int nBeforeMoney = 5000;
			int nBeforeStorageMoney = 1000;
			int nDepositMoney = 5000;

			m_pPlayer->GetPlayerInfo()->SetMoney(nBeforeMoney);
			m_pPlayer->GetPlayerInfo()->SetStorageMoney(nBeforeStorageMoney);

			CHECK_EQUAL(true, gsys.pStorageSystem->DepositMoney(m_pPlayer, nDepositMoney));

			CHECK_EQUAL(1, m_pLink->GetCommandCount());
			CHECK_EQUAL(MC_STORAGE_DEPOSIT_MONEY, m_pLink->GetCommand(0).GetID());
			CHECK_EQUAL(nDepositMoney, m_pLink->GetParam<int>(0, 0));

			CHECK_EQUAL(nBeforeMoney - nDepositMoney, m_pPlayer->GetPlayerInfo()->GetMoney());
			CHECK_EQUAL(nBeforeStorageMoney + nDepositMoney, m_pPlayer->GetPlayerInfo()->GetStorageMoney());
		}
	}

	namespace WithdrawMoney
	{
		TEST_FIXTURE(Fixture, NotEnoughStorageMoney)
		{
			m_pPlayer->GetPlayerInfo()->SetMoney(5000);
			m_pPlayer->GetPlayerInfo()->SetStorageMoney(1000);

			CHECK_EQUAL(false, gsys.pStorageSystem->WithdrawMoney(m_pPlayer, 1001));
		}

		TEST_FIXTURE(Fixture, OverWithdrawMoney)
		{
			m_pPlayer->GetPlayerInfo()->SetMoney(5000);
			m_pPlayer->GetPlayerInfo()->SetStorageMoney(MAX_MONEY);

			CHECK_EQUAL(false, gsys.pStorageSystem->WithdrawMoney(m_pPlayer, MAX_MONEY));
		}

		TEST_FIXTURE(Fixture, WithdrawMoney)
		{
			int nBeforeMoney = 5000;
			int nBeforeStorageMoney = 1000;
			int nWithdrawMoney = 1000;

			m_pPlayer->GetPlayerInfo()->SetMoney(nBeforeMoney);
			m_pPlayer->GetPlayerInfo()->SetStorageMoney(nBeforeStorageMoney);

			CHECK_EQUAL(true, gsys.pStorageSystem->WithdrawMoney(m_pPlayer, nWithdrawMoney));

			CHECK_EQUAL(1, m_pLink->GetCommandCount());
			CHECK_EQUAL(MC_STORAGE_WITHDRAW_MONEY, m_pLink->GetCommand(0).GetID());
			CHECK_EQUAL(nWithdrawMoney, m_pLink->GetParam<int>(0, 0));

			CHECK_EQUAL(nBeforeMoney + nWithdrawMoney, m_pPlayer->GetPlayerInfo()->GetMoney());
			CHECK_EQUAL(nBeforeStorageMoney - nWithdrawMoney, m_pPlayer->GetPlayerInfo()->GetStorageMoney());
		}
	}
}