#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"
#include "GConfig.h"


SUITE(DBTestItemMerge)
{
	GAMEDB_TEST(USP_RZ_ITEM_MERGE_MERGED_ALL)
	{
		const int64 nAID				= 40;
		const int64	nCIDFrom			= 1;
		const int	ITEM_ID				= 12345;
		const int	nSlotTypeFrom		= 1;
		const int	nSlotIDFrom			= 0;
		const int64 nIUIDFrom			= 1;
		const int16 nBefStackAmtFrom	= 5;
		const int16 nNextStackAmtFrom	= 0;
		const int64 nCIDTo				= 2;
		const int	nSlotTypeTo			= 4;
		const int	nSlotIDTo			= 3;
		const int64 nIUIDTo				= 2;
		const int16 nBefStackAmtTo		= 6;
		const int16 nNextStackAmtTo		= 10;
		
		DBTestHelper::InsertEmptyItemSlot(nAID, nCIDFrom, GConfig::m_nMyWorldID, nSlotTypeFrom, nSlotIDFrom, nIUIDFrom);
		DBTestHelper::InsertEmptyItemSlot(nAID, nCIDTo, GConfig::m_nMyWorldID, nSlotTypeTo, nSlotIDTo, nIUIDTo);

		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET ITEM_ID = %d, STACK_AMT = %d WHERE IUID = %I64d;", ITEM_ID, nBefStackAmtFrom, nIUIDFrom);
		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET ITEM_ID = %d, STACK_AMT = %d WHERE IUID = %I64d;", ITEM_ID, nBefStackAmtTo, nIUIDTo);
			
		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_MERGE (%d, %I64d, %d, %d, %I64d, %d, %d, %I64d, %d, %d, %I64d, %d, %d)}"
			, GConfig::m_nMyWorldID
			, nCIDFrom, nSlotTypeFrom, nSlotIDFrom, nIUIDFrom, nBefStackAmtFrom, nNextStackAmtFrom
			, nCIDTo, nSlotTypeTo, nSlotIDTo, nIUIDTo, nBefStackAmtTo, nNextStackAmtTo);

		TDBRecordSet rsf;
		UTestDB.Execute(rsf, L"SELECT IUID, ITEM_ID, STACK_AMT FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d"
			, nCIDFrom, nSlotTypeFrom, nSlotIDFrom);
		CHECK_EQUAL(1, rsf.GetFetchedRowCount());
		CHECK_EQUAL((int64)0, rsf.Field(L"IUID").AsInt64());
		CHECK(rsf.Field(L"ITEM_ID").IsNull());
		CHECK_EQUAL(nNextStackAmtFrom, rsf.Field(L"STACK_AMT").AsInt());
		rsf.Close();

		TDBRecordSet rst;
		UTestDB.Execute(rst, L"SELECT IUID, ITEM_ID, STACK_AMT FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d"
			, nCIDTo, nSlotTypeTo, nSlotIDTo);
		CHECK_EQUAL(1, rst.GetFetchedRowCount());
		CHECK_EQUAL(nIUIDTo, rst.Field(L"IUID").AsInt64());
		CHECK_EQUAL(ITEM_ID, rst.Field(L"ITEM_ID").AsInt());
		CHECK_EQUAL(nNextStackAmtTo, rst.Field(L"STACK_AMT").AsInt());
		rst.Close();
	}

	GAMEDB_TEST(USP_RZ_ITEM_MERGE_MERGED_REMAIN)
	{
		const int64 nAID				= 40;
		const int64	nCIDFrom			= 1;
		const int	ITEM_ID				= 12345;
		const int	nSlotTypeFrom		= 1;
		const int	nSlotIDFrom			= 0;
		const int64 nIUIDFrom			= 1;
		const int16 nBefStackAmtFrom	= 5;
		const int16 nNextStackAmtFrom	= 1;
		const int64 nCIDTo				= 2;
		const int	nSlotTypeTo			= 4;
		const int	nSlotIDTo			= 3;
		const int64 nIUIDTo				= 2;
		const int16 nBefStackAmtTo		= 6;
		const int16 nNextStackAmtTo		= 10;

		DBTestHelper::InsertEmptyItemSlot(nAID, nCIDFrom, GConfig::m_nMyWorldID, nSlotTypeFrom, nSlotIDFrom, nIUIDFrom);
		DBTestHelper::InsertEmptyItemSlot(nAID, nCIDTo, GConfig::m_nMyWorldID, nSlotTypeTo, nSlotIDTo, nIUIDTo);

		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET ITEM_ID = %d, STACK_AMT = %d WHERE IUID = %I64d;", ITEM_ID, nBefStackAmtFrom, nIUIDFrom);
		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET ITEM_ID = %d, STACK_AMT = %d WHERE IUID = %I64d;", ITEM_ID, nBefStackAmtTo, nIUIDTo);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_MERGE (%d, %I64d, %d, %d, %I64d, %d, %d, %I64d, %d, %d, %I64d, %d, %d)}"
			, GConfig::m_nMyWorldID
			, nCIDFrom, nSlotTypeFrom, nSlotIDFrom, nIUIDFrom, nBefStackAmtFrom, nNextStackAmtFrom
			, nCIDTo, nSlotTypeTo, nSlotIDTo, nIUIDTo, nBefStackAmtTo, nNextStackAmtTo);

		TDBRecordSet rsf;
		UTestDB.Execute(rsf, L"SELECT IUID, ITEM_ID, STACK_AMT FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d"
			, nCIDFrom, nSlotTypeFrom, nSlotIDFrom);
		CHECK_EQUAL(1, rsf.GetFetchedRowCount());
		CHECK_EQUAL(nIUIDFrom, rsf.Field(L"IUID").AsInt64());
		CHECK_EQUAL(ITEM_ID, rsf.Field(L"ITEM_ID").AsInt());
		CHECK_EQUAL(nNextStackAmtFrom, rsf.Field(L"STACK_AMT").AsInt());
		rsf.Close();

		TDBRecordSet rst;
		UTestDB.Execute(rst, L"SELECT IUID, ITEM_ID, STACK_AMT FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d"
			, nCIDTo, nSlotTypeTo, nSlotIDTo);
		CHECK_EQUAL(1, rst.GetFetchedRowCount());
		CHECK_EQUAL(nIUIDTo, rst.Field(L"IUID").AsInt64());
		CHECK_EQUAL(ITEM_ID, rst.Field(L"ITEM_ID").AsInt());
		CHECK_EQUAL(nNextStackAmtTo, rst.Field(L"STACK_AMT").AsInt());
		rst.Close();
	}
}