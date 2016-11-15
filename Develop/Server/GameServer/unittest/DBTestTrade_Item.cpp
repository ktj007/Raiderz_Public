#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"


SUITE(DBTestTradeItem)
{
	class TR_ITEM
	{
	public :
		TR_ITEM(int64 nAID, const int64 nCID, int64 nToAID, const int64 nToCID, const uint8 nSlotType, const int16 nSlotID, const int16 nToSlotID, const int nItemID, const int16 nStackAmt, const uint8 nDura, bool bInsertEmptySlot)
			: m_nSlotType(nSlotType), m_nSlotID(nSlotID), m_nToSlotID(nToSlotID), m_nItemID(nItemID), m_nStackAmt(nStackAmt), m_nModStackAmt(0), m_nDura(nDura)
		{
			DBTestHelper::InsertItemInfo(nItemID);
			m_nIUID = DBTestHelper::InsertItem(nAID, nCID, GConfig::m_nMyWorldID, nItemID, nStackAmt, nSlotType, nSlotID);
			UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET DURA = %d, MAX_DURA = %d WHERE IUID = %I64d", nDura, nDura + 10, m_nIUID);
			m_nMaxDura = nDura + 10;

			if (bInsertEmptySlot)
				DBTestHelper::InsertEmptyItemSlot(nToAID, nToCID, GConfig::m_nMyWorldID, nSlotType, nToSlotID);
		}

		int64	m_nIUID;
		uint8	m_nSlotType;
		int16	m_nSlotID;
		int16   m_nToSlotID;
		int		m_nItemID;
		int16	m_nStackAmt;
		int16	m_nModStackAmt;
		uint8	m_nDura;
		uint8	m_nMaxDura;
	};

	typedef vector<TR_ITEM> TR_ITEM_VEC;

	class FIXTURE
	{
	public :
		FIXTURE()
		{
			m_nAID_1 = 1;
			m_nAID_2 = 2;

			m_nCID_1 = DBTestHelper::InsertCharacter(m_nAID_1, GConfig::m_nMyWorldID, L"test1");
			m_nCID_2 = DBTestHelper::InsertCharacter(m_nAID_2, GConfig::m_nMyWorldID, L"test2");

			m_nCharPtm_1 = 100;
			m_nCharPtm_2 = 200;			
		}

		void InsertEmptySlot(bool bInsertEmptySlot = true)
		{
			for (int i = 0; i < 5; ++i)
			{
				m_vTrItem[0].push_back(TR_ITEM(m_nAID_1, m_nCID_1, m_nAID_2, m_nCID_2, 1/*SlotType*/, i		, i + 100/*ToSlotID*/, i + 1/*ItemID*/		, i + 1	, i + 6, bInsertEmptySlot));
				m_vTrItem[1].push_back(TR_ITEM(m_nAID_2, m_nCID_2, m_nAID_1, m_nCID_1, 1/*SlotType*/, i + 10	, i + 150/*ToSlotID*/, i + 100/*ItemID*/	, i + 10, i + 16, bInsertEmptySlot));
			}
		}

		wstring TradeQuery()
		{
			return wstring(L"{CALL dbo.USP_RZ_TRADE ( \
													   %d, %I64d, %I64d, %d, %d, %d \
													 , %d, %I64d, %d, %d, %d, %d, %d, %d \
													 , %d, %I64d, %d, %d, %d, %d, %d, %d \
													 , %d, %I64d, %d, %d, %d, %d, %d, %d \
													 , %d, %I64d, %d, %d, %d, %d, %d, %d \
													 , %d, %I64d, %d, %d, %d, %d, %d, %d \
													 , %I64d, %I64d, %d, %d, %d \
													 , %d, %I64d, %d, %d, %d, %d, %d, %d \
													 , %d, %I64d, %d, %d, %d, %d, %d, %d \
													 , %d, %I64d, %d, %d, %d, %d, %d, %d \
													 , %d, %I64d, %d, %d, %d, %d, %d, %d \
													 , %d, %I64d, %d, %d, %d, %d, %d, %d \
													 , %d)}");
		}

		void TEST_TRADE_ALL_MOVED()
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, TradeQuery().c_str()
				, GConfig::m_nMyWorldID
				, m_nAID_1, m_nCID_1, 0, 0, m_nCharPtm_1 
				, m_vTrItem[0][0].m_nSlotID, m_vTrItem[0][0].m_nIUID, m_vTrItem[0][0].m_nItemID, 0, m_vTrItem[0][0].m_nStackAmt, m_vTrItem[0][0].m_nDura, 0, m_vTrItem[0][0].m_nToSlotID
				, m_vTrItem[0][1].m_nSlotID, m_vTrItem[0][1].m_nIUID, m_vTrItem[0][1].m_nItemID, 0, m_vTrItem[0][1].m_nStackAmt, m_vTrItem[0][1].m_nDura, 0, m_vTrItem[0][1].m_nToSlotID
				, m_vTrItem[0][2].m_nSlotID, m_vTrItem[0][2].m_nIUID, m_vTrItem[0][2].m_nItemID, 0, m_vTrItem[0][2].m_nStackAmt, m_vTrItem[0][2].m_nDura, 0, m_vTrItem[0][2].m_nToSlotID
				, m_vTrItem[0][3].m_nSlotID, m_vTrItem[0][3].m_nIUID, m_vTrItem[0][3].m_nItemID, 0, m_vTrItem[0][3].m_nStackAmt, m_vTrItem[0][3].m_nDura, 0, m_vTrItem[0][3].m_nToSlotID
				, m_vTrItem[0][4].m_nSlotID, m_vTrItem[0][4].m_nIUID, m_vTrItem[0][4].m_nItemID, 0, m_vTrItem[0][4].m_nStackAmt, m_vTrItem[0][4].m_nDura, 0, m_vTrItem[0][4].m_nToSlotID

				, m_nAID_2, m_nCID_2, 0, 0, m_nCharPtm_2
				, m_vTrItem[1][0].m_nSlotID, m_vTrItem[1][0].m_nIUID, m_vTrItem[1][0].m_nItemID, 0, m_vTrItem[1][0].m_nStackAmt, m_vTrItem[1][0].m_nDura, 0, m_vTrItem[1][0].m_nToSlotID
				, m_vTrItem[1][1].m_nSlotID, m_vTrItem[1][1].m_nIUID, m_vTrItem[1][1].m_nItemID, 0, m_vTrItem[1][1].m_nStackAmt, m_vTrItem[1][1].m_nDura, 0, m_vTrItem[1][1].m_nToSlotID
				, m_vTrItem[1][2].m_nSlotID, m_vTrItem[1][2].m_nIUID, m_vTrItem[1][2].m_nItemID, 0, m_vTrItem[1][2].m_nStackAmt, m_vTrItem[1][2].m_nDura, 0, m_vTrItem[1][2].m_nToSlotID
				, m_vTrItem[1][3].m_nSlotID, m_vTrItem[1][3].m_nIUID, m_vTrItem[1][3].m_nItemID, 0, m_vTrItem[1][3].m_nStackAmt, m_vTrItem[1][3].m_nDura, 0, m_vTrItem[1][3].m_nToSlotID
				, m_vTrItem[1][4].m_nSlotID, m_vTrItem[1][4].m_nIUID, m_vTrItem[1][4].m_nItemID, 0, m_vTrItem[1][4].m_nStackAmt, m_vTrItem[1][4].m_nDura, 0, m_vTrItem[1][4].m_nToSlotID

				, 20
				);

			int i = 0;
			for (;!rs.IsEOF(); rs.MoveNext())
			{
				++i;
			}
			CHECK_EQUAL(20, i);
			rs.Close();
		}

		void TEST_TRADE_SPLIT()
		{

		}


	public :
		int64 m_nAID_1;
		int64 m_nAID_2;

		int64 m_nCID_1;
		int64 m_nCID_2;

		int m_nCharPtm_1;
		int m_nCharPtm_2;

		TR_ITEM_VEC m_vTrItem[2];
	};

	void CHECK_INITED_TEST_ITEM(const int64 nCID, const int64 nToCID, const TR_ITEM& TrItem, bool bExistEmptySlot = true)
	{
		CString strSQL(L"SELECT STACK_AMT, DURA, MAX_DURA FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d AND IUID = %I64d");

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL
			, nCID, TrItem.m_nSlotType, TrItem.m_nSlotID, TrItem.m_nIUID);
		CHECK_EQUAL(TrItem.m_nStackAmt, rs.Field(L"STACK_AMT").AsShort());
		CHECK_EQUAL(TrItem.m_nDura, rs.Field(L"DURA").AsByte());
		CHECK_EQUAL(TrItem.m_nMaxDura, rs.Field(L"MAX_DURA").AsByte());
		rs.Close();

		if (bExistEmptySlot)
		{
			TDBRecordSet rsTo;
			UTestDB.Execute(rsTo, L"SELECT IUID FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d;", nToCID, TrItem.m_nToSlotID);
			CHECK_EQUAL(1, rsTo.GetFetchedRowCount());
			rsTo.Close();
		}		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestTradeItemFixture)
	{
		InsertEmptySlot();

		CHECK_INITED_TEST_ITEM(m_nCID_1, m_nCID_2, m_vTrItem[0][0]);		
		CHECK_INITED_TEST_ITEM(m_nCID_1, m_nCID_2, m_vTrItem[0][1]);		
		CHECK_INITED_TEST_ITEM(m_nCID_1, m_nCID_2, m_vTrItem[0][2]);		
		CHECK_INITED_TEST_ITEM(m_nCID_1, m_nCID_2, m_vTrItem[0][3]);		
		CHECK_INITED_TEST_ITEM(m_nCID_1, m_nCID_2, m_vTrItem[0][4]);	

		CHECK_INITED_TEST_ITEM(m_nCID_2, m_nCID_1, m_vTrItem[1][0]);		
		CHECK_INITED_TEST_ITEM(m_nCID_2, m_nCID_1, m_vTrItem[1][1]);		
		CHECK_INITED_TEST_ITEM(m_nCID_2, m_nCID_1, m_vTrItem[1][2]);		
		CHECK_INITED_TEST_ITEM(m_nCID_2, m_nCID_1, m_vTrItem[1][3]);		
		CHECK_INITED_TEST_ITEM(m_nCID_2, m_nCID_1, m_vTrItem[1][4]);		

		const CString strRowCountSQL(L"SELECT COUNT(*) AS Cnt FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d;");

		TDBRecordSet rsRowCount_1;
		UTestDB.Execute(rsRowCount_1, strRowCountSQL, m_nCID_1);
		CHECK_EQUAL(1, rsRowCount_1.GetFetchedRowCount());
		CHECK_EQUAL(10, rsRowCount_1.Field(L"Cnt").AsInt());
		rsRowCount_1.Close();

		TDBRecordSet rsRowCount_2;
		UTestDB.Execute(rsRowCount_2, strRowCountSQL, m_nCID_2);
		CHECK_EQUAL(1, rsRowCount_2.GetFetchedRowCount());
		CHECK_EQUAL(10, rsRowCount_2.Field(L"Cnt").AsInt());
		rsRowCount_2.Close();		
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, DBTestTradeItemFixture_CREATE)
	{
		InsertEmptySlot(false);

		CHECK_INITED_TEST_ITEM(m_nCID_1, m_nCID_2, m_vTrItem[0][0], false);		
		CHECK_INITED_TEST_ITEM(m_nCID_1, m_nCID_2, m_vTrItem[0][1], false);		
		CHECK_INITED_TEST_ITEM(m_nCID_1, m_nCID_2, m_vTrItem[0][2], false);		
		CHECK_INITED_TEST_ITEM(m_nCID_1, m_nCID_2, m_vTrItem[0][3], false);		
		CHECK_INITED_TEST_ITEM(m_nCID_1, m_nCID_2, m_vTrItem[0][4], false);	

		CHECK_INITED_TEST_ITEM(m_nCID_2, m_nCID_1, m_vTrItem[1][0], false);		
		CHECK_INITED_TEST_ITEM(m_nCID_2, m_nCID_1, m_vTrItem[1][1], false);		
		CHECK_INITED_TEST_ITEM(m_nCID_2, m_nCID_1, m_vTrItem[1][2], false);		
		CHECK_INITED_TEST_ITEM(m_nCID_2, m_nCID_1, m_vTrItem[1][3], false);		
		CHECK_INITED_TEST_ITEM(m_nCID_2, m_nCID_1, m_vTrItem[1][4], false);		
	}

	void CHECK_MOVED_ITEM(const int64 nCID, const TR_ITEM& TrItem)
	{
		CString strSQL(L"SELECT STACK_AMT, DURA, MAX_DURA FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d AND IUID = %I64d");

		TDBRecordSet rs;
		UTestDB.Execute(rs, strSQL
			, nCID, TrItem.m_nSlotType, TrItem.m_nToSlotID, TrItem.m_nIUID);
		CHECK_EQUAL(TrItem.m_nStackAmt, rs.Field(L"STACK_AMT").AsShort());
		CHECK_EQUAL(TrItem.m_nDura, rs.Field(L"DURA").AsByte());
		CHECK_EQUAL(TrItem.m_nMaxDura, rs.Field(L"MAX_DURA").AsByte());
		rs.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_TRADE_ALL_MOVED)
	{
		InsertEmptySlot();

		TEST_TRADE_ALL_MOVED();

		CHECK_MOVED_ITEM(m_nCID_2, m_vTrItem[0][0]);
		CHECK_MOVED_ITEM(m_nCID_2, m_vTrItem[0][1]);
		CHECK_MOVED_ITEM(m_nCID_2, m_vTrItem[0][2]);
		CHECK_MOVED_ITEM(m_nCID_2, m_vTrItem[0][3]);
		CHECK_MOVED_ITEM(m_nCID_2, m_vTrItem[0][4]);

		CHECK_MOVED_ITEM(m_nCID_1, m_vTrItem[1][0]);
		CHECK_MOVED_ITEM(m_nCID_1, m_vTrItem[1][1]);
		CHECK_MOVED_ITEM(m_nCID_1, m_vTrItem[1][2]);
		CHECK_MOVED_ITEM(m_nCID_1, m_vTrItem[1][3]);
		CHECK_MOVED_ITEM(m_nCID_1, m_vTrItem[1][4]);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_TRADE_ALL_MOVED_CREATE)
	{
		InsertEmptySlot(false);

		TEST_TRADE_ALL_MOVED();

		CHECK_MOVED_ITEM(m_nCID_2, m_vTrItem[0][0]);
		CHECK_MOVED_ITEM(m_nCID_2, m_vTrItem[0][1]);
		CHECK_MOVED_ITEM(m_nCID_2, m_vTrItem[0][2]);
		CHECK_MOVED_ITEM(m_nCID_2, m_vTrItem[0][3]);
		CHECK_MOVED_ITEM(m_nCID_2, m_vTrItem[0][4]);

		CHECK_MOVED_ITEM(m_nCID_1, m_vTrItem[1][0]);
		CHECK_MOVED_ITEM(m_nCID_1, m_vTrItem[1][1]);
		CHECK_MOVED_ITEM(m_nCID_1, m_vTrItem[1][2]);
		CHECK_MOVED_ITEM(m_nCID_1, m_vTrItem[1][3]);
		CHECK_MOVED_ITEM(m_nCID_1, m_vTrItem[1][4]);
	}
}