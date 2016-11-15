#include "stdafx.h"
#include "CSNPCInfoHelper.h"

bool CSNPCInfoHelper::IsUsingGravityNPC( NPC_TYPE nNPCType )
{
	if (nNPCType == NPC_TYPE_OBJECT ||
		nNPCType == NPC_TYPE_GATHER) return false;

	return true;
}