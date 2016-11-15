#pragma once

#include "GTestWrapper.h"
#include "GItemData.h"
#include "GItemManager.h"
#include "SUnitTestUtil.h"

class FBaseItem
{
public:
	FBaseItem()
	{

	}
	virtual ~FBaseItem()
	{

	}

	GTestMgrWrapper<GItemManager>			m_ItemMgrWrapper;
};