#ifndef _RD3DVertexUtil_h
#define _RD3DVertexUtil_h

// max plugins 도 같이 사용

#include <list>
#include <set>
#include <string>
#if (_MSC_VER >= 1900)
#include <unordered_map>
#else
#include <hash_map>
#endif

#include <D3D9TYPES.h>

#include "RTypes.h"


namespace rs3 {

#define		MAX_NAME_LEN		40
#define		MAX_PATH_NAME_LEN	256
#define		MAX_ANI_KEY			100
#define		MAX_MESH_NODE_TABLE	300
#define		MAX_PHYSIQUE_KEY	3

#ifndef USING_VERTEX_SHADER
#define USING_VERTEX_SHADER
#endif

using namespace std;

/////////////////////////////////////////////////////
// max plugins 관련 버전 관리..

#define EXPORTER_MESH_VER1	0x00000011
#define EXPORTER_MESH_VER2	0x00005001
#define EXPORTER_MESH_VER3	0x00005002
#define EXPORTER_MESH_VER4	0x00005003
#define EXPORTER_MESH_VER5	0x00005004
#define EXPORTER_MESH_VER6	0x00005005
#define EXPORTER_MESH_VER7	0x00005006
#define EXPORTER_MESH_VER8	0x00005007
#define EXPORTER_MESH_VER9	0x00005008
#define EXPORTER_MESH_VER10	0x00005009
#define EXPORTER_MESH_VER11	0x0000500a		// 기본 Visibility 추가
#define EXPORTER_MESH_VER12	0x0000500b		// triangle -> polygon 단위, trilist -> tristrips
#define EXPORTER_MESH_VER13 0x0000500c		// 전처리 계산된 BoundingBox 데이터 추가
#define EXPORTER_MESH_VER14 0x0000500d		// FVF 데이터 추가
#define EXPORTER_MESH_VER15 0x0000500e		// ActorLightMap Channel & FVF 추가
#define EXPORTER_MESH_VER16 0x0000500f		// Tangent RVector3 -> RVector4 w 값을 Face Parity로 사용한다.
#define EXPORTER_MESH_VER17 0x00005010		// 라이트맵 삭제
#define EXPORTER_MESH_VER18 0x00005011		// Extra UV 채널 사용
#define EXPORTER_MESH_VER19 0x00005012
#define EXPORTER_MESH_VER20 0x00005013

#define EXPORTER_CURRENT_MESH_VER	EXPORTER_MESH_VER20

#define EXPORTER_ANI_VER1	0x00000012
#define EXPORTER_ANI_VER2	0x00001001
#define EXPORTER_ANI_VER3	0x00001002
#define EXPORTER_ANI_VER4	0x00001003	// 에니메이션의 회전값을 미리계산해서 넘김 : 로딩속도 개선
#define EXPORTER_ANI_VER5	0x00001004	// scale key 포함, 필요없는 key 제거
#define EXPORTER_ANI_VER6	0x00001005	// 계층정보 포함
#define EXPORTER_ANI_VER7	0x00001006	// vertex animation 시의 bounding box 데이터 포함
#define EXPORTER_ANI_VER8	0x00001007	// animation 의 bounding box 데이터 (0~MaxFrame까지의 전체 AABB)
#define EXPORTER_ANI_VER9	0x00001008	// 이 버전 밑으로 visibility값이 0보다 작은 경우가 있으므로 별도 체크가 필요하다
#define EXPORTER_ANI_VER10	0x00001009
#define EXPORTER_ANI_VER11	0x00001010
#define EXPORTER_ANI_VER12	0x00001011

#define EXPORTER_CURRENT_ANI_VER	EXPORTER_ANI_VER12

#define EXPORTER_SIG		0x0107f060

typedef struct {
	DWORD	sig;
	DWORD	ver;
	int		mtrl_num;
	int		mesh_num;
} ex_hd_t;

typedef struct {
	DWORD	sig;
	DWORD	ver;
	int		maxframe;
	int		model_num;
	int		ani_type;
} ex_ani_t;

enum RPickType {
	pick_bbox = 0,
	pick_collision_mesh,
	pick_real_mesh,
	pick_end
};


enum RShaderBlendInput {
	VPOSITION,
	WEIGHT2,
	MATRIX_INDEX,
	NORMAL,
	TEXTURE_UV
};

///////////////////////////////////////////////////

enum RFVF_TYPE
{
	RFVF_XYZ,
//	RFVF_XYZRHW,
//	RFVF_XYZW,
	RFVF_XYZB3_LASTBETA_UBYTE4,
	RFVF_XYZB4_LASTBETA_UBYTE4,			// blend weight 5개
	RFVF_NORMAL,
//	RFVF_PSIZE,
	RFVF_DIFFUSE,
//	RFVF_SPECULAR,
	RFVF_TEX1,	// 텍스쳐 1개 Diffuse
	RFVF_TEX2,	// 텍스쳐 2개 Diffuse + Extra
	RFVF_TEX2_TEX2SIZE4, // 텍스쳐 2개 Diffuse + NormalMap(vector4)
	RFVF_TEX3_TEX3SIZE4, // 텍스쳐 3개 Diffuse + Extra + NormalMap(vector4)

