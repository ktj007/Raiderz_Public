#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GDB_CODE.h"
#include "GConfig.h"
#include "GTimeCalculator.h"


SUITE(DBTEST_CHAR_LOG)
{
	const int64		nGSN				= 1;
	const int		nWORLD				= 2;
	const int64		nCSN				= 3;
	const int		nCODE				= 4;
	const int		nCHAR_PTM			= 5;
	const int		nDELTA_CHAR_PTM		= 6;
	const int		nLEV				= 7;
	const int		nEXP				= 12;
	const int		nDELTA_EXP			= 13;
	const int		nNPC_ID				= 14;
	const int		nDELTA_MONEY		= 15;
	const int		nFIELD_ID			= 16;
	const float		fPOS_X				= 1.0;
	const float		fPOS_Y				= 2.0;
	const float		fPOS_Z				= 3.0;

	LOGDB_TEST(USP_RZ_CHAR_LEVEL_UP_LOG)
	{
		wstring strRegDate = GTimeCalculator::GetTimeAsString(GTimeSec(0));

		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_LEVEL_UP_LOG (%I64d, %d, %I64d, %d, %s, %d, %d, %d, %d, %d, %d, %d, %f, %f, %f)}"
			, nGSN, nWORLD, nCSN, nCODE, strRegDate.c_str(), nCHAR_PTM, nDELTA_CHAR_PTM, nLEV, nEXP, nDELTA_EXP, nNPC_ID, nFIELD_ID, fPOS_X, fPOS_Y, fPOS_Z);

		TDBRecordSet rsLog;
		UTestDB.Execute(rsLog, L"SELECT REG_DATE, CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_EXP, DELTA_EXP, NPC_ID, FIELD_ID, POS_X, POS_Y, POS_Z \
								FROM dbo.RZ_CHAR_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = %d;"
								, nGSN, nWORLD, nCSN, nCODE);
		CHECK(strRegDate == wstring(L"'") + rsLog.Field(L"REG_DATE").AsWString() + wstring(L"'"));
		CHECK_EQUAL(nCHAR_PTM, rsLog.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(nDELTA_CHAR_PTM, rsLog.Field(L"DELTA_CHAR_PTM").AsInt());
		CHECK_EQUAL(nLEV, rsLog.Field(L"LEV").AsByte());
		CHECK_EQUAL(nEXP, rsLog.Field(L"CUR_EXP").AsInt());
		CHECK_EQUAL(nDELTA_EXP, rsLog.Field(L"DELTA_EXP").AsInt());
		CHECK_EQUAL(nNPC_ID, rsLog.Field(L"NPC_ID").AsInt());
		CHECK_EQUAL(nFIELD_ID, rsLog.Field(L"FIELD_ID").AsInt());
		CHECK_EQUAL(fPOS_X, rsLog.Field(L"POS_X").AsFloat());
		CHECK_EQUAL(fPOS_Y, rsLog.Field(L"POS_Y").AsFloat());
		CHECK_EQUAL(fPOS_Z, rsLog.Field(L"POS_Z").AsFloat());
		rsLog.Close();
	}

	LOGDB_TEST(USP_RZ_CHAR_DIE)
	{
		const int64 nGSN			= 1;
		const int	nWorldID		= 1;
		const int64	nCSN			= 2;
		const int	nCharPtm		= 10;
		const int	nDeltaCharPtm	= 11;
		const int	nLevel			= 2;
		const int	nNpcID			= 3;
		const int	nXP				= 123;
		const int	nFieldID		= 11;
		const float fPosX			= 10.0f;
		const float fPosY			= 11.0f;
		const float fPosZ			= 12.0f;

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_DIE_LOG (%I64d, %d, %I64d, %d, %d, %d, %d, %d, %d, %d, %f, %f, %f)}"
			, nGSN, nWorldID, nCSN, GDB_CODE::CD_L_DIE_BY_NPC, nCharPtm, nDeltaCharPtm, nLevel, nXP, nNpcID, nFieldID, fPosX, fPosY, fPosZ));

		TDBRecordSet rs1;
		CHECK(UTestDB.Execute(rs1, L"SELECT WORLD_ID, CODE, CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_EXP, NPC_ID, FIELD_ID, POS_X, POS_Y, POS_Z FROM dbo.RZ_CHAR_LOG WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d;"
			, nGSN, nCSN));

		CHECK_EQUAL(1, rs1.GetFetchedRowCount());
		CHECK_EQUAL(nCharPtm, rs1.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(nDeltaCharPtm, rs1.Field(L"DELTA_CHAR_PTM").AsInt());
		CHECK_EQUAL(nLevel, rs1.Field(L"LEV").AsInt());
		CHECK_EQUAL(nXP, rs1.Field(L"CUR_EXP").AsInt());
		CHECK_EQUAL(GDB_CODE::CD_L_DIE_BY_NPC, rs1.Field(L"CODE").AsInt());
		CHECK_EQUAL(nNpcID, rs1.Field(L"NPC_ID").AsInt());
		CHECK_EQUAL(nFieldID, rs1.Field(L"FIELD_ID").AsInt());
		CHECK_EQUAL(fPosX, rs1.Field(L"POS_X").AsFloat());
		CHECK_EQUAL(fPosY, rs1.Field(L"POS_Y").AsFloat());
		CHECK_EQUAL(fPosZ, rs1.Field(L"POS_Z").AsFloat());		
		rs1.Close();
	}

	LOGDB_TEST(USP_RZ_CHAR_PVP_DIE)
	{
		class TEST_PVP_CHAR
		{
		public :
			TEST_PVP_CHAR() {}
			TEST_PVP_CHAR(int64 GSN, int64 CSN, int CODE, int CHAR_PTM, int DELTA_CHAR_PTM, uint8 LEV, int CUR_EXP, int FIELD_ID, float POS_X, float POS_Y, float POS_Z)
				: m_GSN(GSN), m_CSN(CSN), m_CODE(CODE), m_CHAR_PTM(CHAR_PTM), m_DELTA_CHAR_PTM(DELTA_CHAR_PTM), m_LEV(LEV), m_CUR_EXP(CUR_EXP), m_FIELD_ID(FIELD_ID), m_POS_X(POS_X), m_POS_Y(POS_Y), m_POS_Z(POS_Z) {}

			void Check(const TEST_PVP_CHAR& ref)
			{
				TDBRecordSet rs;
				UTestDB.Execute(rs, L"SELECT CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_EXP, FIELD_ID, POS_X, POS_Y, POS_Z, REF_ACCN_ID, REF_CHAR_ID\
									 FROM dbo.RZ_CHAR_LOG \
									 WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND CHAR_ID = %I64d AND CODE = %d;"
									 , m_GSN, GConfig::m_nMyWorldID, m_CSN, m_CODE);
				CHECK_EQUAL(1, rs.GetFetchedRowCount());
				CHECK_EQUAL(m_CHAR_PTM, rs.Field(L"CHAR_PTM").AsInt());
				CHECK_EQUAL(m_DELTA_CHAR_PTM, rs.Field(L"DELTA_CHAR_PTM").AsInt());
				CHECK_EQUAL(m_LEV, rs.Field(L"LEV").AsByte());
				CHECK_EQUAL(m_CUR_EXP, rs.Field(L"CUR_EXP").AsInt());
				CHECK_EQUAL(m_FIELD_ID, rs.Field(L"FIELD_ID").AsInt());
				CHECK_EQUAL(m_POS_X, rs.Field(L"POS_X").AsFloat());
				CHECK_EQUAL(m_POS_Y, rs.Field(L"POS_Y").AsFloat());
				CHECK_EQUAL(m_POS_Z, rs.Field(L"POS_Z").AsFloat());
				CHECK_EQUAL(ref.m_GSN, rs.Field(L"REF_ACCN_ID").AsInt64());
				CHECK_EQUAL(ref.m_CSN, rs.Field(L"REF_CHAR_ID").AsInt64());
				rs.Close();
			}

		public :
			int64	m_GSN;
			int64	m_CSN;
			int		m_CODE;
			int		m_CHAR_PTM;
			int		m_DELTA_CHAR_PTM;
			uint8	m_LEV;
			int		m_CUR_EXP;
			int		m_FIELD_ID;
			float	m_POS_X;
			float	m_POS_Y;
			float	m_POS_Z;
		};

		TEST_PVP_CHAR ch(1, 2, 3, 4, 5, 6, 7, 8, 1.0, 2.0, 3.0);
		TEST_PVP_CHAR killer(2, 3, 4, 5, 6, 7, 8, 9, 2.0, 3.0, 4.0);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_CHAR_PVP_DIE_LOG (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %f, %f, %f, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %f, %f, %f)}"
			, GConfig::m_nMyWorldID
			, ch.m_GSN, ch.m_CSN, ch.m_CODE, ch.m_CHAR_PTM, ch.m_DELTA_CHAR_PTM, ch.m_LEV, ch.m_CUR_EXP, ch.m_FIELD_ID, ch.m_POS_X, ch.m_POS_Y, ch.m_POS_Z
			, killer.m_GSN, killer.m_CSN, killer.m_CODE, killer.m_CHAR_PTM, killer.m_DELTA_CHAR_PTM, killer.m_LEV, killer.m_CUR_EXP, killer.m_FIELD_ID, killer.m_POS_X, killer.m_POS_Y, killer.m_POS_Z);

		ch.Check(killer);
		killer.Check(ch);
	}
}