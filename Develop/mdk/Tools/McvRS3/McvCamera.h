#ifndef _MCV_CAMERA_H 
#define _MCV_CAMERA_H 

#include "RCameraSceneNode.h"

using namespace rs3;

class CTCamera : public RCameraSceneNode {
public:
	CTCamera();
	~CTCamera();

	void UpdateData();
	void Reset();
	void Reset(RBox boxModel);
	void ResetSize();

	RVector GetPos();
	RVector GetDir();
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

public:

	float	m_fRotX;
	float	m_fRotY;
	float   m_fDist;

//	float	m_fFov;
//	float	m_fAspect;
//	float	m_fNearZ;
//	float	m_fFarZ;

//	RMatrix	m_matView;
//	RMatrix m_matProj;

//	RVector	m_position;
//	RVector m_direction;
//	RVector m_vUp;

	RVector m_vTargetPos;

	virtual void OnPreRender();
};


#endif//_MCV_CAMERA_H