#include "stdafx.h"
#include "GTestForward.h"
#include "GDespawnReporter.h"

SUITE(DebugReporter)
{
	TEST(Push)
	{
		GDebugReporter reporter;
		reporter.SetRecordLimit(2);

		CHECK_EQUAL(0, reporter.GetRecordQty());

		// 첫번째 레코드 추가
		{
			GDebugReporter::Record r;
			r.uidNPC = MUID(777, 111);
			reporter.Push(r);
		}
		CHECK_EQUAL(1, reporter.GetRecordQty());
		CHECK_EQUAL(MUID(777,111), reporter.GetRecord(0).uidNPC);

		// 두번째 레코드 추가
		{
			GDebugReporter::Record r;
			r.uidNPC = MUID(777, 222);
			reporter.Push(r);
		}
		CHECK_EQUAL(2, reporter.GetRecordQty());
		CHECK_EQUAL(MUID(777,111), reporter.GetRecord(0).uidNPC);
		CHECK_EQUAL(MUID(777,222), reporter.GetRecord(1).uidNPC);

		// 세번째 레코드 추가
		{
			GDebugReporter::Record r;
			r.uidNPC = MUID(777, 333);
			reporter.Push(r);
		}
		CHECK_EQUAL(2, reporter.GetRecordQty());
		CHECK_EQUAL(MUID(777,222), reporter.GetRecord(0).uidNPC);
		CHECK_EQUAL(MUID(777,333), reporter.GetRecord(1).uidNPC);
	}

	TEST(Log)
	{
		GDebugReporter reporter;
		reporter.SetRecordLimit(2);

		CHECK_EQUAL(0, reporter.GetRecordQty());

		// 첫번째 레코드 추가
		{
			GDebugReporter::Record r;
			r.uidNPC = MUID(777, 111);
			reporter.Push(r);
		}
		// 두번째 레코드 추가
		{
			GDebugReporter::Record r;
			r.uidNPC = MUID(777, 222);
			reporter.Push(r);
		}
		
		reporter.Log(MUID(777, 111), L"test1_1");
		reporter.Log(MUID(777, 111), L"test1_2");
		reporter.Log(MUID(777, 222), L"test2_1");
		reporter.Log(MUID(777, 222), L"test2_2");

		CHECK_EQUAL(2, reporter.GetRecordQty());
		CHECK(L"test1_1" == reporter.GetRecord(0).vecLogs[0]);
		CHECK(L"test1_2" == reporter.GetRecord(0).vecLogs[1]);
		CHECK(L"test2_1" == reporter.GetRecord(1).vecLogs[0]);
		CHECK(L"test2_2" == reporter.GetRecord(1).vecLogs[1]);
	}

	TEST(FlushToFile)
	{
		GDebugReporter reporter;
		reporter.SetRecordLimit(2);

		// 첫번째 레코드 추가
		{
			GDebugReporter::Record r;
			r.strName = L"NPC_1";
			r.uidNPC = MUID(777, 111);
			reporter.Push(r);
		}
		// 두번째 레코드 추가
		{
			GDebugReporter::Record r;
			r.strName = L"NPC_2";
			r.uidNPC = MUID(777, 222);
			reporter.Push(r);
		}

		reporter.Log(MUID(777, 111), L"test1_1");
		reporter.Log(MUID(777, 111), L"test1_2");
		reporter.Log(MUID(777, 222), L"test2_1");
		reporter.Log(MUID(777, 222), L"test2_2");
		reporter.FlushToFile();
	}
}