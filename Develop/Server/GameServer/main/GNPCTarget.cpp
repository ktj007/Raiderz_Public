#include "StdAfx.h"
#include "GNPCTarget.h"
#include "GEntityNPC.h"

GNPCTarget::GNPCTarget(GEntityNPC* pOwner)
: m_pOwner(pOwner)
{
}

GNPCTarget::~GNPCTarget(void)
{
}

MUID GNPCTarget::GetTargetUID() const
{
	return m_uidTarget;
}

void GNPCTarget::SetTargetUID( MUID uidTarget )
{
	m_uidTarget = uidTarget;
}

void GNPCTarget::ClearTargetUID()
{
	m_uidTarget.SetInvalid();
}

GEntityActor* GNPCTarget::GetTarget() const
{
	if (m_uidTarget.IsInvalid())
		return NULL;

	return m_pOwner->FindActor(m_uidTarget);
}

