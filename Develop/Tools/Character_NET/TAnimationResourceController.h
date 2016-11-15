#pragma once
#include "RAnimationResource.h"

class TAnimationResourceController : public RAnimationResource
{
public:
	TAnimationResourceController(RSceneManager* pSceneManager);
	virtual ~TAnimationResourceController() {}

	int						GetAnimationMaxFrame(string strAniFileName);
};