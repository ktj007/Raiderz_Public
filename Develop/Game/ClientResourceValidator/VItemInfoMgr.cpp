#include "stdafx.h"
#include "VItemInfoMgr.h"

VItemInfoMgr::VItemInfoMgr()
{

}

VItemInfoMgr::~VItemInfoMgr()
{
	Clear();
}

CSItemData* VItemInfoMgr::NewItemData()
{
	return (new XItemData());
}

XItemData* VItemInfoMgr::GetItemData( int id )
{
	XItemDataMap::iterator i = m_ItemDataMap.find(id);
	if (i != m_ItemDataMap.end())
	{
		return ((*i).second);
	}
	return NULL;
}

void VItemInfoMgr::Cooking()
{
	for each (const XItemDataMap::value_type& each in m_ItemDataMap)
	{
		XItemData* pItemData = each.second;
		if (NULL == pItemData) continue;

		pItemData->Cooking();
	}
}

bool VItemInfoMgr::ParseItem( MXmlElement* pElement, CSItemData* pItemData, MXml* pXml )
{
	CSItemManager::ParseItem(pElement, pItemData, pXml);

	XItemInfoLoader infoLoader;
	return infoLoader.ParseItem(pElement, pItemData, pXml, m_ItemDataMap);
}

void VItemInfoMgr::Clear()
{
	for (XItemDataMap::iterator itor = m_ItemDataMap.begin(); itor != m_ItemDataMap.end(); ++itor)
	{
		SAFE_DELETE( (*itor).second );
	}
	m_ItemDataMap.clear();
}