#include "stdafx.h"
#include "DBTestGlobal.h"
#include "DBTestCraftFixture.h"
#include "GItemConvertPrefixedString.h"
#include "GConfig.h"


SUITE(DBTestCraftInsert)
{
	class FIXTURE_CRAFT_INSERT : public FIXTURE_CRAFT
	{
	public :
		void CHECK_PRODUCT_ITEM(const int64 nProdIUID, DBT_CRAFT_CHAR& cc)
		{
			TDBRecordSet rsProdItem;
			UTestDB.Execute(rsProdItem, L"SELECT ITEM_ID, CHAR_PTM, STACK_AMT, DURA, MAX_DURA, COLOR, CLAIMED, PERIOD, USAGE_PERIOD, EFF_END_DATE \
										FROM dbo.RZ_INVEN \
										WHERE IUID = %I64d;", nProdIUID);
			CHECK_EQUAL(cc.ProdItem.nItemID, rsProdItem.Field(L"ITEM_ID").AsInt());
			CHECK_EQUAL(cc.nCharPtm, rsProdItem.Field(L"CHAR_PTM").AsInt());
			CHECK_EQUAL(cc.ProdItem.nStackAmt, rsProdItem.Field(L"STACK_AMT").AsShort());
			CHECK_EQUAL(cc.ProdItem.nDura, rsProdItem.Field(L"DURA").AsByte());
			CHECK_EQUAL(cc.ProdItem.nDura, rsProdItem.Field(L"MAX_DURA").AsByte());
			CHECK_EQUAL(cc.ProdItem.nColor, rsProdItem.Field(L"COLOR").AsInt());
			CHECK_EQUAL(cc.ProdItem.bClaimed, rsProdItem.Field(L"CLAIMED").AsBool());
			CHECK_EQUAL(cc.ProdItem.bPeriodItem, rsProdItem.Field(L"PERIOD").AsBool());
			CHECK_EQUAL(cc.ProdItem.nUsagePeriod, rsProdItem.Field(L"USAGE_PERIOD").AsInt());
			CHECK(cc.ProdItem.strExpiDt == rsProdItem.Field(L"EFF_END_DATE").AsWString());
			rsProdItem.Close();
		}
	};

	GAMEDB_TEST_FIXTURE(FIXTURE_CRAFT_INSERT, USP_RZ_ITEM_CRAFT_INSERT)
	{
		DBT_CRAFT_CHAR	cc				= DBTestHelper_Craft::CreateCraftTestCharacter();
		
		wstring strString;
		GItemConvertPrefixedString cov;
		strString += cov.Convert(SLOTTYPE_INVENTORY, cc.vRecpItem[0].nSlotID, cc.vRecpItem[0].nStackAmt - cc.vRecpItem[0].nRecpAmt);
		strString += cov.Convert(SLOTTYPE_INVENTORY, cc.vRecpItem[1].nSlotID, cc.vRecpItem[1].nStackAmt - cc.vRecpItem[1].nRecpAmt);		

		const int16		nItemInstCnt	= 3; // recipe item count + prodcut item count.

		const wstring strExpiDt = wstring(L"'") + cc.ProdItem.strExpiDt + wstring(L"'");

		TDBRecordSet rsCraft;
		CHECK(UTestDB.Execute(rsCraft, L"{CALL dbo.USP_RZ_ITEM_CRAFT_INSERT (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %s, %d, '%s', %d, %d, %d)}"
			, GConfig::m_nMyWorldID, cc.nAID, cc.nCID, SLOTTYPE_INVENTORY, cc.ProdItem.nSlotID, cc.ProdItem.nItemID, cc.ProdItem.nStackAmt, cc.ProdItem.nDura, cc.ProdItem.nDura
			, cc.ProdItem.nColor, cc.ProdItem.bClaimed, cc.ProdItem.bPeriodItem, cc.ProdItem.nUsagePeriod, strExpiDt.c_str(), cc.nCharPtm
			, strString.c_str(), nItemInstCnt, cc.nXP, cc.nMoney));
		CHECK(0 < rsCraft.Field(L"IUID").AsInt64());

		const int64 nProdIUID = rsCraft.Field(L"IUID").AsInt64();
		rsCraft.Close();

		CHECK_CHARACTER(cc);
		CHECK_ITEM_RECIPE(cc.vRecpItem);
		CHECK_PRODUCT_ITEM(nProdIUID, cc);
	}

	GAMEDB_TEST_FIXTURE(FIXTURE_CRAFT_INSERT, USP_RZ_ITEM_CRAFT_INSERT_CREATE)
	{
		DBT_CRAFT_CHAR	cc				= DBTestHelper_Craft::CreateCraftTestCharacter(false);

		wstring strString;
		GItemConvertPrefixedString cov;
		strString += cov.Convert(SLOTTYPE_INVENTORY, cc.vRecpItem[0].nSlotID, cc.vRecpItem[0].nStackAmt - cc.vRecpItem[0].nRecpAmt);
		strString += cov.Convert(SLOTTYPE_INVENTORY, cc.vRecpItem[1].nSlotID, cc.vRecpItem[1].nStackAmt - cc.vRecpItem[1].nRecpAmt);		

		const int16		nItemInstCnt	= 3; // recipe item count + prodcut item count.

		const wstring strExpiDt = wstring(L"'") + cc.ProdItem.strExpiDt + wstring(L"'");

		TDBRecordSet rsCraft;
		CHECK(UTestDB.Execute(rsCraft, L"{CALL dbo.USP_RZ_ITEM_CRAFT_INSERT (%d, %I64d, %I64d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %s, %d, '%s', %d, %d, %d)}"
			, GConfig::m_nMyWorldID, cc.nAID, cc.nCID, SLOTTYPE_INVENTORY, cc.ProdItem.nSlotID, cc.ProdItem.nItemID, cc.ProdItem.nStackAmt, cc.ProdItem.nDura, cc.ProdItem.nDura
			, cc.ProdItem.nColor, cc.ProdItem.bClaimed, cc.ProdItem.bPeriodItem, cc.ProdItem.nUsagePeriod, strExpiDt.c_str(), cc.nCharPtm
			, strString.c_str(), nItemInstCnt, cc.nXP, cc.nMoney));
		CHECK(0 < rsCraft.Field(L"IUID").AsInt64());

		const int64 nProdIUID = rsCraft.Field(L"IUID").AsInt64();
		rsCraft.Close();

		CHECK_CHARACTER(cc);
		CHECK_ITEM_RECIPE(cc.vRecpItem);
		CHECK_PRODUCT_ITEM(nProdIUID, cc);
	}
}