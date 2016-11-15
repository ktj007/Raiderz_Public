#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"
#include "GConfig.h"


SUITE(DBTestTalent)
{
	GAMEDB_TEST(USP_RZ_CHAR_GET_TALENT)
	{
		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int nTaltID	= DBTestHelper::InsertTalent(1);
		DBTestHelper::InsertTalentInstance(GConfig::m_nMyWorldID, nAID, nCID, nTaltID);

		TDBRecordSet rsTList;
		CHECK(UTestDB.Execute(rsTList, L"{CALL dbo.USP_RZ_CHAR_GET_TALENT (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, nAID, nCID));
		CHECK_EQUAL(nTaltID, rsTList.Field(L"TALENT_ID").AsInt());
		rsTList.Close();
	}

	void CHECK_CHARACTER(const int64 nCID, const int16 nRemainTP, const int nCharPtm)
	{
		TDBRecordSet rsChar;
		UTestDB.Execute(rsChar, L"SELECT REMAIN_TP, CHAR_PTM FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", nCID);
		CHECK_EQUAL(nRemainTP, rsChar.Field(L"REMAIN_TP").AsShort());
		CHECK_EQUAL(nCharPtm, rsChar.Field(L"CHAR_PTM").AsInt());
		rsChar.Close();
	}

	void CHECK_TALENT(const int64 nCID, const int nTaltID, const bool bExist)
	{
		TDBRecordSet rsTalent;
		UTestDB.Execute(rsTalent, L"SELECT TALENT_ID FROM dbo.RZ_TALENT WHERE CHAR_ID = %I64d AND TALENT_ID = %d;", nCID, nTaltID);
		CHECK_EQUAL(!bExist, rsTalent.Field(L"TALENT_ID").IsNull());
		rsTalent.Close();
	}

	void CHECK_PALETTE(const int64 nCID, const int nOrdNum, const int nTatlID, const uint8 nType)
	{
		TDBRecordSet rsPalette;
		UTestDB.Execute(rsPalette, L"SELECT [ID], ID_TYPE FROM dbo.RZ_PALETTE WHERE CHAR_ID = %I64d AND SLOT_ID = %d;", nCID, nOrdNum);
		CHECK_EQUAL(nTatlID, rsPalette.Field(L"ID").AsInt());
		CHECK_EQUAL(nType, rsPalette.Field(L"ID_TYPE").AsByte());
		CHECK_EQUAL(1, rsPalette.GetFetchedRowCount());
		rsPalette.Close();
	}

	GAMEDB_TEST(USP_RZ_TALENT_LEARN_NOT_ON_PALETTE)
	{
		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID, L"test_character", 10);
		const int nTaltID	= DBTestHelper::InsertTalent(1);
		const int nCharPtm	= 123;
		const int nRemainTP	= 9;
		DBTestHelper::InsertPalette(GConfig::m_nMyWorldID, nAID, nCID, 1);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_TALENT_LEARN (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nCharPtm, nTaltID, nRemainTP, false, 0, 0));

		CHECK_CHARACTER(nCID, nRemainTP, nCharPtm);
		CHECK_TALENT(nCID, nTaltID, true);
		CHECK_PALETTE(nCID, 0, 0, 0);
	}

	GAMEDB_TEST(USP_RZ_TALENT_LEARN_ON_PALETTE)
	{
		const int64 nAID			= 1;
		const int64 nCID			= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID, L"test_character", 10);
		const int nTaltID_OnPalette	= DBTestHelper::InsertTalent(1, L"test_talent_1");
		const int nTaltID_Learn		= DBTestHelper::InsertTalent(2, L"test_talent_2");		
		const int nCharPtm	= 123;
		const int nRemainTP	= 9;

		DBTestHelper::InsertPalette(GConfig::m_nMyWorldID, nAID, nCID, 1);
		DBTestHelper::SetPalette(nCID, 0, 2, nTaltID_OnPalette);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_TALENT_LEARN (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nCharPtm, nTaltID_Learn, nRemainTP, true, nTaltID_OnPalette, 1));

		CHECK_CHARACTER(nCID, nRemainTP, nCharPtm);
		CHECK_TALENT(nCID, nTaltID_Learn, true);
		CHECK_PALETTE(nCID, 0, nTaltID_Learn, 2);		
	}

	GAMEDB_TEST(USP_RZ_TALENT_RESET_NOT_ON_PALETTE)
	{
		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID, L"test_character", 10);
		const int nTaltID	= DBTestHelper::InsertTalent(1);
		const int nCharPtm	= 123;
		const int nRemainTP	= 10;
		DBTestHelper::InsertPalette(GConfig::m_nMyWorldID, nAID, nCID, 1);
		DBTestHelper::InsertTalentInstance(GConfig::m_nMyWorldID, nAID, nCID, nTaltID);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_TALENT_RESET (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nCharPtm, nTaltID, nRemainTP, false, 0, 0));

		CHECK_CHARACTER(nCID, nRemainTP, nCharPtm);
		CHECK_TALENT(nCID, nTaltID, false);
		CHECK_PALETTE(nCID, 0, 0, 0);		
	}

	GAMEDB_TEST(USP_RZ_TALENT_RESET_ON_PALETTE)
	{
		const int64 nAID			= 1;
		const int64 nCID			= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID, L"test_character", 10);
		const int nTaltID_Down		= DBTestHelper::InsertTalent(1, L"test_talent_1");
		const int nTaltID_OnPalette	= DBTestHelper::InsertTalent(2, L"test_talent_2");		
		const int nCharPtm	= 123;
		const int nRemainTP	= 10;

		DBTestHelper::InsertTalentInstance(GConfig::m_nMyWorldID, nAID, nCID, nTaltID_Down);
		DBTestHelper::InsertTalentInstance(GConfig::m_nMyWorldID, nAID, nCID, nTaltID_OnPalette);
		DBTestHelper::InsertPalette(GConfig::m_nMyWorldID, nAID, nCID, 1);
		DBTestHelper::SetPalette(nCID, 0, 2, nTaltID_OnPalette);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_TALENT_RESET (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nCharPtm, nTaltID_OnPalette, nRemainTP, true, nTaltID_Down, 1));

		CHECK_CHARACTER(nCID, nRemainTP, nCharPtm);
		CHECK_TALENT(nCID, nTaltID_OnPalette, false);
		CHECK_PALETTE(nCID, 0, nTaltID_Down, 2);		
	}

	GAMEDB_TEST(USP_RZ_TALENT_RESET_ALL)
	{
		const int64 nAID			= 1;
		const int64 nCID			= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID, L"test_character", 10);
		const int nTaltID_1			= DBTestHelper::InsertTalent(1, L"test_talent_1", 1, true);
		const int nTaltID_2			= DBTestHelper::InsertTalent(2, L"test_talent_2", 1, true);		
		const int nTaltID_NotReset	= DBTestHelper::InsertTalent(3, L"test_talent_3", 1, false);		
		const int nCharPtm			= 123;
		const int nTotalTP			= 10;

		DBTestHelper::InsertTalentInstance(GConfig::m_nMyWorldID, nAID, nCID, nTaltID_1);
		DBTestHelper::InsertTalentInstance(GConfig::m_nMyWorldID, nAID, nCID, nTaltID_2);
		DBTestHelper::InsertTalentInstance(GConfig::m_nMyWorldID, nAID, nCID, nTaltID_NotReset);

		const int SLOT_ID = 0;
		const int64 IUID = DBTestHelper::InsertItem(nAID, nCID, GConfig::m_nMyWorldID, 1, 1, SLOTTYPE_INVENTORY, SLOT_ID);
		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET ACCN_ID = %I64d WHERE IUID = %I64d;", nAID, IUID);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_TALENT_RESET_ALL (%d, %I64d, %I64d, %d, %d, %d, %d, %I64d)}"
			, GConfig::m_nMyWorldID
			, nAID, nCID, nCharPtm, nTotalTP, 0, SLOT_ID, IUID));

		CHECK_CHARACTER(nCID, nTotalTP, nCharPtm);
		CHECK_TALENT(nCID, nTaltID_1, false);
		CHECK_TALENT(nCID, nTaltID_2, false);
		CHECK_TALENT(nCID, nTaltID_NotReset, true);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT ITEM_ID, IUID FROM dbo.RZ_INVEN WHERE ACCN_ID = %I64d AND OWNER_ID = %I64d AND SLOT_ID = %d"
			, nAID, nCID, SLOT_ID);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK(rs.Field(L"ITEM_ID").IsNull());
		CHECK_EQUAL(0, rs.Field(L"IUID").AsInt64());
		rs.Close();
	}
}
