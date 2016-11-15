#pragma once

#include "GItemContainer.h"

class GGuildStorage : public GItemContainer, public MTestMemPool<GGuildStorage>
{
public:
	GGuildStorage();
	virtual ~GGuildStorage(void);

	virtual int		GetSlotSize(void) const override;

	virtual bool	PushItem(int slotID, GItem* pItem) override;

private:
	bool	m_isExpended;
};
