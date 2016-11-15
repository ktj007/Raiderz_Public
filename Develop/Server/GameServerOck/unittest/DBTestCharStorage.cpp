#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"

namespace dbtest_char_storage 
{
	//const int64 GSN = 1;
	//const int64 CHAR_ID = 2;

	//class T_ITEM
	//{
	//public :
	//	T_ITEM(int64 gsn, int64 csn, int slot_id, int item_id, int16 bef_stack_amt, int16 next_stack_amt, int color, uint8 dura, uint8 max_dura, uint8 soul_count
	//		, uint8 claimed, int char_ptm)
	//		: GSN(gsn), CSN(csn), SLOT_ID(slot_id), ITEM_ID(item_id), COLOR(color), DURA(dura), MAX_DURA(max_dura)
	//		, BEF_STACK_AMT(bef_stack_amt), NEXT_STACK_AMT(next_stack_amt), SOUL_COUNT(soul_count), IUID(0)
	//		, CLAIMED(claimed), CHAR_PTM(char_ptm)
	//	{

	//	}

	//	void InsertInven()
	//	{
	//		IUID = DBTestHelper::InsertItem(GSN, CSN, GConfig::m_nMyWorldID, ITEM_ID, BEF_STACK_AMT, 1, SLOT_ID, false, 0, -1);
	//		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET DURA = %d, MAX_DURA = %d, COLOR = %d, SOUL_COUNT = %d, CLAIMED = %d, CHAR_PTM = %d WHERE IUID = %I64d"
	//			, DURA, MAX_DURA, COLOR, SOUL_COUNT, CLAIMED, CHAR_PTM, IUID);
	//	}

	//	void InsertEmptyInven()
	//	{
	//		DBTestHelper::InsertEmptyItemSlot(GSN, CSN, GConfig::m_nMyWorldID, 1, SLOT_ID);
	//	}

	//	void InsertCharStorage()
	//	{
	//		IUID = DBTestHelper::InsertCharStorageItem(GConfig::m_nMyWorldID, GSN, CSN, SLOT_ID, ITEM_ID, BEF_STACK_AMT, DURA, MAX_DURA
	//			, COLOR, SOUL_COUNT, CLAIMED, CHAR_PTM);
	//	}

	//	void InsertEmptyCharStorage()
	//	{
	//		DBTestHelper::InsertEmptyCharStorage(GConfig::m_nMyWorldID, GSN, CSN, SLOT_ID);
	//	}

	//	void CheckInven(int64 gsn, int64 csn, int slot_id, T_ITEM& item)
	//	{
	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"SELECT IUID, ITEM_ID, COLOR, STACK_AMT, DURA, MAX_DURA, SOUL_COUNT, CLAIMED, CHAR_PTM \
	//							 FROM dbo.RZ_INVEN \
	//							 WHERE WORLD_ID = %d AND ACCN_ID = %I64d AND CHAR_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d"
	//							 , GConfig::m_nMyWorldID, gsn, csn, slot_id);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		CHECK_EQUAL(item.IUID, rs.Field(L"IUID").AsInt64());
	//		CHECK_EQUAL(item.ITEM_ID, rs.Field(L"ITEM_ID").AsInt());
	//		CHECK_EQUAL(item.COLOR, rs.Field(L"COLOR").AsInt());
	//		CHECK_EQUAL(item.NEXT_STACK_AMT, rs.Field(L"STACK_AMT").AsShort());			
	//		CHECK_EQUAL(item.DURA, rs.Field(L"DURA").AsByte());
	//		CHECK_EQUAL(item.MAX_DURA, rs.Field(L"MAX_DURA").AsByte());
	//		CHECK_EQUAL(item.SOUL_COUNT, rs.Field(L"SOUL_COUNT").AsInt());
	//		CHECK_EQUAL(item.CLAIMED, rs.Field(L"CLAIMED").AsByte());
	//		CHECK_EQUAL(item.CHAR_PTM, rs.Field(L"CHAR_PTM").AsInt());
	//		rs.Close();
	//	}

	//	void CheckEmptyInven(int64 gsn, int64 csn, int slot_id)
	//	{
	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"SELECT IUID, ITEM_ID, COLOR, STACK_AMT, DURA, MAX_DURA, SOUL_COUNT, CLAIMED, CHAR_PTM \
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
	//		CHECK_EQUAL(0, rs.Field(L"CLAIMED").AsByte());
	//		CHECK_EQUAL(0, rs.Field(L"CHAR_PTM").AsInt());
	//		rs.Close();
	//	}

