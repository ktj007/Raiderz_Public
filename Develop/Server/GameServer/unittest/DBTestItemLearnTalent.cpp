#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"


SUITE(DBTestItemLearnTalent)
{
	class FIXTURE
	{
	public :
		FIXTURE()
		{
			m_nAID = 1;
			m_nCID = DBTestHelper::InsertCharacter(m_nAID, GConfig::m_nMyWorldID, L"test_char");

			m_nItemID = 1;
			DBTestHelper::InsertItemInfo(m_nItemID, L"test_item");

			m_nSlotType = 1;
			m_nSlotID = 0;

			m_nIUID = DBTestHelper::InsertItem(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nItemID, 1, m_nSlotType, m_nSlotID);

			m_nTaltID_1 = 1;
			m_nTaltID_2 = 2;
			m_nTaltID_3 = 3;
			m_nTaltID_4 = 4;
			m_nTaltID_5 = 5;			

			DBTestHelper::InsertTalent(m_nTaltID_1, L"test_talent_1");
			DBTestHelper::InsertTalent(m_nTaltID_2, L"test_talent_2");
			DBTestHelper::InsertTalent(m_nTaltID_3, L"test_talent_3");
			DBTestHelper::InsertTalent(m_nTaltID_4, L"test_talent_4");
			DBTestHelper::InsertTalent(m_nTaltID_5, L"test_talent_5");			

			m_nCharPtm = 123;
		}

	public :
		int		m_nItemID;

		int		m_nTaltID_1;
		int		m_nTaltID_2;
		int		m_nTaltID_3;
		int		m_nTaltID_4;
		int		m_nTaltID_5;
		
		int64	m_nAID;
		int64	m_nCID;
		int64	m_nIUID;
		uint8	m_nSlotType;
		int16	m_nSlotID;
		int		m_nCharPtm;
	};

	void USP_RZ_ITEM_LEARN_TALENT(const int64 nAID, const int64 nCID, const uint8 SLOT_TYPE, const int16 SLOT_ID, const IUID nIUID, const int ITEM_ID
		, const int TALENT_ID_1, const int TALENT_ID_2 = 0, const int TALENT_ID_3 = 0, const int TALENT_ID_4 = 0, const int TALENT_ID_5 = 0)
	{
		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_LEARN_TALENT (%d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, SLOT_TYPE, SLOT_ID, nIUID, ITEM_ID, TALENT_ID_1, TALENT_ID_2, TALENT_ID_3, TALENT_ID_4, TALENT_ID_5));
	}

	void CHECK_ITEM_INSTANCE(const int64 nCID, const uint8 SLOT_TYPE, const int16 SLOT_ID)
	{
		TDBRecordSet rsItem;
		UTestDB.Execute(rsItem, L"SELECT ITEM_ID, IUID, STACK_AMT, SOUL_COUNT, DURA, MAX_DURA, COLOR, CLAIMED, CHAR_PTM, PERIOD, USAGE_PERIOD, EFF_END_DATE \
						 FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d;", nCID, SLOT_TYPE, SLOT_ID);
		CHECK_EQUAL(1, rsItem.GetFetchedRowCount());
		CHECK(rsItem.Field(L"ITEM_ID").IsNull());
		CHECK_EQUAL(0, rsItem.Field(L"IUID").AsInt64());
		CHECK_EQUAL(0, rsItem.Field(L"STACK_AMT").AsShort());
		CHECK_EQUAL(0, rsItem.Field(L"DURA").AsByte());
		CHECK_EQUAL(-1, rsItem.Field(L"COLOR").AsInt());
		CHECK(!rsItem.Field(L"CLAIMED").AsBool());
		CHECK(!rsItem.Field(L"PERIOD").AsBool());
		CHECK_EQUAL(0, rsItem.Field(L"USAGE_PERIOD").AsInt());
		CHECK(rsItem.Field(L"EFF_END_DATE").IsNull());
		rsItem.Close();
	}

	void CHECK_TALENT_INSTANCE_COUNT(const int64 CID, const int COUNT)
	{
		TDBRecordSet rsCount;
		UTestDB.Execute(rsCount, L"SELECT COUNT(*) AS CNT FROM dbo.RZ_TALENT WHERE CHAR_ID = %I64d;", CID);
		CHECK_EQUAL(1, rsCount.GetFetchedRowCount());
		CHECK_EQUAL(COUNT, rsCount.Field(L"CNT").AsInt());
		rsCount.Close();		
	}

	void CHECK_ITEM_LEARN_TALENT(const int64 CID, const int TALENT_ID)
	{
		TDBRecordSet rsTalent;
		UTestDB.Execute(rsTalent, L"SELECT COUNT(*) cnt FROM dbo.RZ_TALENT WHERE CHAR_ID = %I64d AND TALENT_ID = %d;", CID, TALENT_ID);
		CHECK_EQUAL(1, rsTalent.GetFetchedRowCount());
		CHECK_EQUAL(1, rsTalent.Field(L"cnt").AsInt());
		rsTalent.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_LEARN_TALENT_1)
	{
		USP_RZ_ITEM_LEARN_TALENT(m_nAID, m_nCID, m_nSlotType, m_nSlotID, m_nIUID, m_nItemID, m_nTaltID_1);	

		CHECK_ITEM_INSTANCE(m_nCID, m_nSlotType, m_nSlotID);
		CHECK_TALENT_INSTANCE_COUNT(m_nCID, 1);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_1);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_LEARN_TALENT_2)
	{
		USP_RZ_ITEM_LEARN_TALENT(m_nAID, m_nCID, m_nSlotType, m_nSlotID, m_nIUID, m_nItemID, m_nTaltID_1, m_nTaltID_2);	

		CHECK_ITEM_INSTANCE(m_nCID, m_nSlotType, m_nSlotID);
		CHECK_TALENT_INSTANCE_COUNT(m_nCID, 2);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_1);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_2);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_LEARN_TALENT_3)
	{
		USP_RZ_ITEM_LEARN_TALENT(m_nAID, m_nCID, m_nSlotType, m_nSlotID, m_nIUID, m_nItemID, m_nTaltID_1, m_nTaltID_2, m_nTaltID_3);	

		CHECK_ITEM_INSTANCE(m_nCID, m_nSlotType, m_nSlotID);
		CHECK_TALENT_INSTANCE_COUNT(m_nCID, 3);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_1);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_2);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_3);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_LEARN_TALENT_4)
	{
		USP_RZ_ITEM_LEARN_TALENT(m_nAID, m_nCID, m_nSlotType, m_nSlotID, m_nIUID, m_nItemID, m_nTaltID_1, m_nTaltID_2, m_nTaltID_3, m_nTaltID_4);	

		CHECK_ITEM_INSTANCE(m_nCID, m_nSlotType, m_nSlotID);
		CHECK_TALENT_INSTANCE_COUNT(m_nCID, 4);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_1);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_2);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_3);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_4);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_LEARN_TALENT_5)
	{
		USP_RZ_ITEM_LEARN_TALENT(m_nAID, m_nCID, m_nSlotType, m_nSlotID, m_nIUID, m_nItemID, m_nTaltID_1, m_nTaltID_2, m_nTaltID_3, m_nTaltID_4, m_nTaltID_5);	

		CHECK_ITEM_INSTANCE(m_nCID, m_nSlotType, m_nSlotID);
		CHECK_TALENT_INSTANCE_COUNT(m_nCID, 5);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_1);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_2);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_3);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_4);
		CHECK_ITEM_LEARN_TALENT(m_nCID, m_nTaltID_5);
	}
}