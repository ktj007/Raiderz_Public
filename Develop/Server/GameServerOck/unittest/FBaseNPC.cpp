#include "stdafx.h"
#include "FBaseNPC.h"
#include "GUTHelper_NPC.h"

GNPCInfo* FBaseNPC::NewNPCInfo( int nNPCId /*= INVALID_ID*/ )
{
	return GUTHelper_NPC::NewNPCInfo(nNPCId);
}

GEntityNPC* FBaseNPC::NewNPC( MockField* pField, int nNPCId, const vec3& vPos)
{
	return GUTHelper_NPC::SpawnNPC(pField, nNPCId, vPos);
}