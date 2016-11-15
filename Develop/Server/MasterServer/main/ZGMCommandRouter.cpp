#include "StdAfx.h"
#include "ZGMCommandRouter.h"
#include "ZCommandCenter.h"
#include "ZCommandTable.h"

void ZGMCommandRouter::RouteSummon( MUID uidTarget, MUID uidTargetPlayer, MUID uidFieldGroup, int nFieldID, CHANNELID nChannelID, vec3 vPos)
{
	MCommand* pCmd = gsys.pCommandCenter->MakeNewCommand(MMC_GM_SUMMON
		, uidTarget
		, 5
		, NEW_UID(uidTargetPlayer)
		, NEW_UID(uidFieldGroup)
		, NEW_INT(nFieldID)
		, NEW_INT(nChannelID)
		, NEW_VEC(vPos));

	gsys.pCommandCenter->PostCommand(pCmd);
}
