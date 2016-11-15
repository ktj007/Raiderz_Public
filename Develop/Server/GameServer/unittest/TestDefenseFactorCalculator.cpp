#include "stdafx.h"
#include "GDefenseFactorCalculator.h"

SUITE(DefenseFactorCalculator)
{
	class TestDefenseFactorCalculator : public GDefenseFactorCalculator
	{
	public:
		using GDefenseFactorCalculator::_CalcAPFactor;
	};

	class FDefenseFactorCalculator
	{
	public:
		FDefenseFactorCalculator()
		{

		}
		~FDefenseFactorCalculator()
		{

		}
		TestDefenseFactorCalculator	m_Calc;
	};

	TEST_FIXTURE(FDefenseFactorCalculator, TestDefenseFactorCalculator__CalcAPFactor)
	{
		int nAP = 10;
		int nAttackerLevel = 2;
		float fLevelDiffFactor = 1.1f;
		float fDefenseMaxConst = 0.2f;

		float fAPFactor = m_Calc._CalcAPFactor(nAP, nAttackerLevel, fLevelDiffFactor, fDefenseMaxConst);

		CHECK_CLOSE(1.07f, fAPFactor, 0.001f);
	}
}