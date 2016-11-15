#pragma once

#include "RTreeDef.h"
#include "RTreeResource.h"
#include "RPassRenderer.h"
#include "RTechnique.h"
#include "RShaderConstant.h"

namespace rs3 {

class RGrassSystem;
class RFont;
class RShaderFX;

class RS_API RTreePassRenderer : public RPassRenderer	/*, public RListener*/
{
	friend class RGrassSystem;		// for m_pSpeedWind
	friend class RTreeResource;		// for m_pSpeedWind
	friend class RTreeSceneNode;	// for OnRegisterToRender

	// 테크닉을 맵으로 관리
	enum TREE_TECH_TYPE {
		TREETECH_GSTAGE = 0,
		TREETECH_MSTAGE,
		TREETECH_DEFERRED,		// 디퍼드의 경우는 G스테이지 뿐이므로 네이밍을 G,M 가르는게 무의미
		TREETECH_SHADOW,
		TREETECH_PROJECTIONSHADOW,
		TREETECH_COUNT
	};

public:
	RTreePassRenderer();
	virtual ~RTreePassRenderer();

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 구현
	//
	DECLARE_PASS_RENDERER_ID( RRP_SPEEDTREE );

	virtual void				Init();
	virtual void				Destroy();
	virtual void				Reset();

	virtual void				RenderDepthOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RShadowStrategy* pShadowStrategy) override;

	virtual void				RenderGStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;
	virtual void				RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;

	virtual void				RenderOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, TREE_TECH_TYPE eMothod);

	virtual bool				IsCurrentShaderRejective(){ return true; }
	virtual void				Update(float timegap);// 초 단위로 지정한 시간 간격이 지났을때로 업데이트합니다.

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// setter / getter for rendering property
	//

	// 화면 상의 Y축 projection size 로 LOD 및 Culling을 세팅한다. 화면의 절반이 1.f
	void						SetTreeLOD(float n, float f);
	float						GetNearLOD(void)	{	return m_fNearLOD;	}
	float						GetFarLOD(void)		{	return m_fFarLOD;	}

	void						SetRenderDisable(bool rd)		{ m_bDoNotRender = rd; }
	bool						ISRenderDisable()				{ return m_bDoNotRender; }
	void						SetRenderCollisionObject(bool b){ m_bDrawCollisionObject = b; }

	int							GetPolygonCount()		{ return (int)m_nPolyCount; }

	void SetUseInstancing( bool b)	{ m_bUseInstancing = b;		}
	bool IsUseInstancing()			{ return m_bUseInstancing;	}
	void ChangeRTCount(int nCount);


