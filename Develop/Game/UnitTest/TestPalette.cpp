#include "stdafx.h"
#include "TestCommonFixture.h"
#include "MockNetClient.h"

SUITE(Palette)
{
	TEST_FIXTURE(SimpleTestFixture, TestPalette_PutUpDown)
	{
		//포스트 부분은 현재 넘 간단해서 생략

		PALETTE_NUM nNum = PALETTENUM_3;
		PALETTE_SLOT nSlot = PALETTESLOT_6;
		PALETTE_ITEM_TYPE nType = PIT_TALENT;
		int nItemIDorTalentID = 2021;

		m_pNetAgent->OnRecv(MC_PALETTE_PUTUP, 4, NEW_UCHAR(nNum), NEW_UCHAR(nSlot)
			, NEW_UCHAR(nType), NEW_INT(nItemIDorTalentID));

		wstring strMsg;
		gvar.MyInfo.PaletteList.GetItemIDList(strMsg);
		CHECK(strMsg == wstring(L"0,0,0,0,0,0,0,0,0,0"));

		gvar.MyInfo.PaletteList.SetCurPaletteListIndex(nNum);
		gvar.MyInfo.PaletteList.GetItemIDList(strMsg);
		CHECK(strMsg == wstring(L"0,0,0,0,0,T2021,0,0,0,0"));

		vector<PALETTE_NUM> vecNum;
		vecNum.push_back(nNum);
		vector<PALETTE_SLOT> vecSlot;
		vecSlot.push_back(nSlot);

		m_pNetAgent->OnRecv(MC_PALETTE_PUTDOWN, 2, NEW_BLOB(vecNum), NEW_BLOB(vecSlot));

		gvar.MyInfo.PaletteList.GetItemIDList(strMsg);
		CHECK(strMsg == wstring(L"0,0,0,0,0,0,0,0,0,0"));
	}
}
