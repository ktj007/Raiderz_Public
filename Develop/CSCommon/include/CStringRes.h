#ifndef _CSTRING_RES_H
#define _CSTRING_RES_H

#include "MXml.h"
#include <map>
#include <string>
#include "MTstring.h"
#include <tchar.h>
#include <atlconv.h>
#include <atlexcept.h>
#include "CStringNode.h"
#include "CSFormatString.h"
#include "AString.h"

using std::map;
using std::tstring;
using std::pair;

#define STRINGS_TOK_CHAR_MSG_VALUE		_T('$')

// 필수 엘리먼트와 어트리뷰트
#define STRINGS_XML_TAG_MAIET		"maiet"
#define STRINGS_XML_TAG_STR			"STR"
#define STRINGS_XML_ATTR_KEY		"key"
#define STRINGS_XML_ATTR_STRING		"string"

// 옵션 어트리뷰트
#define STRING_XML_ATTR_PLURAL		"plural"


template<class _T>
class CStringRes
{
public:
	typedef map<_T, CStringNode*>	string_map;

private:
	string_map			m_StringMap;

	tstring				m_strTOK_TAG;
	tstring				m_strTOK_ATTR;

#ifndef _PUBLISH
	tstring				m_strError;
#endif

public:
	CStringRes( const TCHAR* pszTOK_TAG = _T(STRINGS_XML_TAG_STR), const TCHAR* pszTOK_ATTR = _T("id") )
		: m_strTOK_TAG( pszTOK_TAG ), m_strTOK_ATTR( pszTOK_ATTR )
	{

	}
	virtual ~CStringRes()
	{
		for each(string_map::value_type value in m_StringMap)
		{
			delete value.second;
		}
	}
	void SetXmlTag(const TCHAR* szTag)
	{
		m_strTOK_TAG = szTag;
	}
	void Release()
	{
		for each(string_map::value_type value in m_StringMap)
		{
			delete value.second;
		}

		m_StringMap.clear();
	}
	bool Load(const TCHAR* pszFileName, bool bIgnoteNonTranslationMark=false)
	{
		if( 0 == pszFileName ) return false;

		MXml xml;		
		if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(pszFileName).c_str())) return false;

		MXmlHandle docHandle = xml.DocHandle();
		MXmlElement* pElement = docHandle.FirstChild(STRINGS_XML_TAG_MAIET).FirstChildElement().Element();

		_T			CID;
		tstring strContents;

		for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
		{
			if (!_stricmp(pElement->Value(), MLocale::ConvTCHARToAnsi(m_strTOK_TAG.c_str()).c_str()))
			{
				// 필수 입력 사항 검사
				if (!_Attribute(CID,	pElement, STRINGS_XML_ATTR_KEY))					continue;
				if (!_Attribute(strContents,	pElement, STRINGS_XML_ATTR_STRING, &xml))	continue;

				// 유일값 검사
				if (m_StringMap.end() != m_StringMap.find(CID))	continue;

				if (bIgnoteNonTranslationMark && strContents.length() >= 2)
				{
					if (strContents[0] == '#')
						strContents = crl::AString::TrimLeft(strContents, _T("#"));
				}
				
				// 필수 입력 사항 쓰기
				CStringNode* pNode = new CStringNode();
				pNode->SetText(strContents);

				// 옵션 속성 쓰기
				tstring strPlural;
				if (_Attribute(strPlural, pElement, STRING_XML_ATTR_PLURAL, &xml))
				{
					pNode->SetAttribute(CStringNode::PLURAL, strPlural);
				}


				// 문자열테 이블에 추가
				m_StringMap.insert( string_map::value_type(CID, pNode));
			}
		}

		return true;
	}

	bool InsertStr( const _T& code, const tstring& value)
	{
		string_map::iterator it = m_StringMap.find( code );
		if( m_StringMap.end() != it ) return false;

		CStringNode* pNode = new CStringNode();
		pNode->SetText(value);

		m_StringMap.insert(string_map::value_type(code, pNode));

		return true;
	}

	bool SetStr( const _T& code, const tstring& value)
	{
		string_map::iterator it = m_StringMap.find( code );
		if( m_StringMap.end() == it ) return false;

		CStringNode* pNode = it->second;
		pNode->SetText(value);

		return true;
	}

	bool DelStr( const _T& code)
	{
		string_map::iterator it = m_StringMap.find( code );
		if( m_StringMap.end() == it ) return false;

		CStringNode* pNode = it->second;
		SAFE_DELETE(pNode);

		m_StringMap.erase(it);		

		return true;
	}

	const TCHAR* GetStr( const _T& code )
	{
		string_map::iterator it = m_StringMap.find( code );
		
		if( m_StringMap.end() == it )
		{
#ifndef _PUBLISH		
			m_strError = CSFormatString::Format(_T("없는 문자열(key={0})"), FSParam(code));
			return m_strError.c_str();
#else
			return _T("");
#endif
		}
		
		CStringNode* pNode = it->second;
		return pNode->GetText();
	}

	const TCHAR* GetStr(const _T& code, CStringNode::Attribute attr)
	{
		string_map::iterator it = m_StringMap.find(code);

		if (it == m_StringMap.end())
		{
#ifndef _PUBLISH
			m_strError = CSFormatString::Format(_T("없는 문자열(key={0})"), FSParam(code));
			return m_strError.c_str();
#else
			return _T("");
#endif
		}

		CStringNode* pNode = it->second;
		return pNode->GetAttribute(attr);
	}

	bool Exist( const _T& code )
	{
		string_map::iterator it = m_StringMap.find(code);

		return (it != m_StringMap.end());
	}
};

