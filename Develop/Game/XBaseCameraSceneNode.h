#pragma once
#include "RCameraSceneNode.h"
#include "XBaseGameCamera.h"

// 극좌표계
struct XPolarCoord
{
	float fRadius;
	float fTheta;		// z와 xy평면 사이의 각, 상하
	float fPhi;			// x축에서 y축으로의 각, 좌우
};

class XBaseCameraSceneNode : public rs3::RCameraSceneNode
{
protected:
	XPolarCoord		m_PolarCoord;
	MUID			m_uidTarget;

	virtual void				OnUpdate();

public:
	XBaseCameraSceneNode(void);
	virtual ~XBaseCameraSceneNode(void);

	XPolarCoord*	GetPolarCoord() { return &m_PolarCoord; }
	void			SetPolarCoord( XPolarCoord* pCoord );

	virtual bool	GetTargetPosition( vec3& outPos );
	void			SetTargetMUID(MUID uid);

	const float		GetDist();
};
