#include "StdAfx.h"
#include "XTextureMtInfo.h"

#define TALENT_XML_TAG_MAIET				"maiet"
#define TALENT_XML_TAG_TEXTURE				"TEXTURE"

#define TALENT_XML_ATTR_NAME				"name"
#define TALENT_XML_ATTR_MATERIAL			"mt"

XTextureMtInfoMgr::XTextureMtInfoMgr(void)
{
}

XTextureMtInfoMgr::~XTextureMtInfoMgr(void)
{
	clear();
}

bool XTextureMtInfoMgr::Load( const wchar_t* szFileName )
{
	MXml xml;

	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TALENT_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), TALENT_XML_TAG_TEXTURE))
		{
			ParseTexture(pElement);
		}
	}

	return true;
}

void XTextureMtInfoMgr::ParseTexture(MXmlElement* pElement)
{
	wstring strName;
	wstring strMt;

	_Attribute(strName,	pElement, TALENT_XML_ATTR_NAME);
	_Attribute(strMt,	pElement, TALENT_XML_ATTR_MATERIAL);

	wchar_t szLowerKey[256];
	wcscpy(szLowerKey, strName.c_str());
	_wcslwr(szLowerKey);

	insert(value_type(szLowerKey, strMt));
}

wstring XTextureMtInfoMgr::GetMaterial(const wchar_t* texturename)
{
	if (texturename != NULL)
	{
		wchar_t szLowerKey[256];
		wcscpy(szLowerKey, texturename);
		_wcslwr(szLowerKey);

		MAP_TEXTURE_MT::iterator it = find(szLowerKey);
		if(it != end())
		{
			return (*it).second;
		}
	}

	 return L"pnt";
}
