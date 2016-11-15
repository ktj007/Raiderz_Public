#include "stdafx.h"
#include "GZoneInfo.h"
#include "MXml.h"
#include "MLocale.h"


bool GZoneInfoFileLoader::Load( const wchar_t* szZoneFileName, GZoneInfo& outZoneInfo )
{
	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szZoneFileName).c_str())) 
	{
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(SERVERZONE_XML_TAG_ZONE).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		char szValue[256];

		if (!_stricmp(pElement->Value(), SERVERZONE_XML_TAG_MAP_SIZE))
		{
			if (_Contents(szValue, pElement))
			{
				sscanf(szValue, "%f %f %f", &outZoneInfo.m_vMapSize.x, &outZoneInfo.m_vMapSize.y, &outZoneInfo.m_vMapSize.z);
			}
		}
		else if (!_stricmp(pElement->Value(), SERVERZONE_XML_TAG_TERRAIN_SIZE))
		{
			if (_Contents(szValue, pElement))
			{
				sscanf(szValue, "%f %f %f", &outZoneInfo.m_vTerrainSize.x, &outZoneInfo.m_vTerrainSize.y, &outZoneInfo.m_vTerrainSize.z);
			}
		}
	}

	return true;
}