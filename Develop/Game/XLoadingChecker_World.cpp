#include "stdafx.h"
#include "XLoadingChecker_World.h"
#include "XGame.h"
#include "XWorld.h"

bool XLoadingChecker_World::CheckLoaded()
{
	if (gg.currentgamestate && gg.currentgamestate->GetWorld() && gg.currentgamestate->GetWorld()->GetTerrain())
	{
		if (gg.currentgamestate->GetWorld()->GetTerrain()->IsLoadingCompleted() == false) return false;
	}

	return true;
}