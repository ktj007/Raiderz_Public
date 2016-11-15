#pragma once

class GItem;

class GPlayerDye : public MTestMemPool<GPlayerDye>
{
public:
	GPlayerDye();

	void Prepare(SH_ITEM_SLOT_TYPE	nTargetSlotType, int nTargetSlotID, int	nDyeSlotID);
	void Clear();

	bool IsDyeingItem(const GItem* pItem);
	
private:
	SH_ITEM_SLOT_TYPE	m_nTargetSlotType;
	int	m_nTargetSlotID;
	int	m_nDyeSlotID;
};
