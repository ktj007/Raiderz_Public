#ifndef _XUTIL_H
#define _XUTIL_H

#include "XTimer.h"
#include "XGlobal.h"
#include "XSystem.h"
#include "AString.h"

// <Data value="100"/>   <!-- Data=100; -->
// 이런 식의 XML 값을 쉽게 가져오기 위한 Define
// int data; _SET_XMLVALUE_NUM(data, "Data", parentElement); // 이렇게 하면 된다.
#define _SET_XMLVALUE_NUM(_variable, _tagname, _parentElement)						\
{																					\
	MXmlElement* _pDataElement = _parentElement->FirstChildElement(_tagname);		\
	if (_pDataElement) _Attribute(&_variable, _pDataElement, "value");				\
}	

#define _SET_XMLVALUE_STR(_variable, _tagname, _parentElement)						\
{																					\
	MXmlElement* _pDataElement = _parentElement->FirstChildElement(_tagname);		\
	if (_pDataElement) _Attribute(_variable, _pDataElement, "value");				\
}	

#define _SET_XMLVALUE_VEC(_variable, _tagname, _parentElement)						\
{																					\
	wchar_t text[64]=L"";																\
	MXmlElement* _pDataElement = _parentElement->FirstChildElement(_tagname);		\
	if ((_pDataElement) && (_Attribute(text, _pDataElement, "value")))				\
	{																				\
		_variable = vec3(0,0,0);													\
		sscanf(text, "%f %f %f", &_variable.x, &_variable.y, &_variable.z);		\
	}																				\
}

#define _SET_XMLVALUE_COLOR(_variable, _tagname, _parentElement)					\
{																					\
	wchar_t text[64]=L"";																\
	MXmlElement* _pDataElement = _parentElement->FirstChildElement(_tagname);		\
	if ((_pDataElement) && (_Attribute(text, _pDataElement, "value")))				\
	{																				\
		int color[4];																\
		sscanf(text, "%d %d %d %d", &color[0], &color[1], &color[2], &color[3]);	\
		_variable = ((unsigned char)(color[0]) << 24) | ((unsigned char)(color[1]) << 16) | ((unsigned char)(color[2]) <<8) | (unsigned char)(color[3]);		\
	}																				\
}

/// 배열중 최대값 반환
/// @param arr 배열
/// @param N 배열의 개수
template<typename Type>
Type findMax(Type* arr, int N)
{
	Type max = arr[0];
	for (int i = 1; i < N; i++)
	{
		if (max < arr[i]) max = arr[i];
	}
	return max;
}

/// 프레임 카운터
class FrameCounter
{
private:
	unsigned long m_nTime;
	unsigned long m_nFrameCounter;
	unsigned long m_nCurrFrame;
	unsigned long m_nLastTime;
public:
	FrameCounter() : m_nTime(0), m_nFrameCounter(0), m_nCurrFrame(0) { m_nLastTime = XGetNowTime(); }
	int Update()		// 반환값이 fps
	{
		m_nFrameCounter++;
		unsigned long nNowTime = XGetNowTime();

		m_nTime += (nNowTime - m_nLastTime);
		m_nLastTime = nNowTime;

		if (m_nTime >= 1000)
		{
			m_nCurrFrame = m_nFrameCounter;
			m_nFrameCounter = 0;
			m_nTime = 0;
		}

		return m_nCurrFrame;
	}
};

static wstring ConvertMoneyToString(int nMoney)
{
	int nGold = nMoney / 10000;
	nMoney = nMoney - (nGold * 10000);

	int nSilver = nMoney / 100;
	nMoney = nMoney - ( nSilver * 100);

	int nCopper = nMoney;

	wstring strMoney;

	if ( nGold > 0)
	{
		wchar_t buff[ 256] = { 0, };
		swprintf( buff, 128, L"{COLOR r=230 g=180 b=20}%dGP{/COLOR}", nGold);
		strMoney += buff;
	}

	if ( nSilver > 0)
	{
		wchar_t buff[ 256] = { 0, };
		swprintf( buff, 128, L"{COLOR r=200 g=200 b=200}%dSP{/COLOR}", nSilver);
		if ( strMoney.empty() == false)		strMoney += L" ";
		strMoney += buff;
	}

	if ( nCopper > 0)
	{
		wchar_t buff[ 256] = { 0, };
		swprintf( buff, 128, L"{COLOR r=180 g=100 b=50}%dCP{/COLOR}", nCopper);
		if ( strMoney.empty() == false)		strMoney += L" ";
		strMoney += buff;
	}

	return strMoney;
}

/// 미번역 문자(#) 제거하는 함수
static wstring ConvertIgnoreNonTranslationMark(wstring strString, bool bIgnoteNonTranslationMark=true)
{
	if (bIgnoteNonTranslationMark && strString.length() >= 2)
	{
		if (strString[0] == '#')
			strString = crl::AString::TrimLeft(strString, _T("#"));
	}
	return strString;
}

#endif