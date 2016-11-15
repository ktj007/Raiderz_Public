#include "stdafx.h"
#include "DBTestGlobal.h"
#include "DBTestHelper_Quest.h"
#include "GQuestDateCalculator.h"
#include "GConfig.h"

SUITE(DBTestQuestFail)
{
	GAMEDB_TEST(USP_RZ_QUEST_FAIL)
	{
		DBT_QUEST_CHAR qc = DBTestHelper_Quest::CreateQuestTestCharacter();
		wstring strAcceptDt;
		GQuestDateCalculator::GetAccpetDt(strAcceptDt);
		DBTestHelper::InsertQuest(GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nSlotID, qc.nQuestID, strAcceptDt);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_QUEST_FAIL (%d, %I64d, %I64d, %d, %d)}"
			, GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nSlotID, qc.nQuestID));

		TDBRecordSet rsQuest;
		CHECK(UTestDB.Execute(rsQuest, L"SELECT [STATE] FROM dbo.RZ_QUEST WHERE ACCN_ID = %I64d AND CHAR_ID = %I64d AND QUEST_ID = %d;", qc.nAID, qc.nCID, qc.nQuestID));
		CHECK_EQUAL(QST_FAIL, rsQuest.Field(L"STATE").AsByte());
		rsQuest.Close();		
	}
}