#pragma once

#include "CSNPCInfoHelper.h"
#include "GNPCInfo.h"

class GNPCInfoHelper : public CSNPCInfoHelper
{
public:
	static bool IsThinkableObject(const GNPCInfo* pNPCInfo)
	{
		if (NPC_TYPE_OBJECT == pNPCInfo->nNpcType && false == pNPCInfo->m_bObjectThinkable)
		{
			return false;
		}
		else if (NPC_TYPE_GATHER == pNPCInfo->nNpcType)
		{
			return false;
		}

		return true;
	}
};
