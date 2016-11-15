#include "stdafx.h"
#include "GStrings.h"

SUITE(Strings)
{
	struct FStringNumbersToIntVector
	{
		FStringNumbersToIntVector()
		{
			m_bScope = false;
		}

		wstring m_strValue;
		vector<int> m_vecInt;
		bool m_bScope;
	};

	TEST_FIXTURE(FStringNumbersToIntVector, OneNumber)
	{
		m_strValue = L"1";
		GStrings::StringNumbersToIntVector(m_strValue, m_vecInt);

		CHECK_EQUAL(1, m_vecInt[0]);
		CHECK_EQUAL(1, m_vecInt.size());
	}

	TEST_FIXTURE(FStringNumbersToIntVector, PluralNumber)
	{
		m_strValue = L"1,2,3";
		GStrings::StringNumbersToIntVector(m_strValue, m_vecInt);

		CHECK_EQUAL(1, m_vecInt[0]);
		CHECK_EQUAL(2, m_vecInt[1]);
		CHECK_EQUAL(3, m_vecInt[2]);
		CHECK_EQUAL(3, m_vecInt.size());
	}

	TEST_FIXTURE(FStringNumbersToIntVector, ScopeNumber)
	{
		m_strValue = L"1~4";
		GStrings::StringNumbersToIntVector(m_strValue, m_vecInt);

		CHECK_EQUAL(1, m_vecInt[0]);
		CHECK_EQUAL(2, m_vecInt[1]);
		CHECK_EQUAL(3, m_vecInt[2]);
		CHECK_EQUAL(4, m_vecInt[3]);
		CHECK_EQUAL(4, m_vecInt.size());
	}
}