	//	void CheckCharStorage(int64 gsn, int64 csn, int slot_id, T_ITEM& item)
	//	{
	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"SELECT IUID, ITEM_ID, COLOR, STACK_AMT, DURA, MAX_DURA, SOUL_COUNT, CLAIMED, CHAR_PTM \
	//							 FROM dbo.RZ_CHAR_STORAGE \
	//							 WHERE WORLD = %d AND GSN = %I64d AND CHAR_ID = %I64d AND SLOT_ID = %d"
	//							 , GConfig::m_nMyWorldID, gsn, csn, slot_id);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		CHECK_EQUAL(item.IUID, rs.Field(L"IUID").AsInt64());
	//		CHECK_EQUAL(item.ITEM_ID, rs.Field(L"ITEM_ID").AsInt());
	//		CHECK_EQUAL(item.COLOR, rs.Field(L"COLOR").AsInt());
	//		CHECK_EQUAL(item.NEXT_STACK_AMT, rs.Field(L"STACK_AMT").AsShort());			
	//		CHECK_EQUAL(item.DURA, rs.Field(L"DURA").AsByte());
	//		CHECK_EQUAL(item.MAX_DURA, rs.Field(L"MAX_DURA").AsByte());
	//		CHECK_EQUAL(item.SOUL_COUNT, rs.Field(L"SOUL_COUNT").AsInt());
	//		CHECK_EQUAL(item.CLAIMED, rs.Field(L"CLAIMED").AsByte());
	//		CHECK_EQUAL(item.CHAR_PTM, rs.Field(L"CHAR_PTM").AsInt());
	//		rs.Close();
	//	}

	//	void CheckEmptyCharStorage(int64 gsn, int64 csn, int slot_id)
	//	{
	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"SELECT IUID, ITEM_ID, COLOR, STACK_AMT, DURA, MAX_DURA, SOUL_COUNT, CLAIMED, CHAR_PTM \
	//							 FROM dbo.RZ_CHAR_STORAGE \
	//							 WHERE WORLD = %d AND GSN = %I64d AND CHAR_ID = %I64d AND SLOT_ID = %d"
	//							 , GConfig::m_nMyWorldID, gsn, csn, slot_id);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		CHECK_EQUAL((int64)0, rs.Field(L"IUID").AsInt64());
	//		CHECK(rs.Field(L"ITEM_ID").IsNull());
	//		CHECK_EQUAL(0, rs.Field(L"COLOR").AsInt());
	//		CHECK_EQUAL(0, rs.Field(L"STACK_AMT").AsShort());			
	//		CHECK_EQUAL(0, rs.Field(L"DURA").AsByte());
	//		CHECK_EQUAL(0, rs.Field(L"MAX_DURA").AsByte());
	//		CHECK_EQUAL(0, rs.Field(L"SOUL_COUNT").AsInt());
	//		CHECK_EQUAL(0, rs.Field(L"CLAIMED").AsByte());
	//		CHECK_EQUAL(0, rs.Field(L"CHAR_PTM").AsInt());
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
	//	uint8	CLAIMED;
	//	int		CHAR_PTM;
	//};

	//SUITE (DBTEST_CHAR_STORAGE_SPLIT)
	//{
	//	GAMEDB_TEST(USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_STORAGE_1_1)
	//	{
	//		T_ITEM iv(GSN, CSN, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	//		T_ITEM cs(GSN, CSN, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);

