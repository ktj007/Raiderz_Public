#pragma once

#include "RCameraSceneNode.h"
using namespace rs3;

class XCameraHelper
{
public:
	static void ResetFov(RCameraSceneNode* pCameraSceneNode);
	static void SetDOFEnableAndParams( RSceneManager* pCameraSceneNode, bool bEnable);
};