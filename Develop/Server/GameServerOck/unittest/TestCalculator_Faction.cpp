#include "stdafx.h"
#include "GFactionCalculator.h"

SUITE(FactionCalculator)
{
	struct Fixture
	{
		GFactionCalculator m_factionCalculator;
	};

	TEST_FIXTURE(Fixture, CalcFactionToCheckTradable)
	{
		uint16 nTestFactionQuantity;

		nTestFactionQuantity = CSFactionCalculator::GetMinQuantity();
		CHECK_EQUAL(false, m_factionCalculator.CheckTradable(nTestFactionQuantity));

		nTestFactionQuantity = CSFactionCalculator::GetBadMaxQuantity();
		CHECK_EQUAL(false, m_factionCalculator.CheckTradable(nTestFactionQuantity));

		nTestFactionQuantity = CSFactionCalculator::GetNormalMinQuantity();
		CHECK_EQUAL(true, m_factionCalculator.CheckTradable(nTestFactionQuantity));

		nTestFactionQuantity = CSFactionCalculator::GetGoodMaxQuantity();
		CHECK_EQUAL(true, m_factionCalculator.CheckTradable(nTestFactionQuantity));

		nTestFactionQuantity = CSFactionCalculator::GetMaxQuantity();
		CHECK_EQUAL(true, m_factionCalculator.CheckTradable(nTestFactionQuantity));
	}
}
