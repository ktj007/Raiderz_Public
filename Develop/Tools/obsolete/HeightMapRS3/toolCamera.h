#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <RCameraSceneNode.h>

// 결국 RCameraSceneNode를 상속받음 .

class I3DCamera : public rs3::RCameraSceneNode
{
public:
	I3DCamera() {}
	virtual ~I3DCamera() {}

	virtual void Reset() {}
	virtual void UpdateTarget(rs3::RVector &vAt) {}
	virtual void Zoom(float n) {}
	virtual void RotationH(float n) {}
	virtual void RotationV(float n) {}
	virtual void MoveAtPoint(const rs3::RVector &vec) {}

	const rs3::RMatrix& GetProjectionMatrix() const
	{
		return m_projMatrix;
	}

};

class L3DCamera : public I3DCamera
{
	MDeclareRTTI;
public:
	rs3::RVector m_defEye;
	rs3::RVector m_defAt;
	
	rs3::RVector m_at;
	float m_fDist;
public:
	L3DCamera();
	virtual ~L3DCamera();
	void Reset();
	
	void UpdateTarget(rs3::RVector &vAt);
	void Zoom(float n);
	
	void RotationH(float n);
	void RotationV(float n);

	void MoveAtPoint(const rs3::RVector &vec);
};

class L3DFPSCamera : public I3DCamera
{
	MDeclareRTTI;
public:
	L3DFPSCamera()
	{
		Reset();
	}
	virtual ~L3DFPSCamera()
	{}

	void Reset();
	void RotationH(float n);
	void RotationV(float n);
};