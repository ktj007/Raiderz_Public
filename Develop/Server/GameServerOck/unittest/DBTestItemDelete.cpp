#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"


SUITE(DBTestItemDelete)
{
	class FIXTURE
	{
	public :
		FIXTURE()
		{
			m_nItemID		= 1;			
			m_nMaxStackAmt	= 10;

			m_nCID = DBTestHelper::InsertCharacter(1, GConfig::m_nMyWorldID, L"test_char");
			CHECK(0 < m_nCID);

			CHECK_EQUAL(m_nItemID, DBTestHelper::InsertItemInfo(m_nItemID, L"test_item", m_nMaxStackAmt));

			m_nSlotType		= SLOTTYPE_INVENTORY;
			m_nSlotID		= 1;
			m_nMaxDura		= 10;
			m_nEnchCnt		= 5;
			m_nColor		= 2;
			m_bClaimed		= false;
			m_bPeriodItem	= true;
			m_nCharPtm		= 10;
			m_nUsagePeriod	= 10;
			m_strExpiDt		= L"2010-7-21 12:56:0";

			m_nIUID = DBTestHelper::InsertItem(1, m_nCID, GConfig::m_nMyWorldID, m_nItemID, m_nMaxStackAmt, m_nSlotType, m_nSlotID, m_bClaimed);
			CHECK(0 < m_nIUID);
			CHECK(UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET PERIOD = %d, USAGE_PERIOD = %d, EFF_END_DATE = '%s' WHERE IUID = %I64d;"
				, m_bPeriodItem, m_nUsagePeriod, m_strExpiDt.c_str(), m_nIUID));
		}

		void USP_RZ_ITEM_DELETE(const uint8 nTier)
		{
			const wstring strExpiDt = wstring(L"'") + m_strExpiDt + wstring(L"'");

			CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_DELETE (%d, %I64d, %d, %d, %I64d)}"
				, GConfig::m_nMyWorldID, m_nCID, m_nSlotType, m_nSlotID, m_nIUID));
		}

		void CHECK_DELETED_ITEM()
		{
			TDBRecordSet rsItem;
			UTestDB.Execute(rsItem, L"SELECT OWNER_ID, STORAGE_REG_DATE FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d AND ITEM_ID IS NULL AND STACK_AMT = 0;"
				, m_nCID, m_nSlotType, m_nSlotID);
			CHECK_EQUAL(1, rsItem.GetFetchedRowCount());
			CHECK(rsItem.Field(L"STORAGE_REG_DATE").IsNull());
			rsItem.Close();
		}

	public :
		int64	m_nCID;		
		int		m_nItemID;
		uint8	m_nMaxDura;
		uint8	m_nMaxStackAmt;
		uint8	m_nEnchCnt;
		uint8	m_nSlotType; 
		int16	m_nSlotID;
		int		m_nColor;
		bool	m_bClaimed;
		bool	m_bPeriodItem;
		int		m_nCharPtm;		
		int		m_nUsagePeriod;
		wstring	m_strExpiDt;
		int64	m_nIUID;
	};

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_DELETE_TIER_VERY_COMMON)
	{
		USP_RZ_ITEM_DELETE(ITEM_TIER_VERY_COMMON);
		CHECK_DELETED_ITEM();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_DELETE_TIER_COMMON)
	{
		USP_RZ_ITEM_DELETE(ITEM_TIER_COMMON);
		CHECK_DELETED_ITEM();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_DELETE_TIER_RARE)
	{
		USP_RZ_ITEM_DELETE(ITEM_TIER_RARE);
		CHECK_DELETED_ITEM();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_DELETE_TIER_TREASURE)
	{
		USP_RZ_ITEM_DELETE(ITEM_TIER_TREASURE);
		CHECK_DELETED_ITEM();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_DELETE_TIER_LEGENDARY)
	{
		USP_RZ_ITEM_DELETE(ITEM_TIER_LEGENDARY);
		CHECK_DELETED_ITEM();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_DELETE_TIER_EPIC)
	{
		USP_RZ_ITEM_DELETE(ITEM_TIER_EPIC);
		CHECK_DELETED_ITEM();
	}
}