private:
	

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// GPU 빌보드 렌더러
	//
	struct STreeBillboardRenderer
	{
		STreeBillboardRenderer();

		RDynamicVertexBuffer<SSpeedTreeBillboard::SVertex>	m_vb;
		int			vertexSize;
		RDevice*	m_pDevice;

		void	Create(int vsz, RDevice* pDevice);
		void	Destroy();
		
		void	AddVertices(SSpeedTreeBillboard::SVertex* data, int itemNum);
		void	ClearVertices();

		int		Render(RTechnique &shaderTechnique);	// render 후 vertices 는 clear 된다.
	};


	const static int			FOG_FAR_GUARD = 3000;

	CSpeedWind*					m_pSpeedWind;					// SpeedWind object that controls wind effects for entire forest
	float						m_fTime;                        // tracks time, used in wind effects (in seconds)
	vector<RMatrix>				m_vWindMatrices;                // vector of float pointers storing wind matrices for each matrix defined by SpeedWind
	vector<float>				m_vWindRustleAngles;            // table of leaf rustle angles shared by all base trees
	vector<float>				m_vWindRockAngles;              // table of leaf rock angles shared by all base trees
	vector<float>				m_vLeafAngleUploadTable;        // used as a repository to batch shader constant uploads
	float						m_fSelfShadowAdvance;           // used to animate the self-shadow texture based on global wind strength

	vector< RTreeResource* >	m_vecTreeResourceToRender;		// 렌더링할 메시

	STreeBillboardRenderer		m_BillboardRenderer;			// 빌보딩 처리된 나무들을 모아서 그리는 클래스	

	RVector						m_LightDirection;
	RVector4					m_LightColor;					// RGBA
	RVector4					m_LightAmbient;					// RGBA

	HVERTEXFORMAT				m_hBranchVertexFormat;
	HVERTEXFORMAT				m_hFrondVertexFormat;
	HVERTEXFORMAT				m_hLeafCardVertexFormat;
	HVERTEXFORMAT				m_hLeafMeshVertexFormat;
	HVERTEXFORMAT				m_hBillboardVertexFormat;

	RShaderFX*					m_pTreeShader;

	TECHNIQUE_MAP				m_mapBranchTechnique;
	TECHNIQUE_MAP				m_mapFrondTechnique;
	TECHNIQUE_MAP				m_mapLeafCardTechnique;
	TECHNIQUE_MAP				m_mapLeafMeshTechnique;
	TECHNIQUE_MAP				m_mapBillboardTechnique;

	RShaderConstant				m_scWorld;
	RShaderConstant				m_scViewProjection;
	RShaderConstant				m_scView;
	RShaderConstant				m_scWindMatrixOffset;
	RShaderConstant				m_scWindMatrices;
	RShaderConstant				m_scCameraAngle;
	RShaderConstant				m_scCameraTrig;
	RShaderConstant				m_scLeafAngles;
	RShaderConstant				m_scLeafAngleScalars;
	RShaderConstant				m_scInstancePosition;
	RShaderConstant				m_scInstanceData;
	RShaderConstant				m_scFogColor;
	RShaderConstant				m_scFogParam;
	RShaderConstant				m_scLightAngle; // 나뭇잎 빌보드 그림자에 사용
	RShaderConstant				m_scRotateYValue;
	RShaderConstant				m_scSpecialAzimuth;
	RShaderConstant				m_sc360Table;
	RShaderConstant				m_scBBNormal;
	RShaderConstant				m_scBBBinormal;
	RShaderConstant				m_scBBTangent;
	RShaderConstant				m_scAlphaRef;
	RShaderConstant				m_scFarDist;
	RShaderConstant				m_scScreenSpaceTransform;
	RShaderConstant				m_scShadowValue;

	bool						m_bDrawCollisionObject;

	bool						m_bDoNotRender;
	size_t						m_nTreeIdx;

	int							m_nPolyCount;

	float						m_fNearLOD;
	float						m_fFarLOD;

	RVector4*					m_pInstancingPosition;
	RVector4*					m_pInstancingData;

	bool						m_bUseInstancing;

	int							m_nMRTCount;



	bool						CreateShader(int nMRTCount);
	void						DestroyShader();

	void						SetTreeInstancingData( int nIdx, RTreeSceneNode* pTreeSceneNode);


	void						ClearChain();

	void						SetRenderState();
	void						RestoreRenderState();


	//////////////////////////////////////////////////////////////////////////////////////////////////
	// Render함수를 분리해 놓은 부분들
	//

	//Culling List사용하는 버전의 함수
	void						RenderBranch( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, TREE_TECH_TYPE eType);
	void						RenderFrond( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, TREE_TECH_TYPE eType);
	void						RenderLeafCard( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, TREE_TECH_TYPE eType);
	void						RenderLeafMesh( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, TREE_TECH_TYPE eType);
	void						RenderBillboard( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, TREE_TECH_TYPE eType);
	void						RenderBillboardedTrees( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, TREE_TECH_TYPE eType);
	void						RenderCollisionObject( RCameraSceneNode* pCurrentCamera,SCENE_SET* pRenderingList);
	void						RenderForMakingLightMap( RCameraSceneNode* pCurrentCamera,SCENE_SET* pRenderingList, const RMatrix& matShadow, bool bProjectionShadow, 
														 const RVector& lightPos = RVector(0,0,0), const float fLightRange = 0);

	void						RenderBranchInstance( RTreeResource* pTreeResource, RTreeSceneNode* pTreeSceneNode, RCameraSceneNode* pCamera);
	void						RenderBranchInstancing( RTreeResource* pTreeResource, RTreeInstancingBuffer* pTreeInstancingBuffer, RCameraSceneNode* pCamera, int nCnt);
	void						RenderFrondInstance( RTreeResource* pTreeResource, RTreeSceneNode* pTreeSceneNode, RCameraSceneNode* pCamera);
	void						RenderFrondInstancing( RTreeResource* pTreeResource, RTreeInstancingBuffer* pTreeInstancingBuffer, RCameraSceneNode* pCamera, int nCnt);
	void						RenderLeafCardInstance( RTreeResource* pTreeResource, RTreeSceneNode* pTreeSceneNode, RCameraSceneNode* pCamera);
	void						RenderLeafCardInstancing( RTreeResource* pTreeResource, RTreeInstancingBuffer* pTreeInstancingBuffer, RCameraSceneNode* pCamera, int nCnt);
	void						RenderLeafMeshInstance( RTreeResource* pTreeResource, RTreeSceneNode* pTreeSceneNode, RCameraSceneNode* pCamera);
	void						RenderLeafMeshInstancing( RTreeResource* pTreeResource, RTreeInstancingBuffer* pTreeInstancingBuffer, RCameraSceneNode* pCamera, int nCnt);

	//////////////////////////////////////////////////////////////////////////
	// SetTexture 류 wrapper
	void						SetCompositeTexture(RTexture* pTexture);
	void						SetBranchTexture(RTexture* pTexture);
	void						SetBranchDetailTexture(RTexture* pTexture);
	void						SetBillboardTexture(RTexture* pTexture);
	void						SetBillboardNormalTexture(RTexture* pTexture);

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// 헬퍼 멤버함수
	//
	void						InitVertexFormats();

	void						SetTreeInstanceConstant( RCameraSceneNode* pCurrentCamera, const RTreeSceneNode* _pTreeSceneNode, float fAlphaRef);
	void						SetCommonTreeInstanceConstant( RCameraSceneNode* pCurrentCamera, bool bUseInstancing, bool bCommit);
	void						SetTreeInstancingConstant( bool bCommit, int nCnt);
	void						UploadShaderConstants( RCameraSceneNode* pCurrentCamera, RSceneManager* pManager, TREE_TECH_TYPE eType);
	void						GetLightParams(RSceneManager* pManager);
};

inline void RTreePassRenderer::SetTreeLOD(float n, float f)
{
 m_fFarLOD = f; 
 m_fNearLOD = n;
}

}

