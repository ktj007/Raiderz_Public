#include "stdafx.h"
#include "DBTestGlobal.h"
#include "GDBTaskDataTrade.h"


SUITE(DBTEST_CHAR_TRADE_LOG)
{
	const int WORLD = 1;

	class TRADE_CHAR_LOG
	{
	public :
		TRADE_CHAR_LOG(const int64 GSN, const int CSN, const int CHAR_PTM, int DELTA_CHAR_PTM, int LEV, const int EXP, const int MONEY, const int DELTA_MONEY
			, const int FIELD_ID, const float POS_X, const float POS_Y, const float POS_Z)
			: m_GSN(GSN), m_CSN(CSN), m_CHAR_PTM(CHAR_PTM), m_DELTA_CHAR_PTM(DELTA_CHAR_PTM), m_LEV(LEV), m_EXP(EXP), m_MONEY(MONEY), m_DELTA_MONEY(DELTA_MONEY)
		, m_FIELD_ID(FIELD_ID), m_POS_X(POS_X), m_POS_Y(POS_Y), m_POS_Z(POS_Z) {}

		int64	m_GSN;
		int64	m_CSN;
		int		m_CHAR_PTM;
		int		m_DELTA_CHAR_PTM;
		int		m_LEV;
		int		m_EXP;
		int		m_MONEY;
		int		m_DELTA_MONEY;

		int		m_FIELD_ID;
		float	m_POS_X;
		float	m_POS_Y;
		float	m_POS_Z;
	};


	void CHECK_CHAR_LOG(TRADE_CHAR_LOG& ch1, TRADE_CHAR_LOG& ch2)
	{
		wchar_t szSQL[] = L"SELECT CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_MONEY, FIELD_ID, POS_X, POS_Y, POS_Z, REF_ACCN_ID, REF_OWNER_ID \
						   FROM dbo.RZ_ITEM_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND OWNER_ID = %I64d AND CODE = %d AND DELTA_MONEY = %d;";

		TDBRecordSet rs1;
		UTestDB.Execute(rs1, szSQL, ch1.m_GSN, WORLD, ch1.m_CSN, 901, -ch1.m_DELTA_MONEY);
		CHECK_EQUAL(1, rs1.GetFetchedRowCount());
		CHECK_EQUAL(ch1.m_CHAR_PTM, rs1.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(ch1.m_DELTA_CHAR_PTM, rs1.Field(L"DELTA_CHAR_PTM").AsInt());
		CHECK_EQUAL(ch1.m_LEV, rs1.Field(L"LEV").AsInt());
		CHECK_EQUAL(ch1.m_MONEY - ch2.m_DELTA_MONEY, rs1.Field(L"CUR_MONEY").AsInt());
		CHECK_EQUAL(ch1.m_FIELD_ID, rs1.Field(L"FIELD_ID").AsInt());
		CHECK_EQUAL(ch1.m_POS_X, rs1.Field(L"POS_X").AsFloat());
		CHECK_EQUAL(ch1.m_POS_Y, rs1.Field(L"POS_Y").AsFloat());
		CHECK_EQUAL(ch1.m_POS_Z, rs1.Field(L"POS_Z").AsFloat());
		CHECK_EQUAL(ch2.m_GSN, rs1.Field(L"REF_ACCN_ID").AsInt64());
		CHECK_EQUAL(ch2.m_CSN, rs1.Field(L"REF_OWNER_ID").AsInt64());
		rs1.Close();

		TDBRecordSet rs2;
		UTestDB.Execute(rs2, szSQL, ch1.m_GSN, WORLD, ch1.m_CSN, 902, ch2.m_DELTA_MONEY);
		CHECK_EQUAL(1, rs2.GetFetchedRowCount());
		CHECK_EQUAL(ch1.m_CHAR_PTM, rs2.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(ch1.m_DELTA_CHAR_PTM, rs2.Field(L"DELTA_CHAR_PTM").AsInt());
		CHECK_EQUAL(ch1.m_LEV, rs2.Field(L"LEV").AsInt());
		CHECK_EQUAL(ch1.m_MONEY, rs2.Field(L"CUR_MONEY").AsInt());
		CHECK_EQUAL(ch1.m_FIELD_ID, rs2.Field(L"FIELD_ID").AsInt());
		CHECK_EQUAL(ch1.m_POS_X, rs2.Field(L"POS_X").AsFloat());
		CHECK_EQUAL(ch1.m_POS_Y, rs2.Field(L"POS_Y").AsFloat());
		CHECK_EQUAL(ch1.m_POS_Z, rs2.Field(L"POS_Z").AsFloat());
		CHECK_EQUAL(ch2.m_GSN, rs2.Field(L"REF_ACCN_ID").AsInt64());
		CHECK_EQUAL(ch2.m_CSN, rs2.Field(L"REF_OWNER_ID").AsInt64());
		rs1.Close();
	}

	void CHECK_ITEM_LOG(TRADE_CHAR_LOG& ch, TRADE_CHAR_LOG& to_ch, GTRADE_ITEM_LOG& item)
	{
		wchar_t szSQL[] = L"SELECT ITEM_ID, CHAR_PTM, DELTA_CHAR_PTM, LEV, CUR_STACK_AMT, DELTA_STACK_AMT, USAGE_PERIOD, REF_ACCN_ID, REF_OWNER_ID, REF_IUID \
						   FROM dbo.RZ_ITEM_LOG WHERE ACCN_ID = %I64d AND WORLD_ID = %d AND OWNER_ID = %I64d AND CODE = %d AND IUID = %I64d;";

		TDBRecordSet rs1;
		UTestDB.Execute(rs1, szSQL, ch.m_GSN, WORLD, ch.m_CSN, 903, item.m_nIUID);
		CHECK_EQUAL(1, rs1.GetFetchedRowCount());
		CHECK_EQUAL(item.m_nItemID, rs1.Field(L"ITEM_ID").AsInt());
		CHECK_EQUAL(ch.m_CHAR_PTM, rs1.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(ch.m_DELTA_CHAR_PTM, rs1.Field(L"DELTA_CHAR_PTM").AsInt());
		CHECK_EQUAL(ch.m_LEV, rs1.Field(L"LEV").AsInt());
		CHECK_EQUAL(item.m_nStackAmt, rs1.Field(L"CUR_STACK_AMT").AsShort());
		CHECK_EQUAL(-item.m_nModStackAmt, rs1.Field(L"DELTA_STACK_AMT").AsShort());
		CHECK_EQUAL(item.m_nUsagePeriod, rs1.Field(L"USAGE_PERIOD").AsInt());
		CHECK_EQUAL(to_ch.m_GSN, rs1.Field(L"REF_ACCN_ID").AsInt64());
		CHECK_EQUAL(to_ch.m_CSN, rs1.Field(L"REF_OWNER_ID").AsInt64());
		CHECK_EQUAL(item.m_nToIUID, rs1.Field(L"REF_IUID").AsInt64());
		rs1.Close();

		TDBRecordSet rs2;
		UTestDB.Execute(rs2, szSQL, to_ch.m_GSN, WORLD, to_ch.m_CSN, 904, item.m_nToIUID);
		CHECK_EQUAL(1, rs2.GetFetchedRowCount());
		CHECK_EQUAL(item.m_nItemID, rs2.Field(L"ITEM_ID").AsInt());
		CHECK_EQUAL(to_ch.m_CHAR_PTM, rs2.Field(L"CHAR_PTM").AsInt());
		CHECK_EQUAL(to_ch.m_DELTA_CHAR_PTM, rs2.Field(L"DELTA_CHAR_PTM").AsInt());
		CHECK_EQUAL(to_ch.m_LEV, rs2.Field(L"LEV").AsInt());
		CHECK_EQUAL(item.m_nModStackAmt, rs2.Field(L"CUR_STACK_AMT").AsShort());
		CHECK_EQUAL(item.m_nModStackAmt, rs2.Field(L"DELTA_STACK_AMT").AsShort());
		CHECK_EQUAL(item.m_nUsagePeriod, rs2.Field(L"USAGE_PERIOD").AsInt());
		CHECK_EQUAL(ch.m_GSN, rs2.Field(L"REF_ACCN_ID").AsInt64());
		CHECK_EQUAL(ch.m_CSN, rs2.Field(L"REF_OWNER_ID").AsInt64());
		CHECK_EQUAL(item.m_nIUID, rs2.Field(L"REF_IUID").AsInt64());
		rs2.Close();
	}

	LOGDB_TEST(USP_RZ_CHAR_TRADE_LOG)
	{
		/*
		 * 8 : 2 => 11 : 2
		 * 17 : 3 => 19 : 3
		 */
		TRADE_CHAR_LOG tch_1(1,	 2,	 3,	 4, 5, 6,	 11, 2,	1,  1.0f, 2.0f, 3.0f);
		TRADE_CHAR_LOG tch_2(11, 12, 13, 14, 15, 16, 19, 3, 11, 1.1f, 2.1f, 3.1f);

		GTRADE_ITEM_LOG ti_1[5];
		GTRADE_ITEM_LOG ti_2[5];

		for (int i = 0; i < 5; ++i)
		{
			ti_1[i].Set(i, i + 1, i + 10000, i + 20, i + 5, i + 1, i + 30);
			ti_2[i].Set(i + 10, i + 100, i + 1000000, i + 30, i + 15, i + 11, i + 40);
		}

		UTestDB.Execute(L"{CALL dbo.USP_RZ_TRADE_LOG (%d \
						 , %I64d, %I64d, %d, %d, %d, %d, %d \
						 , %d, %f, %f, %f \
						 , %d, %I64d, %I64d, %d, %d, %d \
						 , %d, %I64d, %I64d, %d, %d, %d \
						 , %d, %I64d, %I64d, %d, %d, %d \
						 , %d, %I64d, %I64d, %d, %d, %d \
						 , %d, %I64d, %I64d, %d, %d, %d \
						 , %I64d, %I64d, %d, %d, %d, %d, %d \
						 , %d, %f, %f, %f \
						 , %d, %I64d, %I64d, %d, %d, %d \
						 , %d, %I64d, %I64d, %d, %d, %d \
						 , %d, %I64d, %I64d, %d, %d, %d \
						 , %d, %I64d, %I64d, %d, %d, %d \
						 , %d, %I64d, %I64d, %d, %d, %d)}"
						 , WORLD
						 , tch_1.m_GSN, tch_1.m_CSN, tch_1.m_CHAR_PTM, tch_1.m_DELTA_CHAR_PTM, tch_1.m_LEV, tch_1.m_MONEY, tch_1.m_DELTA_MONEY
						 , tch_1.m_FIELD_ID, tch_1.m_POS_X, tch_1.m_POS_Y, tch_1.m_POS_Z
						 , ti_1[0].m_nItemID, ti_1[0].m_nIUID, ti_1[0].m_nToIUID, ti_1[0].m_nStackAmt, ti_1[0].m_nModStackAmt, ti_1[0].m_nUsagePeriod
						 , ti_1[1].m_nItemID, ti_1[1].m_nIUID, ti_1[1].m_nToIUID, ti_1[1].m_nStackAmt, ti_1[1].m_nModStackAmt, ti_1[1].m_nUsagePeriod
						 , ti_1[2].m_nItemID, ti_1[2].m_nIUID, ti_1[2].m_nToIUID, ti_1[2].m_nStackAmt, ti_1[2].m_nModStackAmt, ti_1[2].m_nUsagePeriod
						 , ti_1[3].m_nItemID, ti_1[3].m_nIUID, ti_1[3].m_nToIUID, ti_1[3].m_nStackAmt, ti_1[3].m_nModStackAmt, ti_1[3].m_nUsagePeriod
						 , ti_1[4].m_nItemID, ti_1[4].m_nIUID, ti_1[4].m_nToIUID, ti_1[4].m_nStackAmt, ti_1[4].m_nModStackAmt, ti_1[4].m_nUsagePeriod
						 , tch_2.m_GSN, tch_2.m_CSN, tch_2.m_CHAR_PTM, tch_2.m_DELTA_CHAR_PTM, tch_2.m_LEV, tch_2.m_MONEY, tch_2.m_DELTA_MONEY
						 , tch_2.m_FIELD_ID, tch_2.m_POS_X, tch_2.m_POS_Y, tch_2.m_POS_Z
						 , ti_2[0].m_nItemID, ti_2[0].m_nIUID, ti_2[0].m_nToIUID, ti_2[0].m_nStackAmt, ti_2[0].m_nModStackAmt, ti_2[0].m_nUsagePeriod
						 , ti_2[1].m_nItemID, ti_2[1].m_nIUID, ti_2[1].m_nToIUID, ti_2[1].m_nStackAmt, ti_2[1].m_nModStackAmt, ti_2[1].m_nUsagePeriod
						 , ti_2[2].m_nItemID, ti_2[2].m_nIUID, ti_2[2].m_nToIUID, ti_2[2].m_nStackAmt, ti_2[2].m_nModStackAmt, ti_2[2].m_nUsagePeriod
						 , ti_2[3].m_nItemID, ti_2[3].m_nIUID, ti_2[3].m_nToIUID, ti_2[3].m_nStackAmt, ti_2[3].m_nModStackAmt, ti_2[3].m_nUsagePeriod
						 , ti_2[4].m_nItemID, ti_2[4].m_nIUID, ti_2[4].m_nToIUID, ti_2[4].m_nStackAmt, ti_2[4].m_nModStackAmt, ti_2[4].m_nUsagePeriod);

		CHECK_CHAR_LOG(tch_1, tch_2);
		CHECK_CHAR_LOG(tch_2, tch_1);

		for (int i = 0; i < 5; ++i)
		{
			CHECK_ITEM_LOG(tch_1, tch_2, ti_1[i]);
			CHECK_ITEM_LOG(tch_2, tch_1, ti_2[i]);
		}
	}
}
