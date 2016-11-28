#pragma once

#include "RDevice.h"
#include "RDeviceD3D.h"
#include "RTypes.h"
#include "RSceneNode.h"
#include "RTypes.h"
#include "RViewFrustum.h"
#include "RVType.h"

#include "RTreeMeshBuffer.h"
#include "RHeightField.h"
#include "REngine.h"
#include "RTerrainResource.h"

#include <string>
#include <vector>
#include <deque>

class MXml;
class MFileSystem;
class Horizon;
//////////////////////////////////////////////////////////////////////////
// * 함수 호출 순서가 종속적
// CreateLayerNum -> LoadDisplacementmapFromFile -> LoadAlphaMapFromFile -> LoadTileMapFromFile

namespace rs3 {

RS_API extern const char *RSID_TERRAINSCENENODE;

class RTreeSystem;
class RTerrainPatch;
class RTerrainQuadTree;
class RTerrainPassRenderer;
class RDeviceD3D;
class RTechnique;
class RDirectionalLightSceneNode;
class RCameraSceneNode;
class RTerrainPVS;
class RGrassSystem;
class RShaderCompositeManager;
class RTerrainShader;
class RListener;

#define MAX_LAYER_PER_PATCH	7
#define MAX_LAYER_BY_ONE_MASK	5

// Samplers
enum TERRAIN_SAMPLER 
{
	TS_DIFFUSE		= 0,
	TS_TILEMAP		= 0,
	TS_ALPHAMAP		= 1,
	TS_NORMALMAP	= 2,
	TS_MASKMAP0		= 0,
	TS_MASKMAP1		= 1,
	TS_DIFFUSEMAP0	= 2,
	TS_DIFFUSEMAP1	= 3,
	TS_DIFFUSEMAP2	= 4,
	TS_DIFFUSEMAP3	= 5,
	TS_DIFFUSEMAP4	= 6,
	TS_DIFFUSEMAP5	= 7,
	TS_DIFFUSEMAP6	= 8,
	TS_NORMALMAP0	= 9,
	TS_NORMALMAP1	= 10,
	TS_NORMALMAP2	= 11,
	TS_NORMALMAP3	= 12,
	TS_NORMALMAP4	= 13,
	TS_NORMALMAP5	= 14,
	TS_NORMALMAP6	= 15,
	TS_VIEWSPACE_LIGHTMAP	= 11
};


enum TERRAIN_TILE_LEVEL
{
	TTL_SIMPLE		= 0,
	TTL_DETAIL_HIGH,
	TTL_DETAIL_LOW,
	TTL_COUNT
};


class RS_API RTerrain : public RSceneNode, public RTerrainResource, public RResource::LoadingController
{
	friend RTerrainPassRenderer;

	MDeclareRTTI;
//	DECLARE_USING_PASS(RRP_TERRAIN);
	DECLARE_NODE_ID_STRING(RSID_TERRAINSCENENODE);

public:
	static const char* BAKED_MAP_FILENAME;

	RTerrain();
	virtual ~RTerrain();

	virtual int		CalcPriority();
	virtual void	OnLoadingComplete(RResource*);

	// element의 Child에 반드시 RTERRAIN이 있어야 ..
	virtual bool CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
	virtual void SaveToXML(MXmlElement &element);
	void SetPathAndResourceName();

	bool LoadFromXMLElement(MXmlElement *pRoot, bool bBackgroundCreation = false);
	void SaveToElement(MXmlElement *pElement);		// ( pRoot의 이름은 RTERRAIN )

	virtual void OnRenderShadowMap( RCameraSceneNode* pCurrentCamera, RShadowStrategy* pShadowStrategy ) override;

	int RenderTerrainPatch(RTerrainPatch *pPatch);

	//// 컬링된 패치를 렌더링한다.
	void RenderCullRawMap();
	//// Culling 한다.


	void RenderGStageInferred( RCameraSceneNode* pCurrentCamera);
	void RenderGStageDeferred( RCameraSceneNode* pCurrentCamera);

	virtual void RenderGStage( RRENDERINGMETHOD renderMethod, RCameraSceneNode* pCurrentCamera);
	virtual void RenderMStage( RCameraSceneNode* pCurrentCamera, RRENDERINGMETHOD renderMethod);
	virtual void RenderZStage( RRENDERINGMETHOD renderMethod, RCameraSceneNode* pCurrentCamera);

	void UpdateDirectionalLight(RDirectionalLightSceneNode *pLight);

	virtual bool OnPick(RPICKINFO& pickInfo);
	virtual bool GetNormalVector( RVector* pPos, RVector* pOut);

	virtual bool IsLoadingCompleted() override;

	virtual void OnUpdate();
	virtual void OnUpdateAABB() override;

	virtual void OnAddScene();
	virtual void OnRemoveScene();

	DETAIL_LEVEL GetRenderDetailMap()				{	return m_eDetailLevel; }
	void SetRenderDetailMap(DETAIL_LEVEL eLevel)	{	m_eDetailLevel = eLevel; }

	bool GetPickVertical(RVector &rOut, RVector *pNormal, float x, float y);
	bool GetPickVerticalWithHole(RVector &rOut, RVector *pNormal, float x, float y);	//Hole인 지역도 높이를 가져온다
	bool GetPickLine(RVector &rOut, RVector *pNormal, const RVector &p, const RVector &q);
	virtual int GetVertexFromBound(std::vector<RVt_pos> &vecVertex, const RBoundingBox &aabb) override;

	// baked map
	bool CreateBakedMap();
	bool LoadBakedMap();
	RTexture* GetBakedMap() { return m_hBaseMap; }

