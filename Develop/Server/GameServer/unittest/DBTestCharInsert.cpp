#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GConfig.h"

SUITE(DBTEST_CHAR_CREATE)
{
	class ITEM_INFO
	{
	public :
		ITEM_INFO(int ITEM_ID, uint8 MAX_DURA)
			: m_ITEM_ID(ITEM_ID), m_MAX_DURA(MAX_DURA)
		{

		}

		void Check(int64 CHAR_ID, int SLOT_TYPE, int SLOT_ID, int16 STACK_AMT = 1, int COLOR = -1)
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT STACK_AMT, DURA, MAX_DURA, COLOR, CHAR_PTM FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = %d AND SLOT_ID = %d;"
				, CHAR_ID, SLOT_TYPE, SLOT_ID);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(STACK_AMT, rs.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(m_MAX_DURA, rs.Field(L"DURA").AsByte());
			CHECK_EQUAL(m_MAX_DURA, rs.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(COLOR, rs.Field(L"COLOR").AsInt());
			rs.Close();
		}

		int		m_ITEM_ID;
		uint8	m_MAX_DURA;
	};

	class FIXTURE
	{
	public :
		FIXTURE() :
		   m_ITEM_1(1, 1), m_ITEM_2(2, 1), m_ITEM_4(4, 1), m_ITEM_5(5, 1), m_ITEM_7(7, 1), m_ITEM_8(8, 1)
			   , m_ITEM_9002(9002, 1), m_ITEM_9003(9003, 1), m_ITEM_9005(9005, 1)
			   , m_ITEM_9102(9102, 1), m_ITEM_9103(9103, 1), m_ITEM_9105(9105, 1)
			   , m_ITEM_9202(9202, 1), m_ITEM_9203(9203, 1), m_ITEM_9205(9205, 1)
		   {
			   m_ACCN_ID			= 1;
			   m_NAME				= L"TEST";
			   m_LEV				= 2;
			   m_EXP				= 3;
			   m_MONEY				= 4;
			   m_RACE				= 2;
			   m_SEX				= 1;
			   m_FEAT_HAIR			= 5;
			   m_FEAT_FACE			= 6;
			   m_FEAT_HAIR_COLOR	= 7;
			   m_FEAT_SKIN_COLOR	= 8;
				m_FEAT_EYE_COLOR	= 9;
			   m_TATOO				= 18;
			   m_TATOO_POS_X		= 19;;
			   m_TATOO_POS_Y		= 20;
			   m_TATOO_SCALE		= 21;
			   m_MAKEUP				= 22;
			   m_REMAIN_TP			= 9;
			   m_HP					= 10;
			   m_EN					= 11;
			   m_STA				= 12;
			   m_FATIGUE			= 13;
			   m_SOUL_BINDING_ID	= 14;
			   m_INN_ROOM_ID		= 15;
			   m_PRI_PALETTE_ID		= 16;
			   m_SEC_PALETTE_ID		= 17;			   

			   InsertItemInfo(m_ITEM_1.m_ITEM_ID, m_ITEM_1.m_MAX_DURA);
			   InsertItemInfo(m_ITEM_2.m_ITEM_ID, m_ITEM_2.m_MAX_DURA);
			   InsertItemInfo(m_ITEM_4.m_ITEM_ID, m_ITEM_4.m_MAX_DURA);
			   InsertItemInfo(m_ITEM_5.m_ITEM_ID, m_ITEM_5.m_MAX_DURA);
			   InsertItemInfo(m_ITEM_7.m_ITEM_ID, m_ITEM_7.m_MAX_DURA);
			   InsertItemInfo(m_ITEM_8.m_ITEM_ID, m_ITEM_8.m_MAX_DURA);

			   InsertItemInfo(m_ITEM_9002.m_ITEM_ID, m_ITEM_9002.m_MAX_DURA);
			   InsertItemInfo(m_ITEM_9003.m_ITEM_ID, m_ITEM_9003.m_MAX_DURA);
			   InsertItemInfo(m_ITEM_9005.m_ITEM_ID, m_ITEM_9005.m_MAX_DURA);

			   InsertItemInfo(m_ITEM_9102.m_ITEM_ID, m_ITEM_9102.m_MAX_DURA);
			   InsertItemInfo(m_ITEM_9103.m_ITEM_ID, m_ITEM_9103.m_MAX_DURA);
			   InsertItemInfo(m_ITEM_9105.m_ITEM_ID, m_ITEM_9105.m_MAX_DURA);

			   InsertItemInfo(m_ITEM_9202.m_ITEM_ID, m_ITEM_9202.m_MAX_DURA);
			   InsertItemInfo(m_ITEM_9203.m_ITEM_ID, m_ITEM_9203.m_MAX_DURA);
			   InsertItemInfo(m_ITEM_9205.m_ITEM_ID, m_ITEM_9205.m_MAX_DURA);
		   }

		int64 CharInsert(int STYLE, int DEFENCE_ID, int DEFENCE_COLOR)
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"{CALL dbo.USP_RZ_CHAR_INSERT (\
								   %d, %I64d, N'%s'\
								 , %d, %d, %d, %d, %d\
								 , %d, %d, %d, %d, %d\
								 , %d, %d, %d, %d, %d\
								 , %d, %d, %d, %d, %d, %d\
								 , %d, %d, %d, %d, %d, %d)}"
								 , GConfig::m_nMyWorldID, m_ACCN_ID, m_NAME.c_str()
								 , m_LEV, m_EXP, m_MONEY, m_RACE, m_SEX
								 , m_FEAT_HAIR, m_FEAT_FACE, m_FEAT_HAIR_COLOR, m_FEAT_SKIN_COLOR, m_FEAT_EYE_COLOR
								 , m_TATOO, m_TATOO_POS_X, m_TATOO_POS_Y, m_TATOO_SCALE, m_MAKEUP
								 , m_REMAIN_TP, m_HP, m_EN, m_STA, m_FATIGUE, m_SOUL_BINDING_ID
								 , m_INN_ROOM_ID, m_PRI_PALETTE_ID, m_SEC_PALETTE_ID, STYLE, DEFENCE_ID, DEFENCE_COLOR);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			const int64 CHAR_ID = rs.Field(L"CHAR_ID").AsInt64();
			CHECK(0 < CHAR_ID);
			rs.Close();

			return CHAR_ID;
		}

		void CheckCharacter(int64 CHAR_ID)
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT NAME, LEV, EXP, MONEY, RACE, SEX, FEAT_HAIR, FEAT_FACE, FEAT_HAIR_COLOR, FEAT_SKIN_COLOR, FEAT_EYE_COLOR\
								 , TATOO, TATOO_POS_X, TATOO_POS_Y, TATOO_SCALE, MAKEUP\
								 , REMAIN_TP\
								 , HP, EN, STA, FATIGUE, SOUL_BINDING_ID, INN_ROOM_ID, PRI_PALETTE_ID, SEC_PALETTE_ID \
								 FROM dbo.RZ_CHARACTER \
								 WHERE WORLD_ID = %d AND ACCN_ID = %I64d AND CHAR_ID = %I64d;"
								 , GConfig::m_nMyWorldID, m_ACCN_ID, CHAR_ID);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK(m_NAME == rs.Field(L"NAME").AsWString());
			CHECK_EQUAL(m_LEV, rs.Field(L"LEV").AsByte());
			CHECK_EQUAL(m_EXP, rs.Field(L"EXP").AsInt());
			CHECK_EQUAL(m_MONEY, rs.Field(L"MONEY").AsInt());
			CHECK_EQUAL(m_RACE, rs.Field(L"RACE").AsByte());
			CHECK_EQUAL(m_SEX, rs.Field(L"SEX").AsByte());
			CHECK_EQUAL(m_FEAT_HAIR, rs.Field(L"FEAT_HAIR").AsByte());
			CHECK_EQUAL(m_FEAT_FACE, rs.Field(L"FEAT_FACE").AsByte());
			CHECK_EQUAL(m_FEAT_HAIR_COLOR, rs.Field(L"FEAT_HAIR_COLOR").AsShort());
			CHECK_EQUAL(m_FEAT_SKIN_COLOR, rs.Field(L"FEAT_SKIN_COLOR").AsShort());
			CHECK_EQUAL(m_FEAT_EYE_COLOR, rs.Field(L"FEAT_EYE_COLOR").AsShort());
			CHECK_EQUAL(m_TATOO, rs.Field(L"TATOO").AsByte());
			CHECK_EQUAL(m_TATOO_POS_X, rs.Field(L"TATOO_POS_X").AsShort());
			CHECK_EQUAL(m_TATOO_POS_Y, rs.Field(L"TATOO_POS_Y").AsShort());
			CHECK_EQUAL(m_TATOO_SCALE, rs.Field(L"TATOO_SCALE").AsByte());
			CHECK_EQUAL(m_MAKEUP, rs.Field(L"MAKEUP").AsByte());
			CHECK_EQUAL(m_REMAIN_TP, rs.Field(L"REMAIN_TP").AsShort());
			CHECK_EQUAL(m_HP, rs.Field(L"HP").AsInt());
			CHECK_EQUAL(m_EN, rs.Field(L"EN").AsInt());
			CHECK_EQUAL(m_STA, rs.Field(L"STA").AsInt());
			CHECK_EQUAL(m_FATIGUE, rs.Field(L"FATIGUE").AsInt());
			CHECK_EQUAL(m_SOUL_BINDING_ID, rs.Field(L"SOUL_BINDING_ID").AsInt());
			CHECK_EQUAL(m_INN_ROOM_ID, rs.Field(L"INN_ROOM_ID").AsInt());
			CHECK_EQUAL(m_PRI_PALETTE_ID, rs.Field(L"PRI_PALETTE_ID").AsInt());
			CHECK_EQUAL(m_SEC_PALETTE_ID, rs.Field(L"SEC_PALETTE_ID").AsInt());
			rs.Close();
		}

		void CheckDefenderStyle(int64 CHAR_ID)
		{
			m_ITEM_4.Check(CHAR_ID, 1, 0);
			m_ITEM_8.Check(CHAR_ID, 1, 1);
			m_ITEM_5.Check(CHAR_ID, 1, 2);
			m_ITEM_7.Check(CHAR_ID, 2, 10);
			m_ITEM_1.Check(CHAR_ID, 2, 11);
			m_ITEM_7.Check(CHAR_ID, 2, 12);
			m_ITEM_2.Check(CHAR_ID, 2, 13);
		}

		void CheckBerserkerStyle(int64 CHAR_ID)
		{
			m_ITEM_4.Check(CHAR_ID, 2, 11);
			m_ITEM_8.Check(CHAR_ID, 2, 13);
			m_ITEM_1.Check(CHAR_ID, 1, 0);
			m_ITEM_2.Check(CHAR_ID, 1, 1);
			m_ITEM_5.Check(CHAR_ID, 1, 2);
			m_ITEM_7.Check(CHAR_ID, 1, 3);
		}

		void CheckClericStyle(int64 CHAR_ID)
		{
			m_ITEM_5.Check(CHAR_ID, 2, 11);
			m_ITEM_7.Check(CHAR_ID, 2, 12);
			m_ITEM_2.Check(CHAR_ID, 2, 13);
			m_ITEM_1.Check(CHAR_ID, 1, 0);
			m_ITEM_4.Check(CHAR_ID, 1, 1);
			m_ITEM_8.Check(CHAR_ID, 1, 2);
		}

		void CheckDefence00(int64 CHAR_ID, int COLOR)
		{
			m_ITEM_9002.Check(CHAR_ID, 2, 4, 1, COLOR);
			m_ITEM_9003.Check(CHAR_ID, 2, 5, 1, COLOR);
			m_ITEM_9005.Check(CHAR_ID, 2, 3, 1, COLOR);
		}

		void CheckDefence01(int64 CHAR_ID, int COLOR)
		{
			m_ITEM_9102.Check(CHAR_ID, 2, 4, 1, COLOR);
			m_ITEM_9103.Check(CHAR_ID, 2, 5, 1, COLOR);
			m_ITEM_9105.Check(CHAR_ID, 2, 3, 1, COLOR);
		}

		void CheckDefence02(int64 CHAR_ID, int COLOR)
		{
			m_ITEM_9202.Check(CHAR_ID, 2, 4, 1, COLOR);
			m_ITEM_9203.Check(CHAR_ID, 2, 5, 1, COLOR);
			m_ITEM_9205.Check(CHAR_ID, 2, 3, 1, COLOR);
		}

		void CheckDefenderStyleTalent(int64 CHAR_ID)
		{

			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT TALENT_ID FROM dbo.RZ_TALENT WHERE WORLD_ID = %d AND ACCN_ID = %I64d AND CHAR_ID = %I64d;"
				, GConfig::m_nMyWorldID, m_ACCN_ID, CHAR_ID);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(10701, rs.Field(L"TALENT_ID").AsInt());
			rs.Close();
		}

		void CheckBerserkerStyleTalent(int64 CHAR_ID)
		{

			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT TALENT_ID FROM dbo.RZ_TALENT WHERE WORLD_ID = %d AND ACCN_ID = %I64d AND CHAR_ID = %I64d;"
				, GConfig::m_nMyWorldID, m_ACCN_ID, CHAR_ID);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(21901, rs.Field(L"TALENT_ID").AsInt());
			rs.Close();
		}

		void CheckClericStyleTalent(int64 CHAR_ID)
		{

			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT TALENT_ID FROM dbo.RZ_TALENT WHERE WORLD_ID = %d AND ACCN_ID = %I64d AND CHAR_ID = %I64d;"
				, GConfig::m_nMyWorldID, m_ACCN_ID, CHAR_ID);
			CHECK_EQUAL(1, rs.GetFetchedRowCount());
			CHECK_EQUAL(62001 , rs.Field(L"TALENT_ID").AsInt());
			rs.Close();
		}

	private :
		void InsertItemInfo(int ITEM_ID, uint8 MAX_DURA)
		{
			UTestDB.Execute(L"INSERT INTO dbo.RZ_ITEM_INFO (ITEM_ID, MAX_DURA) VALUES (%d, %d);"
				, ITEM_ID, MAX_DURA);
		}	

	public :
		int64	m_ACCN_ID;
		wstring m_NAME;
		uint8	m_LEV;
		int		m_EXP;
		int		m_MONEY;
		uint8	m_RACE;
		uint8	m_SEX;
		uint8	m_FEAT_HAIR;
		uint8	m_FEAT_FACE;
		int		m_FEAT_HAIR_COLOR;
		int		m_FEAT_SKIN_COLOR;
		int		m_FEAT_EYE_COLOR;
		uint8	m_TATOO;
		short	m_TATOO_POS_X;
		short	m_TATOO_POS_Y;
		uint8	m_TATOO_SCALE;
		uint8	m_MAKEUP;
		int16	m_REMAIN_TP;
		int		m_HP;
		int		m_EN;
		int		m_STA;
		int		m_FATIGUE;
		int		m_SOUL_BINDING_ID;
		int		m_INN_ROOM_ID;
		int		m_PRI_PALETTE_ID;
		int		m_SEC_PALETTE_ID;		

		ITEM_INFO m_ITEM_1;
		ITEM_INFO m_ITEM_2;
		ITEM_INFO m_ITEM_4;
		ITEM_INFO m_ITEM_5;
		ITEM_INFO m_ITEM_7;
		ITEM_INFO m_ITEM_8;

		ITEM_INFO m_ITEM_9002;
		ITEM_INFO m_ITEM_9003;
		ITEM_INFO m_ITEM_9005;

		ITEM_INFO m_ITEM_9102;
		ITEM_INFO m_ITEM_9103;
		ITEM_INFO m_ITEM_9105;

		ITEM_INFO m_ITEM_9202;
		ITEM_INFO m_ITEM_9203;
		ITEM_INFO m_ITEM_9205;
	};

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_CHAR_INSERT_DEFENDER_00)
	{
		const int COLOR		= 111111;

		const int64 CHAR_ID = CharInsert(TS_DEFENDER, 0, COLOR);

		CheckCharacter(CHAR_ID);
		CheckDefenderStyle(CHAR_ID);
		CheckDefence00(CHAR_ID, COLOR);
		CheckDefenderStyleTalent(CHAR_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_CHAR_INSERT_DEFENDER_01)
	{
		const int COLOR		= 111111;

		const int64 CHAR_ID = CharInsert(TS_DEFENDER, 1, COLOR);

		CheckCharacter(CHAR_ID);
		CheckDefenderStyle(CHAR_ID);
		CheckDefence01(CHAR_ID, COLOR);
		CheckDefenderStyleTalent(CHAR_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_CHAR_INSERT_DEFENDER_02)
	{
		const int COLOR		= 111111;

		const int64 CHAR_ID = CharInsert(TS_DEFENDER, 2, COLOR);

		CheckCharacter(CHAR_ID);
		CheckDefenderStyle(CHAR_ID);
		CheckDefence02(CHAR_ID, COLOR);
		CheckDefenderStyleTalent(CHAR_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_CHAR_INSERT_BERSERKER_00)
	{
		const int COLOR		= 111111;

		const int64 CHAR_ID = CharInsert(TS_BERSERKER, 0, COLOR);

		CheckCharacter(CHAR_ID);
		CheckBerserkerStyle(CHAR_ID);
		CheckDefence00(CHAR_ID, COLOR);
		CheckBerserkerStyleTalent(CHAR_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_CHAR_INSERT_BERSERKER_01)
	{
		const int COLOR		= 111111;

		const int64 CHAR_ID = CharInsert(TS_BERSERKER, 1, COLOR);

		CheckCharacter(CHAR_ID);
		CheckBerserkerStyle(CHAR_ID);
		CheckDefence01(CHAR_ID, COLOR);
		CheckBerserkerStyleTalent(CHAR_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_CHAR_INSERT_BERSERKER_02)
	{
		const int COLOR		= 111111;

		const int64 CHAR_ID = CharInsert(TS_BERSERKER, 2, COLOR);

		CheckCharacter(CHAR_ID);
		CheckBerserkerStyle(CHAR_ID);
		CheckDefence02(CHAR_ID, COLOR);
		CheckBerserkerStyleTalent(CHAR_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_CHAR_INSERT_CLERIC_00)
	{
		const int COLOR		= 111111;

		const int64 CHAR_ID = CharInsert(TS_CLERIC, 0, COLOR);

		CheckCharacter(CHAR_ID);
		CheckClericStyle(CHAR_ID);
		CheckDefence00(CHAR_ID, COLOR);
		CheckClericStyleTalent(CHAR_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_CHAR_INSERT_CLERIC_01)
	{
		const int COLOR		= 111111;

		const int64 CHAR_ID = CharInsert(TS_CLERIC, 0, COLOR);

		CheckCharacter(CHAR_ID);
		CheckClericStyle(CHAR_ID);
		CheckDefence01(CHAR_ID, COLOR);
		CheckClericStyleTalent(CHAR_ID);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE, USP_RZ_CHAR_INSERT_CLERIC_02)
	{
		const int COLOR		= 111111;

		const int64 CHAR_ID = CharInsert(TS_CLERIC, 0, COLOR);

		CheckCharacter(CHAR_ID);
		CheckClericStyle(CHAR_ID);
		CheckDefence02(CHAR_ID, COLOR);
		CheckClericStyleTalent(CHAR_ID);
	}
}