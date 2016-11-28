#include "StdAfx.h"
#include "CSNameStringChecker.h"
#include "MLocale.h"
#include "CSGameWordsFilter.h"
#include "CSNameWordFilter.h"


CSNameStringChecker::CSNameStringChecker(const wchar_t* pszWideWord, TYPE nType)
: m_strWord(pszWideWord), m_nType(nType)
{
}

CSNameStringChecker::~CSNameStringChecker(void)
{
}

CSNameStringChecker::ERROR_CODE 
CSNameStringChecker::Check() const
{
	if (m_strWord.empty())
		return ERROR_LENGTH_TOO_SHORT;

	if (Check_NumberOnly())
		return ERROR_NUMBER_ONLY;

	if (Check_NumberAndSpace())
		return ERROR_CONTAIN_NUMBER_OR_SPACE;

	if (Check_Alphabetless())
		return ERROR_CONTAIN_ALPHABETLESS;

	if (Check_LengthShort())
		return ERROR_LENGTH_TOO_SHORT;

	if (Check_LengthLarge())
		return ERROR_LENGTH_TOO_LARGE;

	if (Check_MixingLanguage())
		return ERROR_MIXING_LANGUAGE;

	if (Check_InvalidCharacter())
		return ERROR_CONTAIN_INVALID_CHARACTER;

	if (Check_CpHangul())
		return ERROR_CONTAIN_INVALID_CHARACTER;

	if (!CheckAbuseWords())
		return ERROR_INVALID_WORD;

	if (!CheckGameContentsWords())
		return ERROR_INVALID_WORD;

	return OK;
}

bool CSNameStringChecker::Check_LengthShort() const
{
	// ÃÖ¼Ò±æÀÌ Á¦ÇÑ, ANSI ±âÁØ 3byte ¹Ì¸¸
	wstringstream strLen;
	strLen << L"([a-zA-Z]{0," << GetLengthLimit_MinEnglish() - 1 << L"}|[°¡-ÆR]{0," << GetLengthLimit_MinHangul() - 1 << L"})";

	tr1::wregex pattern(strLen.str());

	tr1::match_results<wstring::const_iterator> result;

	if (!tr1::regex_match(m_strWord, result, pattern))
	{
		return false;
	}

	return true;
}

bool CSNameStringChecker::Check_LengthLarge() const
{
	// ÃÖ´ë±æÀÌ Á¦ÇÑ, ANSI ±âÁØ 16byte ÃÊ°ú
	wstringstream strLen;
	strLen << L"([A-Za-z]{" << GetLengthLimit_MaxEnglish() + 1 << L",}|[°¡-ÆR]{" << GetLengthLimit_MaxHangul() + 1 << L",})";

	tr1::wregex pattern(strLen.str());

	tr1::match_results<wstring::const_iterator> result;

	if (!tr1::regex_match(m_strWord, result, pattern))
	{
		return false;
	}

	return true;
}

bool CSNameStringChecker::Check_MixingLanguage() const
{
	// ¿µ¹® ¿Ü È¥ÇÕ
	tr1::wregex pattern(L".*([A-Za-z]+[^A-Za-z]+[A-Za-z]*|[A-Za-z]*[^A-Za-z]+[A-Za-z]+).*");
	
	tr1::match_results<wstring::const_iterator> result;
		
	if (!tr1::regex_match(m_strWord, result, pattern))
	{
		return false;
	}

	return true;
}

bool CSNameStringChecker::Check_NumberOnly() const
{
	// ¼ýÀÚ·Î¸¸ ÀÌ·¯¿ìÁø ¹®ÀÚ¿­
	tr1::wregex pattern(L"[0-9]+");
	
	tr1::match_results<wstring::const_iterator> result;
		
	if (!tr1::regex_match(m_strWord, result, pattern))
	{
		return false;
	}

	return true;
}

bool CSNameStringChecker::Check_NumberAndSpace() const
{
	// ¼ýÀÚ³ª °ø¹é(°ø¹é¹®ÀÚ) Æ÷ÇÔ
	tr1::wregex pattern(L".*[0-9 \\t\\n\\r\\f].*");
 	
	tr1::match_results<wstring::const_iterator> result;

	if (!tr1::regex_match(m_strWord, result, pattern))
	{
		return false;
	}

	return true;
}

