#ifndef _XITEM_CONTAINER_H
#define _XITEM_CONTAINER_H

class XItem;

class XTradeItemContainer
{
		typedef	vector<XItem*> XItemVector;
protected:
		size_t			m_nNowItemIdx;
		XItemVector		m_vContains;
		MUID			m_uidOwner;
		XTradeItemContainer& operator = (const XTradeItemContainer& rhs); 

public:
		XTradeItemContainer();
		virtual ~XTradeItemContainer();

		virtual void	PushItem(XItem* item);
		virtual bool	PopItem(XItem* item);
		virtual XItem*	InsertItem(const TD_ITEM& tdItem);
		virtual XItem*	InsertItem(const TD_ITEM_INSERT& tdItemInsert);		
		bool			IncreaseItemAmount(int nSlotID, int nAmount);

		size_t			GetSize();
		XItem*			GetItem(int index);
		XItem*			FirstItem();
		XItem*			NextItem();		

		void			Reset();
};

class XItemContainer
{
public:
	XItemContainer(size_t slotSize);
	virtual ~XItemContainer();

	void			Clear();
	virtual  int	GetSlotSize()	{ return m_vecItem.size(); }
	int				GetSize();

	void			PushItem(int nSlotID, XItem* pItem);
	XItem*			PopItem(int nSlotID);

	int				GetItemAmountByID(int nItemID);

	XItem*			GetItemByID(int nItemID);	
	XItem*			GetItem(int nSlotID);

	vector<XItem*>	GetAllItem();	

	XItem*			InsertItem(const TD_ITEM_INSERT& tdItemInsert);
	XItem*			InsertItem(int nSlotID, XItem* pItem, int nAmount);

	bool			IncreaseItemAmount(int nSlotID, int nAmount);
	bool			DecreaseItemAmount(int nSlotID, int nAmount);

	void			ChangeSlot(int nCurSlotID, int nTargetSlotID);
	void			Merge(int nCurSlotID, int nTargetSlotID, int nMergeAmount );

	void			ClearAllNewItemFlag();

	bool			IsHaveItem(int nItemID);
	bool			IsEmpty();

protected:
	vector<XItem*>	m_vecItem;
	
	void			SetSlotItem(int nSlotID,  XItem* pItem);
	bool			UpdateItemAmount(int nSlotID, int nAmount);
};

#endif // _XITEM_CONTAINER_H