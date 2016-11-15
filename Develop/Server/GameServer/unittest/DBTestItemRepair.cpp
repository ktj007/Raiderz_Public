#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GItemConvertPrefixedString.h"
#include "GConfig.h"


SUITE(DBTestItemRepair)
{
	class FIXTURE
	{
	public :
		FIXTURE()
		{
			m_nAID		= 1;
			m_nCID		= DBTestHelper::InsertCharacter(m_nAID, GConfig::m_nMyWorldID, L"test_char");
			m_nCharPtm	= 123;
			m_nMoney	= 1000;
			m_nXP		= 1231;
			m_nModMoney	= 10;
		}

	public :
		int64 m_nAID;
		int64 m_nCID;
		int m_nCharPtm;
		int m_nMoney;
		int m_nModMoney;
		int m_nXP;
	};

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_REPAIR)
	{
		const int nItemID = 1;
		DBTestHelper::InsertItemInfo(nItemID, L"test_item");

		const uint8 nSlotType	= 1;
		const int16 nSlotID		= 0;
		const int64	nIUID		= DBTestHelper::InsertItem(m_nAID, m_nCID, GConfig::m_nMyWorldID, nItemID, 1, nSlotType, nSlotID);
		const int	nMaxDura	= 100;
		
		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET DURA = 90, MAX_DURA = %d WHERE IUID = %I64d;", nMaxDura, nIUID);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_REPAIR (%d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, m_nAID, m_nCID, nSlotType, nSlotID, nIUID, m_nCharPtm, m_nXP, m_nMoney));

		TDBRecordSet rsChar;
		CHECK(UTestDB.Execute(rsChar, L"SELECT [EXP], [MONEY], CHAR_PTM FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", m_nCID));
		CHECK_EQUAL(m_nXP, rsChar.Field(L"EXP").AsInt());
		CHECK_EQUAL(m_nMoney, rsChar.Field(L"MONEY").AsInt());
		CHECK_EQUAL(m_nCharPtm, rsChar.Field(L"CHAR_PTM").AsInt());
		rsChar.Close();

		TDBRecordSet rsItem;
		CHECK(UTestDB.Execute(rsItem, L"SELECT DURA FROM dbo.RZ_INVEN WHERE IUID = %I64d", nIUID));
		CHECK_EQUAL(nMaxDura, rsItem.Field(L"DURA").AsByte());
		rsItem.Close();		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_REPAIR_ALL)
	{
		const int nItemID_1 = 1;
		const int nItemID_2 = 2;

		DBTestHelper::InsertItemInfo(nItemID_1, L"test_item_1");
		DBTestHelper::InsertItemInfo(nItemID_2, L"test_item_2");

		const uint8 nSlotType	= 1;
		const int16 nSlotID_1	= 0;
		const int16 nSlotID_2	= 1;
		const uint8	nMaxDura_1	= 100;
		const uint8 nMaxDura_2	= 200;

		const int64 nIUID_1 = DBTestHelper::InsertItem(m_nAID, m_nCID, GConfig::m_nMyWorldID, nItemID_1, 1, nSlotType, nSlotID_1);
		const int64 nIUID_2 = DBTestHelper::InsertItem(m_nAID, m_nCID, GConfig::m_nMyWorldID, nItemID_2, 1, nSlotType, nSlotID_2);

		const wchar_t* szUpdMaxDuraSQL = L"UPDATE dbo.RZ_INVEN SET MAX_DURA = %d WHERE IUID = %I64d;";
		UTestDB.Execute(szUpdMaxDuraSQL, nMaxDura_1, nIUID_1);
		UTestDB.Execute(szUpdMaxDuraSQL, nMaxDura_2, nIUID_2);
		
		wstring strString;
		GItemConvertPrefixedString cov;
		strString += cov.Convert((SH_ITEM_SLOT_TYPE)nSlotType, nSlotID_1);
		strString += cov.Convert((SH_ITEM_SLOT_TYPE)nSlotType, nSlotID_2);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_REPAIR_ALL (%d, %I64d, %I64d, '%s', %d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, m_nAID, m_nCID, strString.c_str(), 2, m_nCharPtm, m_nXP, m_nMoney));

		TDBRecordSet rsChar;
		CHECK(UTestDB.Execute(rsChar, L"SELECT [EXP], [MONEY], CHAR_PTM FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", m_nCID));
		CHECK_EQUAL(m_nXP, rsChar.Field(L"EXP").AsInt());
		CHECK_EQUAL(m_nMoney, rsChar.Field(L"MONEY").AsInt());
		CHECK_EQUAL(m_nCharPtm, rsChar.Field(L"CHAR_PTM").AsInt());
		rsChar.Close();

		const wchar_t* szDuraSQL = L"SELECT DURA FROM dbo.RZ_INVEN WHERE IUID = %I64d;";

		TDBRecordSet rsItem1;
		UTestDB.Execute(rsItem1, szDuraSQL, nIUID_1);
		CHECK_EQUAL(nMaxDura_1, rsItem1.Field(L"DURA").AsByte());
		rsItem1.Close();

		TDBRecordSet rsItem2;
		UTestDB.Execute(rsItem2, szDuraSQL, nIUID_2);
		CHECK_EQUAL(nMaxDura_2, rsItem2.Field(L"DURA").AsByte());
		rsItem2.Close();
	}
}