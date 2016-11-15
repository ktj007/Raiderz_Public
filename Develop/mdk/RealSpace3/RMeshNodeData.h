#ifndef _RMeshNodeData_h
#define _RMeshNodeData_h

#include "RPreDefine.h"
#include "RMeshUtil.h"

namespace rs3 {

class RMesh;
class RMaterial;

// 순수 모델 데이터 정보

class RS_API RMeshNodeData :public RBaseObject
{
public:
	RMeshNodeData();
	virtual ~RMeshNodeData();

	// bbox

	void		BBoxClear();
	void		CalcLocalBBox();

	int			GetFacePosIndex(int findex,int pindex);
	int			GetFaceNormalIndex(int findex,int pindex);
	int			GetFaceUVIndex(int findex,int pindex);

	RVector		GetFacePos(int findex,int pindex);		// 면의 위치값.
	RVector		GetFaceNormal(int findex,int pindex);	// 면의 노멀값.
	RVector		GetFaceUV(int findex,int pindex);		// 면의 uv 값.
	int			GetFaceMtrl(int findex);				// 면의 재질 인덱스.

	char*		GetParentName() const { return (char*)m_ParentName.c_str(); }

public:

	// id

	int				m_nID;
	int				m_nUserID;
	int				m_nMtrlID;
	int				m_LODProjectionIndex;

	// name

	string			m_ParentName;					// 부모 노드 이름
	int				m_nParentNodeID;				// 부모 노드 ID

	// bbox

	RBoundingBox	m_boundingBox;

	// matrix

	RMatrix			m_matLocal;						// 로컬위치 매트릭스
	RMatrix			m_matResult;					// 최종결과 매트릭스	TODO: 삭제
	float			m_fBaseVisibility;				// 기본 visibility
	float			m_fBaseNoiseRef;

	int				m_nMtrlTableInfoCnt;			// 재질   갯수
	RMtrlTableInfo*	m_pMtrlTableInfo;				// sub-material 인덱스버퍼 정보

	// 저장되어있는 지오메트리 ( polygons )
	int				m_nPointCnt;					// 위치		갯수
	RVector*		m_pPointTableOrg;				// 원위치  정보
	RVector*		m_pPointTable;					// 현재 위치 정보	--> TODO: pointTableCurrent 로 바꾸자
	int				m_nPointColorCnt;				// 컬러		갯수
	RVector*		m_pPointColorTable;				// 컬러    정보
	int				m_nNormalTableCnt;				// 노멀 정보 갯수
	RVector*		m_pNormalTable;					// 노멀    정보

	// 버텍스버퍼에 대응되는 normal 배열
	int				m_nPointNormalCnt;				// 노멀		갯수 
	RVector*		m_pPointNormalTable;			// 현재 point 에 대한 노멀 정보	// TODO: 이거 정리 필요할듯. 

	int				m_nTangentTanTableCnt;			// 노멀 정보 갯수
	RVector4*		m_pTangentTanTable;				// tangent 정보 // version16에서 vector4로 변경 w = FaceParity
	int				m_nTangentBinTableCnt;			// 노멀 정보 갯수
	RVector*		m_pTangentBinTable;				// binormal정보

	int				m_nTexCoordTableCnt;			// 텍스쳐 uv 갯수
	RVector*		m_pTexCoordTable;				// 텍스쳐 uv 정보

	int				m_nTexCoordExtraTableCnt;		// 여벌 텍스쳐 uv 개수
	RVector*		m_pTexCoordExtraTable;			// 여벌 텍스쳐 uv 정보

	int				m_nPhysiqueCnt;					// physique 갯수
	RPhysiqueInfo*	m_pPhysique;					// Physique 정보
	int				m_nFaceCnt;						// 면		갯수	m_pPolygonTable의 개수
	RMeshPolygonData*	m_pPolygonTable;			// polygon 정보
	int				m_nBoneTableCnt;				// 본		갯수 - 있으면 피직으로 판단해도 되나? COMMENT OZ 090709
	RMatrix*		m_pBoneTable;					// Bone		정보
	WORD*			m_pBoneTableIndex;				

	// 렌더링을 위한 지오메트리 ( triangles )
	int				m_nVertexIndexTableCnt;			// 버텍스 버퍼 정점 갯수
	RVertexIndex*	m_pVertexIndexTable;			// 버텍스 버퍼를 구성할 정보
	int				m_nFaceIndexTableCnt;			// 인덱스버퍼 개수
	WORD*			m_pFaceIndexTable;				// 인덱스 버퍼를 구성할 정보

	int				m_nTotalDegree;					// 모든폴리곤의 degree 의 합계
	int				m_nTotalTriangles;				// 모든폴리곤의 삼각형수의 합계
#ifdef MESH_STRIP_TEST
	RMtrlTableInfo*	m_pMtrlTableInfoDefault;		// default & optimized 인덱스버퍼 정보
	RMtrlTableInfo*	m_pMtrlTableInfoStrips;			// strips 인덱스버퍼 정보
#endif

	RFaceSubData*	m_pAllFaceSubData;				// m_pPolygonTable 안에 FaceSubData 의 메모리 할당
};

//////////////////////////////////////////////////////////////////
// Node Mtrl 
//
// 노드 재질관리 함수들 .. 
//		기본재질은 한곳에서 관리하고.. 
//			포인터만 얻어둔다.

class RS_API RMeshNodeMtrl
{
public:
	RMeshNodeMtrl();
	virtual ~RMeshNodeMtrl();

	int			GetMtrlCount();
	RMaterial*	GetMtrl(int i=0);// MtrlTable

public:
	int			m_nMtrlCnt;					// 자신의 노드가 사용하는 재질의 갯수. 보통의 경우 1개.
	RMaterial**	m_pMtrlTable;				// 재질 포인터 테이블.
};

}

#endif//_RMeshNodeData_h
