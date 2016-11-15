#include "stdafx.h"
#include "SUnitTest.h"
#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"
#include "DBTestHelper_Quest.h"
#include "GConfig.h"


SUITE(DBTestQuest)
{
	struct DBTEST_ITEMINST
	{
		int64	nIUID;
		int		nItemID;
		uint8	nStackAmt;
		uint8	nStackModAmt;
	};

	

	GAMEDB_TEST(DBTestHelper_Quest_CreateQuestTestCharacter)
	{
		DBT_QUEST_CHAR qc = DBTestHelper_Quest::CreateQuestTestCharacter();

		TDBRecordSet rsChar;
		CHECK(UTestDB.Execute(rsChar, L"SELECT CHAR_ID FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", qc.nCID));
		CHECK_EQUAL(1, rsChar.GetFetchedRowCount());
		rsChar.Close();

		TDBRecordSet rsQuest;
		CHECK(UTestDB.Execute(rsQuest, L"SELECT QUEST_ID FROM dbo.RZ_QUEST_INFO WHERE QUEST_ID = %d;", qc.nQuestID));
		CHECK_EQUAL(1, rsQuest.GetFetchedRowCount());
		rsQuest.Close();

		TDBRecordSet rsItem_1;
		CHECK(UTestDB.Execute(rsItem_1, L"SELECT ITEM_ID FROM dbo.RZ_ITEM_INFO WHERE ITEM_ID = %d;", qc.vObjItemInst[0].nItemID));
		CHECK_EQUAL(1, rsItem_1.GetFetchedRowCount());
		rsItem_1.Close();

		TDBRecordSet rsItem_2;
		CHECK(UTestDB.Execute(rsItem_2, L"SELECT ITEM_ID FROM dbo.RZ_ITEM_INFO WHERE ITEM_ID = %d;", qc.vObjItemInst[1].nItemID));
		CHECK_EQUAL(1, rsItem_2.GetFetchedRowCount());
		rsItem_2.Close();

		TDBRecordSet rsQuestInst;
		CHECK(UTestDB.Execute(rsQuestInst, L"SELECT CHAR_ID, QUEST_ID FROM dbo.RZ_QUEST WHERE CHAR_ID = %I64d AND QUEST_ID = %d;", qc.nCID, qc.nQuestID));
		CHECK_EQUAL(0, rsQuestInst.GetFetchedRowCount());
		rsQuestInst.Close();	

		TDBRecordSet rsItemInst_1;
		CHECK(UTestDB.Execute(rsItemInst_1, L"SELECT STACK_AMT FROM dbo.RZ_INVEN WHERE IUID = %I64d;", qc.vObjItemInst[0].nIUID));
		CHECK_EQUAL(1, rsItemInst_1.GetFetchedRowCount());
		CHECK_EQUAL(qc.vObjItemInst[0].nStackAmt, rsItemInst_1.Field(L"STACK_AMT").AsShort());
		rsItemInst_1.Close();	

		TDBRecordSet rsItemInst_2;
		CHECK(UTestDB.Execute(rsItemInst_2, L"SELECT STACK_AMT FROM dbo.RZ_INVEN WHERE IUID = %I64d;", qc.vObjItemInst[1].nIUID));
		CHECK_EQUAL(1, rsItemInst_2.GetFetchedRowCount());
		CHECK_EQUAL(qc.vObjItemInst[1].nStackAmt, rsItemInst_2.Field(L"STACK_AMT").AsShort());
		rsItemInst_2.Close();	
	}

	GAMEDB_TEST(DBTestHelper_Quest_CreateQuestObjectTestCharacter)
	{
		DBT_QUEST_CHAR qc = DBTestHelper_Quest::CreateQuestObjectTestCharacter();

		TDBRecordSet rsChar;
		CHECK(UTestDB.Execute(rsChar, L"SELECT CHAR_ID FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", qc.nCID));
		CHECK_EQUAL(1, rsChar.GetFetchedRowCount());
		rsChar.Close();

		TDBRecordSet rsQuest;
		CHECK(UTestDB.Execute(rsQuest, L"SELECT QUEST_ID FROM dbo.RZ_QUEST_INFO WHERE QUEST_ID = %d;", qc.nQuestID));
		CHECK_EQUAL(1, rsQuest.GetFetchedRowCount());
		rsQuest.Close();

		TDBRecordSet rsQuestInst;
		CHECK(UTestDB.Execute(rsQuestInst, L"SELECT CHAR_ID, QUEST_ID FROM dbo.RZ_QUEST WHERE CHAR_ID = %I64d AND QUEST_ID = %d;", qc.nCID, qc.nQuestID));
		CHECK_EQUAL(1, rsQuestInst.GetFetchedRowCount());
		rsQuestInst.Close();		
	}

	GAMEDB_TEST(DBTestHelper_Quest_MakeObjItemToString)
	{
		DBT_QUEST_OBJ_ITEM	i;
		const int16			nRemoveAmt = 3;
		
		i.nSlotID	= 0;
		i.nStackAmt = 10;
	
		wstring str = DBTestHelper_Quest::MakeObjItemToString(i, 5);
		// wstring(L"1.0.7") => SlotID.SlotType.StackAmt
		CHECK(L"1.0.5," == str);
	}

	GAMEDB_TEST(USP_RZ_QUEST_UPDATE_VAR)
	{
		DBT_QUEST_CHAR	qc		= DBTestHelper_Quest::CreateQuestObjectTestCharacter();
		const int		nVar	= 135;

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_QUEST_UPDATE_VAR (%d, %I64d, %I64d, %d, %d, %d)}"
			, GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nSlotID, qc.nQuestID, nVar));

		TDBRecordSet rs;
		CHECK(UTestDB.Execute(rs, L"SELECT [VAR] FROM dbo.RZ_QUEST WHERE CHAR_ID = %I64d AND QUEST_ID = %d;", qc.nCID, qc.nQuestID));
		CHECK_EQUAL(nVar, rs.Field(L"VAR").AsInt());
		rs.Close();
	}
}