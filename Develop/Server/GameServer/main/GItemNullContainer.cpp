#include "StdAfx.h"
#include "GItemNullContainer.h"
#include "GEntityPlayer.h"

GItemNullContainer::GItemNullContainer(GEntityPlayer* pOwner)
: GItemContainer(0, pOwner)
{
}

GItemNullContainer::~GItemNullContainer(void)
{
}
