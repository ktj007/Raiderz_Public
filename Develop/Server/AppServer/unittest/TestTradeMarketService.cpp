#include "stdafx.h"
#include "CCommandTable_App.h"
#include "PTradeMarketService.h"
#include "PTradeMarketServiceConsumer.h"
#include "PCommandCenter.h"
#include "PFixtureHelper.h"
#include "MockLink.h"
#include "SProxyPlayer.h"

SUITE(TradeMarketService)
{
	namespace {
		bool g_workFinished = false;
		void WaitFinishServiceWorking()
		{
			while (false == g_workFinished) {}
		}
	}

	class PTradeMarketService4Test : public PTradeMarketService
	{
	public:
		ITEMS_ORDER_BY_PRICE& Items(const int& typeID)
		{
			return m_itemsByTypeID[typeID];
		}

		size_t TotalCount()
		{
			return m_totalCnt;
		}

		void AddItem(const TD_TRADEMARKETITEM& item)
		{
			__super::Add(item);
		}

		void NotifyAddItem(minet::MCommand* const cmd) const override
		{
			g_workFinished = false;
			__super::NotifyAddItem(cmd);
		}

		void NotifyDelItem(minet::MCommand* const cmd) const override
		{
			g_workFinished = false;
			__super::NotifyDelItem(cmd);
		}

		void RequestSearch(minet::MCommand* const cmd) const override
		{
			g_workFinished = false;
			__super::RequestSearch(cmd);
		}

		void RequestSearchNext(minet::MCommand* const cmd) const override
		{
			g_workFinished = false;
			__super::RequestSearchNext(cmd);
		}

		void OnIO(const DWORD& numOfByteTransfered,OVERLAPPED* const overlapped) override
		{
			__super::OnIO(numOfByteTransfered, overlapped);
			g_workFinished = true;
		}
	};

	class PTradeMarketServiceConsumer4Test : public PTradeMarketServiceConsumer
	{
	public:
		void NotifyAddItem(MCommand* cmd) override
		{
			__super::NotifyAddItem(cmd);
			WaitFinishServiceWorking();
		}

		void NotifyDelItem(MCommand* cmd) override
		{
			__super::NotifyDelItem(cmd);
			WaitFinishServiceWorking();
		}

		void RequestSearch(MCommand* cmd) override
		{
			__super::RequestSearch(cmd);
			WaitFinishServiceWorking();
		}

		void RequestSearchNext(MCommand* cmd) override
		{
			__super::RequestSearchNext(cmd);
			WaitFinishServiceWorking();
		}
	};

	class FTradeMarket: public FBaseGameServer, public FBasePlayer
	{
	public:
		FTradeMarket()
		{
			m_tradeMarketService = m_serviceChanger.Get();
			m_tradeMarketServiceConsumer = m_serviceConsumerChanger.Get();
		}

		virtual ~FTradeMarket()
		{
		}

	protected:
		PTestSysWrapper2<PTradeMarketService, PTradeMarketService4Test> m_serviceChanger;
		PTradeMarketService4Test* m_tradeMarketService;
		PTestSysWrapper2<PTradeMarketServiceConsumer, PTradeMarketServiceConsumer4Test> m_serviceConsumerChanger;
		PTradeMarketServiceConsumer4Test* m_tradeMarketServiceConsumer;
	};

	class FTradeMarketSearch: public FTradeMarket
	{
	public:
		FTradeMarketSearch()
		{
			int nServerID = SUnitTestUtil::NewID();
			MUID uidGameServer = AddGameServerObject(nServerID);
			m_pGameServerObject = gmgr.pGameServerObjectManager->Find(uidGameServer);
			m_pLink = PTestHelper::NewLink(uidGameServer);

			// Add PlayerObject
			int nGameServerID = m_pGameServerObject->GetServerID();
			CID nCID = AddPlayerObject(nGameServerID);

			SProxyPlayerManager* pManager = gsys.pServer->GetPlayerManager();
			m_pProxyPlayer = pManager->FindPlayer(nCID);
		}

		virtual ~FTradeMarketSearch()
		{
		}

	protected:
		MockLink*	m_pLink;
		PGameServerObject* m_pGameServerObject;
		SProxyPlayer* m_pProxyPlayer;
	};

	TEST_FIXTURE(FTradeMarket, TradeMarketService_ItemAdded)
	{
		{
			for (int i = 1; i < 51; ++i)
			{
				TD_TRADEMARKETITEM item(i, 2, 3, 4);
				MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_ADD, 1, NEW_SINGLE_BLOB(item));
				m_tradeMarketServiceConsumer->NotifyAddItem(cmd);
			}

			ASSERT_EQ(50, m_tradeMarketService->TotalCount());

			const TD_TRADEMARKETITEM& addedItem(*m_tradeMarketService->Items(2).begin());
			EXPECT_EQ(1, addedItem.dbid);
			EXPECT_EQ(2, addedItem.typeID);
			EXPECT_EQ(3, addedItem.count);
			EXPECT_EQ(4, addedItem.price);
		}

		// dbid가 같은 아이템을 중복으로 넣을 수 없다
		{
			TD_TRADEMARKETITEM item(1, 3, 2, 10);
			MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_ADD, 1, NEW_SINGLE_BLOB(item));
			m_tradeMarketServiceConsumer->NotifyAddItem(cmd);

			ASSERT_EQ(50, m_tradeMarketService->TotalCount());
		}
	}

	TEST_FIXTURE(FTradeMarket, TradeMarketService_ItemDeleted)
	{
		for (int i = 1; i < 51; ++i)
		{
			TD_TRADEMARKETITEM item(i, 2, 3, 4);
			MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_ADD, 1, NEW_SINGLE_BLOB(item));
			m_tradeMarketServiceConsumer->NotifyAddItem(cmd);
		}

		ASSERT_EQ(50, m_tradeMarketService->TotalCount());

		{
			MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_DEL, 2, NEW_INT(2), NEW_INT64(10));
			m_tradeMarketServiceConsumer->NotifyDelItem(cmd);

			EXPECT_EQ(49, m_tradeMarketService->TotalCount());
		}
		
		{
			// 없는 물건은 지우지 못한다
			MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_DEL, 2, NEW_INT(2), NEW_INT64(10));
			m_tradeMarketServiceConsumer->NotifyDelItem(cmd);

			EXPECT_EQ(49, m_tradeMarketService->TotalCount());
		}

		{
			MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_DEL, 2, NEW_INT(2), NEW_INT64(15));
			m_tradeMarketServiceConsumer->NotifyDelItem(cmd);

			EXPECT_EQ(48, m_tradeMarketService->TotalCount());
		}
	}

	TEST_FIXTURE(FTradeMarketSearch, TradeMarketService_ItemSearch)
	{
		const int 아이템100번수량 = 10;
		for (int64 dbid = 1; dbid <= 아이템100번수량; ++dbid)
		{
			m_tradeMarketService->AddItem(TD_TRADEMARKETITEM(dbid, 100, 3, 4));
		}

		const int 아이템200번수량 = 20;
		for (int64 dbid = 101; dbid <= 100 + 아이템200번수량; ++dbid)
		{
			m_tradeMarketService->AddItem(TD_TRADEMARKETITEM(dbid, 200, 30, 40));
		}

		const int 아이템300번수량 = 17;
		for (int64 dbid = 201; dbid <= 200 + 아이템300번수량; ++dbid)
		{
			m_tradeMarketService->AddItem(TD_TRADEMARKETITEM(dbid, 300, 300, 400));
		}
		
		ASSERT_EQ(아이템100번수량 + 아이템200번수량 + 아이템300번수량, m_tradeMarketService->TotalCount());

		const CID	플레이어 = m_pProxyPlayer->GetCID();
		{
			// 한 페이지 이내에서 검색
			vector<int>	검색희망아이템목록;
			검색희망아이템목록.push_back(100);

			MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_SEARCH, 2, NEW_INT(플레이어), NEW_BLOB(검색희망아이템목록));
			m_tradeMarketServiceConsumer->RequestSearch(cmd);

			ASSERT_EQ(1, m_pLink->GetCommandCount());
			const MCommand& recvedCmd = m_pLink->GetCommand(0);
			ASSERT_EQ(MPC_AG_TRADEMARKET_SEARCH_RESULT, recvedCmd.GetID());
			CID 결과받을플레이어 = 0;
			ASSERT_EQ(true, recvedCmd.GetParameter(&결과받을플레이어, 0, MPT_INT));
			EXPECT_EQ(플레이어, 결과받을플레이어);
			vector<TD_TRADEMARKETITEM> itemResults;
			recvedCmd.GetBlob(itemResults, 1);
			EXPECT_EQ(아이템100번수량, itemResults.size());
			m_pLink->ResetCommands();
		}
		
		{
			// 한 페이지가 넘는 아이템 검색
			vector<int>	검색희망아이템목록;
			검색희망아이템목록.push_back(300);

			MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_SEARCH, 2, NEW_INT(플레이어), NEW_BLOB(검색희망아이템목록));
			m_tradeMarketServiceConsumer->RequestSearch(cmd);

			ASSERT_EQ(1, m_pLink->GetCommandCount());
			const MCommand& recvedCmd = m_pLink->GetCommand(0);
			ASSERT_EQ(MPC_AG_TRADEMARKET_SEARCH_RESULT, recvedCmd.GetID());
			CID 결과받을플레이어 = 0;
			ASSERT_EQ(true, recvedCmd.GetParameter(&결과받을플레이어, 0, MPT_INT));
			EXPECT_EQ(플레이어, 결과받을플레이어);
			vector<TD_TRADEMARKETITEM> itemResults;
			recvedCmd.GetBlob(itemResults, 1);
			EXPECT_EQ(TRADEMARKET_ITEM_SEARCH_PAGE_SIZE, itemResults.size());
			m_pLink->ResetCommands();
		}

		{
			// 여러 아이템 검색(한 페이지 넘음)
			vector<int>	검색희망아이템목록;
			검색희망아이템목록.push_back(100);
			검색희망아이템목록.push_back(200);

			MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_SEARCH, 2, NEW_INT(플레이어), NEW_BLOB(검색희망아이템목록));
			m_tradeMarketServiceConsumer->RequestSearch(cmd);

			ASSERT_EQ(1, m_pLink->GetCommandCount());
			const MCommand& recvedCmd = m_pLink->GetCommand(0);
			ASSERT_EQ(MPC_AG_TRADEMARKET_SEARCH_RESULT, recvedCmd.GetID());
			CID 결과받을플레이어 = 0;
			ASSERT_EQ(true, recvedCmd.GetParameter(&결과받을플레이어, 0, MPT_INT));
			EXPECT_EQ(플레이어, 결과받을플레이어);
			vector<TD_TRADEMARKETITEM> itemResults;
			recvedCmd.GetBlob(itemResults, 1);
			EXPECT_EQ(TRADEMARKET_ITEM_SEARCH_PAGE_SIZE, itemResults.size());
			m_pLink->ResetCommands();
		}

		{
			// 계속 검색
			vector<int>	검색희망아이템목록;
			검색희망아이템목록.push_back(100);
			검색희망아이템목록.push_back(200);
			//검색희망아이템목록.push_back(300);

			{
				MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_SEARCH, 2, NEW_INT(플레이어), NEW_BLOB(검색희망아이템목록));
				m_tradeMarketServiceConsumer->RequestSearch(cmd);

				ASSERT_EQ(1, m_pLink->GetCommandCount());
				const MCommand& recvedCmd = m_pLink->GetCommand(0);
				ASSERT_EQ(MPC_AG_TRADEMARKET_SEARCH_RESULT, recvedCmd.GetID());
				CID 결과받을플레이어 = 0;
				ASSERT_EQ(true, recvedCmd.GetParameter(&결과받을플레이어, 0, MPT_INT));
				EXPECT_EQ(플레이어, 결과받을플레이어);
				vector<TD_TRADEMARKETITEM> itemResults;
				recvedCmd.GetBlob(itemResults, 1);
				EXPECT_EQ(TRADEMARKET_ITEM_SEARCH_PAGE_SIZE, itemResults.size());
				m_pLink->ResetCommands();
			}
			
			{
				MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_SEARCH_NEXT, 1, NEW_INT(플레이어));
				m_tradeMarketServiceConsumer->RequestSearchNext(cmd);

				ASSERT_EQ(1, m_pLink->GetCommandCount());
				const MCommand& recvedCmd = m_pLink->GetCommand(0);
				ASSERT_EQ(MPC_AG_TRADEMARKET_SEARCH_RESULT, recvedCmd.GetID());
				CID 결과받을플레이어 = 0;
				ASSERT_EQ(true, recvedCmd.GetParameter(&결과받을플레이어, 0, MPT_INT));
				EXPECT_EQ(플레이어, 결과받을플레이어);
				vector<TD_TRADEMARKETITEM> itemResults;
				recvedCmd.GetBlob(itemResults, 1);
				EXPECT_EQ(TRADEMARKET_ITEM_SEARCH_PAGE_SIZE, itemResults.size());
				m_pLink->ResetCommands();
			}

			{
				MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_SEARCH_NEXT, 1, NEW_INT(플레이어));
				m_tradeMarketServiceConsumer->RequestSearchNext(cmd);

				ASSERT_EQ(1, m_pLink->GetCommandCount());
				const MCommand& recvedCmd = m_pLink->GetCommand(0);
				ASSERT_EQ(MPC_AG_TRADEMARKET_SEARCH_RESULT, recvedCmd.GetID());
				CID 결과받을플레이어 = 0;
				ASSERT_EQ(true, recvedCmd.GetParameter(&결과받을플레이어, 0, MPT_INT));
				EXPECT_EQ(플레이어, 결과받을플레이어);
				vector<TD_TRADEMARKETITEM> itemResults;
				recvedCmd.GetBlob(itemResults, 1);
				EXPECT_EQ(0, itemResults.size());
				m_pLink->ResetCommands();
			}
		}
	}

	TEST_FIXTURE(FTradeMarketSearch, TradeMarketService_ItemSearchOrderByPrice)
	{
		const int 아이템100번수량 = 15;
		for (int i = 1; i <= 아이템100번수량; ++i)
		{
			m_tradeMarketService->AddItem(TD_TRADEMARKETITEM(i, 100, 3, 4 * i % 10));
		}

		const CID 플레이어 = m_pProxyPlayer->GetCID();

		vector<int>	검색희망아이템목록;
		검색희망아이템목록.push_back(100);

		MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_SEARCH, 2, NEW_INT(플레이어), NEW_BLOB(검색희망아이템목록));
		m_tradeMarketServiceConsumer->RequestSearch(cmd);

		ASSERT_EQ(1, m_pLink->GetCommandCount());
		const MCommand& recvedCmd = m_pLink->GetCommand(0);
		ASSERT_EQ(MPC_AG_TRADEMARKET_SEARCH_RESULT, recvedCmd.GetID());

		vector<TD_TRADEMARKETITEM> itemResults;
		recvedCmd.GetBlob(itemResults, 1);
		
		int lastPrice = 0;
		for each(const TD_TRADEMARKETITEM& item in itemResults)
		{
			EXPECT_LE(lastPrice, item.price);
			lastPrice = item.price;
		}

		m_pLink->ResetCommands();
	}
}
