#include "stdafx.h"
#include "GMagicTalent.h"
#include "GEntityNPC.h"
#include "GTalentHelper.h"

GMagicTalent::GMagicTalent( GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bGainStress ) 
: GTalent(pOwner, pTalentInfo, Target, bGainStress)
{
}

void GMagicTalent::OnStart()
{
	AimingDirection();
	__super::OnStart();
}

void GMagicTalent::OnEnterPhaseAct()
{
	AimingDirection();
	__super::OnEnterPhaseAct();
	GTalentHelper::AddHitterForTargetAndNonProjTalent(this);
}

void GMagicTalent::AimingDirection()
{
	if (!IsAimingDirectionTalent())
		return;

	GEntityNPC* pNPC = static_cast<GEntityNPC*>(m_pOwner);
	pNPC->SetDirForwardToEnemy();
	m_vInitialDir = pNPC->GetDir();
}

