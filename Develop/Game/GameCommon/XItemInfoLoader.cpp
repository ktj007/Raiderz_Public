#include "stdafx.h"
#include "XItemInfoLoader.h"
#include "XStrings.h"
#include "XDef.h"

#define ITEM_XML_ATTR_MESH_PATH			"mesh_path"
#define ITEM_XML_ATTR_NPC_USE_ONLY		"npc_use_only"
#define ITEM_XML_ATTR_ICON				"icon"

bool XItemInfoLoader::ParseItem(MXmlElement* pElement, CSItemData* pItemData, MXml* pXml, XItemDataMap& itemDataMap)
{
	ParseItem_NPC(pElement, pItemData);
	ParseItem_Icon(pElement, pItemData);

	tstring strSoundType;
	if (_Attribute(strSoundType, pElement, "Item_sound"))
	{
		pItemData->m_ItemSoundType = static_cast<SOUND_TYPE>(XStrings::StringToSoundType(strSoundType.c_str()));
	}

	if (0 == pItemData->m_nID)
		return false;

	itemDataMap.insert(XItemDataMap::value_type(pItemData->m_nID, (XItemData*)pItemData));
	return true;
}

void XItemInfoLoader::ParseItem_NPC( MXmlElement* pElement, CSItemData* pItemData )
{
	tstring strPath;
	if (_Attribute(strPath, pElement, ITEM_XML_ATTR_MESH_PATH))
	{
		if (!strPath.empty())
		{
			if (strPath[strPath.length()-1] != '/' || strPath[strPath.length()-1] != '\\')
				strPath  += '/';

			strPath = PATH_MODEL + strPath;

			pItemData->m_strMeshPath = strPath;
		}
	}
}

void XItemInfoLoader::ParseItem_Icon( MXmlElement* pElement, CSItemData* pItemData )
{
	_Attribute(pItemData->m_strIconName, pElement, ITEM_XML_ATTR_ICON);
}