#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"
#include "GConfig.h"

SUITE(DBTestPalette)
{
	struct _PALETTE
	{
		uint8	SLOT_ID;
		uint8	SLOT_TYPE;
		int		ID;
	};

	GAMEDB_TEST(USP_RZ_CHAR_GET_PALETTE)
	{
		const int64 nAID = 1;
		const int64 nCID = DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		
		_PALETTE p1, p2;

		p1.SLOT_ID = 0;
		p1.SLOT_TYPE = 0;
		p1.ID = 1;

		p2.SLOT_ID = 1;
		p2.SLOT_TYPE = 1;
		p2.ID = 2;

		
		DBTestHelper::InsertPalette(GConfig::m_nMyWorldID, nAID, nCID, 2);
		DBTestHelper::SetPalette(nCID, p1.SLOT_ID, p1.SLOT_TYPE, p1.ID);
		DBTestHelper::SetPalette(nCID, p2.SLOT_ID, p2.SLOT_TYPE, p2.ID);
		

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_CHAR_GET_PALETTE (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, nAID, nCID));

		for (; !rs.IsEOF(); rs.MoveNext())
		{
			_PALETTE p = p1;
			if (p2.SLOT_ID == rs.Field(L"SLOT_ID").AsByte())
				p = p2;
			
			CHECK_EQUAL(p.SLOT_TYPE, rs.Field(L"ID_TYPE").AsByte());
			CHECK_EQUAL(p.ID, rs.Field(L"ID").AsInt());
		}
		
		rs.Close();
	}

	void SetPalette(bool bInsertPalette)
	{
		const int64 nAID = 1;
		const int64 nCID = DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);

		if (bInsertPalette)
			DBTestHelper::InsertPalette(GConfig::m_nMyWorldID, nAID, nCID, 1);

		const uint8 nOrdNum	= 0;
		const int	nID		= 1000;
		const uint8 nType	= 2;

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_PALETTE_SET (%d, %I64d, %I64d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nOrdNum, nID, nType));

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT [ID], ID_TYPE FROM dbo.RZ_PALETTE WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d AND SLOT_ID = %d;", nAID, nCID, nOrdNum);
		CHECK_EQUAL(nID, rs.Field(L"ID").AsInt());
		CHECK_EQUAL(nType, rs.Field(L"ID_TYPE").AsByte());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_PALETTE_SET)
	{
		SetPalette(true);
	}

	GAMEDB_TEST(USP_RZ_PALETTE_SET_SET)
	{
		SetPalette(false);
	}


	GAMEDB_TEST(USP_RZ_PALETTE_CHANGE)
	{
		const int64 nAID = 1;
		const int64 nCID = DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);

		_PALETTE p1, p2;

		p1.SLOT_ID = 0;
		p1.SLOT_TYPE = 0;
		p1.ID = 1;

		p2.SLOT_ID = 1;
		p2.SLOT_TYPE = 1;
		p2.ID = 2;


		DBTestHelper::InsertPalette(GConfig::m_nMyWorldID, nAID, nCID, 2);
		DBTestHelper::SetPalette(nCID, p1.SLOT_ID, p1.SLOT_TYPE, p1.ID);
		DBTestHelper::SetPalette(nCID, p2.SLOT_ID, p2.SLOT_TYPE, p2.ID);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_PALETTE_CHANGE (%d, %I64d, %I64d, %u, %d, %u, %u, %d, %u)}"
			, GConfig::m_nMyWorldID
			, nAID
			, nCID
			, p1.SLOT_ID, p1.ID, p1.SLOT_TYPE
			, p2.SLOT_ID, p2.ID, p2.SLOT_TYPE));
		
		
		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT SLOT_ID, ID_TYPE, [ID] FROM dbo.RZ_PALETTE WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d;", nAID, nCID);
		for (; !rs.IsEOF(); rs.MoveNext())
		{
			_PALETTE p = p1;
			if (p1.SLOT_ID == rs.Field(L"SLOT_ID").AsByte())
				p = p2;

			CHECK_EQUAL(p.ID, rs.Field(L"ID").AsInt());
			CHECK_EQUAL(p.SLOT_TYPE, rs.Field(L"ID_TYPE").AsByte());
		}

		rs.Close();
	}


	GAMEDB_TEST(USP_RZ_PALETTE_RESET)
	{
		const int64 nAID = 1;
		const int64 nCID = DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		
		const uint8 nOrdNum	= 0;
		const int	nID		= 1000;
		const uint8 nType	= 2;

		DBTestHelper::InsertPalette(GConfig::m_nMyWorldID, nAID, nCID, 1);
		DBTestHelper::SetPalette(nCID, nOrdNum, nType, nID);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_PALETTE_RESET (%d, %I64d, %I64d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, nOrdNum));
		
		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT [ID], ID_TYPE FROM dbo.RZ_PALETTE WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d AND SLOT_ID = %d;", nAID, nCID, nOrdNum);
		CHECK_EQUAL(0, rs.Field(L"ID").AsInt());
		CHECK_EQUAL(0, rs.Field(L"ID_TYPE").AsByte());
		rs.Close();
	}
}
