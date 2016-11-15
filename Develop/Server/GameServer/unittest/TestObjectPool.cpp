#include "stdafx.h"
#include "GTestForward.h"
#include "GObjectPool.h"

class TestClass
{
public:
	TestClass()
	{
		bInit = false;
		bFini = false;
	}

	~TestClass()
	{
		
	}

	void Create() { bInit = true; }
	void Destroy() { bFini = true; }

	bool bInit;
	bool bFini;
};

SUITE(ObjectPool)
{
	struct FObjectPool
	{
		~FObjectPool()
		{
			GetPool<TestClass>().Fini();
		}
	};

	TEST_FIXTURE(FObjectPool, ObjectPool_Init)
	{
		CHECK_EQUAL(0U, GetPool<TestClass>().GetPoolSize());
		CHECK_EQUAL(0U, GetPool<TestClass>().GetUnusedObjectCount());
		GetPool<TestClass>().Init(100);
		CHECK_EQUAL(100U, GetPool<TestClass>().GetPoolSize());
		CHECK_EQUAL(100U, GetPool<TestClass>().GetUnusedObjectCount());
		GetPool<TestClass>().Fini();
		CHECK_EQUAL(0U, GetPool<TestClass>().GetPoolSize());
		CHECK_EQUAL(0U, GetPool<TestClass>().GetUnusedObjectCount());
	}

	TEST_FIXTURE(FObjectPool, ObjectPool_Recycle)
	{
		GetPool<TestClass>().Init(10);
		TestClass* pItem1 = GetPool<TestClass>().New();
		GetPool<TestClass>().Drop(pItem1);
		TestClass* pItem2 = GetPool<TestClass>().New();
		CHECK_EQUAL(pItem1, pItem2);
	}

	TEST_FIXTURE(FObjectPool, ObjectPool_OverPool)
	{
		GetPool<TestClass>().Init(2);
		CHECK_EQUAL(2U, GetPool<TestClass>().GetPoolSize());
		GetPool<TestClass>().New();
		CHECK_EQUAL(2U, GetPool<TestClass>().GetPoolSize());
		GetPool<TestClass>().New();
		CHECK_EQUAL(2U, GetPool<TestClass>().GetPoolSize());
		GetPool<TestClass>().New();
		CHECK_EQUAL(3U, GetPool<TestClass>().GetPoolSize());
	}

	TEST_FIXTURE(FObjectPool, ObjectPool_SubjectCallback)
	{
		TestClass* pItem1 = GetPool<TestClass>().New();
		CHECK_EQUAL(true, pItem1->bInit);
		CHECK_EQUAL(false, pItem1->bFini);
		GetPool<TestClass>().Drop(pItem1);
		CHECK_EQUAL(true, pItem1->bInit);
		CHECK_EQUAL(true, pItem1->bFini);
	}
}

