#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"

SUITE(DBTEST_ITEM_SPLIT)
{
	class IS_ITEM
	{
	public :
		IS_ITEM(int64 GSN, int64 CSN, int SLOT_TYPE, int SLOT_ID, int ITEM_ID, int BEF_STACK_AMT, int NEXT_TACK_AMT, int CHAR_PTM)
			: m_GSN(GSN), m_CSN(CSN), m_SLOT_TYPE(SLOT_TYPE), m_SLOT_ID(SLOT_ID), m_IUID(0), m_ITEM_ID(ITEM_ID)
			, m_BEF_STACK_AMT(BEF_STACK_AMT), m_NEXT_STACK_AMT(NEXT_TACK_AMT), m_CHAR_PTM(CHAR_PTM)
		{
		
		}

		void Insert()
		{
			m_IUID = DBTestHelper::InsertItem(m_GSN, m_CSN, GConfig::m_nMyWorldID, m_ITEM_ID, m_BEF_STACK_AMT, m_SLOT_TYPE, m_SLOT_ID);
			UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET DURA = 0, MAX_DURA = 0, CHAR_PTM = %d, PERIOD = 0, USAGE_PERIOD = 0, EFF_END_DATE = NULL WHERE IUID = %I64d;"
				, m_CHAR_PTM, m_IUID);
		}

		void InsertEmptySlot()
		{
			DBTestHelper::InsertEmptyItemSlot(m_GSN, m_CSN, GConfig::m_nMyWorldID, m_SLOT_TYPE, m_SLOT_ID);
		}

		void Check(int CHAR_PTM)
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT ITEM_ID, STACK_AMT, CHAR_PTM FROM dbo.RZ_INVEN WHERE ACCN_ID = %I64d AND OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d AND IUID = %I64d;"
				, m_GSN, m_CSN, m_SLOT_TYPE, m_SLOT_ID, m_IUID);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(m_ITEM_ID, rs.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(m_NEXT_STACK_AMT, rs.Field(L"STACK_AMT").AsInt());			
			CHECK_EQUAL(CHAR_PTM, rs.Field(L"CHAR_PTM").AsInt());
			rs.Close();
		}

		int64	m_GSN;
		int64	m_CSN;
		int		m_SLOT_TYPE;
		int		m_SLOT_ID;
		int64	m_IUID;
		int		m_ITEM_ID;
		int		m_BEF_STACK_AMT;
		int		m_NEXT_STACK_AMT;
		int		m_CHAR_PTM;
	};

	GAMEDB_TEST(USP_RZ_ITEM_SPLIT_TO_NOT_EXIST_SLOT)
	{
		const int ITEM_ID		= 5;
		const int CHAR_PTM		= 10;
		const int NEW_CHAR_PTM	= 20;

		IS_ITEM ITEM_FROM(1, 2, 1, 4, ITEM_ID, 4, 2, CHAR_PTM);
		IS_ITEM ITEM_TO(2, 3, 4, 2, ITEM_ID, 0, 2, CHAR_PTM);

		ITEM_FROM.Insert();

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_ITEM_SPLIT (%d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %I64d, %I64d, %d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID
			, ITEM_FROM.m_GSN, ITEM_FROM.m_CSN, ITEM_FROM.m_SLOT_TYPE, ITEM_FROM.m_SLOT_ID, ITEM_FROM.m_IUID, ITEM_FROM.m_BEF_STACK_AMT, ITEM_FROM.m_NEXT_STACK_AMT
			, ITEM_TO.m_GSN, ITEM_TO.m_CSN, ITEM_TO.m_SLOT_TYPE, ITEM_TO.m_SLOT_ID, ITEM_TO.m_NEXT_STACK_AMT, NEW_CHAR_PTM);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		ITEM_TO.m_IUID = rs.Field(L"NEW_IUID").AsInt64();
		CHECK(0 < ITEM_TO.m_IUID);
		rs.Close();

		ITEM_FROM.Check(CHAR_PTM);
		ITEM_TO.Check(0);
	}

	GAMEDB_TEST(USP_RZ_ITEM_SPLIT_STORAGE_TO_INVEN)
	{
		const int ITEM_ID		= 5;
		const int CHAR_PTM		= 10;
		const int NEW_CHAR_PTM	= 20;

		IS_ITEM ITEM_FROM(1, 2, 4, 4, ITEM_ID, 4, 2, 0);
		IS_ITEM ITEM_TO(2, 3, 1, 2, ITEM_ID, 0, 2, 0);

		ITEM_FROM.Insert();

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_ITEM_SPLIT (%d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %I64d, %I64d, %d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID
			, ITEM_FROM.m_GSN, ITEM_FROM.m_CSN, ITEM_FROM.m_SLOT_TYPE, ITEM_FROM.m_SLOT_ID, ITEM_FROM.m_IUID, ITEM_FROM.m_BEF_STACK_AMT, ITEM_FROM.m_NEXT_STACK_AMT
			, ITEM_TO.m_GSN, ITEM_TO.m_CSN, ITEM_TO.m_SLOT_TYPE, ITEM_TO.m_SLOT_ID, ITEM_TO.m_NEXT_STACK_AMT, NEW_CHAR_PTM);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		ITEM_TO.m_IUID = rs.Field(L"NEW_IUID").AsInt64();
		CHECK(0 < ITEM_TO.m_IUID);
		rs.Close();

		ITEM_FROM.Check(0);
		ITEM_TO.Check(NEW_CHAR_PTM);
	}

	GAMEDB_TEST(USP_RZ_ITEM_SPLIT_TO_EMPTY_SLOT)
	{
		const int ITEM_ID	= 5;
		const int CHAR_PTM	= 10;
		const int NEW_CHAR_PTM	= 20;

		IS_ITEM ITEM_FROM(1, 2, 1, 4, ITEM_ID, 4, 2, CHAR_PTM);
		IS_ITEM ITEM_TO(2, 3, 4, 2, ITEM_ID, 0, 2, CHAR_PTM);

		ITEM_FROM.Insert();
		ITEM_TO.InsertEmptySlot();

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_ITEM_SPLIT (%d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %I64d, %I64d, %d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID
			, ITEM_FROM.m_GSN, ITEM_FROM.m_CSN, ITEM_FROM.m_SLOT_TYPE, ITEM_FROM.m_SLOT_ID, ITEM_FROM.m_IUID, ITEM_FROM.m_BEF_STACK_AMT, ITEM_FROM.m_NEXT_STACK_AMT
			, ITEM_TO.m_GSN, ITEM_TO.m_CSN, ITEM_TO.m_SLOT_TYPE, ITEM_TO.m_SLOT_ID, ITEM_TO.m_NEXT_STACK_AMT, NEW_CHAR_PTM);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		ITEM_TO.m_IUID = rs.Field(L"NEW_IUID").AsInt64();
		CHECK(0 < ITEM_TO.m_IUID);
		rs.Close();

		ITEM_FROM.Check(CHAR_PTM);
		ITEM_TO.Check(0);
	}	
}