class CStringTranslator
{
public:
	bool TransArgs(TCHAR* poutDestStr, const int nDestStrSize, const TCHAR* szSrcStr, const int argnum, const TCHAR* arg1, va_list args )
	{
		// 인자가 없으면 단지 복사만 하고 끝낸다.
		if ((argnum <= 0) || (argnum > 9))
		{
			_tcscpy_s(poutDestStr, nDestStrSize, szSrcStr);
			return true;
		}

		const TCHAR* argv[9] = {NULL, };

		//va_list args;
		//va_start(args, arg1);
		argv[0] = arg1;

		for (int i = 1; i < argnum; i++)
		{
			argv[i] = va_arg(args, const TCHAR*);
		}
		//va_end(args);

		bool bPercent=false;

		int taridx = 0;
		poutDestStr[taridx] = 0;

		size_t nSrcStrLen = _tcslen(szSrcStr);
		tstring formatstring = tstring(szSrcStr);


		// 인자 파싱
		for(size_t j=0;j<nSrcStrLen;j++)
		{
			TCHAR cur=szSrcStr[j];
			if(bPercent)
			{
				if(cur==STRINGS_TOK_CHAR_MSG_VALUE)
				{
					poutDestStr[taridx++] = STRINGS_TOK_CHAR_MSG_VALUE;
					poutDestStr[taridx] = 0;
				}
				else
					if ( ('1' <= cur) && (cur <= '9') )
					{
						int nParam = cur - '0' - 1;
						if ( (nParam < argnum) && (argv[nParam] != NULL) )
						{
							_tcscat_s(poutDestStr, nDestStrSize, argv[nParam]);
							taridx += (int)_tcslen(argv[nParam]);
						}
						else
						{
							_ASSERT(0);		// xml의 인자 설정이 잘못되었거나, 함수호출시 가변인자가 부족하다.
						}
					}

					bPercent = false;
					continue;
			}

			if(!bPercent)
			{
				if(cur==STRINGS_TOK_CHAR_MSG_VALUE)
					bPercent=true;
				else
				{
					poutDestStr[taridx++] = cur;
					poutDestStr[taridx] = 0;
				}
			}
		}

		return true;
	}
};



#endif // _CSTRING_RES_H