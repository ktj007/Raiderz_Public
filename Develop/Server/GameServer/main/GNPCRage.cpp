#include "StdAfx.h"
#include "GNPCRage.h"
#include "GConst.h"
#include "GEntityNPC.h"

GNPCRage::GNPCRage(GEntityNPC* pOwner)
: m_pOwner(pOwner)
, m_nRage(0)
{
	m_pOwner->AttachObserver(this);
}

GNPCRage::~GNPCRage( void )
{
	m_pOwner->DetachObserver(this);
}

void GNPCRage::OnHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	AddRage(GConst::HIT_RAGE_AMOUNT);
}

void GNPCRage::OnCriHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	AddRage(GConst::CRIHIT_RAGE_AMOUNT);
}

void GNPCRage::ResetRage()
{
	m_nRage = 0;
}

int GNPCRage::GetRage() const
{
	return m_nRage;
}

void GNPCRage::AddRage( int nAmount )
{
	const int MAX_RAGE = 10000;
	if (m_nRage + nAmount >= MAX_RAGE)
	{
		m_nRage = MAX_RAGE;
		return;
	}

	m_nRage += nAmount;
}
