#include "stdafx.h"
#include "MMemPool.h"
#include "MMCode.h"
#include "GMath.h"
#include "AStlUtil.h"
#include "SUnitTestUtil.h"

/// cml 테스트 코드는 마땅히 둘 데가 없으므로 서버 코드에 함께 작성한다.
SUITE(CML)
{
	TEST(MemPoolTest)
	{
		const int objs_in_a_block = 50;

		class Foo : public MMemPool<Foo, objs_in_a_block>
		{
		private:
			char buf[256];
			int a;
		public:
			Foo() {}
			virtual ~Foo() {}
		};

		Foo* pFoo = new Foo();
		CHECK_EQUAL(Foo::capacity(), objs_in_a_block);
		CHECK_EQUAL(Foo::TotalCount(), 1);

		delete pFoo;
		CHECK_EQUAL(Foo::capacity(), objs_in_a_block);
		CHECK_EQUAL(Foo::TotalCount(), 0);

		Foo* pFoos[101];
		for (int i = 0; i < 101; i++)
		{
			pFoos[i] = new Foo();
		}
		CHECK_EQUAL(Foo::capacity(), 150);
		CHECK_EQUAL(Foo::TotalCount(), 101);

		for (int i = 0; i < 100; i++)
		{
			delete pFoos[i];
		}
		CHECK_EQUAL(Foo::capacity(), 150);
		CHECK_EQUAL(Foo::TotalCount(), 1);

		delete pFoos[100];
		CHECK_EQUAL(Foo::TotalCount(), 0);


		Foo::release();
		CHECK_EQUAL(Foo::capacity(), 0);
		CHECK_EQUAL(Foo::TotalCount(), 0);

		CHECK_EQUAL(sizeof(Foo), 256 + sizeof(int) + 4);
	}

	TEST(MemPoolTest2)
	{
		int cnt = 0;

		class Foo
		{
		protected:
			char buf[256];
			int a;
			int& cnt;
		public:
			Foo(int& c) : cnt(c) {}
			virtual ~Foo() 
			{
				++cnt;
			}
		};

		class Var : public Foo, public MMemPool<Var, 50>
		{
		private:
			int b;
		public:
			Var(int& c) : Foo(c) {}
			virtual ~Var() 
			{
				++cnt;
			}
		};

		Var* pVar = new Var(cnt);
		delete pVar;

		CHECK_EQUAL(cnt, 2);
		CHECK_EQUAL(Var::capacity(), 50);

		Var::release();
		CHECK_EQUAL(Var::capacity(), 0);
	}

	TEST(MemPool_Release)
	{
		const int objs_in_a_block = 5;

		class Foo : public MMemPool<Foo, objs_in_a_block>
		{
		private:
			char buf[256];
			int a;
		public:
			Foo() {}
			virtual ~Foo() {}
		};

		vector<Foo*>	m_FooList;

		for (int i = 0; i < 1000; i++)
		{
			int num = GMath::RandomNumber(1, 100);
			for (int t = 0; t < num; t++)
			{
				Foo* pFoo = new Foo();
				m_FooList.push_back(pFoo);
			}
			for (size_t j = 0; j < m_FooList.size(); j++)
			{
				delete m_FooList[j];
			}
			m_FooList.clear();
			Foo::release();
		}

		Foo::release();
	}

	TEST(MMCode_ControlCodeTest)
	{
		mmcode::MTagManager mgr;

		int value = 0;

		class TestControlCodeProc : public mmcode::MControlCodeProc
		{
			virtual void Proc(const string& strCodeParam, void* pParam=0)
			{
				int param = atoi(strCodeParam.c_str());
				*(int*)pParam = param;
			}
		};

		mgr.AddControlCode("test", new mmcode::MControlCode("test", new TestControlCodeProc()));
		mgr.AddControlCode("motion", new mmcode::MControlCode("motion", new TestControlCodeProc()));

		// 1차 시도 --
		mmcode::MControlCodeTransformer transformer("aaa{motion=idle}bbb", mgr);
		mmcode::MControlCodeTransResult result = transformer.transform();
		CHECK_EQUAL(result.m_strOutput, "aaabbb");

		// 2차 시도 --
		transformer.Input("aaa{motion=idlebbb");
		result = transformer.transform();
		CHECK_EQUAL(result.m_strOutput, "aaa{motion=idlebbb");

		// 3차 시도 --
		value = 0;
		transformer.Input("aaa{test=10}bbb");
		result = transformer.transform();
		CHECK_EQUAL(result.m_strOutput, "aaabbb");
		CHECK_EQUAL(value, 0);

		result.Proc(&value);
		CHECK_EQUAL(value, 10);

		// 4차 시도 --
		value = 0;
		transformer.Input("aaa{{test=10}bbb");
		result = transformer.transform();
		result.Proc(&value);
		CHECK_EQUAL(result.m_strOutput, "aaa{bbb");
		CHECK_EQUAL(value, 10);

		// 5차 시도 --
		value = 0;
		transformer.Input("aaa{test=}bbb");
		result = transformer.transform();
		result.Proc(&value);
		CHECK_EQUAL(result.m_strOutput, "aaabbb");
		CHECK_EQUAL(value, 0);

		// 6차 시도 --
		value = 0;
		transformer.Input("aaa{test}bbb");
		result = transformer.transform();
		result.Proc(&value);
		CHECK_EQUAL(result.m_strOutput, "aaabbb");
		CHECK_EQUAL(value, 0);
	}

	TEST(MMCode_ReplaceCharsTest)
	{
		mmcode::MTagManager mgr;

		mgr.AddReplaceChars("a", new mmcode::MReplaceChars("B"));
		mgr.AddReplaceChars("Hello", new mmcode::MReplaceChars("World"));
		mgr.AddReplaceChars("abc", new mmcode::MReplaceChars("TTT"));

		// 1차 시도 --
		mmcode::MReplaceCharsTransformer transformer("abc", mgr);
		string result = transformer.transform();
		CHECK_EQUAL(result, "Bbc");

		// 2차 시도 --
		transformer.Input("Hello hello World");
		result = transformer.transform();
		CHECK_EQUAL(result, "World hello World");

		// 3차 시도 --
		transformer.Input("abcdef ab");
		result = transformer.transform();
		CHECK_EQUAL(result, "Bbcdef Bb");
	}

	TEST(MMCode_MMCodeTest)
	{
		mmcode::MTagManager mgr;

		mgr.AddTag("color", new mmcode::MTag("color"));

		// 1차 시도 --
		mmcode::MMMCodeTransformer transformer("aaa[color=red]bbb[/color]ccc", mgr);
		mmcode::MMMCodeTransResult result = transformer.transform();
		CHECK_EQUAL(result.m_strOutput, "aaabbbccc");
		CHECK_EQUAL(result.m_Nodes.size(), 3);

		int color_tags = 0;
		for (size_t i = 0; i < result.m_Nodes.size(); ++i)
		{
			mmcode::MNode& node = result.m_Nodes[i];
			if (node.m_pTag && node.m_pTag->GetName() == "color") 
				color_tags++;
		}
		CHECK_EQUAL(color_tags, 1);

		// 2차 시도 --
		transformer.Input("aaa[color=red]bbb[/colorccc");
		result = transformer.transform();
		CHECK_EQUAL(result.m_strOutput, "aaa[color=red]bbb[/colorccc");

		// 3차 시도 --
		transformer.Input("aaa[color=]bbb[/colo]");
		result = transformer.transform();
		CHECK_EQUAL(result.m_strOutput, "aaa[color=]bbb[/colo]");

		// 4차 시도 --
		transformer.Input("aaa[color]bbb[/color]");
		result = transformer.transform();
		CHECK_EQUAL(result.m_strOutput, "aaabbb");

	}

	TEST(TestCheckBitSet_uint16)
	{
		uint16 nFlags = 0;
		SetBitSet(nFlags, 1);
		SetBitSet(nFlags, 3);
		SetBitSet(nFlags, 9);
		SetBitSet(nFlags, 15);

		CHECK_EQUAL(true, CheckBitSet(nFlags, 1)>0);
		CHECK_EQUAL(false, CheckBitSet(nFlags, 2)>0);
		CHECK_EQUAL(true, CheckBitSet(nFlags, 3)>0);
		CHECK_EQUAL(false, CheckBitSet(nFlags, 8)>0);
		CHECK_EQUAL(true, CheckBitSet(nFlags, 9)>0);
		CHECK_EQUAL(false, CheckBitSet(nFlags, 14)>0);
		CHECK_EQUAL(true, CheckBitSet(nFlags, 15)>0);

	}

	TEST(MapSecondVectorInserter)
	{
		map<int, vector<int>> test_map;
		int nKey = 0;
		int nElement = 0;
		nKey = 1; nElement = 1; MapSecondVectorInserter(test_map, nKey, nElement);
		nKey = 1; nElement = 2; MapSecondVectorInserter(test_map, nKey, nElement);
		nKey = 1; nElement = 3; MapSecondVectorInserter(test_map, nKey, nElement);
		nKey = 2; nElement = 4; MapSecondVectorInserter(test_map, nKey, nElement);
		nKey = 2; nElement = 5; MapSecondVectorInserter(test_map, nKey, nElement);
		nKey = 2; nElement = 6; MapSecondVectorInserter(test_map, nKey, nElement);

		TVALID_EQ(2, test_map.size());
		TVALID(test_map.find(1) != test_map.end());
		TVALID(test_map.find(2) != test_map.end());
		TVALID_EQ(3, test_map[1].size());
		TVALID_EQ(3, test_map[2].size());
		TVALID_EQ(1, test_map[1][0]);
		TVALID_EQ(2, test_map[1][1]);
		TVALID_EQ(3, test_map[1][2]);
		TVALID_EQ(4, test_map[2][0]);
		TVALID_EQ(5, test_map[2][1]);
		TVALID_EQ(6, test_map[2][2]);
	}

	TEST(DateRandomSeed)
	{
		//time_t rawtime;
		//struct tm * timeinfo;
		//time ( &rawtime );
		//timeinfo = localtime ( &rawtime );

		struct tm timeinfo;
		timeinfo.tm_year = 110;
		timeinfo.tm_mon = 12;
		timeinfo.tm_mday = 28;

		unsigned int nSeed = timeinfo.tm_year*timeinfo.tm_mon*timeinfo.tm_mday;
		srand(nSeed);
		int nR1 = rand();
		srand(nSeed);
		int nR2 = rand();
		CHECK_EQUAL(nR1, nR2);
	}
}