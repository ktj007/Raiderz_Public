#include "stdafx.h"
#include "GJobT_Die.h"
#include "GEntityNPC.h"
#include "GNPCLoot.h"
#include "GHateTable.h"
#include "GEntityPlayer.h"

GJobT_Die::GJobT_Die(GModuleAI* pmAI, MUID uidRewarder) 
: GInstantJob(pmAI)
, m_uidRewarder(uidRewarder)
{
}

GJobT_Die::~GJobT_Die()
{
	// do nothing
}

bool GJobT_Die::OnStart()
{
	if (m_uidRewarder.IsValid())
	{
		GEntityPlayer* pRewarder = GetNPC()->FindPlayer(m_uidRewarder);
		if (pRewarder)
		{
			GetNPC()->GetHateTable().AddPoint_FoundEnemy(pRewarder);
		}
	}
	
	GetNPC()->doDie();

	return true;
}

string GJobT_Die::GetParamString() const
{
	GEntityNPC* pTarget = GetOwnerNPC()->FindNPC(m_uidRewarder);
	if (NULL == pTarget)	
		return "Rewarder: invalid_target";

	char buff[512];
	sprintf(buff, "Rewarder: %s(%I64d)", 
	MLocale::ConvUTF16ToAnsi(pTarget->GetName()).c_str(), m_uidRewarder.Value);
	return buff;
}
