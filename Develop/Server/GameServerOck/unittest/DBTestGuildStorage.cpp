#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"

namespace dbtest_guild_storage
{
	//class T_ITEM
	//{
	//public :
	//	T_ITEM(int64 gsn, int64 csn, int64 guild_id, int slot_id, int item_id, int16 bef_stack_amt, int16 next_stack_amt, int color, uint8 dura, uint8 max_dura, uint8 soul_count)
	//		: GSN(gsn), CSN(csn), GUILD_ID(guild_id), SLOT_ID(slot_id), ITEM_ID(item_id), COLOR(color), DURA(dura), MAX_DURA(max_dura)
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

	//	void InsertGuildStorage()
	//	{
	//		IUID = DBTestHelper::InsertGuildStorageItem(GConfig::m_nMyWorldID, GUILD_ID, SLOT_ID, ITEM_ID, BEF_STACK_AMT, DURA, MAX_DURA, COLOR, SOUL_COUNT);
	//	}

	//	void InsertEmptyGuildStorage()
	//	{
	//		DBTestHelper::InsertEmptyGuildStorage(GConfig::m_nMyWorldID, GUILD_ID, SLOT_ID);
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

	//	void CheckGuildStorage(int64 guild_id, int slot_id, T_ITEM& item)
	//	{
	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"SELECT IUID, ITEM_ID, COLOR, STACK_AMT, DURA, MAX_DURA, SOUL_COUNT \
	//							 FROM dbo.RZ_GUILD_STORAGE \
	//							 WHERE WORLD = %d AND GUILD_ID = %I64d AND SLOT_ID = %d"
	//							 , GConfig::m_nMyWorldID, guild_id, slot_id);
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

	//	void CheckEmptyGuildStorage(int64 guild_id, int slot_id)
	//	{
	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"SELECT IUID, ITEM_ID, COLOR, STACK_AMT, DURA, MAX_DURA, SOUL_COUNT \
	//							 FROM dbo.RZ_GUILD_STORAGE \
	//							 WHERE WORLD = %d AND GUILD_ID = %I64d AND SLOT_ID = %d"
	//							 , GConfig::m_nMyWorldID, guild_id, slot_id);
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
	//	int64	GUILD_ID;
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

	//SUITE(DBTEST_GUILD_STORAGE_AND_INVEN_SWAP)
	//{
	//	const int64 GSN			= 1;
	//	const int64 CSN			= 2;
	//	const int64 GUILD_ID	= 3;
	//	const int	CHAR_PTM	= 4;

	//	GAMEDB_TEST(USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_GUILD_STORAGE_1)
	//	{
	//		T_ITEM iv(GSN, CSN, GUILD_ID, 1, 2, 3, 4, 5, 6, 7, 8);
	//		T_ITEM gs(GSN, CSN, GUILD_ID, 11, 12, 13, 14, 15, 16, 17, 18);

	//		iv.InsertInven();
	//		gs.InsertGuildStorage();

