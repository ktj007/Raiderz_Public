#pragma once

#include "RPassRenderer.h"
#include "RMesh.h"
#include "RMeshNode.h"
#include "RActorNode.h"
#include "RActorPassRenderer.h"

namespace rs3 {

class RShaderFX;
class RDevice;
class RTechnique;
class RMaterial;
class REnvironmentSet;

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// 액터 패스렌더러
//
//  1. RActor::GetUsingPass(virtual) 에서 m_bUsingActorPassRenderer 플래그를 통해
//     알맞는 RenderCandidate를 만든다.( RRP_ACTOR / RRP_NORMAL )
//      - 일반적인 렌러딩을 쓴다면, OnPreRender / OnRender / OnPostRender 가 호출되고,
//        OnPreRender에서 UpdateForRender(예전의 OnPreRender), OnRender에서 RenderSingle이 호출된다.
//
//  2. RActor::OnRegisterToRender 에서는 UpdateForRender가 호출되고,
//     UpdateForRender에서 UpdateAnimationAndMakeRenderChain을 통해 m_bUsingActorPassRenderer가 참인 경우에 한해
//     렌더링할 목록인 렌더체인이 만들어진다.
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
//  8. 렌더체인이란, 렌더링할 목록을 엮어 놓은 단일 링크드 리스트이고 RTreeSceneNode와 RActorNode에서 구현해 사용하고 있다.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

class RS_API RAdditivePassRenderer : public RActorPassRenderer
{
public:
	RAdditivePassRenderer();
	virtual ~RAdditivePassRenderer(){}


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pass Renderer ID 설정
	//
	DECLARE_PASS_RENDERER_ID( RRP_ALPHAADDITIVE );


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//
	virtual void		Init();
	virtual void		Destroy();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// update / rendering
	//
	virtual void		RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;
	virtual void		RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;
	virtual void		RenderDepthOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RShadowStrategy* pShadowStrategy ) override;
};

}
