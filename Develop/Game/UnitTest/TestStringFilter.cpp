#include "stdafx.h"
#include "CSStringFilter.h"

SUITE(StringFilter)
{
	struct Fixture
	{
		Fixture()
		{
			m_filter = GetStringFilter();
			m_filter->LoadFromStringDB(_T("³ª»Û³Ñ\r\naibeast\r\nEND"));
		}
		~Fixture()
		{
			m_filter->Clear();
		}

		CSStringFilter* m_filter;
	};

	TEST_FIXTURE(Fixture, TestStringFilter_Success)
	{
		tstring strOriginString = _T("¾È³çÇÏ¼¼¿© aibeastÀÔ´Ï´Ù. ai@#$beast");

		bool bRet = m_filter->IsValidStr(strOriginString.c_str());
		CHECK_EQUAL(false, bRet);

		tstring strFilteredString = m_filter->Filter(strOriginString.c_str());
		tstring strExpectString = _T("¾È³çÇÏ¼¼¿© *******ÀÔ´Ï´Ù. **********");

		bRet = (strExpectString == strFilteredString);
		CHECK_EQUAL(true, bRet);
	}

	TEST_FIXTURE(Fixture, TestStringFilter_Success2)
	{
		tstring strOriginString = _T("!@#aibeast");

		bool bRet = m_filter->IsValidStr(strOriginString.c_str());
		CHECK_EQUAL(false, bRet);

		tstring strFilteredString = m_filter->Filter(strOriginString.c_str());
		tstring strExpectString = _T("!@#*******");

		bRet = (strExpectString == strFilteredString);
		CHECK_EQUAL(true, bRet);
	}

	TEST_FIXTURE(Fixture, TestStringFilter_Success3)
	{
		tstring strOriginString = _T("³ª1»Û³Ñ");

		bool bRet = m_filter->IsValidStr(strOriginString.c_str());
		CHECK_EQUAL(false, bRet);

		tstring strFilteredString = m_filter->Filter(strOriginString.c_str());
		tstring strExpectString = _T("****");

		bRet = (strExpectString == strFilteredString);
		CHECK_EQUAL(true, bRet);
	}

	TEST_FIXTURE(Fixture, TestStringFilter_NotFiltered)
	{
		tstring strOriginString = _T("Âø1ÇÑ³Ñ");

		bool bRet = m_filter->IsValidStr(strOriginString.c_str());
		CHECK_EQUAL(true, bRet);

		tstring strFilteredString = m_filter->Filter(strOriginString.c_str());
		tstring strExpectString = _T("Âø1ÇÑ³Ñ");

		bRet = (strExpectString == strFilteredString);
		CHECK_EQUAL(true, bRet);
	}

}
