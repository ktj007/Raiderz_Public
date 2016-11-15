#include "StdAfx.h"
#include "GUTHelper_Actor.h"
#include "GEntityActor.h"
#include "SUnitTestUtil.h"

CHR_INFO* GUTHelper_Actor::DefaultActorCharInfo()
{
	static CHR_INFO info;
	info.nMaxHP = 10000;
	info.nMaxEN = 10000;
	info.nMaxSTA = 10000;
	info.fSpeed = 100;
	return &info;
}

