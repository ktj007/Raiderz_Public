#pragma once

#include "GITEM_STACK_AMT.h"
#include "CTransData.h"

class GInventory;
class GEquipmentSlot;
class GItem;
class GStorage;
class GItemContainer;
class GItemNullContainer;
struct CHECK_ADDABLE_ITEM;

class GItemHolder : public MTestMemPool<GItemHolder>
{
public:
	GItemHolder(GEntityPlayer* pOwner, GInventory* pInventory, GEquipmentSlot* pEquipment, GStorage* pStorage);
	virtual ~GItemHolder();
	
	void	Clear(void);

	GEquipmentSlot&	GetEquipment() const;
	GInventory&		GetInventory() const;
	GStorage&		GetStorage() const;

	GItem*	GetItem(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID) const;
	GItem*	GetItem(SH_ITEM_SLOT_TYPE nSlotType, int64 nIUID) const;
	GItem*	GetItem(SH_ITEM_SLOT_TYPE nSlotType, MUID uidItem) const;
	GItem*	GetMergeableItem(SH_ITEM_SLOT_TYPE nSlotType, int nItemID, int nMergeAmt) const;

	int		GetItemCount(SH_ITEM_SLOT_TYPE nSlotType) const;
	int		GetItemCount(SH_ITEM_SLOT_TYPE nSlotType, int id);
	int		GetItemCountInSlot(SH_ITEM_SLOT_TYPE nSlotType, const int& slot) const;

	bool	PushItem(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID, GItem* pItem);
	GItem*	PopItem(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID);
	
	GItem*	IncreaseAmount(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID, int nAmount);
	GItem*	DecreaseAmount(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID, int nAmount);
	GItem*	SetAmount(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID, int nAmount);

	void	CollectAllItem(SH_ITEM_SLOT_TYPE nSlotType, vector<GItem*>& vecItem);
	void	CollectItemByID(SH_ITEM_SLOT_TYPE nSlotType, int nID, vector<GItem*>& outvecItem);
	void	CollectExpiringItem(SH_ITEM_SLOT_TYPE nSlotType, int nCharPlayTimeMin, vector<GItem*>& outvecItem);
	int		CollectItemUntilAmount(SH_ITEM_SLOT_TYPE nSlotType, int nItemID, int nNeedAmount, GITEM_STACK_AMT_VEC& outvecItemStackAmt);
	void	MakeTD(SH_ITEM_SLOT_TYPE nSlotType, vector<TD_ITEM>& outvecTDItem);	
	
	int		ReserveEmptySlot(SH_ITEM_SLOT_TYPE nSlotType, int nStartSlotID);
	void	ReserveSlot(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID);
	bool	IsReservedSlot(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID);
	void	CancelReserveSlot(SH_ITEM_SLOT_TYPE nSlotType, int nReservedSlotID);

	void	ReserveDummySlot(SH_ITEM_SLOT_TYPE nSlotType,int nSlotID);
	bool	IsReservedDummySlot(SH_ITEM_SLOT_TYPE nSlotType, int nSlotID);
	void	CollectReservedDummySlotInfo(vector<ITEM_SLOT_INFO>& vecDummySlotInfo);

	int		GetSlotSize(SH_ITEM_SLOT_TYPE nSlotType) const;
	int		GetEmptySlotCount(SH_ITEM_SLOT_TYPE nSlotType) const;
	bool	HasItemAddableSlot(SH_ITEM_SLOT_TYPE nSlotType, const vector<CHECK_ADDABLE_ITEM>& vecCheckAddableItem);

	bool	IsExist(SH_ITEM_SLOT_TYPE nSlotType, int nItemID);
	bool 	IsFull(SH_ITEM_SLOT_TYPE nSlotType) const;

private:
	GItemContainer&	GetContainer(SH_ITEM_SLOT_TYPE nSlotType) const;

private:
	GEntityPlayer*		m_pOwner;

	GEquipmentSlot*		m_pEquipment;
	GInventory*			m_pInventory;
	GStorage*			m_pStorage;
	GItemContainer*		m_pNullContainer;
};
	
