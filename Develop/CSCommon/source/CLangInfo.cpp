#include "stdafx.h"
#include "CLangInfo.h"
#include "MLocale.h"


// lang.xml 엘리먼트 및 어트리뷰트
#define LANGUAGES_XML_TAG_MAIET		"maiet"
#define LANGUAGES_XML_TAG_LANGS		"LANGS"
#define LANGUAGES_XML_TAG_LANG		"LANG"

#define LANGUAGES_XML_ATTR_NAME		"name"
#define LANGUAGES_XML_ATTR_COUNTRY	"country"
#define LANGUAGES_XML_ATTR_LOCALE	"locale"
#define LANGUAGES_XML_ATTR_PATH		"path"


CLangInfo::CLangInfo()
{
	// do nothing
}

CLangInfo::~CLangInfo()
{
	// do nothing
}

bool CLangInfo::Load(wstring strFileName)
{
	// 파일 열기
	MXml xml;

	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(strFileName.c_str()).c_str()))
	{
		return false;
	}


	// 마이에트 루트
	MXmlHandle doc = xml.DocHandle();	
	MXmlElement* pRoot = doc.FirstChild(LANGUAGES_XML_TAG_MAIET).Element();
	if (pRoot == NULL)	return false;


	// 언어 그룹
	MXmlElement* pLanguages = pRoot->FirstChildElement(LANGUAGES_XML_TAG_LANGS);
	if (pLanguages == NULL)	return false;


	// 개별 언어 설정
	MXmlElement* pLang = pLanguages->FirstChildElement();

	while(pLang != NULL)
	{
		if (0 == _stricmp(pLang->Value(), LANGUAGES_XML_TAG_LANG))
		{
			CLangNode node;

			if (!_Attribute(node.strName, pLang, LANGUAGES_XML_ATTR_NAME, &xml))		continue;
			if (!_Attribute(node.strCountry, pLang, LANGUAGES_XML_ATTR_COUNTRY, &xml))	continue;
			if (!_Attribute(node.strLocale, pLang, LANGUAGES_XML_ATTR_LOCALE, &xml))	continue;
			if (!_Attribute(node.strPath, pLang, LANGUAGES_XML_ATTR_PATH, &xml))		continue;

			__super::push_back(node);
		}
		
		pLang = pLang->NextSiblingElement();
	}


	// 기본 로케일 설정
	wstring strDefaultLocale;
	_Attribute(strDefaultLocale, pLanguages, LANGUAGES_XML_ATTR_LOCALE, &xml);

	for each(const CLangNode& node in *this)
	{
		// 기본 로케일 설정이 없거나 못찾으면, 첫번째 언어를 기본으로 설정
		if (m_strLocale.empty())
		{
			m_strLocale = node.strLocale;
		}

		// 기본 로케일과 일치하는 언어로 설정
		if (strDefaultLocale == node.strLocale)
		{
			m_strLocale = node.strLocale;
			break;
		}
	}

	return true;
}

wstring CLangInfo::GetLocale(void) const
{
	return m_strLocale;
}
