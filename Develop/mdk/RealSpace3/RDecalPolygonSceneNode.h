/* Copyright (C) Eric Lengyel, 2001. 
* All rights reserved worldwide.
*
* This software is provided "as is" without express or implied
* warranties. You may freely copy and compile this source into
* applications you distribute provided that the copyright text
* below is included in the resulting source code, for example:
* "Portions Copyright (C) Eric Lengyel, 2001"
*/

#pragma once

#include "RDecalCollision.h"
#include "RSceneNode.h"
#include <deque>

namespace rs3 {

// RTexture 별로 분류해놓은 Batch Render Set
	class RDecalPolygonRenderSceneNode : public RSceneNode
{
public:
	std::deque<int>				m_deqDecalPolygon;	///< 이 재질에서의 Render DecalPolygon List (최적화를 위해 VertexCount만 가진다.)
	// 하나의 데칼 객체에 대해 정점을 수집한 AABB 데이터를 따로 관리한다.
	// 매번 AABB 구축을 하되 Lock 도 걸 필요 없고 AABB를 합하는 연산만 하면 되므로 비용이 적다.
	std::deque<RBoundingBox>	m_deqDecalAABB;
	
	RTexture*					m_pTexture;

	// Batch Render --------------------------------------------------------
	// End가 Start보다 작으면 두번 그린다.
	//  1. 0 to end
	//  2. start to max
	// 아니면 한번 그린다.
	//  1. 0 to end
	// ---------------------------------------------------------------------
	int		m_nOffsetStart;
	int		m_nOffsetEnd;
	int		m_nOffsetFree;
	HVERTEXBUFFER					m_hVertexBuffer;

	// 
	int		m_nOffsetDeletedStart;
	int		m_nOffsetDeletedEnd;
	int		m_nOffsetDeletedFree;
	HVERTEXBUFFER					m_hDeletedVertexBuffer;	// 삭제중인 폴리곤을 그릴 버퍼
	std::vector<RVt_pos_clr_tex1>	m_vecDeletedVertices;	// 아직 작업하지 않음

	int		m_nVertexSize;
	DWORD	m_dwBlendTime;		///< 삭제시 블랜딩 될 시간
	int		m_nPriority;		///< Overlay 높은 값일수록 위에 덧그려진다.

	RDecalPolygonRenderSceneNode::RDecalPolygonRenderSceneNode(int nPriority, DWORD dwBlendTime, int nVertexSize = 9999 );
	RDecalPolygonRenderSceneNode::~RDecalPolygonRenderSceneNode();

	void Create();
	void AddDecal(std::vector<RVt_pos_clr_tex1>& vecVertex, RTexture* pTexture = NULL);
	void UpdateAABB(std::vector<RVt_pos_clr_tex1>& vecVertex);

	virtual void	SetRenderState( RCameraSceneNode* pCurrentCamera);
	virtual void	OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod) override;

	RVector4		m_vTFactor;					///< 아직 지원안됨. 언제 설정하게 하지..
	// D3DCOLOR_ARGB(255, 255, 255, 255)등으로 설정한다
	void			SetColor(DWORD dwColor) { 
		DWORD2RGBA(dwColor, m_vTFactor.r, m_vTFactor.g, m_vTFactor.b, m_vTFactor.a); 
		m_fVisibility = m_vTFactor.a;
	}
	void			SetColor(RVector4 vColor) { m_vTFactor = vColor; }
};

class RS_API RDecalPolygonSceneNode : public RSceneNode
{
	// RenderSet 전체 관리.
	std::map<std::pair<int, RTexture*>, RDecalPolygonRenderSceneNode*>	m_mapDecalPolygonRenderSet;

protected:
	RVector3					m_vPos;						///<
	float						m_fDepth;
	DWORD						m_dwFlags;					///< 적용될 영역에 대한 Flag

	std::vector<RVt_pos>		m_vecVertexTerrain;			///< Terrain 에서 얻어오는 정점들.
	std::vector<RVt_pos>		m_vecVertexActor;			///< cl2 충돌 메쉬에서 얻어오는 정점들.

	RCollisionTree*				m_pCollisionTree;			///< cl2
	RDecalCollision				m_DecalCollision;

public:
	// Clip Polygon
	float			m_fDecalEpsilon;

	RVector3		m_vDecalCenter;
	RVector3		m_vDecalNormal;

	RVector4		m_vLeftPlane;
	RVector4		m_vRightPlane;
	RVector4		m_vBottomPlane;
	RVector4		m_vTopPlane;
	RVector4		m_vFrontPlane;
	RVector4		m_vBackPlane;

	int				m_nCountVertex;
	int				m_nCountTriangle;

	// 아래 배열은 모두 DecalPolygon 생성용으로만 사용된다.
	std::vector<RVector3>			m_vecVertex;
	std::vector<WORD>				m_vecIndex;
	std::vector<RVt_pos_clr_tex1>	m_vecTempVertex;
	std::vector<RVt_pos_clr_tex1>	m_vecDecalVertex;

public:
	RDecalPolygonSceneNode();
	~RDecalPolygonSceneNode();

	bool			Create(rs3::RCollisionTree* pCollisionTree);
	void			Destroy();

	void			CreatePolygon(std::vector<RVt_pos>& vecVertex, RVector3& center, RVector3& normal, RVector3& tangent, float width, float height, float depth);
	bool			AddPolygon(int vertexCount, RVector3* vertex, RVector3* normal, bool *vIsClip);
	void			ClipMesh(int nFaceCount, std::vector<RVt_pos>& vecVertex, RVector3* vecNormal);
	int				ClipPolygon(int vertexCount, RVector3* vecVertex, RVector3* vecNormal, RVector3* newVertex, RVector3 *newNormal, bool *vIsClip);
	static int		ClipPolygonAgainstPlane(RVector4& plane, int vertexCount, RVector3* vecVertex, RVector3* vecNormal, RVector3* newVertex, RVector3* newNormal, bool *vIsClip);

	// nPriority는 클수록 위에 덮여 그린다.
	void			Add(string strTexName, RVector3 vPos, float fHalfSizeX, float fHalfSizeY, float fHalfSizeZ, float fRotateZ = 0.0f, int nPriority = 0, int nVertexBufferSize = 9999, DWORD dwBlendTime = 3000, DWORD dwFlag = DECALFLAG_TERRAIN | DECALFLAG_ACTOR);
	void			DeleteAll(string strTexName);	///< 해당 재질의 RenderSet 항목 모두를 삭제한다.

	bool			GetVertexFromBound(RBoundingBox aabb);
	void			IntersectionTriangle(std::vector<RVt_pos>& vecVertex, float& fPickDistanceSq, int& nTriIndex);

	float			m_fBias;
	float			m_fSlopeBias;
	void			SetDepthBias(float fBias, float fSlopeBias) {
		m_fBias = fBias;
		m_fSlopeBias = fSlopeBias;
	}

	bool			m_bWire;
	void			IsRenderWire(bool bWire) { m_bWire = bWire; }

	void			SetOverlayPriority(string strTexName, int nPriority);
};

} //namespace rs3
