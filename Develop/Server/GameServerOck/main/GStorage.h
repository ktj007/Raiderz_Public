#pragma once

#include "GItemContainer.h"

class GStorage : public GItemContainer
{
public:
	GStorage(GEntityPlayer* pOwner);
	virtual ~GStorage(void);

	virtual void	Clear(void) override;
	virtual int		GetSlotSize(void) const override;

	virtual bool	PushItem(int slotID, GItem* pItem) override;


	bool	IsSerialized()	{ return m_bSerialized; }
	void	SetSerialized()	{ m_bSerialized = true; }

private:
	bool	m_isExpended;
	bool	m_bSerialized;
};
