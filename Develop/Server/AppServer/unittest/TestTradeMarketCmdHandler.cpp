#include "stdafx.h"
#include "PFixtureHelper.h"
#include "MockLink.h"
#include "CCommandTable_App.h"
#include "PTradeMarketServiceConsumer.h"
#include "PCommandCenter.h"

SUITE(TradeMarketCmdHandler)
{
	struct MockTradeMarketServiceConsumer: public PTradeMarketServiceConsumer
	{
		MOCK_METHOD1(NotifyAddItem,		void(MCommand*));
		MOCK_METHOD1(NotifyDelItem,		void(MCommand*));
		MOCK_METHOD1(RequestSearch,		void(MCommand*));
		MOCK_METHOD1(RequestSearchNext,	void(MCommand*));
	};

	class FTradeMarket: public FBaseGameServer, public FBasePlayer
	{
	public:
		FTradeMarket()
		{
			// Add GameServerObject
			MUID uidGameServer = AddGameServerObject(SUnitTestUtil::NewID());
			m_pLink = PTestHelper::NewLink(uidGameServer);
			m_pTradeMarketServiceConsumer = m_pTradeMarketServiceConsumerWrapper.Get();
		}

		virtual ~FTradeMarket()
		{
		}

	protected:
		MockLink* m_pLink;
		PTestSysWrapper2<PTradeMarketServiceConsumer, MockTradeMarketServiceConsumer> m_pTradeMarketServiceConsumerWrapper;
		MockTradeMarketServiceConsumer* m_pTradeMarketServiceConsumer;
	};

	TEST_FIXTURE(FTradeMarket, CommandHandler_AddedItem)
	{
		TD_TRADEMARKETITEM item(1, 2, 3, 4);
		MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_ADD, 1, NEW_SINGLE_BLOB(item));

		EXPECT_CALL(*m_pTradeMarketServiceConsumer, NotifyAddItem(cmd));

		MCommandResult nCommandResult = m_pLink->OnRecvCommand(*cmd);
		CHECK_EQUAL(CR_TRUE, nCommandResult);

		delete cmd;
	}

	TEST_FIXTURE(FTradeMarket, CommandHandler_DeletedItem)
	{
		MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_DEL, 2, NEW_INT(1), NEW_INT64(2));
		EXPECT_CALL(*m_pTradeMarketServiceConsumer, NotifyDelItem(cmd));

		MCommandResult nCommandResult = m_pLink->OnRecvCommand(*cmd);
		CHECK_EQUAL(CR_TRUE, nCommandResult);

		delete cmd;
	}

	TEST_FIXTURE(FTradeMarket, CommandHandler_RequestSearch)
	{
		const CID player(100);
		std::vector<int> respectSearch;
		MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_SEARCH, 2, NEW_INT(player), NEW_BLOB(respectSearch));
		EXPECT_CALL(*m_pTradeMarketServiceConsumer, RequestSearch(cmd));

		MCommandResult nCommandResult = m_pLink->OnRecvCommand(*cmd);

		CHECK_EQUAL(CR_TRUE, nCommandResult);

		delete cmd;
	}

	TEST_FIXTURE(FTradeMarket, CommandHandler_RequestSearchNext)
	{
		MCommand* const cmd = gsys.pCommandCenter->MakeNewCommand(MPC_GA_TRADEMARKET_SEARCH_NEXT, 1, NEW_INT(100));
		EXPECT_CALL(*m_pTradeMarketServiceConsumer, RequestSearchNext(cmd));

		MCommandResult nCommandResult = m_pLink->OnRecvCommand(*cmd);

		CHECK_EQUAL(CR_TRUE, nCommandResult);

		delete cmd;
	}
}
