#include "stdafx.h"
#include "DBTestGlobal.h"
#include "DBTestCraftFixture.h"

void FIXTURE_CRAFT::CHECK_CHARACTER( DBT_CRAFT_CHAR& cc )
{
	TDBRecordSet rsChar;
	UTestDB.Execute(rsChar, L"SELECT [EXP], [MONEY], CHAR_PTM FROM dbo.RZ_CHARACTER WHERE CHAR_ID = %I64d;", cc.nCID);
	CHECK_EQUAL(1, rsChar.GetFetchedRowCount());
	CHECK_EQUAL(cc.nXP, rsChar.Field(L"EXP").AsInt());
	CHECK_EQUAL(cc.nMoney, rsChar.Field(L"MONEY").AsInt());
	CHECK_EQUAL(cc.nCharPtm, rsChar.Field(L"CHAR_PTM").AsInt());
	rsChar.Close();
}

void FIXTURE_CRAFT::CHECK_ITEM_RECIPE( DBT_RECP_ITEM_VEC& vRecpItem )
{
	TDBRecordSet rsRecp_1;
	UTestDB.Execute(rsRecp_1, L"SELECT IUID, ITEM_ID, STACK_AMT, CHAR_PTM FROM dbo.RZ_INVEN WHERE SLOT_TYPE = %d AND SLOT_ID = %d;", SLOTTYPE_INVENTORY, vRecpItem[0].nSlotID);
	CHECK_EQUAL(1, rsRecp_1.GetFetchedRowCount());
	CHECK_EQUAL(0, rsRecp_1.Field(L"IUID").AsInt64());
	CHECK(rsRecp_1.Field(L"ITEM_ID").IsNull());
	CHECK_EQUAL(0, rsRecp_1.Field(L"STACK_AMT").AsShort());
	CHECK(rsRecp_1.Field(L"CHAR_PTM").IsNull());
	rsRecp_1.Close();

	TDBRecordSet rsRecp_2;
	UTestDB.Execute(rsRecp_2, L"SELECT ITEM_ID, STACK_AMT FROM dbo.RZ_INVEN WHERE IUID = %I64d;", vRecpItem[1].nIUID);
	CHECK_EQUAL(vRecpItem[1].nItemID, rsRecp_2.Field(L"ITEM_ID").AsInt());
	CHECK_EQUAL(vRecpItem[1].nStackAmt - vRecpItem[1].nRecpAmt, rsRecp_2.Field(L"STACK_AMT").AsShort());
	rsRecp_2.Close();
}