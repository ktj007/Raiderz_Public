#pragma once

#include "RDecalCollision.h"

namespace rs3 {

// Projection Decal은 항상 Dir이 -Z인 카메라 행렬을 가진다.
// Terrain과 StaticMesh에서 데칼이 그려져야할 영역(AABB) 만큼의 폴리곤을 수집한다.
// ZFighting을 없애기 위해 Shader를 사용해서 그린다.
class RS_API RDecalProjectionSceneNode : public RSceneNode
{
protected:
	RVector3					m_vPos;						///<
	float						m_fDepth;
	DWORD						m_dwFlags;					///< 적용될 영역에 대한 Flag

	std::vector<RVt_pos>		m_vecVertexTerrain;			///< Terrain 에서 얻어오는 정점들.
	std::vector<RVt_pos>		m_vecVertexActor;			///< cl2 충돌 메쉬에서 얻어오는 정점들.

	RCollisionTree*				m_pCollisionTree;			///< cl2
	RDecalCollision				m_DecalCollision;

	RTexture*		m_pTexture;					///< 텍스쳐.
	float			m_fZBias;					///< 직접 설정할 수 있게
	
public:
	RDecalProjectionSceneNode();
	virtual ~RDecalProjectionSceneNode();

	virtual void	SetRenderState( RCameraSceneNode* pCurrentCamera);
	virtual void	OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod) override;

	// Z의 높낮이 값을 직접 설정할 수 있게 한다.
	bool			Create(rs3::RCollisionTree* pCollisionTree, string strTexName, RVector3 vPos, float fHalfSizeX, float fHalfSizeY, float fHalfSizeZ, float fRotateZ = 0.0f, DWORD dwFlag = DECALFLAG_TERRAIN | DECALFLAG_ACTOR );
	void			Update(RVector3 vPos, float fHalfSizeX, float fHalfSizeY, float fHalfSizeZ);
	void			GetVertexFromBound();

	void			UpdateTextureTransform();
	RVector3		GetInteractionScale(const RBoundingBox& rbb) const;
	// 기본값은 -1.f / 65535.f 이고 뒤의 숫자로 정밀도를 조절합니다
	// 뒤의값이 작을 수록 ZBias 가 더 두드러지고 더 많은 Offset을 가지게 됨, 클수록 정밀도가 떨어집니다
	void			SetZBias(float fZBias) { m_fZBias = fZBias; }

	RVector4		m_vTFactor;					///<
	// D3DCOLOR_ARGB(255, 255, 255, 255)등으로 설정한다
	void			SetColor(DWORD dwColor) { 
		DWORD2RGBA(dwColor, m_vTFactor.r, m_vTFactor.g, m_vTFactor.b, m_vTFactor.a); 
		m_fVisibility = m_vTFactor.a;
	}
	void			SetColor(RVector4 vColor) { m_vTFactor = vColor; }

	float			m_fRotate;					///< 회전
	void			SetRotate(float fAxisZ) { m_fRotate = fAxisZ; }
};

} //namespace rs3
