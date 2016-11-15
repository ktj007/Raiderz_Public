#pragma once

#include "RPassRenderer.h"

namespace rs3 {

class RSkyPassRenderer : public RPassRenderer
{
public:
	RSkyPassRenderer();
	virtual ~RSkyPassRenderer();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pass Renderer ID ¼³Á¤
	//
	DECLARE_PASS_RENDERER_ID( RRP_SKYBOX );

	virtual void RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;
	virtual void RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;

private:
	class RSkyRenderStrategy
	{
	public: virtual void Render( RSceneNode* pNode, RRENDERINGMETHOD renderMethod, RCameraSceneNode* pCamera ) const = 0;
	};
	class RSkyGStageRenderStrategy : public RSkyRenderStrategy
	{
	public: virtual void Render( RSceneNode* pNode, RRENDERINGMETHOD renderMethod, RCameraSceneNode* pCamera ) const override;
	};
	class RSkyMStageRenderStrategy : public RSkyRenderStrategy
	{
	public: virtual void Render( RSceneNode* pNode, RRENDERINGMETHOD renderMethod, RCameraSceneNode* pCamera ) const override;
	};

	void Render( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod, const RSkyRenderStrategy& renderStrategy );

	RSkyGStageRenderStrategy m_GStageRenderStrategy;
	RSkyMStageRenderStrategy m_MStageRenderStrategy;
};

}