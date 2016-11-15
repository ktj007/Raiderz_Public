#include "stdafx.h"
#include "GJobT_GainBuff.h"
#include "GEntityNPC.h"

GJobT_GainBuff::GJobT_GainBuff(GModuleAI* pmAI, int nBuffID) 
: GInstantJob(pmAI)
, m_nBuffID(nBuffID)
{
	// do nothing
}

GJobT_GainBuff::~GJobT_GainBuff()
{
	// do nothing
}

bool GJobT_GainBuff::OnStart()
{	
	GetNPC()->GainBuff(m_nBuffID);
	return true;
}

string GJobT_GainBuff::GetParamString() const
{
	char buff[512];
	sprintf(buff, "BuffID: %d", 
		m_nBuffID);
	return buff;
}
