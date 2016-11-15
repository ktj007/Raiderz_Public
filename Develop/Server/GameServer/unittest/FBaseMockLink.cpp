#include "stdafx.h"
#include "FBaseMockLink.h"
#include "GUTHelper.h"

MockLink* FBaseMockLink::NewLink( GEntityPlayer* pPlayer, bool bIgnoreAllSendCommandID /*= false*/ )
{
	return GUTHelper::NewLink(pPlayer, bIgnoreAllSendCommandID);
}

MockLink* FBaseMockLink::NewLink( MUID uidLink, bool bIgnoreAllSendCommandID /*= false*/ )
{
	return GUTHelper::NewLink(uidLink, bIgnoreAllSendCommandID);
}

void FBaseMockLink::ClearLinks()
{
	GUTHelper::ClearLinks();
}