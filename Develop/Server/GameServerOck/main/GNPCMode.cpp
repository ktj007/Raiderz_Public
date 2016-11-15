#include "StdAfx.h"
#include "GNPCMode.h"
#include "GEntityNPC.h"
#include "GNPCHitcapsuleGroup.h"
#include "GScriptManager.h"
#include "GScriptGroup_NPC.h"
#include "GGlobal.h"
#include "GInteractionSystem.h"
#include "GPlayerNPCIconSender.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GNPCInteractor.h"

GNPCMode::GNPCMode(GEntityNPC* pOwner)
: m_pOwner(pOwner)
, m_nMode(NPC_MODE_DEFAULT)
{
}

GNPCMode::~GNPCMode(void)
{
}

void GNPCMode::Init(NPC_MODE nMode)
{
	SetMode(nMode);
}

void GNPCMode::ChangeMode( NPC_MODE nNextMode )
{
	SetMode(nNextMode);

	GNPCInfo* pNewInfo = m_pOwner->GetNPCInfo();

	gsys.pInteractionSystem->GetNPCInteractor().End(m_pOwner);

	m_pOwner->SendNPCIcon();

	m_pOwner->GetNPCStatusForced().nAA = NAAT_INVALID;
	m_pOwner->GetNPCStatusForced().nAttackable = NAT_INVALID;

	if (m_pOwner->GetNPCStatusForced().nAttackable == NAT_INVALID)
	{
		// 초기 상태는 attackable의 변경이 없을 때만 유효
		m_pOwner->ChangeAttackable(pNewInfo->nInitAttackable);
	}

	if (m_pOwner->GetNPCStatusForced().nAA == NAAT_INVALID)
	{
		// 초기 상태는 attackable의 변경이 없을 때만 유효
		m_pOwner->GetNPCStatus().nAA = pNewInfo->nAA;
	}
}

NPC_MODE GNPCMode::GetMode() const
{
	return m_nMode;
}

void GNPCMode::SetMode( NPC_MODE nNextMode )
{
	VALID(m_pOwner->GetNPCInfoMaster());
	if (nNextMode > (int)m_pOwner->GetNPCInfoMaster()->vecModes.size()) 
		return;

	NPC_MODE nBeforeMode = m_nMode;
	m_nMode = nNextMode;
	m_pOwner->GetNPCHitcapsuleGroup().ChangeCapsuleGroup_ByMode(m_nMode);
	if (m_pOwner->GetDelegator())
	{
		GetScriptNPC()->OnChangeMode(m_pOwner->GetDelegator(), nBeforeMode, m_nMode);
	}
}