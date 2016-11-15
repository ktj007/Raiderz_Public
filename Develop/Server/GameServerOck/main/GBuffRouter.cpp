#include "StdAfx.h"
#include "GBuffRouter.h"
#include "GBuff.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GPartySystem.h"
#include "GBuffInfo.h"

GBuffRouter::GBuffRouter(GBuff* pOwner)
: m_pOwner(pOwner)
{
	m_pOwner->AttachObserver(this);
}

GBuffRouter::~GBuffRouter(void)
{
	m_pOwner->DetachObserver(this);
}

void GBuffRouter::NotifyForParty()
{
	GEntitySync* pOwnerActor = m_pOwner->GetOwner();

	if (!pOwnerActor->IsPlayer())	
		return;

	gsys.pPartySystem->UpdateMember(ToEntityPlayer(pOwnerActor));
}

void GBuffRouter::RouteGainBuff()
{
	GEntitySync* pOwnerActor = m_pOwner->GetOwner();
	VALID(pOwnerActor);

	if (!pOwnerActor->IsActor())	
		return;

	MCommand* pNewCommand = MakeNewCommand(MC_BUFF_GAIN, 3, 
		NEW_USHORT(ToEntityActor(pOwnerActor)->GetUIID()), 
		NEW_INT(m_pOwner->GetBuffID()), 
		NEW_FLOAT(m_pOwner->GetDurationTime()));

	pOwnerActor->RouteToThisCell(pNewCommand);
}

void GBuffRouter::RouteLostBuff()
{
	GEntitySync* pOwnerActor = m_pOwner->GetOwner();
	VALID(pOwnerActor);

	if (!pOwnerActor->IsActor())	
		return;

	MCommand* pNewCommand = MakeNewCommand(MC_BUFF_LOST, 2,
		NEW_USHORT(ToEntityActor(pOwnerActor)->GetUIID()), 
		NEW_INT(m_pOwner->GetBuffID()));

	pOwnerActor->RouteToThisCell(pNewCommand);
}

void GBuffRouter::OnGain(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo)
{
	VALID(pTarget);
	VALID(pBuffInfo);

	bool bNotify = true;
	
	if (pBuffInfo->IsInstantEffect())
	{
		bNotify = false;
	}

	if (pBuffInfo->IsEnchant())
	{
		bNotify = false;
	}


	if (bNotify)
	{
		NotifyForParty();
		RouteGainBuff();
	}
}

void GBuffRouter::OnLost(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo, bool bRoute)
{
	VALID(pBuffInfo);

	bool bNotify = true;

	if (pBuffInfo->IsInstantEffect())
	{
		bNotify = false;
	}

	if (pBuffInfo->IsEnchant())
	{
		bNotify = false;
	}


	if (bNotify)
	{
		NotifyForParty();
		if (bRoute)
		{
			RouteLostBuff();
		}
	}
}

void GBuffRouter::OnDuplicated(MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo)
{
	VALID(pBuffInfo);
	if (pBuffInfo->m_bUpdateRoute)
	{
		RouteGainBuff();
	}
}