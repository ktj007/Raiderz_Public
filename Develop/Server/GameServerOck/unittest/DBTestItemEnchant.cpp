#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"


SUITE(RZ_ITEM_ENCHANT)
{
	class TEST_ENCH_ITEM
	{
	public :
		TEST_ENCH_ITEM() {}
		TEST_ENCH_ITEM(const int ITEM_ID, const int SLOT_TYPE, const int SLOT_ID, const int16 STACK_AMT, const int64 IUID)
			: m_ITEM_ID(ITEM_ID), m_SLOT_TYPE(SLOT_TYPE), m_SLOT_ID(SLOT_ID), m_STACK_AMT(STACK_AMT), m_IUID(IUID) {}

	public:
		int		m_ITEM_ID;
		int		m_SLOT_TYPE;
		int		m_SLOT_ID;
		int16	m_STACK_AMT;
		int64	m_IUID;
	};

	class FIXTURE
	{
	public :
		FIXTURE()
		{
			GSN					= 1;
			CSN					= DBTestHelper::InsertCharacter(GSN, GConfig::m_nMyWorldID);
			STACK_AMT			= 1;
			EN_DURA				= 14;
			EN_MAX_DURA			= 15;
			BUF_ID				= 2;
			FAIL_ITEM_ID			= -1;

			m_ES_ITEM = TEST_ENCH_ITEM(1, 1, 0, 1, DBTestHelper::InsertItem(GSN, CSN, GConfig::m_nMyWorldID, 1, STACK_AMT, 1, 0));
			m_EA_ITEM = TEST_ENCH_ITEM(2, 1, 1, 1, DBTestHelper::InsertItem(GSN, CSN, GConfig::m_nMyWorldID, 2, STACK_AMT, 1, 1));
			m_EN_ITEM = TEST_ENCH_ITEM(3, 1, 2, 1, DBTestHelper::InsertItem(GSN, CSN, GConfig::m_nMyWorldID, 3, STACK_AMT, 1, 2));
		}

		void ITEM_ENCHANT_SUCCESS(const int ENCH_SLOT_ID, const bool CLAIMED = true)
		{
			UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_ENCHANT (%d, %I64d, %I64d, %d, %I64d, %d, %I64d, %d, %d, %I64d, %d, %d, %d, %d, %d)}"
				, GConfig::m_nMyWorldID
				, GSN, CSN
				, m_ES_ITEM.m_SLOT_ID, m_ES_ITEM.m_IUID
				, m_EA_ITEM.m_SLOT_ID, m_EA_ITEM.m_IUID
				, m_EN_ITEM.m_SLOT_TYPE, m_EN_ITEM.m_SLOT_ID, m_EN_ITEM.m_IUID
				, EN_DURA, EN_MAX_DURA, ENCH_SLOT_ID, BUF_ID, CLAIMED);
		}

		void ITEM_ENCHANT_FAIL(const int ENCH_SLOT_ID, const bool CLAIMED = true)
		{
			UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_ENCHANT (%d, %I64d, %I64d, %d, %I64d, %d, %I64d, %d, %d, %I64d, %d, %d, %d, %d, %d)}"
				, GConfig::m_nMyWorldID
				, GSN, CSN
				, m_ES_ITEM.m_SLOT_ID, m_ES_ITEM.m_IUID
				, m_EA_ITEM.m_SLOT_ID, m_EA_ITEM.m_IUID
				, m_EN_ITEM.m_SLOT_TYPE, m_EN_ITEM.m_SLOT_ID, m_EN_ITEM.m_IUID
				, EN_DURA, EN_MAX_DURA, ENCH_SLOT_ID, -1, CLAIMED);
		}

		void CHECK_ENCHANT_STONE_ITEM()
		{
			TDBRecordSet rsER;
			UTestDB.Execute(rsER, L"SELECT IUID, ITEM_ID, CHAR_PTM FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d;"
				, CSN, m_ES_ITEM.m_SLOT_ID);
			CHECK_EQUAL(1, rsER.GetFetchedRowCount());
			CHECK_EQUAL(0, rsER.Field(L"IUID").AsInt64());
			CHECK(rsER.Field(L"ITEM_ID").IsNull());
			CHECK(rsER.Field(L"CHAR_PTM").IsNull());
			rsER.Close();
		}

		void CHECK_ENCHANT_AGENT_ITEM()
		{
			TDBRecordSet rsEA;
			UTestDB.Execute(rsEA, L"SELECT IUID, ITEM_ID, CHAR_PTM FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d;"
				, CSN, m_EA_ITEM.m_SLOT_ID);
			CHECK_EQUAL(1, rsEA.GetFetchedRowCount());
			CHECK_EQUAL(0, rsEA.Field(L"IUID").AsInt64());
			CHECK(rsEA.Field(L"ITEM_ID").IsNull());
			CHECK(rsEA.Field(L"CHAR_PTM").IsNull());
			rsEA.Close();
		}

		void CHECK_ENCHANT_ITEM(const int ENCH_SLOT_ID, const int EXPECT_BUF_ID, const bool CLAIMED = true)
		{
			int ENCH_BUF_ID[5] = {0,};
			ENCH_BUF_ID[ENCH_SLOT_ID] = EXPECT_BUF_ID;

			TDBRecordSet rsi;
			UTestDB.Execute(rsi, L"SELECT STACK_AMT, CLAIMED, DURA, MAX_DURA \
								   FROM dbo.RZ_INVEN \
								   WHERE OWNER_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d;"
				, CSN, m_EN_ITEM.m_SLOT_ID);
			CHECK_EQUAL(1, rsi.GetFetchedRowCount());
			CHECK_EQUAL(CLAIMED, rsi.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(STACK_AMT, rsi.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(EN_DURA, rsi.Field(L"DURA").AsByte());
			CHECK_EQUAL(EN_MAX_DURA, rsi.Field(L"MAX_DURA").AsByte());
			rsi.Close();

			TDBRecordSet rse;
			UTestDB.Execute(rse, L"SELECT ENCH_ITEM_ID_1, ENCH_ITEM_ID_2, ENCH_ITEM_ID_3, ENCH_ITEM_ID_4, ENCH_ITEM_ID_5 \
								   FROM dbo.RZ_ITEM_ENCHANT \
								   WHERE IUID = %I64d;"
								   , m_EN_ITEM.m_IUID);
			CHECK_EQUAL(1, rse.GetFetchedRowCount());			
			CHECK_EQUAL(ENCH_BUF_ID[0], rse.Field(L"ENCH_ITEM_ID_1").AsInt());
			CHECK_EQUAL(ENCH_BUF_ID[1], rse.Field(L"ENCH_ITEM_ID_2").AsInt());
			CHECK_EQUAL(ENCH_BUF_ID[2], rse.Field(L"ENCH_ITEM_ID_3").AsInt());
			CHECK_EQUAL(ENCH_BUF_ID[3], rse.Field(L"ENCH_ITEM_ID_4").AsInt());
			CHECK_EQUAL(ENCH_BUF_ID[4], rse.Field(L"ENCH_ITEM_ID_5").AsInt());
			rse.Close();
		}

	public :
		int64	GSN;
		int64	CSN;
		uint8	STACK_AMT;
		uint8	EN_DURA;
		uint8	EN_MAX_DURA;
		int		BUF_ID;
		int		FAIL_ITEM_ID;
		
		TEST_ENCH_ITEM m_ES_ITEM;	// 강화석.
		TEST_ENCH_ITEM m_EA_ITEM;	// 강화제.
		TEST_ENCH_ITEM m_EN_ITEM;	// 강화 아이템.
	};

	
	
	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_ENCHANT_1_SUCCESS)
	{
		const int ENCH_SLOT_ID  = 0;

		ITEM_ENCHANT_SUCCESS(ENCH_SLOT_ID );

		CHECK_ENCHANT_STONE_ITEM();
		CHECK_ENCHANT_AGENT_ITEM();
		CHECK_ENCHANT_ITEM(ENCH_SLOT_ID , BUF_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_ENCHANT_1_FAIL)
	{
		const int ENCH_SLOT_ID  = 0;

		ITEM_ENCHANT_FAIL(ENCH_SLOT_ID);

		CHECK_ENCHANT_STONE_ITEM();
		CHECK_ENCHANT_AGENT_ITEM();
		CHECK_ENCHANT_ITEM(ENCH_SLOT_ID, FAIL_ITEM_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_ENCHANT_2_SUCCESS)
	{
		const int ENCH_SLOT_ID  = 1;

		DBTestHelper::InsertEmptyItemEnch(GConfig::m_nMyWorldID, m_EN_ITEM.m_IUID);

		ITEM_ENCHANT_SUCCESS(ENCH_SLOT_ID );		

		CHECK_ENCHANT_STONE_ITEM();
		CHECK_ENCHANT_AGENT_ITEM();
		CHECK_ENCHANT_ITEM(ENCH_SLOT_ID , BUF_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_ENCHANT_2_FAIL)
	{
		const int ENCH_SLOT_ID  = 1;

		DBTestHelper::InsertEmptyItemEnch(GConfig::m_nMyWorldID, m_EN_ITEM.m_IUID);

		ITEM_ENCHANT_FAIL(ENCH_SLOT_ID);

		CHECK_ENCHANT_STONE_ITEM();
		CHECK_ENCHANT_AGENT_ITEM();
		CHECK_ENCHANT_ITEM(ENCH_SLOT_ID, FAIL_ITEM_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_ENCHANT_3_SUCCESS)
	{
		const int ENCH_SLOT_ID  = 2;

		DBTestHelper::InsertEmptyItemEnch(GConfig::m_nMyWorldID, m_EN_ITEM.m_IUID);

		ITEM_ENCHANT_SUCCESS(ENCH_SLOT_ID );

		CHECK_ENCHANT_STONE_ITEM();
		CHECK_ENCHANT_AGENT_ITEM();
		CHECK_ENCHANT_ITEM(ENCH_SLOT_ID , BUF_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_ENCHANT_3_FAIL)
	{
		const int ENCH_SLOT_ID  = 2;

		DBTestHelper::InsertEmptyItemEnch(GConfig::m_nMyWorldID, m_EN_ITEM.m_IUID);

		ITEM_ENCHANT_FAIL(ENCH_SLOT_ID);

		CHECK_ENCHANT_STONE_ITEM();
		CHECK_ENCHANT_AGENT_ITEM();
		CHECK_ENCHANT_ITEM(ENCH_SLOT_ID, FAIL_ITEM_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_ENCHANT_4_SUCCESS)
	{
		const int ENCH_SLOT_ID  = 3;

		DBTestHelper::InsertEmptyItemEnch(GConfig::m_nMyWorldID, m_EN_ITEM.m_IUID);

		ITEM_ENCHANT_SUCCESS(ENCH_SLOT_ID );

		CHECK_ENCHANT_STONE_ITEM();
		CHECK_ENCHANT_AGENT_ITEM();
		CHECK_ENCHANT_ITEM(ENCH_SLOT_ID , BUF_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_ENCHANT_4_FAIL)
	{
		const int ENCH_SLOT_ID  = 3;

		DBTestHelper::InsertEmptyItemEnch(GConfig::m_nMyWorldID, m_EN_ITEM.m_IUID);

		ITEM_ENCHANT_FAIL(ENCH_SLOT_ID);

		CHECK_ENCHANT_STONE_ITEM();
		CHECK_ENCHANT_AGENT_ITEM();
		CHECK_ENCHANT_ITEM(ENCH_SLOT_ID, FAIL_ITEM_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_ENCHANT_5_SUCCESS)
	{
		const int ENCH_SLOT_ID  = 4;

		DBTestHelper::InsertEmptyItemEnch(GConfig::m_nMyWorldID, m_EN_ITEM.m_IUID);

		ITEM_ENCHANT_SUCCESS(ENCH_SLOT_ID );

		CHECK_ENCHANT_STONE_ITEM();
		CHECK_ENCHANT_AGENT_ITEM();
		CHECK_ENCHANT_ITEM(ENCH_SLOT_ID , BUF_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_ENCHANT_5_FAIL)
	{
		const int ENCH_SLOT_ID  = 4;

		DBTestHelper::InsertEmptyItemEnch(GConfig::m_nMyWorldID, m_EN_ITEM.m_IUID);

		ITEM_ENCHANT_FAIL(ENCH_SLOT_ID);

		CHECK_ENCHANT_STONE_ITEM();
		CHECK_ENCHANT_AGENT_ITEM();
		CHECK_ENCHANT_ITEM(ENCH_SLOT_ID, FAIL_ITEM_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_ENCHANT_REMAIN_ENCHANT_AGENT)
	{
		int16 EA_STACK_AMT = 5;
		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET STACK_AMT = %d WHERE OWNER_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d;"
			, EA_STACK_AMT, CSN, m_EA_ITEM.m_SLOT_ID);

		const int ENCH_SLOT_ID  = 0;

		ITEM_ENCHANT_SUCCESS(ENCH_SLOT_ID);

		CHECK_ENCHANT_STONE_ITEM();
		CHECK_ENCHANT_ITEM(ENCH_SLOT_ID , BUF_ID);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT STACK_AMT FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d AND IUID = %I64d;"
			, CSN, m_EA_ITEM.m_SLOT_ID, m_EA_ITEM.m_IUID);
		CHECK_EQUAL(EA_STACK_AMT - 1, rs.Field(L"STACK_AMT").AsShort());
		rs.Close();
	}
}
