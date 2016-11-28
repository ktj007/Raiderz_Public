#pragma once

#include <gmock/gmock.h>

// GoogleMock 실패 메시지 UnitTest++ 리포터로 출력 클래스
class CSGoogleMockListener: public testing::EmptyTestEventListener
{
public:
	static void InitListener(void);

	virtual void OnTestPartResult(const ::testing::TestPartResult& test_part_result);	
};
