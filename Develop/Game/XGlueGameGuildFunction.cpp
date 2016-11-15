#include "stdafx.h"
#include "XGlueGameGuildFunction.h"
#include "XPost_Guild.h"
#include "XInteractionInfo.h"

void XGlueGameGuildFunction::GuildDestroy()
{
	XPostGuild_Destroy();
}

void XGlueGameGuildFunction::GuildInviteQ()
{
	XPostGuild_Invite_Req(XGetInteractionInfo().InteractionTargetUID);
}

void XGlueGameGuildFunction::GuildRequiredAnswer( bool bAnswer)
{
	//XPostGuild_Invite_Res(bAnswer);
}
