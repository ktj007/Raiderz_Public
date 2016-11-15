#include "StdAfx.h"
#include "GJobT_Summon.h"
#include "GEntityNPC.h"
#include "GNPCMinion.h"

GJobT_Summon::GJobT_Summon(GModuleAI* pmAI, int nNPCID, vec3 vStartPos) 
: GInstantJob(pmAI)
, m_nNPCID(nNPCID)
, m_vStartPos(vStartPos)
{

}

GJobT_Summon::~GJobT_Summon()
{

}

bool GJobT_Summon::OnStart()
{
	GetNPC()->GetNPCMinion().Summon(m_nNPCID, m_vStartPos);

	return true;
}

string GJobT_Summon::GetParamString() const
{
	char buff[512];
	sprintf(buff, "NPCID: %d, TarPos: %.1f %.1f %.1f", 
		m_nNPCID, m_vStartPos.x, m_vStartPos.y, m_vStartPos.z);
	return buff;
}