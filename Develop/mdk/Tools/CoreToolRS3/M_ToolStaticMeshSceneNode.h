#pragma once

#include "RStaticMeshSceneNode.h"
#include "RStaticMeshRenderStrategy.h"

namespace rs3 {

class CORETOOL_API RToolStaticMeshSceneNode : public RStaticMeshSceneNode
{
protected:

public:
	RToolStaticMeshSceneNode();
	~RToolStaticMeshSceneNode();

	bool IsRenderDebug() { return m_bRenderDebug; }
	void SetRenderDebug( bool bRenderDebug ) { m_bRenderDebug = bRenderDebug; }

	bool IsRenderAll() { return m_bRenderAll; }
	void SetRenderAll( bool bRenderAll ) { m_bRenderAll = bRenderAll; }

	void RenderRawAll(  RCameraSceneNode* pCurrentCamera);

protected:
	virtual void OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMethod = RENDERING_NULL) override;

	virtual void Cull(RCameraSceneNode* pCamera,const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar) override;

private:
	void CullNone();
	void RenderDebug();

	bool m_bRenderDebug;
	bool m_bRenderAll;
};

}