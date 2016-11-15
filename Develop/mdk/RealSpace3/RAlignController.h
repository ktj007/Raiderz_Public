#pragma once

// 특정 scenenode 에 target을 가진 회전값을 주기 위해 만들어진 controller.

#include "RSceneNodeController.h"

namespace rs3 {

class RS_API RAlignController : public RSceneNodeController
{
public:
	DECLARE_NODE_ID_STRING("AlignController");

	RAlignController(void);
	virtual ~RAlignController(void);

	virtual void Update(RCameraSceneNode* pCamera) override;

};

}