bool CSNameStringChecker::Check_CpHangul() const
{
	// ÇÑ±ÛÈ®ÀÎ
	tr1::wregex pattern(L"[°¡-ÆR]+");

	tr1::match_results<wstring::const_iterator> result;

	if (!tr1::regex_match(m_strWord, result, pattern))
	{
		return false;
	}
	
	// KS ¿Ï¼ºÇüÄÚµå
	string strAnsi = MLocale::ConvUTF16ToAnsi(m_strWord.c_str());

	for (size_t i=0; i < strAnsi.size(); ++i)
	{
		UCHAR cFirst = strAnsi[i];

		if (i+1 >= strAnsi.size())
			continue; // ´ÙÀ½ ¹®ÀÚ¸¦ ±¸ÇÒ ¼ö ¾øÀ½

		// ±âº» ÇÑ±Û ¿Ï¼ºÇü ÄÚµå Ã¼Å©
		UCHAR cSecond = strAnsi[++i];
		USHORT nChar = ((USHORT)cFirst << 8) + cSecond;
		if (0xb0a1 <= nChar && nChar <= 0xc8fe)
			continue;

		// È®Àå ÇÑ±Û È¯¼ºÇü ÄÚµå Ã¼Å©
		if (0x8141 <= nChar && nChar <= 0xc65a)
		{
			// Çã¿ëµÈ ±ÛÀÚÀÎÁö Ã¼Å©
			char chAnsi[4] = { (char)cFirst, (char)cSecond, 0, };
			tstring tCharacter;
#ifdef _UNICODE
			tCharacter = MLocale::ConvAnsiToUCS2(chAnsi);
#else
			tCharacter = chAnsi;
#endif
			if (GetNameWordFilter()->IsAllowedChar(tCharacter))
				continue;
		}

		return true;
	}

	return false;
}

bool CSNameStringChecker::Check_InvalidCharacter() const
{
	// Æ¯¼ö±âÈ£
	tr1::wregex pattern(L".*[^A-Za-z°¡-ÆR].*");

	tr1::match_results<wstring::const_iterator> result;

	if (!tr1::regex_match(m_strWord, result, pattern))
	{
		return false;
	}

	return true;
}

bool CSNameStringChecker::Check_Alphabetless() const
{
	// ±âÈ£
	tr1::wregex pattern(L".*[[:punct:]].*");

	tr1::match_results<wstring::const_iterator> result;

	if (!tr1::regex_match(m_strWord, result, pattern))
	{
		return false;
	}

	return true;
}

bool CSNameStringChecker::CheckAbuseWords() const
{
#ifdef _UNICODE
	return GetNameWordFilter()->IsIllegalStr(m_strWord.c_str(), true);
#else
	return GetNameWordFilter()->IsIllegalStr(MLocale::ConvUTF16ToAnsi(m_strWord.c_str()).c_str(), true);
#endif	
}

bool CSNameStringChecker::CheckGameContentsWords() const
{
#ifdef _UNICODE
	return GetGameWordsFilter()->IsValidStr(m_strWord.c_str());
#else
	return GetGameWordsFilter()->IsValidStr(MLocale::ConvUTF16ToAnsi(m_strWord.c_str()).c_str());
#endif
}


CSNameStringChecker::LENGTH_LIMIT CSNameStringChecker::GetLengthLimit_MinHangul() const
{
	switch(m_nType)
	{
	case TYPE_CHARACTER:	return CHARACTER_MIN_HANGUL;
	case TYPE_GUILD:		return GUILD_MIN_HANGUL;
	}

	return CHARACTER_MIN_HANGUL;
}

CSNameStringChecker::LENGTH_LIMIT CSNameStringChecker::GetLengthLimit_MaxHangul() const
{
	switch(m_nType)
	{
	case TYPE_CHARACTER:	return CHARACTER_MAX_HANGUL;
	case TYPE_GUILD:		return GUILD_MAX_HANGUL;
	}

	return CHARACTER_MAX_HANGUL;
}

CSNameStringChecker::LENGTH_LIMIT CSNameStringChecker::GetLengthLimit_MinEnglish() const
{
	switch(m_nType)
	{
	case TYPE_CHARACTER:	return CHARACTER_MIN_ENGLISH;
	case TYPE_GUILD:		return GUILD_MIN_ENGLISH;
	}

	return CHARACTER_MIN_ENGLISH;
}


CSNameStringChecker::LENGTH_LIMIT CSNameStringChecker::GetLengthLimit_MaxEnglish() const
{
	switch(m_nType)
	{
	case TYPE_CHARACTER:	return CHARACTER_MAX_ENGLISH;
	case TYPE_GUILD:		return GUILD_MAX_ENGLISH;
	}

	return CHARACTER_MAX_ENGLISH;
}


