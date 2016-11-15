#include "StdAfx.h"
#include "REffectPassRenderer.h"
#include "RCameraSceneNode.h"
#include "RShaderCompositeManager.h"
#include "RPostEffectDistortion.h"
#include "REffectBase.h"
#include "REffectActor.h"
#include "RParticleShader.h"

namespace rs3 {

struct SORT_INFO
{
	RSceneNode*		pParentEffectSceneNode;
	float			fDistance;
	int				nRenderOrder;

	bool operator > ( const SORT_INFO& other ) const
	{
		if( fDistance > other.fDistance )
		{
			return true;
		}

		if( pParentEffectSceneNode == other.pParentEffectSceneNode &&
			nRenderOrder < other.nRenderOrder )
		{
			return true;
		}

		return false;
	}
};

const SORT_INFO& SetSortInfo( RCameraSceneNode* pCamera, RSceneNode* pParentEffectSceneNode, int nRenderOrder )
{
	_ASSERT( pParentEffectSceneNode );

	static SORT_INFO info;

	info.pParentEffectSceneNode = pParentEffectSceneNode;
	info.fDistance = pCamera->GetDistanceTo( pParentEffectSceneNode->GetWorldPosition() );
	info.nRenderOrder = nRenderOrder;

	return info;
}

///////////////////////////////////////////////////////////////////////////////

REffectPassRenderer::REffectPassRenderer() :
 m_bSort(true)
,m_pParticleShader(NULL)
{
	m_pParticleShader = new RParticleShader;
}

REffectPassRenderer::~REffectPassRenderer()
{
	Destroy();
	SAFE_DELETE( m_pParticleShader );
}

void REffectPassRenderer::Init()
{
	_ASSERT( m_pParticleShader );
	if( m_pParticleShader )
		m_pParticleShader->Init();
}

void REffectPassRenderer::Destroy()
{
	if( m_pParticleShader )
		m_pParticleShader->Destroy();
}

void REffectPassRenderer::RenderOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;

	typedef multimap<SORT_INFO, RSceneNode*, greater<SORT_INFO>> DISTANCENODEMAP;
	DISTANCENODEMAP sortingNodes;
	SCENE_SET::iterator	iter;
	for( iter = pCulledList->begin() ; iter != pCulledList->end() ; ++iter)
	{
		RSceneNode *pSceneNode = (*iter);
		if( false == IsRenderable( pSceneNode ) )
			continue;

		if( REffectBase* pEffect = MDynamicCast(REffectBase, pSceneNode) )
		{
			const SORT_INFO& info = SetSortInfo( pCurrentCamera, pEffect->GetParentEffectSceneNode(), pEffect->GetRenderOrder() );
			sortingNodes.insert( DISTANCENODEMAP::value_type(info,pSceneNode) );
		}
	}

	REngine::GetResourceProfiler().StartMeshProfiler();

	DISTANCENODEMAP::iterator itr;
	for(itr = sortingNodes.begin(); itr!= sortingNodes.end(); ++itr)
	{
		RSceneNode* pNode = itr->second;
		pNode->Render( pCurrentCamera, -1, renderMethod);	/// 어떠한 랜더링 모드이든 상관이 없이 그리는 방법은 일괄적.
	}
}

void REffectPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;

	SetRenderState( pCurrentCamera );
	RenderOnPass( pCurrentCamera, pRenderingList, renderMethod);
	RestoreRenderState();

	RPFC_GPU_CALL;
}

bool REffectPassRenderer::IsRenderable( const RSceneNode* pSceneNode )
{
	if( false == pSceneNode->GetVisible() )
		return false;

	_ASSERT( pSceneNode->GetVisibilityResult() >= 0.f || pSceneNode->GetVisibilityResult() <= 1.f );
	if( pSceneNode->GetVisibilityResult() <= 0.f )
		return false;

	if( false == pSceneNode->GetUsable() )
		return false;

	return true;
}

void REffectPassRenderer::SetRenderState( RCameraSceneNode* pCamera )
{
	RRenderingStrategy* pRenderingStrategy = REngine::GetSceneManager().GetRenderingStrategy();
	_ASSERT( pRenderingStrategy );

	_ASSERT( m_pParticleShader );
	m_pParticleShader->SetTransformConstant( RMatrix::IDENTITY, pCamera->GetViewMatrix(), pCamera->GetViewProjectionMatrix(),
		pCamera->GetFarClipCornerInView(), pRenderingStrategy->GetWidth(), pRenderingStrategy->GetHeight(), pCamera->GetFarZ());
	m_pParticleShader->SetFogConstant( pCamera->GetFogEnable(), pCamera->GetFogNear(), pCamera->GetFogFar(), pCamera->GetFogColor() );
	m_pParticleShader->SetDepthBiasBlendDistConstant( 30.f );

	REngine::GetDevice().SetTexture( AS_VIEWSPACE_DEPTHMAP, pRenderingStrategy->GetDepthBuffer());
	REngine::GetDevice().SetTextureAddressClamp(AS_VIEWSPACE_DEPTHMAP);
	REngine::GetDevice().SetTextureFilter(AS_VIEWSPACE_DEPTHMAP, RTF_POINT);
}

void REffectPassRenderer::RestoreRenderState()
{

}

}