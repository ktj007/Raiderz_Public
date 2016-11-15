#pragma once

#define TIXML_USE_STL		// STL 사용

#include "windows.h"
#include "tinyxml/tinyxml.h"
#include <string>
#include <stdlib.h>
#include "RLib.h"

typedef TiXmlElement		MXmlElement;
typedef TiXmlComment		MXmlComment;
typedef TiXmlUnknown		MXmlUnknown;
typedef TiXmlAttribute		MXmlAttribute;
typedef TiXmlText			MXmlText;
typedef TiXmlDeclaration	MXmlDeclaration;
typedef TiXmlHandle			MXmlHandle;
typedef TiXmlDocument		MXmlDocument;
typedef TiXmlNode			MXmlNode;
typedef TiXmlVisitor		MXmlVisitor;
typedef TiXmlPrinter		MXmlPrinter;

/** Xml을 읽거나 저장하기 위한 최상위 클래스

*/
class CML2_API MXml
{
private:
	MXmlDocument		m_Document;
	bool				m_bUTF8;
	bool				CheckUTF8();
	LANGID				m_LangID;
	std::string			m_strFileName;

public:
						MXml();
	virtual			~MXml();
	void				Clear();

	void				AfterCreateFileRead(const char* szFileName);
	bool				LoadFile(const char* szFileName);
	bool				LoadStream(const char* pStream);
	bool				SaveFile(const char* szFileName);
	bool				SaveFile();
	MXmlNode*			LinkEndChild( MXmlNode* addThis );
	std::string			ToAnsi(const char* pszString);
	std::wstring		ToUTF16(const char* pszString);
	MXmlDeclaration*	CreateDeclaration(const std::string& strVersion, 
										  const std::string& strEncoding,
										  const std::string& strStandalone = "");

	MXmlDocument*		Doc()				{ return &m_Document; }
	MXmlHandle			DocHandle()			{ return MXmlHandle( &m_Document ); }
	bool				IsUTF8Encoding()	{ return m_bUTF8; }

	void				SetLanguage( LANGID id)	{ m_LangID = id;	}
	LANGID				GetLanguage()			{ return m_LangID;	}

	std::string			GetBuffer();
};

/// UTF8 텍스트를 ANSI로 쉽게 변환하기 위한 함수
// inline void _UToA(char* pOut, MXml* pXml, const char* text)
// {
// 	strcpy(pOut, pXml->ToAnsi(text).c_str());
// }

/// 엘리먼트의 속성값을 반환한다. 만약 XML이 UTF-8로 인코딩되어 있으면 Ansi로 자동으로 변환한다. -------------

// 문자열 속성값
CML2_API bool _Attribute(std::string& strOutValue, MXmlElement* pElement, const char* name, MXml* pXml=NULL);
CML2_API bool _Attribute(std::string& strOutValue, MXmlElement* pElement, const char* name, const char* szDefaultValue, MXml* pXml=NULL);

CML2_API bool _Attribute(std::wstring& strOutValue, MXmlElement* pElement, const char* name, MXml* pXml=NULL);
CML2_API bool _Attribute(std::wstring& strOutValue, MXmlElement* pElement, const char* name, const char* szDefaultValue, MXml* pXml=NULL);

// 문자열 속성값
CML2_API bool _Attribute(char* pOutValue, MXmlElement* pElement, const char* name, MXml* pXml=NULL);
CML2_API bool _Attribute(char* pOutValue, MXmlElement* pElement, const char* name, const char* szDefaultValue, MXml* pXml=NULL);
CML2_API bool _Attribute(char* pOutValue, size_t len, MXmlElement* pElement, const char* name, const char* szDefaultValue, MXml* pXml=NULL);

