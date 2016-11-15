#pragma once

#include "RPassRenderer.h"
#include "RMesh.h"
#include "RMeshNode.h"
#include "RActorNode.h"

namespace rs3 {

class RShaderFX;
class RDevice;
class RTechnique;
class RMaterial;
class REnvironmentSet;
class RPostEffectDistortion;
class RActorNodeRenderStrategyShadowMap;

enum ACTOR_TECH_TYPE {
	ACTORTECH_GSTAGE_IN = 0,
	ACTORTECH_MSTAGE_IN,
	ACTORTECH_GSTAGE_DE,
	ACTORTECH_MSTAGE_DE,
	ACTORTECH_SHADOW,
	ACTORTECH_COUNT
};

// ACTOR_TECH_TYPE와 짝이 맞아야..
const RRENDERINGMETHOD renderMethodByTech[] = {
	RENDERING_INFERRED,
	RENDERING_INFERRED,
	RENDERING_DEFERRED,
	RENDERING_DEFERRED,
	RENDERING_FORWARD,
	RENDERING_NULL,
	RENDERING_END
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// 액터 패스렌더러
//
//  1. RActor::GetUsingPass(virtual) 에서 m_bUsingActorPassRenderer 플래그를 통해
//     알맞는 RenderCandidate를 만든다.( RRP_ACTOR / RRP_NORMAL )
//      - 일반적인 렌러딩을 쓴다면, OnPreRender / OnRender / OnPostRender 가 호출되고,
//        OnPreRender에서 UpdateForRender(예전의 OnPreRender), OnRender에서 RenderSingle이 호출된다.
//
//
//  3. RActor::RenderSingle은 디테일을 설정할수 있으며,
//     모든 액터에 적용가능한 SetGlobalActorRenderState과 UpdateGlobalActorTransform를 하고
//     액터노드의 RenderSingle을 호출한다.
//
//  4. RActorNode::RenderSingle은 메시노드로 부터 얻은 버텍스를 가지고 버텍스 설정을 하고
//     메시노드의 얻은 마테리얼을 가지고 마테리얼 설정을 한다.
//      - 마테리얼의 설정의 책임은 RActorPassRender가 가진다.
//        (예전에는 책임을 RActorNode::Render가 가지고 있었고, BeginMaterial을 통해 세팅을 했었다. )
//
//  5. RActorNode의 쉐이더 이팩트와 테크닉은 RActorNode가 생성되는 시점에서 액터로부터 얻어
//     마테리얼에 따라 인덱싱을 해 가지고 있고, RenderSingle 및 RActorPassRenderer::PassOnRender에서 사용한다.
//
//  6. 환경 타일링 이펙트의 경우, 필요한 디바이스 설정의 책임은 RActorPassRenderer가 가진다.
//
//  7. RActorPassRenderer를 사용하지 않는 액터
//      - REffectActor(타일링, 라이트팩터 등이 안먹어야 하는 액터) / RSceneSky / RWaterActor 는 각자가 렌더링 책임을 가진다.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

class RS_API RActorPassRenderer : public RPassRenderer
{
public:
	RActorPassRenderer();
	virtual ~RActorPassRenderer(){}


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pass Renderer ID 설정
	//
	DECLARE_PASS_RENDERER_ID( RRP_ACTOR );


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//
	virtual void		Init();
	virtual void		Destroy();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// update / rendering
	//
	virtual bool		IsCurrentShaderRejective(){ return true; }
	virtual void		RenderDepthOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RShadowStrategy* pShadowStrategy ) override;
	void				RenderDoubleShadowDepthOnPass( RCameraSceneNode* pCurrentCamera, RShadowStrategy* pShadowStrategy );

	virtual void		RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;
	virtual void		RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;
	
	template<class KEY_TYPE>
	void				RenderOnPass(RCameraSceneNode* pCurrentCamera, RActorNodeRenderStrategy* pActorRenderStrategy, RRenderChain<KEY_TYPE>* pActorNodeRenderChain);

	/// 인스턴싱 랜더 처리
	virtual void		RenderInstancingNode( RActorNode* pActorNode, RCameraSceneNode* pCurrentCamera, RActorNodeRenderStrategy* pActorRenderStrategy, RTechnique& currentTechnique, int& nInstanceCnt, RActorNode** pInstancingActorNode, int index);
	virtual void		RenderNode(RCameraSceneNode* pCamera, RActorNodeRenderStrategy* pActorRenderStrategy, RActorNode* pActorNode, int nInstanceCnt, RTechnique& currentTechnique, int index, bool bAbleInstancing);

	virtual void		SetActorNodeInstancingData(int nIdx, RActorNode* pNode);
	virtual void		SetInstancingConstant(int nCnt);
	virtual bool		BeginTechnique(RTechnique& currentTechnique, RShaderFX* pShader);
	virtual bool		EndTechnique( RTechnique& currentTechnique);
	virtual void		SetupForRenderInstancing( RActorNodeInstancingBuffer* pInstancingBuffer, UINT nCnt);
	virtual void		RestoreForRenderInstancing();

	inline void			EnableRender()	{ m_bRender = true; }
	inline void			DisableRender()	{ m_bRender = false;}
	inline bool			IsRenderEnabled(){ return m_bRender; }


protected:
	virtual void		UpdateShaderIDForMStage(RSCID& rscComponents, bool bInstancing, bool bNoiseRef, bool bBlendingMaterial, bool bMultiplyColor) {}
	virtual void		AddShaderEffectShaderIDForGStage(RActorNode* pActorNode, RSCID& rscComponents);
	virtual void		AddShaderEffectShaderIDForMStage(RActorNode* pActorNode, RSCID& rscComponents);
	void				RenderTypeOnPass(RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, bool bUseBlending, RRENDERPASS _passType);

protected:

	bool CheckCastShadow(RActor* pActor, RShadowStrategy* pShadowStrategy, RCameraSceneNode* pCurrentCamera);

	static RActorNodeInstancingBuffer			m_InstancingBuffer;
	static RMatrix*								m_pInstancingData;

	bool				m_bRender;

private:
	void		RenderShadowMap(RCameraSceneNode* pCurrentCamera, RShadowStrategy* pShadowStrategy, bool bDoubleShadowPass );
};

}
