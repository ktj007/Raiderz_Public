#include "StdAfx.h"
#include "RDecalProjectionSceneNode.h"
#include "RTerrain.h"
#include "RStaticMeshSceneNode.h"
#include "RCameraSceneNode.h"
#include "RShaderFX.h"
#include "RShaderCompositeManager.h"
#include "RCollisionTree.h"

namespace rs3 {

RDecalCollision::RDecalCollision()
{
	m_source		= MVector3(0,0,0);
	m_destination	= MVector3(0,0,0);
	m_vecVertex		= NULL;
}

RDecalCollision::~RDecalCollision()
{
}

bool RDecalCollision::IsCollidable( const MBox &box )
{
	return m_bbMovement.ContainsAABB(box);
}

bool RDecalCollision::CheckPolygon( const MVector3 &lv0,const MVector3 &lv1,const MVector3 &lv2, const DWORD dwAttribute /*= 0*/ )
{
	RVt_pos vertex;
	vertex.vPos = lv0;
	m_vecVertex->push_back(vertex);
	vertex.vPos = lv1;
	m_vecVertex->push_back(vertex);
	vertex.vPos = lv2;
	m_vecVertex->push_back(vertex);
	
	return false;
}

bool RDecalCollision::CheckTangency( const MVector3& vPos, const RCollisionPolygon& poly )
{
	return true;
}

void RDecalCollision::GetBoundingBox( MBox &outBox )
{
	outBox = m_aabb;
}

float RDecalCollision::GetWalkableDistance( const RCollisionPolygon &collisionPolygon )
{
	return true;
}

MVector3 RDecalCollision::GetContactPoint(const MVector3 &vPos, const MVector3 &vNorm)
{
	return MVector3(0,0,0);//vPos - (vNorm * (m_fRadius - 10));
}

void RDecalCollision::SetAABB(const RBoundingBox& rbb)
{
	m_aabb = rbb;
}

void RDecalCollision::SetVertex(std::vector<RVt_pos>* vecVertex)
{
	m_vecVertex = vecVertex;
}

void RDecalCollision::CalcBoundingBox()
{
	RCollision::CalcBoundingBox();
}

//////////////////////////////////////////////////////////////////////////

RDecalProjectionSceneNode::RDecalProjectionSceneNode()
: RSceneNode(RRP_DECAL_PROJECTION)
, m_pTexture(R_NONE)
{
	m_vTFactor  = RVector4(1, 1, 1, 1);
	m_fRotate	= 0;
	m_fZBias	= -1.f / 32768.f;//65535.f;

	m_vecVertexTerrain.clear();
	m_vecVertexActor.clear();
}

RDecalProjectionSceneNode::~RDecalProjectionSceneNode()
{
	if (m_pTexture != R_NONE) {
		REngine::GetDevice().DeleteTexture(m_pTexture);
		m_pTexture = R_NONE;
	}

	m_vecVertexTerrain.clear();
	m_vecVertexActor.clear();
}

bool RDecalProjectionSceneNode::Create(rs3::RCollisionTree* pCollisionTree, string strTexName, RVector3 vPos, float fHalfSizeX, float fHalfSizeY, float fHalfSizeZ, float fRotateZ, DWORD dwFlag)
{
	_ASSERT(m_pTexture == R_NONE);
	m_pTexture = REngine::GetDevice().CreateTexture( strTexName.c_str(), RTF_LINEAR, RCT_AUTOGENMIPMAP );
	m_pCollisionTree	= pCollisionTree;
	m_dwFlags			= dwFlag;
	m_fRotate			= fRotateZ;

	Update(vPos, fHalfSizeX, fHalfSizeY, fHalfSizeZ);

	if( ( m_vecVertexTerrain.size() + m_vecVertexActor.size() ) <= 0 )
		return false;

	return true;
}

void RDecalProjectionSceneNode::Update( RVector3 vPos, float fHalfSizeX, float fHalfSizeY, float fHalfSizeZ )
{
	m_vPos		= vPos;
	m_fDepth	= fHalfSizeZ;

	RBoundingBox aabb( -RVector(fHalfSizeX, fHalfSizeY, fHalfSizeZ) + m_vPos, RVector(fHalfSizeX, fHalfSizeY, fHalfSizeZ) + m_vPos );
	SetAABB( aabb );

	// Terrain 과 StaticMesh에서 Vertex를 수집한다
	GetVertexFromBound();
}

void RDecalProjectionSceneNode::GetVertexFromBound()
{
	if (GetAABB().minx == FLT_MAX || GetAABB().minx == FLT_MIN) return;
	m_vecVertexTerrain.clear();
	m_vecVertexActor.clear();

	if( ( m_dwFlags & DECALFLAG_TERRAIN ) && ( REngine::GetSceneManager().GetCurrentTerrain() ) ) // Terrain
	{
		REngine::GetSceneManager().GetCurrentTerrain()->GetVertexFromBound(m_vecVertexTerrain, GetAABB());
	}

	if( m_dwFlags & DECALFLAG_ACTOR )
	{
		m_DecalCollision.SetAABB(GetAABB());			// 
		m_DecalCollision.SetVertex(&m_vecVertexActor);	// 버텍스 버퍼를 넘기고
		m_DecalCollision.CalcBoundingBox();				// 
		if( m_pCollisionTree )
			m_pCollisionTree->GetCollidablePolygons( m_DecalCollision, RCollisionTriangle::MESH );	// MESH 부분의 폴리곤만 얻어온다
	}

}


RVector3 RDecalProjectionSceneNode::GetInteractionScale(const RBoundingBox& rbb) const
{
	RBoundingBox aabb;
	aabb.vmax = RVector( FLT_MAX, FLT_MAX, FLT_MAX);
	aabb.vmin = RVector(-FLT_MAX,-FLT_MAX,-FLT_MAX);

	if (rbb.vmax > aabb.vmax ||	rbb.vmin < aabb.vmin)
		_ASSERT("GetInteractionScale() AABB가 이상합니다");

	vec3 vCab = rbb.vmax - rbb.vmin;

	return vCab;
}

void RDecalProjectionSceneNode::UpdateTextureTransform()
{
	RVector3 vScale = GetInteractionScale(GetAABB());
	vec3 vPos		= GetAABB().GetCenter();

	// 1. 원점으로 이동
	RMatrix matTranslate;
	matTranslate.MakeIdentity();
	matTranslate.SetTranslation( -m_vPos );//+ RVector( 0.5f, 0.5f, 0.f ) );

	// 2. 업벡터를 노말에 맞추고 Tangent, Binormal를 uv 좌표축으로 설정 // 데칼은 항상 Z축(0,0,1)평면에서 그린다고 가정한다.
	RMatrix matRotate;
	matRotate.MakeIdentity();
	RVector _dir = RVector::AXISY;//m_vRotate.Normalize();
	RVector _up  = RVector::AXISZ;
	RVector _right = Normalize(_dir.CrossProduct(_up));
	matRotate._11 = _right.x;	matRotate._12 = _right.y;	matRotate._13 = _right.z;
	matRotate._21 = _dir.x;		matRotate._22 = _dir.y;		matRotate._23 = _dir.z;
	matRotate._31 = _up.x;		matRotate._32 = _up.y;		matRotate._33 = _up.z;

	// 3. 회전값 적용
	RMatrix matRotTangent;
	matRotTangent.MakeIdentity();
	matRotTangent.SetRotationZ(DegreeToRadian(-m_fRotate));
	
	// 4. -0.5 ~ 0.5 사이로 변환
	RMatrix matScale;
	matScale.MakeIdentity();
	matScale._11 = 1.0f/vScale.x;
	matScale._22 = 1.0f/vScale.y;
	matScale._33 = 1.0f/vScale.z;
	//matScale.SetScale( 1.0f/fScale );

	// 5. +0.5로 0 ~ 1 사이로 변환 
	RMatrix matHalf;
	matHalf.MakeIdentity();
	matHalf.SetTranslation( RVector( 0.5f, 0.5f, 0.f ) );
	
	RMatrix matTexTransform = matTranslate * matRotate * matRotTangent * matScale * matHalf;
	RShaderCompositeManager::m_cTextureTransform.SetMatrix(matTexTransform);

}

void RDecalProjectionSceneNode::SetRenderState( RCameraSceneNode* pCurrentCamera)
{
	UpdateTextureTransform();

	RShaderCompositeManager::m_cDecalCenterPos.SetVector4(RVector4(m_vPos.x, m_vPos.y, m_vPos.z, m_fDepth));
	RShaderCompositeManager::m_cTextureFactor.SetVector4( m_vTFactor );
	RShaderCompositeManager::m_cVisiblity.SetFloat(GetVisibility());
}

void RDecalProjectionSceneNode::OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod)
{
	SetRenderState(pCurrentCamera);

	REngine::GetDevicePtr()->SetTexture(AS_DECALMAP, m_pTexture);
	REngine::GetDevicePtr()->SetTextureFilter(AS_DECALMAP, RTF_BILINEAR);
	REngine::GetDevicePtr()->SetTextureAddress(AS_DECALMAP, RTADDRESS_BORDER, RTADDRESS_BORDER, RTADDRESS_BORDER);
	REngine::GetDevicePtr()->SetFvF(RVt_pos::FVF);

	// 쉐이더를 쓸필요없지만, 고정파이프라인과 쉐이더를 같이쓰면 z-fighting 이 일어나서 쓴다
	RSCID rscComponent = RSC_POSITION | RSC_TRANSFORM_CONSTANT | RSC_TRANSFORM | REngine::GetSceneManager().GetShaderCompositeManager()->GetProjectionTexcoordComponent() | 
		RSC_DECAL | RSC_PPL_VISIBLITY | RSC_PPL_FOG;
	RShaderFX* pShader = REngine::GetSceneManager().GetShaderCompositeManager()->GetShader(rscComponent, false);
	if( pShader )
	{
		RTechnique Technique = pShader->GetTechniqueByIndex(0);

		Technique.Begin(NULL, TECH_PRESERVE_NOT);
		Technique.BeginPass();

		// 같은 쉐이더를 타므로 상관없음
		if( m_vecVertexTerrain.size() > 0 )
			REngine::GetDevicePtr()->DrawPrimitiveUP( RPT_TRIANGLELIST, m_vecVertexTerrain.size() / 3, (LPVOID)&m_vecVertexTerrain[0], sizeof(RVt_pos) );

		// 임의의 cl2 의 폴리곤을 사용하므로 Z 값 조절이 필요함
		REngine::GetDevicePtr()->SetDepthBias(-1.f / 8192.f);//m_fZBias);//, m_fSlopeBias);
		if( m_vecVertexActor.size() > 0 )
			REngine::GetDevicePtr()->DrawPrimitiveUP( RPT_TRIANGLELIST, m_vecVertexActor.size() / 3, (LPVOID)&m_vecVertexActor[0], sizeof(RVt_pos) );

		Technique.EndPass();
		Technique.End();
	}

	REngine::GetDevicePtr()->SetTexture(0, R_NONE);
	REngine::GetDevicePtr()->SetDepthBias(0);
}

} //namespace rs3
