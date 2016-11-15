#pragma once

#include <vector>
#include <string>
#include "MBox.h"
#include "RVType.h"
#include "RUtil.h"
#include "RCollision.h"

namespace rs3 {

class RCollisionTree;
class RTexture;
class RCollisionTree;
class RDecalCollision;

#define DECALFLAG_TERRAIN		0x001
//#define DECALFLAG_STATICMESH	0x002	// 없어졌음
#define DECALFLAG_ACTOR			0x008	// 캐릭터를 제외한 Actor (cl2에 포함되는 데이터)

class RDecalCollision : public RCollision
{
protected:
	virtual bool			IsCollidable(const MBox &box);
	virtual bool			CheckPolygon(const MVector3 &lv0,const MVector3 &lv1,const MVector3 &lv2, const DWORD dwAttribute = 0);
	virtual bool			CheckTangency(const MVector3& vPos, const RCollisionPolygon& poly);	// 이 폴리곤과 접하는가?

	virtual void			GetBoundingBox(MBox &outBox);
	virtual float			GetWalkableDistance(const RCollisionPolygon &collisionPolygon);
	virtual MVector3		GetContactPoint(const MVector3& vPos, const MVector3& vNorm);

public:
	RDecalCollision();
	~RDecalCollision();

	RBoundingBox			m_aabb;
	std::vector<RVt_pos>*	m_vecVertex;

	void					SetAABB(RBoundingBox& rbb);
	void					SetVertex(std::vector<RVt_pos>* vecVertex);
	void					CalcBoundingBox();		// 이번 움직임 전체의 바운딩박스를 구한다
};

class RS_API RDecalSceneNode : public RSceneNode
{
protected:
	RVector3					m_vPos;						///<
	RVector4					m_vTFactor;					///<
	float						m_fDepth;
	DWORD						m_dwFlags;					///< 적용될 영역에 대한 Flag

	std::vector<RVt_pos>		m_vecVertexTerrain;			///< Terrain 에서 얻어오는 정점들.
	std::vector<RVt_pos>		m_vecVertexActor;			///< cl2 충돌 메쉬에서 얻어오는 정점들.

	RCollisionTree*				m_pCollisionTree;			///< cl2
	RDecalCollision				m_DecalCollision;

	virtual void	OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod) override {};
	// dwFlag는 DECALFLAG로 적용시킬 영역에 대한 설정값이다 DECALFLAG_TERRAIN | DECALFLAG_ACTOR 이런식으로 중첩해서 사용가능
	virtual bool	Create(string strTexName, RVector3 vPos, float fRadiusX, float fRadiusY, float fRadiusZ, float fRotateZ = 0.0f, DWORD dwFlag = DECALFLAG_TERRAIN | DECALFLAG_ACTOR, rs3::RCollisionTree* pCollisionTree = NULL);
	virtual void	Destroy();

public:
	RDecalSceneNode();
	virtual ~RDecalSceneNode();

	virtual void	SetRenderState( RCameraSceneNode* pCurrentCamera);
	// D3DCOLOR_ARGB(255, 255, 255, 255)등으로 설정한다
	void			SetColor(DWORD dwColor) { 
		DWORD2RGBA(dwColor, m_vTFactor.r, m_vTFactor.g, m_vTFactor.b, m_vTFactor.a); 
		m_fVisibility = m_vTFactor.a;
	}
	void			SetColor(RVector4 vColor) { m_vTFactor = vColor; }
	void			GetVertexFromBound();	// 폴리곤 수집

	float			m_fRotate;					///< 회전
	void			SetRotate(float fAxisZ) { m_fRotate = fAxisZ; }

	vec3			GetPosition() { return m_vPos; }
};

} //namespace rs3
