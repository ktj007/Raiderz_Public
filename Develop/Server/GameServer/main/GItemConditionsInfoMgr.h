#ifndef _G_ITEM_CONDITIONS_INFO_MGR_H_
#define _G_ITEM_CONDITIONS_INFO_MGR_H_

#include "GConditionsInfoMgr.h"

class GItemConditionsInfoMgr : public GConditionsInfoMgr, public MTestMemPool<GItemConditionsInfoMgr>
{
public:
	GItemConditionsInfoMgr(void);
	~GItemConditionsInfoMgr(void);
};

#endif
