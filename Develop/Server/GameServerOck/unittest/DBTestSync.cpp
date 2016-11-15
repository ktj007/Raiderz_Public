#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"


SUITE(DBTestSync)
{
	GAMEDB_TEST(DBTestSyncCharacter)
	{
		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int nXP		= 1000;
		const int nMoney	= 2000;


		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_SYNC (%d, %I64d, %I64d, %d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nXP, nMoney));

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT [EXP], [MONEY] FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", nCID);
		CHECK_EQUAL(nXP, rs.Field(L"EXP").AsInt());
		CHECK_EQUAL(nMoney, rs.Field(L"MONEY").AsInt());
		rs.Close();
	}

	GAMEDB_TEST(DBTestSyncItem)
	{
		const int64 nCID		= DBTestHelper::InsertCharacter(1, GConfig::m_nMyWorldID);
		const int nItemID		= DBTestHelper::InsertItemInfo(1);
		const uint8 nSlotType	= 1;
		const int16 nSlotID		= 0;
		const IUID nIUID		= DBTestHelper::InsertItem(1, nCID, GConfig::m_nMyWorldID, nItemID, 1, nSlotType, nSlotID);
		const int16 nStackAmt	= 20;
		const int nDura			= 5;

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_SYNC (%d, %I64d, %d, %d, %I64d, %d, %d)}"
			, GConfig::m_nMyWorldID, nCID, nSlotType, nSlotID, nIUID, nStackAmt, nDura));

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT STACK_AMT, DURA FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d;", nCID, nSlotType, nSlotID);
		CHECK_EQUAL(nStackAmt, rs.Field(L"STACK_AMT").AsShort());
		CHECK_EQUAL(nDura, rs.Field(L"DURA").AsByte());
		rs.Close();
	}
}