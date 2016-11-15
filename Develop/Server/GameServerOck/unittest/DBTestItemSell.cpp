#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GItemConvertPrefixedString.h"
#include "GConfig.h"


SUITE(DBItemSell)
{
	class FIXTURE_CHARACTER_AND_ITEM_INSTANCE
	{
	public :
		FIXTURE_CHARACTER_AND_ITEM_INSTANCE()
		{
			m_nItemID = 1;
			CHECK_EQUAL(m_nItemID, DBTestHelper::InsertItemInfo(m_nItemID, L"test_item"));

			m_nAID = 1;
			m_nCID = DBTestHelper::InsertCharacter(m_nAID, GConfig::m_nMyWorldID, L"test_char");
			CHECK(0 < m_nCID);

			m_nSlotType		= SLOTTYPE_INVENTORY;
			m_nSlotID		= 1;
			m_nMaxStackAmt	= 10;
			m_nMaxDura		= 10;
			m_nColor		= 2;
			m_bClaimed		= false;
			m_nCharPtm		= 10;
			m_bPeriodItem	= true;
			m_nUsagePeriod	= 5;
			m_nEnchCnt		= 6;
			m_strExpiDt		= L"NULL";
			m_nMoney		= 100;
			m_nModMoney		= 10;
			m_nXP			= 112;
			
			m_nIUID = DBTestHelper::InsertItem(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nItemID, m_nMaxStackAmt, m_nSlotType, m_nSlotID, false, m_nUsagePeriod);
			CHECK(0 < m_nIUID);

			CHECK(UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET DURA = %d, MAX_DURA = %d WHERE IUID = %d;", m_nMaxDura, m_nMaxDura, m_nIUID));
		}

		void CHECK_CHARACTER()
		{
			TDBRecordSet rsChar;
			UTestDB.Execute(rsChar, L"SELECT [EXP], [MONEY] FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", m_nCID);
			CHECK_EQUAL(m_nMoney, rsChar.Field(L"MONEY").AsInt());
			CHECK_EQUAL(m_nXP, rsChar.Field(L"EXP").AsInt());
			rsChar.Close();			
		}

		void SELL_ITEM_DELETE()
		{
			CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_SELL_DELETED (%d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %d)}"
				, GConfig::m_nMyWorldID, m_nAID, m_nCID, m_nSlotType, m_nSlotID, m_nIUID, m_nCharPtm, m_nXP, m_nMoney));
		}

	public :
		int64	m_nAID;
		int64	m_nCID;
		int		m_nItemID;
		uint8	m_nSlotType; 
		int16	m_nSlotID;
		uint8	m_nMaxDura;
		int16	m_nMaxStackAmt;
		int		m_nColor;
		bool	m_bClaimed;
		int		m_nCharPtm;
		bool	m_bPeriodItem;
		int		m_nUsagePeriod;
		wstring	m_strExpiDt;
		int64	m_nIUID;	
		uint8	m_nEnchCnt;
		int		m_nMoney;
		int		m_nModMoney;
		int		m_nXP;
	};

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_INSTANCE, USP_RZ_ITEM_SELL_DELETED)
	{
		SELL_ITEM_DELETE();		
		CHECK_CHARACTER();
		
		TDBRecordSet rsItem;
		UTestDB.Execute(rsItem, L"SELECT ITEM_ID, IUID, STACK_AMT, CHAR_PTM \
								 FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d;", m_nCID, m_nSlotType, m_nSlotID);
		CHECK_EQUAL(1, rsItem.GetFetchedRowCount());
		CHECK(rsItem.Field(L"ITEM_ID").IsNull());
		CHECK_EQUAL(0, rsItem.Field(L"IUID").AsInt64());
		CHECK_EQUAL(0, rsItem.Field(L"STACK_AMT").AsShort());
		CHECK(rsItem.Field(L"CHAR_PTM").IsNull());
		rsItem.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_INSTANCE, USP_RZ_ITEM_SELL_UPDATE)
	{
		const int16 nModStackAmt	= 5;
		const int16 nStackAmt		= m_nMaxStackAmt - nModStackAmt;

		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET DURA = 0, MAX_DURA = 0, PERIOD = 0, USAGE_PERIOD = 0, EFF_END_DATE = 0 WHERE IUID = %I64d", m_nIUID);
		
		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_SELL_UPDATE (%d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, m_nAID, m_nCID, m_nSlotType, m_nSlotID, m_nIUID, nStackAmt, m_nCharPtm, m_nXP, m_nMoney));

		CHECK_CHARACTER();
		
		TDBRecordSet rsItem;
		UTestDB.Execute(rsItem, L"SELECT STACK_AMT FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d;", m_nCID, m_nSlotType, m_nSlotID);
		CHECK_EQUAL(nStackAmt, rsItem.Field(L"STACK_AMT").AsShort());		
		rsItem.Close();
	}

	GAMEDB_TEST(USP_RZ_ITEM_SELL_VERY_COMMON)
	{
		const int nItemID_1 = 1;
		const int nItemID_2 = 2;
		const int nItemID_3 = 3;
		const int nXP		= 102134;

		DBTestHelper::InsertItemInfo(nItemID_1, L"test_item_1");
		DBTestHelper::InsertItemInfo(nItemID_2, L"test_item_2");
		DBTestHelper::InsertItemInfo(nItemID_3, L"test_item_3");

		UTestDB.Execute(L"UPDATE dbo.RZ_ITEM_INFO SET TIER = CASE WHEN ITEM_ID < 3 THEN 0 ELSE 1 END;");

		const int64 nAID	= 1;
		const int64 nCID	= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID, L"test_char");
		const int nCharPtm	= 123;
		const int nMoney	= 80;
		const int nModMoney = 10;

		const uint8 nSlotType			= SLOTTYPE_INVENTORY;
		const int16 nStackAmt			= 1;
		const int16 nVeryComSlotID_1	= 0;
		const int16 nVeryComSlotID_2	= 1;
		const int16 nComSlotID			= 2;

		const int64 nIUID_1 = DBTestHelper::InsertItem(nAID, nCID, GConfig::m_nMyWorldID, nItemID_1, nStackAmt, nSlotType, nVeryComSlotID_1);
		const int64 nIUID_2 = DBTestHelper::InsertItem(nAID, nCID, GConfig::m_nMyWorldID, nItemID_2, nStackAmt, nSlotType, nVeryComSlotID_2);
		const int64 nIUID_3 = DBTestHelper::InsertItem(nAID, nCID, GConfig::m_nMyWorldID, nItemID_3, nStackAmt, nSlotType, nComSlotID);

		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET DURA = 0, MAX_DURA = 0 WHERE IUID IN (%I64d, %I64d);", nIUID_1, nIUID_2);

		wstring strString;
		GItemConvertPrefixedString cov;

		strString += cov.Convert((SH_ITEM_SLOT_TYPE)nSlotType, nVeryComSlotID_1);
		strString += cov.Convert((SH_ITEM_SLOT_TYPE)nSlotType, nVeryComSlotID_2);
		

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_SELL_VERY_COMMON (%d, %I64d, %I64d, '%s', %d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, nAID, nCID, /*szSellItem*/ strString.c_str(), 2, nCharPtm, nXP, nMoney));

		TDBRecordSet rsChar;
		CHECK(UTestDB.Execute(rsChar, L"SELECT [EXP], [MONEY], CHAR_PTM FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", nCID));
		CHECK_EQUAL(nXP, rsChar.Field(L"EXP").AsInt());
		CHECK_EQUAL(nMoney, rsChar.Field(L"MONEY").AsInt());
		CHECK_EQUAL(nCharPtm, rsChar.Field(L"CHAR_PTM").AsInt());
		rsChar.Close();

		TDBRecordSet rsSoldItemInst;
		CHECK(UTestDB.Execute(rsSoldItemInst, L"SELECT COUNT(*) cnt FROM dbo.RZ_INVEN \
											   WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID IN (%d, %d) AND ITEM_ID IS NOT NULL AND STACK_AMT > 0 AND IUID > 0;"
			, nCID, nSlotType, nVeryComSlotID_1, nVeryComSlotID_2));
		CHECK_EQUAL(0, rsSoldItemInst.Field(L"cnt").AsInt());
		rsSoldItemInst.Close();

		TDBRecordSet rsRemainItemInst;
		CHECK(UTestDB.Execute(rsRemainItemInst, L"SELECT ITEM_ID FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d;"
			, nCID, nSlotType, nComSlotID));
		CHECK_EQUAL(nItemID_3, rsRemainItemInst.Field(L"ITEM_ID").AsInt());
		rsRemainItemInst.Close();				
	}
}