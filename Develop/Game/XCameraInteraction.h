#pragma once

#include "XCameraSceneNodeController.h"
#include "XBaseGameCamera.h"

#include "XBaseCameraSceneNode.h"

class XCameraInteraction
{
	enum CAMERA_POSITION_TYPE
	{
		CPT_LEFT,
		CPT_RIGHT
	};

private:
	vec3					m_vStartLocalPos;
	vec3					m_vStartDir;

	vec3					m_vTargetLocalPos;
	vec3					m_vTargetDir;

	vec3					m_vCurrentPos;
	vec3					m_vCurrentDir;

	bool					m_bActive;
	bool					m_bStart;
	bool					m_bEnd;
	bool					m_bDone;

	float					m_fElapsedTime;
	float					m_fMoveTime;

private:
	CAMERA_POSITION_TYPE	GetPositionType(vec3 vOwnerDir);

protected:
	virtual void			UpdateCameraController(RMatrix& _rMatApplyToCamera) {}

public:
	XCameraInteraction(void);
	virtual ~XCameraInteraction(void);

	void					UpdateCamera(float fDelta, vec3 vOwnerPos, vec3& vPosition, vec3& vDirection);

	virtual void			Init(RCameraSceneNode* pCameraSceneNode, vec3 vOwnerPos);
	
	void					Start(MUID uidTarget, vec3 vOwnerPos, RCameraSceneNode* pCameraSceneNode);
	void					End(vec3 vOwnerPos);
	void					ForceEnd();

	bool					GetDone() { return m_bDone; }
	bool					IsActive() { return m_bActive; }
};
