#include "stdafx.h"
#include "SUnitTest.h"
#include "GDBTaskExecCounter.h"

SUITE(DBTaskCounter)
{
	class MockDBTaskExecCount : public GDBTaskExecCounter
	{
	public :
		
	};

	struct FIXTURE_TASKCOUNTER
	{
		MockDBTaskExecCount Counter;
	};

	TEST_FIXTURE(FIXTURE_TASKCOUNTER, NoExistTaskCount)
	{
		CHECK_EQUAL(0, Counter.GetCount(SDBTID_ACCOUNTINSERT).nCnt);
		CHECK_EQUAL(0, Counter.GetCount(SDBTID_ACCOUNTINSERT).nWorkTm);
	}

	TEST_FIXTURE(FIXTURE_TASKCOUNTER, CountNewTask)
	{
		const int64 nWorkTm = 121;

		Counter.AddCount(SDBTID_ACCOUNTINSERT, nWorkTm);

		CHECK_EQUAL(1, Counter.GetCount(SDBTID_ACCOUNTINSERT).nCnt);
		CHECK_EQUAL(nWorkTm, Counter.GetCount(SDBTID_ACCOUNTINSERT).nWorkTm);
	}

	TEST_FIXTURE(FIXTURE_TASKCOUNTER, AddOneMore)
	{
		const int64 nWorkTm1 = 10;
		const int64 nWorkTm2 = 12;

		Counter.AddCount(SDBTID_ACCOUNTINSERT, nWorkTm1);
		Counter.AddCount(SDBTID_ACCOUNTINSERT, nWorkTm2);

		CHECK_EQUAL(2, Counter.GetCount(SDBTID_ACCOUNTINSERT).nCnt);
		CHECK_EQUAL(nWorkTm1 + nWorkTm2, Counter.GetCount(SDBTID_ACCOUNTINSERT).nWorkTm);
	}

	TEST_FIXTURE(FIXTURE_TASKCOUNTER, GetDecrSortedList_One)
	{
		Counter.AddCount(SDBTID_ACCOUNTINSERT, 0);

		GDBTaskSortedList lst = Counter.GetDecrSortedList();
		CHECK_EQUAL(1, lst.size());
		if (1 == lst.size())
		{
			CHECK_EQUAL(1, lst.begin()->second.nCnt);
		}
	}

	TEST_FIXTURE(FIXTURE_TASKCOUNTER, GetDecrSortedList_Two)
	{
		Counter.AddCount(SDBTID_ACCOUNTINSERT, 0);
		Counter.AddCount(SDBTID_CHARADDMONEY, 0);
		Counter.AddCount(SDBTID_CHARADDMONEY, 0);

		GDBTaskSortedList lst = Counter.GetDecrSortedList();
		CHECK_EQUAL(2, lst.size());
		if (2 != lst.size())
			return;

		GDBTaskSortedList::iterator it = lst.begin();

		CHECK_EQUAL(SDBTID_CHARADDMONEY, it->first);
		CHECK_EQUAL(2, it->second.nCnt);

		++it;
		CHECK_EQUAL(SDBTID_ACCOUNTINSERT, it->first);
		CHECK_EQUAL(1, it->second.nCnt);
	}

	TEST_FIXTURE(FIXTURE_TASKCOUNTER, GetDecrSortedList_Three)
	{
		Counter.AddCount(SDBTID_ACCOUNTINSERT, 0);
		Counter.AddCount(SDBTID_CHARADDMONEY, 0);
		Counter.AddCount(SDBTID_CHARADDMONEY, 0);
		Counter.AddCount(SDBTID_CHARGETLOOKLIST, 0);
		Counter.AddCount(SDBTID_CHARGETLOOKLIST, 0);
		Counter.AddCount(SDBTID_CHARGETLOOKLIST, 0);

		GDBTaskSortedList lst = Counter.GetDecrSortedList();
		CHECK_EQUAL(3, lst.size());
		if (3 != lst.size())
			return;

		GDBTaskSortedList::iterator it = lst.begin();

		CHECK_EQUAL(SDBTID_CHARGETLOOKLIST, it->first);
		CHECK_EQUAL(3, it->second.nCnt);

		++it;
		CHECK_EQUAL(SDBTID_CHARADDMONEY, it->first);
		CHECK_EQUAL(2, it->second.nCnt);

		++it;
		CHECK_EQUAL(SDBTID_ACCOUNTINSERT, it->first);
		CHECK_EQUAL(1, it->second.nCnt);
	}	

	TEST_FIXTURE(FIXTURE_TASKCOUNTER, GetDecrSortedList_Four)
	{
		Counter.AddCount(SDBTID_ACCOUNTINSERT, 0);
		Counter.AddCount(SDBTID_CHARADDMONEY, 0);
		Counter.AddCount(SDBTID_CHARADDMONEY, 0);
		Counter.AddCount(SDBTID_CHARGETLOOKLIST, 0);
		Counter.AddCount(SDBTID_CHARGETLOOKLIST, 0);
		Counter.AddCount(SDBTID_CHARGETLOOKLIST, 0);
		Counter.AddCount(SDBTID_CHARMINUSMONEY, 0);
		Counter.AddCount(SDBTID_CHARMINUSMONEY, 0);
		Counter.AddCount(SDBTID_CHARMINUSMONEY, 0);
		Counter.AddCount(SDBTID_CHARMINUSMONEY, 0);

		GDBTaskSortedList lst = Counter.GetDecrSortedList();
		CHECK_EQUAL(4, lst.size());
		if (4 != lst.size())
			return;

		GDBTaskSortedList::iterator it = lst.begin();

		CHECK_EQUAL(SDBTID_CHARMINUSMONEY, it->first);
		CHECK_EQUAL(4, it->second.nCnt);

		++it;
		CHECK_EQUAL(SDBTID_CHARGETLOOKLIST, it->first);
		CHECK_EQUAL(3, it->second.nCnt);

		++it;
		CHECK_EQUAL(SDBTID_CHARADDMONEY, it->first);
		CHECK_EQUAL(2, it->second.nCnt);

		++it;
		CHECK_EQUAL(SDBTID_ACCOUNTINSERT, it->first);
		CHECK_EQUAL(1, it->second.nCnt);
	}	
}