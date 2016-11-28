#include "stdafx.h"
#include "FSContext.h"
#include "CStringResMgr.h"

FSContext::FSContext(const vector<wstring>& param, CStringResMgr* pDictionary)
: m_vecParam(param)
, m_pDictionary(pDictionary)
{
	Trans(&FSContext::NormalTextState);
}

FSContext::~FSContext()
{
	// do nothing
}

wstring FSContext::GetResult(void) const
{
	return m_strResult + m_strBracket;	
}

void FSContext::NormalTextState(wchar_t token)
{
	if (token == L'{')
	{
		m_strBracket.clear();
		m_strBracket.push_back(token);
		Trans(&FSContext::OpeningBracketState);		
	}
	else
	{
		m_strResult.push_back(token);
	}
}

void FSContext::OpeningBracketState(wchar_t token)
{
	size_t nIndex = token - L'0';

	if (0 <= nIndex && nIndex < 10)
	{
		if (nIndex < m_vecParam.size())
		{
			m_strCasting = m_vecParam.at(nIndex);			
		}
		else
		{
			m_strCasting.push_back(token);
		}

		m_strBracket.push_back(token);
		Trans(&FSContext::IndexState);
	}
	else
	{	
		m_strBracket.push_back(token);
		m_strResult += m_strBracket;
		m_strBracket.clear();
		Trans(&FSContext::NormalTextState);
	}
}

void FSContext::IndexState(wchar_t token)
{
	switch(token)
	{
	case L'}':
		m_strResult += m_strCasting + m_strPostfix;
		m_strCasting.clear();		
		m_strPostfix.clear();
		m_strBracket.clear();
		Trans(&FSContext::NormalTextState);
		break;
	case L',':
		m_strBracket.push_back(token);
		Trans(&FSContext::CastingState);
		break;
	case L':':
		m_strBracket.push_back(token);
		m_strFormat.clear();
		m_nFormatGroup = 0;
		Trans(&FSContext::FormatState);
		break;
	default:
		m_strBracket.push_back(token);
		m_strResult += m_strBracket;
		m_strBracket.clear();
		Trans(&FSContext::NormalTextState);
		break;
	}
}

void FSContext::CastingState(wchar_t token)
{
	switch(token)
	{
	case L'을':
	case L'를':
		if (HasJonsung())	m_strPostfix.push_back(L'을');
		else m_strPostfix.push_back(L'를');

		m_strBracket.push_back(token);
		Trans(&FSContext::CastEndState);
		break;
	case L'가':
		if (HasJonsung())	m_strPostfix.push_back(L'이');
		else m_strPostfix.push_back(L'가');

		m_strBracket.push_back(token);
		Trans(&FSContext::CastEndState);
		break;
	case L'는':
		if (HasJonsung())	m_strPostfix.push_back(L'은');
		else m_strPostfix.push_back(L'는');

		m_strBracket.push_back(token);
		Trans(&FSContext::CastEndState);
		break;
	case L's':
	case L'S':
		if (m_pDictionary != NULL)
		{
			m_strCasting = m_pDictionary->Plural(m_strCasting);
		}

		m_strBracket.push_back(token);
		Trans(&FSContext::CastEndState);
		break;
	default:
		m_strBracket.push_back(token);
		m_strResult += m_strBracket;
		m_strBracket.clear();
		Trans(&FSContext::NormalTextState);
		break;
	}
}

void FSContext::CastEndState(wchar_t token)
{
	switch(token)
	{
	case L'}':
		m_strResult += m_strCasting + m_strPostfix;
		m_strCasting.clear();
		m_strPostfix.clear();
		m_strBracket.clear();		
		Trans(&FSContext::NormalTextState);
		break;
	case L':':
		m_strBracket.push_back(token);
		m_strFormat.clear();
		m_nFormatGroup = 0;
		Trans(&FSContext::FormatState);
		break;
	default:
		m_strBracket.push_back(token);
		m_strResult += m_strBracket;
		m_strBracket.clear();
		Trans(&FSContext::NormalTextState);
		break;
	}
}

void FSContext::FormatState(wchar_t token)
{
	switch(token)
	{
	case L'}':
		m_strResult += m_strFormat + m_strPostfix;
		m_strPostfix.clear();
		m_strCasting.clear();
		m_strBracket.clear();
		Trans(&FSContext::NormalTextState);
		break;
	case L'{':
		m_strBracket.push_back(token);
		m_strNestedBracket.clear();
		m_strNestedBracket.push_back(token);
		Trans(&FSContext::NestedBracketState);
		break;
	case L'$':		
		m_strBracket.push_back(token);
		m_strFormat += m_strCasting;		
		break;
	case L';':
		if (m_nFormatGroup == _wtoi(m_strCasting.c_str()))
		{
			m_nEscapeBracketDepth = 1;
			Trans(&FSContext::EscapeBracketState);
		}
		else
		{
			m_strFormat.clear();
			m_nFormatGroup++;
		}		

		m_strBracket.push_back(token);
		break;
	default:
		m_strFormat.push_back(token);
		m_strBracket.push_back(token);
		break;			
	}
}

void FSContext::NestedBracketState(wchar_t token)
{
	switch(token)
	{
	case L'}':
		m_strNestedBracket.push_back(token);
		
		{
			FSContext context(m_vecParam, m_pDictionary);

			for each(wchar_t nestedToken in m_strNestedBracket)
			{
				context.Dispatch(nestedToken);
			}

			m_strFormat += context.GetResult();
		}

		m_strBracket.push_back(token);
		Trans(&FSContext::FormatState);
		break;
	default:
		m_strNestedBracket.push_back(token);
		m_strBracket.push_back(token);
		break;
	}
}

void FSContext::EscapeBracketState(wchar_t token)
{
	switch(token)
	{
	case L'{':
		m_nEscapeBracketDepth++;
		m_strBracket.push_back(token);
		break;
	case L'}':			
		m_nEscapeBracketDepth--;		
		m_strBracket.push_back(token);

		if (m_nEscapeBracketDepth == 0)
		{
			m_strResult += m_strFormat + m_strPostfix;
			m_strCasting.clear();
			m_strPostfix.clear();
			m_strBracket.clear();
			Trans(&FSContext::NormalTextState);
		}
		break;
	default:
		m_strBracket.push_back(token);
		break;
	}
}

bool FSContext::HasJonsung(void) const
{
	if (m_strCasting.empty()) return false;


	wchar_t lastLetter = *m_strCasting.rbegin();

	// 유니코드 한글페이지
	if (44032 <= lastLetter && lastLetter <= 55203)
	{
		return 0 != (lastLetter - 44032) % 28;
	}
	else
	{
		wstring letter(1, lastLetter);		
		switch(_wtoi(letter.c_str()) % 10)
		{
		case 0:
		case 1:
		case 3:
		case 6:
		case 7:
		case 8:		
			return true;
		case 2:
		case 4:
		case 5:
		case 9:
			return false;
		}
	}

	return true;
}
