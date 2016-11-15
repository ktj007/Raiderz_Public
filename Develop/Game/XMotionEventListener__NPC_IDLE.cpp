#include "stdafx.h"
#include "XMotionEventListener__NPC_IDLE.h"
#include "XModuleMotion.h"
#include "XNonPlayer.h"
#include "XMotionMgr.h"

XMotion* XMotionEventLister_NPC_IDLE::OnPreEnter( XActor* pOwner )
{
	if (pOwner->GetEntityType() == ETID_NPC) 
	{
		XNonPlayer* pNonPlayer = static_cast<XNonPlayer*>(pOwner);
		if (pNonPlayer->IsCombating() && pNonPlayer->HasCombatIdleAnimation())
		{
			XMotion* pIdleCombat = info.motion->GetMotion(MOTION_NAME_NPC_IDLE_COMBAT);
			return pIdleCombat;
		}
	}

	return NULL;
}