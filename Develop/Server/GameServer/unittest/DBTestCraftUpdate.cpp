#include "stdafx.h"
#include "DBTestGlobal.h"
#include "DBTestCraftFixture.h"
#include "GItemConvertPrefixedString.h"
#include "GConfig.h"


SUITE(DBTestCraftUpdate)
{
	class FIXTURE_CRAFT_UPDATE : public FIXTURE_CRAFT
	{
	public :
		void CHECK_PRODUCT_ITEM(const int64 nProdIUID, const int16 nStackAmt, const int16 nModStackAmt)
		{
			TDBRecordSet rsProdItem;
			UTestDB.Execute(rsProdItem, L"SELECT STACK_AMT FROM dbo.RZ_INVEN WHERE IUID = %I64d;", nProdIUID);
			CHECK_EQUAL(nStackAmt, rsProdItem.Field(L"STACK_AMT").AsShort());
			rsProdItem.Close();			
		}
	};

	GAMEDB_TEST_FIXTURE(FIXTURE_CRAFT_UPDATE, USP_RZ_ITEM_CRAFT_UPDATE)
	{
		DBT_CRAFT_CHAR	cc				= DBTestHelper_Craft::CreateCraftTestCharacter();
		const int16		nStackAmt		= 10;
		const int16		nSlotID			= 4;
		const int		nItemInstCnt	= 3;

		wstring strString;
		GItemConvertPrefixedString cov;
		strString += cov.Convert(SLOTTYPE_INVENTORY, cc.vRecpItem[0].nSlotID, cc.vRecpItem[0].nStackAmt - cc.vRecpItem[0].nRecpAmt);
		strString += cov.Convert(SLOTTYPE_INVENTORY, cc.vRecpItem[1].nSlotID, cc.vRecpItem[1].nStackAmt - cc.vRecpItem[1].nRecpAmt);

		const int64		nProdIUID		= DBTestHelper::InsertItem(cc.nAID, cc.nCID, GConfig::m_nMyWorldID, cc.ProdItem.nItemID, nStackAmt, SLOTTYPE_INVENTORY, nSlotID);

		cc.ProdItem.nStackAmt			= 5;

		CHECK(UTestDB.Execute(L"{CALL dbo.USP_RZ_ITEM_CRAFT_UPDATE (%d, %I64d, %I64d, %d, %d, %d, %I64d, %d, %d, %d, %d, '%s', %d)}"
			, GConfig::m_nMyWorldID, cc.nAID, cc.nCID, SLOTTYPE_INVENTORY, nSlotID, cc.ProdItem.nItemID, nProdIUID, cc.nXP, cc.nMoney
			, cc.ProdItem.nStackAmt + nStackAmt, cc.nCharPtm, strString.c_str(), nItemInstCnt));

		CHECK_CHARACTER(cc);
		CHECK_ITEM_RECIPE(cc.vRecpItem);
		CHECK_PRODUCT_ITEM(nProdIUID, cc.ProdItem.nStackAmt + nStackAmt, cc.ProdItem.nStackAmt);
	}
}