#ifndef _XITEM_MANAGER_H
#define _XITEM_MANAGER_H

#include "CSItemManager.h"
#include "XItemInfoLoader.h"

class XItemManager : public CSItemManager
{
protected:
		XItemDataMap			m_ItemDataMap;
		XItemVec				m_ItemVec;							// [2007/9/6 isnara] 아이템 관리, 메모리릭으로 추가된 내용

		virtual CSItemData*		NewItemData();
		virtual bool			ParseItem(MXmlElement* pElement, CSItemData* pItemData, MXml* pXml) override;
public:
		XItemManager();
		virtual ~XItemManager();

		virtual void			Clear();
		virtual XItem*			MakeNewItem(int id);
		virtual XItem*			MakeNewItem(XItem* pItem, int nAmount);
		XItemData*				GetItemData(int id);						// id의 아이템 데이타를 리턴.
		bool					IsItemID(int id);

		void					DelItem(XItem* pItem);

		virtual void			Cooking();

		size_t					GetSize()	{ return m_ItemDataMap.size(); };
		XItemDataMap&			GetItemDataMap() { return m_ItemDataMap; }
		virtual bool CreateItemFromString( const std::wstring& _strXmlData ) { return false; }

		int						GetEnchantItemBuffID(XItem* pItem, int nEnchantSlot);
};
	

#endif // _XITEM_MANAGER_H