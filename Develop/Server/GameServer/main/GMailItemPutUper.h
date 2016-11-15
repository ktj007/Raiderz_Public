#pragma once

#include "CCommandResultTable.h"

class GEntityPlayer;
class GItem;
struct TD_MAIL_RESERVE_ITEM;

class GMailItemPutUper : public MTestMemPool<GMailItemPutUper>
{
public:
	GMailItemPutUper();
	virtual ~GMailItemPutUper();	

	virtual bool Do(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pReserveItem);
	virtual bool Check(GEntityPlayer* pPlayer, const GItem* pItem, int nAddAmount);

	bool CheckAppendable(GEntityPlayer* pPlayer, const GItem* pItem);
	bool CheckItemAmount(GEntityPlayer* pPlayer, const GItem* pItem, int nAddAmount);
	bool CheckMailInteraction(GEntityPlayer* pPlayer);
	
	CCommandResultTable GetLastFailure(void);

private:	
	CCommandResultTable m_nLastFailure;
};
