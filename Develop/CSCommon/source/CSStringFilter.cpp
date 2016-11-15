/************************************************************************
  CSStringFilter

  desc : Gunz 에 있던 MChattingFilter를 복사하여 네이밍과 인터페이스를 Raiderz 에 맞게 수정합니다.
  date : 2010-02-25
  name : 원창현
*************************************************************************/

#include "stdafx.h"
#include "CSStringFilter.h"
#include "MFileSystem.h"
#include "MLocale.h"
#include <regex>
#include "AStlUtil.h"
#include "CSDef.h"

const int MAX_ABUSE_WORD_LEN = 256;

/// 비속어 사이에 끼워끌 만한 글자들
const tstring CSStringFilter::ms_CheatingChar = _T(" `~!@#$%^&*()-_=+\\|[{]};:'\",<.>/?1");

CSStringFilter::CSStringFilter()
: m_pRoot(NULL)
{
}

CSStringFilter::~CSStringFilter()
{
	Clear();
}

void CSStringFilter::Clear()
{
	SAFE_DELETE(m_pRoot);
}

bool CSStringFilter::LoadFromFile(const TCHAR* szFileName)
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

void CSStringFilter::LoadFromStringDB( TCHAR* pStringDB)
{
	if (NULL == pStringDB) return;

#ifdef _DEBUG
	FILE* fp = _tfopen( _T("abuse_db.txt"), _T("w"));
#endif

	while ( 1)
	{
		TCHAR szText[ 256]	= {0, };

		ParseLine( pStringDB, szText);

		if ( (int)_tcslen( szText) == 0)		continue;
		if (_tcsicmp( szText, _T("END")) == 0)	break;

		AddAbuseWord( szText );

#ifdef _DEBUG
		_ftprintf( fp, _T("insert into abuselist(workd) values('%%%s%%')\n")
			, szText );
#endif

		NextLine( pStringDB);
	}

#ifdef _DEBUG
	fclose( fp );
#endif
}

void CSStringFilter::ParseLine( TCHAR*& prfBuf, TCHAR* szText)
{
	int  nTextCount = 0;

	*szText = 0;

	while ( 1)
	{
		TCHAR ch = *prfBuf++;

		if ( (ch == 0) || (ch == _T('\n')) || (ch == _T('\r')))
			break;

		*(szText + nTextCount++) = ch;
		*(szText + nTextCount) = 0;
	}
}


bool CSStringFilter::AddAbuseWord( const TCHAR* szAbuse )
{
	tstring strAbuse = szAbuse;

	if (strAbuse.size() > MAX_ABUSE_WORD_LEN ||
		strAbuse.find_first_of(ms_CheatingChar) != std::tstring::npos)
		return false;
	
	TCHAR buf[256+1] = {0, };
	_sntprintf((TCHAR*)buf, sizeof(buf)-1, _T("%s"), strAbuse.c_str());
	buf[sizeof(buf)-1] = 0;
	
	// 단어의 모든 글자를 트리 형태로 구성한다.
	if (m_pRoot == NULL)
		m_pRoot = new CSAbuseCharNode();

	CSAbuseCharNode* pCurrent = m_pRoot;
	for (size_t i = 0; i < strAbuse.size(); i++)
	{
		pCurrent = pCurrent->AddChild(buf[i]);
	}
	
	// 단어의 끝이 되는 글자를 flag로 표시.
	pCurrent->SetLeafNode(true);

	return true;
}

void CSStringFilter::NextLine( TCHAR*& prfBuf)
{
	for ( ; *prfBuf != _T('\n'); ++prfBuf )
		NULL;
		
	++prfBuf;
}


bool CSStringFilter::IsValidStr( const TCHAR* szSrcString, bool bCheckSpecialChar)
{
	if ( NULL == szSrcString)
		return false;
	
	TCHAR* pCopiedString = _tcsdup(szSrcString);
	tstring original = _tcslwr(pCopiedString);	///< 소문자로 변환
	free(pCopiedString);

	for (size_t i=0; i<original.size(); i++)
	{
		if (Match(original.substr(i, original.size() - i)) > 0)
			return false;
	}

	return true;
}

tstring CSStringFilter::Filter( const TCHAR* szSrcString )
{
	TCHAR* pCopiedString = _tcsdup(szSrcString);
	tstring original = _tcslwr(pCopiedString);	///< 소문자로 변환
	free(pCopiedString);

	std::tstring text(original);

	for (size_t i=0; i<original.size();)
	{
		size_t size = Match(original.substr(i, original.size() - i));
		if (size > 0)
		{
			text.replace(i, size, std::tstring(size, '*'));
			i += size;
		}
		else
		{
#ifdef _UNICODE
			i++;
#else
			// 한글이라면 2바이트씩, 아니라면 1바이트씩 넘어간다.
			i += original[i] & 0x80 ? 2 : 1;
#endif
		}
	}

	return text;
}

