#include "StdAfx.h"
#include "GJobT_FaceTo.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GModuleAI.h"

GJobT_FaceTo::GJobT_FaceTo(GModuleAI* pmAI, MUID uidTarget)
: GInstantJob(pmAI)
, m_uidTarget(uidTarget)
{
}

GJobT_FaceTo::~GJobT_FaceTo(void)
{
}

bool GJobT_FaceTo::OnStart()
{
	GEntityActor* pTarget = GetNPC()->FindActor(m_uidTarget);
	if (!pTarget)
		return false;

	GetNPC()->SetDirForwardToActor(pTarget, true);
	return true;
}

string GJobT_FaceTo::GetParamString() const
{
	GEntityNPC* pTarget = m_pOwner->GetOwnerNPC()->FindNPC(m_uidTarget);
	if (NULL == pTarget)	
		return "Target: invalid_target";

	char buff[512];
	sprintf(buff, "Target: %s", 
		MLocale::ConvUTF16ToAnsi(pTarget->GetName()).c_str());
	return buff;
}
