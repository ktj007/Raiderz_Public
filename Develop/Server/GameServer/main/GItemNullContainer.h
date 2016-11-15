#pragma once

#include "GItemContainer.h"

class GEntityPlayer;

class GItemNullContainer : public GItemContainer, public MTestMemPool<GItemNullContainer>
{
public:
	GItemNullContainer(GEntityPlayer* pOwner);
	~GItemNullContainer(void);
};
