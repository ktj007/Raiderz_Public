#pragma once
#include "XCameraSceneNodeController.h"

#include "XBaseCameraSceneNode.h"

class XBaseCameraSceneNode;

class XCameraGroundTarget
{
private:
	XPolarCoord		m_BackViewPolarCoord;


	bool			m_bGroundTarget;
	bool			m_bGroundTargetMove;

	float			m_fTime;
	float			m_fBaseTime;

	float			m_fStartDis;
	float			m_fEndDis;
	float			m_fStartTheta;
	float			m_fEndTheta;

private:

protected:

public:
	XCameraGroundTarget(void);
	virtual ~XCameraGroundTarget(void);

	void			Start(XPolarCoord* pCur);
	void			End(XPolarCoord* pCur);
	void			StartEndUpdate(float fDelta, XPolarCoord* pCur);
	void			CalcPosDirForGroundTarget(XPolarCoord* pCur, vec3 vTar, vec3& vPosition, vec3& vDirection);

	bool			IsGroundTargetActive();
	bool			IsGroundTargetState()	{ return (m_bGroundTarget || m_bGroundTargetMove); }
};
