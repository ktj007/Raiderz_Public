#include "stdafx.h"
#include "DBTestGlobal.h"
#include "Gconfig.h"

SUITE(DBTEST_GM_QUEST)
{
	GAMEDB_TEST(USP_RZ_GM_QUEST_HISTORY_RESET_ALL)
	{
		const int64 GSN = 1;
		const int64 CSN = DBTestHelper::InsertCharacter(GSN, GConfig::m_nMyWorldID, L"TEST");

		DBTestHelper::InsertQuestHistory(GConfig::m_nMyWorldID, GSN, CSN, 1);
		DBTestHelper::InsertQuestHistory(GConfig::m_nMyWorldID, GSN, CSN, 2);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_GM_QUEST_HISTORY_RESET_ALL (%I64d, %I64d)}", GSN, CSN);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT COUNT(*) AS CNT FROM dbo.RZ_QUEST_HISTORY WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d;"
			, GSN, CSN);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(0, rs.Field(L"CNT").AsInt());
		rs.Close();
	}

	GAMEDB_TEST(USP_RZ_GM_QUEST_RESET_ALL)
	{
		const int64 GSN = 1;
		const int64 CSN = DBTestHelper::InsertCharacter(GSN, GConfig::m_nMyWorldID, L"TEST");

		DBTestHelper::InsertQuest(GConfig::m_nMyWorldID, GSN, CSN, 0, 1);
		DBTestHelper::InsertQuest(GConfig::m_nMyWorldID, GSN, CSN, 1, 2);

		UTestDB.Execute(L"{CALL dbo.USP_RZ_GM_QUEST_RESET_ALL (%I64d, %I64d)}", GSN, CSN);

		TDBRecordSet rs;
		UTestDB.Execute(rs, L"SELECT COUNT(*) AS CNT FROM dbo.RZ_QUEST WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d AND QUEST_ID IS NOT NULL AND STATE <> 0;"
			, GSN, CSN);
		CHECK_EQUAL(1, rs.GetFetchedRowCount());
		CHECK_EQUAL(0, rs.Field(L"CNT").AsInt());
		rs.Close();
	}
}