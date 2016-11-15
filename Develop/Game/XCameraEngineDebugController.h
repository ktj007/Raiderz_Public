#pragma once

#include "XCameraFreeLookController.h"

class XCameraEngineDebugController : public XCameraFreeLookController
{
protected:
	//virtual void			UpdateCameraController(RMatrix& _rMatApplyToCamera);

public:
	XCameraEngineDebugController(void);
	virtual ~XCameraEngineDebugController(void);
};
