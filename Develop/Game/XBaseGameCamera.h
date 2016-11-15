#pragma once

#include "XGlobal.h"
#include "XMoveCollision.h"
#include "RCameraSceneNode.h"

using namespace rs3;

class XBaseGameCamera
{
protected:

	XMoveCollision	m_Collision;								// 카메라 충돌 검사용

	// 카메라 스무스 이동
	vec3			m_vOldPos;
	vec3			m_vOldDir;
	float			m_fCameraSmoothMoveTime;
	bool			m_bSmoothMove;
	bool			m_bColTerrain;

protected:
	void			SetCollisionRadius( float fNearZ, float fFoveX, float fFoveY );
	bool			CheckTerrainCollision(vec3& eye, vec3& tar, vec3& outPos);							// 지형 충돌 검사
	void			UpdateCameraSmoothMove(vec3& eye, vec3& tar, vec3& dir, vec3& outPos, vec3& outDir, float fDelta, bool bColTerrain); // 카메라 이동을 완만하게
	float			BezierCurve(float fStart, float fP1, float fP2, float fEnd, float fRatio);
	float			GetCameraRadiusNearZ(float fNearZ, float fFoveX, float fFoveY);

public:
	XBaseGameCamera(void);
	virtual ~XBaseGameCamera(void);
};


