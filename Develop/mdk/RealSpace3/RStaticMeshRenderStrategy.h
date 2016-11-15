#pragma once

namespace rs3 {

struct RenderInfo;
class RStaticMeshSceneNode;
class RTechnique;
class RShaderCompositeManager;

class RStaticMeshRenderStrategy
{
public:
	RStaticMeshRenderStrategy( RStaticMeshSceneNode* pStaticMesh );
	virtual ~RStaticMeshRenderStrategy();

	virtual bool CheckAndSetTechnique(  RCameraSceneNode* pCurrentCamera, RenderInfo* pRenderInfo, int nLightMapID, bool bFogEnable );

	virtual void BeginTechnique();
	virtual void EndTechnique();

protected:
	RStaticMeshSceneNode*		m_pStaticMesh;
	RTechnique*					m_pTechnique;
	RTechnique*					m_pCurrentTechnique;
	RShaderCompositeManager*	m_pShaderCompositeManager;
};

}