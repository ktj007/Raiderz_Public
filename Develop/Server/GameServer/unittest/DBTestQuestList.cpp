#include "stdafx.h"
#include "DBTestGlobal.h"
#include "DBTestHelper_Quest.h"
#include "GQuestDateCalculator.h"
#include "GConfig.h"


SUITE(DBTestQuestGetList)
{
	GAMEDB_TEST(USP_RZ_CHAR_GET_QUEST)
	{
		const uint8	nSlotID		= 0;
		const int64 nAID		= 1;
		const int64	nCID		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int	nQuestID	= DBTestHelper::InsertQuestInfo(1, L"test_quest_1");
		const int	nVar		= 1;
		const int16 nDone		= 2;
		const int16	nPro1		= 1;
		const int16	nPro2		= 2;
		const int16	nPro3		= 3;
		const int16	nPro4		= 4;
		const int16	nPro5		= 5;

		wstring strAcceptDt;
		wstring strExpiDt;

		GQuestDateCalculator::GetAccpetDt(strAcceptDt);
		GQuestDateCalculator::GetExpiDt(true, strExpiDt);
		
		DBTestHelper::InsertQuest(GConfig::m_nMyWorldID, nAID, nCID, nSlotID, nQuestID, strAcceptDt, strExpiDt);
		
		const wchar_t* szSQL = L"UPDATE dbo.RZ_QUEST SET [VAR] = %d, [STATE] = %d\
							, OBJ1_PROG = %d, OBJ2_PROG = %d, OBJ3_PROG = %d, OBJ4_PROG = %d, OBJ5_PROG = %d\
							WHERE CHAR_ID = %I64d AND QUEST_ID = %d;";

		UTestDB.Execute(szSQL, nVar, nDone, nPro1, nPro2, nPro3, nPro4, nPro5, nCID, nQuestID);

		TDBRecordSet rsQuestList;
		CHECK(UTestDB.Execute(rsQuestList, L"{CALL dbo.USP_RZ_CHAR_GET_QUEST (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, nAID, nCID));
		CHECK_EQUAL(nSlotID, rsQuestList.Field(L"SLOT_ID").AsByte());
		CHECK_EQUAL(nQuestID, rsQuestList.Field(L"QUEST_ID").AsInt());
		CHECK_EQUAL(nVar, rsQuestList.Field(L"VAR").AsInt());
		CHECK_EQUAL(nDone, rsQuestList.Field(L"STATE").AsByte());
		CHECK(strAcceptDt == rsQuestList.Field(L"ACCP_DATE").AsWString());
		CHECK(strExpiDt == rsQuestList.Field(L"EXPI_DATE").AsWString());
		CHECK_EQUAL(nPro1, rsQuestList.Field(L"OBJ1_PROG").AsShort());
		CHECK_EQUAL(nPro2, rsQuestList.Field(L"OBJ2_PROG").AsShort());
		CHECK_EQUAL(nPro3, rsQuestList.Field(L"OBJ3_PROG").AsShort());
		CHECK_EQUAL(nPro4, rsQuestList.Field(L"OBJ4_PROG").AsShort());
		CHECK_EQUAL(nPro5, rsQuestList.Field(L"OBJ5_PROG").AsShort());
		rsQuestList.Close();
	}

	GAMEDB_TEST(Helper_InsertQuestHistory)
	{
		const int64 nAID		= 1;
		const int64	nCID		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int	nQuestID_1	= DBTestHelper::InsertQuestInfo(1, L"test_quest_1");
		const int	nQuestID_2	= DBTestHelper::InsertQuestInfo(2, L"test_quest_2");

		DBTestHelper::InsertQuestHistory(GConfig::m_nMyWorldID, nAID, nCID, nQuestID_1);
		DBTestHelper::InsertQuestHistory(GConfig::m_nMyWorldID, nAID, nCID, nQuestID_2);

		TDBRecordSet rsQuestHis;
		CHECK(UTestDB.Execute(rsQuestHis, L"SELECT COUNT(*) cnt FROM dbo.RZ_QUEST_HISTORY WHERE CHAR_ID = %I64d AND QUEST_ID IN (%d, %d);", nCID, nQuestID_1, nQuestID_2));
		CHECK_EQUAL(2, rsQuestHis.Field(L"cnt").AsInt());
		rsQuestHis.Close();
	}

	GAMEDB_TEST(USP_RZ_CHAR_GET_QUEST_HISTORY)
	{
		const int64 nAID		= 1;
		const int64	nCID		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int	nQuestID	= DBTestHelper::InsertQuestInfo(1, L"test_quest_1");

		DBTestHelper::InsertQuestHistory(GConfig::m_nMyWorldID, nAID, nCID, nQuestID);

		TDBRecordSet rsQuestHis;
		UTestDB.Execute(rsQuestHis, L"{CALL dbo.USP_RZ_CHAR_GET_QUEST_HISTORY (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, nAID, nCID);
		CHECK_EQUAL(1, rsQuestHis.GetFetchedRowCount());
		CHECK_EQUAL(nQuestID, rsQuestHis.Field(L"QUEST_ID").AsInt());
		rsQuestHis.Close();
	}

	GAMEDB_TEST(USP_RZ_CHAR_GET_QUEST_HISTORY_FILTERED)
	{
		const int64 nAID		= 1;
		const int64	nCID		= DBTestHelper::InsertCharacter(nAID, GConfig::m_nMyWorldID);
		const int	nQuestID	= DBTestHelper::InsertQuestInfo(1, L"test_quest_1");

		DBTestHelper::InsertQuestHistory(GConfig::m_nMyWorldID, nAID, nCID, nQuestID, L"2010-01-01 04:30");

		TDBRecordSet rsQuestHis;
		UTestDB.Execute(rsQuestHis, L"{CALL dbo.USP_RZ_CHAR_GET_QUEST_HISTORY (%d, %I64d, %I64d)}", GConfig::m_nMyWorldID, nAID, nCID);
		CHECK_EQUAL(0, rsQuestHis.GetFetchedRowCount());
		rsQuestHis.Close();
	}
}