#pragma once

#include "RPassRenderer.h"
#include "RPointLightSceneNode.h"

namespace rs3 {

class RLightingShader;
class RRenderingStrategy;

struct PL_INSTANCE_DATA
{
	RVector3 vPosition;
	RVector3 vLightInfo;	//x: radius, y: AttenuationStart, z: Visibility

	RVector4 vAxisX;
	RVector4 vAxisY;
	RVector4 vAxisZ;

	DWORD dwColor;
};

class RS_API RPointLightPassRenderer : public RPassRenderer
{
public:
	RPointLightPassRenderer(void);
	virtual ~RPointLightPassRenderer(void);

	DECLARE_PASS_RENDERER_ID( RRP_LIGHT );

	void RenderPointLightingToLBuffer(RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RLightingShader* pLightingShader, RRENDERING_MODE eRRM);

	
private:
	void CreateShader();
	void CategorizeLight(RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, 
							RSceneNodeList* outPoint, RSceneNodeList* outPointWithCamera, RSceneNodeList* outArea, RSceneNodeList* outAreaWithCamera);
	bool GetInstanceData(RSceneNode* pSceneNode, PL_INSTANCE_DATA& outInstanceData);

	int UpdateInstanceVertexBuffer(RCameraSceneNode* pCamera, RSceneNodeList* pSceneNodeList);

	void RenderLight(RCameraSceneNode* pCurrentCamera, RSceneNodeList* pLightList, RShaderFX* pShader);
	

	RVector3 m_GeometryData[8];
	WORD m_IndexData[36];

	PL_INSTANCE_DATA* m_InstanceData;

	HVERTEXBUFFER m_GeometryBuffer;
	HVERTEXBUFFER m_InstanceBuffer;
	
	HINDEXBUFFER m_GeometryIndexBuffer;

	HVERTEXFORMAT m_VertexFormat;

	int m_nMaxLightCount;
	int m_nOffsetForWrite;
	int m_nOffsetForRender;
};

}
