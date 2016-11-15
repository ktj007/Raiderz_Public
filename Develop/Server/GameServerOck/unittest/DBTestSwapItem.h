#ifndef _TEST_SWAP_ITEM_H
#define _TEST_SWAP_ITEM_H

#include "Gconfig.h"

class TEST_SWAP_ITEM
{
public :
	int64		m_nAID;
	int64		m_nCID;
	uint8		m_nSlotType;
	int16		m_nSlotID;
	IUID		m_nIUID;
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
	int			m_nEnchBufID_1;
	int			m_nEnchBufID_2;
	int			m_nEnchBufID_3;
	int			m_nEnchBufID_4;
	int			m_nEnchBufID_5;

	void SetTestData(const int64 nCID, const uint8 nSlotType, const int16 nSlotID)
	{
		m_nAID			= 1;
		m_nCID			= nCID;
		m_nSlotType		= nSlotType;
		m_nSlotID		= nSlotID;
		m_nItemID		= 4;
		m_nStackAmt		= 5;
		m_nSoulCnt		= 6;
		m_nDura			= 7;
		m_nMaxDura		= 8;
		m_nEnchCnt		= 9;
		m_nColor		= 10;
		m_bClaimed		= true;
		m_nCharPtm		= 11;
		m_bPeriodItem	= true;
		m_nUsagePeriod	= 12;
		m_strExpiDt		= L"2010-12-31 0:0:0";
		m_nEnchBufID_1	= 1;
		m_nEnchBufID_2	= 2;
		m_nEnchBufID_3	= 3;
		m_nEnchBufID_4	= 4;
		m_nEnchBufID_5	= 5;

		DBTestHelper::InsertItemInfo(m_nItemID);
		m_nIUID = DBTestHelper::InsertItem(m_nAID, nCID, GConfig::m_nMyWorldID, m_nItemID, m_nStackAmt, nSlotType, nSlotID);

		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET ITEM_ID = 4, STACK_AMT = 5, SOUL_COUNT = 6, DURA = 7, MAX_DURA = 8, COLOR = 10, CLAIMED = 1\
		, CHAR_PTM = 11, PERIOD = 1, USAGE_PERIOD = 12, EFF_END_DATE = '2010-12-31 0:0:0' \
		, ENCH_ITEM_ID_1 = 1, ENCH_ITEM_ID_2 = 2, ENCH_ITEM_ID_3 = 3, ENCH_ITEM_ID_4 = 4, ENCH_ITEM_ID_5 = 5 WHERE IUID = %I64d;", m_nIUID);
	}

	void SetTestToData(const int64 nCID, const uint8 nSlotType, const int16 nSlotID)
	{
		m_nAID			= 1;
		m_nCID			= nCID;
		m_nSlotType		= nSlotType;
		m_nSlotID		= nSlotID;
		m_nItemID		= 14;
		m_nStackAmt		= 15;
		m_nSoulCnt		= 16;
		m_nDura			= 17;
		m_nMaxDura		= 18;
		m_nEnchCnt		= 19;
		m_nColor		= 20;
		m_bClaimed		= true;
		m_nCharPtm		= 21;
		m_bPeriodItem	= true;
		m_nUsagePeriod	= 22;
		m_strExpiDt		= L"2010-1-1 0:0:0";
		m_nEnchBufID_1	= 11;
		m_nEnchBufID_2	= 12;
		m_nEnchBufID_3	= 13;
		m_nEnchBufID_4	= 14;
		m_nEnchBufID_5	= 15;

		DBTestHelper::InsertItemInfo(m_nItemID);
		m_nIUID = DBTestHelper::InsertItem(m_nAID, nCID, GConfig::m_nMyWorldID, m_nItemID, m_nStackAmt, nSlotType, nSlotID);

		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET ITEM_ID = 14, STACK_AMT = 15, SOUL_COUNT = 16, DURA = 17, MAX_DURA = 18, COLOR = 20, CLAIMED = 1\
						 , CHAR_PTM = 21, PERIOD = 1, USAGE_PERIOD = 22, EFF_END_DATE = '2010-1-1 0:0:0' \
						 , ENCH_ITEM_ID_1 = 11, ENCH_ITEM_ID_2 = 12, ENCH_ITEM_ID_3 = 13, ENCH_ITEM_ID_4 = 14, ENCH_ITEM_ID_5 = 15 WHERE IUID = %I64d;", m_nIUID);
	}

