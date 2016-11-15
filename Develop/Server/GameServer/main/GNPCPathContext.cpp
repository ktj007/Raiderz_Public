#include "StdAfx.h"
#include "GNPCPathContext.h"
#include "GEntityNPC.h"
#include "GField.h"

GNPCPathContext::GNPCPathContext(GEntityNPC* pOwner)
: m_pOwner(pOwner)
{
	m_pOwner->AttachObserver(this);
}

GNPCPathContext::~GNPCPathContext(void)
{
	m_pOwner->DetachObserver(this);
}

void GNPCPathContext::OnAddHateEntry( GEntityActor* pEnemy )
{
	MUID uidTarget = pEnemy->GetUID();
	GField* pField = m_pOwner->GetField();
	if (!pField) return;

	pField->RegisterToPathGroup(uidTarget, m_pOwner);
}

void GNPCPathContext::OnCombatEnd(GEntityActor* pOwner)
{
	GField* pField = m_pOwner->GetField();
	if (!pField) return;

	pField->RevokeFromPathGroup(m_pOwner);
}