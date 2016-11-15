#pragma once

#include "XItemManager.h"

class MockItemInfoMgr : public XItemManager
{
public:
	virtual bool CreateItemFromString( const std::wstring& _strXmlData );
	bool SetItem( CSItemData* pItemData, const std::wstring& _strXmlData );

	XItemData* NewItemData()
	{
		return (new XItemData());
	}

	void					InsertItemData(XItemData* pItemData);
};
