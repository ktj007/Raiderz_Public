#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"


SUITE(DBTestItemEquipment)
{
	const wstring strSQL = L"{CALL dbo.USP_RZ_ITEM_UPDATE_EQUIPMENT (%d, %I64d, %d, %d, %I64d, %d, %d, %d, %d, %I64d, %d, %d, %d, %I64d, %d, %d)}";

	class EQUIPMENT_TEST_ITEM
	{
	public :
		EQUIPMENT_TEST_ITEM() : m_nCID(0), m_nSlotType(-1), m_nSlotID(-1), m_nIUID(0), m_nItemID(0)
			, m_nStackAmt(0), m_nSoulCnt(0), m_nDura(0), m_nMaxDura(0)
			, m_nEnchCnt(0), m_nColor(0), m_bClaimed(0), m_nCharPtm(0), m_bPeriodItem(0)
			, m_nUsagePeriod(0) {}

		EQUIPMENT_TEST_ITEM(const int64 nCID, const uint8 nSlotType, const int16 nSlotID, const IUID nIUID, const int nItemID
			, const int16 nStackAmt, const uint8 nSoulCnt, const uint8 nDura, const uint8 nMaxDura, const uint8 nEnchCnt
			, const int nColor, const bool bClaimed, const int nCharPtm, const bool bPeriodItem, const int nUsagePeriod, const wstring& strExpiDt)
			: m_nCID(nCID), m_nSlotType(nSlotType), m_nSlotID(nSlotID), m_nIUID(nIUID), m_nItemID(nItemID)
			, m_nStackAmt(nStackAmt), m_nSoulCnt(nSoulCnt), m_nDura(nDura), m_nMaxDura(nMaxDura)
			, m_nEnchCnt(nEnchCnt), m_nColor(nColor), m_bClaimed(bClaimed), m_nCharPtm(nCharPtm), m_bPeriodItem(bPeriodItem), m_strExpiDt(strExpiDt)
			, m_nUsagePeriod(nUsagePeriod)
		{

		}

	public :
		int64	m_nAID;
		int64	m_nCID;
		uint8	m_nSlotType;
		int16	m_nSlotID;
		IUID	m_nIUID;
		int		m_nItemID;
		int16	m_nStackAmt;
		uint8	m_nSoulCnt;
		uint8	m_nDura;
		uint8	m_nMaxDura;
		uint8	m_nEnchCnt;
		int		m_nColor;
		bool	m_bClaimed;
		int		m_nCharPtm;
		bool	m_bPeriodItem;
		int		m_nUsagePeriod;
		wstring	m_strExpiDt;
	};

	class FIXTURE
	{
	public :
		FIXTURE()
		{
			m_nAID		= 1;
			m_nCID		= DBTestHelper::InsertCharacter(m_nAID, GConfig::m_nMyWorldID);			
			m_InvenItem = CreateItem(m_nCID, 1, 0, 1, false, L"NULL");
		}

		void SetTestAllEquipment(const int64 nCID)
		{
			m_REquipItem = CreateItem(nCID, 2, 0, 2, true, L"'2010-1-1 1:1:0'");
			m_LEquipItem = CreateItem(nCID, 2, 1, 3, true, L"'2010-1-2 10:10:0'");
		}

		void SetTestRightEquipment(const int64 nCID)
		{
			m_REquipItem = CreateItem(nCID, 2, 0, 2, true, L"'2010-1-1 1:1:0'");
		}

		void SetTestLeftEquipment(const int64 nCID)
		{
			m_LEquipItem = CreateItem(nCID, 2, 1, 3, true, L"'2010-1-2 10:10:0'");
		}

		EQUIPMENT_TEST_ITEM CreateItem(const int64 nCID, const int nSlotType, const int nSlotID, const int nItemID, const bool bClaimed, const wstring& strExpiDt)
		{
			TDBRecordSet rsItem;
			UTestDB.Execute(rsItem, L"SELECT ITEM_ID FROM dbo.RZ_ITEM_INFO WHERE ITEM_ID = %d;", nItemID);
			if (0 == rsItem.GetFetchedRowCount())
				DBTestHelper::InsertItemInfo(nItemID);
			rsItem.Close();

			EQUIPMENT_TEST_ITEM ei(nCID, nSlotType, nSlotID, 0, nItemID, 1, nSlotID + 1, nSlotID + 2, nSlotID + 3, nSlotID + 4, nSlotID + 5
				, bClaimed, nSlotID + 6, true, nSlotID + 7, strExpiDt);

			ei.m_nIUID = DBTestHelper::InsertItem(m_nAID, ei.m_nCID, GConfig::m_nMyWorldID, ei.m_nItemID, ei.m_nStackAmt, ei.m_nSlotType, ei.m_nSlotID, ei.m_bClaimed, ei.m_nUsagePeriod);

			UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET SOUL_COUNT = %d, DURA = %d, MAX_DURA = %d, COLOR = %d, CHAR_PTM = %d, EFF_END_DATE = %s WHERE IUID = %I64d;"
				, ei.m_nSoulCnt, ei.m_nDura, ei.m_nMaxDura, ei.m_nColor, ei.m_nCharPtm, ei.m_strExpiDt.c_str(), ei.m_nIUID);

			return ei;
		}

		void CHECK_ITEM(const EQUIPMENT_TEST_ITEM& item, const uint8 nSlotTypeTo, const int16 nSlotIDTo, const bool bClaimed)
		{
			TDBRecordSet rsItem;
			UTestDB.Execute(rsItem, L"SELECT SLOT_TYPE, SLOT_ID, ITEM_ID, STACK_AMT, SOUL_COUNT, DURA, MAX_DURA, COLOR, CLAIMED, CHAR_PTM \
								   , PERIOD, USAGE_PERIOD, EFF_END_DATE FROM dbo.RZ_INVEN WHERE IUID = %I64d AND OWNER_ID = %I64d;"
								   , item.m_nIUID, item.m_nCID);
			CHECK_EQUAL(1, rsItem.GetFetchedRowCount());
			CHECK_EQUAL(nSlotTypeTo, rsItem.Field(L"SLOT_TYPE").AsByte());
			CHECK_EQUAL(nSlotIDTo, rsItem.Field(L"SLOT_ID").AsShort());
			CHECK_EQUAL(item.m_nItemID, rsItem.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(item.m_nStackAmt, rsItem.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(item.m_nSoulCnt, rsItem.Field(L"SOUL_COUNT").AsByte());
			CHECK_EQUAL(item.m_nDura, rsItem.Field(L"DURA").AsByte());
			CHECK_EQUAL(item.m_nMaxDura, rsItem.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(item.m_nColor, rsItem.Field(L"COLOR").AsInt());
			CHECK_EQUAL(bClaimed, rsItem.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(item.m_nCharPtm, rsItem.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(item.m_bPeriodItem, rsItem.Field(L"PERIOD").AsBool());
			CHECK_EQUAL(item.m_nUsagePeriod, rsItem.Field(L"USAGE_PERIOD").AsInt());

			wstring strExpiDt = rsItem.Field(L"EFF_END_DATE").AsWString();
			if (L"NULL" != rsItem.Field(L"EFF_END_DATE").AsWString())
				strExpiDt = wstring(L"'") + rsItem.Field(L"EFF_END_DATE").AsWString() + wstring(L"'");
			CHECK(item.m_strExpiDt == strExpiDt);

			rsItem.Close();
		}

		void CHECK_EMPTY_SLOT(const int64 nCID, const uint8 nSlotType, const int16 nSlotID)
		{
			TDBRecordSet rsEmpty;
			UTestDB.Execute(rsEmpty, L"SELECT IUID, ITEM_ID, STACK_AMT FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d;"
				, nCID, nSlotType, nSlotID);

			CHECK_EQUAL(1, rsEmpty.GetFetchedRowCount());
			CHECK_EQUAL(0, rsEmpty.Field(L"IUID").AsInt64());
			CHECK(rsEmpty.Field(L"ITEM_ID").IsNull());
			CHECK_EQUAL(0, rsEmpty.Field(L"STACK_AMT").AsShort());
			rsEmpty.Close();
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDALL_EQUIPRIGHTSLOT(bool bCreateEmptySlot)
		{
			SetTestAllEquipment(m_nCID);

			const int16 nUneqSlotIDTo1	= 10;
			const int16 nUneqSlotIDTo2	= 11;		
			const bool	bClaimed		= true;

			if (bCreateEmptySlot)
			{
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo1);
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo2);
			}		

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID, m_InvenItem.m_nIUID, m_InvenItem.m_nDura, bClaimed, m_REquipItem.m_nSlotID
				, m_REquipItem.m_nSlotID, m_REquipItem.m_nIUID, m_REquipItem.m_nDura, nUneqSlotIDTo1
				, m_LEquipItem.m_nSlotID, m_LEquipItem.m_nIUID, m_LEquipItem.m_nDura, nUneqSlotIDTo2);

			CHECK_ITEM(m_InvenItem, SLOTTYPE_EQUIP, m_REquipItem.m_nSlotID, bClaimed);
			CHECK_ITEM(m_REquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo1, m_REquipItem.m_bClaimed);
			CHECK_ITEM(m_LEquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo2, m_LEquipItem.m_bClaimed);

			if (bCreateEmptySlot)
			{
				CHECK_EMPTY_SLOT(m_nCID, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID);
				CHECK_EMPTY_SLOT(m_nCID, m_LEquipItem.m_nSlotType, m_LEquipItem.m_nSlotID);
			}			
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDALL_EQUIPRIGHTSLOT_SWAPPOSITION(bool bCreateEmptySlot)
		{
			SetTestAllEquipment(m_nCID);

			const int16 nUneqSlotIDTo	= 10;
			const bool	bClaimed		= true;

			if (bCreateEmptySlot)
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo);

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID, m_InvenItem.m_nIUID, m_InvenItem.m_nDura, bClaimed, m_REquipItem.m_nSlotID
				, m_REquipItem.m_nSlotID, m_REquipItem.m_nIUID, m_REquipItem.m_nDura, m_InvenItem.m_nSlotID
				, m_LEquipItem.m_nSlotID, m_LEquipItem.m_nIUID, m_LEquipItem.m_nDura, nUneqSlotIDTo);

			CHECK_ITEM(m_InvenItem, SLOTTYPE_EQUIP, m_REquipItem.m_nSlotID, bClaimed);
			CHECK_ITEM(m_REquipItem, SLOTTYPE_INVENTORY, m_InvenItem.m_nSlotID, m_REquipItem.m_bClaimed);
			CHECK_ITEM(m_LEquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo, m_LEquipItem.m_bClaimed);

			if (bCreateEmptySlot)
				CHECK_EMPTY_SLOT(m_nCID, m_LEquipItem.m_nSlotType, m_LEquipItem.m_nSlotID);
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDALL_EQUIPLEFTSLOT(bool bCreateEmptySlot)
		{
			SetTestAllEquipment(m_nCID);

			const int16 nUneqSlotIDTo1	= 10;
			const int16 nUneqSlotIDTo2	= 11;		
			const bool	bClaimed		= true;

			if (bCreateEmptySlot)
			{
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo1);
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo2);
			}		

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID, m_InvenItem.m_nIUID, m_InvenItem.m_nDura, bClaimed, m_LEquipItem.m_nSlotID
				, m_REquipItem.m_nSlotID, m_REquipItem.m_nIUID, m_REquipItem.m_nDura, nUneqSlotIDTo1
				, m_LEquipItem.m_nSlotID, m_LEquipItem.m_nIUID, m_LEquipItem.m_nDura, nUneqSlotIDTo2);

			CHECK_ITEM(m_InvenItem, SLOTTYPE_EQUIP, m_LEquipItem.m_nSlotID, bClaimed);
			CHECK_ITEM(m_REquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo1, m_REquipItem.m_bClaimed);
			CHECK_ITEM(m_LEquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo2, m_LEquipItem.m_bClaimed);

			if (bCreateEmptySlot)
			{
				CHECK_EMPTY_SLOT(m_nCID, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID);
				CHECK_EMPTY_SLOT(m_nCID, m_REquipItem.m_nSlotType, m_REquipItem.m_nSlotID);
			}			
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDALL_EQUIPEMPTYSLOT(bool bCreateEmptySlot)
		{
			SetTestAllEquipment(m_nCID);

			const int16 nUneqSlotIDTo1	= 10;
			const int16 nUneqSlotIDTo2	= 11;	
			const int16 nEmptySlotID	= 20;
			const bool	bClaimed		= true;

			if (bCreateEmptySlot)
			{
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo1);
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo2);
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_EQUIP, nEmptySlotID);
			}		

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID, m_InvenItem.m_nIUID, m_InvenItem.m_nDura, bClaimed, nEmptySlotID
				, m_REquipItem.m_nSlotID, m_REquipItem.m_nIUID, m_REquipItem.m_nDura, nUneqSlotIDTo1
				, m_LEquipItem.m_nSlotID, m_LEquipItem.m_nIUID, m_LEquipItem.m_nDura, nUneqSlotIDTo2);

			CHECK_ITEM(m_InvenItem, SLOTTYPE_EQUIP, nEmptySlotID, bClaimed);
			CHECK_ITEM(m_REquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo1, m_REquipItem.m_bClaimed);
			CHECK_ITEM(m_LEquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo2, m_LEquipItem.m_bClaimed);

			if (bCreateEmptySlot)
			{
				CHECK_EMPTY_SLOT(m_nCID, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID);
				CHECK_EMPTY_SLOT(m_nCID, m_REquipItem.m_nSlotType, m_REquipItem.m_nSlotID);
				CHECK_EMPTY_SLOT(m_nCID, m_LEquipItem.m_nSlotType, m_LEquipItem.m_nSlotID);
			}			
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDRIGHT_EQUIPRIGHTSLOT(bool bCreateEmptySlot)
		{
			SetTestRightEquipment(m_nCID);

			const int16 nUneqSlotIDTo	= 10;
			const bool	bClaimed		= true;

			if (bCreateEmptySlot)
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo);

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID, m_InvenItem.m_nIUID, m_InvenItem.m_nDura, bClaimed, m_REquipItem.m_nSlotID
				, m_REquipItem.m_nSlotID, m_REquipItem.m_nIUID, m_REquipItem.m_nDura, nUneqSlotIDTo
				, -1, (int64)0, 0, -1);

			CHECK_ITEM(m_InvenItem, SLOTTYPE_EQUIP, m_REquipItem.m_nSlotID, bClaimed);
			CHECK_ITEM(m_REquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo, m_REquipItem.m_bClaimed);	

			if (bCreateEmptySlot)
				CHECK_EMPTY_SLOT(m_nCID, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID);
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDRIGHT_EQUIPEMPTYSLOT(bool bCreateEmptySlot)
		{
			SetTestRightEquipment(m_nCID);

			const int16 nUneqSlotIDTo	= 10;
			const int16 nEmptySlotID	= 20;
			const bool	bClaimed		= true;

			if (bCreateEmptySlot)
			{
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo);
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_EQUIP, nEmptySlotID);
			}

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID, m_InvenItem.m_nIUID, m_InvenItem.m_nDura, bClaimed, nEmptySlotID
				, m_REquipItem.m_nSlotID, m_REquipItem.m_nIUID, m_REquipItem.m_nDura, nUneqSlotIDTo
				, -1, (int64)0, 0, -1);

			CHECK_ITEM(m_InvenItem, SLOTTYPE_EQUIP, nEmptySlotID, bClaimed);
			CHECK_ITEM(m_REquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo, m_REquipItem.m_bClaimed);

			if (bCreateEmptySlot)
			{
				CHECK_EMPTY_SLOT(m_nCID, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID);
				CHECK_EMPTY_SLOT(m_nCID, m_REquipItem.m_nSlotType, m_REquipItem.m_nSlotID);
			}			
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDLEFT_EQUIPLEFTSLOT(bool bCreateEmptySlot)
		{
			SetTestLeftEquipment(m_nCID);

			const int16 nUneqSlotIDTo	= 10;
			const bool	bClaimed		= true;

			if (bCreateEmptySlot)
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo);

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID, m_InvenItem.m_nIUID, m_InvenItem.m_nDura, bClaimed, m_LEquipItem.m_nSlotID
				, m_LEquipItem.m_nSlotID, m_LEquipItem.m_nIUID, m_LEquipItem.m_nDura, nUneqSlotIDTo
				, -1, (int64)0, 0, -1);

			CHECK_ITEM(m_InvenItem, SLOTTYPE_EQUIP, m_LEquipItem.m_nSlotID, bClaimed);
			CHECK_ITEM(m_LEquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo, m_LEquipItem.m_bClaimed);	

			if (bCreateEmptySlot)
				CHECK_EMPTY_SLOT(m_nCID, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID);		
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDLEFT_EQUIPEMPTYSLOT(bool bCrateEmptySlot)
		{
			SetTestLeftEquipment(m_nCID);

			const int16 nUneqSlotIDTo	= 10;
			const int16 nEmptySlotID	= 20;
			const bool	bClaimed		= true;

			if (bCrateEmptySlot)
			{
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo);
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_EQUIP, nEmptySlotID);
			}		

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID, m_InvenItem.m_nIUID, m_InvenItem.m_nDura, bClaimed, nEmptySlotID
				, m_LEquipItem.m_nSlotID, m_LEquipItem.m_nIUID, m_LEquipItem.m_nDura, nUneqSlotIDTo
				, -1, (int64)0, 0, -1);

			CHECK_ITEM(m_InvenItem, SLOTTYPE_EQUIP, nEmptySlotID, bClaimed);
			CHECK_ITEM(m_LEquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo, m_LEquipItem.m_bClaimed);	

			if (bCrateEmptySlot)
			{
				CHECK_EMPTY_SLOT(m_nCID, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID);
				CHECK_EMPTY_SLOT(m_nCID, m_LEquipItem.m_nSlotType, m_LEquipItem.m_nSlotID);
			}			
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDALL_UNEQUIPALLSLOT(bool bCreateEmptySlot)
		{
			SetTestAllEquipment(m_nCID);

			int16 nUneqSlotIDTo1 = 10;
			int16 nUneqSlotIDTo2 = 11;

			if (bCreateEmptySlot)
			{
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo1);
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo2);
			}		

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, 0, -1, (int64)0, 0, false, -1
				, m_REquipItem.m_nSlotID, m_REquipItem.m_nIUID, m_REquipItem.m_nDura, nUneqSlotIDTo1
				, m_LEquipItem.m_nSlotID, m_LEquipItem.m_nIUID, m_LEquipItem.m_nDura, nUneqSlotIDTo2);

			CHECK_ITEM(m_InvenItem, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID, false);
			CHECK_ITEM(m_REquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo1, m_REquipItem.m_bClaimed);	
			CHECK_ITEM(m_LEquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo2, m_LEquipItem.m_bClaimed);	

			if (bCreateEmptySlot)
			{
				CHECK_EMPTY_SLOT(m_nCID, m_REquipItem.m_nSlotType, m_REquipItem.m_nSlotID);
				CHECK_EMPTY_SLOT(m_nCID, m_LEquipItem.m_nSlotType, m_LEquipItem.m_nSlotID);
			}			
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDALL_UNEQUIPRIGHTSLOT(bool bCreateEmptySlot)
		{
			SetTestAllEquipment(m_nCID);

			int16 nUneqSlotIDTo = 10;

			if (bCreateEmptySlot)
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo);

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, 0, -1, (int64)0, 0, false, -1
				, m_REquipItem.m_nSlotID, m_REquipItem.m_nIUID, m_REquipItem.m_nDura, nUneqSlotIDTo
				, -1, (int64)0, 0, -1);

			CHECK_ITEM(m_InvenItem, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID, false);
			CHECK_ITEM(m_REquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo, m_REquipItem.m_bClaimed);	
			CHECK_ITEM(m_LEquipItem, m_LEquipItem.m_nSlotType, m_LEquipItem.m_nSlotID, m_LEquipItem.m_bClaimed);	

			if (bCreateEmptySlot)
				CHECK_EMPTY_SLOT(m_nCID, m_REquipItem.m_nSlotType, m_REquipItem.m_nSlotID);		
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDALL_UNEQUIPLEFTSLOT(bool bCreateEmptySlot)
		{
			SetTestAllEquipment(m_nCID);

			int16 nUneqSlotIDTo = 10;

			if (bCreateEmptySlot)
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo);

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, 0, -1, (int64)0, 0, false, -1
				, m_LEquipItem.m_nSlotID, m_LEquipItem.m_nIUID, m_LEquipItem.m_nDura, nUneqSlotIDTo
				, -1, (int64)0, 0, -1);

			CHECK_ITEM(m_InvenItem, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID, false);
			CHECK_ITEM(m_REquipItem, m_REquipItem.m_nSlotType, m_REquipItem.m_nSlotID, m_REquipItem.m_bClaimed);	
			CHECK_ITEM(m_LEquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo, m_LEquipItem.m_bClaimed);

			if (bCreateEmptySlot)
				CHECK_EMPTY_SLOT(m_nCID, m_LEquipItem.m_nSlotType, m_LEquipItem.m_nSlotID);
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDRIGHT_UNEQUIPRIGHTSLOT(bool bCreateEmptySlot)
		{
			SetTestRightEquipment(m_nCID);

			int16 nUneqSlotIDTo = 10;

			if (bCreateEmptySlot)
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo);

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, 0, -1, (int64)0, 0, false, -1
				, m_REquipItem.m_nSlotID, m_REquipItem.m_nIUID, m_REquipItem.m_nDura, nUneqSlotIDTo
				, -1, (int64)0, 0, -1);

			CHECK_ITEM(m_InvenItem, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID, false);
			CHECK_ITEM(m_REquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo, m_REquipItem.m_bClaimed);	
			
			if (bCreateEmptySlot)
				CHECK_EMPTY_SLOT(m_nCID, m_REquipItem.m_nSlotType, m_REquipItem.m_nSlotID);		
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDLEFT_UNEQUIPLEFTSLOT(bool bCreateEmptySlot)
		{
			SetTestLeftEquipment(m_nCID);

			int16 nUneqSlotIDTo = 10;

			if (bCreateEmptySlot)
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo);

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, 0, -1, (int64)0, 0, false, -1
				, m_LEquipItem.m_nSlotID, m_LEquipItem.m_nIUID, m_LEquipItem.m_nDura, nUneqSlotIDTo
				, -1, (int64)0, 0, -1);

			CHECK_ITEM(m_InvenItem, m_InvenItem.m_nSlotType, m_InvenItem.m_nSlotID, false);
			CHECK_ITEM(m_LEquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo, m_LEquipItem.m_bClaimed);	

			if (bCreateEmptySlot)
				CHECK_EMPTY_SLOT(m_nCID, m_LEquipItem.m_nSlotType, m_LEquipItem.m_nSlotID);
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDALL_SECOND_RIGHT_EQUIPMENT_TO_PRIMARY_RIGHT_EQUIPMENT(bool bCreateEmptySlot)
		{
			SetTestAllEquipment(m_nCID);

			EQUIPMENT_TEST_ITEM m_SecREquipItem = CreateItem(m_nCID, 2, 2, 4, true, L"'2010-1-1 1:1:0'");
			EQUIPMENT_TEST_ITEM m_SecLEquipItem = CreateItem(m_nCID, 2, 3, 5, true, L"'2010-1-1 1:1:0'");

			int16 nSecToUneqSlotID = m_REquipItem.m_nSlotID;
			int16 nUneqSlotIDTo1 = 10;
			int16 nUneqSlotIDTo2 = 11;

			if (bCreateEmptySlot)
			{
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo1);
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo2);
			}		

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, m_SecREquipItem.m_nSlotType, m_SecREquipItem.m_nSlotID, m_SecREquipItem.m_nIUID, m_SecREquipItem.m_nDura, true, nSecToUneqSlotID
				, m_REquipItem.m_nSlotID, m_REquipItem.m_nIUID, m_REquipItem.m_nDura, nUneqSlotIDTo1
				, m_LEquipItem.m_nSlotID, m_LEquipItem.m_nIUID, m_LEquipItem.m_nDura, nUneqSlotIDTo2);

			CHECK_ITEM(m_SecREquipItem, SLOTTYPE_EQUIP, nSecToUneqSlotID, true);
			CHECK_ITEM(m_SecLEquipItem, SLOTTYPE_EQUIP, m_SecLEquipItem.m_nSlotID, true);
			CHECK_ITEM(m_REquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo1, true);	
			CHECK_ITEM(m_LEquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo2, true);	

			if (bCreateEmptySlot)
				CHECK_EMPTY_SLOT(m_nCID, m_SecREquipItem.m_nSlotType, m_SecREquipItem.m_nSlotID);
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDALL_SECOND_RIGHT_EQUIPMENT_TO_PRIMARY_LEFT_EQUIPMENT(bool bCrateEmptySlot)
		{
			SetTestAllEquipment(m_nCID);

			EQUIPMENT_TEST_ITEM m_SecREquipItem = CreateItem(m_nCID, 2, 2, 4, true, L"'2010-1-1 1:1:0'");
			EQUIPMENT_TEST_ITEM m_SecLEquipItem = CreateItem(m_nCID, 2, 3, 5, true, L"'2010-1-1 1:1:0'");

			int16 nSecToUneqSlotID = m_LEquipItem.m_nSlotID;
			int16 nUneqSlotIDTo1 = 10;
			int16 nUneqSlotIDTo2 = 11;

			if (bCrateEmptySlot)
			{
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo1);
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo2);
			}		

			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, m_SecREquipItem.m_nSlotType, m_SecREquipItem.m_nSlotID, m_SecREquipItem.m_nIUID, m_SecREquipItem.m_nDura, true, nSecToUneqSlotID
				, m_REquipItem.m_nSlotID, m_REquipItem.m_nIUID, m_REquipItem.m_nDura, nUneqSlotIDTo1
				, m_LEquipItem.m_nSlotID, m_LEquipItem.m_nIUID, m_LEquipItem.m_nDura, nUneqSlotIDTo2);

			CHECK_ITEM(m_SecREquipItem, SLOTTYPE_EQUIP, nSecToUneqSlotID, true);
			CHECK_ITEM(m_SecLEquipItem, SLOTTYPE_EQUIP, m_SecLEquipItem.m_nSlotID, true);
			CHECK_ITEM(m_REquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo1, true);	
			CHECK_ITEM(m_LEquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo2, true);	

			if (bCrateEmptySlot)
				CHECK_EMPTY_SLOT(m_nCID, m_SecREquipItem.m_nSlotType, m_SecREquipItem.m_nSlotID);
		}

		void DBTestItemEquipment_EquipedAll_Second_Left_Equipment_To_Primary_Right_Equipment(bool bCreateEmptySlot)
		{
			SetTestAllEquipment(m_nCID);

			EQUIPMENT_TEST_ITEM m_SecREquipItem = CreateItem(m_nCID, 2, 2, 4, true, L"'2010-1-1 1:1:0'");
			EQUIPMENT_TEST_ITEM m_SecLEquipItem = CreateItem(m_nCID, 2, 3, 5, true, L"'2010-1-1 1:1:0'");

			int16 nSecToUneqSlotID = m_REquipItem.m_nSlotID;
			int16 nUneqSlotIDTo1 = 10;
			int16 nUneqSlotIDTo2 = 11;

			if (bCreateEmptySlot)
			{
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo1);
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo2);
			}
			
			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, m_SecLEquipItem.m_nSlotType, m_SecLEquipItem.m_nSlotID, m_SecLEquipItem.m_nIUID, m_SecLEquipItem.m_nDura, true, nSecToUneqSlotID
				, m_REquipItem.m_nSlotID, m_REquipItem.m_nIUID, m_REquipItem.m_nDura, nUneqSlotIDTo1
				, m_LEquipItem.m_nSlotID, m_LEquipItem.m_nIUID, m_LEquipItem.m_nDura, nUneqSlotIDTo2);

			CHECK_ITEM(m_SecLEquipItem, SLOTTYPE_EQUIP, nSecToUneqSlotID, true);
			CHECK_ITEM(m_SecREquipItem, SLOTTYPE_EQUIP, m_SecREquipItem.m_nSlotID, true);
			CHECK_ITEM(m_REquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo1, true);	
			CHECK_ITEM(m_LEquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo2, true);	

			if (bCreateEmptySlot)
				CHECK_EMPTY_SLOT(m_nCID, m_SecLEquipItem.m_nSlotType, m_SecLEquipItem.m_nSlotID);
		}

		void DBTESTITEMEQUIPMENT_EQUIPEDALL_SECOND_LEFT_EQUIPMENT_TO_PRIMARY_LEFT_EQUIPMENT(bool bCreateEmptySlot)
		{
			SetTestAllEquipment(m_nCID);

			EQUIPMENT_TEST_ITEM m_SecREquipItem = CreateItem(m_nCID, 2, 2, 4, true, L"'2010-1-1 1:1:0'");
			EQUIPMENT_TEST_ITEM m_SecLEquipItem = CreateItem(m_nCID, 2, 3, 5, true, L"'2010-1-1 1:1:0'");

			int16 nSecToUneqSlotID = m_LEquipItem.m_nSlotID;
			int16 nUneqSlotIDTo1 = 10;
			int16 nUneqSlotIDTo2 = 11;

			if (bCreateEmptySlot)
			{
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo1);
				DBTestHelper::InsertEmptyItemSlot(m_nAID, m_nCID, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, nUneqSlotIDTo2);
			}
			
			UTestDB.Execute(strSQL.c_str()
				, GConfig::m_nMyWorldID
				, m_nCID
				, m_SecLEquipItem.m_nSlotType, m_SecLEquipItem.m_nSlotID, m_SecLEquipItem.m_nIUID, m_SecLEquipItem.m_nDura, true, nSecToUneqSlotID
				, m_REquipItem.m_nSlotID, m_REquipItem.m_nIUID, m_REquipItem.m_nDura, nUneqSlotIDTo1
				, m_LEquipItem.m_nSlotID, m_LEquipItem.m_nIUID, m_LEquipItem.m_nDura, nUneqSlotIDTo2);

			CHECK_ITEM(m_SecLEquipItem, SLOTTYPE_EQUIP, nSecToUneqSlotID, true);
			CHECK_ITEM(m_SecREquipItem, SLOTTYPE_EQUIP, m_SecREquipItem.m_nSlotID, true);
			CHECK_ITEM(m_REquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo1, true);	
			CHECK_ITEM(m_LEquipItem, SLOTTYPE_INVENTORY, nUneqSlotIDTo2, true);	

			if (bCreateEmptySlot)
				CHECK_EMPTY_SLOT(m_nCID, m_SecLEquipItem.m_nSlotType, m_SecLEquipItem.m_nSlotID);
		}

	public :
		int64				m_nAID;
		int64				m_nCID;
		EQUIPMENT_TEST_ITEM	m_InvenItem;
		EQUIPMENT_TEST_ITEM m_REquipItem;
		EQUIPMENT_TEST_ITEM m_LEquipItem;
	};

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_EquipRightSlot)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_EQUIPRIGHTSLOT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_EquipRightSlot_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_EQUIPRIGHTSLOT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_EquipRightSlot_SwapPosition)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_EQUIPRIGHTSLOT_SWAPPOSITION(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_EquipRightSlot_SwapPosition_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_EQUIPRIGHTSLOT_SWAPPOSITION(false);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_EquipLeftSlot)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_EQUIPLEFTSLOT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_EquipLeftSlot_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_EQUIPLEFTSLOT(false);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_EquipEmptySlot)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_EQUIPEMPTYSLOT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_EquipEmptySlot_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_EQUIPEMPTYSLOT(false);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedRight_EquipRightSlot)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDRIGHT_EQUIPRIGHTSLOT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedRight_EquipRightSlot_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDRIGHT_EQUIPRIGHTSLOT(false);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedRight_EquipEmptySlot)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDRIGHT_EQUIPEMPTYSLOT(true);		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedRight_EquipEmptySlot_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDRIGHT_EQUIPEMPTYSLOT(false);		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedLeft_EquipLeftSlot)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDLEFT_EQUIPLEFTSLOT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedLeft_EquipLeftSlot_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDLEFT_EQUIPLEFTSLOT(false);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedLeft_EquipEmptySlot)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDLEFT_EQUIPEMPTYSLOT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedLeft_EquipEmptySlot_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDLEFT_EQUIPEMPTYSLOT(false);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_UnequipAllSlot)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_UNEQUIPALLSLOT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_UnequipAllSlot_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_UNEQUIPALLSLOT(false);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_UnequipRightSlot)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_UNEQUIPRIGHTSLOT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_UnequipRightSlot_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_UNEQUIPRIGHTSLOT(false);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_UnequipLeftSlot)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_UNEQUIPLEFTSLOT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_UnequipLeftSlot_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_UNEQUIPLEFTSLOT(false);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedRight_UnequipRightSlot)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDRIGHT_UNEQUIPRIGHTSLOT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedRight_UnequipRightSlot_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDRIGHT_UNEQUIPRIGHTSLOT(false);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedLeft_UnequipLeftSlot)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDLEFT_UNEQUIPLEFTSLOT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedLeft_UnequipLeftSlot_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDLEFT_UNEQUIPLEFTSLOT(false);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_Second_Right_Equipment_To_Primary_Right_Equipment)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_SECOND_RIGHT_EQUIPMENT_TO_PRIMARY_RIGHT_EQUIPMENT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_Second_Right_Equipment_To_Primary_Right_Equipment_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_SECOND_RIGHT_EQUIPMENT_TO_PRIMARY_RIGHT_EQUIPMENT(false);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_Second_Right_Equipment_To_Primary_Left_Equipment)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_SECOND_RIGHT_EQUIPMENT_TO_PRIMARY_LEFT_EQUIPMENT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_Second_Right_Equipment_To_Primary_Left_Equipment_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_SECOND_RIGHT_EQUIPMENT_TO_PRIMARY_LEFT_EQUIPMENT(false);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_Second_Left_Equipment_To_Primary_Right_Equipment)
	{
		DBTestItemEquipment_EquipedAll_Second_Left_Equipment_To_Primary_Right_Equipment(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_Second_Left_Equipment_To_Primary_Right_Equipment_CREATE)
	{
		DBTestItemEquipment_EquipedAll_Second_Left_Equipment_To_Primary_Right_Equipment(false);
	}

	

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_Second_Left_Equipment_To_Primary_Left_Equipment)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_SECOND_LEFT_EQUIPMENT_TO_PRIMARY_LEFT_EQUIPMENT(true);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestItemEquipment_EquipedAll_Second_Left_Equipment_To_Primary_Left_Equipment_CREATE)
	{
		DBTESTITEMEQUIPMENT_EQUIPEDALL_SECOND_LEFT_EQUIPMENT_TO_PRIMARY_LEFT_EQUIPMENT(false);
	}
}