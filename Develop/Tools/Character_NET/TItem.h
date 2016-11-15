#pragma once

#include "MUID.h"
#include "XItem.h"
#include "CSItemManager.h"
#include "CSDef.h"

class TItemManager : public CSItemManager
{
public:
	typedef map<int, CSItemData*>	CSItemDataMap;

protected:
	CSItemDataMap			m_ItemDataMap;

	virtual	CSItemData*		NewItemData();
	virtual bool			ParseItem(MXmlElement* pElement, CSItemData* pItemData, MXml* pXml) override;

public:
	TItemManager();
	virtual ~TItemManager();

	CSItemDataMap&			GetDataMap() { return m_ItemDataMap; }
	CSItemData*				GetItemData(int id);
	CSItemData*				GetItemData(string name);

	void					Save_SubMtrl(const char* filename);
	bool					AddSubMtrl_Item(MXmlElement* pItemElement, CSItemData* pItem);

	CSItemData*				MakeItemData(int id);

	void					Destroy();

	virtual void			Cooking();
};

