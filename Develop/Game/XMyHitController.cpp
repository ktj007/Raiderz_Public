#include "stdafx.h"
#include "XMyHitController.h"
#include "XTalentHitParam.h"
#include "CSMotionFactorHelper.h"

XMyHitController::XMyHitController()
{
	InitHitData();
}

void XMyHitController::InitHitData()
{
	nMFState		= MF_NONE;
	nMFWeight		= 0;
	nFlags			= 0;
}

void XMyHitController::SetHitData( XTalentHitParam* pHitInfo )
{
	nMFState		= pHitInfo->nMFState;
	nMFWeight		= pHitInfo->nMFWeight;
	nFlags			= pHitInfo->nFlags;
}

void XMyHitController::DelHitData()
{
	InitHitData();	
}

bool XMyHitController::CheckSendMovePost()
{
	if (!CSMotionFactorHelper::IsNoneOrFakeBeaten(nMFState))
	{
		return false;
	}

	return true;
}
