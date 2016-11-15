#pragma once

#include <vector>

using namespace std;

#include "RTypes.h"
#include "RVType.h"
#include "RSceneNode.h"
#include "RStaticMeshResource.h"

class MFile;

const float RS_LIGHT_SCALE_FACTOR = .25f;
const float RS_LIGHT_INVSCALE_FACTOR = 4.f;

namespace rs3 {

RS_API extern const char *RSID_STATICMESHSCENENODE;

class RMaterial;
class RStaticMeshSceneNode;
class RMaterialResource;
class RShaderCompositeManager;
class REnvironmentSet;
class RStaticMeshPassRenderer;
class RStaticMeshRenderStrategy;
class RStaticMeshDepthRenderStrategy;
class RStaticMeshGStageRenderStrategy;
class RStaticMeshMStageRenderStrategy;

struct RRsRenderHelper {
	WORD m_indicies[INDEX_FLUSH_COUNT];
	int m_nCount;
	int m_nCurMinIndex;
	int m_nCurMaxIndex;
	int m_nMaxIndex;
	int m_nBaseIndex;
	int m_nMaterial;
	int m_nLightmapID;
	RenderInfo* m_pRenderInfo;

	inline void Append(WORD* pIndicies,int nCount,int nMin,int nMax)
	{
		memcpy(&m_indicies[m_nCount],pIndicies,sizeof(WORD)*nCount);
		m_nCount+=nCount;
		m_nCurMaxIndex = max(m_nCurMaxIndex,nMax);
		m_nCurMinIndex = min(m_nCurMinIndex,nMin);
	}

	inline bool isEnoughToInsert(int nCount)
	{
		return m_nCount+nCount<INDEX_FLUSH_COUNT;
	}

	inline void Reset() { m_nCurMaxIndex = m_nCount = 0; m_nCurMinIndex = m_nMaxIndex-1; }
};

struct RSTATICMESHPICKINFO
{
	// picking을 위한 변수들
	float		m_fPickDistanceSq;
	bool		m_bPickFound;
	RVector		m_pickOrigin,m_pickTo,m_pickDir;
	RPICKINFO	*m_pPickInfo;
	bool		m_bBackFaceCull;
};

/// 움직이지 않는 정적 메쉬, 셀의 집합이며 포탈로 연결되어있다
class RS_API RStaticMeshSceneNode : public RSceneNode, public RStaticMeshResource, public RResource::LoadingController
{
	friend RStaticMeshPassRenderer;
	friend RStaticMeshRenderStrategy;
	friend RStaticMeshDepthRenderStrategy;

	MDeclareRTTI;
	DECLARE_NODE_ID_STRING(RSID_STATICMESHSCENENODE);
	
	static RShaderCompositeManager *m_pShaderCompositeManager;
	static REnvironmentSet		*m_pEnvironmentSet;

	RVector3				m_worldLightVec;				///< 영향을 미치는 (전역)광원의 방향
	RVector3				m_worldLightPos;				///< 영향을 미치는 광원의 월드좌표
	RVector3				m_worldLightColor;				///< 영향을 미치는 광원의 색
	RVector3				m_worldLightAmbient;			///< ambient color

	// 렌더링용 변수
	struct RRENDERINFO
	{
		RGeometry*		pGeometry;
		RRsAABBTree*	pAABBTree;
		RRsNode*		pNode;
	};
	vector<RRENDERINFO>		m_renderNodes[2];

	RVector					m_currentCameraPosition;
	int						m_nCurrentCell;
	int						m_nCullingPlanes;
	const RPlane*			m_pCullingPlanes;						///< culling 을 위한 portal 을 이루는 평면
	const RPlane*			m_pCullingNearPlane;
	const RPlane*			m_pCullingFarPlane;
	int						m_nUserCullingPlanes;
	const RPlane*			m_pUserCullingPlanes;					///< culling 을 위한 나머지 평면들 ( near-far 평면포함 )
	const RLightSceneNode*	m_pCurrentLight;
	static RRsRenderHelper	m_rsRenderer;
	unsigned int			m_unRenderCounter;
	int						m_nCurrentLightCount;					///< 현재 light multipass 패스에서 그리는 광원 수

	float				m_fRemainedBlendingTime;

