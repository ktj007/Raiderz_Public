#include "stdafx.h"
#include "CSGoogleMockListener.h"
#include "CurrentTest.h"
#include "TestResults.h"
#include "TestDetails.h"

void CSGoogleMockListener::InitListener(void)
{
	::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();

	// 기존 리스너 제거
	delete listeners.Release(listeners.default_result_printer());

	// 새 리스너 추가
	listeners.Append(new CSGoogleMockListener);
}

void CSGoogleMockListener::OnTestPartResult(const ::testing::TestPartResult& test_part_result) 
{
	// 실패 시, UnitTest++ 결과로 출력
	if (test_part_result.failed())
	{
		UnitTest::CurrentTest::Results()->OnTestFailure(
			UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), test_part_result.line_number()), 
			test_part_result.summary()
			);
	}
}
