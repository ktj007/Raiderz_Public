#include "stdafx.h"
#include "XColorTable.h"

#define COLORTABLE_XML_TAG_MAIET				"maiet"
#define COLORTABLE_XML_TAG_COLOR_TABLE			"ColorTable"
#define COLORTABLE_XML_TAG_COLOR				"Color"

#define COLORTABLE_XML_ATTR_NAME				"name"
#define COLORTABLE_XML_ATTR_ID					"id"
#define COLORTABLE_XML_ATTR_COLOR				"color"

bool XColorTable::Load( const wchar_t* szFileName )
{
	MXml xml;

	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) 
	{
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(COLORTABLE_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), COLORTABLE_XML_TAG_COLOR_TABLE))
		{	
			ParseColorTable(pElement);
		}
	}

	return true;
}

void XColorTable::ParseColorTable( MXmlElement* pXmlElement )
{
	XColorGroup* pColorGroup = new XColorGroup();

	wstring strName;
	_Attribute(strName, pXmlElement, COLORTABLE_XML_ATTR_NAME, "default");

	pColorGroup->SetName(strName.c_str());

	// 0번은 없는 색깔로 취급한다.
	pColorGroup->SetColor(0, MCOLOR(0));

	MXmlElement* child;
	for( child = pXmlElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement() )
	{
		if (!_stricmp(child->Value(), COLORTABLE_XML_TAG_COLOR))
		{
			wstring strColor;
			int id = 0;

			if (_Attribute(&id, child, COLORTABLE_XML_ATTR_ID) == false) continue;
			if (_Attribute(strColor, child, COLORTABLE_XML_ATTR_COLOR) == false) continue;
			
			uint32 nColor = ParseColorValue(strColor.c_str());

			pColorGroup->SetColor(id, MCOLOR(nColor));
		}
	}

	AddColorGroup(pColorGroup);
}

void XColorTable::AddColorGroup( XColorGroup* pColorGroup )
{
	wstring strGroupName = pColorGroup->GetName();

	if (GetGroup(strGroupName.c_str()) != NULL) 
	{
		_ASSERT(0);
		delete pColorGroup;
		return;
	}

	m_ColorGroupMap.insert(map<wstring, XColorGroup*>::value_type(strGroupName, pColorGroup));
}

void XColorTable::DeleteAllColorGroups()
{
	for (map<wstring, XColorGroup*>::iterator itor = m_ColorGroupMap.begin(); itor != m_ColorGroupMap.end(); ++itor)
	{
		XColorGroup* pColorGroup = (*itor).second;
		delete pColorGroup;
	}
	m_ColorGroupMap.clear();
}

XColorTable::~XColorTable()
{
	DeleteAllColorGroups();
}

XColorGroup* XColorTable::GetGroup( const wchar_t* szGroupName )
{
	map<wstring, XColorGroup*>::iterator itor = m_ColorGroupMap.find(wstring(szGroupName));
	if (itor == m_ColorGroupMap.end()) return NULL;
	return (*itor).second;
}

mint3::MCOLOR XColorTable::GetColor( const wchar_t* szGroupName, int nID )
{
	XColorGroup* pColorGroup = GetGroup(szGroupName);
	if (pColorGroup)
	{
		return pColorGroup->GetColor(nID);
	}
	return MCOLOR(0);
}

uint32 XColorTable::ParseColorValue( const wchar_t* szColorValue )
{
	uint32 nColor = wcstol(szColorValue, NULL, 16);		// XML에 16진수로 저장되어있다.

	nColor = 0xFF000000 | nColor;

	return nColor;
}

int XColorGroup::GetID( size_t nIndex)
{
	if ( nIndex < 0)		return -1;

	map<int,MCOLOR>::iterator itr = m_mapColors.begin();
	for ( size_t i = 0;  i < nIndex;  i++, itr++);
	
	return (*itr).first;
}

void XColorGroup::SetColor( int nID, MCOLOR color )
{
	if (nID<0) return;

	m_mapColors.insert( map<int,MCOLOR>::value_type( nID, color));
}

MCOLOR XColorGroup::GetColor( int nID )
{
	map<int,MCOLOR>::iterator _find = m_mapColors.find( nID);
	if ( _find != m_mapColors.end())		return (*_find).second;

	return MCOLOR( 0);
}