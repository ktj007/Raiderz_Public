#include "StdAfx.h"
#include "XCameraEngineDebugController.h"

#include "XCameraControllerGroup.h"

XCameraEngineDebugController::XCameraEngineDebugController(void)
: XCameraFreeLookController( RCCT_PREUPDATE, XCameraTypeStr[CAMERA_ENGINE_DEBUG], true )
{
}

XCameraEngineDebugController::~XCameraEngineDebugController(void)
{
}
