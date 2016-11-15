#include "stdafx.h"
#include "GTestForward.h"
#include "GLuaThisEnchanter.h"

SUITE(TestLuaThisEnchanter)
{
	struct Fixture
	{
		Fixture()
		{
			gsys.pScriptManager->Init();
		}

		~Fixture() 
		{
			gsys.pScriptManager->Fini();
		}
	};

	TEST(ConvertEachLine)
	{
		const int ActionQty=4;
		struct ScriptConvertCheck
		{
			const char* pszScript;
			const char* pszExpected;
		} cases[] = {
			//// 인자 0개 --------------------------------------------
			// [무시] 전역함수, 함수명매칭
			{"function OnAggro()", "function OnAggro()"	},
			// [무시] 전역함수, 함수명비매칭
			{"function OnPowerup()", "function OnPowerup()"},
			// [변환] 멤버함수, 함수명매칭
			{"function NPC_0110001:OnAggro()","function NPC_0110001:OnAggro(this)"},
			// [변환] 멤버함수 (공간추가), 함수명매칭
			{"function NPC_0110001:OnAggro( )","function NPC_0110001:OnAggro(this )"},
			// [무시] 멤버함수, 함수명비매칭
			{"function NPC_0110001:OnPowerup()","function NPC_0110001:OnPowerup()"},
			// [무시] 멤버함수 (공백추가), 함수명비매칭
			{"function NPC_0110001:OnPowerup( )","function NPC_0110001:OnPowerup( )"},

			// 인자 1개 --------------------------------------------
			// [무시] 전역함수, 함수명매칭
			{"function OnAggro(arg1)", "function OnAggro(arg1)"	},
			// [무시] 전역함수, 함수명비매칭
			{"function OnPowerup(arg1)", "function OnPowerup(arg1)"},
			// [변환] 멤버함수, 함수명매칭
			{"function NPC_0110001:OnAggro(arg1)","function NPC_0110001:OnAggro(this, arg1)"},
			// [무시] 멤버함수, 함수명비매칭
			{"function NPC_0110001:OnPowerup(arg1)","function NPC_0110001:OnPowerup(arg1)"},

			// 인자 2개 --------------------------------------------
			// [무시] 전역함수, 함수명매칭
			{"function OnAggro(arg1, arg2)", "function OnAggro(arg1, arg2)"	},
			// [무시] 전역함수, 함수명비매칭
			{"function OnPowerup(arg1, arg2)", "function OnPowerup(arg1, arg2)"},
			// [변환] 멤버함수, 함수명매칭
			{"function NPC_0110001:OnAggro(arg1, arg2)","function NPC_0110001:OnAggro(this, arg1, arg2)"},
			// [무시] 멤버함수, 함수명비매칭
			{"function NPC_0110001:OnPowerup(arg1, arg2)","function NPC_0110001:OnPowerup(arg1, arg2)"},

			// 가변 함수명 --------------------------------------------
			{"function NPC_0110001:OnHitCapsule_1(arg1, arg2)","function NPC_0110001:OnHitCapsule_1(this, arg1, arg2)"},
			{"function NPC_0110001:OnHitCapsule_3_2(arg1, arg2)","function NPC_0110001:OnHitCapsule_3_2(this, arg1, arg2)"}
		};

		// 멤버함수명 등록
		GLuaThisEnchanter enchanter;
		enchanter.AddMatchMemFunc("OnDialog");
		enchanter.AddMatchMemFunc("OnAggro");
		enchanter.AddMatchMemFunc("OnHitCapsule");
		enchanter.AddMatchMemFunc("OnInteract");

		// 테스트시작
		for (int i=0; i<ArraySize(cases); ++i) 
		{
			ScriptConvertCheck check = cases[i];
			CHECK_EQUAL(check.pszExpected, enchanter.ConvertLine(check.pszScript));	
		}
	}


	TEST(ConvertLuaScriptStreamParam0)
	{
		string strLua = "function TEST_1:OnTest()\nlocal Val = 1\nend";

		const string strCheck = "function TEST_1:OnTest(this)\nlocal Val = 1\nend\n";

		string strCnv;
		GLuaThisEnchanter e;

		e.AddMatchMemFunc("TEST_1:OnTest");

		CHECK(e.ConvertFromStream(strLua, strCnv));
		CHECK_EQUAL(strCheck, strCnv);
	}


	TEST(ConvertLuaScriptStreamParam1)
	{
		string strLua = "function TEST_1:OnTest(arg1)\nlocal Val = 1\nend";

		const string strCheck = "function TEST_1:OnTest(this, arg1)\nlocal Val = 1\nend\n";

		string strCnv;
		GLuaThisEnchanter e;

		e.AddMatchMemFunc("TEST_1:OnTest");

		CHECK(e.ConvertFromStream(strLua, strCnv));
		CHECK_EQUAL(strCheck, strCnv);
	}


	TEST(ConvertLuaScriptStreamParam2)
	{
		string strLua = "function TEST_1:OnTest(arg1, arg2)\nlocal Val = 1\nend";

		const string strCheck = "function TEST_1:OnTest(this, arg1, arg2)\nlocal Val = 1\nend\n";

		string strCnv;
		GLuaThisEnchanter e;

		e.AddMatchMemFunc("TEST_1:OnTest");

		CHECK(e.ConvertFromStream(strLua, strCnv));
		CHECK_EQUAL(strCheck, strCnv);
	}
}


