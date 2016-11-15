#pragma once
#include "RCameraSceneNodeController.h"

#include "XBaseCameraSceneNode.h"

class XCameraSceneNodeController : public RCameraSceneNodeController
{
protected:
	MUID			m_uidObject;

	virtual void UpdateSceneNode();
public:
	XCameraSceneNodeController(RCAMERACONTROLLERTYPE eType = RCCT_PREUPDATE, const char* _pSzCameraControllerID = NULL, bool _bManagedByCamera = true);
	virtual ~XCameraSceneNodeController(void);

	XBaseCameraSceneNode*	GetSceneNode()	{ return (XBaseCameraSceneNode*)m_pCameraSceneNode; }
	XPolarCoord* GetPolarCoord() { return GetSceneNode()->GetPolarCoord(); }

	virtual void Init()	{}
};