// 정수 속성값
CML2_API bool _Attribute(int* pOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(int* pOutValue, MXmlElement* pElement, const char* name, int nDefaultValue);
CML2_API bool _Attribute(unsigned int* pOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(unsigned char* pOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(unsigned short* pOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(unsigned int* pOutValue, MXmlElement* pElement, const char* name, unsigned int nDefaultValue);
CML2_API bool _Attribute(unsigned long* pOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(unsigned long* pOutValue, MXmlElement* pElement, const char* name, unsigned int nDefaultValue);
CML2_API bool _Attribute(unsigned __int64* pOutValue, MXmlElement* pElement, const char* name, unsigned __int64 nDefaultValue=0);
CML2_API bool _Attribute(short* pOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(short* pOutValue, MXmlElement* pElement, const char* name, short nDefaultValue);

CML2_API bool _Attribute(int& nOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(int& nOutValue, MXmlElement* pElement, const char* name, int nDefaultValue);
CML2_API bool _Attribute(unsigned int& nOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(unsigned char& pOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(unsigned short& pOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(unsigned int& nOutValue, MXmlElement* pElement, const char* name, unsigned int nDefaultValue);
CML2_API bool _Attribute(unsigned long& nOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(unsigned long& nOutValue, MXmlElement* pElement, const char* name, unsigned int nDefaultValue);
CML2_API bool _Attribute(unsigned __int64& nOutValue, MXmlElement* pElement, const char* name, unsigned __int64 nDefaultValue=0);
CML2_API bool _Attribute(short& nOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(short& nOutValue, MXmlElement* pElement, const char* name, short nDefaultValue);

// 실수 속성값
CML2_API bool _Attribute(float* pOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(float* pOutValue, MXmlElement* pElement, const char* name, float fDefaultValue);

CML2_API bool _Attribute(float& pOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(float& pOutValue, MXmlElement* pElement, const char* name, float fDefaultValue);

// 불린 속성값
CML2_API bool _Attribute(bool* pOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(bool* pOutValue, MXmlElement* pElement, const char* name, bool bDefaultValue);

CML2_API bool _Attribute(bool& pOutValue, MXmlElement* pElement, const char* name);
CML2_API bool _Attribute(bool& pOutValue, MXmlElement* pElement, const char* name, bool bDefaultValue);
/// 엘리먼트의 컨텐츠값을 반환한다. --------------------------------------------------------------------------

// 문자열 컨텐츠
CML2_API bool _Contents(std::string& strOutValue, MXmlElement* pElement, MXml* pXml=NULL);
CML2_API bool _Contents(std::string& strOutValue, MXmlElement* pElement, const char* szDefaultValue, MXml* pXml=NULL);

CML2_API bool _Contents(std::wstring& strOutValue, MXmlElement* pElement, MXml* pXml=NULL);
CML2_API bool _Contents(std::wstring& strOutValue, MXmlElement* pElement, const char* szDefaultValue, MXml* pXml=NULL);

// 문자열 컨텐츠
CML2_API bool _Contents(char* pOutValue, MXmlElement* pElement, MXml* pXml=NULL);
CML2_API bool _Contents(char* pOutValue, MXmlElement* pElement, const char* szDefaultValue, MXml* pXml=NULL);

// 정수 컨텐츠
CML2_API bool _Contents(int* pOutValue, MXmlElement* pElement);
CML2_API bool _Contents(int* pOutValue, MXmlElement* pElement, int nDefaultValue);

// 실수 컨텐츠
CML2_API bool _Contents(float* pOutValue, MXmlElement* pElement);
CML2_API bool _Contents(float* pOutValue, MXmlElement* pElement, float fDefaultValue);

// 불린 컨텐츠
CML2_API bool _Contents(bool* pOutValue, MXmlElement* pElement);
CML2_API bool _Contents(bool* pOutValue, MXmlElement* pElement, bool bDefaultValue);

/// 엘리먼트의 컨텐츠값을 반환한다.
CML2_API void _SetContents(MXmlElement* pElement, const std::string& szText);
CML2_API void _SetContents(MXmlElement* pElement, const char* szText);
CML2_API void _SetContents(MXmlElement* pElement, int nValue);
CML2_API void _SetContents(MXmlElement* pElement, float fValue);
CML2_API void _SetContents(MXmlElement* pElement, bool bValue);

/// 엘리먼트의 속성값을 설정한다.
CML2_API void _SetAttribute(MXmlElement* pElement, const char* szName, const char* szValue);
CML2_API void _SetAttribute(MXmlElement* pElement, const char* szName, const std::string& szValue);
CML2_API void _SetAttribute(MXmlElement* pElement, const char* szName, int nValue);
CML2_API void _SetAttribute(MXmlElement* pElement, const char* szName, float fValue);
CML2_API void _SetAttribute(MXmlElement* pElement, const char* szName, bool bValue);

///////////////////////////////////////////////////////
// 사용방법
//
// 읽기 ----------
//
//	MXml xml;
//	if (xml.LoadFile(szFileName) == false) return;
//
//	MXmlHandle docHandle = xml.DocHandle();
//	MXmlElement* child = docHandle.FirstChild("XML").FirstChild(szTagName).Element();
//
//	for( child; child != NULL; child=child->NextSiblingElement() )
//	{
//		char szTagName[256],text[512], ansi[512];
//		strcpy(szTagName, child->Value());
//		log(szLogFIle, szTagName);
//
//		MXml* p = &xml;
//		_Attribute(ansi, child, "id", "", p);
//
//		sprintf(text, "%s -> ", ansi);
//		log(szLogFIle, text);
//
//		_Contents(text, child, "", &xml);
//		strcat(text, "\n");
//		log(szLogFIle, text);
//	}
//
// 저장 ----------
//	MXml xml;
//
//	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
//	xml.Doc()->LinkEndChild(pDec);
//	
//	MXmlElement* pElement1 = new MXmlElement( "Earth" );
//	xml.Doc()->LinkEndChild(pElement1);
//
//	std::string str = MLocale::ConvAnsiToUTF8( "남기룡" );
//	MXmlElement* pElement = new MXmlElement( "Hello" );
//	pElement1->LinkEndChild(pElement);
//	_SetContents(pElement, " World");
//
//	////////
//	xml.Doc()->SaveFile("c:/111.xml");
//