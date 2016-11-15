#include "stdafx.h"
#include "GItemHolder.h"
#include "GInventory.h"
#include "GEquipmentSlot.h"
#include "GItem.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GCommand.h"
#include "CCommandTable.h"
#include "GStorage.h"
#include "GItemNullContainer.h"
#include "AStlUtil.h"
#include "GGuildStorage.h"
#include "GGuildMgr.h"

GItemHolder::GItemHolder(GEntityPlayer* pOwner, GInventory* pInventory, GEquipmentSlot* pEquipment, GStorage* pStorage)
: m_pOwner(pOwner)
{
	_ASSERT(pOwner != NULL);
	_ASSERT(pEquipment != NULL);
	_ASSERT(pInventory != NULL);
	_ASSERT(pStorage != NULL);

	m_pEquipment = pEquipment;
	m_pInventory = pInventory;
	m_pStorage = pStorage;

	m_pNullContainer = new GItemNullContainer(pOwner);	
}

GItemHolder::~GItemHolder()
{
	SAFE_DELETE(m_pEquipment);
	SAFE_DELETE(m_pInventory);
	SAFE_DELETE(m_pStorage);
	SAFE_DELETE(m_pNullContainer);
}

void GItemHolder::Clear(void)
{
	m_pEquipment->Clear();
	m_pInventory->Clear();
	m_pStorage->Clear();	
	m_pNullContainer->Clear();
}

GItemContainer& GItemHolder::GetContainer( SH_ITEM_SLOT_TYPE nSlotType ) const
{
	if (SLOTTYPE_EQUIP == nSlotType)
	{
		return *m_pEquipment;
	}
	else if (SLOTTYPE_INVENTORY == nSlotType)
	{
		return *m_pInventory;
	}
	else if (SLOTTYPE_STORAGE == nSlotType)
	{
		return *m_pStorage;
	}
	else if (SLOTTYPE_GUILDSTORAGE == nSlotType)
	{
		GGuildStorage* pGuildStorage = gmgr.pGuildMgr->GetStorage(m_pOwner->GetGID());
		if (NULL != pGuildStorage)
			return *pGuildStorage;
	}

	return *m_pNullContainer;
}

GEquipmentSlot& GItemHolder::GetEquipment() const
{
	return *m_pEquipment;
}

GInventory& GItemHolder::GetInventory() const
{
	return *m_pInventory;
}

GStorage& GItemHolder::GetStorage() const
{
	return *m_pStorage;
}


GItem* GItemHolder::GetItem( SH_ITEM_SLOT_TYPE nSlotType, int nSlotID ) const
{
	return GetContainer(nSlotType).GetItem(nSlotID);
}

GItem* GItemHolder::GetItem(SH_ITEM_SLOT_TYPE nSlotType, int64 nIUID) const
{
	return GetContainer(nSlotType).GetItem(nIUID);
}

GItem* GItemHolder::GetItem(SH_ITEM_SLOT_TYPE nSlotType, MUID uidItem ) const
{
	return GetContainer(nSlotType).GetItem(uidItem);
}

GItem* GItemHolder::GetMergeableItem( SH_ITEM_SLOT_TYPE nSlotType, int nItemID, int nMergeAmt ) const
{
	return GetContainer(nSlotType).GetMergeableItem(nItemID, nMergeAmt);
}

int GItemHolder::GetItemCount( SH_ITEM_SLOT_TYPE nSlotType ) const
{
	return GetContainer(nSlotType).GetItemCount();
}

int GItemHolder::GetItemCount( SH_ITEM_SLOT_TYPE nSlotType, int id )
{
	return GetContainer(nSlotType).GetItemCountByID(id);
}

bool GItemHolder::PushItem( SH_ITEM_SLOT_TYPE nSlotType,int nSlotID, GItem* pItem )
{
	return GetContainer(nSlotType).PushItem(nSlotID, pItem);
}

GItem* GItemHolder::IncreaseAmount( SH_ITEM_SLOT_TYPE nSlotType, int nSlotID, int nAmount )
{
	return GetContainer(nSlotType).IncreaseAmount(nSlotID, nAmount);
}

GItem* GItemHolder::DecreaseAmount( SH_ITEM_SLOT_TYPE nSlotType, int nSlotID, int nAmount )
{
	return GetContainer(nSlotType).DecreaseAmount(nSlotID, nAmount);
}

GItem* GItemHolder::SetAmount( SH_ITEM_SLOT_TYPE nSlotType, int nSlotID, int nAmount )
{
	return GetContainer(nSlotType).SetAmount(nSlotID, nAmount);
}

GItem* GItemHolder::PopItem( SH_ITEM_SLOT_TYPE nSlotType,int nSlotID )
{
	return GetContainer(nSlotType).PopItem(nSlotID);
}

void GItemHolder::CollectAllItem( SH_ITEM_SLOT_TYPE nSlotType, vector<GItem*>& vecItem )
{
	return GetContainer(nSlotType).CollectAllItem(vecItem);
}

