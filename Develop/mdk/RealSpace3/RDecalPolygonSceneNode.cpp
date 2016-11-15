#include "StdAfx.h"
#include "RDecalPolygonSceneNode.h"
#include "RStaticMeshSceneNode.h"
#include "RShaderCompositeManager.h"
#include "RShaderFX.h"
#include "RTerrain.h"
#include "RCollisionTree.h"

namespace rs3 {

RDecalPolygonRenderSceneNode::RDecalPolygonRenderSceneNode(int nPriority, DWORD dwBlendTime, int nVertexSize)
: RSceneNode(RRP_DECAL_POLYGON)
{
	m_nPriority		= nPriority;
	m_dwBlendTime	= dwBlendTime;
	m_nVertexSize	= nVertexSize;

	m_vTFactor  = RVector4(1, 1, 1, 1);

	Create();
}

void RDecalPolygonRenderSceneNode::Create()
{
	m_nOffsetStart	= 0;
	m_nOffsetEnd	= 0;
	m_nOffsetFree	= m_nVertexSize;

	m_nOffsetDeletedStart	= 0;
	m_nOffsetDeletedEnd		= 0;
	m_nOffsetDeletedFree	= m_nVertexSize;

	m_hVertexBuffer = REngine::GetDevice().CreateVertexBuffer( sizeof(RVt_pos_clr_tex1) * m_nVertexSize, sizeof(RVt_pos_clr_tex1), 0, (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY));
	if(!m_hVertexBuffer) {
		_ASSERT("decal polygon vb create failed");
	}

	m_hDeletedVertexBuffer = R_NONE;
	m_pTexture = R_NONE;
/*
	m_hDeletedVertexBuffer = REngine::GetDevice().CreateVertexBuffer( sizeof(RVt_pos_clr_tex1) * nMaxVertexSize, sizeof(RVt_pos_clr_tex1), 0, (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY));
	if(!m_hDeletedVertexBuffer) {
		_ASSERT("decal polygon deleted vb create failed");
	}*/

//	m_hDeletedVertexBuffer = REngine::GetDevice().CreateVertexBuffer( sizeof(RVt_pos_clr_tex1) * nMaxVertexSize)
}

RDecalPolygonRenderSceneNode::~RDecalPolygonRenderSceneNode()
{
	RemoveFromParent();

	if ( m_hVertexBuffer != R_NONE ) {
		REngine::GetDevice().DeleteVertexBuffer(m_hVertexBuffer);
		m_hVertexBuffer = R_NONE;
	}

	if ( m_hDeletedVertexBuffer != R_NONE ) {
		REngine::GetDevice().DeleteVertexBuffer(m_hDeletedVertexBuffer);
		m_hDeletedVertexBuffer = R_NONE;
	}
}

void RDecalPolygonRenderSceneNode::UpdateAABB(std::vector<RVt_pos_clr_tex1>& vecVertex)
{
	RBoundingBox aabb; // AABB 갱신
	aabb.Initialize();

	for( size_t i = 0; i < vecVertex.size(); ++i )
		aabb.Add(vecVertex[i].vPos);	

	m_deqDecalAABB.push_back(aabb);

	RBoundingBox aabbSum;
	aabbSum.Initialize();
	for( size_t i = 0; i < m_deqDecalAABB.size(); ++i )
	{
		aabbSum.Add(m_deqDecalAABB[i]);
	}
	SetAABB( aabbSum );
}

void RDecalPolygonRenderSceneNode::AddDecal(std::vector<RVt_pos_clr_tex1>& vecVertex, RTexture* pTexture)
{
	RPFC_THISFUNC;

	int nCountVertex = vecVertex.size();
	if( pTexture )
		m_pTexture = pTexture;
	m_nOffsetDeletedStart = m_nOffsetStart;

	// 최대 버퍼 사이즈 보다 크다!!
	_ASSERT(m_nVertexSize >= nCountVertex);
	while( m_nOffsetFree < nCountVertex ) 
	{
		// 남은 공간이 부족하면 앞에서 부터 삭제한다.
		int nCountDeleteDecalPolygon = m_deqDecalPolygon[0];

		m_nOffsetStart+= m_nOffsetEnd + nCountDeleteDecalPolygon;
		m_nOffsetFree += nCountDeleteDecalPolygon;

		m_nOffsetStart %= m_nVertexSize;

		m_deqDecalPolygon.pop_front();
		m_deqDecalAABB.pop_front();
	}

	m_deqDecalPolygon.push_back(nCountVertex);
	
	UpdateAABB(vecVertex);
	
	int nOffsetLeftPolygon = nCountVertex;
	int nOffsetPolygon = 0;
	
	/*
	RVt_pos_clr_tex1 *arrDeletedVertex = (RVt_pos_clr_tex1*)REngine::GetDevice().LockVertexBuffer(m_hDeletedVertexBuffer);
	_ASSERT(arrDeletedVertex);
	// 순회 하면서 Visibility 가 0인 폴리곤이 있으면 삭제
	if( arrDeletedVertex )
	{
		for( size_t i = 0; i < m_vecDeletedVertices.size(); ++i )
		{
			RVt_pos_clr_tex1 DeleteVertex = m_vecDeletedVertices[i];
			//DeleteVertex.dwColor
		}
	}
	*/

	RVt_pos_clr_tex1 *arrVertex = (RVt_pos_clr_tex1*)REngine::GetDevice().LockVertexBuffer(m_hVertexBuffer, D3DLOCK_NOOVERWRITE);
	_ASSERT(arrVertex);

	/*
	// 삭제할 폴리곤이 있으면 삭제 폴리곤 데이터를 수집한다.
	if ( nTotalCountDeletedPolygon && arrVertex )
	{
		for( int i = m_nOffsetDeletedStart; i < m_nOffsetDeletedStart + nTotalCountDeletedPolygon; ++i )
		{
			if( i >= m_nMaxSizeBuffer )
				i %= m_nMaxSizeBuffer;

			RVt_pos_clr_tex1 DeleteVertex = (RVt_pos_clr_tex1)arrVertex[i];
			m_vecDeletedVertices.push_back(DeleteVertex);
		}
	}

	if( m_vecDeletedVertices.size() > 0 )
		memcpy(&arrDeletedVertex, &m_vecDeletedVertices[0], sizeof(RVt_pos_clr_tex1) * m_vecDeletedVertices.size());
	*/
	
	// 정점 버퍼에 담는다.
	if( m_nVertexSize - m_nOffsetEnd < nCountVertex )
	{
		// 버퍼의 마지막을 넘으면 두번 나누어 담는다.
		// 1. End부터 끝까지(남은 폴리곤을 계산)
		// 2. 0부터 남은 폴리곤을 마저 그린다
		nOffsetPolygon = nCountVertex - ( nCountVertex - (m_nVertexSize - m_nOffsetEnd) );
		memcpy(&arrVertex[m_nOffsetEnd], &vecVertex[0], sizeof(RVt_pos_clr_tex1) * nOffsetPolygon);
		m_nOffsetEnd = 0;
		nOffsetLeftPolygon -= nOffsetPolygon;
	}

	// 한바퀴를 돌았다면, 이때 End 는 0이다
	memcpy(&arrVertex[m_nOffsetEnd], &vecVertex[nOffsetPolygon], sizeof(RVt_pos_clr_tex1) * nOffsetLeftPolygon);
	REngine::GetDevice().UnlockVertexBuffer(m_hVertexBuffer);
	//REngine::GetDevice().UnlockVertexBuffer(m_hDeletedVertexBuffer);

	m_nOffsetEnd  += nOffsetLeftPolygon % m_nVertexSize;
	m_nOffsetFree -= nCountVertex;
	m_nOffsetStart = (m_nOffsetEnd + m_nOffsetFree) % m_nVertexSize;

//	mlog("%d / %d\n", m_nVertexSize-m_nOffsetFree, m_nVertexSize);

}

RDecalPolygonSceneNode::RDecalPolygonSceneNode()
{
	m_mapDecalPolygonRenderSet.clear();
	m_vecVertexTerrain.clear();
	m_vecVertexActor.clear();

	m_fDecalEpsilon 	= 0.25f;
	m_fBias				= -1.f / 65535.f;//16777215.f;
	m_fSlopeBias		= 0.f;
	m_bWire				= false;
}

RDecalPolygonSceneNode::~RDecalPolygonSceneNode()
{
	Destroy();
}

bool RDecalPolygonSceneNode::Create(rs3::RCollisionTree* pCollisionTree )
{
	m_pCollisionTree = pCollisionTree;

	return true;
}

void RDecalPolygonSceneNode::Destroy()
{
	for(std::map<std::pair<int, RTexture*>, RDecalPolygonRenderSceneNode*>::iterator itr = m_mapDecalPolygonRenderSet.begin(); itr != m_mapDecalPolygonRenderSet.end(); ++itr) {
		RDecalPolygonRenderSceneNode* pRenderSceneNode = (*itr).second;
		RTexture* pTexture = (*itr).first.second;
		for( size_t i = 0; i < pRenderSceneNode->m_deqDecalPolygon.size(); ++i )
		{
			REngine::GetDevicePtr()->DeleteTexture(pTexture);
		}
		pTexture = R_NONE;
		SAFE_DELETE((*itr).second);
	}

	m_mapDecalPolygonRenderSet.clear();

	m_vecTempVertex.clear();
	m_vecVertexTerrain.clear();
	m_vecVertexActor.clear();
}

//----------------------------------------------------------------------------------------------------
// Add(string strTexName, RVector3 vPos, float fHalfSizeX, float fHalfSizeY, float fHalfSizeZ, float fRotateZ, int nPriority, int nVertexBufferSize, DWORD dwBlendTime, DWORD dwFlag)
/**
@brief		RDecalPolygonSceneNode 에 Decal 객체 추가.
@param		strTexName			\n[in] 추가할 텍스쳐 이름.
@param		vPos				\n[in] 픽킹 위치. 이 지점에 해당하는 폴리곤을 찾고 그 폴리곤의 노말이 Z 축이 된다.
@param		fHalfSizeX			\n[in] 데칼을 그릴 X 영역의 크기.
@param		fHalfSizeY			\n[in] 데칼을 그릴 Y 영역의 크기.
@param		fHalfSizeZ			\n[in] 데칼을 그릴 Z 영역의 크기. (깊이로 이해하면 됨)
@param		fRotateZ			\n[in] Z 축 기준 회전값(왼손 기준이므로 시계방향으로 회전함).
@param		nPriority			\n[in] 우선 순위. 우선 순위가 높으면 위에 덧그리게 된다.
@param		nVertexBufferSize	\n[in] 버텍스 버퍼의 사이즈. 데칼의 개수가 아님에 유의한다.
@param		dwBlendTime			\n[in] (지원예정) 블랜딩 되며 지워질 데칼이 완전히 사라지는데까지 걸리는 시간.
@param		dwFlag				\n[in] 픽킹 폴리곤을 수집할 범위.  DECALFLAG_TERRAIN = 지형, DECALFLAG_ACTOR = 액터(cl2)
@return		void
*/
//----------------------------------------------------------------------------------------------------
void RDecalPolygonSceneNode::Add(string strTexName, RVector3 vPos, float fHalfSizeX, float fHalfSizeY, float fHalfSizeZ, float fRotateZ, int nPriority, int nVertexBufferSize, DWORD dwBlendTime, DWORD dwFlag)
{
	RPFC_THISFUNC;

	m_dwFlags	= dwFlag;
	m_vPos		= vPos;
	nVertexBufferSize -= (nVertexBufferSize % 3);	///< 3 으로 나눠 떨어져야 한다.
	
	RBoundingBox aabb;
	aabb.vmax =  RVector(fHalfSizeX, fHalfSizeY, fHalfSizeZ) + m_vPos;
	aabb.vmin = -RVector(fHalfSizeX, fHalfSizeY, fHalfSizeZ) + m_vPos;
	m_fDepth = 10000.0f;	// Polygon에서는 사용하지 않는다.
	
	RBoundingBox scenenNodeAABB( GetAABB() );
	scenenNodeAABB.Add( aabb );
	SetAABB( scenenNodeAABB );

	// Terrain 과 StaticMesh에서 Vertex를 수집한다
	if( GetVertexFromBound(aabb) == false) {
		//mlog("Decal 생성 실패!! Picking 된 삼각형이 없습니다!");
		return;
	}

	RVector3 vDefNormal		= RVector3(0,0,1);	// Z+
	RVector3 vTangent;

	// 기준 Normal 벡터의 회전 행렬을 구하고
	// 이를 기준 Tangent 벡터에 적용해 실제 변환된 Tangent 를 구한다.
	RMatrix matRot;

	RVector vCross			= vDefNormal.CrossProduct(m_vDecalNormal);	// 회전 기준축
	float	fAngleRadian	= vDefNormal.AngleBetween(m_vDecalNormal);	// 세타를 구한다

	matRot.SetRotationMatrixAxis(vCross, fAngleRadian);
	RMatrix matRotZ;
	matRotZ.SetRotationZ(DegreeToRadian(fRotateZ));
	matRot *= matRotZ;
	matRot.TransformVect(RVector3(1,0,0), vTangent);

	RVector3 vBinormal	= m_vDecalNormal.CrossProduct(vTangent);
	vTangent = vBinormal.CrossProduct(m_vDecalNormal);

	m_vDecalNormal.Normalize();
	vTangent.Normalize();

	// VertexTerrain에 옮겨담는다
	for( int i = 0; i < (int)m_vecVertexActor.size(); ++i ) {
		m_vecVertexTerrain.push_back(m_vecVertexActor[i]);
	}

	m_vecDecalVertex.clear();
	CreatePolygon(m_vecVertexTerrain, vPos, m_vDecalNormal, vTangent, fHalfSizeX * 2, fHalfSizeY * 2, fHalfSizeZ * 2); // 지름을 넘겨준다.
	if( m_nCountVertex <= 0 ) {
		//mlog("Decal 생성 실패!! RDecalPolygon 정점이 0개 입니다!\n");
		return;
	}

	//	_ASSERT(m_hTexture == R_NONE);
	// SceneNode에서 RenderSet을 지울때 해제한다.
	RTexture* pTexture = REngine::GetDevice().CreateTexture( strTexName.c_str(), RTF_LINEAR, RCT_AUTOGENMIPMAP );
	
	//////////////////////////////////////////////////////////////////////////
	// DecalPolygon Deque
	//////////////////////////////////////////////////////////////////////////
	// 해당 Texture 별로 Set에 등록/추가 한다.
	std::map<std::pair<int, RTexture*>, RDecalPolygonRenderSceneNode*>::iterator itr = m_mapDecalPolygonRenderSet.find(make_pair(nPriority, pTexture));
	if (itr == m_mapDecalPolygonRenderSet.end()) // end()값이 넘어오면 찾지 못한 것임.
	{
		// 새로운 RenderSet 추가
		RDecalPolygonRenderSceneNode* pRenderSet = new RDecalPolygonRenderSceneNode(nPriority, dwBlendTime, nVertexBufferSize);	// 버퍼도 할당 된다.
		m_mapDecalPolygonRenderSet.insert( make_pair( make_pair(nPriority, pTexture), pRenderSet) );
		pRenderSet->AddDecal(m_vecDecalVertex, pTexture);
		REngine::GetSceneManager().AddSceneNode(pRenderSet);
	}
	else	// 이미 있다면
	{
		RDecalPolygonRenderSceneNode* pRenderSet = (*itr).second;
		pRenderSet->AddDecal(m_vecDecalVertex, pTexture);
	}
}

bool RDecalPolygonSceneNode::GetVertexFromBound(RBoundingBox aabb)
{
	RPFC_THISFUNC;

	if (aabb.minx == FLT_MAX || aabb.minx == FLT_MIN) return false;
	m_vecVertexTerrain.clear();
	m_vecVertexActor.clear();

	float		fPickDistanceSq_Terrain = FLT_MAX;
	float		fPickDistanceSq_StaticMesh = FLT_MAX;

	RVector3	vPickTriangle[3] = { RVector3(0, 0, 0), RVector3(0, 0, 0), RVector3(0, 0, 0) };

	if( ( m_dwFlags & DECALFLAG_TERRAIN ) && ( REngine::GetSceneManager().GetCurrentTerrain() ) ) // Terrain
	{
		REngine::GetSceneManager().GetCurrentTerrain()->GetVertexFromBound(m_vecVertexTerrain, aabb);

		int nTriIndex = -1;
		// Picking된 Face를 찾는다. 
		IntersectionTriangle(m_vecVertexTerrain, fPickDistanceSq_Terrain, nTriIndex);

		if( fPickDistanceSq_Terrain != FLT_MAX )
		{
			vPickTriangle[0] = m_vecVertexTerrain[nTriIndex + 0].vPos;
			vPickTriangle[1] = m_vecVertexTerrain[nTriIndex + 1].vPos;
			vPickTriangle[2] = m_vecVertexTerrain[nTriIndex + 2].vPos;
		}
	}

	if( m_dwFlags & DECALFLAG_ACTOR )
	{
		m_DecalCollision.SetAABB(aabb);				// 
		m_DecalCollision.SetVertex(&m_vecVertexActor);	// 버텍스 버퍼를 넘기고
		m_DecalCollision.CalcBoundingBox();				// 
		if( m_pCollisionTree )
			m_pCollisionTree->GetCollidablePolygons( m_DecalCollision, RCollisionTriangle::MESH );	// MESH 부분의 폴리곤만 얻어온다

		int nTriIndex = -1;
		// Picking된 Face를 찾는다. 
		IntersectionTriangle(m_vecVertexActor, fPickDistanceSq_StaticMesh, nTriIndex);

		if( fPickDistanceSq_Terrain > fPickDistanceSq_StaticMesh )
		{
			vPickTriangle[0] = m_vecVertexActor[nTriIndex + 0].vPos;
			vPickTriangle[1] = m_vecVertexActor[nTriIndex + 1].vPos;
			vPickTriangle[2] = m_vecVertexActor[nTriIndex + 2].vPos;
		}
	}

	// Picking Face의 Normal을 구한다.
	RVector3 v0 = vPickTriangle[1] - vPickTriangle[0];
	RVector3 v1 = vPickTriangle[2] - vPickTriangle[0];
	m_vDecalNormal = v0.CrossProduct(v1);
	m_vDecalNormal.Normalize();

	if ( (fPickDistanceSq_Terrain == FLT_MAX) && (fPickDistanceSq_StaticMesh == FLT_MAX) )
		return false;

	return true;
}

void RDecalPolygonSceneNode::DeleteAll(string strTexName)
{
	// RenderSet을 삭제한다.
	for(std::map<std::pair<int, RTexture*>, RDecalPolygonRenderSceneNode*>::iterator itr = m_mapDecalPolygonRenderSet.begin(); itr != m_mapDecalPolygonRenderSet.end(); ) 
	{
		RTexture* pTexture = (*itr).first.second;
		if( pTexture->GetName() == strTexName )
		{
			SAFE_DELETE((*itr).second);
			itr = m_mapDecalPolygonRenderSet.erase(itr);
			return;
		}
		else
			++itr;
	}
}

// 어차피 우선순위가 정해져 나올텐데 필요한가?
void RDecalPolygonSceneNode::SetOverlayPriority(string strTexName, int nPriority)
{
	//for(std::map<std::pair<int, RTexture*>, RDecalPolygonRenderSceneNode*>::iterator itr = m_mapDecalPolygonRenderSet.begin(); itr != m_mapDecalPolygonRenderSet.end(); itr++) 
	//{
	//	RTexture* pTexture = (*itr).first.second;
	//	if( pTexture->GetName() == strTexName )
	//	{
	//		((*itr).first.first) = nPriority;

	//		return;
	//	}
	//}
}

void RDecalPolygonSceneNode::IntersectionTriangle(std::vector<RVt_pos>& vecVertex, float& fPickDistanceSq, int& nTriIndex )
{
	RCameraSceneNode* pCamera = REngine::GetSceneManager().GetPrimaryCamera();
	RVector3	vPickOrg(pCamera->GetWorldTransform()._41, pCamera->GetWorldTransform()._42, pCamera->GetWorldTransform()._43);
	RVector3	vPickDir(m_vPos - vPickOrg); vPickDir.Normalize();
	RVector3	vPickPos;
	
	// StaticMesh
	for( int i = 0; i < (int)vecVertex.size(); i+=3 ) 
	{
		bool bPicked;
		
		// 삼각형 구축이 PAN 방식임에 유의
		bPicked = MMath::IsIntersectNoCull( vPickOrg, vPickDir, vecVertex[i].vPos, vecVertex[i + 1].vPos, vecVertex[i + 2].vPos, &vPickPos );

		if(bPicked) 
		{
			float lengthSq = (vPickPos - vPickOrg).LengthSq();
			if(fPickDistanceSq > lengthSq) 
			{
				nTriIndex = i;
				fPickDistanceSq = lengthSq;
			}
		}
	}
}

void RDecalPolygonSceneNode::CreatePolygon(std::vector<RVt_pos>& vecVertexSource, RVector3& vCenter, RVector3& vNormal, RVector3& vTangent, float fWidth, float fHeight, float fDepth)
{
	RPFC_THISFUNC;

	m_vDecalCenter = vCenter;
	m_vDecalNormal = vNormal;

	RVector3 binormal = vNormal.CrossProduct(vTangent);

	// Calculate boundary planes
	float d = vCenter.DotProduct(vTangent);
	m_vLeftPlane	= RVector4(vTangent.x, vTangent.y, vTangent.z, fWidth * 0.5F - d);
	m_vRightPlane	= RVector4(-vTangent.x, -vTangent.y, -vTangent.z, fWidth * 0.5F + d);

	d = vCenter.DotProduct(binormal);
	m_vBottomPlane	= RVector4(binormal.x, binormal.y, binormal.z, fHeight * 0.5F - d);
	m_vTopPlane		= RVector4(-binormal.x, -binormal.y, -binormal.z, fHeight * 0.5F + d);

	d = vCenter.DotProduct(vNormal);
	m_vBackPlane	= RVector4(vNormal.x, vNormal.y, vNormal.z, fDepth - d);
	m_vFrontPlane	= RVector4(-vNormal.x, -vNormal.y, -vNormal.z, fDepth + d);
	
	// Begin with empty mesh
	m_nCountVertex		= 0;
	m_nCountTriangle	= 0;

	m_vecVertex.clear();
	m_vecIndex.clear();

	ClipMesh((int)vecVertexSource.size() / 3, vecVertexSource, NULL);

	// Assign texture mapping coordinates
	float one_over_w = 1.0F / fWidth;
	float one_over_h = 1.0F / fHeight;

	m_vecTempVertex.clear();
	m_nCountVertex = m_nCountVertex;
	for (int i = 0; i < m_nCountVertex; ++i)
	{
		RVector3 v = m_vecVertex[i] - vCenter;
		float fU = v.DotProduct(vTangent) * one_over_w + 0.5F;
		float fV = v.DotProduct(binormal) * one_over_h + 0.5F;

		RVt_pos_clr_tex1 Vertex;
		Vertex.vPos		= m_vecVertex[i];
		Vertex.u		= fU;
		Vertex.v		= fV;
		Vertex.dwColor	= D3DCOLOR_ARGB( 255, 255, 255, 255 ); //m_arrColor[i];
		m_vecTempVertex.push_back(Vertex);
	}

	// Index를 사용하지 않기 때문에 공유하는 정점 없이 Triangle * 3 개수 만큼 버퍼에 담는다.
	m_nCountVertex = m_nCountTriangle * 3;
	for (int i = 0; i < m_nCountTriangle * 3; ++i)
	{
		int nVertexIndex = m_vecIndex[i];
		m_vecDecalVertex.push_back(m_vecTempVertex[nVertexIndex]);
	}
}

void RDecalPolygonSceneNode::ClipMesh(int nFaceCount, std::vector<RVt_pos>& arrVertex, RVector* arrNormal)
{
	RPFC_THISFUNC;

	RVector3		vNewVertex[9];
	RVector3		vNewNormal[9];
	bool			vIsClip[9] = {false, };

	// Clip one triangle at a time
	for (int i = 0; i < nFaceCount; ++i)
	{
		int i1 = i * 3 + 0;
		int i2 = i * 3 + 1;
		int i3 = i * 3 + 2;

		const RVector3& v1 = arrVertex[i1].vPos;
		const RVector3& v2 = arrVertex[i2].vPos;
		const RVector3& v3 = arrVertex[i3].vPos;

		RVector3 cross = (v2 - v1).CrossProduct(v3 - v1);
		if ( m_vDecalNormal.DotProduct(cross) > m_fDecalEpsilon * cross.Length() )
		{
			vNewVertex[0] = v1;
			vNewVertex[1] = v2;
			vNewVertex[2] = v3;

			RVector vFaceNormal = cross.Normalize();	// Vertex Normal로 개선하자
			vNewNormal[0] = vFaceNormal;
			vNewNormal[1] = vFaceNormal;
			vNewNormal[2] = vFaceNormal;

			int nCount = ClipPolygon(3, vNewVertex, vNewNormal, vNewVertex, vNewNormal, vIsClip);
			if ((nCount != 0) && (!AddPolygon(nCount, vNewVertex, vNewNormal, vIsClip))) break;
		}
	}
}

int RDecalPolygonSceneNode::ClipPolygon(int nVertexCount, RVector3 *vVertex, RVector3 *vNormal, RVector3 *vNewVertex, RVector3 *vNewNormal, bool *vIsClip)
{
	RPFC_THISFUNC;

	RVector3		vTempVertex[9];
	RVector3		vTempNormal[9];

	// Clip against all six planes
	int nCount = ClipPolygonAgainstPlane(m_vLeftPlane, nVertexCount, vVertex, vNormal, vTempVertex, vTempNormal, vIsClip);
	if (nCount == 0) return nCount;
	
	nCount = ClipPolygonAgainstPlane(m_vRightPlane, nCount, vTempVertex, vTempNormal, vNewVertex, vNewNormal, vIsClip);
	if (nCount == 0) return nCount;
		
	nCount = ClipPolygonAgainstPlane(m_vBottomPlane, nCount, vNewVertex, vNewNormal, vTempVertex, vTempNormal, NULL);
	if (nCount == 0) return nCount;
			
	nCount = ClipPolygonAgainstPlane(m_vTopPlane, nCount, vTempVertex, vTempNormal, vNewVertex, vNewNormal, NULL);
	if (nCount == 0) return nCount;

	nCount = ClipPolygonAgainstPlane(m_vBackPlane, nCount, vNewVertex, vNewNormal, vTempVertex, vTempNormal, NULL);
	if (nCount == 0) return nCount;
	
	nCount = ClipPolygonAgainstPlane(m_vFrontPlane, nCount, vTempVertex, vTempNormal, vNewVertex, vNewNormal, NULL);
	return nCount;
}

int RDecalPolygonSceneNode::ClipPolygonAgainstPlane(RVector4& plane, int nVertexCount, RVector3 *vVertex, RVector3 *vNormal, RVector3 *vNewVertex, RVector3 *vNewNormal, bool *bIsClip)
{
	RPFC_THISFUNC;

	bool	negative[10];

	// Classify vertices
	int negativeCount = 0;
	for (int i = 0; i < nVertexCount; ++i)
	{
		RVector4 vec4 = RVector4(vVertex[i].x, vVertex[i].y, vVertex[i].z, 1);
		bool neg = (plane.DotProduct(vec4) < 0.0F);
		negative[i] = neg;
		negativeCount += neg;
	}

	// Discard this polygon if it's completely culled
	if (negativeCount == nVertexCount) return (0);

	int nCount = 0;
	for (int i = 0; i < nVertexCount; ++i)
	{
		// j is the index of the previous vVertex
		int j = (i != 0) ? i - 1 : nVertexCount - 1;

		if (negative[i])
		{
			if (!negative[j])
			{
				// Current vVertex is on negative side of plane,
				// but previous Vertex is on positive side.
				const RVector3& v1 = vVertex[j];
				const RVector3& v2 = vVertex[i];
				float t = plane.DotProduct(RVector4(v1, 1)) / (plane.x * (v1.x - v2.x) + plane.y * (v1.y - v2.y) + plane.z * (v1.z - v2.z));
				vNewVertex[nCount] = v1 * (1.0F - t) + v2 * t;
				
				const RVector3& n1 = vNormal[j];
				const RVector3& n2 = vNormal[i];
				vNewNormal[nCount] = n1 * (1.0F - t) + n2 * t;
				++nCount;
			}
		}
		else
		{
			if (negative[j])
			{
				// Current vVertex is on positive side of plane,
				// but previous Vertex is on negative side.
				const RVector3& v1 = vVertex[i];
				const RVector3& v2 = vVertex[j];
				float t = plane.DotProduct(RVector4(v1, 1)) / (plane.x * (v1.x - v2.x) + plane.y * (v1.y - v2.y) + plane.z * (v1.z - v2.z));
				vNewVertex[nCount] = v1 * (1.0F - t) + v2 * t;

				const RVector3& n1 = vNormal[i];
				const RVector3& n2 = vNormal[j];
				vNewNormal[nCount] = n1 * (1.0F - t) + n2 * t;
				++nCount;
			}

			// Include current vVertex
			vNewVertex[nCount] = vVertex[i];
			vNewNormal[nCount] = vNormal[i];
			++nCount;
		}
	}

	// Return number of vertices in clipped polygon
	return (nCount);
}

bool RDecalPolygonSceneNode::AddPolygon(int nVertexCount, RVector3 *vVertex, RVector3 *vNormal, bool *bIsClip)
{
	RPFC_THISFUNC;

	// Add polygon as a triangle fan
	m_nCountTriangle += nVertexCount - 2;
	for (int i = 2; i < nVertexCount; ++i)
	{
		m_vecIndex.push_back((unsigned short) m_nCountVertex);
		m_vecIndex.push_back((unsigned short) (m_nCountVertex + i - 1));
		m_vecIndex.push_back((unsigned short) (m_nCountVertex + i));
	}

	// Assign Vertex colors
	for (int i = 0; i < nVertexCount; ++i)
	{
		m_vecVertex.push_back(vVertex[i]);

		// 깊이에 따라 점점 옅어지는거 할려다가 어려워서 때려친 잔재 코드

		//const RVector3& n = vNormal[i];
		//float fAlpha = (m_vDecalNormal.DotProduct(n) / n.Length() - m_fDecalEpsilon) * f;
		//m_arrColor[nCount] = D3DCOLOR_ARGB( 255, 255, 255, 255 );
		//m_arrColor[nCount] = D3DCOLOR_ARGB( (bIsClip[nCount] == true) ? 50 : 255, 255, 255, 255 );

		++m_nCountVertex;
	}

	return (true);
}

void RDecalPolygonRenderSceneNode::SetRenderState( RCameraSceneNode* pCurrentCamera)
{
	// PolygonDecal 에서는 사용하지 않는다. 기본값으로 넘겨준다.
	RShaderCompositeManager::m_cDecalCenterPos.SetVector4(RVector4(0.f, 0.f, 0.f, 10000.f));
	RShaderCompositeManager::m_cTextureFactor.SetVector4( m_vTFactor );
	RShaderCompositeManager::m_cVisiblity.SetFloat(GetVisibility());
}

void RDecalPolygonRenderSceneNode::OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod)
{
	SetRenderState(pCurrentCamera);

	_ASSERT( m_nOffsetEnd % 3 == 0 );
	_ASSERT( m_nOffsetDeletedEnd % 3 == 0 );
	_ASSERT( (m_nVertexSize - m_nOffsetStart) % 3 == 0 );
	_ASSERT( (m_nVertexSize - m_nOffsetDeletedStart) % 3 == 0 );

	REngine::GetDevicePtr()->SetVertexBuffer(m_hVertexBuffer, 0, 0);
	REngine::GetDevicePtr()->SetTexture(AS_DECALMAP, m_pTexture);
	REngine::GetDevicePtr()->SetTextureFilter(AS_DECALMAP, RTF_BILINEAR);
	REngine::GetDevicePtr()->SetTextureAddress(AS_DECALMAP, RTADDRESS_BORDER, RTADDRESS_BORDER, RTADDRESS_BORDER);

	RSCID rscComponent = RSC_POSITION | RSC_TRANSFORM_CONSTANT | RSC_TRANSFORM | REngine::GetSceneManager().GetShaderCompositeManager()->GetVertexTexcoordComponent() | 
		RSC_DECAL | RSC_VCOLOR | RSC_PPL_ALPHA_FROM_VCOLOR_A | RSC_PPL_FOG;
	RShaderFX* pShader = REngine::GetSceneManager().GetShaderCompositeManager()->GetShader(rscComponent, false);
	if( pShader )
	{
		RTechnique Technique = pShader->GetTechniqueByIndex(0);

		Technique.Begin(NULL, TECH_PRESERVE_NOT);
		Technique.BeginPass();

		// Batch Render
		if( m_nOffsetEnd <= m_nOffsetStart )
			REngine::GetDevicePtr()->DrawPrimitive( RPT_TRIANGLELIST, m_nOffsetStart, (m_nVertexSize - m_nOffsetStart) / 3);
		REngine::GetDevicePtr()->DrawPrimitive( RPT_TRIANGLELIST, 0, m_nOffsetEnd / 3);

		Technique.EndPass();
		Technique.End();
	}

	/*
	// Deleted Render // 반투명이므로 뒤에 그린다.
	REngine::GetDevicePtr()->SetVertexBuffer(m_hDeletedVertexBuffer, 0, 0);
	if( m_nOffsetDeletedEnd <= m_nOffsetDeletedStart )
		REngine::GetDevicePtr()->DrawPrimitive( RPT_TRIANGLELIST, m_nOffsetDeletedStart, (m_nMaxSizeBuffer - m_nOffsetDeletedStart) / 3);
	REngine::GetDevicePtr()->DrawPrimitive( RPT_TRIANGLELIST, 0, m_nOffsetDeletedEnd / 3);
	*/
}

} //namespace rs3
