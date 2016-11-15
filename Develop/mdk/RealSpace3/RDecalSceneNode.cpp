#include "StdAfx.h"
#include "RDecalSceneNode.h"
#include "RTerrain.h"
#include "RStaticMeshSceneNode.h"
#include "RCameraSceneNode.h"
#include "RShaderComposite.h"
#include "RShaderCompositeManager.h"

namespace rs3 {

RDecalSceneNode::RDecalSceneNode()
: RSceneNode(RRP_DECAL)
, m_dwFlags(0)
{
	m_vPos.Set(RVector::ZERO);
	m_vTFactor	= RVector4(1, 1, 1, 1);
	m_fRotate = 0;
}

RDecalSceneNode::~RDecalSceneNode()
{
	Destroy();
}

void RDecalSceneNode::Destroy()
{

}

bool RDecalSceneNode::Create(string strTexName, RVector3 vPos, float fRadiusX, float fRadiusY, float fRadiusZ, float fRotateZ, DWORD dwFlag, rs3::RCollisionTree* pCollisionTree)
{
	m_dwFlags	= dwFlag;
	m_fRotate	= fRotateZ;
	
	m_vPos		= vPos;
	m_aabb.vmax =  RVector(fRadiusX, fRadiusY, fRadiusZ) + m_vPos;
	m_aabb.vmin = -RVector(fRadiusX, fRadiusY, fRadiusZ) + m_vPos;

	UpdateBoundingVolume();

	// Terrain 과 StaticMesh에서 Vertex를 수집한다
	GetVertexFromBound();

	if( ( m_vecVertexTerrain.size() + m_vecVertexActor.size() ) <= 0 )
		return false;

	return true;
}

void RDecalSceneNode::GetVertexFromBound()
{
	if (m_aabb.minx == FLT_MAX || m_aabb.minx == FLT_MIN) return;
	m_vecVertexTerrain.clear();
	m_vecVertexActor.clear();

	if( ( m_dwFlags & DECALFLAG_ACTOR ) && ( REngine::GetSceneManager().GetCurrentStaticMeshSceneNode() ) ) // StaticMesh
	{
		REngine::GetSceneManager().GetCurrentStaticMeshSceneNode()->GetVertexFromBound(&m_vecVertexActor, m_aabb);
	}

	if( ( m_dwFlags & DECALFLAG_TERRAIN ) && ( REngine::GetSceneManager().GetCurrentTerrain() ) ) // Terrain
	{
		REngine::GetSceneManager().GetCurrentTerrain()->GetVertexFromBound(m_vecVertexTerrain, m_aabb);
	}
}

void RDecalSceneNode::SetRenderState( RCameraSceneNode* pCurrentCamera)
{
	RDevice* pDev = REngine::GetDevicePtr();
	_ASSERT( pDev );

	// RenderState
	pDev->SetTextureAddress(AS_DECALMAP, RTADDRESS_BORDER, RTADDRESS_BORDER, RTADDRESS_BORDER);
	
	pDev->SetCullMode( RCULL_NONE );
	pDev->SetAlphaTestEnable(false);
	pDev->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);

	pDev->SetTextureStageState(0, RTSS_COLOROP, RTOP_MODULATE);
	pDev->SetTextureStageState(0, RTSS_COLORARG1, RTA_TEXTURE);
	pDev->SetTextureStageState(0, RTSS_COLORARG2, RTA_TFACTOR);

	pDev->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_MODULATE);
	pDev->SetTextureStageState(0, RTSS_ALPHAARG1, RTA_DIFFUSE );
	pDev->SetTextureStageState(0, RTSS_ALPHAARG2, RTA_TFACTOR);

	// 알파를 끄면 얻어오는 버텍스의 크기를 알수 있다.[3/21/2008 madduck]
	//pDev->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_DISABLE);

	pDev->SetTextureStageState(1, RTSS_COLOROP, RTOP_DISABLE);
	pDev->SetTextureStageState(1, RTSS_ALPHAOP, RTOP_DISABLE);

	m_vTFactor.a = m_fVisibility;
	RShaderCompositeManager::m_cTextureFactor.SetVector4( m_vTFactor );
	RShaderCompositeManager::m_cVisiblity.SetFloat(1.f);

	REngine::GetDevice().SetTransform(RST_VIEW, pCurrentCamera->GetViewMatrix());
	REngine::GetDevice().SetTransform(RST_PROJECTION, pCurrentCamera->GetProjectionMatrix());
	REngine::GetDevice().SetTransform(RST_WORLD, RMatrix::IDENTITY);	// Data가 World 정점이다.

	RMatrix matViewProjection = REngine::GetDevice().GetTransform(RST_VIEW)*REngine::GetDevice().GetTransform(RST_PROJECTION);
	RShaderCompositeManager::m_cWorldViewProj.SetMatrix( matViewProjection );
	RShaderCompositeManager::m_cWorld.SetMatrix( RMatrix::IDENTITY );
}



} //namespace rs3
