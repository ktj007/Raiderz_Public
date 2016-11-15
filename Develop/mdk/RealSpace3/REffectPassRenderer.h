#pragma once

#include "RPassRenderer.h"

namespace rs3 {

class RParticleShader;

class REffectPassRenderer : public RPassRenderer
{
public:

	REffectPassRenderer();
	virtual ~REffectPassRenderer(void);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pass Renderer ID ¼³Á¤
	//
	DECLARE_PASS_RENDERER_ID( RRP_PARTICLE );

	virtual void Init() override;
	virtual void Destroy() override;

	virtual void RenderOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD eMothod) override;
	virtual void RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;

	void SetSort( bool b)	{ m_bSort = b;}

	RParticleShader* GetParticleShader() { return m_pParticleShader; }

protected:
	bool IsRenderable( const RSceneNode* pNode );

	void SetRenderState( RCameraSceneNode* pCamera );
	void RestoreRenderState();

	bool m_bSort;
	RParticleShader* m_pParticleShader;
};

}