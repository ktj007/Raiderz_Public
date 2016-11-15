#ifndef _XMY_XMYLOOTINGITEMCONTAIRER_H
#define _XMY_XMYLOOTINGITEMCONTAIRER_H


#include <vector>
#include "XObjectManager.h"


struct XLooterInfo
{
	MUID		m_UID;
	wstring		m_strName;

	XLooterInfo( const MUID& uid, const wstring& strName) : m_UID( uid), m_strName( strName)			{}
};





struct XLootingItem
{
	MUID					m_NpcUID;
	MUID					m_ItemUID;
	int						m_nItemID;
	DROP_ITEM_TYPE			m_nItemType;
	int						m_nAmount;
	vector< XLooterInfo>	m_vLooter;
};






class XMyLootingItemContainer
{
public:
	float						fTime;

private:
	vector< XLootingItem*>		m_vItemList;


public:
	XMyLootingItemContainer() : fTime( 0.0f)		{}
	virtual ~XMyLootingItemContainer()			{ Clear();									}

	size_t GetSize() const						{ return  m_vItemList.size();				}

	void Clear()
	{
		for ( vector< XLootingItem*>::iterator itr = m_vItemList.begin();  itr != m_vItemList.end();  itr++)		delete (*itr);
		m_vItemList.clear();
	}

	void InsertItem( const TD_ITEM_DROP& tdDropItem)
	{
		if ( tdDropItem.m_nItemID <= 0  ||  tdDropItem.m_nAmount < 0)	return;

		XLootingItem* pItem = new XLootingItem();
		pItem->m_ItemUID = tdDropItem.m_nDropItemUID;
		pItem->m_nItemID = tdDropItem.m_nItemID;
		pItem->m_nItemType = tdDropItem.m_nDropItemType;
		pItem->m_nAmount = tdDropItem.m_nAmount;
		m_vItemList.push_back( pItem);
	}

	void InsertItem( const MUID& uidNPC, const TD_LOOT_MASTERLOOT_UI& tdLootItem)
	{
		if ( tdLootItem.m_nItemID <= 0  ||  tdLootItem.m_nItemAmount < 0)	return;

		XLootingItem* pItem = new XLootingItem();
		pItem->m_NpcUID = uidNPC;
		pItem->m_ItemUID = tdLootItem.m_nDropItemUID;
		pItem->m_nItemID = tdLootItem.m_nItemID;
		pItem->m_nAmount = tdLootItem.m_nItemAmount;

		for ( int i = 0;  i < MAX_PARTY_MEMBER_COUNT;  i++)
		{
			MUID uID = tdLootItem.m_Beneficiarys[ i];
			if ( uID.IsValid() == false)		break;

			wstring strName;
			XObject* pObject = gg.omgr->Find( uID);
			if ( pObject != NULL  &&  pObject->GetEntityType() == ETID_PLAYER)	strName = pObject->GetName();

			pItem->m_vLooter.push_back( XLooterInfo( uID, strName));
		}
		m_vItemList.push_back( pItem);
	}

	void DeleteItem( int nIndex)
	{
		vector< XLootingItem*>::iterator itr = m_vItemList.begin();
		for ( int i = 0;  i < nIndex;  i++, itr++)		;
		delete (*itr);
		m_vItemList.erase( itr);
	}

	XLootingItem* Get( int nindex)				{ return m_vItemList[ nindex];				}
};

#endif // _XMY_XMYLOOTINGITEMCONTAIRER_H