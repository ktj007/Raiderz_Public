#ifndef _GITEM_DATA_H
#define _GITEM_DATA_H

#include "CSItemData.h"

class GItemData : public CSItemData, public MTestMemPool<GItemData>
{
public:	
	
	GItemData();
	virtual ~GItemData();
};



#endif // _GITEM_DATA_H