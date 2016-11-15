#include "stdafx.h"
#include "TestModule.h"
#include "XPlayerFaction.h"
#include "MockFactionInfoMgr.h"
#include "CSFactionCalculator.h"

SUITE(PlayerFaction)
{
	TEST_FIXTURE(SimpleTestFixture, TestPlayerFaction)
	{
		bool bLoad = info.faction->CreateFactionFromString(L"<FACTION id=\"101\" default=\"28000\" title=\"[샘플]인젠자유항 상인연합\" desc=\"[샘플]인젠의 상인들에 대한 우호도.\" />");
		CHECK(bLoad);

		gvar.MyInfo.FactionInfo.SetQuantity(101, CSFactionCalculator::GetGoodMinQuantity());

		CHECK(gvar.MyInfo.FactionInfo.IsExist(101));	

		CHECK_EQUAL(gvar.MyInfo.FactionInfo.GetRelation(101), FRT_GOOD);	

		//MWLua::table t = gvar.MyInfo.FactionInfo.GetFactionInfoTable();
		//CHECK_EQUAL(t.get<int>("id")	, 201);
		//CHECK_EQUAL(t.get<int>("value")	, 50);
		//CHECK_EQUAL(t.get<char*>("title"), "");
		//CHECK_EQUAL(t.get<char*>("desc"), "");
		//CHECK_EQUAL(t.get<int>("type")	, 3);
	}

}