#include "stdafx.h"
#include "DBTestGlobal.h"
#include "DBTestHelper_Quest.h"
#include "GItemConvertPrefixedString.h"
#include "GConfig.h"


SUITE(DBTestQuestGiveUp)
{
	GAMEDB_TEST(USP_RZ_QUEST_GIVEUP)
	{
		DBT_QUEST_CHAR	qc		= DBTestHelper_Quest::CreateQuestTestCharacter();
		DBTestHelper::InsertQuest(GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nSlotID, qc.nQuestID);

		const int16 nRemoveStackAmt_1 = qc.vObjItemInst[0].nStackAmt;
		const int16 nRemoveStackAmt_2 = 2;

		wstring strString;
		GItemConvertPrefixedString cov;
		strString += cov.Convert((SH_ITEM_SLOT_TYPE)1, qc.vObjItemInst[0].nSlotID, qc.vObjItemInst[0].nStackAmt - nRemoveStackAmt_1);
		strString += cov.Convert((SH_ITEM_SLOT_TYPE)1, qc.vObjItemInst[1].nSlotID, qc.vObjItemInst[1].nStackAmt - nRemoveStackAmt_2);

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_QUEST_GIVEUP (%d, %I64d, %I64d, %d, %d, '%s', %d)}"
			, GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nSlotID, qc.nQuestID, strString.c_str(), 2));

		TDBRecordSet rsQuest;
		CHECK(UTestDB.Execute(rsQuest, L"SELECT QUEST_ID, [STATE] FROM dbo.RZ_QUEST WHERE CHAR_ID = %I64d AND SLOT_ID = %d;", qc.nCID, qc.nSlotID));
		CHECK_EQUAL(1, rsQuest.GetFetchedRowCount());
		CHECK(rsQuest.Field(L"QUEST_ID").IsNull());
		CHECK_EQUAL(0, rsQuest.Field(L"STATE").AsByte());
		rsQuest.Close();

		TDBRecordSet rsItem_1;
		CHECK(UTestDB.Execute(rsItem_1, L"SELECT IUID, STACK_AMT, ITEM_ID FROM dbo.RZ_INVEN WHERE OWNER_ID = %I64d AND SLOT_TYPE = 1 AND SLOT_ID = %d;", qc.nCID, qc.vObjItemInst[0].nSlotID));
		CHECK_EQUAL(qc.vObjItemInst[0].nStackAmt - nRemoveStackAmt_1, rsItem_1.Field(L"STACK_AMT").AsShort());
		rsItem_1.Close();

		TDBRecordSet rsItem_2;
		CHECK(UTestDB.Execute(rsItem_2, L"SELECT STACK_AMT FROM dbo.RZ_INVEN WHERE IUID = %I64d;", qc.vObjItemInst[1].nIUID));
		CHECK_EQUAL(qc.vObjItemInst[1].nStackAmt - nRemoveStackAmt_2, rsItem_2.Field(L"STACK_AMT").AsInt64());
		rsItem_2.Close();
	}
}