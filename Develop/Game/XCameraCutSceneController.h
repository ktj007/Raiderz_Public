#pragma once

#include "XBaseGameCamera.h"
#include "XCameraSceneNodeController.h"

class XCameraCutSceneController : public XBaseGameCamera, public XCameraSceneNodeController, public RListener
{
protected:
	virtual void			UpdateCameraController(RMatrix& _rMatApplyToCamera);

public:
	XCameraCutSceneController(void);
	virtual ~XCameraCutSceneController(void);

	void				Init(vec3& vPos, vec3& vDir);
};
