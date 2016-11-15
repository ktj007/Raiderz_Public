#include "stdafx.h"
#include "../MemPoolObjCounter.h"
#include "../MMemPool.h"
#include <list>

#pragma comment(lib, "winmm.lib")

TEST( TestMemPool, MemPoolAndCounterTest )
{
	////////////////////////////////////////////////////////////////////////// 테스트 준비
	class CTestMemPool1 : public MMemPool<CTestMemPool1>
	{
		int member;
	};

	class CTestMemPool2 : public MMemPool<CTestMemPool2>
	{
		int member;
	};

	class PoolReporter : public MemPoolObjCounter::IVisitor
	{
	public:
		PoolReporter() { Reset(); }

		void	Reset() { totalCnt = 0; }
		int		TotoalObjCnt() const { return totalCnt; }

	private:
		void OnVisit(const std::string& objName,const int& objSize,const int& objCnt)
		{
			totalCnt += objCnt;
		}

		int totalCnt;
	};

	typedef std::list<CTestMemPool1*> POOLS1;
	typedef std::list<CTestMemPool2*> POOLS2;
	////////////////////////////////////////////////////////////////////////// 테스트 시작
	srand(::timeGetTime());
	POOLS1 poolList1;
	POOLS2 poolList2;
	PoolReporter reporter;

	int randCount1(100 + rand() % 100000);
	for (int i = 0; i < randCount1; ++i)
	{
		poolList1.push_back(new CTestMemPool1);
	}
	EXPECT_EQ(randCount1, CTestMemPool1::TotalCount());

	int randCount2(100 + rand() % 100000);
	for (int i = 0; i < randCount2; ++i)
	{
		poolList2.push_back(new CTestMemPool2);
	}
	EXPECT_EQ(randCount2, CTestMemPool2::TotalCount());

	MemPoolObjCounter::Instance().Visit(reporter);

	EXPECT_EQ(randCount1 + randCount2, reporter.TotoalObjCnt());

	const int delCnt1(rand() % randCount1);
	{
		POOLS1::iterator it = poolList1.begin();
		for (int i = 0; i < delCnt1; ++i)
		{
			assert(it != poolList1.end());
			CTestMemPool1* const obj(*it);
			assert(NULL!=obj);
			delete obj;
			++it;
		}
		poolList1.erase(poolList1.begin(), it);
		EXPECT_EQ(randCount1 - delCnt1, CTestMemPool1::TotalCount());
	}

	const int delCnt2(rand() % randCount2);
	{
		POOLS2::iterator it = poolList2.begin();
		for (int i = 0; i < delCnt2; ++i)
		{
			assert(it != poolList2.end());
			CTestMemPool2* const obj(*it);
			assert(NULL!=obj);
			delete obj;
			++it;
		}
		poolList2.erase(poolList2.begin(), it);
		EXPECT_EQ(randCount2 - delCnt2, CTestMemPool2::TotalCount());
	}

	reporter.Reset();
	MemPoolObjCounter::Instance().Visit(reporter);
	EXPECT_EQ(randCount1 + randCount2 - delCnt1 - delCnt2, reporter.TotoalObjCnt());

	for each (CTestMemPool1* const obj in poolList1)
	{
		assert(NULL!=obj);
		delete obj;
	}

	for each (CTestMemPool2* const obj in poolList2)
	{
		assert(NULL!=obj);
		delete obj;
	}

	reporter.Reset();
	MemPoolObjCounter::Instance().Visit(reporter);
	EXPECT_EQ(0, reporter.TotoalObjCnt());
	CTestMemPool1::release();
	CTestMemPool2::release();
}