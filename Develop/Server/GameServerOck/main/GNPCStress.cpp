#include "StdAfx.h"
#include "GNPCStress.h"
#include "GEntityNPC.h"
#include "GModuleAI.h"
#include "GColtMgr.h"

GNPCStress::GNPCStress(GEntityNPC* pOwner)
: m_pOwner(pOwner)
, m_nStress(0)
, m_bStressChanged(false)
{
	m_pOwner->AttachObserver(this);
}

GNPCStress::~GNPCStress(void)
{
	m_pOwner->DetachObserver(this);
}

int GNPCStress::GetStress() const
{
	return m_nStress;
}

void GNPCStress::AddStress( int val )
{
	m_nStress += val;
	m_bStressChanged = true;
}

void GNPCStress::SetStress( int val )
{
	m_nStress = val;
}

void GNPCStress::Clear()
{
	m_bStressChanged = false;
	m_nStress = 0;
}

void GNPCStress::Update( float fDelta )
{
	if (!m_bStressChanged)
		return;

	m_bStressChanged = false;
	m_pOwner->GetModuleAI()->GetColt()->RunColtStress(m_pOwner);
}

void GNPCStress::OnCombatBegin(GEntityActor* pOwner)
{
	Clear();
}

void GNPCStress::OnHitEnemy( GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	Clear();
}

