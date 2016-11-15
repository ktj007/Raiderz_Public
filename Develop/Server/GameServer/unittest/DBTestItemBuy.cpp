#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"


SUITE(DBTestItemBuy)
{
	class FIXTURE_CHARACTER_AND_ITEM_DEF
	{
	public :
		FIXTURE_CHARACTER_AND_ITEM_DEF()
		{
			m_nAID			= 1;
			m_nCID			= DBTestHelper::InsertCharacter(m_nAID, GConfig::m_nMyWorldID);
			m_nItemID		= DBTestHelper::InsertItemInfo(1);
			m_nSlotType		= SLOTTYPE_INVENTORY;
			m_nSlotID		= 1;
			m_nStackAmt		= 5;
			m_nMaxDura		= 10;
			m_nColor		= 2;
			m_bClaimed		= false;
			m_nCharPtm		= 10;
			m_bPeriodItem	= false;
			m_strExpiDt		= L"2000-1-1 9:21:1";
			m_nModMoney		= 10;			
		}

		int64 USP_RZ_ITEM_BUY_INSERT(const int nUsagePeriod, const uint8 nTier, const int nXP, const int nMoney)
		{
			const wstring strExpiDt = wstring(L"'") + m_strExpiDt + wstring(L"'");

			TDBRecordSet rs;
			CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_ITEM_BUY_INSERT (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %s, %d, %d, %d)}"
				, GConfig::m_nMyWorldID
				, m_nAID, m_nCID, m_nSlotType, m_nSlotID, m_nItemID, m_nStackAmt, m_nMaxDura, m_nMaxDura, m_nColor, m_bClaimed, m_bPeriodItem
				, nUsagePeriod, strExpiDt.c_str(), m_nCharPtm, nXP, nMoney));
			const int64 nIUID = rs.Field(L"IUID").AsInt64();
			CHECK(0 < nIUID);
			return nIUID;
		}

		void CHECK_BUY_ITEM_ATTRIBUTE(const int64 nIUID, const int16 nStackAmt, const int nUsagePeriod)
		{
			TDBRecordSet rs;
			CHECK(UTestDB.Execute(rs, L"SELECT SLOT_TYPE, SLOT_ID, ITEM_ID, STACK_AMT, DURA, MAX_DURA, COLOR, CLAIMED, PERIOD, USAGE_PERIOD, EFF_END_DATE, CHAR_PTM\
									  FROM dbo.RZ_INVEN \
									  WHERE OWNER_ID = %I64d AND IUID = %I64d;", m_nCID, nIUID));

			CHECK_EQUAL(m_nSlotType, rs.Field(L"SLOT_TYPE").AsByte());
			CHECK_EQUAL(m_nSlotID, rs.Field(L"SLOT_ID").AsShort());
			CHECK_EQUAL(m_nItemID, rs.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(nStackAmt, rs.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(m_nMaxDura, rs.Field(L"DURA").AsByte());
			CHECK_EQUAL(m_nMaxDura, rs.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(m_nColor, rs.Field(L"COLOR").AsInt());
			CHECK_EQUAL(m_bClaimed, rs.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(m_bPeriodItem, rs.Field(L"PERIOD").AsBool());
			CHECK(m_strExpiDt ==  rs.Field(L"EFF_END_DATE").AsWString());
			CHECK_EQUAL(m_nCharPtm, rs.Field(L"CHAR_PTM").AsInt());

			rs.Close();
		}

		void CHECK_BUY_CHARACTER_XP_AND_MONEY(const int nMoney)
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT [MONEY] FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", m_nCID);
			CHECK_EQUAL(nMoney, rs.Field(L"MONEY").AsInt());
			rs.Close();
		}		

	public :
		int64	m_nAID;
		int64	m_nCID;
		int		m_nItemID;
		uint8	m_nSlotType; 
		int16	m_nSlotID;
		uint8	m_nMaxDura;
		uint8	m_nStackAmt;		
		int		m_nColor;
		bool	m_bClaimed;
		int		m_nCharPtm;		
		bool	m_bPeriodItem;
		wstring	m_strExpiDt;
		int		m_nModMoney;
	};

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_INSERT_NON_PERIOD)
	{
		const int nUsagePeriod	= 0;
		const int nXP			= 1020;
		const int nMoney		= 90;
		
		DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nSlotType, m_nSlotID);

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_VERY_COMMON, nXP, nMoney);
		
		CHECK_BUY_ITEM_ATTRIBUTE(nIUID, m_nStackAmt, nUsagePeriod);
		CHECK_BUY_CHARACTER_XP_AND_MONEY(nMoney);		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_INSERT_NON_PERIOD_CREATE)
	{
		const int nUsagePeriod	= 0;
		const int nXP			= 1020;
		const int nMoney		= 90;

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_VERY_COMMON, nXP, nMoney);

		CHECK_BUY_ITEM_ATTRIBUTE(nIUID, m_nStackAmt, nUsagePeriod);
		CHECK_BUY_CHARACTER_XP_AND_MONEY(nMoney);		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_INSERT_USAGE_PERIOD)
	{
		const int nUsagePeriod	= 5;
		const int nXP			= 1020;
		const int nMoney		= 90;

		DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nSlotType, m_nSlotID);

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_VERY_COMMON, nXP, nMoney);

		CHECK_BUY_ITEM_ATTRIBUTE(nIUID, m_nStackAmt, nUsagePeriod);
		CHECK_BUY_CHARACTER_XP_AND_MONEY(nMoney);		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_INSERT_USAGE_PERIOD_CREATE)
	{
		const int nUsagePeriod	= 5;
		const int nXP			= 1020;
		const int nMoney		= 90;

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_VERY_COMMON, nXP, nMoney);

		CHECK_BUY_ITEM_ATTRIBUTE(nIUID, m_nStackAmt, nUsagePeriod);
		CHECK_BUY_CHARACTER_XP_AND_MONEY(nMoney);		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_INSERT_EXPIDT)
	{
		const int nUsagePeriod	= 0;
		const int nXP			= 1020;
		const int nMoney		= 90;

		DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nSlotType, m_nSlotID);

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_VERY_COMMON, nXP, nMoney);

		CHECK_BUY_ITEM_ATTRIBUTE(nIUID, m_nStackAmt, nUsagePeriod);
		CHECK_BUY_CHARACTER_XP_AND_MONEY(nMoney);		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_INSERT_EXPIDT_CREATE)
	{
		const int nUsagePeriod	= 0;
		const int nXP			= 1020;
		const int nMoney		= 90;

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_VERY_COMMON, nXP, nMoney);

		CHECK_BUY_ITEM_ATTRIBUTE(nIUID, m_nStackAmt, nUsagePeriod);
		CHECK_BUY_CHARACTER_XP_AND_MONEY(nMoney);		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_INSERT_USAGE_PERIOD_AND_EXPIDT)
	{
		const int nUsagePeriod	= 5;
		const int nXP			= 1020;
		const int nMoney		= 90;

		DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nSlotType, m_nSlotID);

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_VERY_COMMON, nXP, nMoney);

		CHECK_BUY_ITEM_ATTRIBUTE(nIUID, m_nStackAmt, nUsagePeriod);
		CHECK_BUY_CHARACTER_XP_AND_MONEY(nMoney);		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_INSERT_USAGE_PERIOD_AND_EXPIDT_CREATE)
	{
		const int nUsagePeriod	= 5;
		const int nXP			= 1020;
		const int nMoney		= 90;

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_VERY_COMMON, nXP, nMoney);

		CHECK_BUY_ITEM_ATTRIBUTE(nIUID, m_nStackAmt, nUsagePeriod);
		CHECK_BUY_CHARACTER_XP_AND_MONEY(nMoney);		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_UPDATE)
	{
		const int	nUsagePeriod	= 0;
		const int	nXP				= 1020;
		const int	nMoney			= 80;
		const int16 nModStackAmt	= 5;		
		const int	nCharPtm		= m_nCharPtm + 10;

		DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nSlotType, m_nSlotID);

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_VERY_COMMON, nXP, nMoney);

		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET DURA = 0, MAX_DURA = 0, PERIOD = 0, USAGE_PERIOD = 0, EFF_END_DATE = 0, CLAIMED = 0 WHERE IUID =%I64d", nIUID);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_BUY_UPDATE (%d, %I64d, %I64d, %d, %d, %I64d, %d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, m_nAID, m_nCID, m_nSlotType, m_nSlotID, nIUID, m_nStackAmt + nModStackAmt, nCharPtm, nXP, nMoney));

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT STACK_AMT FROM dbo.RZ_INVEN WHERE IUID = %I64d;", nIUID);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(m_nStackAmt + nModStackAmt, rs.Field(L"STACK_AMT").AsShort());
		rs.Close();

		CHECK_BUY_CHARACTER_XP_AND_MONEY(nMoney);		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_VERY_COMMON_CHECK_LOG)
	{
		const int nUsagePeriod	= 5;
		const int nXP			= 1020;
		const int nMoney		= 90;

		DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nSlotType, m_nSlotID);

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_VERY_COMMON, nXP, nMoney);		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_COMMON_CHECK_LOG)
	{
		const int nUsagePeriod	= 5;
		const int nXP			= 1020;
		const int nMoney		= 90;

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_COMMON, nXP, nMoney);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_TREASURE_CHECK_LOG)
	{
		const int nUsagePeriod	= 5;
		const int nXP			= 1020;
		const int nMoney		= 90;

		DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nSlotType, m_nSlotID);

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_TREASURE, nXP,nMoney);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_LEGENDARY_CHECK_LOG)
	{
		const int nUsagePeriod	= 5;
		const int nXP			= 1020;
		const int nMoney		= 90;

		DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nSlotType, m_nSlotID);

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_LEGENDARY, nXP, nMoney);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_ITEM_BUY_EPIC_CHECK_LOG)
	{
		const int nUsagePeriod	= 5;
		const int nAddExpiMin	= 10;
		const int nXP			= 1020;
		const int nMoney		= 90;

		DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nSlotType, m_nSlotID);

		const int64 nIUID = USP_RZ_ITEM_BUY_INSERT(nUsagePeriod, ITEM_TIER_EPIC, nXP,nMoney);
	}
}