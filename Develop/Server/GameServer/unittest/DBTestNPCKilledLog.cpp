#include "stdafx.h"
#include "DBTestGlobal.h"


SUITE(DBTestNpcKilledLog)
{
	LOGDB_TEST(USP_RZ_KILL_NPC_LOG)
	{
		const int		WORLD		= 1;
		const wstring	REG_DATE	= L"'2010-12-7 11:30:0'";
		const int		NPC_ID		= 3;

		GNPC_KILLER PLAYER[10];

		for (int i = 0; i < GNPC_KILLER::MAX_COUNT; ++i)
			PLAYER[i] = GNPC_KILLER(i+1, i+2, i+3, i+4, i+5);			
		
		UTestDB.Execute(L"{CALL dbo.USP_RZ_KILL_NPC_LOG (%d, %s, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d, %I64d, %I64d, %d, %d, %d)}"
			, WORLD, REG_DATE.c_str(), NPC_ID
			, PLAYER[0].m_nAID, PLAYER[0].m_nCID, PLAYER[0].m_nCharPtm, PLAYER[0].m_nDeltaCharPtm, PLAYER[0].m_nLevel 
			, PLAYER[1].m_nAID, PLAYER[1].m_nCID, PLAYER[1].m_nCharPtm, PLAYER[1].m_nDeltaCharPtm, PLAYER[1].m_nLevel
			, PLAYER[2].m_nAID, PLAYER[2].m_nCID, PLAYER[2].m_nCharPtm, PLAYER[2].m_nDeltaCharPtm, PLAYER[2].m_nLevel
			, PLAYER[3].m_nAID, PLAYER[3].m_nCID, PLAYER[3].m_nCharPtm, PLAYER[3].m_nDeltaCharPtm, PLAYER[3].m_nLevel
			, PLAYER[4].m_nAID, PLAYER[4].m_nCID, PLAYER[4].m_nCharPtm, PLAYER[4].m_nDeltaCharPtm, PLAYER[4].m_nLevel);

		for (int i = 0; i < GNPC_KILLER::MAX_COUNT; ++i)
		{
			TDBRecordSet rs;
			UTestDB.Execute(rs, L"SELECT CHAR_PTM, DELTA_CHAR_PTM, LEV FROM dbo.RZ_CHAR_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = 1801"
				, PLAYER[i].m_nAID, WORLD, PLAYER[i].m_nCID);

			CHECK_EQUAL(PLAYER[i].m_nCharPtm, rs.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(PLAYER[i].m_nDeltaCharPtm, rs.Field(L"DELTA_CHAR_PTM").AsInt());
			CHECK_EQUAL(PLAYER[i].m_nLevel, rs.Field(L"LEV").AsByte());

			rs.Close();
		}
	}
}