	RFVF_NUM
};

struct RFVFPair
{
	DWORD	fvf;
	int		size;
};
static const RFVFPair RFVF[RFVF_NUM] =
{
	{ D3DFVF_XYZ,		sizeof(RVector3) },
//	{ D3DFVF_XYZRHW,	sizeof(RVector4) },
//	{ D3DFVF_XYZW,		sizeof(RVector4) },
	{ D3DFVF_XYZB3 | D3DFVF_LASTBETA_UBYTE4, sizeof(RVector3) }, // RFVF_XYZ크기를 제외
	{ D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4, (sizeof(float)*4) }, // RFVF_XYZ크기를 제외 blend weight 4개 
	{ D3DFVF_NORMAL,	sizeof(RVector3) },
//	{ D3DFVF_PSIZE,		sizeof(float) },
	{ D3DFVF_DIFFUSE,	sizeof(DWORD) },
//	{ D3DFVF_SPECULAR,	sizeof(DWORD) },
	{ D3DFVF_TEX1,		sizeof(RVector2) },
	{ D3DFVF_TEX2,		sizeof(RVector2) },
	{ D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE4(1), sizeof(RVector4) }, // Tex0:Diffuse(Vector2):크기 제외 / Tex1:NormalMap(Vector4)
	{ D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE4(2), sizeof(RVector4) } // Tex0:Diffuse(Vector2):크기 제외 / Tex1:LightMap(Vector2) / Tex2:NormalMap(Vector4)
};

// 버텍스 버퍼의 데이터의 각 요소들의 위치값
struct RVertexOffsetInfo
{
	RVertexOffsetInfo() : size(0), pos(-1), weight(-1), normal(-1), color(-1), texCoord(-1) {}

	int size;		// 단위 버텍스 크기
	int pos;		// Position값 오프셋
	int weight;		// Blend Weight값 오프셋
	int normal;		// Normal값 오프셋
	int color;		// Vertex Color값 오프셋
	int texCoord;	// Texture Coordinate값 오프셋
};

struct	RLVertex { 
	RVector p;
	DWORD color;
	FLOAT tu, tv;
};
#define RLVertexType ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

struct	RTestVertex { 
	RVector p;   
	DWORD color; 
};
#define RTestVertexType	( D3DFVF_XYZ | D3DFVF_DIFFUSE )


// new face data type

struct RFaceSubData
{
	WORD p;		// pos		index	
	WORD uv;	// tex uv	index
	WORD uv2;	// extra uv index
	WORD n;		// normal	index
	WORD n_tan;	// tangent	index
	WORD n_bin;	// binormal  index
};

struct RFaceData {
	RFaceSubData index[3];
	short		 mtrl_id;
};

struct RMeshPolygonData {
	int			nVertices;
	RFaceSubData *index;
	short		 mtrl_id;
//	unsigned char *triIndicies;	// 이 폴리곤의 삼각형구성 정보 (nVertices-2)*3 개의 0 base index
};

struct RPhysiqueSubData {
	string		name;
	float		weight;
	WORD		cid;
	WORD		pid;
};

struct RMtrlTableInfo
{
	int		mtrlid;
	WORD	offset;
	WORD	count;

	// 3D맥스에서 폴리곤 단위로 서브 마테리얼 ID를 지정 할 수 있다. 그 값을 저장한 것.
	// 엔진에서는 이 값을 가지고 메시 일부분을 그려야 할지 판단하는 용도로 사용한다.
	// RActorNode::SetSubMaterialMask(), RActorNode::GetSubMaterialMask() 참조.
	int		nSubMaterialIDForDrawMasking;	// version 9에서 추가
};


#define PHYSIQUE_MAX_WEIGHT		3		// 웨이트의 제한을 변경 용이하게 디파인화 
										// 버텍스 저장을 본에 붙어있는 상태로 미리 저장을 해 놓으면 이상하게 보이는데 이와 관련이 있을지도... 
struct RPhysiqueInfo {

