#include "stdafx.h"
#include "GItemTalent.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GField.h"
#include "GModuleBuff.h"
#include "GBuffInfo.h"
#include "CCommandTable.h"
#include "GModuleCombat.h"
#include "GTalentHitter.h"
#include "GTalentEffector.h"
#include "GCommand.h"
#include "GTalentHelper.h"

GItemTalent::GItemTalent( GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bGainStress ) 
: GTalent(pOwner, pTalentInfo, Target, bGainStress)
{
}

void GItemTalent::OnEnterPhaseAct()
{
	__super::OnEnterPhaseAct();

	GTalentHelper::AddHitterForTargetAndNonProjTalent(this);
}