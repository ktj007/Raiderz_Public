#include "stdafx.h"
#include "DBTestHelper_Quest.h"
#include "DBTestHelper.h"
#include "GQuestDateCalculator.h"
#include "UTestDatabaseContainer.h"
#include "UTestDatabase.h"
#include "Gconfig.h"


DBT_QUEST_CHAR DBTestHelper_Quest::CreateQuestTestCharacter(const bool bChallanger)
{
	DBT_QUEST_CHAR qc;

	wstring strAcceptDt;
	wstring strExpiDt;

	GQuestDateCalculator::GetAccpetDt(strAcceptDt);
	GQuestDateCalculator::GetExpiDt(bChallanger, strExpiDt);

	qc.nAID				= 1;
	qc.nCID				= DBTestHelper::InsertCharacter(qc.nAID, GConfig::m_nMyWorldID, L"test_char");
	qc.nLevel			= 5;
	qc.nCharPtm			= 123;
	qc.nSlotID			= 0;
	qc.nQuestID			= DBTestHelper::InsertQuestInfo(1);
	qc.nXP				= 500;
	qc.nMoney			= 10000;
	qc.nModMoney		= 10;
	qc.strAcceptDt		= strAcceptDt;
	qc.strExpiDt		= strExpiDt;

	DBT_QUEST_OBJ_ITEM item_1;
	DBT_QUEST_OBJ_ITEM item_2;

	item_1.nItemID		= DBTestHelper::InsertItemInfo(1, L"test_object_item_1");
	item_1.nSlotID		= 0;
	item_1.nStackAmt	= 10;	
	item_1.nIUID		= DBTestHelper::InsertItem(qc.nAID, qc.nCID, GConfig::m_nMyWorldID, item_1.nItemID, item_1.nStackAmt, 1, item_1.nSlotID);

	item_2.nItemID		= DBTestHelper::InsertItemInfo(2, L"test_object_item_2");
	item_2.nSlotID		= 1;
	item_2.nStackAmt	= 10;	
	item_2.nIUID		= DBTestHelper::InsertItem(qc.nAID, qc.nCID, GConfig::m_nMyWorldID, item_2.nItemID, item_2.nStackAmt, 1, item_2.nSlotID);

	UTestDB.Execute(L"UPDATE dbo.RZ_INVEN SET ACCN_ID = %I64d;", qc.nAID);

	qc.vObjItemInst.push_back(item_1);
	qc.vObjItemInst.push_back(item_2);

	qc.Pos.nFieldID		= 1;
	qc.Pos.fX			= 1.0f;
	qc.Pos.fY			= 2.0f;
	qc.Pos.fZ			= 3.0f;

	return qc;
}

DBT_QUEST_CHAR DBTestHelper_Quest::CreateQuestObjectTestCharacter()
{
	DBT_QUEST_CHAR qc;

	wstring strAcceptDt;

	GQuestDateCalculator::GetAccpetDt(strAcceptDt);

	qc.nAID				= 1;
	qc.nCID				= DBTestHelper::InsertCharacter(qc.nAID, GConfig::m_nMyWorldID, L"test_char");
	qc.nLevel			= 5;
	qc.nCharPtm			= 123;
	qc.nSlotID			= 0;
	qc.nQuestID			= DBTestHelper::InsertQuestInfo(1);
	qc.strAcceptDt		= strAcceptDt;

	qc.Pos.nFieldID		= 1;
	qc.Pos.fX			= 1.0f;
	qc.Pos.fY			= 2.0f;
	qc.Pos.fZ			= 3.0f;	
	
	DBTestHelper::InsertQuest(GConfig::m_nMyWorldID, qc.nAID, qc.nCID, qc.nSlotID, qc.nQuestID, strAcceptDt);

	return qc;
}

wstring DBTestHelper_Quest::MakeObjItemToString( DBT_QUEST_OBJ_ITEM& ObjItem, const int16 nRemoveAmt )
{
	wchar_t szItem[1024] = {0, };

	swprintf_s(szItem, L"%d.%d.%d,", 1, ObjItem.nSlotID, ObjItem.nStackAmt - nRemoveAmt);

	return szItem;
}