#include "stdafx.h"
#include "GJobT_Attack.h"
#include "GEntityNPC.h"

GJobT_Attack::GJobT_Attack(GModuleAI* pmAI, MUID uidTarget, int nDamage)
: GInstantJob(pmAI)
, m_uidTarget(uidTarget)
, m_nDamage(nDamage)
{
	// do nothing
}

GJobT_Attack::~GJobT_Attack()
{
	// do nothingwl
}

bool GJobT_Attack::OnStart()
{
	GetNPC()->doAttackLazy(m_uidTarget, m_nDamage);
	return true;
}

string GJobT_Attack::GetParamString() const
{
	GEntityNPC* pTarget = GetOwnerNPC()->FindNPC(m_uidTarget);
	if (NULL == pTarget)	
		return "Target: invalid_target";

	char buff[512];
	sprintf(buff, "Target: %s, Damage: %d", 
		MLocale::ConvUTF16ToAnsi(pTarget->GetName()).c_str(), m_nDamage);
	return buff;
}
