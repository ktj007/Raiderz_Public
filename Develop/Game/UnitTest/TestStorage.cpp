#include "stdafx.h"
#include "FTestGame.h"

SUITE(Storage)
{
	TEST_FIXTURE(FTestGame, StorageShowUI)
	{
		int nItemID = 200;
		XUnitTestHelper::CreateItemSwordInfo(nItemID);

		CHECK_EQUAL(gvar.MyInfo.Storage.GetSlotSize(), BASE_STORAGE_SIZE);

		vector<TD_ITEM>	vecTDItem;
		TD_ITEM tdItem;
		tdItem.m_nItemID = nItemID;
		tdItem.m_nQuantity = 1;
		tdItem.m_nSlotID = 0;
		vecTDItem.push_back(tdItem);

		m_pNetAgent->OnRecv(MC_STORAGE_SHOW_UI, 3, NEW_INT(1000), NEW_BLOB(vecTDItem), NEW_BOOL(false));

		CHECK_EQUAL(gvar.MyInfo.Storage.GetSize(), 1);

		gvar.MyInfo.Storage.Clear();

		CHECK_EQUAL(gvar.MyInfo.Storage.GetSize(), 0);
	}
}