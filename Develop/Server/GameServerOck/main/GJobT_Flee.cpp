#include "StdAfx.h"
#include "GJobT_Flee.h"
#include "GEntityNPC.h"

GJobT_Flee::GJobT_Flee(GModuleAI* pmAI, FleeType nFleeType, float fDurationTime) 
: GInstantJob(pmAI)
, m_nFleeType(nFleeType)
, m_fDurationTime(fDurationTime)
{

}

GJobT_Flee::~GJobT_Flee()
{

}

bool GJobT_Flee::OnStart()
{
	GetNPC()->Flee(m_nFleeType, m_fDurationTime);

	return true;
}

string GJobT_Flee::GetParamString() const
{
	char buff[512];
	sprintf(buff, "Type: %d, DurationTime: %.1f", 
		m_nFleeType, m_fDurationTime);
	return buff;
}
