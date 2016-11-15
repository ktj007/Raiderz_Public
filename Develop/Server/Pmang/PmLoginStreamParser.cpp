#include "stdafx.h"
#include "PmLoginStreamParser.h"
#include "SDef.h"
#include "MDebug.h"
#include <regex>
#include "MDebug.h"

template <typename R, typename T>
void SetResult(R& result, int idx, T& n)
{
	n = (T)_wtol(wstring(result[idx]).c_str());
}

template <typename R>
void SetResult(R& result, int idx, wstring& s)
{
	s = result[idx];
}


PmLoginStreamParser::PmLoginStreamParser(wstring strLoginStream)
: m_strLoginStream(strLoginStream)
, m_timeStamp(0)
{
	// do nothing
}

bool PmLoginStreamParser::Parse()
{
	// 패턴 정의: UserSerial|UserID|TimeStamp|AppendedData|HashedValue StatIndex
	const tr1::wregex pattern(L"((\\d+)\\|([^| ]{1,12})\\|(\\d+)((?:\\|[^| ]+)*))\\|([a-f\\d]{32}) (\\d{2})(\\d{1})(\\d{1})(\\d{4})");

	enum TOKEN
	{
		ALL_STREAM = 0,
		HASH_INPUT_DATA,
		USER_SERIAL,
		USER_ID,
		TIME_STAMP,
		APPENDED_DATA_STREAM,
		HASHED_VALUE,
		REGION_CODE,
		AGE_CODE,
		GENDER_CODE,
		YEAR_OR_BIRTH,

		TOKEN_MAX
	};

	
	// 패턴 매칭
	tr1::match_results<wstring::const_iterator> result;

	if (!tr1::regex_match(m_strLoginStream, result, pattern))
	{
		return false;
	}

	
	// 매칭 결과 정리
	VALID_RET(TOKEN_MAX == result.size(), false);

	SetResult(result, HASH_INPUT_DATA,		m_strHashKeyStream);
	SetResult(result, TIME_STAMP,			m_timeStamp);
	SetResult(result, HASHED_VALUE,			m_strHashedValue);

	SetResult(result, USER_SERIAL,			m_pmangUserData.strUserSerial);
	SetResult(result, USER_ID,				m_pmangUserData.strUserID);

	SetResult(result, REGION_CODE,			m_pmangUserData.statIndex.nRegionCode);	
	SetResult(result, AGE_CODE,				m_pmangUserData.statIndex.nAgeCode);
	SetResult(result, GENDER_CODE,			m_pmangUserData.statIndex.nGenderCode);
	SetResult(result, YEAR_OR_BIRTH,		m_pmangUserData.statIndex.nYearOfBirth);	

	
	// statIndex 유효성 검사
	if (!m_pmangUserData.statIndex.IsValid())
	{
		mlog3("Not allowed statIndex (Region:%d, Age:%d, Gender%d)\n", m_pmangUserData.statIndex.nRegionCode, m_pmangUserData.statIndex.nAgeCode, m_pmangUserData.statIndex.nGenderCode);
		return false;
	}


	// 부분 패턴 재해석
	SetResult(result, APPENDED_DATA_STREAM,	m_strAppendedStream);

	if (!ParseAppendedData())
	{
		return false;
	}


	return true;
}

bool PmLoginStreamParser::ParseAppendedData()
{	
	// 패턴 정의: "|NEOWIZ_PCBID|SERVICE_TYPE|USER_TYPE", 값없음으로 "_" 사용
	const tr1::wregex pattern(L"\\|(?:_|([^| ]{1,12}))\\|(?:_|([^| ]{1,12}))\\|(\\d{4})");

	enum TOKEN
	{
		ALL_STREAM = 0,
		NEOWIZ_PCBID,
		SERVICE_TYPE,
		USER_TYPE,

		TOKEN_MAX
	};

	// 패턴 매칭
	tr1::match_results<wstring::const_iterator> result;

	if (!tr1::regex_match(m_strAppendedStream, result, pattern))
	{
		return false;
	}


	// 매칭 결과 정리
	VALID_RET(TOKEN_MAX == result.size(), false);

	SetResult(result, NEOWIZ_PCBID, m_pmangUserData.strPCBID);
	SetResult(result, SERVICE_TYPE, m_pmangUserData.strServiceType);
	SetResult(result, USER_TYPE, m_pmangUserData.nUserType);

	
	return true;
}

wstring PmLoginStreamParser::GetHashKeyStream() const
{
	return m_strHashKeyStream;
}

wstring PmLoginStreamParser::GetHashedValue() const
{
	return m_strHashedValue;
}

wstring PmLoginStreamParser::GetLoginStream() const
{
	return m_strLoginStream;
}

time_t PmLoginStreamParser::GetTimeStamp() const
{
	return m_timeStamp;
}

wstring PmLoginStreamParser::GetOptionStream() const
{
	return m_strAppendedStream;
}

PmUserData PmLoginStreamParser::GetUserData() const
{
	return m_pmangUserData;
}
