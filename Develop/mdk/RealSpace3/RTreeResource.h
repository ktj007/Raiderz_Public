#pragma once

#pragma push_macro ("new")
#ifdef new
#undef new
#endif
#include "Dependencies/speedtree/SpeedTreeRT.h"
#pragma pop_macro ("new")

#include "RResource.h"
#include "RTreeBillboard.h"
#include "RTreeMaterial.h"
#include "RTreeMeshBuffer.h"
#include "RTreeMeshFormat.h"

namespace rs3 {

class RTreeSceneNode;
class RTreeResourceMgr;
class RTreePassRenderer;
class RTreeCollision;
class RTreeCollisionObject;

class RSceneManager;

enum RTreeCollisionType;

class RS_API RTreeResource : public RResource, public RResource::LoadingController
{
	friend class RTreeSceneNode;
	friend class RTreeResourceMgr;
	friend class RTreePassRenderer;
	friend class RTreeCollision;

public:
	RTreeResource(const char* _pSzResourceName, RSceneManager* pSceneManager);
	virtual ~RTreeResource();
	DECLARE_USING_RES_MGR(RTreeResourceMgr);

	enum TREE_TEXTRUE
	{
		SPT_BRAHCH = 0,
		SPT_BRANCH_DETAIL = 1,
		SPT_SELF_SHADOW = 2,
		SPT_COMPOSITE = 3,
		SPT_BILLBOARD = 4,
		SPT_BILLBOARD_NORMAL = 5,

		SPT_TEXTURE_NUM
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 스피드 트리 생성값 설정
	//
	void SetSpeedTreeCreationInfo( int nSeed = 1, float fSize = -1, float fSizeVariance = -1 )
	{
		m_nSeed = nSeed;
		m_fSize = fSize;
		m_fSizeVariance = fSizeVariance;
	}

protected:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 리소스 - Fill / Load
	//
	virtual bool		Fill() override;
	virtual LOAD_RESULT Load() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Tree Texture
	//
	virtual int						CalcPriority();	///< 하위 텍스쳐 로딩 우선순위를 위한 것.
	bool							LoadTextures();
	bool							IsNormalMap( TREE_TEXTRUE eTexture );
	static const char*				GetTextureLayerName ( TREE_TEXTRUE eTexture );
	string							m_strTextureNames[ SPT_TEXTURE_NUM ];
	RTexture*						m_arrayTreeTexture[ SPT_TEXTURE_NUM ];
	const char*						GetTextureFileName(const char* filename);

	static void						CreateInstancingBuffer();

private:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Creation Info
	//
	int								m_nSeed;
	float							m_fSize;
	float							m_fSizeVariance;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Speed Tree Object
	//
	CSpeedTreeRT*						m_pSpeedTree;				// SpeedTree object
	CSpeedTreeRT::SMapBank*				m_pTextureInfo;				// cached texture info
	CSpeedTreeRT::SGeometry*			m_pGeometryCache;			// geometry cache
	CSpeedTreeRT::SLightShaderParams	m_LightParam;				// light param

	float							GetLightGlobal()	{ return m_LightParam.m_fGlobalLightScalar; }	// light factor getter
	float							GetLightBranch()	{ return m_LightParam.m_fBranchLightScalar; }
	float							GetLightFrond()		{ return m_LightParam.m_fFrondLightScalar; }
	float							GetLightLeaf()		{ return m_LightParam.m_fLeafLightScalar; }
	float							GetLightAmbient()	{ return m_LightParam.m_fAmbientScalar; }

	float							GetLeafLightingAdjustment() { return m_pSpeedTree->GetLeafLightingAdjustment(); }

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// tree material
	//
	RTreeMaterial					m_BranchMaterial;
	RTreeMaterial					m_FrondMaterial;
	RTreeMaterial					m_LeafMaterial;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render Chain for batching rendering
	//
	RTreeSceneNode*					m_pFirstRenderChain;
	RTreeSceneNode*					m_pLastRenderChain;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Bounding box and collision
	//
	float							m_afBoundingBox[6];
	RBoundingBox					m_Box;
	RTreeCollision*					m_pCollisionInfo;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// scene manager for resource creation / etc
	//
	RSceneManager*					m_pSceneManager;
	std::vector< std::string >		m_vErrorMessages;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 
	//
	int								m_unBranchVertexCount;
	int								m_unFrondVertexCount;

	float							m_fLeafRockScalar;          // one rock/rustle table is uploaded for all trees, these values help
	float							m_fLeafRustleScalar;        // scale the table up or down to how the tree was tuned in CAD

	SSpeedTreeBillboard				m_cBillboardVertices;       // used as a template for updating billboards of this t
	int								m_nBillboardNum;
	vector<float>					m_vTexCoord360;

	RTreeMeshBuffer<SFVFBranchVertex_GPU_Light>*	m_pBranchBuffer;
	RTreeMeshBuffer<SFVFFrondVertex_GPU_Light>*		m_pFrondBuffer;
	RTreeMeshBuffer<SFVFLeafVertex_GPU_Light>*		m_pLeafCardBuffer;
	RTreeMeshBuffer<SFVFLeafMeshVertex_GPU_Light>*	m_pLeafMeshBuffer;

	static RTreeInstancingBuffer					m_InstancingBuffer;

	bool							FillBranchBuffer(CSpeedWind* wind);					// fill
	bool							FillFrondBuffer(CSpeedWind* wind);
	bool							FillLeafCardBuffer(CSpeedWind* wind);
	bool							FillLeafMeshBuffer(CSpeedWind* wind);
	bool							FillBillboardBuffer();

	bool							LoadBranchBuffer();									// load
	bool							LoadFrondBuffer();
	bool							LoadLeafCardBuffer();
	bool							LoadLeafMeshBuffer();

	bool							SetupForRenderBranch();								// setup for rendering
	bool							SetupForRenderFrond();
	bool							SetupForRenderLeafCard();
	bool							SetupForRenderLeafMesh();
	bool							SetupForRenderBillboard();
	static bool						SetupForRenderInstancing( RTreeInstancingBuffer* pTreeInstancingBuffer, UINT nCnt);
	static bool						RestoreForRenderInstancing();

	int								RenderBranch(int nBranchLodLevel);
	int								RenderBranchInstancing( RTreeInstancingBuffer* pTreeInstancingBuffer, int nCnt);
	int								RenderFrond(int nFrondLodLevel);
	int								RenderFrondInstancing( RTreeInstancingBuffer* pTreeInstancingBuffer, int nCnt);
	int								RenderLeafCard(int nLeafLodLevel);
	int								RenderLeafMesh(int nLeafLodLevel);
	SSpeedTreeBillboard::SVertex*	RenderBillboard(const float* pPos, float fAzimuth, float fFade, float fScale);	// getting billboard vertices ( don't dp call )

public:
	RTreeCollisionObject*	GetTreeCollisionInfo(RTreeCollisionType type);
};

inline bool RTreeResource::IsNormalMap( TREE_TEXTRUE eTexture )
{
	if ( eTexture==SPT_BILLBOARD_NORMAL) return true;
	return false;
}

}
