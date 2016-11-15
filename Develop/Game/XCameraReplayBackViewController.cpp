#include "StdAfx.h"
#include "XCameraReplayBackViewController.h"
#include "XModuleEntity.h"

#include "XCameraControllerGroup.h"

XCameraReplayBackViewController::XCameraReplayBackViewController(void)
: XCameraBackViewController( RCCT_PREUPDATE, XCameraTypeStr[CAMERA_REPLAY_BACKVIEW], true )
{
}

XCameraReplayBackViewController::~XCameraReplayBackViewController(void)
{
}
