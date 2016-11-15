#pragma once

#include "GTestWrapper.h"
#include "GFieldMgr.h"

class GSharedField;

struct FBaseFieldMgr
{
	FBaseFieldMgr()
	{

	}
	virtual ~FBaseFieldMgr()
	{

	}

	GSharedField* AddSharedField(MUID uidNewField, int nFieldID, int nChannelID);

	GTestMgrWrapper<GFieldMgr>		m_FieldMgrWrapper;
};