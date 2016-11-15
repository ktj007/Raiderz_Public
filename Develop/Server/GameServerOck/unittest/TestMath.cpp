#include "StdAfx.h"
#include "GMath.h"
#include "SUnitTestUtil.h"

SUITE(Math)
{
	vec3 vDefaultDir(0,1,0);
	TEST(GetAnglePos_Invalid)
	{
		const vec3 vPoint = vec3(1000.0f, 1000.0f, 1000.0f);
		const float fDistance = 100.0f;
		vector<vec3> vecPos;

		int nAngleQty = -1;
		GMath::GetAnglePos(vPoint, vDefaultDir, fDistance, nAngleQty, vecPos);
		TVALID_EQ(0, vecPos.size());
		
		nAngleQty = 0;
		GMath::GetAnglePos(vPoint, vDefaultDir, fDistance, nAngleQty, vecPos);
		TVALID_EQ(0, vecPos.size());
	}

	TEST(GetAnglePos_2Angle)
	{
		const vec3 vPoint = vec3(1000.0f, 1000.0f, 1000.0f);
		const float fDistance = 100.0f;
		const int nAngleQty = 2;
		vector<vec3> vecPos;
		GMath::GetAnglePos(vPoint, vDefaultDir, fDistance, nAngleQty, vecPos);

		TVALID_EQ(2, vecPos.size());
		CHECK_EQUAL(true, vecPos[0].IsEqual(vec3(1000.0f, 1100.0f, 1000.0f)));
		CHECK_EQUAL(true, vecPos[1].IsEqual(vec3(1000.0f, 900.0f, 1000.0f)));
	}

	TEST(GetAnglePos_4Angle)
	{
		const vec3 vPoint = vec3(1000.0f, 1000.0f, 1000.0f);
		const float fDistance = 100.0f;
		const int nAngleQty = 4;
		vector<vec3> vecPos;
		GMath::GetAnglePos(vPoint, vDefaultDir, fDistance, nAngleQty, vecPos);

		TVALID_EQ(4, vecPos.size());
		CHECK_EQUAL(true, vecPos[0].IsEqual(vec3(1100.0f, 1000.0f, 1000.0f)));
		CHECK_EQUAL(true, vecPos[1].IsEqual(vec3(1000.0f, 1100.0f, 1000.0f)));
		CHECK_EQUAL(true, vecPos[2].IsEqual(vec3(900.0f, 1000.0f, 1000.0f)));
		CHECK_EQUAL(true, vecPos[3].IsEqual(vec3(1000.0f, 900.0f, 1000.0f)));
	}

	TEST(GetMinRadiusFromCapsule)
	{
		MCapsule cap(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 100.0f), 50.0f);
		cap.CalcHeight();

		CHECK_CLOSE(50.0f, GMath::GetActorMinRadiusFromCapsule(cap), 0.001f);


		cap = MCapsule(vec3(0.0f, 100.0f, 50.0f), vec3(0.0f, -200.0f, 100.0f), 50.0f);
		cap.CalcHeight();

		CHECK_CLOSE(50.0f, GMath::GetActorMinRadiusFromCapsule(cap), 0.001f);


		cap = MCapsule(vec3(0.0f, 100.0f, 50.0f), vec3(100.0f, -1000.0f, 100.0f), 70.0f);
		cap.CalcHeight();

		CHECK_CLOSE(70.0f, GMath::GetActorMinRadiusFromCapsule(cap), 0.001f);

		cap = MCapsule(vec3(0.0f, 100.0f, 50.0f), vec3(0.0f, 20.0f, 100.0f), 50.0f);
		cap.CalcHeight();

		CHECK_CLOSE(30.0f, GMath::GetActorMinRadiusFromCapsule(cap), 0.001f);

	}

	class TestRandChecker
	{
	private:
		size_t m_nMatch;

		size_t m_nExpectedMin;
		size_t m_nExpectedMax;
		float GetToler() { return 0.05f; }
	public:
		TestRandChecker() : m_nMatch(0) {}
		void Check(size_t nCount, float fRandMin, float fRandMax, float fExpected)
		{
			m_nMatch = 0;
			float fRate = (fExpected - fRandMin) / (fRandMax - fRandMin);
			m_nExpectedMin = min(0, size_t(nCount * fRate) - size_t(nCount * GetToler()));
			m_nExpectedMax = size_t(nCount * fRate) + size_t(nCount * GetToler());


			for (size_t i = 0; i < nCount; i++)
			{
				float fRand = GMath::RandomNumber(fRandMin, fRandMax);

				// 10%
				if (fRand < fExpected)
				{
					m_nMatch++;
				}
			}
		}
		size_t GetMatch() { return m_nMatch; }
		size_t GetExpectedMin() { return m_nExpectedMin; }
		size_t GetExpectedMax() { return m_nExpectedMax; }

	};

	class FMath_RandomNumber
	{
	public:
		FMath_RandomNumber()
		{

		}
		virtual ~FMath_RandomNumber()
		{

		}
		void CheckAssert()
		{
			size_t nMatch = m_RandChecker.GetMatch();
			size_t nExpectedMin = m_RandChecker.GetExpectedMin();
			size_t nExpectedMax = m_RandChecker.GetExpectedMax();

			CHECK(nMatch >nExpectedMin && nMatch < nExpectedMax);
		}

		TestRandChecker m_RandChecker;
	};

	TEST_FIXTURE(FMath_RandomNumber, TestRandomNumber_Float1)
	{
		const size_t MAX_RAND_COUNT = 1000000;

		m_RandChecker.Check(MAX_RAND_COUNT, 0.0f, 100.0f, 10.0f);
		CheckAssert();
	}

	TEST_FIXTURE(FMath_RandomNumber, TestRandomNumber_Float2)
	{
		const size_t MAX_RAND_COUNT = 1000000;

		m_RandChecker.Check(MAX_RAND_COUNT, 0.0f, 100.0f, 5.0f);
		CheckAssert();
	}
	TEST_FIXTURE(FMath_RandomNumber, TestRandomNumber_Float3)
	{
		const size_t MAX_RAND_COUNT = 1000000;

		m_RandChecker.Check(MAX_RAND_COUNT, 0.0f, 100.0f, 50.0f);
		CheckAssert();
	}

	TEST_FIXTURE(FMath_RandomNumber, TestRandomNumber_Float4)
	{
		const size_t MAX_RAND_COUNT = 1000000;

		m_RandChecker.Check(MAX_RAND_COUNT, 0.0f, 100.0f, 90.0f);
		CheckAssert();
	}

	TEST(NaN_Sample)
	{
		float a = 0.0f;
		float b = 0.0f;
		float c = a/b;
		CHECK(0 == _isnan(a));
		CHECK(0 == _isnan(b));
		CHECK(1 == _isnan(c));
	}
}