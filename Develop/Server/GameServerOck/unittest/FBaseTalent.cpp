#include "stdafx.h"
#include "FBaseTalent.h"
#include "GUTHelper_Talent.h"

GTalentInfo* FBaseTalent::NewTalentInfo( int nID/*=INVALID_ID*/, int nTalentLine/*=0*/, int8 nRank/*=0*/ )
{
	GUTHelper_Talent helper;
	return helper.NewTalentInfo(nID, nTalentLine, nRank);
}