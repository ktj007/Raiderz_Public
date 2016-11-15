#pragma once

#include "VFileName.h"
#include "VErrorMsg.h"

class VFieldInfoMgr 
{
public:
	VErrorMsg							m_ErrorMsg;


public:
	VFieldInfoMgr();
	virtual ~VFieldInfoMgr();

	//virtual CSItemData*					NewItemData();
	//virtual bool						ParseItem(MXmlElement* pElement, CSItemData* pItemData, MXml* pXml);
	//virtual void						Cooking();
	//virtual void						Clear();

	//XItemData*							GetItemData(int id);						// id의 아이템 데이타를 리턴.
};

