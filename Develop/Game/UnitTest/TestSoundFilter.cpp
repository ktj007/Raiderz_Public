#include "stdafx.h"
//#include "XSoundFilter.h"
/*
SUITE(SoundFilter)
{
	class TestSoundFilter : public XSoundFilter
	{
	protected:
		uint32			m_nExpectTime;
		virtual uint32 GetNowTime()
		{
			return m_nExpectTime;
		}
	public:
		TestSoundFilter() : XSoundFilter() {}
		virtual ~TestSoundFilter() {}

		void ExpectCheckTime(uint32 nNowTime)
		{
			m_nExpectTime = nNowTime;
		}
	};

	TEST(TestSoundFilter)
	{
		TestSoundFilter filter;
		XSoundInfo soundEntity;

		filter.ExpectCheckTime(0);
		CHECK_EQUAL(true, filter.Check3DSound(L"sound1", vec3::ZERO, &soundEntity));

		// 거의 동시에 플레이한 사운드는 플레이하지 않는다.
		filter.ExpectCheckTime(10);
		CHECK_EQUAL(false, filter.Check3DSound(L"sound1", vec3::ZERO, &soundEntity));


		filter.ExpectCheckTime(1000);
		CHECK_EQUAL(true, filter.Check3DSound(L"sound1", vec3::ZERO, &soundEntity));

	}


}
*/