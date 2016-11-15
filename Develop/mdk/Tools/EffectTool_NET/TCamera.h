#pragma once

//#include "RMesh.h"
#include "RCameraSceneNode.h"
//#include "RCameraArcBallSceneNode.h"

using namespace rs3;

class TCamera : public RCameraSceneNode
{
public:
	TCamera();

	void SetPostEffect();
	~TCamera();

	//virtual void OnUpdate();

	void ResetSize();
	void UpdateData();
	void Reset(RVector vPos = RVector(0.f,0.f,90.f));

	//RVector GetPos();
	//RVector GetDir();
	RVector GetRight();

	//void	SetPos(RVector pos) { m_vPos = pos; }

	void Up(float f);
	void Down(float f);
	void Front(float f);
	void Back(float f);
	void Left(float f);
	void Right(float f);
	void CenterZoomIn(float f);
	void CenterZoomOut(float f);
	void CenterRotationLeft(float f);
	void CenterRotationRight(float f);
	void CenterRotationUp(float f);
	void CenterRotationDown(float f);

	bool OnMouseMove(int drag, RVector2 point, int delta);

	void OnPreRender();

public:

	float	m_fRotX;
	float	m_fRotY;
	float   m_fDist;

	//float	m_fFov;
	//float	m_fAspect;
	//float	m_fNear;
	//float	m_fFar;

	//RMatrix m_matView;
	//RMatrix m_matProj;

	//RVector	m_vPos;
	//RVector m_vDir;
	RVector m_vUp;

	RVector m_vTargetPos;

	RVector2	m_oldCursorPos;

};
