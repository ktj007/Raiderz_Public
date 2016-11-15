#include "stdafx.h"
#include "GArcheryTalent.h"
#include "GTalentRouter.h"

void GArcheryTalent::OnStart()
{
	GTalent::OnStart();

	m_TalentProjectile.Init(this);

	m_pRouter->RouteAttackArchery();
}

void GArcheryTalent::OnUpdatePhaseAct( float fDelta )
{
	PFI_BLOCK_THISFUNC(9050);

	GTalent::OnUpdatePhaseAct(fDelta);

	ChangePhase(TALENT_PHASE_FINISH);
}