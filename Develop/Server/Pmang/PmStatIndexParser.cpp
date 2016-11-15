#include "stdafx.h"
#include "PmStatIndexParser.h"
#include "SDef.h"
#include "MDebug.h"
#include <regex>

template <typename R, typename T>
void SetResult(R& result, int idx, T& n)
{
	n = (T)_wtol(wstring(result[idx]).c_str());
}

PmStatIndexParser::PmStatIndexParser(wstring strStatIndex)
: m_strStatIndex(strStatIndex)
{
	// do nothing
}

bool PmStatIndexParser::Parser()
{
	enum TOKEN
	{
		ALL_STREAM = 0,
		REGION_CODE,
		AGE_CODE,
		GENDER_CODE,
		YEAR_OR_BIRTH,

		TOKEN_MAX
	};


	// ÆÐÅÏ ¸ÅÄª
	const tr1::wregex pattern(L"(\\d{2})(\\d{1})(\\d{1})(\\d{4})");
	tr1::match_results<wstring::const_iterator> result;

	if (!tr1::regex_match(m_strStatIndex, result, pattern))
	{
		return false;
	}

	// ¸ÅÄª °á°ú Á¤¸®
	VALID_RET(TOKEN_MAX == result.size(), false);

	SetResult(result, REGION_CODE,			m_statIndex.nRegionCode);
	SetResult(result, AGE_CODE,				m_statIndex.nAgeCode);
	SetResult(result, GENDER_CODE,			m_statIndex.nGenderCode);
	SetResult(result, YEAR_OR_BIRTH,		m_statIndex.nYearOfBirth);	


	return true;
}

PmStatIndex PmStatIndexParser::GetStatIndex() const
{
	return m_statIndex;
}