	RStaticMeshRenderStrategy*				m_pRenderStrategy;			///< 렌더링 전략들
	RStaticMeshDepthRenderStrategy*			m_pDepthRenderStrategy;
	RStaticMeshGStageRenderStrategy*		m_pGStageRenderStrategy;
	RStaticMeshMStageRenderStrategy*		m_pMStageRenderStrategy;
	RStaticMeshRenderStrategy*				m_pCurrentRenderStrategy;	///< 셋팅된 렌더링 전략
	
	bool				m_bPointColor;

	// 현재 상황 디버그용
	int					m_nRenderedTriangles;
	int					m_nRenderedNodes;
	int					m_nRenderedCells;

	// TODO: 삭제 예정 - charmke
//	int					m_nCullTestCount; 

protected:
	virtual void				OnAddScene();
	virtual void				OnRemoveScene();
	
	virtual int		CalcPriority() override;
	virtual void	OnLoadingComplete(RResource*) override;

	virtual void Cull(RCameraSceneNode* pCamera,const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar);
	void CullRecurse(RCell *pCell,const RPlane *pPortalPlanes, int nPlanes);
	void CullGeometry(RGeometry* pGeometry);
	void CullNode(RGeometry* pGeometry, RRsAABBTree* pAABBTree, RRsNode* pNode, bool bCulling);

	static const int CULL_OUTSIDE = -1;
	static const int CULL_PARTIAL =  0;
	static const int CULL_INSIDE  =  1;
	int CullTest(RRsNode* pNode);

	virtual void OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod) override;
	virtual void OnRenderShadowMap( RCameraSceneNode* pCurrentCamera, RShadowStrategy* pShadowStrategy ) override;

	virtual bool CreateFromXML(MXmlElement &element, bool bBackgroundCreation = false);
	virtual void SaveToXML(MXmlElement &element);

	bool ClipPlanes(const RVector& pos, const RPlane *pClipPlanes, int nClipPlanes,
		const RVector* pPlaneVertices, int nPlaneVertices, vector<RPlane> &outClippedPlanes, bool bDebugRender = false);

	void ClearRenderNodes();

	void RestoreRenderState();

private:
	RCell		*m_pCurrentCell;

	bool Pick(RRsNode *pNode, RSTATICMESHPICKINFO& info, RVertexOffsetInfo& VertexInfo);

	void GetVertexFromBound(RRsNode* pNode, std::vector<RVt_pos>* vecVertex, const RBoundingBox &aabb, RVertexOffsetInfo& VertexInfo);

	void UpdateAABB();

public:
	RStaticMeshSceneNode();
	virtual ~RStaticMeshSceneNode();

	static bool CreateSharedResources();			// 필요한 쉐이더등 리소스 할당
	static void Destroy();

	void SetCurrentLightmapsGroup( const char* pSzName );
	void BeginBlendingLightmapsGroups( const char* pSzSrcGroup, const char* pSzTargetGroup, float _fProcessTime, bool bDeleteSrcOnComplete = true){}

	int	GetRenderedTriangleCount() { return m_nRenderedTriangles; };
	int	GetRenderedNodeCount() { return m_nRenderedNodes; };
	int	GetRenderedCellCount() { return m_nRenderedCells; };

//	virtual bool Pick(const RVector &origin,const RVector &to,RPICKINFO *pPickInfo);
	virtual bool OnPick(RPICKINFO& pickInfo);

	virtual void OnUpdateAABB() override;

	/// 특정 위치에 속한 셀을 찾는다
	int FindCell(const RVector& vecPosition);

public:
	int GetVertexFromBound(std::vector<RVt_pos>* vecVertex, const RBoundingBox &aabb);

	// 임시
	void Render( RCameraSceneNode* pCurrentCamera);
	void RenderRaw( RCameraSceneNode* pCurrentCamera);
	void RenderGeometry(RGeometry* pGeometry);

	void RenderGStage( RCameraSceneNode* pCurrentCamera);
	void RenderMStage( RCameraSceneNode* pCurrentCamera);

private:
	void CollectStaticCollisionPolygons(RRsNode* pNode, RSceneNodePolygonCollector* pCollector, RVertexOffsetInfo& VertexInfo) const;
	virtual void CollectStaticCollisionPolygons(RSceneNodePolygonCollector* pCollector) const override;

private:
	bool	m_bEnablePicking;	//피킹이 될것인가?
public:
	void	setEnalbePicking(bool b)	{	m_bEnablePicking = b;		}
	bool	getEnablePicking(void)		{	return m_bEnablePicking;	}
};

}