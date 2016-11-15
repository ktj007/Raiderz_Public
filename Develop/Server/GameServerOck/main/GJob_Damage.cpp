#include "StdAfx.h"
#include "GJob_Damage.h"
#include "GEntityNPC.h"

GJob_Damage::GJob_Damage(GModuleAI* pmAI, uint32 nDamageType, uint32 nDamage, MUID	uidOpponent) 
: GInstantJob(pmAI)
, m_nDamageType(nDamageType)
, m_nDamage(nDamage)
, m_uidOpponent(uidOpponent)
{

}

GJob_Damage::~GJob_Damage()
{

}

bool GJob_Damage::OnStart()
{
	if (GetNPC()->doDamage(m_uidOpponent, DAMAGE_ATTRIB(m_nDamageType), m_nDamage))
	{
		GetNPC()->doDie();
	}

	return true;
}
