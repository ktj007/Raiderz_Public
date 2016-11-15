#pragma once

#include "XItemRelatedNPCInfoMgr.h"

class XItem_DroperInfoMgr : public XItemRelatedNPCInfoMgr
{
public:
	XItem_DroperInfoMgr(void);
	~XItem_DroperInfoMgr(void);

	size_t GetDropperCount( int nItemID);
	const wchar_t* GetDropperName( int nItemID, int nIndex);
};
