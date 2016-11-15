#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"

namespace dbtest_inven
{
	//class T_ITEM
	//{
	//public :
	//	T_ITEM(int64 gsn, int64 csn, int slot_id, int item_id, int16 bef_stack_amt, int16 next_stack_amt, int color, uint8 dura, uint8 max_dura, uint8 soul_count)
	//		: GSN(gsn), CSN(csn), SLOT_ID(slot_id), ITEM_ID(item_id), COLOR(color), DURA(dura), MAX_DURA(max_dura)
	//		, BEF_STACK_AMT(bef_stack_amt), NEXT_STACK_AMT(next_stack_amt), SOUL_COUNT(soul_count), IUID(0)
	//	{

	//	}

	//	void InsertInven()
	//	{
	//		IUID = DBTestHelper::InsertItem(GSN, CSN, GConfig::m_nMyWorldID, ITEM_ID, BEF_STACK_AMT, 1, SLOT_ID, false, 0, -1);
	//		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET DURA = %d, MAX_DURA = %d, COLOR = %d, SOUL_COUNT = %d WHERE IUID = %I64d", DURA, MAX_DURA, COLOR, SOUL_COUNT, IUID);
	//	}

	//	void InsertEmptyInven()
	//	{
	//		DBTestHelper::InsertEmptyItemSlot(GSN, CSN, GConfig::m_nMyWorldID, 1, SLOT_ID);
	//	}

	//	void CheckInven(int64 gsn, int64 csn, int slot_id, T_ITEM& item)
	//	{
	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"SELECT IUID, ITEM_ID, COLOR, STACK_AMT, DURA, MAX_DURA, SOUL_COUNT \
	//							 FROM dbo.RZ_INVEN \
	//							 WHERE WORLD = %d AND GSN = %I64d AND CHAR_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d"
	//							 , GConfig::m_nMyWorldID, gsn, csn, slot_id);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		CHECK_EQUAL(item.IUID, rs.Field(L"IUID").AsInt64());
	//		CHECK_EQUAL(item.ITEM_ID, rs.Field(L"ITEM_ID").AsInt());
	//		CHECK_EQUAL(item.COLOR, rs.Field(L"COLOR").AsInt());
	//		CHECK_EQUAL(item.NEXT_STACK_AMT, rs.Field(L"STACK_AMT").AsShort());			
	//		CHECK_EQUAL(item.DURA, rs.Field(L"DURA").AsByte());
	//		CHECK_EQUAL(item.MAX_DURA, rs.Field(L"MAX_DURA").AsByte());
	//		CHECK_EQUAL(item.SOUL_COUNT, rs.Field(L"SOUL_COUNT").AsInt());
	//		rs.Close();
	//	}

	//	void CheckEmptyInven(int64 gsn, int64 csn, int slot_id)
	//	{
	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"SELECT IUID, ITEM_ID, COLOR, STACK_AMT, DURA, MAX_DURA, SOUL_COUNT \
	//							 FROM dbo.RZ_INVEN \
	//							 WHERE WORLD = %d AND GSN = %I64d AND CHAR_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d"
	//							 , GConfig::m_nMyWorldID, gsn, csn, slot_id);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		CHECK_EQUAL((int64)0, rs.Field(L"IUID").AsInt64());
	//		CHECK(rs.Field(L"ITEM_ID").IsNull());
	//		CHECK_EQUAL(0, rs.Field(L"COLOR").AsInt());
	//		CHECK_EQUAL(0, rs.Field(L"STACK_AMT").AsShort());			
	//		CHECK_EQUAL(0, rs.Field(L"DURA").AsByte());
	//		CHECK_EQUAL(0, rs.Field(L"MAX_DURA").AsByte());
	//		CHECK_EQUAL(0, rs.Field(L"SOUL_COUNT").AsInt());
	//		rs.Close();
	//	}

	//	int64	GSN;
	//	int64	CSN;
	//	int		SLOT_ID;
	//	int64	IUID;
	//	int		ITEM_ID;
	//	int		COLOR;
	//	int16	BEF_STACK_AMT;
	//	int16	NEXT_STACK_AMT;
	//	uint8	DURA;
	//	uint8	MAX_DURA;
	//	uint8	SOUL_COUNT;
	//};

	//const int64 GSN	= 1;
	//const int64 CSN	= 2;

	//SUITE (USP_RZ_INVEN_SWAP_SLOT)
	//{
	//	GAMEDB_TEST(USP_RZ_INVEN_SWAP_SLOT_1)
	//	{
	//		T_ITEM ivf (GSN, CSN, 1, 2, 3, 4, 5, 6, 7, 8);
	//		T_ITEM ivt (GSN, CSN, 11, 12, 13, 14, 15, 16, 17, 18);

	//		ivf.InsertInven();
	//		ivt.InsertInven();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_INVEN_SWAP_SLOT (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckInven(GSN, CSN, ivf.SLOT_ID, ivt);
	//		ivt.CheckInven(GSN, CSN, ivt.SLOT_ID, ivf);
	//	}		

	//	GAMEDB_TEST(USP_RZ_INVEN_SWAP_SLOT_2)
	//	{
	//		T_ITEM ivf (GSN, CSN, 1, 2, 3, 4, 5, 6, 7, 8);
	//		T_ITEM ivt (GSN, CSN, 11, 0, 0, 0, 0, 0, 0, 0);

