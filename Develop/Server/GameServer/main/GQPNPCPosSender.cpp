#include "StdAfx.h"
#include "GQPNPCPosSender.h"
#include "GConst.h"
#include "CCommandTable.h"
#include "GEntityNPC.h"
#include "GCommand.h"
#include "GField.h"
#include "GQuestPVP.h"


GQPNPCPosSender::GQPNPCPosSender(GEntityNPC* pNPC)
: m_pOwner(pNPC), m_bEnable(false)
{
	m_pRegular = new MRegulator();
	m_pRegular->SetTickTime(GConst::QUESTPVP_MARKPOS_REFRESH_TIME);
}

GQPNPCPosSender::~GQPNPCPosSender(void)
{
	SAFE_DELETE(m_pRegular);
}

void GQPNPCPosSender::Enable()
{
	if (true == m_bEnable) return;

	m_bEnable = true;
	SendMarkPos();
}

void GQPNPCPosSender::Disable()
{
	if (false == m_bEnable) return;

	m_bEnable = false;
	SendUnmarkPos();
}

void GQPNPCPosSender::Update(float fDelta)
{
	if (false == m_bEnable) return;
	if (false == m_pRegular->IsReady(fDelta))	return;

	SendMarkPos();
}

void GQPNPCPosSender::SendMarkPos()
{
	GField* pField = m_pOwner->GetField();
	if (NULL == pField) return;	

	MCommand* pNewCmd = MakeNewCommand(MC_QUESTPVP_MARK_NPCPOS,
		2,
		NEW_INT(m_pOwner->GetID()),
		NEW_VEC(m_pOwner->GetPos()));

	pField->GetQuestPVP()->RouteToQuestPVPTeam(pNewCmd);
}

void GQPNPCPosSender::SendUnmarkPos()
{
	GField* pField = m_pOwner->GetField();
	if (NULL == pField) return;	

	MCommand* pNewCmd = MakeNewCommand(MC_QUESTPVP_UNMARK_NPCPOS,
		1,
		NEW_INT(m_pOwner->GetID()));

	pField->GetQuestPVP()->RouteToQuestPVPTeam(pNewCmd);
}

void GQPNPCPosSender::OnDead()
{
	Disable();
}

void GQPNPCPosSender::OnDestroy()
{
	Disable();
}
