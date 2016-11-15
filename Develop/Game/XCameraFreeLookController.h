#pragma once

#include "XCameraSceneNodeController.h"

#include "XBaseCameraSceneNode.h"

class XCameraFreeLookController : public XCameraSceneNodeController
{
protected:
	float		m_fRotateSpeed;
	float		m_fMoveSpeed;
	float		m_fCameraX;
	float		m_fCameraZ;
	vec3		m_vDir;

	bool		m_bAutoRun;
	bool		m_bMaskAutoRunFlag;

	virtual void	UpdateCameraController(RMatrix& _rMatApplyToCamera) {}

	void SetRotateSpeed(float fSpeed)		{ m_fRotateSpeed = fSpeed; }
	void SetMoveSpeed(float fSpeed)			{ m_fMoveSpeed = fSpeed; }

	void _UpdateCamera(float fDelta);

public:
	XCameraFreeLookController(void);
	XCameraFreeLookController(RCAMERACONTROLLERTYPE eType, const char* _pSzCameraControllerID, bool _bManagedByCamera);
	virtual ~XCameraFreeLookController(void);

	void UpdateCamera( float fDelta, int nDeltaX, int nDeltaY );
	virtual void Init();
};
