#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GDB_CODE.h"

SUITE(ENCHANT_LOG)
{
	class FIXTURE
	{
	public :
		void CHECK_ITEM_LOG(const int CODE, const int64 IUID, const int ITEM_ID, int16 STACK_AMT, int16 DELTA_STACK_AMT, const int64 REF_IUID, const bool REF_IUID_NULL)
		{
			TDBRecordSet rsEnch;
			UTestDB.Execute(rsEnch, L"SELECT CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_MONEY, IUID, ITEM_ID, CUR_STACK_AMT, DELTA_STACK_AMT, REF_IUID\
									 FROM dbo.RZ_ITEM_LOG\
									 WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND OWNER_ID = %I64d AND CODE = %d"
									 , m_GSN, m_WORLD, m_CSN, CODE);
			CHECK_EQUAL(1,rsEnch.GetFetchedRowCount());
			CHECK_EQUAL(m_CHAR_PTM, rsEnch.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(m_DELTA_CHAR_PTM, rsEnch.Field(L"DELTA_CHAR_PTM").AsInt());
			CHECK_EQUAL(m_LEV, rsEnch.Field(L"LEV").AsByte());
			CHECK_EQUAL(IUID, rsEnch.Field(L"IUID").AsInt64());
			CHECK_EQUAL(ITEM_ID, rsEnch.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(STACK_AMT, rsEnch.Field(L"CUR_STACK_AMT").AsShort());
			CHECK_EQUAL(DELTA_STACK_AMT, rsEnch.Field(L"DELTA_STACK_AMT").AsShort());
			CHECK_EQUAL(REF_IUID_NULL, rsEnch.Field(L"REF_IUID").IsNull());
			CHECK_EQUAL(REF_IUID, rsEnch.Field(L"REF_IUID").AsInt64());
			rsEnch.Close();
		}

	public :
		static const int64	m_GSN					= 1;
		static const int	m_WORLD					= 2;
		static const int64	m_CSN					= 3;
		static const int	m_CHAR_PTM				= 4;
		static const int	m_DELTA_CHAR_PTM		= 5;
		static const uint8	m_LEV					= 6;
		static const int	m_CUR_MONEY				= 7;
		static const int64	m_ENCH_IUID				= 7;
		static const int	m_ENCH_ITEM_ID			= 8;
		static const int64	m_STONE_IUID			= 9;
		static const int	m_STONE_ITEM_ID			= 10;
		static const int64	m_AGENT_IUID			= 11;
		static const int	m_AGENT_ITEM_ID			= 12;
		static const int16	m_AGENT_STACK_AMT		= 13;
		static const int16	m_AGENT_DELTA_STACK_AMT = 14;
	};
	
	LOGDB_TEST_FIXTURE(FIXTURE, USP_RZ_ITEM_ENCH_LOG)
	{
		UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_ENCH_LOG (%I64d, %d, %I64d, %d, %d, %d, %d, %d, %I64d, %d, %I64d, %d, %I64d, %d, %d, %d)}"
			, m_GSN, m_WORLD, m_CSN, GDB_CODE::CD_L_ENCH_SUCCESS, m_CHAR_PTM, m_DELTA_CHAR_PTM, m_LEV, m_CUR_MONEY, m_ENCH_IUID, m_ENCH_ITEM_ID, m_STONE_IUID, m_STONE_ITEM_ID
			, m_AGENT_IUID, m_AGENT_ITEM_ID, m_AGENT_STACK_AMT, m_AGENT_DELTA_STACK_AMT);

		
		CHECK_ITEM_LOG(GDB_CODE::CD_L_ENCH_SUCCESS, m_ENCH_IUID, m_ENCH_ITEM_ID, 0, 0, 0, true);
		CHECK_ITEM_LOG(GDB_CODE::CD_L_ENCH_STONE, m_STONE_IUID, m_STONE_ITEM_ID, 0, -1, m_ENCH_IUID, false);
		CHECK_ITEM_LOG(GDB_CODE::CD_L_ENCH_AGENT, m_AGENT_IUID, m_AGENT_ITEM_ID, m_AGENT_STACK_AMT, m_AGENT_DELTA_STACK_AMT, m_ENCH_IUID, false);
	}
}