	//		UTestDB.Execute(L"{CALL USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_GUILD_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT, CHAR_PTM
	//			, GUILD_ID, gs.SLOT_ID, gs.IUID, gs.BEF_STACK_AMT, gs.NEXT_STACK_AMT);

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, gs);
	//		gs.CheckGuildStorage(GUILD_ID, gs.SLOT_ID, iv);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_GUILD_STORAGE_2_1)
	//	{
	//		T_ITEM iv(GSN, CSN, GUILD_ID, 1, 2, 3, 4, 5, 6, 7, 8);
	//		T_ITEM gs(GSN, CSN, GUILD_ID, 11, 0, 0, 0, 0, 0, 0, 0);

	//		iv.InsertInven();
	//		gs.InsertEmptyGuildStorage();

	//		UTestDB.Execute(L"{CALL USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_GUILD_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT, CHAR_PTM
	//			, GUILD_ID, gs.SLOT_ID, gs.IUID, gs.BEF_STACK_AMT, gs.NEXT_STACK_AMT);

	//		iv.CheckEmptyInven(GSN, CSN, iv.SLOT_ID);
	//		gs.CheckGuildStorage(GUILD_ID, gs.SLOT_ID, iv);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_GUILD_STORAGE_2_2)
	//	{
	//		T_ITEM iv(GSN, CSN, GUILD_ID, 1, 2, 3, 4, 5, 6, 7, 8);
	//		T_ITEM gs(GSN, CSN, GUILD_ID, 11, 0, 0, 0, 0, 0, 0, 0);

	//		iv.InsertInven();

	//		UTestDB.Execute(L"{CALL USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_GUILD_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT, CHAR_PTM
	//			, GUILD_ID, gs.SLOT_ID, gs.IUID, gs.BEF_STACK_AMT, gs.NEXT_STACK_AMT);

	//		iv.CheckEmptyInven(GSN, CSN, iv.SLOT_ID);
	//		gs.CheckGuildStorage(GUILD_ID, gs.SLOT_ID, iv);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_GUILD_STORAGE_3_1)
	//	{
	//		T_ITEM iv(GSN, CSN, GUILD_ID, 1, 0, 0, 0, 0, 0, 0, 0);
	//		T_ITEM gs(GSN, CSN, GUILD_ID, 11, 12, 13, 14, 15, 16, 17, 18);

	//		iv.InsertEmptyInven();
	//		gs.InsertGuildStorage();

	//		UTestDB.Execute(L"{CALL USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_GUILD_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT, CHAR_PTM
	//			, GUILD_ID, gs.SLOT_ID, gs.IUID, gs.BEF_STACK_AMT, gs.NEXT_STACK_AMT);

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, gs);
	//		gs.CheckEmptyGuildStorage(GUILD_ID, gs.SLOT_ID);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_GUILD_STORAGE_3_2)
	//	{
	//		T_ITEM iv(GSN, CSN, GUILD_ID, 1, 0, 0, 0, 0, 0, 0, 0);
	//		T_ITEM gs(GSN, CSN, GUILD_ID, 11, 12, 13, 14, 15, 16, 17, 18);

	//		iv.InsertEmptyInven();
	//		gs.InsertGuildStorage();

	//		UTestDB.Execute(L"{CALL USP_RZ_ITEM_SWAP_SLOT_INVEN_AND_GUILD_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT, CHAR_PTM
	//			, GUILD_ID, gs.SLOT_ID, gs.IUID, gs.BEF_STACK_AMT, gs.NEXT_STACK_AMT);

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, gs);
	//		gs.CheckEmptyGuildStorage(GUILD_ID, gs.SLOT_ID);
	//	}
	//}

	//SUITE (DBTEST_GUILD_STORAGE_AND_INVEN_SPLIT)
	//{
	//	const int64 GSN			= 1;
	//	const int64 CSN			= 2;
	//	const int64 GUILD_ID	= 3;
	//	const int	CHAR_PTM	= 4;

	//	GAMEDB_TEST(USP_RZ_ITEM_SPLIT_INVEN_TO_GUILD_STORAGE_1_1)
	//	{
	//		T_ITEM iv(GSN, CSN, GUILD_ID, 1, 2, 3, 1, 5, 0, 0, 0);
	//		T_ITEM gs(GSN, CSN, GUILD_ID, 11, 0, 0, 2, 0, 0, 0, 0);

	//		iv.InsertInven();
	//		gs.InsertEmptyGuildStorage();

	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_ITEM_SPLIT_INVEN_TO_GUILD_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, GUILD_ID, gs.SLOT_ID, gs.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_GUILD_STORAGE_IUID").AsInt64();
	//		CHECK(0 < NEW_IUID);
	//		rs.Close();

	//		T_ITEM sp(GSN, CSN, GUILD_ID, gs.SLOT_ID, iv.ITEM_ID, gs.BEF_STACK_AMT, gs.NEXT_STACK_AMT, iv.COLOR, 0, 0, 0);
	//		sp.IUID = NEW_IUID;

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, iv);
	//		gs.CheckGuildStorage(GUILD_ID, gs.SLOT_ID, sp);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_SPLIT_INVEN_TO_GUILD_STORAGE_1_2)
	//	{
	//		T_ITEM iv(GSN, CSN, GUILD_ID, 1, 2, 3, 1, 5, 0, 0, 0);
	//		T_ITEM gs(GSN, CSN, GUILD_ID, 11, 0, 0, 2, 0, 0, 0, 0);

	//		iv.InsertInven();

	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_ITEM_SPLIT_INVEN_TO_GUILD_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, GUILD_ID, gs.SLOT_ID, gs.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_GUILD_STORAGE_IUID").AsInt64();
	//		CHECK(0 < NEW_IUID);
	//		rs.Close();

	//		T_ITEM sp(GSN, CSN, GUILD_ID, gs.SLOT_ID, iv.ITEM_ID, gs.BEF_STACK_AMT, gs.NEXT_STACK_AMT, iv.COLOR, 0, 0, 0);
	//		sp.IUID = NEW_IUID;

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, iv);
	//		gs.CheckGuildStorage(GUILD_ID, gs.SLOT_ID, sp);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_SPLITGUILD_STORAGE_TO_INVEN_2_1)
	//	{
	//		T_ITEM iv(GSN, CSN, GUILD_ID, 1, 0, 0, 2, 0, 0, 0, 0);
	//		T_ITEM gs(GSN, CSN, GUILD_ID, 11, 12, 13, 11, 5, 0, 0, 0);

	//		iv.InsertEmptyInven();
	//		gs.InsertGuildStorage();

	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_ITEM_SPLIT_GUILD_STORAGE_TO_INVEN (%d, %I64d, %I64d, %d, %d, %d, %I64d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.NEXT_STACK_AMT, CHAR_PTM
	//			, GUILD_ID, gs.SLOT_ID, gs.IUID, gs.BEF_STACK_AMT, gs.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_INVEN_IUID").AsInt64();
	//		CHECK(0 < NEW_IUID);
	//		rs.Close();

	//		T_ITEM sp(GSN, CSN, GUILD_ID, iv.SLOT_ID, gs.ITEM_ID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT, gs.COLOR, 0, 0, 0);
	//		sp.IUID = NEW_IUID;

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, sp);
	//		gs.CheckGuildStorage(GUILD_ID, gs.SLOT_ID, gs);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_SPLITGUILD_STORAGE_TO_INVEN_2_2)
	//	{
	//		T_ITEM iv(GSN, CSN, GUILD_ID, 1, 0, 0, 2, 0, 0, 0, 0);
	//		T_ITEM gs(GSN, CSN, GUILD_ID, 11, 12, 13, 11, 5, 0, 0, 0);

	//		gs.InsertGuildStorage();

	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_ITEM_SPLIT_GUILD_STORAGE_TO_INVEN (%d, %I64d, %I64d, %d, %d, %d, %I64d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.NEXT_STACK_AMT, CHAR_PTM
	//			, GUILD_ID, gs.SLOT_ID, gs.IUID, gs.BEF_STACK_AMT, gs.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_INVEN_IUID").AsInt64();
	//		CHECK(0 < NEW_IUID);
	//		rs.Close();

	//		T_ITEM sp(GSN, CSN, GUILD_ID, iv.SLOT_ID, gs.ITEM_ID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT, gs.COLOR, 0, 0, 0);
	//		sp.IUID = NEW_IUID;

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, sp);
	//		gs.CheckGuildStorage(GUILD_ID, gs.SLOT_ID, gs);
	//	}
	//}

	//SUITE (DBTEST_GUILD_STORAGE_AND_INVEN_MERGE)
	//{
	//	const int64 GSN			= 1;
	//	const int64 CSN			= 2;
	//	const int64 GUILD_ID	= 3;	

	//	GAMEDB_TEST(USP_RZ_ITEM_MERGE_INVEN_AND_GUILD_STORAGE_1_1)
	//	{
	//		T_ITEM iv(GSN, CSN, GUILD_ID, 1, 2, 3, 1, 5, 0, 0, 0);
	//		T_ITEM gs(GSN, CSN, GUILD_ID, 11, 12, 13, 15, 15, 0, 0, 0);

	//		iv.InsertInven();
	//		gs.InsertGuildStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_MERGE_INVEN_AND_GUILD_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %I64d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, GUILD_ID, gs.SLOT_ID, gs.IUID, gs.BEF_STACK_AMT, gs.NEXT_STACK_AMT);

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, iv);
	//		gs.CheckGuildStorage(GUILD_ID, gs.SLOT_ID, gs);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_MERGE_INVEN_AND_GUILD_STORAGE_1_2)
	//	{
	//		T_ITEM iv(GSN, CSN, GUILD_ID, 1, 2, 3, 0, 5, 0, 0, 0);
	//		T_ITEM gs(GSN, CSN, GUILD_ID, 11, 12, 13, 16, 15, 0, 0, 0);

	//		iv.InsertInven();
	//		gs.InsertGuildStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_MERGE_INVEN_AND_GUILD_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %I64d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, GUILD_ID, gs.SLOT_ID, gs.IUID, gs.BEF_STACK_AMT, gs.NEXT_STACK_AMT);

	//		iv.CheckEmptyInven(GSN, CSN, iv.SLOT_ID);
	//		gs.CheckGuildStorage(GUILD_ID, gs.SLOT_ID, gs);
	//	}

	//	GAMEDB_TEST(USP_RZ_ITEM_MERGE_INVEN_AND_GUILD_STORAGE_1_3)
	//	{
	//		T_ITEM iv(GSN, CSN, GUILD_ID, 1, 2, 3, 16, 5, 0, 0, 0);
	//		T_ITEM gs(GSN, CSN, GUILD_ID, 11, 12, 13, 0, 15, 0, 0, 0);

	//		iv.InsertInven();
	//		gs.InsertGuildStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_MERGE_INVEN_AND_GUILD_STORAGE (%d, %I64d, %I64d, %d, %I64d, %d, %d, %I64d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID
	//			, GSN, CSN, iv.SLOT_ID, iv.IUID, iv.BEF_STACK_AMT, iv.NEXT_STACK_AMT
	//			, GUILD_ID, gs.SLOT_ID, gs.IUID, gs.BEF_STACK_AMT, gs.NEXT_STACK_AMT);

	//		iv.CheckInven(GSN, CSN, iv.SLOT_ID, iv);
	//		gs.CheckEmptyGuildStorage(GUILD_ID, gs.SLOT_ID);
	//	}
	//}

	//SUITE (DBTEST_GUILD_STORAGE_SWAP_SLOT)
	//{
	//	const int64 GSN			= 1;
	//	const int64 CSN			= 2;
	//	const int64 GUILD_ID	= 3;

	//	GAMEDB_TEST(USP_RZ_GUILD_STORAGE_SWAP_SLOT_1)
	//	{
	//		T_ITEM gsf (GSN, CSN, GUILD_ID, 1, 2, 3, 4, 5, 6, 7, 8);
	//		T_ITEM gst (GSN, CSN, GUILD_ID, 11, 12, 13, 14, 15, 16, 17, 18);

	//		gsf.InsertGuildStorage();
	//		gst.InsertGuildStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_STORAGE_SWAP_SLOT (%d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GUILD_ID
	//			, gsf.SLOT_ID, gsf.IUID, gsf.BEF_STACK_AMT, gsf.NEXT_STACK_AMT
	//			, gst.SLOT_ID, gst.IUID, gst.BEF_STACK_AMT, gst.NEXT_STACK_AMT);

	//		gsf.CheckGuildStorage(GUILD_ID, gsf.SLOT_ID, gst);
	//		gst.CheckGuildStorage(GUILD_ID, gst.SLOT_ID, gsf);
	//	}

	//	GAMEDB_TEST(USP_RZ_GUILD_STORAGE_SWAP_SLOT_2_1)
	//	{
	//		T_ITEM gsf (GSN, CSN, GUILD_ID, 1, 2, 3, 4, 5, 6, 7, 8);
	//		T_ITEM gst (GSN, CSN, GUILD_ID, 11, 0, 0, 0, 0, 0, 0, 0);

	//		gsf.InsertGuildStorage();
	//		gst.InsertEmptyGuildStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_STORAGE_SWAP_SLOT (%d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GUILD_ID
	//			, gsf.SLOT_ID, gsf.IUID, gsf.BEF_STACK_AMT, gsf.NEXT_STACK_AMT
	//			, gst.SLOT_ID, gst.IUID, gst.BEF_STACK_AMT, gst.NEXT_STACK_AMT);

	//		gsf.CheckEmptyGuildStorage(GUILD_ID, gsf.SLOT_ID);
	//		gst.CheckGuildStorage(GUILD_ID, gst.SLOT_ID, gsf);
	//	}

	//	GAMEDB_TEST(USP_RZ_GUILD_STORAGE_SWAP_SLOT_2_2)
	//	{
	//		T_ITEM gsf (GSN, CSN, GUILD_ID, 1, 2, 3, 4, 5, 6, 7, 8);
	//		T_ITEM gst (GSN, CSN, GUILD_ID, 11, 0, 0, 0, 0, 0, 0, 0);

	//		gsf.InsertGuildStorage();
	//	
	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_STORAGE_SWAP_SLOT (%d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GUILD_ID
	//			, gsf.SLOT_ID, gsf.IUID, gsf.BEF_STACK_AMT, gsf.NEXT_STACK_AMT
	//			, gst.SLOT_ID, gst.IUID, gst.BEF_STACK_AMT, gst.NEXT_STACK_AMT);

	//		gst.CheckGuildStorage(GUILD_ID, gst.SLOT_ID, gsf);
	//	}

	//	GAMEDB_TEST(USP_RZ_GUILD_STORAGE_SWAP_SLOT_3_1)
	//	{
	//		T_ITEM gsf (GSN, CSN, GUILD_ID, 1, 0, 0, 0, 0, 0, 0, 0);
	//		T_ITEM gst (GSN, CSN, GUILD_ID, 11, 12, 13, 14, 15, 16, 17, 18);

	//		gsf.InsertEmptyGuildStorage();
	//		gst.InsertGuildStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_STORAGE_SWAP_SLOT (%d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GUILD_ID
	//			, gsf.SLOT_ID, gsf.IUID, gsf.BEF_STACK_AMT, gsf.NEXT_STACK_AMT
	//			, gst.SLOT_ID, gst.IUID, gst.BEF_STACK_AMT, gst.NEXT_STACK_AMT);

	//		gsf.CheckGuildStorage(GUILD_ID, gsf.SLOT_ID, gst);
	//		gst.CheckEmptyGuildStorage(GUILD_ID, gst.SLOT_ID);
	//	}

	//	GAMEDB_TEST(USP_RZ_GUILD_STORAGE_SWAP_SLOT_3_2)
	//	{
	//		T_ITEM gsf (GSN, CSN, GUILD_ID, 1, 0, 0, 0, 0, 0, 0, 0);
	//		T_ITEM gst (GSN, CSN, GUILD_ID, 11, 12, 13, 14, 15, 16, 17, 18);

	//		gst.InsertGuildStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_STORAGE_SWAP_SLOT (%d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GUILD_ID
	//			, gsf.SLOT_ID, gsf.IUID, gsf.BEF_STACK_AMT, gsf.NEXT_STACK_AMT
	//			, gst.SLOT_ID, gst.IUID, gst.BEF_STACK_AMT, gst.NEXT_STACK_AMT);

	//		gsf.CheckGuildStorage(GUILD_ID, gsf.SLOT_ID, gst);		
	//	}
	//}

	//SUITE (USP_RZ_GUILD_STORAGE_MERGE)
	//{
	//	const int64 GSN			= 1;
	//	const int64 CSN			= 2;
	//	const int64 GUILD_ID	= 3;

	//	GAMEDB_TEST (USP_RZ_GUILD_STORAGE_MERGE_1)
	//	{
	//		T_ITEM gsf (GSN, CSN, GUILD_ID, 1, 2, 3, 1, 5, 0, 0, 0);
	//		T_ITEM gst (GSN, CSN, GUILD_ID, 11, 12, 13, 15, 15, 0, 0, 0);

	//		gsf.InsertGuildStorage();
	//		gst.InsertGuildStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_STORAGE_MERGE (%d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GUILD_ID
	//			, gsf.SLOT_ID, gsf.IUID, gsf.BEF_STACK_AMT, gsf.NEXT_STACK_AMT
	//			, gst.SLOT_ID, gst.IUID, gst.BEF_STACK_AMT, gst.NEXT_STACK_AMT);

	//		gsf.CheckGuildStorage(GUILD_ID, gsf.SLOT_ID, gsf);
	//		gst.CheckGuildStorage(GUILD_ID, gst.SLOT_ID, gst);
	//	}

	//	GAMEDB_TEST (USP_RZ_GUILD_STORAGE_MERGE_2)
	//	{
	//		T_ITEM gsf (GSN, CSN, GUILD_ID, 1, 2, 3, 0, 5, 0, 0, 0);
	//		T_ITEM gst (GSN, CSN, GUILD_ID, 11, 12, 13, 15, 16, 0, 0, 0);

	//		gsf.InsertGuildStorage();
	//		gst.InsertGuildStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_STORAGE_MERGE (%d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GUILD_ID
	//			, gsf.SLOT_ID, gsf.IUID, gsf.BEF_STACK_AMT, gsf.NEXT_STACK_AMT
	//			, gst.SLOT_ID, gst.IUID, gst.BEF_STACK_AMT, gst.NEXT_STACK_AMT);

	//		gsf.CheckEmptyGuildStorage(GUILD_ID, gsf.SLOT_ID);
	//		gst.CheckGuildStorage(GUILD_ID, gst.SLOT_ID, gst);
	//	}

	//	GAMEDB_TEST (USP_RZ_GUILD_STORAGE_MERGE_3)
	//	{
	//		T_ITEM gsf (GSN, CSN, GUILD_ID, 1, 2, 3, 16, 5, 0, 0, 0);
	//		T_ITEM gst (GSN, CSN, GUILD_ID, 11, 12, 13, 0, 15, 0, 0, 0);

	//		gsf.InsertGuildStorage();
	//		gst.InsertGuildStorage();

	//		UTestDB.Execute(L"{CALL dbo.USP_RZ_GUILD_STORAGE_MERGE (%d, %I64d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GUILD_ID
	//			, gsf.SLOT_ID, gsf.IUID, gsf.BEF_STACK_AMT, gsf.NEXT_STACK_AMT
	//			, gst.SLOT_ID, gst.IUID, gst.BEF_STACK_AMT, gst.NEXT_STACK_AMT);

	//		gsf.CheckGuildStorage(GUILD_ID, gsf.SLOT_ID, gsf);
	//		gst.CheckEmptyGuildStorage(GUILD_ID, gst.SLOT_ID);
	//	}
	//}

	//SUITE (USP_RZ_GUILD_STORAGE_SPLIT)
	//{
	//	const int64 GSN			= 1;
	//	const int64 CSN			= 2;
	//	const int64 GUILD_ID	= 3;

	//	GAMEDB_TEST (USP_RZ_GUILD_STORAGE_SPLIT_1)
	//	{
	//		T_ITEM gsf (GSN, CSN, GUILD_ID, 1, 2, 3, 1, 5, 0, 0, 0);
	//		T_ITEM gst (GSN, CSN, GUILD_ID, 11, 0, 0, 2, 0, 0, 0, 0);

	//		gsf.InsertGuildStorage();
	//		gst.InsertEmptyGuildStorage();

	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_GUILD_STORAGE_SPLIT (%d, %I64d, %d, %I64d, %d, %d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GUILD_ID
	//			, gsf.SLOT_ID, gsf.IUID, gsf.BEF_STACK_AMT, gsf.NEXT_STACK_AMT
	//			, gst.SLOT_ID, gst.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_GUILD_STORAGE_IUID").AsInt64();
	//		rs.Close();

	//		T_ITEM sp (GSN, CSN, GUILD_ID, gst.SLOT_ID, gsf.ITEM_ID, gst.BEF_STACK_AMT, gst.NEXT_STACK_AMT, gsf.COLOR, 0, 0, 0);
	//		sp.IUID = NEW_IUID;

	//		gsf.CheckGuildStorage(GUILD_ID, gsf.SLOT_ID, gsf);
	//		gst.CheckGuildStorage(GUILD_ID, gst.SLOT_ID, sp);
	//	}

	//	GAMEDB_TEST (USP_RZ_GUILD_STORAGE_SPLIT_2)
	//	{
	//		T_ITEM gsf (GSN, CSN, GUILD_ID, 1, 2, 3, 1, 5, 0, 0, 0);
	//		T_ITEM gst (GSN, CSN, GUILD_ID, 11, 0, 0, 2, 0, 0, 0, 0);

	//		gsf.InsertGuildStorage();

	//		TDBRecordSet rs;
	//		UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_GUILD_STORAGE_SPLIT (%d, %I64d, %d, %I64d, %d, %d, %d, %d)}"
	//			, GConfig::m_nMyWorldID, GUILD_ID
	//			, gsf.SLOT_ID, gsf.IUID, gsf.BEF_STACK_AMT, gsf.NEXT_STACK_AMT
	//			, gst.SLOT_ID, gst.NEXT_STACK_AMT);
	//		CHECK_EQUAL(1, rs.GetFetchedRowCount());
	//		const int64 NEW_IUID = rs.Field(L"NEW_GUILD_STORAGE_IUID").AsInt64();
	//		rs.Close();

	//		T_ITEM sp (GSN, CSN, GUILD_ID, gst.SLOT_ID, gsf.ITEM_ID, gst.BEF_STACK_AMT, gst.NEXT_STACK_AMT, gsf.COLOR, 0, 0, 0);
	//		sp.IUID = NEW_IUID;

	//		gsf.CheckGuildStorage(GUILD_ID, gsf.SLOT_ID, gsf);
	//		gst.CheckGuildStorage(GUILD_ID, gst.SLOT_ID, sp);
	//	}		
	//}
}


