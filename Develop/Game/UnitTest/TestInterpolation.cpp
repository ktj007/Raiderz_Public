#include "stdafx.h"
#include "XInterpolation.h"

SUITE(Interpolation)
{
	class FDirectionInterpolation
	{
	public:
		FDirectionInterpolation()
		{

		}
		~FDirectionInterpolation()
		{

		}
		bool Setup(vec3 vFrom, vec3 vTo, float fTime)
		{
			vFrom.Normalize();
			vTo.Normalize();

			return m_Interpo.Setup(vFrom, vTo, fTime);
		}

		void CHECK_ForCase1()
		{
			for (int i = 0; i < 10; i++)
			{
				vec3 vOldValue = m_Interpo.GetValue();

				m_Interpo.Interpolate(0.1f);

				vec3 vNewValue = m_Interpo.GetValue();

				CHECK(vOldValue.x <= vNewValue.x);
				CHECK(vOldValue.y >= vNewValue.y);
				CHECK_CLOSE(0.0f, vNewValue.z, 0.001f);
			}
		}

		XDirectionInterpolation	m_Interpo;
	};

	TEST_FIXTURE(FDirectionInterpolation, TestDirectionInterpolation_Setup)
	{
		vec3 vFrom = vec3(0.0f, 1.0f, 0.0f);
		vec3 vTo = vec3(1.0f, 1.0f, 0.0f);
		float fTime = 1.0f;

		bool ret = Setup(vFrom, vTo, fTime);
		CHECK_EQUAL(true, ret);

		vFrom.Normalize();
		vTo.Normalize();

		CHECK_CLOSE(vFrom.x, m_Interpo.GetFrom().x, 0.001f);
		CHECK_CLOSE(vFrom.y, m_Interpo.GetFrom().y, 0.001f);
		CHECK_CLOSE(vFrom.z, m_Interpo.GetFrom().z, 0.001f);

		CHECK_CLOSE(vTo.x, m_Interpo.GetTo().x, 0.001f);
		CHECK_CLOSE(vTo.y, m_Interpo.GetTo().y, 0.001f);
		CHECK_CLOSE(vTo.z, m_Interpo.GetTo().z, 0.001f);

		CHECK_CLOSE(vFrom.x, m_Interpo.GetValue().x, 0.001f);
		CHECK_CLOSE(vFrom.y, m_Interpo.GetValue().y, 0.001f);
		CHECK_CLOSE(vFrom.z, m_Interpo.GetValue().z, 0.001f);

	}

	TEST_FIXTURE(FDirectionInterpolation, TestDirectionInterpolation_Setup_Return_Negative)
	{
		bool ret = Setup(vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), -1.0f);
		CHECK_EQUAL(false, ret);
	}

	TEST_FIXTURE(FDirectionInterpolation, TestDirectionInterpolation_Interpolate_Case1)
	{
		float fTime = 1.0f;

		Setup(vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), fTime);

		CHECK_ForCase1();
	}

	TEST_FIXTURE(FDirectionInterpolation, TestDirectionInterpolation_Interpolate_Case2)
	{
		float fTime = 10.0f;

		Setup(vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), fTime);

		CHECK_ForCase1();
	}

	TEST_FIXTURE(FDirectionInterpolation, TestDirectionInterpolation_Interpolate_Return)
	{
		float fTime = 1.0f;

		Setup(vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), fTime);

		CHECK_EQUAL(false, m_Interpo.Interpolate(0.1f));
		CHECK_EQUAL(false, m_Interpo.Interpolate(0.1f));
		CHECK_EQUAL(false, m_Interpo.Interpolate(0.1f));
		CHECK_EQUAL(true, m_Interpo.Interpolate(1.0f));
	}

	TEST_FIXTURE(FDirectionInterpolation, TestDirectionInterpolation_Interpolate_GetValue)
	{
		vec3 vFrom = vec3(0.0f, 1.0f, 0.0f);
		vec3 vTo = vec3(1.0f, 1.0f, 0.0f);
		vFrom.Normalize();
		vTo.Normalize();

		float fTime = 1.0f;

		m_Interpo.Setup(vFrom, vTo, fTime);

		CHECK_CLOSE(vFrom.x, m_Interpo.GetValue().x, 0.00f);
		CHECK_CLOSE(vFrom.y, m_Interpo.GetValue().y, 0.00f);
		CHECK_CLOSE(vFrom.z, m_Interpo.GetValue().z, 0.00f);

		m_Interpo.Interpolate(1000.0f);

		CHECK_CLOSE(vTo.x, m_Interpo.GetValue().x, 0.00f);
		CHECK_CLOSE(vTo.y, m_Interpo.GetValue().y, 0.00f);
		CHECK_CLOSE(vTo.z, m_Interpo.GetValue().z, 0.00f);
	}

}