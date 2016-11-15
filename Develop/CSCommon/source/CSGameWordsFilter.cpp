#include "stdafx.h"
#include "CSGameWordsFilter.h"
#include "MFileSystem.h"
#include "MLocale.h"

CSGameWordsFilter::CSGameWordsFilter()
{
}

CSGameWordsFilter::~CSGameWordsFilter()
{
	Clear();
}

void CSGameWordsFilter::Clear()
{
	while ( m_listWords.empty() == false)
	{
		m_listWords.pop_front();
	}
}

bool CSGameWordsFilter::LoadFromFile(const TCHAR* szFileName)
{
	if (NULL == szFileName)
		return false;

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

	LoadFromStringDB((TCHAR*)strTempBuf.c_str());

	mzf.Close();

	delete [] buffer;

	return true;
}

void CSGameWordsFilter::LoadFromStringDB( TCHAR* pStringDB)
{
	if (NULL == pStringDB) return;

	Clear();

#ifdef _DEBUG
	FILE* fp = _tfopen( _T("abuse_db.txt"), _T("w"));
#endif

	while ( 1)
	{
		TCHAR szText[ 256]	= {0, };

		ParseLine( pStringDB, szText);

		if ( (int)_tcslen( szText) == 0)		continue;
		if (_tcsicmp( szText, _T("END")) == 0)	break;

		AddWord( szText);

#ifdef _DEBUG
		_ftprintf( fp, _T("insert into abuselist(workd) values('%%%s%%')\n"), szText );
#endif

		NextLine( pStringDB);
	}

#ifdef _DEBUG
	fclose( fp );
#endif
}

void CSGameWordsFilter::AddWord( tstring strWord)
{
	m_listWords.push_back( strWord);
}

void CSGameWordsFilter::ParseLine( TCHAR*& prfBuf, TCHAR* szText)
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

void CSGameWordsFilter::NextLine( TCHAR*& prfBuf)
{
	for ( ; *prfBuf != _T('\n'); ++prfBuf )
		NULL;

	++prfBuf;
}

bool CSGameWordsFilter::IsValidStr( tstring strSrcString )
{
	for ( list<tstring>::iterator itr = m_listWords.begin();  itr != m_listWords.end();  itr++)
	{
		const tstring& strWord = (*itr);
		if (strWord == strSrcString)
			return false;
	}

	return true;
}


/************************************************************************
GetInstance

desc : 인스턴스를 구한다
arg  : none
ret  : none
*************************************************************************/
CSGameWordsFilter* CSGameWordsFilter::GetInstance()
{
	static CSGameWordsFilter ChattingFilter;
	return &ChattingFilter;
}
