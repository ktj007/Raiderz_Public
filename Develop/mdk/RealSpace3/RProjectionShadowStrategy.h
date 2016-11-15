#pragma once

#include "RDepthMapShadowStrategy.h"

namespace rs3 {

class RProjectionShadowStrategy : public RDepthMapShadowStrategy
{
public:
	RProjectionShadowStrategy(const RShadowDesc& desc);
	virtual ~RProjectionShadowStrategy(void);

	virtual void Init();
	virtual void Destroy();

	//Lost device 발생시 ShadowMap은 다시 생성해주어야하기 때문에 별도로 생성/삭제한다.
	void CreateShadowMap();
	void DestroyShadowMap();

	virtual void Render(RCameraSceneNode *pViewCamera, RWorldSceneNode *pWorld) override;

	virtual const RMatrix GetCurrentLightTransform();
	virtual const RMatrix GetCurrentLightTransform(RCameraSceneNode& LightCamera);
	virtual const RMatrix GetCurrentLightViewTransform() { return m_pLightCamera.GetViewMatrix(); }
	virtual RSCID GetDepthWriteComponent();
	virtual const RCameraSceneNode* GetCurrentDepthCamera() { return &m_pLightCamera; }


	void RenderShadowBuffer(RCameraSceneNode* pCamera, RLightingShader* pLightingShader);

	virtual RTexture* GetCurrentDepthMap() { return m_pShadowMap; }

protected:
	virtual void SetandClearRenderTarget(RCameraSceneNode* pCamera, RTexture* pRenderTarget);

private:
	RListener*		m_pEventListener;			///< OnRestoreDevice / OnLostDevice 이벤트 수신객체

	RCameraSceneNode	m_pLightCamera;

	RTexture* m_pShadowMap;
};

}