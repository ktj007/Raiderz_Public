#pragma once

#include "XItemData.h"
#include "XItem.h"

typedef map<int, XItemData*>	XItemDataMap;
typedef vector<XItem*>			XItemVec;

class XItemInfoLoader
{
protected:
	void					ParseItem_NPC(MXmlElement* pElement, CSItemData* pItemData);
	void					ParseItem_Icon( MXmlElement* pElement, CSItemData* pItemData );

public:
	XItemInfoLoader() {}
	virtual ~XItemInfoLoader() {}

	bool ParseItem(MXmlElement* pElement, CSItemData* pItemData, MXml* pXml, XItemDataMap& itemDataMap);

};