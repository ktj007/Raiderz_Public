#include "stdafx.h"
#include "TestCommonFixture.h"
#include "XFieldInfo.h"
#include "XRestrictedAreaChecker.h"

SUITE(RestrictedAreaChecker)
{
	class FRestrictedAreaChecker : public XRestrictedAreaChecker
	{
	public:
		XFieldInfo			m_TestFieldInfo;
		CSFieldFileInfo		m_TestFieldFileInfo;

	public:
		FRestrictedAreaChecker()	
		{
			m_TestFieldInfo.m_rtBoundary.left		= 1000.0f;
			m_TestFieldInfo.m_rtBoundary.top		= 1000.0f;
			m_TestFieldInfo.m_rtBoundary.right		= 20000.0f;
			m_TestFieldInfo.m_rtBoundary.bottom		= 20000.0f;

			m_TestFieldFileInfo.bDynamic			= true; 
		}
		~FRestrictedAreaChecker()	{}
	};

	TEST_FIXTURE(FRestrictedAreaChecker, TestRestrictedArea_in_Min_X)
	{
		_UpdateRestrictedArea(0.5f, vec3(1200.0f, 5000.0f, 0.0f), &m_TestFieldInfo, &m_TestFieldFileInfo);

		CHECK(m_fRestrictedAreaOutTime > 0.0f);
	}

	TEST_FIXTURE(FRestrictedAreaChecker, TestRestrictedArea_in_Min_Y)
	{
		_UpdateRestrictedArea(0.5f, vec3(5000.0f, 2000.0f, 0.0f), &m_TestFieldInfo, &m_TestFieldFileInfo);

		CHECK(m_fRestrictedAreaOutTime > 0.0f);
	}
	
	TEST_FIXTURE(FRestrictedAreaChecker, TestRestrictedArea_in_Max_X)
	{
		_UpdateRestrictedArea(0.5f, vec3(8000.0f, 5000.0f, 0.0f), &m_TestFieldInfo, &m_TestFieldFileInfo);

		CHECK(m_fRestrictedAreaOutTime > 0.0f);
	}
	
	TEST_FIXTURE(FRestrictedAreaChecker, TestRestrictedArea_in_Max_Y)
	{
		_UpdateRestrictedArea(0.5f, vec3(5000.0f, 9000.0f, 0.0f), &m_TestFieldInfo, &m_TestFieldFileInfo);

		CHECK(m_fRestrictedAreaOutTime > 0.0f);
	}

	TEST_FIXTURE(FRestrictedAreaChecker, TestRestrictedArea_in_out)
	{
		_UpdateRestrictedArea(0.5f, vec3(1200.0f, 3000.0f, 0.0f), &m_TestFieldInfo, &m_TestFieldFileInfo);
		_UpdateRestrictedArea(0.5f, vec3(6500.0f, 6500.0f, 0.0f), &m_TestFieldInfo, &m_TestFieldFileInfo);

		CHECK_EQUAL(m_fRestrictedAreaOutTime, 0.0f);
	}

	TEST_FIXTURE(FRestrictedAreaChecker, TestNotDynamicField)
	{
		CSFieldFileInfo		TestFieldFileInfo;
		_UpdateRestrictedArea(0.5f, vec3(1200.0f, 5000.0f, 0.0f), &m_TestFieldInfo, &TestFieldFileInfo);

		CHECK_EQUAL(m_fRestrictedAreaOutTime, 0.0f);
	}
}
