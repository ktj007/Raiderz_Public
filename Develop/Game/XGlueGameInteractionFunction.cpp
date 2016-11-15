#include "stdafx.h"
#include "XGlueGameInteractionFunction.h"
#include "XPost_Interaction.h"
#include "XPost_Sensor.h"
#include "XInteractionInfo.h"

void XGlueGameInteractionFunction::InteractionElementReq(int InteractionType, int nElementID)
{
	if (InteractionType == MENU_NPC)
	{
		XPostNPCInteraction_IE(XGetInteractionInfo().InteractionTargetUID, nElementID);
	}
	else if (InteractionType == MENU_SECTOR)
	{
		XPostSensorTriggerSelection(nElementID);
	}
	else if (InteractionType == MENU_FOLLOW_WARP)
	{
		XPosSensorTriggerFollowWarp(nElementID);
	}
}

void XGlueGameInteractionFunction::InteractionEndRequest()
{
	XPostNPCInteraction_End();
}