	//		ivf.InsertInven();
	//		ivt.InsertEmptyInven();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_INVEN_SWAP_SLOT (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckEmptyInven(GSN, CSN, ivf.SLOT_ID);
	//		ivt.CheckInven(GSN, CSN, ivt.SLOT_ID, ivf);
	//	}		

	//	GAMEDB_TEST(USP_RZ_INVEN_SWAP_SLOT_3)
	//	{
	//		T_ITEM ivf (GSN, CSN, 1, 0, 0, 0, 0, 0, 0, 0);
	//		T_ITEM ivt (GSN, CSN, 11, 12, 13, 14, 15, 16, 17, 18);

	//		ivf.InsertEmptyInven();
	//		ivt.InsertInven();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_INVEN_SWAP_SLOT (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckInven(GSN, CSN, ivf.SLOT_ID, ivt);
	//		ivt.CheckEmptyInven(GSN, CSN, ivt.SLOT_ID);
	//	}	
	//}

	//SUITE (USP_RZ_INVEN_MERGE)
	//{
	//	GAMEDB_TEST(USP_RZ_INVEN_MERGE_1_1)
	//	{
	//		T_ITEM ivf (GSN, CSN, 1, 2, 3, 1, 5, 0, 0, 0);
	//		T_ITEM ivt (GSN, CSN, 11, 12, 13, 15, 15, 0, 0, 0);

	//		ivf.InsertInven();
	//		ivt.InsertInven();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_INVEN_MERGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckInven(GSN, CSN, ivf.SLOT_ID, ivf);
	//		ivt.CheckInven(GSN, CSN, ivt.SLOT_ID, ivt);
	//	}

	//	GAMEDB_TEST(USP_RZ_INVEN_MERGE_1_2)
	//	{
	//		T_ITEM ivf (GSN, CSN, 1, 2, 3, 13, 5, 0, 0, 0);
	//		T_ITEM ivt (GSN, CSN, 11, 12, 13, 3, 15, 0, 0, 0);

	//		ivf.InsertInven();
	//		ivt.InsertInven();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_INVEN_MERGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckInven(GSN, CSN, ivf.SLOT_ID, ivf);
	//		ivt.CheckInven(GSN, CSN, ivt.SLOT_ID, ivt);
	//	}

	//	GAMEDB_TEST(USP_RZ_INVEN_MERGE_2_1)
	//	{
	//		T_ITEM ivf (GSN, CSN, 1, 2, 3, 16, 5, 0, 0, 0);
	//		T_ITEM ivt (GSN, CSN, 11, 12, 13, 0, 15, 0, 0, 0);

	//		ivf.InsertInven();
	//		ivt.InsertInven();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_INVEN_MERGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckInven(GSN, CSN, ivf.SLOT_ID, ivf);
	//		ivt.CheckEmptyInven(GSN, CSN, ivt.SLOT_ID);
	//	}

	//	GAMEDB_TEST(USP_RZ_INVEN_MERGE_2_2)
	//	{
	//		T_ITEM ivf (GSN, CSN, 1, 2, 3, 0, 5, 0, 0, 0);
	//		T_ITEM ivt (GSN, CSN, 11, 12, 13, 16, 15, 0, 0, 0);

	//		ivf.InsertInven();
	//		ivt.InsertInven();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_INVEN_MERGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckEmptyInven(GSN, CSN, ivf.SLOT_ID);
	//		ivt.CheckInven(GSN, CSN, ivt.SLOT_ID, ivt);
	//	}
	//}

	//SUITE (USP_RZ_INVEN_SPLIT)
	//{
	//	GAMEDB_TEST(USP_RZ_INVEN_SPLIT_1_1)
	//	{
	//		T_ITEM ivf (GSN, CSN, 1, 2, 3, 1, 5, 0, 0, 0);
	//		T_ITEM ivt (GSN, CSN, 11, 0, 0, 2, 0, 0, 0, 0);

	//		ivf.InsertInven();
	//		ivt.InsertEmptyInven();
	//
	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_INVEN_SPLIT (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_INVEN_IUID").AsInt64();
	//		CHECK(0 < NEW_IUID);
	//		rs.Close();

	//		T_ITEM sp (GSN, CSN, ivt.SLOT_ID, ivf.ITEM_ID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT, ivf.COLOR, 0, 0, 0);
	//		sp.IUID = NEW_IUID;

	//		ivf.CheckInven(GSN, CSN, ivf.SLOT_ID, ivf);
	//		ivt.CheckInven(GSN, CSN, sp.SLOT_ID, sp);
	//	}	

	//	GAMEDB_TEST(USP_RZ_INVEN_SPLIT_1_2)
	//	{
	//		T_ITEM ivf (GSN, CSN, 1, 2, 3, 1, 5, 0, 0, 0);
	//		T_ITEM ivt (GSN, CSN, 11, 0, 0, 2, 0, 0, 0, 0);

	//		ivf.InsertInven();
	//		
	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_INVEN_SPLIT (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_INVEN_IUID").AsInt64();
	//		CHECK(0 < NEW_IUID);
	//		rs.Close();

	//		T_ITEM sp (GSN, CSN, ivt.SLOT_ID, ivf.ITEM_ID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT, ivf.COLOR, 0, 0, 0);
	//		sp.IUID = NEW_IUID;

	//		ivf.CheckInven(GSN, CSN, ivf.SLOT_ID, ivf);
	//		ivt.CheckInven(GSN, CSN, sp.SLOT_ID, sp);
	//	}
	//}
}

