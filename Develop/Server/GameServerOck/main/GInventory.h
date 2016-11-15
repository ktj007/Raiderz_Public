#pragma once

#include "GItemContainer.h"

class GItem;

class GInventory: public GItemContainer
{
public:
	GInventory(GEntityPlayer* pOwner);
	virtual ~GInventory();
	
	virtual void	Clear(void);
	virtual int		GetSlotSize(void) const override;

	virtual GItem*	IncreaseAmount(int nSlotID, int nAmount) override;
	virtual GItem*	DecreaseAmount(int nSlotID, int nAmount) override;
	virtual GItem*	SetAmount(int nSlotID, int nAmount) override;
	virtual bool	PushItem(int slotID, GItem* pItem) override;
	virtual GItem*	PopItem(int slotID) override;

	bool	HasGatherTool(GATHER_TYPE eGatherType);		
	GItem*	GetItemByGatherType(GATHER_TYPE gatherType, int nStartSlotID);	
	void	CollectVeryCommonItem(vector<GItem*>& outvecitem);

private:	
	bool			m_isExpended;
	vector<int>		m_vecHadGatherTools;	
};
