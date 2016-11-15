#include "stdafx.h"
#include "GJobT_Despawn.h"
#include "GEntityNPC.h"
#include "GGlobal.h"
#include "GTestSystem.h"

GJobT_Despawn::GJobT_Despawn(GModuleAI* pmAI, bool bSpawnEnable) 
: GInstantJob(pmAI)
, m_bSpawnEnable(bSpawnEnable)
{
	// do nothing
}

GJobT_Despawn::~GJobT_Despawn()
{
	// do nothing
}

bool GJobT_Despawn::OnStart()
{
	GEntityNPC* pNPCOwner = GetNPC();
	pNPCOwner->doDespawn(m_bSpawnEnable);
	gsys.pTestSystem->DespawnLog(pNPCOwner->GetUID(), __FUNCTIONW__);
	return true;
}

string GJobT_Despawn::GetParamString() const
{
	char buff[512];
	sprintf(buff, "SpawnEnable: %s", 
		m_bSpawnEnable?"true":"false");
	return buff;
}
