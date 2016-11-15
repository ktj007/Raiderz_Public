#include "StdAfx.h"
#include "GTestForward.h"

SUITE(WLua)
{
	class TestForLua
	{
	public:
		TestForLua(): var1(333) {}
		virtual int Func() { return 777; }
		MWLua::ret_int2 FuncReturnInt2WithSomeArgs(float aaa, int bbb)	{ return MWLua::ret_int2((int)aaa, bbb); }

		int var1;
	};

	class TestForLuaChild : public TestForLua
	{
	public:
		virtual int Func() override { return 999; }
	};

	MWLua::ret_int2 FuncReturnInt2()
	{
		return MWLua::ret_int2(111, 222);
	}

	MWLua::ret_int2 FuncReturnInt2WithSomeArgs(float aaa, int bbb)
	{
		return MWLua::ret_int2((int)aaa, bbb);
	}

	MWLua::ret_double2 FuncReturnDouble2()
	{
		return MWLua::ret_double2(111.111, 222.222);
	}

	struct FWLua
	{
		FWLua()
		{ 
			gsys.pScriptManager->Init();
		}

		~FWLua() 
		{
			gsys.pScriptManager->Fini();
		}
	};

	TEST_FIXTURE(FWLua, RegisterSameSymbolTwice)
	{
		// TestForLua 심볼 등록
		MWLua::class_<TestForLua>(WLUA->GetState(), "TestForLua")
			.def("Func",			&TestForLua::Func)
			.def_readwrite("var1",	&TestForLua::var1)
			;

		// 인스턴스로 TestForLua 심볼 등록
		TestForLua test;
		WLUA->SetVar<TestForLua*>("TestForLua", &test);

		// 제대로 된 인스턴스 등록
		TestForLua test2;
		WLUA->SetVar<TestForLua*>("test", &test2);

		WRUNSTRING(WLUA, "TEST_VAR = test:Func()");	// 크러쉬가 나면 안된다!
		WRUNSTRING(WLUA, "TEST_VAR2 = test.var1");	// 크러쉬가 나면 안된다!

		// 함수반환 값 제대로 가져왔는지 확인
		CHECK_EQUAL(777, WLUA->GetVar<int>("TEST_VAR"));
		CHECK_EQUAL(333, WLUA->GetVar<int>("TEST_VAR2"));
	}

	TEST_FIXTURE(FWLua, RegisterSameSymbolRefTwice)
	{
		// TestForLua 심볼 등록
		MWLua::class_<TestForLua>(WLUA->GetState(), "TestForLua")
			.def("Func",			&TestForLua::Func)
			.def_readwrite("var1",	&TestForLua::var1)
			;

		// 인스턴스로 TestForLua 심볼 등록
		TestForLua test;
		WLUA->SetVar<TestForLua&>("TestForLua", test);

		// 제대로 된 인스턴스 등록
		TestForLua test2;
		WLUA->SetVar<TestForLua&>("test", test2);

		WRUNSTRING(WLUA, "TEST_VAR = test:Func()");	// 크러쉬가 나면 안된다!
		WRUNSTRING(WLUA, "TEST_VAR2 = test.var1");	// 크러쉬가 나면 안된다!

		// 함수반환 값 제대로 가져왔는지 확인
		CHECK_EQUAL(777, WLUA->GetVar<int>("TEST_VAR"));
		CHECK_EQUAL(333, WLUA->GetVar<int>("TEST_VAR2"));
	}

	TEST_FIXTURE(FWLua, RegisterSameSymbolRefTwice2)
	{
		// TestForLua 심볼 등록
		MWLua::class_<TestForLua>(WLUA->GetState(), "TestForLua")
			.def("Func",			&TestForLua::Func)
			.def_readwrite("var1",	&TestForLua::var1)
			;

		// 인스턴스로 TestForLua 심볼 등록
		TestForLua test;
		WLUA->SetVarRef<TestForLua>("TestForLua", test);

		// 제대로 된 인스턴스 등록
		TestForLua test2;
		WLUA->SetVarRef<TestForLua>("test", test2);

		WRUNSTRING(WLUA, "TEST_VAR = test:Func()");	// 크러쉬가 나면 안된다!
		WRUNSTRING(WLUA, "TEST_VAR2 = test.var1");	// 크러쉬가 나면 안된다!

		// 함수반환 값 제대로 가져왔는지 확인
		CHECK_EQUAL(777, WLUA->GetVar<int>("TEST_VAR"));
		CHECK_EQUAL(333, WLUA->GetVar<int>("TEST_VAR2"));
	}

	TEST_FIXTURE(FWLua, SetNull)
	{
		// TestForLua 심볼 등록
		MWLua::class_<TestForLua>(WLUA->GetState(), "TestForLua")
			.def("Func",			&TestForLua::Func)
			.def_readwrite("var1",	&TestForLua::var1)
			;

		// 제대로 된 인스턴스 등록
		TestForLua test2;
		WLUA->SetVar<TestForLua*>("test", &test2);

		CHECK_EQUAL(true, WLUA->IsExistGlobal("test"));

		// 널로 설정
		WLUA->SetNull("test");
		CHECK_EQUAL(false, WLUA->IsExistGlobal("test"));
	}

	TEST_FIXTURE(FWLua, MultiReturn)
	{
		WLUA->Def("FuncReturnInt2", &FuncReturnInt2);
		WLUA->Def("FuncReturnInt2WithSomeArgs", &FuncReturnInt2WithSomeArgs);
		WLUA->Def("FuncReturnDouble2", &FuncReturnDouble2);

		WRUNSTRING(WLUA, 
			"TEST_VAR1, TEST_VAR2 = FuncReturnInt2()\n"
			"TEST_VAR3, TEST_VAR4 = FuncReturnInt2WithSomeArgs(333, 444)\n"
			"TEST_VAR5, TEST_VAR6 = FuncReturnDouble2()\n"
			);
		CHECK_EQUAL(111, WLUA->GetVar<int>("TEST_VAR1"));
		CHECK_EQUAL(222, WLUA->GetVar<int>("TEST_VAR2"));
		CHECK_EQUAL(333, WLUA->GetVar<int>("TEST_VAR3"));
		CHECK_EQUAL(444, WLUA->GetVar<int>("TEST_VAR4"));
		CHECK_EQUAL(111.111, WLUA->GetVar<double>("TEST_VAR5"));
		CHECK_EQUAL(222.222, WLUA->GetVar<double>("TEST_VAR6"));
	}

	TEST_FIXTURE(FWLua, MemberFuncMultiReturn)
	{
		// TestForLua 심볼 등록
		MWLua::class_<TestForLua>(WLUA->GetState(), "TestForLua")
			.def(MWLua::constructor<>())
			.def("FuncReturnInt2WithSomeArgs",			&TestForLua::FuncReturnInt2WithSomeArgs)
			;

		WRUNSTRING(WLUA, 
			"local t = TestForLua()\n"
			"TEST_VAR1, TEST_VAR2 = t:FuncReturnInt2WithSomeArgs(111, 222)\n"
			);

		CHECK_EQUAL(111, WLUA->GetVar<int>("TEST_VAR1"));
		CHECK_EQUAL(222, WLUA->GetVar<int>("TEST_VAR2"));
	}

	TEST_FIXTURE(FWLua, OverrideCall)
	{
		// TestForLua 심볼 등록
		MWLua::class_<TestForLua>(WLUA->GetState(), "TestForLua")
			.def("Func",			&TestForLua::Func)
			.def_readwrite("var1",	&TestForLua::var1)
			;

		MWLua::class_<TestForLuaChild>(WLUA->GetState(), "TestForLuaChild")
			.def("Func",			&TestForLuaChild::Func)
			;

		// 제대로 된 인스턴스 등록
		TestForLua test;
		WLUA->SetVar<TestForLua*>("test", &test);
		TestForLuaChild test_child;
		WLUA->SetVar<TestForLuaChild*>("test_child", &test_child);

		WRUNSTRING(WLUA, "TEST_VAR1 = test:Func()");
		CHECK_EQUAL(777, WLUA->GetVar<int>("TEST_VAR1"));

		WRUNSTRING(WLUA, "TEST_VAR2 = test_child:Func()");
		CHECK_EQUAL(999, WLUA->GetVar<int>("TEST_VAR2"));
	}


}