	//		iv.InsertInven();
	//		cs.InsertCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_CHAR_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, iv.GSN, iv.CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, cs.SLOT_ID, cs.IUID, cs.BEF_STACK_AMT, cs.NEXT_STACK_AMT);

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, cs);
	//		cs.CheckCharStorage(GSN, CSN, cs.SLOT_ID, iv);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_STORAGE_1_2)
	//	{
	//		T_ITEM iv(GSN, CSN, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	//		T_ITEM cs(GSN, CSN, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	//		iv.InsertInven();
	//		cs.InsertEmptyCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_CHAR_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, iv.GSN, iv.CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, cs.SLOT_ID, cs.IUID, cs.BEF_STACK_AMT, cs.NEXT_STACK_AMT);

	//		iv.CheckEmptyInven(GSN, CSN, iv.SLOT_ID);
	//		cs.CheckCharStorage(GSN, CSN, cs.SLOT_ID, iv);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_STORAGE_1_3)
	//	{
	//		T_ITEM iv(GSN, CSN, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	//		T_ITEM cs(GSN, CSN, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	//		iv.InsertInven();
	//		
	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_CHAR_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, iv.GSN, iv.CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, cs.SLOT_ID, cs.IUID, cs.BEF_STACK_AMT, cs.NEXT_STACK_AMT);

	//		iv.CheckEmptyInven(GSN, CSN, iv.SLOT_ID);
	//		cs.CheckCharStorage(GSN, CSN, cs.SLOT_ID, iv);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_STORAGE_1_4)
	//	{
	//		T_ITEM iv(GSN, CSN, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	//		T_ITEM cs(GSN, CSN, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);

	//		iv.InsertEmptyInven();
	//		cs.InsertCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_CHAR_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, iv.GSN, iv.CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, cs.SLOT_ID, cs.IUID, cs.BEF_STACK_AMT, cs.NEXT_STACK_AMT);

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, cs);
	//		cs.CheckEmptyCharStorage(GSN, CSN, cs.SLOT_ID);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_STORAGE_1_5)
	//	{
	//		T_ITEM iv(GSN, CSN, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	//		T_ITEM cs(GSN, CSN, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);

	//		cs.InsertCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_CHAR_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, iv.GSN, iv.CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, cs.SLOT_ID, cs.IUID, cs.BEF_STACK_AMT, cs.NEXT_STACK_AMT);

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, cs);
	//		cs.CheckEmptyCharStorage(GSN, CSN, cs.SLOT_ID);
	//	}
	//}

	//SUITE (USP_RZ_ITEM_SPLIT_INVEN_TO_CHAR_STORAGE)
	//{
	//	GAMEDB_TEST(USP_RZ_ITEM_SPLIT_INVEN_TO_CHAR_STORAGE_1_1)
	//	{
	//		T_ITEM iv(GSN, CSN, 1, 2, 3, 1, 5, 0, 0, 0, 9, 10);
	//		T_ITEM cs(GSN, CSN, 11, 0, 0, 2, 0, 0, 0, 0, 0, 0);

	//		iv.InsertInven();
	//		cs.InsertEmptyCharStorage();

	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_ITEM_SPLIT_INVEN_TO_CHAR_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, cs.SLOT_ID, cs.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_CHAR_STORAGE_IUID").AsInt64();
	//		CHECK(0 < NEW_IUID);
	//		rs.Close();

	//		T_ITEM sp(GSN, CSN, cs.SLOT_ID, iv.ITEM_ID, 0, cs.NEXT_STACK_AMT, iv.COLOR, 0, 0, 0, iv.CLAIMED, iv.CHAR_PTM);
	//		sp.IUID = NEW_IUID;

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, iv);
	//		cs.CheckCharStorage(GSN, CSN, cs.SLOT_ID, sp);
	//	}		

	//	GAMEDB_TEST(USP_RZ_ITEM_SPLIT_INVEN_TO_CHAR_STORAGE_1_2)
	//	{
	//		T_ITEM iv(GSN, CSN, 1, 2, 3, 1, 5, 0, 0, 0, 9, 10);
	//		T_ITEM cs(GSN, CSN, 11, 0, 0, 2, 0, 0, 0, 0, 0, 0);

	//		iv.InsertInven();
	//		
	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_ITEM_SPLIT_INVEN_TO_CHAR_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, cs.SLOT_ID, cs.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_CHAR_STORAGE_IUID").AsInt64();
	//		CHECK(0 < NEW_IUID);
	//		rs.Close();

	//		T_ITEM sp(GSN, CSN, cs.SLOT_ID, iv.ITEM_ID, 0, cs.NEXT_STACK_AMT, iv.COLOR, 0, 0, 0, iv.CLAIMED, iv.CHAR_PTM);
	//		sp.IUID = NEW_IUID;

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, iv);
	//		cs.CheckCharStorage(GSN, CSN, cs.SLOT_ID, sp);
	//	}	

	//	GAMEDB_TEST(USP_RZ_ITEM_SPLIT_INVEN_TO_CHAR_STORAGE_2_1)
	//	{
	//		T_ITEM iv(GSN, CSN, 1, 2, 0, 3, 0, 0, 0, 0, 0, 0);
	//		T_ITEM cs(GSN, CSN, 11, 12, 13, 10, 15, 0, 0, 0, 19, 20);

	//		iv.InsertEmptyInven();
	//		cs.InsertCharStorage();

	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_ITEM_SPLIT_CHAR_STORAGE_TO_INVEN (%d, %I64d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.NEXT_STACK_AMT
	//			, cs.SLOT_ID, cs.IUID, cs.BEF_STACK_AMT, cs.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_INVEN_IUID").AsInt64();
	//		CHECK(0 < NEW_IUID);
	//		rs.Close();

	//		T_ITEM sp(GSN, CSN, iv.SLOT_ID, cs.ITEM_ID, 0, iv.NEXT_STACK_AMT, cs.COLOR, 0, 0, 0, cs.CLAIMED, cs.CHAR_PTM);
	//		sp.IUID = NEW_IUID;

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, sp);
	//		cs.CheckCharStorage(GSN, CSN, cs.SLOT_ID, cs);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_SPLIT_INVEN_TO_CHAR_STORAGE_2_2)
	//	{
	//		T_ITEM iv(GSN, CSN, 1, 2, 0, 3, 0, 0, 0, 0, 0, 0);
	//		T_ITEM cs(GSN, CSN, 11, 12, 13, 10, 15, 0, 0, 0, 19, 20);

	//		cs.InsertCharStorage();

	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_ITEM_SPLIT_CHAR_STORAGE_TO_INVEN (%d, %I64d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.NEXT_STACK_AMT
	//			, cs.SLOT_ID, cs.IUID, cs.BEF_STACK_AMT, cs.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_INVEN_IUID").AsInt64();
	//		CHECK(0 < NEW_IUID);
	//		rs.Close();

	//		T_ITEM sp(GSN, CSN, iv.SLOT_ID, cs.ITEM_ID, 0, iv.NEXT_STACK_AMT, cs.COLOR, 0, 0, 0, cs.CLAIMED, cs.CHAR_PTM);
	//		sp.IUID = NEW_IUID;

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, sp);
	//		cs.CheckCharStorage(GSN, CSN, cs.SLOT_ID, cs);
	//	}
	//}

	//SUITE (USP_RZ_ITEM_MERGE_INVEN_AND_CHAR_STORAGE)
	//{
	//	GAMEDB_TEST(USP_RZ_ITEM_MERGE_INVEN_AND_CHAR_STORAGE_1_1)
	//	{
	//		T_ITEM iv(GSN, CSN, 1, 2, 3, 1, 5, 0, 0, 0, 9, 10);
	//		T_ITEM cs(GSN, CSN, 11, 12, 13, 15, 15, 0, 0, 0, 19, 20);

	//		iv.InsertInven();
	//		cs.InsertCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_MERGE_INVEN_AND_CHAR_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, cs.SLOT_ID, cs.IUID, cs.BEF_STACK_AMT, cs.NEXT_STACK_AMT);

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, iv);
	//		cs.CheckCharStorage(GSN, CSN, cs.SLOT_ID, cs);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_MERGE_INVEN_AND_CHAR_STORAGE_1_2)
	//	{
	//		T_ITEM iv(GSN, CSN, 1, 2, 3, 0, 5, 0, 0, 0, 9, 10);
	//		T_ITEM cs(GSN, CSN, 11, 12, 13, 16, 15, 0, 0, 0, 19, 20);

	//		iv.InsertInven();
	//		cs.InsertCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_MERGE_INVEN_AND_CHAR_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, cs.SLOT_ID, cs.IUID, cs.BEF_STACK_AMT, cs.NEXT_STACK_AMT);

	//		iv.CheckEmptyInven(GSN, CSN, iv.SLOT_ID);
	//		cs.CheckCharStorage(GSN, CSN, cs.SLOT_ID, cs);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_MERGE_INVEN_AND_CHAR_STORAGE_1_3)
	//	{
	//		T_ITEM iv(GSN, CSN, 1, 2, 3, 16, 5, 0, 0, 0, 9, 10);
	//		T_ITEM cs(GSN, CSN, 11, 12, 13, 0, 15, 0, 0, 0, 19, 20);

	//		iv.InsertInven();
	//		cs.InsertCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_MERGE_INVEN_AND_CHAR_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, cs.SLOT_ID, cs.IUID, cs.BEF_STACK_AMT, cs.NEXT_STACK_AMT);

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, iv);
	//		cs.CheckEmptyCharStorage(GSN, CSN, cs.SLOT_ID);
	//	}
	//}

	//SUITE (USP_RZ_CHAR_STORAGE_SWAP_SLOT)
	//{
	//	GAMEDB_TEST (USP_RZ_CHAR_STORAGE_SWAP_SLOT_1)
	//	{
	//		T_ITEM ivf(GSN, CSN, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	//		T_ITEM ivt(GSN, CSN, 11, 12, 13, 15, 15, 16, 17, 18, 19, 20);

	//		ivf.InsertCharStorage();
	//		ivt.InsertCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_STORAGE_SWAP_SLOT (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckCharStorage(GSN, CSN, ivf.SLOT_ID, ivt);
	//		ivt.CheckCharStorage(GSN, CSN, ivt.SLOT_ID, ivf);
	//	}		

	//	GAMEDB_TEST (USP_RZ_CHAR_STORAGE_SWAP_SLOT_2_1)
	//	{
	//		T_ITEM ivf(GSN, CSN, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	//		T_ITEM ivt(GSN, CSN, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	//		ivf.InsertCharStorage();
	//		ivt.InsertEmptyCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_STORAGE_SWAP_SLOT (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckEmptyCharStorage(GSN, CSN, ivf.SLOT_ID);
	//		ivt.CheckCharStorage(GSN, CSN, ivt.SLOT_ID, ivf);
	//	}		

	//	GAMEDB_TEST (USP_RZ_CHAR_STORAGE_SWAP_SLOT_2_2)
	//	{
	//		T_ITEM ivf(GSN, CSN, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	//		T_ITEM ivt(GSN, CSN, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	//		ivf.InsertCharStorage();
	//		
	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_STORAGE_SWAP_SLOT (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivt.CheckCharStorage(GSN, CSN, ivt.SLOT_ID, ivf);
	//	}	

	//	GAMEDB_TEST (USP_RZ_CHAR_STORAGE_SWAP_SLOT_3_1)
	//	{
	//		T_ITEM ivf(GSN, CSN, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	//		T_ITEM ivt(GSN, CSN, 11, 12, 13, 15, 15, 16, 17, 18, 19, 20);

	//		ivf.InsertEmptyCharStorage();
	//		ivt.InsertCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_STORAGE_SWAP_SLOT (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckCharStorage(GSN, CSN, ivf.SLOT_ID, ivt);
	//		ivt.CheckEmptyCharStorage(GSN, CSN, ivt.SLOT_ID);
	//	}

	//	GAMEDB_TEST (USP_RZ_CHAR_STORAGE_SWAP_SLOT_3_2)
	//	{
	//		T_ITEM ivf(GSN, CSN, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	//		T_ITEM ivt(GSN, CSN, 11, 12, 13, 15, 15, 16, 17, 18, 19, 20);

	//		ivt.InsertCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_STORAGE_SWAP_SLOT (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckCharStorage(GSN, CSN, ivf.SLOT_ID, ivt);			
	//	}
	//}

	//SUITE (USP_RZ_CHAR_STORAGE_MERGE)
	//{
	//	GAMEDB_TEST (USP_RZ_CHAR_STORAGE_MERGE_1_1)
	//	{
	//		T_ITEM ivf(GSN, CSN, 1, 2, 3, 1, 5, 0, 0, 0, 9, 10);
	//		T_ITEM ivt(GSN, CSN, 11, 12, 13, 15, 15, 0, 0, 0, 19, 20);

	//		ivf.InsertCharStorage();
	//		ivt.InsertCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_STORAGE_MERGE (%d, %I64d,%I64d, %d, %I64d, %d,%d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckCharStorage(GSN, CSN, ivf.SLOT_ID, ivf);
	//		ivt.CheckCharStorage(GSN, CSN, ivt.SLOT_ID, ivt);
	//	}	

	//	GAMEDB_TEST (USP_RZ_CHAR_STORAGE_MERGE_1_2)
	//	{
	//		T_ITEM ivf(GSN, CSN, 1, 2, 3, 15, 5, 0, 0, 0, 9, 10);
	//		T_ITEM ivt(GSN, CSN, 11, 12, 13, 1, 15, 0, 0, 0, 19, 20);

	//		ivf.InsertCharStorage();
	//		ivt.InsertCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_STORAGE_MERGE (%d, %I64d,%I64d, %d, %I64d, %d,%d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckCharStorage(GSN, CSN, ivf.SLOT_ID, ivf);
	//		ivt.CheckCharStorage(GSN, CSN, ivt.SLOT_ID, ivt);
	//	}	

	//	GAMEDB_TEST (USP_RZ_CHAR_STORAGE_MERGE_2_1)
	//	{
	//		T_ITEM ivf(GSN, CSN, 1, 2, 3, 0, 5, 0, 0, 0, 9, 10);
	//		T_ITEM ivt(GSN, CSN, 11, 12, 13, 16, 15, 0, 0, 0, 19, 20);

	//		ivf.InsertCharStorage();
	//		ivt.InsertCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_STORAGE_MERGE (%d, %I64d,%I64d, %d, %I64d, %d,%d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckEmptyCharStorage(GSN, CSN, ivf.SLOT_ID);
	//		ivt.CheckCharStorage(GSN, CSN, ivt.SLOT_ID, ivt);
	//	}	

	//	GAMEDB_TEST (USP_RZ_CHAR_STORAGE_MERGE_2_2)
	//	{
	//		T_ITEM ivf(GSN, CSN, 1, 2, 3, 16, 5, 0, 0, 0, 9, 10);
	//		T_ITEM ivt(GSN, CSN, 11, 12, 13, 0, 15, 0, 0, 0, 19, 20);

	//		ivf.InsertCharStorage();
	//		ivt.InsertCharStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_STORAGE_MERGE (%d, %I64d,%I64d, %d, %I64d, %d,%d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.IUID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT);

	//		ivf.CheckCharStorage(GSN, CSN, ivf.SLOT_ID, ivf);
	//		ivt.CheckEmptyCharStorage(GSN, CSN, ivt.SLOT_ID);
	//	}	
	//}

	//SUITE (USP_RZ_CHAR_STORAGE_SPLIT)
	//{
	//	GAMEDB_TEST (USP_RZ_CHAR_STORAGE_SPLIT_1)
	//	{
	//		T_ITEM ivf(GSN, CSN, 1, 2, 3, 1, 5, 0, 0, 0, 9, 10);
	//		T_ITEM ivt(GSN, CSN, 11, 0, 0, 2, 15, 0, 0, 0, 0, 0);

	//		ivf.InsertCharStorage();
	//		ivt.InsertEmptyCharStorage();

	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_CHAR_STORAGE_SPLIT (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_CHAR_STORAGE_IUID").AsInt64();
	//		rs.Close();

	//		T_ITEM sp(GSN, CSN, ivt.SLOT_ID, ivf.ITEM_ID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT, ivf.COLOR, 0, 0, 0, ivf.CLAIMED, ivf.CHAR_PTM);
	//		sp.IUID = NEW_IUID;

	//		ivf.CheckCharStorage(GSN, CSN, ivf.SLOT_ID, ivf);
	//		ivt.CheckCharStorage(GSN, CSN, ivt.SLOT_ID, sp);
	//	}

	//	GAMEDB_TEST (USP_RZ_CHAR_STORAGE_SPLIT_2)
	//	{
	//		T_ITEM ivf(GSN, CSN, 1, 2, 3, 1, 5, 0, 0, 0, 9, 10);
	//		T_ITEM ivt(GSN, CSN, 11, 0, 0, 2, 15, 0, 0, 0, 0, 0);

	//		ivf.InsertCharStorage();

	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_CHAR_STORAGE_SPLIT (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GSN, CSN
	//			, ivf.SLOT_ID, ivf.IUID, ivf.BEF_STACK_AMT, ivf.NEXT_STACK_AMT
	//			, ivt.SLOT_ID, ivt.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_CHAR_STORAGE_IUID").AsInt64();
	//		rs.Close();

	//		T_ITEM sp(GSN, CSN, ivt.SLOT_ID, ivf.ITEM_ID, ivt.BEF_STACK_AMT, ivt.NEXT_STACK_AMT, ivf.COLOR, 0, 0, 0, ivf.CLAIMED, ivf.CHAR_PTM);
	//		sp.IUID = NEW_IUID;

	//		ivf.CheckCharStorage(GSN, CSN, ivf.SLOT_ID, ivf);
	//		ivt.CheckCharStorage(GSN, CSN, ivt.SLOT_ID, sp);
	//	}
	//}
}
