#include "stdafx.h"
#include "GJobT_Assist.h"
#include "GEntityNPC.h"

GJobT_Assist::GJobT_Assist(GModuleAI* pmAI, MUID uidTarget) 
: GInstantJob(pmAI)
, m_uidTarget(uidTarget)
{
	// do nothing
}

GJobT_Assist::~GJobT_Assist()
{
	// do nothing
}

bool GJobT_Assist::OnStart()
{
	GEntityNPC* pTarget = GetNPC()->FindNPC(m_uidTarget);
	if (!pTarget)
		return false;

	GetNPC()->doAssist(pTarget);
	return true;
}

string GJobT_Assist::GetParamString() const
{
	GEntityNPC* pTarget = GetOwnerNPC()->FindNPC(m_uidTarget);
	if (NULL == pTarget)	
		return "Target: invalid_target";

	char buff[512];
	sprintf(buff, "Target: %s", 
		MLocale::ConvUTF16ToAnsi(pTarget->GetName()).c_str());
	return buff;
}