	// navi mesh 생성유무
	void	setCreateNaviMesh(bool	b)	{	m_bCreateNaviMesh = b;		}
	bool	getCreateNaviMesh(void)		{	return m_bCreateNaviMesh;	}

	// NormalMap File(TextureName + '_nm') 검증
	string  GetNormalMapFileName(string strTileFileName);

	// LOD
	virtual bool CalculateLOD(RCameraSceneNode* pCamera, const RSphere* pBoundingSphere) override;

	static void SetDetailDistance( float f)	{ 
		m_fDetailDistance = f; 
		m_fDetailDistanceSq = m_fDetailDistance * m_fDetailDistance;
		m_fDetailDistanceHalf = m_fDetailDistance/2; 
		m_fDetailDistanceHalfSq = m_fDetailDistanceHalf * m_fDetailDistanceHalf;
	}
	static float GetDetailDistance()		{ return m_fDetailDistance;}
	static float GetDetailDistanceSq()		{ return m_fDetailDistanceSq;}

private:
	bool	m_bCreateNaviMesh;	// 네비게이션 메쉬를 생성하냐 ,안하냐
	// 디테일 거리를 상수로 고정하지 않고 조절 가능하게 변수로 제공하자.
	static float m_fDetailDistance;
	static float m_fDetailDistanceSq;
	static float m_fDetailDistanceHalf;
	static float m_fDetailDistanceHalfSq;

	RListener*	m_pEventListener;
		
protected:
	// LOD function
	void CalcLODLevel(const RVector *pCameraPos, int nX, int nY, int nZ);
	inline TERRAIN_TILE_LEVEL CalcDetail(const RVector *pCameraPos, const RBoundingSphere *pSphere);
	
	void Cull(RCameraSceneNode* pCamera,const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar);
	void CullTree(RCameraSceneNode* pCamera,RTerrainQuadTree *pNode, const RVector* pCameraPos, const RPlane* pCameraNearPlane, const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar);
	virtual bool VisibilityTestAABB(const RBox* pAABB, const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar);

	void UpdateAABB();

protected:
	// detail
	DETAIL_LEVEL m_eDetailLevel;

	RVector m_vLightDir;
	RVector4 m_vAmbient;
	RVector4 m_vDiffuse;

	/// 툴에서의 랜더 용도들
	bool m_bGridFlag;			
	int	m_nMarkingLayerNumber;

	void UpdateDirectionalLight(const RVector &vDirection, const RVector4 &vDiffuse, const RVector4 &vAmbient, float fPower, float fSize);

	void SetRenderState( RCameraSceneNode* pCurrentCamera, const RMatrix &viewMatrix, const RMatrix &projMatrix, RTerrainShader* pTerrainShader = NULL);
	void RenderSingleLayer( RTechnique* pTech, std::vector<RTerrainPatch*>& arrPatch, RTerrainShader* pShader);
	void RenderBakedMapMStage( RCameraSceneNode* pCurrentCamera, bool bLighting, RTerrainShader* pShader);
	void RenderBakedMapGStage( RCameraSceneNode* pCurrentCamera, bool bFullMap, RTerrainShader* pShader);
	void RenderGrid( RCameraSceneNode* pCurrentCamera, RTerrainShader* pShader);
	void RenderMarkingLayer( RCameraSceneNode* pCurrentCamera, RTerrainShader* pShader, int nMarkingLayerNumber);
	/// 다중 패스 랜더링
	void RenderSplattingLayerOnePassMStage( RCameraSceneNode* pCurrentCamera, RTerrainShader* pShader);
	/// 단일 패스 랜더링
	void MakeMaskTexture( int nPatchIdx, RTerrainShader* pTerrainShader);
	void CheckAndCreateMaskTexture( std::vector<RTerrainPatch*>& arrPatch, RTerrainShader* pTerrainShader);
	int GetMaskTextureCount( int nLayerCount);
	void ClearMaskTexture( int nPatch);
	void RenderSplattingLayerOnePassGStage( RCameraSceneNode* pCurrentCamera, RTerrainShader* pTerrainShader);
	void RenderSplattingLayerOnePass( bool bLit, std::vector<RTerrainPatch*>& arrPatch, RTerrainShader* pTerrainShader, int nRenderTargetCount, TERRAIN_TILE_LEVEL tileLevel, int nMarkingLayer);
	void RenderMarkingLayerOnePass( std::vector<RTerrainPatch*>& arrPatch, RTerrainShader* pTerrainShader, int nRenderTargetCount, int nMarkinglayer);
	void SetRenderStateForSplattingLayerOnePass();
	void SetLayerMaskTexture( RTerrainPatch* pDrawPatch, int nLayerCount);
	void SetLayerTileTexture( LayerInfo *pInfo, int nLayerIdx, bool bNormalMap);
	void RestoreRenderState();
	void RenderTillingEffect()	{}
	void RenderShadow();
	void RenderDepthInfo();		// 터레인이 다중 경로로 그리게 되면서 알파의 정보들 때문에... - 090624, OZ
	void SetLightInfo( const RVector& _vLightDir, const RVector4& _vDiffuse, const RVector4& _vAmb);

public:
	void ClearMaskTexture();
	void SetGridFlag(bool bFlag) { m_bGridFlag = bFlag; }
	bool GetGridFlag() { return m_bGridFlag; }
	void SetMarkingLayerNumber(int num) { 
		_ASSERT(num<m_iLayerNum);
		m_nMarkingLayerNumber = num;
	}
	int GetMarkingLayerNumber() { return m_nMarkingLayerNumber; }
};


}
