#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"


SUITE(DBTest_ItemInsert)
{
	class FIXTURE
	{
	public :
		FIXTURE()
		{
			m_nAID			= 1;
			m_nCID			= DBTestHelper::InsertCharacter(m_nAID, GConfig::m_nMyWorldID);
			m_nItemID		= DBTestHelper::InsertItemInfo(1);
			m_nSlotType		= SLOTTYPE_INVENTORY;
			m_nSlotID		= 1;
			m_nStackAmt		= 21;
			m_nMaxDura		= 10;
			m_nColor		= 2;
			m_bClaimed		= false;
			m_nCharPtm		= 10;
			m_bPeriodItem	= true;
			m_strExpiDt		= L"2011-1-11 3:12:32";

			DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nSlotType, m_nSlotID);
		}

		int64 USP_RZ_ITEM_INSERT(const int nUsagePeriod)
		{
			const wstring strExpiDt = wstring(L"'") + m_strExpiDt + wstring(L"'");

			TDBRecordSet rs;
			CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_ITEM_INSERT (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %s, %d)}"
				, GConfig::m_nMyWorldID, m_nAID, m_nCID, m_nSlotType, m_nSlotID, m_nItemID, m_nStackAmt, m_nMaxDura, m_nMaxDura, m_nColor, m_bClaimed, m_bPeriodItem, nUsagePeriod, strExpiDt.c_str(), m_nCharPtm));
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			int64 nIUID = rs.Field(L"IUID").AsInt64();
			CHECK(0 < nIUID);
			return nIUID;
		}

		void CHECK_INSERTED_ITEM_ATTRIBUTE(const int nUsagePeriod)
		{
			TDBRecordSet rs;
			CHECK(UTestDB.Execute(rs, L"SELECT SLOT_TYPE, SLOT_ID, ITEM_ID, STACK_AMT, DURA, MAX_DURA, COLOR, CLAIMED, PERIOD, USAGE_PERIOD, EFF_END_DATE, CHAR_PTM\
								FROM dbo.RZ_INVEN \
								WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d;", m_nCID, m_nSlotType, m_nSlotID));
		
			CHECK_EQUAL(m_nSlotType, rs.Field(L"SLOT_TYPE").AsByte());
			CHECK_EQUAL(m_nSlotID, rs.Field(L"SLOT_ID").AsShort());
			CHECK_EQUAL(m_nItemID, rs.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(m_nStackAmt, rs.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(m_nMaxDura, rs.Field(L"DURA").AsByte());
			CHECK_EQUAL(m_nMaxDura, rs.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(m_nColor, rs.Field(L"COLOR").AsInt());
			CHECK_EQUAL(m_bClaimed, rs.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(m_bPeriodItem, rs.Field(L"PERIOD").AsBool());
			CHECK(m_strExpiDt == rs.Field(L"EFF_END_DATE").AsWString());
			CHECK_EQUAL(m_nCharPtm, rs.Field(L"CHAR_PTM").AsInt());

			rs.Close();
		}
		
	public :
		int64	m_nAID;
		int64	m_nCID;		
		int		m_nItemID;
		uint8	m_nMaxDura;
		int16	m_nStackAmt;
		uint8	m_nSlotType; 
		int16	m_nSlotID;
		int		m_nColor;
		bool	m_bClaimed;
		int		m_nCharPtm;		
		bool	m_bPeriodItem;
		wstring m_strExpiDt;
	};

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_INSERT_NON_PERIOD_ITEM)
	{
		const int	nUsagePeriod	= 0;

		const int64 nIUID = USP_RZ_ITEM_INSERT(nUsagePeriod);
		CHECK_INSERTED_ITEM_ATTRIBUTE(nUsagePeriod);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_INSERT_USEAGE_PERIOD)
	{
		const int	nUsagePeriod	= 10;

		const int64 nIUID = USP_RZ_ITEM_INSERT(nUsagePeriod);
		CHECK_INSERTED_ITEM_ATTRIBUTE(nUsagePeriod);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_INSERT_EXPIDT)
	{
		const int	nUsagePeriod	= 0;
		const int	nAddExpiMin		= 5;
		
		const int64 nIUID = USP_RZ_ITEM_INSERT(nUsagePeriod);
		CHECK_INSERTED_ITEM_ATTRIBUTE(nUsagePeriod);
	}
}