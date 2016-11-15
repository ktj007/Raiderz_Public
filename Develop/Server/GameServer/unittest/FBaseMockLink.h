#pragma once

#include "MockLink.h"

class GEntityPlayer;

class FBaseMockLink
{
public:
	FBaseMockLink()
	{

	}
	virtual ~FBaseMockLink()
	{
		ClearLinks();
	}

	MockLink* NewLink( GEntityPlayer* pPlayer, bool bIgnoreAllSendCommandID = false);
	MockLink* NewLink( MUID uidLink, bool bIgnoreAllSendCommandID = false);	
	void ClearLinks();
};
