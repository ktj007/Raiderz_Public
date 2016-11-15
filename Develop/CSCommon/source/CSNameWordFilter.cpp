#include "StdAfx.h"
#include "CSNameWordFilter.h"
#include "MFileSystem.h"
#include "MLocale.h"
#include "CSStringFilter.h"


CSNameWordFilter::CSNameWordFilter()
{
}

CSNameWordFilter::~CSNameWordFilter()
{
	Clear();
}

void CSNameWordFilter::Clear()
{
	m_IllegalWordsList.clear();
	m_AllowedCharsList.clear();
}

bool CSNameWordFilter::LoadFromFile( const TCHAR* szFileName_IllegalWords, const TCHAR* szFileName_AllowedWords)
{
	if (NULL == szFileName_IllegalWords || NULL == szFileName_AllowedWords)
		return false;

	/// 금칙어 목록 로드
	if (!_LoadFileDB(szFileName_IllegalWords, m_IllegalWordsList))
		return false;

	/// 허용글자 목록 로드
	if (!_LoadFileDB(szFileName_AllowedWords, m_AllowedCharsList))
		return false;

	return true;
}

bool CSNameWordFilter::LoadFromString(TCHAR* szString_IllegalWords, TCHAR* szString_AllowedChars)
{
	if (NULL == szString_IllegalWords || NULL == szString_AllowedChars)
		return false;

	/// 금칙어 목록 로드
	_LoadFromStringDB(szString_IllegalWords, m_IllegalWordsList);

	/// 허용글자 목록 로드
	_LoadFromStringDB(szString_AllowedChars, m_AllowedCharsList);

	return true;
}

bool CSNameWordFilter::_LoadFileDB(const TCHAR* szFileName, CSWordsList& refListWords)
{
	MFile mzf;
	if ( !mzf.Open(MLocale::ConvTCHARToAnsi(szFileName).c_str()))
		return false;

	char* buffer;	
	buffer = new char[ mzf.GetLength() + 1];
	mzf.Read( buffer, mzf.GetLength());	
	buffer[ mzf.GetLength()] = NULL;

	tstring strTempBuf;

#ifdef _UNICODE
	strTempBuf = MLocale::ConvAnsiToUCS2(buffer);
#else
	strTempBuf = buffer;
#endif

	_LoadFromStringDB((TCHAR*)strTempBuf.c_str(), refListWords);

	mzf.Close();

	delete [] buffer;

	return true;
}

void CSNameWordFilter::_LoadFromStringDB(TCHAR* pStringDB, CSWordsList& refListWords)
{
	if (NULL == pStringDB) return;

	refListWords.clear();

#ifdef _DEBUG
	FILE* fp = _tfopen( _T("allow_db.txt"), _T("w"));
#endif

	while ( 1)
	{
		TCHAR szText[ 256]	= {0, };

		ParseLine( pStringDB, szText);

		if ( (int)_tcslen( szText) == 0)		continue;
		if (_tcsicmp( szText, _T("END")) == 0)	break;

		refListWords.push_back(szText);

#ifdef _DEBUG
		_ftprintf( fp, _T("insert into allowlist(workd) values('%%%s%%')\n"), szText );
#endif

		NextLine( pStringDB);
	}

#ifdef _DEBUG
	fclose( fp );
#endif
}

void CSNameWordFilter::AddIllegalWord( tstring strWord)
{
	m_IllegalWordsList.push_back( strWord);
}

void CSNameWordFilter::ParseLine( TCHAR*& prfBuf, TCHAR* szText)
{
	bool bType = true;
	int  nTypeCount = 0;
	int  nTextCount = 0;

	*szText = 0;

	while ( 1)
	{
		TCHAR ch = *prfBuf++;

		if ( (ch == 0) || (ch == _T('\n')) || (ch == _T('\r')))
			break;


		if ( ch == _T(','))
		{
			bType = false;
			continue;
		}


		*(szText + nTextCount++) = ch;
		*(szText + nTextCount) = 0;
	}
}

void CSNameWordFilter::NextLine( TCHAR*& prfBuf)
{
	for ( ; *prfBuf != _T('\n'); ++prfBuf )
		NULL;

	++prfBuf;
}

bool CSNameWordFilter::IsIllegalStr( tstring strString, bool bCheckSpecialChar/*=false*/ )
{
	bool bHasSpecialChar;
	string strSrcString;

#ifdef _UNICODE
	bHasSpecialChar = CSStringFilter::PreTranslateStr(MLocale::ConvUTF16ToAnsi(strString.c_str()), strSrcString);
#else
	bHasSpecialChar = CSStringFilter::PreTranslateStr(strString.c_str(), strSrcString);
#endif

	// 비허용 특수문자가 있는지 검사
	if ( bCheckSpecialChar && bHasSpecialChar )
		return false;
	
	for (CSWordsList::iterator itr = m_IllegalWordsList.begin(); itr != m_IllegalWordsList.end(); itr++)
	{
		string strAbuseWord;
#ifdef _UNICODE
		strAbuseWord = MLocale::ConvUTF16ToAnsi((*itr).c_str());
#else
		strAbuseWord = (*itr);
#endif

		int lenAbuseWord = (int)strAbuseWord.length();
		int lenScanSrcString = (int)strSrcString.length() - lenAbuseWord + 1;

		for (int indexSrcString = 0;  indexSrcString < lenScanSrcString;  indexSrcString++)
		{
			int indexAbuseWord = 0;
			for ( ;  indexAbuseWord < lenAbuseWord;  indexAbuseWord++)
			{
				char cSrcChar = strSrcString.at( indexSrcString + indexAbuseWord);				
				char cAbuseChar = strAbuseWord.at(indexAbuseWord);

				if ( cSrcChar != cAbuseChar)
					break;
			}

			if ( indexAbuseWord == lenAbuseWord)
			{
				return false;
			}

			// 한글의 첫번째 바이트라면 TRUE 반환
			if ( IsDBCSLeadByteEx(MLocale::GetCodePageFromLang(LANG_KOREAN), strSrcString.at( indexSrcString)) == TRUE)
				indexSrcString++;
		}
	}

	return true;
}

bool CSNameWordFilter::IsAllowedChar( tstring strSrcChar )
{
	if (m_AllowedCharsList.empty())
		return true;

	for ( list<tstring>::iterator itr = m_AllowedCharsList.begin();  itr != m_AllowedCharsList.end();  itr++)
	{
		const tstring& strAllowedChar = (*itr);
		if (strAllowedChar == strSrcChar)
			return true;
	}
	return false;
}

CSNameWordFilter* CSNameWordFilter::GetInstance()
{
	static CSNameWordFilter AllowedWordChecker;
	return &AllowedWordChecker;
}
