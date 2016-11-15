#pragma once

#include "AOptionSetBase.h"


template<typename T>
class AOptionSet: public AOptionSetBase
{
public:
	AOptionSet(wstring option, wstring description, T& outValue)
		: AOptionSetBase(option, description)
		, m_outValue(outValue)
	{
	}

	// bool SetArgument() - 구현안함. 템플릿 특수화가 구현되지 않은 타입 사용 시 빌드에러를 낸다.

private:
	T& m_outValue;
};


// 불리언 옵션 - 옵션값을 읽지 않는다. 옵션 스위치만 있어도 true 설정
template<>
class AOptionSet<bool>: public AOptionSetBase
{
public:
	AOptionSet(wstring option, wstring description, bool& outValue)
		: AOptionSetBase(option, description)
		, m_outValue(outValue)
	{
		m_outValue = false;
	}

	virtual bool SetArgument(wstring arguments, wstring splitter)
	{
		// 옵션값이 없거나 true, yes, enable, allow 값을 가지면 true. 그 외 모두 false
		tr1::wregex pattern(L" *(true|t|yes|y|enable|e|allow|a)? *", tr1::regex_constants::icase);

		tr1::match_results<wstring::const_iterator> result;
		if (!tr1::regex_match(arguments, result, pattern))
		{
			return false;
		}

		m_outValue = true;
		return true;	
	}

private:
	bool& m_outValue;
};


// 문자열 옵션 - 옵션값을 통채로 읽는다.
template<>
class AOptionSet<wstring>: public AOptionSetBase
{
public:
	AOptionSet(wstring option, wstring description, wstring& outValue)
		: AOptionSetBase(option, description)
		, m_outValue(outValue)
	{
	}

	virtual bool SetArgument(wstring arguments, wstring splitter)
	{
		m_outValue = arguments;
		return true;	
	}

private:
	wstring& m_outValue;
};


// 정수 옵션 - 숫자로 읽는다. 숫자가 아닌 값 읽으면 0.
template<>
class AOptionSet<int>: public AOptionSetBase
{
public:
	AOptionSet(wstring option, wstring description, int& outValue)
		: AOptionSetBase(option, description)
		, m_outValue(outValue)
	{
		m_outValue = 0;
	}

	virtual bool SetArgument(wstring arguments, wstring splitter)
	{
		m_outValue = _wtoi(arguments.c_str());
		return true;
	}

private:
	int& m_outValue;
};


// 문자열 목록 옵션 - 스플리터 설정에 따라, 옵션값을 분리해 읽는다.
template<>
class AOptionSet<vector<wstring> >: public AOptionSetBase
{
public:
	AOptionSet(wstring option, wstring description, vector<wstring>& outValue)
		: AOptionSetBase(option, description)
		, m_outValue(outValue)
	{
	}

	virtual bool SetArgument(wstring arguments, wstring splitter)
	{
		return Split(arguments, splitter);
	}

	virtual bool SetSplitArgument(wstring argument)
	{
		m_outValue.push_back(argument);
		return true;
	}

private:
	vector<wstring>& m_outValue;
};
