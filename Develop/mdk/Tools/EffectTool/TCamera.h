#pragma once

#include "RCameraSceneNode.h"

using namespace rs3;

namespace EffectTool
{

class TCamera : public RCameraSceneNode
{
public:
	TCamera();
	~TCamera();

	void ResetSize();
	void UpdateData();
	void Reset(RVector vPos = RVector(0.f,0.f,90.f));

	RVector GetRight();

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

	RVector m_vUp;

	RVector m_vTargetPos;

	RVector2 m_oldCursorPos;
};

}