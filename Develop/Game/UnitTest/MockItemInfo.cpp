#include "stdafx.h"
#include "MockItemInfo.h"

bool MockItemInfoMgr::CreateItemFromString( const std::wstring& _strXmlData )
{
	CSItemData* pNewItemData = NewItemData();
	return SetItem(pNewItemData, _strXmlData);
}

bool MockItemInfoMgr::SetItem( CSItemData* pItemData, const std::wstring& _strXmlData )
{
	USES_CONVERSION_EX;

	MXmlDocument doc;
	doc.Parse(W2A_EX(_strXmlData.c_str(), 0));
	MXmlElement* pRootElement = doc.FirstChildElement();
	if(pRootElement == NULL)
		return false;

	ParseItem(pRootElement, pItemData, NULL);

	return true;
}

void MockItemInfoMgr::InsertItemData( XItemData* pItemData )
{
	m_ItemDataMap.insert(XItemDataMap::value_type(pItemData->m_nID, (pItemData)));
}