/************************************************************************
	PreTranslateStr

	desc	스트링을 소문자화하고 특수문자를 제거한다.
	arg		strInText	처리할 스트링
			strOutText	처리된 스트링
	re		true		허용되지 않는 특수문자를 포함하고 있음
			false		비허용 특수문자 없음
*************************************************************************/
bool CSStringFilter::PreTranslateStr( const string& strInText, string& strOutText)
{
	char *pStrLowercase;
	//	_strlwr_s( pStrLowercase = _strdup( strInText.c_str()), 512);			// 보안용
	//	pStrLowercase = _strlwr( _strdup( strInText.c_str()));					// 비보안용
	pStrLowercase = _strdup( strInText.c_str());
	strOutText = pStrLowercase;

	// 캐릭터 검사
	int nPos = 0;					// 스트링 검사 위치
	bool bHaveSpcChar = false;		// 비허용 특수문자 포함 여부
	char ch;
	unsigned char u_ch;

	while (nPos < (int)strOutText.size())
	{
		ch = strOutText.at( nPos);
		u_ch = ch;

		// 첫 2바이트가 국가코드 문자인지 확인		
		if (IsDBCSLeadByteEx(MLocale::GetCodePageFromLang(LANG_KOREAN), ch) == TRUE)
		{
#ifdef LOCALE_JAPAN   // 일본 전각 문자의 경우 특수문자도 전부 2바이트다. 
			if ( ch == -127 && strOutText.at( nPos +1) == 64) // 일단 스페이스만 막음 
			{
				bHaveSpcChar = true;
				m_strLastFilterdWord = strOutText.substr(nPos, 2);;
			}
#endif
			nPos += 2;
		}
		// 일반 ASCII 문자인지 확인
#ifdef LOCALE_JAPAN   
		else if ( ( (ch >= 'a') && (ch <= 'z') ) ||	
			( (ch >= 'A') && (ch <= 'Z') ) ||
			( (ch >= '0') && (ch <= '9') ) ||
			( (u_ch >= 0xA1 ) && (u_ch <= 0xDF ) ) )  // 일본 반각 문자인지 확인
#else 
		else if ( ( (ch >= 'a') && (ch <= 'z') ) ||	
			( (ch >= 'A') && (ch <= 'Z') ) ||
			( (ch >= '0') && (ch <= '9') )  )  
#endif
		{
			if( ( (ch >= 'A') && (ch <= 'Z') ) )  // 소문자 변환
			{
				strOutText[nPos] += 0x20;
			}
			nPos += 1;
		}
		else
		{
			// 삭제하기 전에 해당 캐릭터가 비허용 특수문자인지 검사
			if ( (ch != '_') && (ch != '[') && (ch != ']') )
			{
				bHaveSpcChar = true;
			}

			// 캐릭터 삭제
			strOutText.erase( nPos, 1);
		}
	}

	free( pStrLowercase);

	return bHaveSpcChar;
}

wstring CSStringFilter::RemoveGameSystemCharacter(wstring strText)
{
	tr1::wregex pattern(L"\\{.*\\}|\\{|\\}");
	return tr1::regex_replace(strText, pattern, wstring(L""));
}


size_t CSStringFilter::Match(const std::tstring& text) const
{
	if (text.empty()) return 0;
	if (IsPunctutation(text[0])) return 0;
	if (m_pRoot == NULL) return 0;

	CSAbuseCharNode* pCurrent = m_pRoot;
	size_t i = 0;

	while (i < text.size())
	{
		if (IsPunctutation(text[i]))
		{
			// 현재 글자가 문장 부호일 경우에는 continue
			// 비속어 사이사이에 문장 부호를 넣은 경우를 검색하기 위해서.
			// ex) 나쁜!넘
			i++;
			continue;
		}

		// 자식 노드 중에 현재 문자와 일치하는 값으로 이어지는 것을 찾는다.
		pCurrent = pCurrent->FindChild(text[i]);

		// 더 이상 이어지는 노드가 없다는 말은 인자로 받은 문자들과 일치하는
		// 단어가 노드 트리 상에 존재하지 않는다는 뜻.
		if (pCurrent == NULL) return 0;

		// 필터링해야하는 단어다!
		if (pCurrent->IsLeafNode()) return i + 1;	///< 비속어가 포함되어 있을 경우에는 그 비속어의 길이를 리턴.

		i++;
	}

	return 0;
}

bool CSStringFilter::IsPunctutation(TCHAR nChar) const
{
	return ms_CheatingChar.find(nChar) != std::tstring::npos;
}



CSStringFilter* CSStringFilter::GetInstance()
{
	static CSStringFilter ChattingFilter;
	return &ChattingFilter;
}



//////////////////////////////////////////////////////////////////////////


CSAbuseCharNode::CSAbuseCharNode()
: m_bLeafNode(false)
{
}

CSAbuseCharNode::~CSAbuseCharNode()
{
	SAFE_DELETE_MAP(*this);
}

CSAbuseCharNode* CSAbuseCharNode::FindChild(TCHAR nChar) const
{
	const_iterator itr(find(nChar));
	return itr != end() ? itr->second : NULL;
}

CSAbuseCharNode* CSAbuseCharNode::AddChild(TCHAR nChar)
{
	// 해당하는 자식이 없을 경우, 새로운 노드를 생성해서 추가한다.
	iterator itr(find(nChar));
	if (itr == end()) itr = insert(value_type(nChar, new CSAbuseCharNode)).first;

	return itr->second;
}

