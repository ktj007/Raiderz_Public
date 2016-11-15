#pragma once

#include "LTestHelper.h"

//////////////////////////////////////////////////////////////////////////
struct FBaseMockLink
{
	FBaseMockLink()
	{

	}
	virtual ~FBaseMockLink()
	{
		LTestHelper::ClearLinks();
	}

	MockLink* NewLink(MUID uidNew=MUID::Invalid())
	{
		return LTestHelper::NewLink(uidNew);
	}
};
