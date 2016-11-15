#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"
#include "GConfig.h"

SUITE(DBTestItem)
{
	class FIXTURE_ITEM_DEF
	{
	public :
		FIXTURE_ITEM_DEF()
		{
			m_nItemID_1 = 1;
			m_nItemID_2 = 2;

			CHECK_EQUAL(m_nItemID_1, DBTestHelper::InsertItemInfo(m_nItemID_1, L"test_item_1"));
			CHECK_EQUAL(m_nItemID_2, DBTestHelper::InsertItemInfo(m_nItemID_2, L"test_item_2"));
		}

	public :
		int m_nItemID_1;
		int m_nItemID_2;
	};

	class FIXTURE_CHARACTER_AND_ITEM_DEF : public FIXTURE_ITEM_DEF
	{
	public :
		FIXTURE_CHARACTER_AND_ITEM_DEF()
		{
			m_nAID = 1;
			m_nCID = DBTestHelper::InsertCharacter(m_nAID, GConfig::m_nMyWorldID, L"test_char");
			CHECK(0 < m_nCID);
		}

	public :
		int64 m_nAID;
		int64 m_nCID;
	};

	class FIXTURE_CHARACTER_AND_ITEM_INSTANCE : public FIXTURE_CHARACTER_AND_ITEM_DEF
	{
	public :
		FIXTURE_CHARACTER_AND_ITEM_INSTANCE()
		{
			m_nSlotType = SLOTTYPE_INVENTORY;
			m_nSlotID	= 1;

			m_nIUID = DBTestHelper::InsertItem(m_nAID, m_nCID, GConfig::m_nMyWorldID, m_nItemID_1, 1, m_nSlotType, m_nSlotID);
			CHECK(0 < m_nIUID);
		}

		int64 m_nIUID;
		uint8 m_nSlotType;
		int16 m_nSlotID;
	};

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_DEF, USP_RZ_CHAR_GET_ITEM)
	{
		struct _ITEM
		{
			_ITEM() {}
			_ITEM(const int nItemID, const int16 nStackAmt, const uint8 nSlotType, const int16 nSlotID)
			{
				nItemID_	= nItemID;
				nStackAmt_	= nStackAmt;
				nSlotType_	= nSlotType;
				nSlotID_	= nSlotID;
			}

			int		nItemID_;
			int16	nStackAmt_;
			uint8	nSlotType_;
			int16	nSlotID_;
		};

		vector<_ITEM> vItem;
		vItem.push_back(_ITEM(m_nItemID_1, 1, SLOTTYPE_INVENTORY, 1));
		vItem.push_back(_ITEM(m_nItemID_2, 2, SLOTTYPE_EQUIP, 2));
		
		const int64 nIUID1 = DBTestHelper::InsertItem(m_nAID, m_nCID, GConfig::m_nMyWorldID, vItem[0].nItemID_, vItem[0].nStackAmt_, vItem[0].nSlotType_, vItem[0].nSlotID_);
		CHECK(0 < nIUID1);
		const int64 nIUID2 = DBTestHelper::InsertItem(m_nAID, m_nCID, GConfig::m_nMyWorldID, vItem[1].nItemID_, vItem[1].nStackAmt_, vItem[1].nSlotType_, vItem[1].nSlotID_);
		CHECK(0 < nIUID2);

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_CHAR_GET_ITEM (%d, %I64d)}", GConfig::m_nMyWorldID, m_nCID));

		for (; !rs.IsEOF(); rs.MoveNext())
		{
			_ITEM item;
			if (nIUID1 == rs.Field(L"IUID").AsInt64())
				item = vItem[0];
			else if (nIUID2 == rs.Field(L"IUID").AsInt64())
				item = vItem[1];
			else
			{
				CHECK(false);
				break;
			}

			CHECK_EQUAL(item.nSlotType_, rs.Field(L"SLOT_TYPE").AsByte());
			CHECK_EQUAL(item.nSlotID_, rs.Field(L"SLOT_ID").AsShort());
			CHECK_EQUAL(item.nItemID_, rs.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(item.nStackAmt_, rs.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(0, rs.Field(L"SOUL_COUNT").AsByte());
			CHECK_EQUAL(10, rs.Field(L"DURA").AsByte());
			CHECK_EQUAL(10, rs.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(5, rs.Field(L"USAGE_PERIOD").AsInt());
		}

		CHECK_EQUAL(2, rs.GetFetchedRowCount());
		rs.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_INSTANCE, USP_RZ_ITEM_UPDATE_STACK_AMT)
	{
		UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET DURA = 0, MAX_DURA = 0 WHERE IUID = %I64d;", m_nIUID);

		const int16 nUpdStackAmt = 10;
		CHECK(UTestDB.Execute(L"{CALL USP_RZ_ITEM_UPDATE_STACK_AMT (%d, %I64d, %d, %d, %I64d, %d)}"
			, GConfig::m_nMyWorldID
			, m_nCID, m_nSlotType, m_nSlotID, m_nIUID, nUpdStackAmt));

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"SELECT STACK_AMT FROM dbo.RZ_INVEN WHERE IUID = %I64d", m_nIUID));
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(nUpdStackAmt, rs.Field(L"STACK_AMT").AsInt());
		rs.Close();
	}


	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_INSTANCE, USP_RZ_ITEM_UPDATE_DURA)
	{
		const uint8 nUpdDura = 110;
		CHECK(UTestDB.Execute(L"{CALL USP_RZ_ITEM_UPDATE_DURA (%d, %I64d, %d, %d, %I64d, %d)}"
			, GConfig::m_nMyWorldID
			, m_nCID, m_nSlotType, m_nSlotID, m_nIUID, nUpdDura));

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"SELECT DURA FROM dbo.RZ_INVEN WHERE IUID = %d", m_nIUID));
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(nUpdDura, rs.Field(L"DURA").AsInt());
		rs.Close();
	}


	GAMEDB_TEST(USP_RZ_ITEM_DYE)
	{
		const int64 CSN						= DBTestHelper::InsertCharacter(1, GConfig::m_nMyWorldID);
		const int	DYE_ITEM_SLOT_ID		= 0;
		const int64 DYE_IUID				= DBTestHelper::InsertItem(1, CSN, GConfig::m_nMyWorldID, 1, 1, 1, DYE_ITEM_SLOT_ID);
		const int	TARGET_ITEM_SLOT_TYPE	= 2;
		const int	TARGET_ITEM_SLOT_ID		= 1;
		const int64 TARGET_IUID				= DBTestHelper::InsertItem(1, CSN, GConfig::m_nMyWorldID, 2, 1, TARGET_ITEM_SLOT_TYPE, TARGET_ITEM_SLOT_ID);
		const int	COLOR					= 12345;

		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_DYE (%d, %I64d, %d, %I64d, %d, %d, %I64d, %d)}"
			, GConfig::m_nMyWorldID
			, CSN
			, DYE_ITEM_SLOT_ID, DYE_IUID
			, TARGET_ITEM_SLOT_TYPE, TARGET_ITEM_SLOT_ID, TARGET_IUID
			, COLOR);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT SLOT_ID, IUID, COLOR FROM dbo.RZ_INVEN WHERE (SLOT_TYPE = %d AND SLOT_ID = %d) OR (SLOT_TYPE = %d AND SLOT_ID = %d)"
			, 1, DYE_ITEM_SLOT_ID
			, TARGET_ITEM_SLOT_TYPE, TARGET_ITEM_SLOT_ID);
		for (; !rs.IsEOF(); rs.MoveNext())
		{
			if (DYE_ITEM_SLOT_ID == rs.Field(L"SLOT_ID").AsInt())
			{
				CHECK_EQUAL(0, rs.Field(L"IUID").AsInt64());
				CHECK_EQUAL(-1, rs.Field(L"COLOR").AsInt());
			}
			else if (TARGET_ITEM_SLOT_ID == rs.Field(L"SLOT_ID").AsInt())
			{
				CHECK_EQUAL(TARGET_IUID, rs.Field(L"IUID").AsInt64());
				CHECK_EQUAL(COLOR, rs.Field(L"COLOR").AsInt());
			}
		}
		CHECK_EQUAL(2, rs.GetFetchedRowCount());
		rs.Close();

	}

	LOGDB_TEST(USP_RZ_ITEM_DYE_LOG)
	{
		const int64		GSN				= 1;
		const int		WORLD			= 2;
		const int64		CSN				= 3;
		const int		CHAR_PTM		= 4;
		const int		DELTA_CHAR_PTM	= 5;
		const  uint8	LEV				= 6;
		const int		CUR_MONEY		= 7;
		const int64		TARGET_IUID		= 8;
		const int		TARGET_ITEM_ID	= 9;
		const int64		DYE_IUID		= 10;
		const int		DYE_ITEM_ID		= 11;

		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_DYE_LOG (%I64d, %d, %I64d, %d, %d, %d, %d, %I64d, %d, %I64d, %d)}"
			, GSN, WORLD, CSN, CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_MONEY, TARGET_IUID, TARGET_ITEM_ID, DYE_IUID, DYE_ITEM_ID);

		TDBRecordSet rsTarget;
		UTestDB.Execute(rsTarget, L"SELECT CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_MONEY, IUID, ITEM_ID FROM dbo.RZ_ITEM_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND	OWNER_ID = %I64d AND CODE = 1101;"
			, GSN, WORLD, CSN);
		CHECK_EQUAL(1, rsTarget.GetFetchedRowCount());
		CHECK_EQUAL(CHAR_PTM, rsTarget.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(DELTA_CHAR_PTM, rsTarget.Field(L"DELTA_CHAR_PTM").AsInt());
		CHECK_EQUAL(LEV, rsTarget.Field(L"LEV").AsByte());
		CHECK_EQUAL(CUR_MONEY, rsTarget.Field(L"CUR_MONEY").AsInt());
		CHECK_EQUAL(TARGET_IUID, rsTarget.Field(L"IUID").AsInt64());
		CHECK_EQUAL(TARGET_ITEM_ID, rsTarget.Field(L"ITEM_ID").AsInt());
		rsTarget.Close();

		TDBRecordSet rsDye;
		UTestDB.Execute(rsDye, L"SELECT CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_MONEY, IUID, ITEM_ID, CUR_STACK_AMT, DELTA_STACK_AMT FROM dbo.RZ_ITEM_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND OWNER_ID = %I64d AND CODE = 1102;"
			, GSN, WORLD, CSN);
		CHECK_EQUAL(1, rsDye.GetFetchedRowCount());
		CHECK_EQUAL(CHAR_PTM, rsDye.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(DELTA_CHAR_PTM, rsDye.Field(L"DELTA_CHAR_PTM").AsInt());
		CHECK_EQUAL(LEV, rsDye.Field(L"LEV").AsByte());
		CHECK_EQUAL(CUR_MONEY, rsDye.Field(L"CUR_MONEY").AsInt());
		CHECK_EQUAL(DYE_IUID, rsDye.Field(L"IUID").AsInt64());
		CHECK_EQUAL(DYE_ITEM_ID, rsDye.Field(L"ITEM_ID").AsInt());
		CHECK_EQUAL(0, rsDye.Field(L"CUR_STACK_AMT").AsShort());
		CHECK_EQUAL(-1, rsDye.Field(L"DELTA_STACK_AMT").AsShort());
		rsDye.Close();
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CHARACTER_AND_ITEM_INSTANCE, USP_RZ_ITEM_UPDATE_SOUL_CNT)
	{
		const uint8 nSoulCnt = 10;
		CHECK(UTestDB.Execute(L"{CALL USP_RZ_ITEM_UPDATE_SOUL_CNT (%d, %I64d, %d, %d, %I64d, %d)}"
			, GConfig::m_nMyWorldID, m_nCID, m_nSlotType, m_nSlotID, m_nIUID, nSoulCnt));
		
		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"SELECT SOUL_COUNT FROM dbo.RZ_INVEN WHERE IUID = %d", m_nIUID));
		CHECK_EQUAL(nSoulCnt, rs.Field(L"SOUL_COUNT").AsInt());
		rs.Close();		
	}	

	GAMEDB_TEST(USP_RZ_CHAR_UPDATE_STORAGE_MONEY)
	{
		const int64 GSN			= 1;
		const int64 CSN			= DBTestHelper::InsertCharacter(GSN, GConfig::m_nMyWorldID);
		const int MONEY			= 10;
		const int STORAGE_MONEY = 100;

		UTestDB.Execute(L"UPDATE dbo.RZ_CHARACTER SET [MONEY] = 100, STORAGE_MONEY = 10 WHERE CHAR_ID = %I64d;", CSN);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_UPDATE_STORAGE_MONEY (%d, %I64d, %I64d, %d, %d)}"
			, GConfig::m_nMyWorldID
			, GSN
			, CSN
			, MONEY
			, STORAGE_MONEY);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT [MONEY], STORAGE_MONEY FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d", CSN);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(MONEY, rs.Field(L"MONEY").AsInt());
		CHECK_EQUAL(STORAGE_MONEY, rs.Field(L"STORAGE_MONEY").AsInt());
		rs.Close();
	}
}