#pragma once

#include "RSpotLightSceneNode.h"
#include "RShaderComposite.h"

namespace rs3 {

class RLightingShader;

#define MAX_INSTANCE 10

//#define USE_VERTEXBUFFER

struct SL_INSTANCE_DATA
{
	RVector3 vPosition[MAX_INSTANCE];
	RVector3 vPositionInWorld[MAX_INSTANCE];
	RVector3 vLightInfo[MAX_INSTANCE];	//x: Far, y: AttenuationStart, z: Visibility
	RVector3 vColor[MAX_INSTANCE];
	RVector4 vLightDirectionFOV[MAX_INSTANCE];	//xyz: Light Direction, w:FOV
	RViewFrustum ViewFrustum[MAX_INSTANCE];

	bool bCameraInLight[MAX_INSTANCE];
};

class RS_API RSpotLightPassRenderer : public RPassRenderer
{
public:
	RSpotLightPassRenderer(void);
	virtual ~RSpotLightPassRenderer(void);

	DECLARE_PASS_RENDERER_ID( RRP_SPOTLIGHT );

	void RenderSpotLightingToLBuffer( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RLightingShader* pLightingShader, RRENDERING_MODE eRRM);

private:
	void CategorizeLight(RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, 
		RSceneNodeList* outInstancing, RSceneNodeList* outNoInstancing,
		RSceneNodeList* outInstancingWithCamera, RSceneNodeList* outNoInstancingWithCamera);

	bool CheckInstancing(RSpotLightSceneNode* pSpotLight);
	void SetInstanceData(RCameraSceneNode* pCurrentCamera, RSpotLightSceneNode* pSpotLightSceneNode, int nIndex);

	void RenderLight(RCameraSceneNode* pCurrentCamera, RSceneNodeList* pLightList, RLightingShader* pLightingShader, RRENDERING_MODE eRRM);
	void RenderLightOnebyOne(RCameraSceneNode* pCurrentCamera, RSceneNodeList* pLightList, RLightingShader* pLightingShader, RRENDERING_MODE eRRM);
	void RenderInstancing(int nCount, RLightingShader* pLightingShader, RRENDERING_MODE eRRM);
	void RenderSingleSpotLight(RCameraSceneNode* pCurrentCamera, RSpotLightSceneNode* pSpotLight, RLightingShader* pLightingShader, RRENDERING_MODE eRRM);

	SL_INSTANCE_DATA m_InstanceData;

 #ifdef USE_VERTEXBUFFER
	HVERTEXFORMAT m_VertexFormat;
	HVERTEXBUFFER m_VertexBuffer;
	HINDEXBUFFER m_IndexBuffer;
#else
	RVector4 m_VertexBuffer[5*MAX_INSTANCE];
	WORD m_IndexBuffer[18*MAX_INSTANCE];
#endif
};

}