void GItemHolder::CollectItemByID(SH_ITEM_SLOT_TYPE nSlotType, int nID, vector<GItem*>& outvecItem )
{
	return GetContainer(nSlotType).CollectItemByID(nID, outvecItem);
}

void GItemHolder::CollectExpiringItem(SH_ITEM_SLOT_TYPE nSlotType, int nCharPlayTimeMin, vector<GItem*>& outvecItem )
{
	return GetContainer(nSlotType).CollectExpiringItem(nCharPlayTimeMin, outvecItem);
}

int GItemHolder::CollectItemUntilAmount(SH_ITEM_SLOT_TYPE nSlotType, int nItemID, int nNeedAmount, GITEM_STACK_AMT_VEC& outvecItemStackAmt)
{
	return	GetContainer(nSlotType).CollectItemUntilAmount(nItemID, nNeedAmount, outvecItemStackAmt);		
}

void GItemHolder::MakeTD(SH_ITEM_SLOT_TYPE nSlotType, vector<TD_ITEM>& outvecTDItem)
{
	vector<GItem*> vecItem;
	CollectAllItem(nSlotType, vecItem);

	outvecTDItem.resize(vecItem.size());
	
	for(size_t i = 0; i < vecItem.size(); i++)
	{
		outvecTDItem[i] = vecItem[i]->ToTD_ITEM();
	}
}

int GItemHolder::ReserveEmptySlot(SH_ITEM_SLOT_TYPE nSlotType,int nStartSlotID)
{
	return GetContainer(nSlotType).ReserveEmptySlot(nStartSlotID);
}

void GItemHolder::ReserveSlot( SH_ITEM_SLOT_TYPE nSlotType, int nSlotID )
{
	return GetContainer(nSlotType).ReserveSlot(nSlotID);
}

bool GItemHolder::IsReservedSlot(SH_ITEM_SLOT_TYPE nSlotType,  int nSlotID)
{
	return GetContainer(nSlotType).IsReservedSlot(nSlotID);
}

void GItemHolder::CancelReserveSlot(SH_ITEM_SLOT_TYPE nSlotType, int nReservedSlotID)
{
	return GetContainer(nSlotType).CancelReserveSlot(nReservedSlotID);
}

void GItemHolder::ReserveDummySlot(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID)
{
	return GetContainer(nSlotType).ReserveDummySlot(nSlotID);
}

bool GItemHolder::IsReservedDummySlot( SH_ITEM_SLOT_TYPE nSlotType, int nSlotID )
{
	return GetContainer(nSlotType).IsReservedDummySlot(nSlotID);
}

void GItemHolder::CollectReservedDummySlotInfo(vector<ITEM_SLOT_INFO>& vecDummySlotInfo)
{
	vector<int> vecEquipDummySlotID;
	vector<int> vecInvenDummySlotID;	
	vector<int> vecStorageDummySlotID;

	GetContainer(SLOTTYPE_EQUIP).CollectReservedDummySlot(vecEquipDummySlotID);
	GetContainer(SLOTTYPE_INVENTORY).CollectReservedDummySlot(vecInvenDummySlotID);
	GetContainer(SLOTTYPE_STORAGE).CollectReservedDummySlot(vecStorageDummySlotID);

	for each (int nDummySlotID in vecEquipDummySlotID)
	{
		vecDummySlotInfo.push_back(ITEM_SLOT_INFO(SLOTTYPE_EQUIP, nDummySlotID));
	}
	for each (int nDummySlotID in vecInvenDummySlotID)
	{
		vecDummySlotInfo.push_back(ITEM_SLOT_INFO(SLOTTYPE_INVENTORY, nDummySlotID));
	}	
	for each (int nDummySlotID in vecStorageDummySlotID)
	{
		vecDummySlotInfo.push_back(ITEM_SLOT_INFO(SLOTTYPE_STORAGE, nDummySlotID));
	}
}

int	GItemHolder::GetSlotSize(SH_ITEM_SLOT_TYPE nSlotType) const
{
	return GetContainer(nSlotType).GetSlotSize();
}

int GItemHolder::GetEmptySlotCount(SH_ITEM_SLOT_TYPE nSlotType) const
{
	return GetContainer(nSlotType).GetEmptySlotCount();
}

bool GItemHolder::HasItemAddableSlot(SH_ITEM_SLOT_TYPE nSlotType, const vector<CHECK_ADDABLE_ITEM>& vecCheckAddableItem )
{
	return GetContainer(nSlotType).HasItemAddableSlot(vecCheckAddableItem);
}

bool GItemHolder::IsExist( SH_ITEM_SLOT_TYPE nSlotType, int nItemID )
{
	return GetContainer(nSlotType).IsExist(nItemID);
}

bool GItemHolder::IsFull(SH_ITEM_SLOT_TYPE nSlotType) const
{
	return GetContainer(nSlotType).IsFull();
}