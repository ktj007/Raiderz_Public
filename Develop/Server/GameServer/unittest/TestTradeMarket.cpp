#include "stdafx.h"
#include "FBaseMockLink.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "GItemManager.h"
#include "GServerResLoader.h"
#include "GUTHelper_Item.h"
#include "GItemHolder.h"
#include "GTradeMarket.h"
#include "CCommandTable.h"
#include "CTradeMarketConstant.h"
#include "CTradeMarketDefine.h"
#include "GUTHelper_Field.h"

SUITE(TradeMarket)
{
	struct FTradeMarket : public FBaseMockLink
	{
		FTradeMarket()
		{
			field = GUTHelper_Field::DefaultMockField();
			player = GUTHelper::NewEntityPlayer(field, vec3(0.0f, 0.0f, 0.0f));
			playerLink = GUTHelper::NewLink(player);
			itemHolder = player->GetItemHolder();
		}

		void PushInventoryTestItem(const int& slot,const int& itemID,const int& quantity)
		{
			GItem* const item = GUTHelper_Item::NewItem();
			item->m_nIUID = itemID;
			itemHolder->PushItem(SLOTTYPE_INVENTORY, slot, item);
			itemHolder->SetAmount(SLOTTYPE_INVENTORY, slot, quantity);
		}

		~FTradeMarket()
		{
			field->Destroy();
			GUTHelper::ClearLinks();
		}

		MockField*		field;
		GEntityPlayer*	player;
		MockLink*		playerLink;
		GItemHolder*	itemHolder;
	};

	TEST_FIXTURE(FTradeMarket, PutOn_Overload)
	{
		const int64	거래아이템아이디	= 100;
		const int	아이템원본수량		= 10;
		const int	아이템슬롯번호		= 1;
		const int	희망가격			= 50;
		const int	기간				= 3;

		PushInventoryTestItem(아이템슬롯번호, 거래아이템아이디, 아이템원본수량);
		ASSERT_EQ(아이템원본수량, itemHolder->GetItemCountInSlot(SLOTTYPE_INVENTORY, 아이템슬롯번호));

		// 더 많이 올릴 수 없다
		EXPECT_EQ(false, GTradeMarket::PutOn(player->GetUID(), 아이템슬롯번호, 희망가격, 아이템원본수량 + 10, 기간));

		ASSERT_EQ(아이템원본수량, itemHolder->GetItemCountInSlot(SLOTTYPE_INVENTORY, 아이템슬롯번호));
		ASSERT_EQ(1, playerLink->GetCommandCount());
		{
			MCommand& cmd = playerLink->GetCommand(0);
			EXPECT_EQ(MC_SC_TRADEMARKET_EXCEPTION, cmd.GetID());
			EXPECT_EQ(1, cmd.GetParameterCount());
			UCHAR errorCode = 0;
			EXPECT_EQ(true, cmd.GetParameter(&errorCode, 0, MPT_UCHAR));
			EXPECT_EQ(TRADEMARKET_EXCEPTION_PUTON_INVALID_ITEM_COUNT, errorCode);
			playerLink->ResetCommands();
		}

// 		GTradeMarket::Search(player->GetUID(), 거래아이템아이디);
// 		ASSERT_EQ(0, playerLink->GetCommandCount());
	}

	TEST_FIXTURE(FTradeMarket, PutOn_AllItems)
	{
		const int64	거래아이템아이디	= 100;
		const int	아이템원본수량		= 10;
		const int	아이템슬롯번호		= 1;
		const int	희망가격			= 50;
		const int	기간				= 3;

		PushInventoryTestItem(아이템슬롯번호, 거래아이템아이디, 아이템원본수량);
		ASSERT_EQ(아이템원본수량, itemHolder->GetItemCountInSlot(SLOTTYPE_INVENTORY, 아이템슬롯번호));

		EXPECT_EQ(true, GTradeMarket::PutOn(player->GetUID(), 아이템슬롯번호, 희망가격, 아이템원본수량, 기간));

		ASSERT_EQ(0, itemHolder->GetItemCountInSlot(SLOTTYPE_INVENTORY, 아이템슬롯번호));
		ASSERT_EQ(1, playerLink->GetCommandCount());
		{
			MCommand& cmd = playerLink->GetCommand(0);
			EXPECT_EQ(MC_SC_TRADEMARKET_PUTTED_ON, cmd.GetID());
			EXPECT_EQ(3, cmd.GetParameterCount());
			
			int 커맨드에들어간슬롯번호 = 0;
			EXPECT_EQ(true, cmd.GetParameter(&커맨드에들어간슬롯번호, 0, MPT_INT));
			EXPECT_EQ(아이템슬롯번호, 커맨드에들어간슬롯번호);

			int 커맨드에들어간아이템갯수 = 0;
			EXPECT_EQ(true, cmd.GetParameter(&커맨드에들어간아이템갯수, 1, MPT_INT));
			EXPECT_EQ(아이템원본수량, 커맨드에들어간아이템갯수);

			int64 물품거래소디비일련번호 = 0;
			EXPECT_EQ(true, cmd.GetParameter(&물품거래소디비일련번호, 2, MPT_INT64));
			//EXPECT_TRUE(0 < 물품거래소디비일련번호);

			playerLink->ResetCommands();
		}

		{
// 			GTradeMarket::Search(player->GetUID(), 거래아이템아이디);
// 			ASSERT_EQ(1, playerLink->GetCommandCount());
// 			MCommand& cmd = playerLink->GetCommand(0);
// 			EXPECT_EQ(MC_SC_TRADEMARKET_ITEM_LIST, cmd.GetID());
// 
// 			vector<TRADEMARKET_ITEM> 물품거래소아이템목록결과;
// 			EXPECT_EQ(true, cmd.GetBlob(물품거래소아이템목록결과, 0));
// 			ASSERT_EQ(1, 물품거래소아이템목록결과.size());
// 			const TRADEMARKET_ITEM& 결과아이템(*물품거래소아이템목록결과.begin());
// 			EXPECT_EQ(거래아이템아이디, 결과아이템.id);
// 			EXPECT_EQ(아이템원본수량, 결과아이템.count);
		}
	}

	TEST_FIXTURE(FTradeMarket, PutOn_DivisionsOfItems)
	{
		const int64	거래아이템아이디	= 100;
		const int	아이템원본수량		= 10;
		const int	아이템슬롯번호		= 1;
		const int	등록갯수			= 4;
		const int	희망가격			= 50;
		const int	기간				= 3;
		const int	등록후인벤토리에남을아이템수량 = 아이템원본수량 - 등록갯수;

		PushInventoryTestItem(아이템슬롯번호, 거래아이템아이디, 아이템원본수량);
		ASSERT_EQ(아이템원본수량, itemHolder->GetItemCountInSlot(SLOTTYPE_INVENTORY, 아이템슬롯번호));

		EXPECT_EQ(true, GTradeMarket::PutOn(player->GetUID(), 아이템슬롯번호, 희망가격, 등록갯수, 기간));

		ASSERT_EQ(등록후인벤토리에남을아이템수량, itemHolder->GetItemCountInSlot(SLOTTYPE_INVENTORY, 아이템슬롯번호));
		ASSERT_EQ(1, playerLink->GetCommandCount());
		{
			MCommand& cmd = playerLink->GetCommand(0);
			EXPECT_EQ(MC_SC_TRADEMARKET_PUTTED_ON, cmd.GetID());
			EXPECT_EQ(3, cmd.GetParameterCount());

			int 커맨드에들어간슬롯번호 = 0;
			EXPECT_EQ(true, cmd.GetParameter(&커맨드에들어간슬롯번호, 0, MPT_INT));
			EXPECT_EQ(아이템슬롯번호, 커맨드에들어간슬롯번호);

			int 커맨드에들어간아이템갯수 = 0;
			EXPECT_EQ(true, cmd.GetParameter(&커맨드에들어간아이템갯수, 1, MPT_INT));
			EXPECT_EQ(등록갯수, 커맨드에들어간아이템갯수);

			int64 물품거래소디비일련번호 = 0;
			EXPECT_EQ(true, cmd.GetParameter(&물품거래소디비일련번호, 2, MPT_INT64));
			//EXPECT_TRUE(0 < 물품거래소디비일련번호);

			playerLink->ResetCommands();
		}
	}
} // SUITE(testTradeMarket)
