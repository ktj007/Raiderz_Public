#pragma once

#include "RPassRenderer.h"

namespace rs3 {

class RLightingShader;

class RS_API RDirectionalLightPassRenderer : public RPassRenderer
{
public:
	RDirectionalLightPassRenderer(void);
	virtual ~RDirectionalLightPassRenderer(void);

	DECLARE_PASS_RENDERER_ID( RRP_DIRLIGHT );

	void RenderLightingToLBuffer( RCameraSceneNode* pCamera, SCENE_SET* pCulledList, RLightingShader* pLightingShader, DWORD dwWidth, DWORD dwHeight, RGIProcessor* pGIProcessor, RRENDERING_MODE eRRm);

private:
	void SetDirectionalLightShaderConstant( RCameraSceneNode* pCamera, RLightingShader* pLightingShader, RDirectionalLightSceneNode* pDirLight );
	int SetSubLightShaderConstant(RCameraSceneNode* pCamera, RLightingShader* pLightingShader, SCENE_SET* pSubLightList);
	void SetShadowRenderState( RCameraSceneNode* pCamera, RLightingShader* pLightingShader );
	void SetCloudRenderState( RCameraSceneNode* pCamera, RLightingShader* pLightingShader );

	void RenderSubLightOnly(RCameraSceneNode* pCamera, RLightingShader* pLightingShader, SCENE_SET* pSubLightList, DWORD dwWidth, DWORD dwHeight);

	void SetRenderState(bool bDirLightOnlyGI, bool bShadowOnlyGI, RGIProcessor* pGIProcessor);
	void RestoreRenderState();

	void ExportShadow(RGIProcessor* pGIProcessor, RLightingShader* pLightingShader, RDirectionalLightSceneNode* pDirLight, DWORD dwWidth, DWORD dwHeight);
};

}
