#include "stdafx.h"
#include "ZPlayer.h"
#include "ZFixtureHelper.h"
#include "ZGameServerObject.h"
#include "ZPlayerLogic.h"
#include "CTransData.h"
#include "ZLootLogic.h"
#include "CCommandTable_Master.h"
#include "SCommandTable.h"
#include "AStlUtil.h"

SUITE(LootLogic)
{
	struct Fixture : public FBaseServerField, public FBasePlayer, public FBaseMockLink
	{
		Fixture()
		{
			m_pReceiveGameServer = AddGameServerObject();
			m_pReceiveGameServerLink = NewLink(m_pReceiveGameServer->GetUID());
			m_pReceiveGameServerLink->AddIgnoreID(MSC_PROXY_PLAYER_UPDATE);
			
			m_pReceiver = AddNewPlayerInWorld(m_pReceiveGameServer->GetID());
		}

		~Fixture()
		{

		}

		ZGameServerObject* m_pReceiveGameServer;
		MockLink*	m_pReceiveGameServerLink;

		ZPlayer* m_pReceiver;
	};

	TEST_FIXTURE(Fixture, MasterLootNotifyReq)
	{
		vector<int> vecBeneficiaryCID;
		vecBeneficiaryCID.push_back(m_pReceiver->GetCID());
		vector<TD_LOOT_MASTERLOOT_NOTIFY>	vecTDLootMasterLootNotify;
		vecTDLootMasterLootNotify.push_back(TD_LOOT_MASTERLOOT_NOTIFY(ZTestHelper::NewID(), ZTestHelper::NewID()));

		gsys.pLootLogic->MasterLootNotifyReq(vecBeneficiaryCID, vecTDLootMasterLootNotify);

		ASSERT_EQUAL(MMC_LOOT_MASTERLOOT_NOTIFY, m_pReceiveGameServerLink->GetCommandID(0));

		vector<int> vecBeneficiaryCID_CMD;
		ASSERT_EQUAL(true, m_pReceiveGameServerLink->GetCommand(0).GetBlob(vecBeneficiaryCID_CMD, 0));
		CHECK_EQUAL(1, vecBeneficiaryCID_CMD.size());		
		CHECK_EQUAL(vecBeneficiaryCID.front(), vecBeneficiaryCID_CMD.front());

		vector<TD_LOOT_MASTERLOOT_NOTIFY> vecTDLootMasterLootNotify_CMD;
		ASSERT_EQUAL(true, m_pReceiveGameServerLink->GetCommand(0).GetBlob(vecTDLootMasterLootNotify_CMD, 1));
		CHECK(vecTDLootMasterLootNotify.front() == vecTDLootMasterLootNotify_CMD.front());
	}

	TEST_FIXTURE(Fixture, OtherGainitemReq)
	{
		vector<int> vecBeneficiaryCID;
		vecBeneficiaryCID.push_back(m_pReceiver->GetCID());
		MUID nGainerUID = ZTestHelper::NewUID();
		vector<TD_LOOT_OTHERGAIN_ITEM>	vecTDOtherGainItem;
		vecTDOtherGainItem.push_back(TD_LOOT_OTHERGAIN_ITEM(ZTestHelper::NewID(), ZTestHelper::NewNumber()));

		gsys.pLootLogic->OtherGainitemReq(vecBeneficiaryCID, nGainerUID, vecTDOtherGainItem);

		ASSERT_EQUAL(MMC_LOOT_OTHER_GAIN_ITEM, m_pReceiveGameServerLink->GetCommandID(0));

		vector<int> vecBeneficiaryCID_CMD;
		ASSERT_EQUAL(true, m_pReceiveGameServerLink->GetCommand(0).GetBlob(vecBeneficiaryCID_CMD, 0));
		CHECK_EQUAL(1, vecBeneficiaryCID_CMD.size());		
		CHECK_EQUAL(vecBeneficiaryCID.front(), vecBeneficiaryCID_CMD.front());

		MUID nGainerUID_CMD;
		ASSERT_EQUAL(true, m_pReceiveGameServerLink->GetCommand(0).GetParameter(&nGainerUID_CMD, 1, MPT_UID));
		CHECK_EQUAL(nGainerUID, nGainerUID_CMD);

		vector<TD_LOOT_OTHERGAIN_ITEM> vecTDOtherGainItem_CMD;
		ASSERT_EQUAL(true, m_pReceiveGameServerLink->GetCommand(0).GetBlob(vecTDOtherGainItem_CMD, 2));
		CHECK(vecTDOtherGainItem.front() == vecTDOtherGainItem_CMD.front());
	}

	TEST_FIXTURE(Fixture, GettableItemAddReq)
	{
		vector<TD_LOOT_GETTABLE_ITEM_ADD>	vecTDGettableItemAdd;
		vecTDGettableItemAdd.push_back(TD_LOOT_GETTABLE_ITEM_ADD(ZTestHelper::NewID(), ZTestHelper::NewNumber(), UIR_OTHERSERVER));

		gsys.pLootLogic->GettableItemAddReq(m_pReceiver->GetCID(), vecTDGettableItemAdd);

		ASSERT_EQUAL(MMC_LOOT_GETTABLE_ITEM_ADD, m_pReceiveGameServerLink->GetCommandID(0));

		int nBeneficiary_CMD;
		ASSERT_EQUAL(true, m_pReceiveGameServerLink->GetCommand(0).GetParameter(&nBeneficiary_CMD, 0, MPT_INT));
		CHECK_EQUAL(m_pReceiver->GetCID(), nBeneficiary_CMD);

		vector<TD_LOOT_GETTABLE_ITEM_ADD> vecTDGettableItemAdd_CMD;
		ASSERT_EQUAL(true, m_pReceiveGameServerLink->GetCommand(0).GetBlob(vecTDGettableItemAdd_CMD, 1));
		CHECK(vecTDGettableItemAdd.front() == vecTDGettableItemAdd_CMD.front());
	}

	TEST_FIXTURE(Fixture, RollResultReq)
	{
		vector<int> vecBeneficiaryCID;
		vecBeneficiaryCID.push_back(m_pReceiver->GetCID());
		vector<TD_LOOT_ROLL_ITEM> vecTDRollItem;
		vector<TD_LOOT_ROLL_RESULT> vecTDRollResult;
		vecTDRollItem.push_back(TD_LOOT_ROLL_ITEM(ZTestHelper::NewNumber(), ZTestHelper::NewID(), ZTestHelper::NewNumber()));
		vecTDRollResult.push_back(TD_LOOT_ROLL_RESULT(ZTestHelper::NewNumber(), ZTestHelper::NewUID(), ZTestHelper::NewNumber()));

		gsys.pLootLogic->RollResultReq(vecBeneficiaryCID, vecTDRollItem, vecTDRollResult);

		ASSERT_EQUAL(MMC_LOOT_ROLLRESULT, m_pReceiveGameServerLink->GetCommandID(0));

		vector<int> vecBeneficiaryCID_CMD;
		ASSERT_EQUAL(true, m_pReceiveGameServerLink->GetCommand(0).GetBlob(vecBeneficiaryCID_CMD, 0));
		CHECK_EQUAL(1, vecBeneficiaryCID_CMD.size());		
		CHECK_EQUAL(vecBeneficiaryCID.front(), vecBeneficiaryCID_CMD.front());

		vector<TD_LOOT_ROLL_ITEM> vecTDRollItem_CMD;
		ASSERT_EQUAL(true, m_pReceiveGameServerLink->GetCommand(0).GetBlob(vecTDRollItem_CMD, 1));
		CHECK(vecTDRollItem.front() == vecTDRollItem_CMD.front());

		vector<TD_LOOT_ROLL_RESULT> vecTDRollResult_CMD;
		ASSERT_EQUAL(true, m_pReceiveGameServerLink->GetCommand(0).GetBlob(vecTDRollResult_CMD, 2));
		CHECK(vecTDRollResult.front() == vecTDRollResult_CMD.front());
	}
}