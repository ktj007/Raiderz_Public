#pragma once

#include "GITEM_STACK_AMT.h"

class GItem;
struct CHECK_ADDABLE_ITEM;

class GItemContainer
{
public:
	GItemContainer(size_t slotSize, GEntityPlayer* pOwnerPlayer);
	virtual ~GItemContainer();

	virtual void	Clear(void);	
	virtual void	CollectAllItem(vector<GItem*>& outvecitem);
	virtual int		GetSlotSize(void) const;

	virtual GItem*	IncreaseAmount(int nSlotID, int nAmount);
	virtual GItem*	DecreaseAmount(int nSlotID, int nAmount);
	virtual GItem*	SetAmount(int nSlotID, int nAmount);
	virtual bool	PushItem(int nSlotID, GItem* pItem);
	virtual GItem*	PopItem(int nSlotID);

	void CollectReservedDummySlot(vector<int>& vecDummySlotID);

	GEntityPlayer* GetOwnerPlayer(void);
	GItem* GetItem(int nSlotID) const;
	int GetItemCount(void) const;	
	bool IsEmpty(void) const;
	bool IsFull(void) const;	
	
	int	GetItemCountByID( int id );
	GItem* GetItem(int nItemID, int nFindStartSlotID);
	GItem* GetItem(MUID uidItem) const;
	GItem* GetItem(int64 nIUID) const;
	GItem* GetMergeableItem(int nItemID, int nMergeAmt);	

	void CollectItemByID(int nID, vector<GItem*>& outvecItem);
	void CollectExpiringItem(int nCharPlayTimeMin, vector<GItem*>& outvecItem);	
	int CollectItemUntilAmount( int nItemID, int nNeedAmount, GITEM_STACK_AMT_VEC& outvecItemStackAmt);	

	int		ReserveEmptySlot(int nFindStartSlotID);
	void	ReserveSlot(int nSlotID);
	void	CancelReserveSlot(int nSlotID);
	bool	IsReservedSlot(int nSlotID);
	int		GetReserveSlotCount() const { return m_nReserveCount; } 
	int		GetEmptySlotCount(void) const;

	bool	HasItemAddableSlot(const vector<CHECK_ADDABLE_ITEM>& vecCheckAddableItem);

	void	ReserveDummySlot(int nSlotID);
	bool	IsReservedDummySlot(int nSlotID);
	bool	IsExist(int nItemID);

		
private:
	bool CheckSlotID(int nSlotID) const;

private:	
	vector<GItem*>		m_vecItemSlot;
	int					m_nItemCount;	
	vector<bool>		m_vecSlotReserved;
	int					m_nReserveCount;	

protected:
	GEntityPlayer*		m_pOwnerPlayer;
	set<int>			m_setDummySlotReserved;	// DB에는 존재하고 서버의 xml에는 존재하지 않는 아이템의 슬롯을 가상점유
};
