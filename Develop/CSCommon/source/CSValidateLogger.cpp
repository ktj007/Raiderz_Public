#include "stdafx.h"
#include "CSValidateLogger.h"
#include "CSLogListener.h"
#include "MDebug.h"
#include "CSChrInfo.h"
#include "CSTalentInfo.h"
#include "CSRecipeInfo.h"
#include "MLocale.h"

CSValidateLogger::CSValidateLogger() : m_pListener(NULL), m_bValidateResult(true), m_bLogging(true)
{

}

CSValidateLogger::~CSValidateLogger()
{

}

void CSValidateLogger::Log( const TCHAR* szAddtionLog, ... )
{
	if (m_bLogging == false) return;

	SetValidateResultFailed();

	TCHAR szBuff[1024];	

	va_list args;
	va_start(args,szAddtionLog);
	_vstprintf_s(szBuff, 1024, szAddtionLog, args);
	va_end(args);

	tstring strLog;

	strLog += m_strFileName;
	/*strLog += "파일의";
	char szLine[512];
	_itoa_s(m_nLine, szLine, 512, 10);
	strLog += szLine;
	strLog += "번줄";*/

	for (size_t i = 0; i < m_qElement.size(); i++)
	{
		strLog += _T("<");
		strLog += m_qElement[i].m_strTag;

		for (size_t j = 0; j < m_qElement[i].m_qAttribute.size(); j++)
		{
			strLog += _T(" ");
			strLog += m_qElement[i].m_qAttribute[j].m_strName;
			strLog += _T("=");
			strLog += _T("\"");
			strLog += m_qElement[i].m_qAttribute[j].m_strValue;
			strLog += _T("\"");
		}

		strLog += _T(">");
	}

	strLog += _T(": ");
	strLog += szBuff;

	// Listener가 있으면 Listener에서 로그남기고 없으면 mlog로 남긴다.
	if (m_pListener)
	{		
		m_pListener->OnLog(MLocale::ConvTCHARToAnsi(strLog.c_str()).c_str());
	}
	else
	{
		mlog3(MLocale::ConvTCHARToAnsi(strLog.c_str()).c_str());
	}
}

void CSValidateLogger::PushElement(const ELEMENT& element)
{
	m_qElement.push_back(element);
}

void CSValidateLogger::PopElement()
{
	m_qElement.pop_back();
}

void CSValidateLogger::ValidateMinus(int nValue, const TCHAR* szName)
{
	if (0 > nValue)
	{
		Log(PERCENT_S IS_MINUS, szName);
	}
}

void CSValidateLogger::ValidateMinus(float fValue, const TCHAR* szName)
{
	if (0.0f > fValue)
	{
		Log(PERCENT_S IS_MINUS, szName);
	}
}


void CSValidateLogger::ValidateMinusORZero(int nValue, const TCHAR* szName)
{
	if (0 >= nValue)
	{
		Log(PERCENT_S IS_ZERO_OR_MINUS, szName);
	}
}

void CSValidateLogger::ValidateMinusORZero(float fValue, const TCHAR* szName)
{
	if (0.0f >= fValue)
	{
		Log(PERCENT_S IS_ZERO_OR_MINUS, szName);
	}
}

void CSValidateLogger::ValidateBiggerThan(int nSmallValue, int nBigValue, const TCHAR* szSmallName, const TCHAR* szBigName)
{	
	if (nSmallValue > nBigValue)
	{
		Log(BIGGER_THAN, szSmallName, szBigName);
	}
}

void CSValidateLogger::ValidateBiggerThan(float fSmallValue, float fBigValue, const TCHAR* szSmallName, const TCHAR* szBigName)
{
	if (fSmallValue > fBigValue)
	{
		Log(BIGGER_THAN, szSmallName, szBigName);
	}
}

void CSValidateLogger::ValidateBiggerOrEqualThan(float fSmallValue, float fBigValue, const TCHAR* szSmallName, const TCHAR* szBigName)
{
	if (fSmallValue >= fBigValue)
	{
		Log(BIGGER_THAN, szSmallName, szBigName);
	}
}

void CSValidateLogger::ValidateMuchThan(int nSmallValue, int nBigValue, const TCHAR* szSmallName, const TCHAR* szBigName )
{
	if (nSmallValue > nBigValue)
	{
		Log(MUCH_THAN, szSmallName, szBigName);
	}
}

void CSValidateLogger::ValidateEmptyText(const tstring& strText, const TCHAR* szName)
{
	if (true == strText.empty())
	{
		Log(PERCENT_S IS_EMPTY, szName);
	}
}

void CSValidateLogger::ValidateLengthText(const tstring& strText, int nMaxLength, const TCHAR* szName)
{
	if (nMaxLength < (int)strText.length())
	{
		Log(TEXT_EXCEEDED_ALLOWED_LENGTH, szName, nMaxLength);
	}
}

void CSValidateLogger::ValidateSex(int nValue, const TCHAR* szName)
{
	if (RACE_NA >= nValue || RACE_MAX <= nValue)
	{
		Log(PERCENT_S IS_INVALID_TYPE, szName);
	}
}

void CSValidateLogger::ValidateRace(int nValue, const TCHAR* szName)
{
	if (SEX_NA >= nValue ||	SEX_MAX <= nValue)
	{
		Log(PERCENT_S IS_INVALID_TYPE, szName);
	}
}

void CSValidateLogger::Init(const TCHAR* szFileName)
{
	m_strFileName = szFileName;
	m_qElement.clear();
}

void CSValidateLogger::ValidateTrue(bool bBool, const TCHAR* szName )
{
	if (false == bBool)
	{
		Log(PERCENT_S IS_NOT_TRUE, szName);
	}
}

void CSValidateLogger::ValidateFalse(bool bBool, const TCHAR* szName )
{
	if (true == bBool)
	{
		Log(PERCENT_S IS_NOT_FALSE, szName);
	}
}

void CSValidateLogger::ValidateMinMax( const TCHAR* szValueName, float fValue, float fMinValue, float fMaxValue )
{
	if (fValue < fMinValue || fValue > fMaxValue)
	{
		Log(THE_VALUE_MUST_BE_IN_MINMAX, szValueName, fValue, fMinValue, fMaxValue);
	}
}

void CSValidateLogger::ValidateMinMax( const TCHAR* szValueName, int nValue, int nMinValue, int nMaxValue )
{
	if (nValue < nMinValue || nValue > nMaxValue)
	{
		Log(THE_VALUE_MUST_BE_IN_MINMAX_D, szValueName, nValue, nMinValue, nMaxValue);
	}
}

void CSValidateLogger::ValidateTooBig(int nValue, int nMaxValue, const TCHAR* szValueName)
{
	if (nValue > nMaxValue)
	{
		Log(IS_TOO_BIG, szValueName);
	}
}

void CSValidateLogger::ValidateID( int nID )
{
	ValidateMinusORZero(nID, _T("id"));
}