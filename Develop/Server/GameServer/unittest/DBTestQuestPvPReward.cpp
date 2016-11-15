#include "stdafx.h"
#include "DBTestGlobal.h"
#include "DBTestHelper_Quest.h"
#include "GConfig.h"

SUITE(QUEST_PVP_REWARD)
{
	class QPVP_ITEM
	{
	public :
		int64	IUID;
		int		SLOT_ID;
		uint8	TYPE;
		int		ITEM_ID;
		int16	STACK_AMT;
		bool	CLAIMED;
		uint8	MAX_DURA;
		bool	PERIOD;
		int		USAGE_PERIOD;
		wstring EFF_END_DATE;		
	};

	class FIXTURE_QPVP
	{
	public :
		enum {MAX_ITEM_COUNT = 5, TYPE_INSERT = 1, TYPE_UPDATE = 2};

		FIXTURE_QPVP()
		{
			m_CSN = DBTestHelper::InsertCharacter(m_GSN, GConfig::m_nMyWorldID);
			UTestDB.Execute(L"UPDATE dbo.RZ_CHARACTER SET MONEY = 0, EXP = 0 WHERE CHAR_ID = %I64d;", m_GSN);

			for (int i = 0; i < MAX_ITEM_COUNT; ++i)
			{
				// DBTestHelper::InsertEmptyItemSlot(m_GSN, m_CSN, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, i);

				QPVP_ITEM_LIST[i].IUID				= 0;
				QPVP_ITEM_LIST[i].SLOT_ID			= i;
				QPVP_ITEM_LIST[i].TYPE				= TYPE_INSERT;
				QPVP_ITEM_LIST[i].ITEM_ID			= i + 1;
				QPVP_ITEM_LIST[i].STACK_AMT			= i + 1;
				QPVP_ITEM_LIST[i].CLAIMED			= 1 != (i % 2);
				QPVP_ITEM_LIST[i].MAX_DURA			= i + 2;
				QPVP_ITEM_LIST[i].PERIOD			= 1 == (i % 2);
				QPVP_ITEM_LIST[i].USAGE_PERIOD		= i + 3;
				QPVP_ITEM_LIST[i].EFF_END_DATE		= L"NULL";				
			}		

			m_SNDR_NAME = wstring(L"TEST SENDER");
			m_TITLE = wstring(L"TEST REWARD");
		}

		void InsertEmptySlot()
		{
			for (int i = 0; i < MAX_ITEM_COUNT; ++i)
				DBTestHelper::InsertEmptyItemSlot(m_GSN, m_CSN, GConfig::m_nMyWorldID, SLOTTYPE_INVENTORY, QPVP_ITEM_LIST[i].SLOT_ID);			
		}

		void QPVP_REWARD_ITEM_INVEN()
		{
			const wchar_t* pSQL = L"{CALL dbo.USP_RZ_QPVP_REWARD_INVEN (%d, %I64d, %I64d, %d, %d, %d, %d\
								   , %d, %I64d, %d, %d, %d, %d, %d, %s\
								   , %d, %I64d, %d, %d, %d, %d, %d, %s\
								   , %d, %I64d, %d, %d, %d, %d, %d, %s\
								   , %d, %I64d, %d, %d, %d, %d, %d, %s\
								   , %d, %I64d, %d, %d, %d, %d, %d, %s)}";

			TDBRecordSet rs;
			UTestDB.Execute(rs, pSQL
				, GConfig::m_nMyWorldID
				, m_GSN, m_CSN, m_CHAR_PTM, m_MONEY, m_EXP, m_LEV
				, QPVP_ITEM_LIST[0].SLOT_ID, QPVP_ITEM_LIST[0].IUID, QPVP_ITEM_LIST[0].ITEM_ID, QPVP_ITEM_LIST[0].STACK_AMT, QPVP_ITEM_LIST[0].CLAIMED, QPVP_ITEM_LIST[0].MAX_DURA
				, QPVP_ITEM_LIST[0].PERIOD, QPVP_ITEM_LIST[0].EFF_END_DATE.c_str()
				
				, QPVP_ITEM_LIST[1].SLOT_ID, QPVP_ITEM_LIST[1].IUID, QPVP_ITEM_LIST[1].ITEM_ID, QPVP_ITEM_LIST[1].STACK_AMT, QPVP_ITEM_LIST[1].CLAIMED, QPVP_ITEM_LIST[1].MAX_DURA
				, QPVP_ITEM_LIST[1].PERIOD, QPVP_ITEM_LIST[1].EFF_END_DATE.c_str()
				
				, QPVP_ITEM_LIST[2].SLOT_ID, QPVP_ITEM_LIST[2].IUID, QPVP_ITEM_LIST[2].ITEM_ID, QPVP_ITEM_LIST[2].STACK_AMT, QPVP_ITEM_LIST[2].CLAIMED, QPVP_ITEM_LIST[2].MAX_DURA
				, QPVP_ITEM_LIST[2].PERIOD, QPVP_ITEM_LIST[2].EFF_END_DATE.c_str()
				
				, QPVP_ITEM_LIST[3].SLOT_ID, QPVP_ITEM_LIST[3].IUID, QPVP_ITEM_LIST[3].ITEM_ID, QPVP_ITEM_LIST[3].STACK_AMT, QPVP_ITEM_LIST[3].CLAIMED, QPVP_ITEM_LIST[3].MAX_DURA
				, QPVP_ITEM_LIST[3].PERIOD, QPVP_ITEM_LIST[3].EFF_END_DATE.c_str()
				
				, QPVP_ITEM_LIST[4].SLOT_ID, QPVP_ITEM_LIST[4].IUID, QPVP_ITEM_LIST[4].ITEM_ID, QPVP_ITEM_LIST[4].STACK_AMT, QPVP_ITEM_LIST[4].CLAIMED, QPVP_ITEM_LIST[4].MAX_DURA
				, QPVP_ITEM_LIST[4].PERIOD, QPVP_ITEM_LIST[4].EFF_END_DATE.c_str());
			for (; !rs.IsEOF(); rs.MoveNext())
			{
				const int i = rs.Field(L"SLOT_ID").AsInt();
				QPVP_ITEM_LIST[i].IUID = rs.Field(L"NEW_IUID").AsInt64();		

				CHECK(0 < QPVP_ITEM_LIST[i].IUID);
			}
			CHECK_EQUAL(FIXTURE_QPVP::MAX_ITEM_COUNT, rs.GetFetchedRowCount());
			rs.Close();
		}

		void CHECK_CHAR(int64 CSN, int CHAR_PTM, int MONEY, int EXP, int LEV)
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT CHAR_PTM, MONEY, EXP, LEV FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", CSN);
			CHECK_EQUAL(CHAR_PTM, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(MONEY, rs.Field(L"MONEY").AsInt());
			CHECK_EQUAL(EXP, rs.Field(L"EXP").AsInt());
			CHECK_EQUAL(LEV, rs.Field(L"LEV").AsByte());
			rs.Close();
		}

		void CHECK_INVEN_INSERT_ITEM(int CHAR_PTM, QPVP_ITEM& qi)
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT ITEM_ID, STACK_AMT, SOUL_COUNT, DURA, MAX_DURA, COLOR, CLAIMED, CHAR_PTM, PERIOD\
								, USAGE_PERIOD, EFF_START_DATE, EFF_END_DATE, REG_DATE\
								FROM dbo.RZ_INVEN WHERE SLOT_TYPE = 1 AND SLOT_ID = %d AND IUID = %I64d;", qi.SLOT_ID, qi.IUID);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(qi.ITEM_ID, rs.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(qi.STACK_AMT, rs.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(0, rs.Field(L"SOUL_COUNT").AsByte());
			CHECK_EQUAL(qi.MAX_DURA, rs.Field(L"DURA").AsByte());
			CHECK_EQUAL(qi.MAX_DURA, rs.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(-1, rs.Field(L"COLOR").AsInt());
			CHECK_EQUAL(qi.CLAIMED, rs.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(CHAR_PTM, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(qi.PERIOD, rs.Field(L"PERIOD").AsBool());
			CHECK_EQUAL(0, rs.Field(L"USAGE_PERIOD").AsInt());
			CHECK_EQUAL(false, rs.Field(L"EFF_START_DATE").IsNull());
			CHECK_EQUAL(true, rs.Field(L"EFF_END_DATE").IsNull());
			rs.Close();
		}

		void CHECK_INVEN_UPDATE_ITEM(int CHAR_PTM, QPVP_ITEM& qi)
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT ITEM_ID, STACK_AMT, SOUL_COUNT, DURA, MAX_DURA, COLOR, CLAIMED, CHAR_PTM, PERIOD\
								 , USAGE_PERIOD, EFF_START_DATE, EFF_END_DATE, REG_DATE\
								 FROM dbo.RZ_INVEN WHERE SLOT_TYPE = 1 AND SLOT_ID = %d AND IUID = %I64d;", qi.SLOT_ID, qi.IUID);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(qi.ITEM_ID, rs.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(qi.STACK_AMT, rs.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(3, rs.Field(L"SOUL_COUNT").AsByte());
			CHECK_EQUAL(1, rs.Field(L"DURA").AsByte());
			CHECK_EQUAL(2, rs.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(4, rs.Field(L"COLOR").AsInt());
			CHECK_EQUAL(true, rs.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(1, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(true, rs.Field(L"PERIOD").AsBool());
			CHECK_EQUAL(6, rs.Field(L"USAGE_PERIOD").AsInt());
			CHECK(wstring(L"2010-1-1 1:1:1") == rs.Field(L"REG_DATE").AsWString());
			CHECK(wstring(L"2010-1-2 2:2:2") == rs.Field(L"EFF_START_DATE").AsWString());
			CHECK(wstring(L"2010-1-3 3:3:3") == rs.Field(L"EFF_END_DATE").AsWString());			
			rs.Close();
		}

		int64 QPVP_REWARD_ITEM_MAIL()
		{
			const wchar_t* pSQL = L"{CALL dbo.USP_RZ_QPVP_REWARD_MAIL (%d, %I64d, %I64d, N'%s', N'%s', %d, %d, %d, %d, %d\
								   , %d, %d, %d\
								   , %d, %d, %d\
								   , %d, %d, %d\
								   , %d, %d, %d\
								   , %d, %d, %d)}";

			TDBRecordSet rs;
			UTestDB.Execute(rs, pSQL
				, GConfig::m_nMyWorldID
				, m_GSN, m_CSN, m_SNDR_NAME.c_str(), m_TITLE.c_str(), 0, m_CHAR_PTM, m_MONEY, m_EXP, m_LEV
				, QPVP_ITEM_LIST[0].ITEM_ID, QPVP_ITEM_LIST[0].STACK_AMT, QPVP_ITEM_LIST[0].MAX_DURA				
				, QPVP_ITEM_LIST[1].ITEM_ID, QPVP_ITEM_LIST[1].STACK_AMT, QPVP_ITEM_LIST[1].MAX_DURA
				, QPVP_ITEM_LIST[2].ITEM_ID, QPVP_ITEM_LIST[2].STACK_AMT, QPVP_ITEM_LIST[2].MAX_DURA
				, QPVP_ITEM_LIST[3].ITEM_ID, QPVP_ITEM_LIST[3].STACK_AMT, QPVP_ITEM_LIST[3].MAX_DURA
				, QPVP_ITEM_LIST[4].ITEM_ID, QPVP_ITEM_LIST[4].STACK_AMT, QPVP_ITEM_LIST[4].MAX_DURA);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			const int64 MAIL_UID = rs.Field(L"MAIL_ID").AsInt64();
			CHECK(0 < MAIL_UID);
			rs.Close();

			return MAIL_UID;
		}

		void CHECK_MAIL(const int64 MAIL_UID)
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT MAIL_ID, SNDR_NAME, TITLE, HAVE_TEXT, MONEY, READ_FLAG, DEF_ITEM_ID\
								 FROM dbo.RZ_MAIL WHERE MAIL_ID = %I64d AND ACCN_ID = %I64d AND CHAR_ID = %I64d AND MAIL_TYPE = 2"
								 , MAIL_UID, m_GSN, m_CSN);
			CHECK_EQUAL(1, rs.GetFetchedCount());
			CHECK(0 < rs.Field(L"MAIL_ID").AsInt64());
			CHECK(m_SNDR_NAME == rs.Field(L"SNDR_NAME").AsWString());
			CHECK(m_TITLE == rs.Field(L"TITLE").AsWString());
			CHECK(!rs.Field(L"HAVE_TEXT").AsBool());
			CHECK_EQUAL(0, rs.Field(L"MONEY").AsInt());
			CHECK_EQUAL(false, rs.Field(L"READ_FLAG").AsBool());
			CHECK_EQUAL(QPVP_ITEM_LIST[0].ITEM_ID, rs.Field(L"DEF_ITEM_ID").AsInt());
			rs.Close();
		}

		void CHECK_MAIL_ITEM(int64 MAIL_UID, QPVP_ITEM& qi, const int16 SLOT_ID)
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT ITEM_ID, STACK_AMT, SOUL_COUNT, DURA, MAX_DURA, COLOR, PERIOD\
								 , USAGE_PERIOD, EFF_START_DATE, EFF_END_DATE, REG_DATE\
								 FROM dbo.RZ_MAIL_ITEM WHERE MAIL_ID = %I64d AND ITEM_ID = %d;", MAIL_UID, qi.ITEM_ID);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(qi.ITEM_ID, rs.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(qi.STACK_AMT, rs.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(0, rs.Field(L"SOUL_COUNT").AsByte());
			CHECK_EQUAL(qi.MAX_DURA, rs.Field(L"DURA").AsByte());
			CHECK_EQUAL(qi.MAX_DURA, rs.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(-1, rs.Field(L"COLOR").AsInt());
			CHECK_EQUAL(false, rs.Field(L"PERIOD").AsBool());
			CHECK_EQUAL(0, rs.Field(L"USAGE_PERIOD").AsInt());			
			rs.Close();
		}

	protected :
		static const int64		m_GSN		= 1;
		int64					m_CSN;
		static const int		m_CHAR_PTM	= 3;
		static const int		m_MONEY		= 4;
		static const int		m_EXP		= 5;
		static const int		m_LEV		= 6;

		wstring					m_SNDR_NAME;
		wstring					m_TITLE;

		QPVP_ITEM	QPVP_ITEM_LIST[MAX_ITEM_COUNT];
	};

	

	GAMEDB_TEST_FIXTURE(FIXTURE_QPVP, USP_RZ_QPVP_REWARD_INVEN_INSERT)
	{
		InsertEmptySlot();
			
		QPVP_REWARD_ITEM_INVEN();

		CHECK_CHAR(m_CSN, m_CHAR_PTM, m_MONEY, m_EXP, m_LEV);
		CHECK_INVEN_INSERT_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[0]);
		CHECK_INVEN_INSERT_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[1]);
		CHECK_INVEN_INSERT_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[2]);
		CHECK_INVEN_INSERT_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[3]);
		CHECK_INVEN_INSERT_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[4]);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_QPVP, USP_RZ_QPVP_REWARD_INVEN_INSERT_CREATE)
	{
		QPVP_REWARD_ITEM_INVEN();

		CHECK_CHAR(m_CSN, m_CHAR_PTM, m_MONEY, m_EXP, m_LEV);
		CHECK_INVEN_INSERT_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[0]);
		CHECK_INVEN_INSERT_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[1]);
		CHECK_INVEN_INSERT_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[2]);
		CHECK_INVEN_INSERT_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[3]);
		CHECK_INVEN_INSERT_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[4]);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_QPVP, USP_RZ_QPVP_REWARD_INVEN_UPDATE)
	{
		InsertEmptySlot();

		for (int i = 0; i < MAX_ITEM_COUNT; ++i)
		{
			QPVP_ITEM_LIST[i].IUID = i + 1;
			QPVP_ITEM_LIST[i].TYPE = TYPE_UPDATE;

			UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET IUID = %I64d, ITEM_ID = %d, CHAR_PTM = 1, STACK_AMT = 2, SOUL_COUNT = 3, COLOR = 4, CLAIMED = 1, PERIOD = 1\
							 , USAGE_PERIOD = 6, DURA = 1, MAX_DURA = 2\
							 , REG_DATE = '2010-1-1 1:1:1', EFF_START_DATE = '2010-1-2 2:2:2', EFF_END_DATE = '2010-1-3 3:3:3'\
							 WHERE OWNER_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d", QPVP_ITEM_LIST[i].IUID, QPVP_ITEM_LIST[i].ITEM_ID, m_CSN, i);
		}

		QPVP_REWARD_ITEM_INVEN();

		CHECK_CHAR(m_CSN, m_CHAR_PTM, m_MONEY, m_EXP, m_LEV);
		CHECK_INVEN_UPDATE_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[0]);
		CHECK_INVEN_UPDATE_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[1]);
		CHECK_INVEN_UPDATE_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[2]);
		CHECK_INVEN_UPDATE_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[3]);
		CHECK_INVEN_UPDATE_ITEM(m_CHAR_PTM, QPVP_ITEM_LIST[4]);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_QPVP, USP_RZ_QPVP_REWARD_MAIL)
	{
		for (int i = 0; i < MAX_ITEM_COUNT; ++i)
			QPVP_ITEM_LIST[i].IUID = i + 1;

		const int64 MAIL_UID = QPVP_REWARD_ITEM_MAIL();
		
		CHECK_CHAR(m_CSN, m_CHAR_PTM, m_MONEY, m_EXP, m_LEV);

		CHECK_MAIL(MAIL_UID);		
		CHECK_MAIL_ITEM(MAIL_UID, QPVP_ITEM_LIST[0], 0);
		CHECK_MAIL_ITEM(MAIL_UID, QPVP_ITEM_LIST[1], 1);
		CHECK_MAIL_ITEM(MAIL_UID, QPVP_ITEM_LIST[2], 2);
		CHECK_MAIL_ITEM(MAIL_UID, QPVP_ITEM_LIST[3], 3);
		CHECK_MAIL_ITEM(MAIL_UID, QPVP_ITEM_LIST[4], 4);
	}
}