#include "StdAfx.h"
#include "GJobT_Yell.h"
#include "GEntityNPC.h"

GJobT_Yell::GJobT_Yell(GModuleAI* pmAI, int nRange) 
: GInstantJob(pmAI)
, m_nRange(nRange)
{

}

GJobT_Yell::~GJobT_Yell()
{

}

bool GJobT_Yell::OnStart()
{
	GetNPC()->doYell(m_nRange);

	return true;
}

string GJobT_Yell::GetParamString() const
{
	char buff[512];
	sprintf(buff, "Range: %d", 
		m_nRange);
	return buff;
}

