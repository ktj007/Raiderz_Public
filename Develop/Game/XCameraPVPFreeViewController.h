#pragma once

#include "XCameraFreeLookController.h"
#include "XMoveCollision.h"
#include "XBaseCameraSceneNode.h"

class XCameraPVPFreeViewController : public XCameraFreeLookController
{
	enum FREE_VIEW_STATE
	{
		FVS_NONE,
		FVS_START,
		FVS_FREE,
		FVS_END,
	};

private:
	FREE_VIEW_STATE					m_eState;
	XMoveCollision					m_moveCollision;

private:
	void							UpdateStartEndPVPFreeViewCamera(float fDelta);

protected:
	virtual void					UpdateCameraController(RMatrix& _rMatApplyToCamera) {}

public:
	XCameraPVPFreeViewController(void);
	XCameraPVPFreeViewController(RCAMERACONTROLLERTYPE eType, const char* _pSzCameraControllerID, bool _bManagedByCamera);
	virtual ~XCameraPVPFreeViewController(void);

	void							UpdateCamera( float fDelta, int nDeltaX, int nDeltaY );
	virtual void					Init();

	void							Start(vec3 vOwnerPos);
	void							End();
};
