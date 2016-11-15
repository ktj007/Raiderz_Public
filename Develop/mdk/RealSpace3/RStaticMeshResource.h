#pragma once

#include "RResource.h"
#include "RD3DVertexUtil.h"
#include "RShaderComponentID.h"
#include "RTechnique.h"

// 백그라운드 로딩

namespace rs3 {

const int INDEX_FLUSH_COUNT	= 3000*3;	// 3의 배수여야한다

extern bool bStaicMeshRenderAll;

class RCell;
class RPortal;
typedef list< pair<RPortal*,RCell*> > RPortalList;
class RRsNode;
class RMaterialResource;

struct RPOLYGONINFO {	// nBaseIndex,nBaseIndex+1,nBaseIndex+2 가 폴리곤을 이룬다
	int		nMaterial;
	DWORD	dwFlags;
	char	*pVertices;
	int		nVertices;
};

// 로딩을 위한 구조체
struct RRsLoadHelper
{
	RRsNode*			pLoadingNode;
	RPOLYGONINFO*		pLoadingInfo;
	char*				pLoadingVertex;
	WORD*				pLoadingIndex;
	int					nBaseIndex;
	int					nIndicies;
};

class RRsNode
{
public:
	RBoundingBox	m_bb;
	RRsNode*		m_pChildren[2];

	RPOLYGONINFO*	m_pPolygonInfo;	// 해당 폴리곤의 정보
	int				m_nPolygon;
	unsigned int	m_unRenderCounter;	// 노드 중복 렌더링 체크를 위해 그릴 때 마다 갱신

//	bool Open(MFile &file,RRsLoadHelper& loading,RVertexOffsetInfo &VertexInfo);
	bool IsLeafNode() { return m_nPolygon!=0; }

public:
	RRsNode() : m_hIB(R_NONE), m_unRenderCounter(0), m_pPolygonInfo( NULL ), m_nPolygon( 0 ), m_nIndicies( 0 ), m_nMinIndex( 0 ),
		m_nMaxIndex( 0 ), m_pIndicies( NULL )
	{ 
		m_pChildren[0]=m_pChildren[1]= NULL;
	}

	int		m_nIndicies;
	int		m_nMinIndex, m_nMaxIndex;
	WORD*	m_pIndicies;

	HINDEXBUFFER m_hIB;
};

/// material 당 쓰이는 technique 들을 모아놓은 캐쉬
struct RenderInfo
{
	RMaterial*		m_pMaterial;
	RTechnique		m_technique;
	RSCID			m_effectId;
	RTechnique		m_fogTechnique;
	RSCID			m_fogEffectId;
	RTechnique		m_depthTechnique;
	RSCID			m_depthEffectId;
	RTechnique		m_lightTechnique;
	RSCID			m_lightEffectId;
	RTechnique		m_SpotlightTechnique;
	RSCID			m_SpotlightEffectId;
	// G-Stage
	RTechnique		m_GStageTechnique;
	RSCID			m_GStageEffectId;
	// M-Stage
	RTechnique		m_MStageTechnique;
	RSCID			m_MStageEffectId;
	RTechnique		m_MStageFogTechnique;
	RSCID			m_MStageFogEffectId;

	bool			m_bPointColor;

	RenderInfo() {}
};

class RRsAABBTree
{
public:
	int			m_nBaseIndex;	// 전체 vb 의 baseIndex
	int			m_nMaterial;
	int			m_nLightmapID;
	int			m_nVertices;
	RRsNode*	m_pRootNode;
	RenderInfo	m_renderInfo;

	//
	HINDEXBUFFER m_hIB;
	int			m_nIndexCount;
	WORD*		m_pIndicies;
};

class RPortal {
public:
	RPortal() : m_pVertices(NULL), m_nVertices(0), m_nCellID1(0), m_nCellID2(0)  { }
	~RPortal();

	string		m_strName;
	int			m_nVertices;
	RVector*	m_pVertices;
	RPlane		m_plane;

    int			m_nCellID1;
	int			m_nCellID2;
};

class RGeometry {
public:
	DWORD					m_dwFVF;
	
	int 					m_nNode;
	int 					m_nPolygon;
	int 					m_nVertices;
	int 					m_nIndices;
	int 					m_nTrees;

	RVertexOffsetInfo		m_VertexInfo;
	HVERTEXBUFFER			m_hVertexBuffer;
	char*					m_pVertices;
	RPOLYGONINFO*			m_pPolygonInfo;	// 해당 폴리곤의 정보
	WORD*					m_pIndices;
	RRsNode*				m_pNodes;			// 노드들의 array 첫번째가 root이다.
	vector<RRsAABBTree*>	m_pTrees;

	RGeometry();
	~RGeometry();
	void MakeVertexInfo( RVertexOffsetInfo& info );
};

class RCellFace
{
public:
	RCellFace() : nVerts(0), pVerts(NULL) {}
	~RCellFace() { SAFE_DELETE_ARRAY( pVerts ); }

	int			nVerts;
	RVector*	pVerts;
};

class RCell {
public:
	string					m_strName;

	RPortalList				m_portals;			// 이 셀에 포탈로 연결된 셀들의 리스트
	list<RPlane>			m_planes;			// 섹터의 모양을 정의하는 평면들
	RBox					m_bb;

	vector<RCellFace*>		m_cellFaces;

	// FVF 로 분류된 기하학 정보의 리스트
	typedef map<DWORD, RGeometry*>	IDGEOMETRYMAP;
	IDGEOMETRYMAP			m_mapGeometryList;


	RCell();
	~RCell();
	bool Contains(const RVector& p);	// 이 점을 포함하고 있는지
};

class RS_API RStaticMeshResource : public RResource
{
public:
	RStaticMeshResource(void);
	virtual ~RStaticMeshResource(void);

	const char*						GetFileName() { return m_strFileName.c_str(); }

	int								GetCellCount()	{ return (int)m_cells.size(); }
	RCell*							GetCell(int i)	{ return m_cells[i]; }
	vector<RCell*>&					GetCell(){ return m_cells; }
	
	static void						MakeDirectoryNameFromFileName(const char* pSzFileName, char* pOut, int nCount);

	const vector<RMaterialResource*>&	GetMaterialList(){ return m_vecMaterials; }


protected:

	virtual bool					Fill() override;
	virtual LOAD_RESULT				Load() override;

	string							m_strFileName;	// 현재 파일 이름, RStaticMeshSceneNode에서 CreateFromXML시에 설정
	char							m_szCurrentDirectory[_MAX_PATH];
	static const char*				m_pSzDefaultLigntmapsGroup;

	HINDEXBUFFER					m_hIndexBuffer;

	DWORD							m_dwVersion;	// rs file version
	vector<RMaterialResource*>		m_vecMaterials;
	vector<RCell*>					m_cells;
	vector<RPortal*>				m_portals;
	RCell							m_globalCell;

	RListener*						m_pEventListener;	///< 환경, 그림자설정이 바뀌었을때 이벤트 수신객체

	void							Destroy();
	bool							OpenDescription(const char *szFileName, bool bBackground);
	bool							OpenMaterials(MXmlElement* pElement, bool bBackground);
	void							RecreateVertexBuffer();
	void							CreateShaders();

public:
	void							CreateShaderForDepth(RenderInfo* pRenderInfo);
	void							CreateShaderForGStage(RenderInfo* pRenderInfo);
	void							CreateShaderForMStage(RenderInfo* pRenderInfo, bool bFogEnable);

};

}