	RPhysiqueInfo()	{
		m_num = 0;
		m_Data = NULL;
	};

	~RPhysiqueInfo() {
		Destroy();
	}

	void Create(int num) {
		m_num = num;
		if(num)
			m_Data = new RPhysiqueSubData[num];
	}

	void Destroy() {
		SAFE_DELETE_ARRAY(m_Data);
		m_num = 0;
	}

	void SetName(int index,string& name) {	if(m_Data)	m_Data[index].name = name;	}
	void SetWeight(int index,float weight) { if(m_Data) m_Data[index].weight = weight;	}
	void SetParentID(int index,int pid) {	if(m_Data)	m_Data[index].pid = pid; }
	void SetBoneID(int index,int cid) {	if(m_Data)	m_Data[index].cid = cid; }

	char* GetName(int index) {	return (char*)m_Data[index].name.c_str(); }
	float GetWeight(int index) { return m_Data[index].weight; }
	int   GetParentID(int index) { return m_Data[index].pid; }
	int   GetBoneID(int index) { return m_Data[index].cid; }

	RPhysiqueSubData* m_Data;

	int		m_num;
};

#define _ALMOST_ZERO 1.0e-3f

inline BOOL Equal(RVector& p1,RVector& p2)
{
	if (fabs(p1.x - p2.x) > _ALMOST_ZERO)	return FALSE;
	if (fabs(p1.y - p2.y) > _ALMOST_ZERO)	return FALSE;
	if (fabs(p1.z - p2.z) > _ALMOST_ZERO)	return FALSE;

	return TRUE;
}

// table 들은 o(nlog(n))인 MCTable.h 로 대체

//class RIntTable : public vector<int>
//{
//public:
//	RIntTable() {}
//
//	int IsExist(int n) {
//		int nCnt = size();
//		for(int i=0;i<nCnt;i++) {
//			if(at(i)==n)
//				return i;
//		}
//		return -1;
//	}
//
//	int Add(int n) {
//
//		// 같은것이 없다면..
//		int nPos = IsExist(n);
//
//		if(  nPos != -1 ) // 이미있다면..
//			return nPos;
//
//		push_back(n);
//
//		return size()-1;
//	}
//};

//class RVectorTable : public vector<RVector>
//{
//public:
//	RVectorTable() {}
//	~RVectorTable() {}
//
//	int IsExist(RVector& v)	{
//		int nCnt = size();
//
//		for(int i=0;i<nCnt;i++) {
//			if(Equal(at(i),v))
//				return i;
//		}
//		return -1;
//	}
//
//	int Add(RVector& v) {
//
//		// 같은것이 없다면..
//		int nPos = IsExist(v);
//
//		if(  nPos != -1 ) // 이미있다면..
//			return nPos;
//
//		push_back(v);
//
//		return size()-1;
//	}
//};

struct RVertexIndex
{
	union {
		struct{
			WORD p;
			WORD n;
			WORD uv;
			WORD uv2;	// Extra UV
			WORD n_tan;
			WORD n_bin;
		};
		WORD index[6];
	};
};

//class RVertexIndexTable : public vector<RVertexIndex>
//{
//public:
//	RVertexIndexTable() {}
//	~RVertexIndexTable() {}
//
//	bool isEqual(RVertexIndex& pSrc,RVertexIndex& pDest) {
//		if(pSrc.p == pDest.p)
//			if(pSrc.n == pDest.n)
//				if(pSrc.uv == pDest.uv)
//					return true;
//		return false;
//	}
//
//	int IsExist(RVertexIndex& p) {
//		int nCnt = size();
//
//		for(int i=0;i<nCnt;i++) {
//			if(isEqual(at(i),p))
//				return i;
//		}
//		return -1;
//	}
//
//	int Add(RVertexIndex& p,bool& bExist) {
//
//		// 같은것이 없다면..
//		int nPos = IsExist(p);
//
//		bExist = false;
//
//		if(  nPos != -1 ) { // 이미있다면..
//			bExist = true;
//			return nPos;
//		}
//
//		push_back(p);
//
//		return size()-1;
//	}
//};

}

#endif//_RD3DVertexUtil_h
