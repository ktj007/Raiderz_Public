#include "stdafx.h"
#include "GMotionFactorHelper.h"
#include "CSMotionFactorInfo.h"

float GMotionFactorHelper::CalcReleaseTime( MF_STATE nMF, int nWeight, float fStandupTime, ENTITY_TYPEID nEntityTypeID )
{
	float fMFReleaseTime = 0.0f;

	switch (nMF)
	{
	case MF_BEATEN:
	case MF_STUN:
	case MF_DEFLECT:	
	case MF_GRAPPLED:
	case MF_SWALLOWED:
		{
			fMFReleaseTime = (nWeight / 1000.0f);
		}
		break;
	case MF_KNOCKDOWN:
	case MF_THROWUP:
		{			
			fMFReleaseTime = fStandupTime + (nWeight / 1000.0f);
		}
		break;
	case MF_UPPERED:
	case MF_KNOCKBACK:
		{			
			fMFReleaseTime = (nWeight / 100.0f) * KNOCKBACK_TIME;
		}
		break;
	case MF_DRAG:
		{			
			fMFReleaseTime = ((nWeight / 100.0f) * DRAG_TIME ) + 1.0f;		// 일단 Knockback보다 1초 더 가만히 있음
		}
		break;
	}


	// 강제 보정치 추가
	if ( nEntityTypeID == ETID_NPC)
	{
		fMFReleaseTime += FORCED_RELEASE_TIME_DELAY;
	}

	return fMFReleaseTime;

}