	void SetToEmpty(const int64 nCID, const uint8 nSlotType, const int16 nSlotID, bool bCreateEmptySlot = true)
	{
		m_nAID			= 1;
		m_nCID			= nCID;
		m_nSlotType		= nSlotType;
		m_nSlotID		= nSlotID;
		m_nItemID		= 0;
		m_nStackAmt		= 0;
		m_nSoulCnt		= 0;
		m_nDura			= 0;
		m_nMaxDura		= 0;
		m_nEnchCnt		= 0;
		m_nColor		= 0;
		m_bClaimed		= false;
		m_nCharPtm		= 0;
		m_bPeriodItem	= false;
		m_nUsagePeriod	= 0;
		m_nEnchBufID_1	= 0;
		m_nEnchBufID_2	= 0;
		m_nEnchBufID_3	= 0;
		m_nEnchBufID_4	= 0;
		m_nEnchBufID_5	= 0;

		if (bCreateEmptySlot)
			DBTestHelper::InsertEmptyItemSlot(m_nAID, nCID, GConfig::m_nMyWorldID, nSlotType, nSlotID);

		m_nIUID = 0;
	}

	void CHECK_MOVE_ITEM(const int64 nCID, const uint8 nSlotType, const int16 nSlotID, int nStackAmt)
	{
		TDBRecordSet rsItem;
		UTestDB.Execute(rsItem, L"SELECT IUID, ITEM_ID, STACK_AMT, SOUL_COUNT, DURA, MAX_DURA, COLOR, CLAIMED, CHAR_PTM, PERIOD, USAGE_PERIOD, EFF_END_DATE\
								 , ENCH_ITEM_ID_1, ENCH_ITEM_ID_2, ENCH_ITEM_ID_3, ENCH_ITEM_ID_4, ENCH_ITEM_ID_5  \
								 FROM dbo.RZ_INVEN \
								 WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d;"
								 , nCID, nSlotType, nSlotID);

		CHECK_EQUAL(1, rsItem.GetFetchedRowCount());
		CHECK_EQUAL(m_nIUID, rsItem.Field(L"IUID").AsInt64());
		CHECK_EQUAL(nStackAmt, rsItem.Field(L"STACK_AMT").AsShort());
		CHECK_EQUAL(m_nSoulCnt, rsItem.Field(L"SOUL_COUNT").AsByte());
		CHECK_EQUAL(m_nDura, rsItem.Field(L"DURA").AsByte());
		CHECK_EQUAL(m_nMaxDura, rsItem.Field(L"MAX_DURA").AsByte());
		CHECK_EQUAL(m_nColor, rsItem.Field(L"COLOR").AsInt());
		CHECK_EQUAL(m_bClaimed, rsItem.Field(L"CLAIMED").AsBool());
		CHECK_EQUAL(m_nCharPtm, rsItem.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(m_bPeriodItem, rsItem.Field(L"PERIOD").AsBool());
		CHECK_EQUAL(m_nUsagePeriod, rsItem.Field(L"USAGE_PERIOD").AsInt());
		if (m_nIUID > 0 ) CHECK(m_strExpiDt == rsItem.Field(L"EFF_END_DATE").AsWString());
		CHECK_EQUAL(m_nEnchBufID_1, rsItem.Field(L"ENCH_ITEM_ID_1").AsInt());
		CHECK_EQUAL(m_nEnchBufID_2, rsItem.Field(L"ENCH_ITEM_ID_2").AsInt());
		CHECK_EQUAL(m_nEnchBufID_3, rsItem.Field(L"ENCH_ITEM_ID_3").AsInt());
		CHECK_EQUAL(m_nEnchBufID_4, rsItem.Field(L"ENCH_ITEM_ID_4").AsInt());
		CHECK_EQUAL(m_nEnchBufID_5, rsItem.Field(L"ENCH_ITEM_ID_5").AsInt());
		rsItem.Close();
	}

	
};


#endif