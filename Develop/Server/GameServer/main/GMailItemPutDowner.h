#pragma once

#include "CCommandResultTable.h"

class GEntityPlayer;
class GItem;
struct TD_MAIL_RESERVE_ITEM;

class GMailItemPutDowner : public MTestMemPool<GMailItemPutDowner>
{
public:
	GMailItemPutDowner();
	virtual ~GMailItemPutDowner();

	virtual bool Do(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pReserveItem);
	virtual bool Check(GEntityPlayer* pPlayer, const GItem* pItem, int nSubAmount);
	
	bool CheckMailInteraction(GEntityPlayer* pPlayer);
	bool CheckItemAmount(GEntityPlayer* pPlayer, const GItem* pItem, int nSubAmount);

	CCommandResultTable GetLastFailure(void);

private:
	CCommandResultTable m_nLastFailure;
};
