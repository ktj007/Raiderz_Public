#include "stdafx.h"
#include "XGlobalVar.h"

GlobalVar_Debug::GlobalVar_Debug()
{
	bShowAABB	=	false;
	bShowCollision=false;
	bShowHitCapsule = false;
	bShowHitEffectCapsule = false;
	bShowSector=false;
	bShowSector2=false;
	bShowSensor=false;
	
	nTestMF = MF_BEATEN;
	nTestMFWeight = 1000;
	bWarpToPickingPos = false;

	bRenderMoveDebug = true;

	bAutoReportCrashDump = false;

	bShowMoveCollision_Real = false;
	bShowMoveCollision_Poly = false;
	bShowmovecollision_Down = false;
	bShowMoveCollision_View = false;
	bShowMoveCollision = false;
	bShowmovecollision_Investigate = false;
	bShowObjectCollision_Investigate = false;
	bShowMagicArea = false;
	bShowArea = false;
	bShowID	= false;
	bShowNpcMoveInfo = false;
#ifdef _DEBUG
	bShowID	= true;
#endif
	bShowSoundRange = false;

	nShowMoveCollision_InvestigateIndex = 0;
	nShowObjectCollision_InvestigateIndex = 0;

	bCameraLock = false;
	uidCameraLock.SetZero();
	strCameraLockBoneName.clear();
	fTestCameraLockRightValue = 43.0f;
	fTestCameraLockBackValue = 104.0f;
	fTestCameraLockHeightValue = 26.0f;

	bForceGuard = false;
}
