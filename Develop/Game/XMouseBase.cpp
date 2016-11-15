#include "stdafx.h"
#include "XMouseBase.h"
#include "XModuleCollision.h"
#include "XCameraManager.h"
#include "RUtil.h"

bool XMouseBase::GetMouseLine(const LONG sx, const LONG sy, RVector& origin, RVector& to ) const
{
	XBaseCameraSceneNode* pCameraNode = global.camera->GetCamera();
	if(pCameraNode == NULL) return false;

	RECT rt				= global.ui->GetScreenRect();

	RVector vCamPosP, vMouseDir, vOnMouseRayQ;
	vCamPosP = pCameraNode->GetPosition();

	//vMouseDir ±¸ÇÏ±â
	RGetScreenLine(sx, sy, rt,
		pCameraNode->GetViewMatrix(),
		pCameraNode->GetProjectionMatrix(),
		&vMouseDir);

	const RPlane *pPlane = &pCameraNode->GetViewFrustum().GetPlanes()[RV_FAR];
	if ( !MMath::IsIntersectionPlane(vOnMouseRayQ, vCamPosP, vMouseDir, *pPlane)) //vOnMouseRayQ
		return false;

	to = vOnMouseRayQ;
	origin = vCamPosP;

	return true;
}

bool XMouseBase::GetTrans3DCoordinates( int x, int y, vec3 &vPos, vec3 &vDir )
{
	RMatrix matView = global.device->GetTransform(RST_VIEW);
	RMatrix matProj = global.device->GetTransform(RST_PROJECTION);

	vec3 origin;
	vec3 to;
	if (GetMouseLine(x, y, origin, to) == false) 
	{
		return false;
	}

	vPos = origin;
	vDir = to - origin;
	vDir.Normalize();

	return true;
}
