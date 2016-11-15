#include "StdAfx.h"
#include "GTestForward.h"

GTestHelpers test;

//////////////////////////////////////////////////////////////////////////
//
// Util Functions
//
//////////////////////////////////////////////////////////////////////////

void MakeLevelFactor(int nLF, GEntityActor* pRequester, GEntityActor* pTarget)
{
	VALID(nLF>=0);
	int nLevel=pTarget->GetLevel();

	int nRelationLevel=-1;
	if (nLF==5)			
		nRelationLevel = nLevel;
	else if (nLF < 5)	
		nRelationLevel = nLevel + 5-nLF+1;
	else if (nLF > 5)	
		nRelationLevel = nLevel + 5-nLF-1;
	else	
		assert(false);

	pRequester->SetLevel(nRelationLevel);
}