#pragma once

#include "CSItemManager.h"
#include "VFileName.h"
#include "VErrorMsg.h"
#include "XItemInfoLoader.h"

class VItemInfoMgr : public CSItemManager
{
public:
	VErrorMsg							m_ErrorMsg;

	XItemDataMap						m_ItemDataMap;

public:
	VItemInfoMgr();
	virtual ~VItemInfoMgr();

	virtual CSItemData*					NewItemData();
	virtual bool						ParseItem(MXmlElement* pElement, CSItemData* pItemData, MXml* pXml);
	virtual void						Cooking();
	virtual void						Clear();

	XItemData*							GetItemData(int id);						// id의 아이템 데이타를 리턴.
};

