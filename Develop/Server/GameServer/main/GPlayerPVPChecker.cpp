#include "StdAfx.h"
#include "GPlayerPVPChecker.h"
#include "GConst.h"
#include "GEntityPlayer.h"

GPlayerPVPChecker::GPlayerPVPChecker(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
, m_bNowPVP(false)
{
	m_rgrReleaser.Stop();

	m_pOwner->AttachObserver(this);
}

GPlayerPVPChecker::~GPlayerPVPChecker()
{
	m_pOwner->DetachObserver(this);
}

void GPlayerPVPChecker::doMakePeace()
{
	m_bNowPVP = false;
}

bool GPlayerPVPChecker::IsNowPVP() const
{
	return m_bNowPVP;
}

void GPlayerPVPChecker::OnHit( GEntityActor* pOwner, GEntityActor* pAttacker, GTalentInfo* pTalentInfo )
{
	VALID(pAttacker);
	VALID(pTalentInfo);
	if (!pAttacker->IsPlayer())
		return;	// 플레이어 아니면 무시

	if (!pTalentInfo->HasDamage())
		return;	// 피해 없으면 무시

	BeginPVP();
}

void GPlayerPVPChecker::OnHitEnemy( GEntityActor* pOwner, uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo )
{
	VALID(pTarget);
	VALID(pTalentInfo);
	if (!pTarget->IsPlayer())
		return;	// 플레이어 아니면 무시

	if (!pTalentInfo->HasDamage())
		return;	// 피해 없으면 무시

	BeginPVP();
}

void GPlayerPVPChecker::Update( float fDelta )
{
	PFC_THISFUNC;
	if (!m_rgrReleaser.IsReady(fDelta))
		return;

	FinishPVP();
}

void GPlayerPVPChecker::BeginPVP()
{
	m_rgrReleaser.SetTickTime(GConst::PVP_DURATIONTIME);
	m_rgrReleaser.Start();
	m_bNowPVP = true;

	m_pOwner->OnCombatBegin(m_pOwner);
}

void GPlayerPVPChecker::FinishPVP()
{
	m_rgrReleaser.Stop();
	m_bNowPVP = false;
}

