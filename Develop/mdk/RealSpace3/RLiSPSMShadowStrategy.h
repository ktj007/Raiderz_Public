#pragma once

#include "RDepthMapShadowStrategy.h"

namespace rs3 {

class RS_API RLispsmShadowStrategy : public RDepthMapShadowStrategy
{
public:
	RLispsmShadowStrategy(const RShadowDesc& desc);

	virtual ~RLispsmShadowStrategy();

	virtual void Init();
	virtual void Destroy();

	virtual void Render(RCameraSceneNode *pViewCamera, RWorldSceneNode *pWorld) override;

	virtual const RMatrix GetCurrentLightTransform();
	virtual const RMatrix GetCurrentLightTransform(RCameraSceneNode& LightCamera);
	virtual const RMatrix GetCurrentLightViewTransform();	
	virtual const RCameraSceneNode* GetCurrentDepthCamera();
	virtual RBoundingBox GetCurrentLightFrustumAABB();

protected:
	RCameraSceneNode*	m_pShadowCamera;
	RBoundingBox		m_cameraFrustumAABB;
};

class RS_API RExponentialShadowStrategy : public RLispsmShadowStrategy
{
public:
	RExponentialShadowStrategy(const RShadowDesc& desc);

	virtual void Init();
};

}