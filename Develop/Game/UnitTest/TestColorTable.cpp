#include "stdafx.h"
#include "XColorTable.h"

SUITE(ColorTable)
{
	TEST(TestColorGroup)
	{
		XColorGroup cg;
		cg.SetColor(5, MCOLOR(0xFFFF0000));
		cg.SetColor(11, MCOLOR(0xFFFFFFFF));
		cg.SetColor(12, MCOLOR(0x00000001));
		cg.SetColor(0, MCOLOR(0x000F0000));

		CHECK(MCOLOR(0xFFFF0000) == cg.GetColor(5));
		CHECK(MCOLOR(0xFFFFFFFF) == cg.GetColor(11));
		CHECK(MCOLOR(0x00000001) == cg.GetColor(12));
		CHECK(MCOLOR(0x000F0000) == cg.GetColor(0));
	}

	TEST(TestColorTableSimple)
	{
		XColorTable colorTable;
		XColorGroup* pColorGroup = new XColorGroup();
		pColorGroup->SetName(L"default1");

		pColorGroup->SetColor(0, MCOLOR(0xFFFFFFFF));
		pColorGroup->SetColor(5, MCOLOR(0xFF000002));

		colorTable.AddColorGroup(pColorGroup);

		CHECK(MCOLOR(0) == colorTable.GetColor(L"aaa", 0));
		CHECK(MCOLOR(0) == colorTable.GetColor(L"default1", 3));
		CHECK(MCOLOR(0xFFFFFFFF) == colorTable.GetColor(L"default1", 0));
		CHECK(MCOLOR(0xFF000002) == colorTable.GetColor(L"default1", 5));
	}

	TEST(TestColorTable_ParseColorValue)
	{
		class TestColorTable : public XColorTable
		{
		public:
			using XColorTable::ParseColorValue;
		};

		TestColorTable ct;

		CHECK_EQUAL(0xFFFFFFFF, ct.ParseColorValue(L"FFFFFF"));
		CHECK_EQUAL(0xFFFFFFFF, ct.ParseColorValue(L"FFFFFFFF"));
		CHECK_EQUAL(0xFF000001, ct.ParseColorValue(L"1"));

		CHECK_EQUAL(0xFF0000FF, ct.ParseColorValue(L"FF"));
		CHECK_EQUAL(0xFF001aFF, ct.ParseColorValue(L"1aFF"));
		CHECK_EQUAL(0xFF001234, ct.ParseColorValue(L"1234"));
	}
}