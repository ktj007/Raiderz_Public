#pragma once

namespace rs3 {

class RSceneManager;
class RDevice;
struct RCULLDESC;
class RShadowStrategy;
enum RRENDERINGMETHOD;

//////////////////////////////////////////////////////////////////////////
//
// 패스렌더러 선언 매크로
//
//////////////////////////////////////////////////////////////////////////
#define DECLARE_PASS_RENDERER_ID(x)		enum { PASS_RENDERER_ID = x };

//////////////////////////////////////////////////////////////////////////
//
// 패스렌더러
//
//  1. RSceneManager::Cull / RSceneManager::UpdateAndCull 에서
//     카메라의 RegisterToRender가 호출되어 씬노드의 OnRegisterToRender가 불릴때,
//     각각의 씬노드는 자신들의 정책에 패스렌더러에 등록을 한다.
//
//  2. RCameraSceneNode::RenderPass 에서 패스에 맞는 패스렌더러가 있다면
//     PassRenderer의 PassOnRender를 통해 렌더링을 한다.
//
//////////////////////////////////////////////////////////////////////////
class RS_API RPassRenderer
{
public:
	RPassRenderer(){}
	virtual ~RPassRenderer(){}

	//////////////////////////////////////////////////////////////////////////
	// Pass Renderer ID 설정
	//
	DECLARE_PASS_RENDERER_ID( RRP_NONE );


	//////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//
	virtual void Init()		{}
	virtual void Destroy()	{}
	virtual void Reset()	{}

	//////////////////////////////////////////////////////////////////////////
	// update
	//
	virtual void Update(float fTimeDelta)	{ }

	//////////////////////////////////////////////////////////////////////////
	// culling
	//
	virtual	bool CullByViewFrustum(const RSceneNode* pNode,RCameraSceneNode* pCamera);
	virtual bool CullByPlanes(RCameraSceneNode* pCamera, const RSceneNode* pNode, const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar );

	//////////////////////////////////////////////////////////////////////////
	// rendering
	//
	virtual bool IsCurrentShaderRejective()	{ return true; }
	virtual void RenderOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RRENDERINGMETHOD eMothod);
	virtual void RenderDepthOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RShadowStrategy* pShadowStrategy);
	
	virtual void RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod);
	virtual void RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod);

	virtual void RenderZStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod);

protected:
	bool CullDefault( const RBox& aabb, const vector<RCULLDESC>& cullDescs );
};

}
