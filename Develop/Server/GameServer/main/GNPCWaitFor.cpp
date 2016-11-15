#include "StdAfx.h"
#include "GNPCWaitFor.h"
#include "GField.h"
#include "GEntityNPC.h"

GNPCWaitFor::GNPCWaitFor(GEntityNPC* pOwner)
: m_pOwner(pOwner)
{
}

GNPCWaitFor::~GNPCWaitFor(void)
{
}

void GNPCWaitFor::AddWaiter( MUID uidWaiter )
{
	m_vecWaiters.push_back(uidWaiter);
}

void GNPCWaitFor::AddWaiting( MUID uidTarget )
{
	if (m_pOwner->GetUID() == uidTarget)
		return;	// 자기자신을 기다릴 수 없음

	m_setWaitings.insert(uidTarget);
}

void GNPCWaitFor::RunNextAction()
{
	GField* pField = m_pOwner->GetField();
	VALID(pField);

	for each (const MUID& each in m_vecWaiters)
	{
		GEntityNPC* pNPC = pField->FindNPC(each);
		if (!pNPC)
			continue;

		pNPC->GetNPCWaitFor().OnNextAction(m_pOwner->GetUID());
	}

	m_vecWaiters.clear();
}

void GNPCWaitFor::OnNextAction( MUID uidActor )
{
	m_setWaitings.erase(uidActor);
}

bool GNPCWaitFor::IsWaiting()
{
	return !m_setWaitings.empty();
}

