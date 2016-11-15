#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"
#include "Gconfig.h"


SUITE(DBTestItemLoot)
{
	class FIXTURE_LOOT_INSERT
	{
	public :
		FIXTURE_LOOT_INSERT()
		{
			m_nAID			= 1;
			m_nCID			= DBTestHelper::InsertCharacter(m_nAID, GConfig::m_nMyWorldID);
			m_nItemID		= DBTestHelper::InsertItemInfo(1);
			m_nSlotType		= 1;
			m_nSlotID		= 1;
			m_nStackAmt		= 2;
			m_nSoulCnt		= 3;
			m_nDura			= 4;
			m_nMaxDura		= 5;
			m_nEnchCnt		= 6;
			m_nColor		= 7;
			m_bClaimed		= false;
			m_bPeriodItem	= true;
			m_nUsagePeriod	= 8;
			m_strExpiDt		= L"2100-11-1 1:2:3";			
		}

		void CHECK_COUNT()
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT COUNT(*) AS CNT FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d", m_nCID);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(1, rs.Field(L"CNT").AsInt());
			rs.Close();
		}

		IUID ItemLoot(const uint8 nTier)
		{
			const wstring strExpiDt = wstring(L"'") + m_strExpiDt + wstring(L"'");

			TDBRecordSet rsIUID;
			UTestDB.Execute(rsIUID, L"{CALL dbo.USP_RZ_ITEM_LOOT_INSERT (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %s)}"
				, GConfig::m_nMyWorldID
				, m_nAID, m_nCID, m_nSlotType, m_nSlotID, m_nItemID, m_nStackAmt, m_nDura, m_nMaxDura, m_nColor, m_bClaimed
				, m_nCharPtm, m_bPeriodItem, m_nUsagePeriod, strExpiDt.c_str());
			CHECK_EQUAL(1, rsIUID.GetFetchedRowCount());
			const IUID nIUID = rsIUID.Field(L"IUID").AsInt64();
			CHECK(0 < nIUID);
			rsIUID.Close();
			return nIUID;
		}

		void CHECK_INSERTED_ITEM(const IUID nIUID)
		{
			TDBRecordSet rsItem;
			UTestDB.Execute(rsItem, L"SELECT OWNER_ID, SLOT_TYPE, SLOT_ID, ITEM_ID, STACK_AMT, SOUL_COUNT, DURA, MAX_DURA, COLOR, CLAIMED, CHAR_PTM \
										, PERIOD, USAGE_PERIOD, EFF_END_DATE FROM dbo.RZ_INVEN WHERE IUID = %I64d;", nIUID);
			CHECK_EQUAL(1, rsItem.GetFetchedRowCount());
			CHECK_EQUAL(m_nCID, rsItem.Field(L"OWNER_ID").AsInt());
			CHECK_EQUAL(m_nSlotType, rsItem.Field(L"SLOT_TYPE").AsByte());
			CHECK_EQUAL(m_nSlotID, rsItem.Field(L"SLOT_ID").AsShort());
			CHECK_EQUAL(m_nItemID, rsItem.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(m_nStackAmt, rsItem.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(m_nDura, rsItem.Field(L"DURA").AsByte());
			CHECK_EQUAL(m_nMaxDura, rsItem.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(m_nColor, rsItem.Field(L"COLOR").AsInt());
			CHECK_EQUAL(m_bClaimed, rsItem.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(m_nCharPtm, rsItem.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_bPeriodItem, rsItem.Field(L"PERIOD").AsBool());
			CHECK_EQUAL(m_nUsagePeriod, rsItem.Field(L"USAGE_PERIOD").AsInt());
			CHECK(m_strExpiDt == rsItem.Field(L"EFF_END_DATE").AsWString());
			rsItem.Close();
		}

		int64		m_nAID;
		int64		m_nCID;
		uint8		m_nSlotType;
		int16		m_nSlotID;
		int			m_nItemID;
		int16		m_nStackAmt;
		uint8		m_nSoulCnt;
		uint8		m_nDura;
		uint8		m_nMaxDura;
		uint8		m_nEnchCnt;
		int			m_nColor;
		bool		m_bClaimed;
		int			m_nCharPtm;
		bool		m_bPeriodItem;
		int			m_nUsagePeriod;
		wstring		m_strExpiDt;	
	};

	GAMEDB_TEST_FIXTURE(FIXTURE_LOOT_INSERT, USP_RZ_ITEM_LOOT_INSERT_NOT_LOG_TIER)
	{
		DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nSlotType, m_nSlotID);
		const IUID nIUID = ItemLoot(0);
		CHECK_INSERTED_ITEM(nIUID);
		CHECK_COUNT();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_LOOT_INSERT, USP_RZ_ITEM_LOOT_INSERT_CREATE)
	{
		const IUID nIUID = ItemLoot(0);
		CHECK_INSERTED_ITEM(nIUID);
		CHECK_COUNT();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_LOOT_INSERT, USP_RZ_ITEM_LOOT_INSERT_LOG_TIER)
	{
		DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nSlotType, m_nSlotID);
		const IUID nIUID = ItemLoot(2);
		CHECK_INSERTED_ITEM(nIUID);
		CHECK_COUNT();
	}

	GAMEDB_TEST(USP_RZ_ITEM_LOOT_UPDATE_NOT)
	{
		const int64	nCID			= DBTestHelper::InsertCharacter(1, GConfig::m_nMyWorldID);
		const int	nCharPtm		= 123;
		const int	nItemID			= DBTestHelper::InsertItemInfo(1);
		const uint8 nSlotType		= 1;
		const int16 nSlotID			= 0;
		const int16	nStackAmt		= 5;
		const int16	nAddStackAmt	= 5;
		const int64 nIUID			= DBTestHelper::InsertItem(1, nCID, GConfig::m_nMyWorldID, nItemID, nStackAmt, nSlotType, nSlotID, false, 0, -1);

		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET DURA = 0, MAX_DURA = 0 WHERE IUID = %I64d;", nIUID);
		
		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_LOOT_UPDATE (%d, %I64d, %d, %d, %I64d, %d)}"
			, GConfig::m_nMyWorldID, nCID, nSlotType, nSlotID, nIUID, nStackAmt + nAddStackAmt));

		TDBRecordSet rsAmt;
		UTestDB.Execute(rsAmt, L"SELECT STACK_AMT FROM dbo.RZ_INVEN WHERE IUID = %I64d;", nIUID);
		CHECK_EQUAL(nStackAmt + nAddStackAmt, rsAmt.Field(L"STACK_AMT").AsShort());
		rsAmt.Close();
	}
}