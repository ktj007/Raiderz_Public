#include "stdafx.h"
#include "GTestForward.h"
#include "GLootSystem.h"
#include "GLootSystemForMasterServer.h"
#include "CTransData.h"

SUITE(LootSystemForMasterServer)
{
	struct Fixture
	{
		Fixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pBeneficiary = test.player.NewPlayer(m_pField);
		}

		MockField*			m_pField;
		GEntityPlayer*		m_pBeneficiary;
	};

	TEST_FIXTURE(Fixture, MasterLootNotify)
	{
		vector<int> vecBeneficiaryCID;		
		vecBeneficiaryCID.push_back(m_pBeneficiary->GetCID());		
		vector<TD_LOOT_MASTERLOOT_NOTIFY> vecTDLootMasterLootNotify;
		vecTDLootMasterLootNotify.push_back(TD_LOOT_MASTERLOOT_NOTIFY(GUnitTestUtil::NewID(), GUnitTestUtil::NewNumber()));

		MockLink* pBeneficiaryLink = test.network.NewLink(m_pBeneficiary);

		gsys.pLootSystem->ForMasterServer().MasterLootNotify(vecBeneficiaryCID, vecTDLootMasterLootNotify);
	
		vector<TD_LOOT_MASTERLOOT_NOTIFY>	vecTDLootMasterLootNotify_CMD;

		CHECK_EQUAL(1, pBeneficiaryLink->GetCommandCount());
		ASSERT_EQUAL(MC_LOOT_MASTERLOOT_NOTIFY, pBeneficiaryLink->GetCommand(0).GetID());	
		pBeneficiaryLink->GetCommand(0).GetBlob(vecTDLootMasterLootNotify_CMD, 0);
		CHECK(vecTDLootMasterLootNotify.front() == vecTDLootMasterLootNotify_CMD.front());
	}

	TEST_FIXTURE(Fixture, OtherGainItem)
	{
		vector<int> vecBeneficiaryCID;		
		vecBeneficiaryCID.push_back(m_pBeneficiary->GetCID());
		MUID nGainnerUID = GUnitTestUtil::NewUID();
		vector<TD_LOOT_OTHERGAIN_ITEM> vecTDOtherGainItem;
		vecTDOtherGainItem.push_back(TD_LOOT_OTHERGAIN_ITEM(GUnitTestUtil::NewID(), GUnitTestUtil::NewNumber()));

		MockLink* pBeneficiaryLink = test.network.NewLink(m_pBeneficiary);

		gsys.pLootSystem->ForMasterServer().OtherGainItem(vecBeneficiaryCID, nGainnerUID, vecTDOtherGainItem);

		
		CHECK_EQUAL(1, pBeneficiaryLink->GetCommandCount());
		ASSERT_EQUAL(MC_LOOT_OTHER_GAIN_ITEM, pBeneficiaryLink->GetCommand(0).GetID());	
		MUID nGainnerUID_CMD;
		pBeneficiaryLink->GetCommand(0).GetParameter(&nGainnerUID_CMD, 0, MPT_UID);
		vector<TD_LOOT_OTHERGAIN_ITEM>	vecTDOtherGainItem_CMD;
		pBeneficiaryLink->GetCommand(0).GetBlob(vecTDOtherGainItem_CMD, 1);
		CHECK(vecTDOtherGainItem.front() == vecTDOtherGainItem_CMD.front());
	}

	TEST_FIXTURE(Fixture, GettableItemAdd)
	{
		vector<TD_LOOT_GETTABLE_ITEM_ADD> vecTDGettableItemAdd;
		vecTDGettableItemAdd.push_back(TD_LOOT_GETTABLE_ITEM_ADD(GUnitTestUtil::NewID(), GUnitTestUtil::NewNumber(), UIR_OTHERSERVER));

		MockLink* pBeneficiaryLink = test.network.NewLink(m_pBeneficiary);

		gsys.pLootSystem->ForMasterServer().GettableItemAdd(m_pBeneficiary->GetCID(), vecTDGettableItemAdd);

		CHECK_EQUAL(1, pBeneficiaryLink->GetCommandCount());
		ASSERT_EQUAL(MC_LOOT_GETTABLE_ITEM_ADD, pBeneficiaryLink->GetCommand(0).GetID());	
		vector<TD_LOOT_GETTABLE_ITEM_ADD>	vecTDGettableItemAdd_CMD;
		pBeneficiaryLink->GetCommand(0).GetBlob(vecTDGettableItemAdd_CMD, 0);
		CHECK(vecTDGettableItemAdd.front() == vecTDGettableItemAdd_CMD.front());
	}

	TEST_FIXTURE(Fixture, RollResult)
	{
		vector<int> vecBeneficiaryCID;		
		vecBeneficiaryCID.push_back(m_pBeneficiary->GetCID());
		vector<TD_LOOT_ROLL_ITEM> vecTDRollItem;		
		vecTDRollItem.push_back(TD_LOOT_ROLL_ITEM(GUnitTestUtil::NewNumber(), GUnitTestUtil::NewID(), GUnitTestUtil::NewNumber()));
		vector<TD_LOOT_ROLL_RESULT> vecTDRollResult;
		vecTDRollResult.push_back(TD_LOOT_ROLL_RESULT(GUnitTestUtil::NewNumber(), GUnitTestUtil::NewUID(), GUnitTestUtil::NewNumber()));

		MockLink* pBeneficiaryLink = test.network.NewLink(m_pBeneficiary);

		gsys.pLootSystem->ForMasterServer().RollResult(vecBeneficiaryCID, vecTDRollItem, vecTDRollResult);

		CHECK_EQUAL(1, pBeneficiaryLink->GetCommandCount());
		ASSERT_EQUAL(MC_LOOT_ROLLRESULT, pBeneficiaryLink->GetCommand(0).GetID());	

		vector<TD_LOOT_ROLL_ITEM>	vecTDRollItem_CMD;
		pBeneficiaryLink->GetCommand(0).GetBlob(vecTDRollItem_CMD, 0);
		CHECK(vecTDRollItem.front() == vecTDRollItem_CMD.front());

		vector<TD_LOOT_ROLL_RESULT>	vecTDRollResult_CMD;
		pBeneficiaryLink->GetCommand(0).GetBlob(vecTDRollResult_CMD, 1);
		CHECK(vecTDRollResult.front() == vecTDRollResult_CMD.front());
	}
}