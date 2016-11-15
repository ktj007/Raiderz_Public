#if !defined(AFX_EL_MESH_H__A5FA3593_78B0_4304_B36D_635328A82A04__INCLUDED_)
#define AFX_EL_MESH_H__A5FA3593_78B0_4304_B36D_635328A82A04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable : 4530)

#include "RD3DVertexUtil.h"
#include "EffectList.h"
#include "MCAnimationKey.h"
#include "MCTable.h"
#include "d3dx9.h"
#include "LightList.h"
#include "MBox.h"
#include "RShaderComponentID.h"

#include <list>

using namespace std;
using namespace rs3;

class RMaxMesh;
class INode;
class RMaxMaterial;
class RMaxMaterialKeeper;

namespace rs3 {
class MCollision;
}

struct TangentInfo
{
	RVector tan;
	RVector bin;
	RVector nor;
};

struct RMaxFaceData {
	RMaxFaceData();
	~RMaxFaceData();

	int				nDeg;
	RFaceSubData	*index;
	RMaxMaterial	*pMaterial;
	int				nSubMaterialID;
	unsigned char	*triangleIndices;
};

class RMaxMeshFile
{
public:
	RMaxMeshFile() { m_fp = NULL; }
	RMaxMeshFile(FILE* fp) { Set(fp); }

	void Set(FILE* fp) { m_fp = fp; }

	// help func

	//	void Load();

	void Save(BYTE n);					// byte
	void Save(char n);
	void Save(WORD n);					// word
	void Save(short n);
	void Save(DWORD n);					// dword
	void Save(int n);
	void Save(float f);

	void Save(char* str);				// str
	void Save(string& str);				// str

	void Save(D3DXVECTOR3& v);			// vector
	void Save(D3DXVECTOR4& v);
	void Save(D3DXCOLOR& v);			// color
	void Save(D3DXMATRIX& m);			// matrix

	void Save(D3DXVECTOR3* v,int nCnt);	// vector
	void Save(D3DXVECTOR4* v,int nCnt);
	void Save(D3DXCOLOR* v  ,int nCnt);	// color
	void Save(D3DXMATRIX* m ,int nCnt);	// matrix

	void Save(RMaxFaceData* p	,int nCnt);
	void Save(RPhysiqueInfo* p  ,int nCnt);

	void Save(RVisKey* p  ,int nCnt);

	void Save(void* data,int nSize);	// data

	////////////////////////////////////////////////////
	// Text

	void SaveText(char* name,D3DXCOLOR& c);
	void SaveText(char* name,D3DXVECTOR3& v);
	void SaveText(char* name,D3DXVECTOR3& v,float f);
	void SaveText(char* name,D3DXMATRIX& c);

	void SaveText(char* name,int n1);
	void SaveText(char* name,int n1,int n2);
	void SaveText(char* name,int n1,int n2,int n3);

	void SaveText(char* name,float f1);
	void SaveText(char* name,float f1,float f2);
	void SaveText(char* name,float f1,float f2,float f3);

	void SaveText(char* name,string& s1);

	void SaveText(char* name,char* s1);
	void SaveText(char* name,char* s1,char* s2);
	void SaveText(char* name,char* s1,char* s2,char* s3);

	void SaveText(char* format,...);

public:

	FILE*	m_fp;

};

class RMaxMeshNode
{
public:
	RMaxMeshNode();
	virtual ~RMaxMeshNode();

	void OptimizeAnimation();	// 애니메이션의 불필요한 key 를 줄여 없앤다.

	void Save(RMaxMeshFile& mfp);
//	void SaveText(RMaxMeshFile& mfp);

	void GetPolygons(std::vector<vector<MVector3> > &outPolygonArray);
	//void GetPolygonData(std::vector<MVector3> &outVertices, std::vector<pair<int, int> > &outEdges);

	void CalcPhysiquePID(RMaxMesh* pMesh);
	void CalcBufferIndex(RMaxMesh* pMesh);
	int  CalculateParity(TangentInfo& info);
	float CalculateAngleWeight(RMaxFaceData* pF, size_t vidx0, size_t vidx1, size_t vidx2);
	void AddFaceTangentSpaceToVertices( RMaxFaceData* pF, TangentInfo& info, size_t faceIndex );
	void CalcName2Type(INode* node);

	void AddKeys(Matrix3 tm, int nTime, BOOL pos, BOOL rot, BOOL scale);

	bool IsAttachableMeshNode();
	void Attach( vector<RMaxMeshNode*>* pSrcList, vector<RMaxMeshNode*>* pOutDeleteList );

private:

	void GetFaceTangentInfo(RMaxFaceData* pF,TangentInfo& info,int index);

public:

	void	AddFlag(DWORD dwFlag) { m_dwFlag |= dwFlag; }
	void	DelFlag(DWORD dwFlag) { m_dwFlag = ~dwFlag & m_dwFlag; }
	DWORD	GetFlag() { return m_dwFlag; }
	bool	CheckFlag(DWORD dwCheckFlag) { return (m_dwFlag & dwCheckFlag)? true : false; }

	int				m_nID;
	int				m_nParentID;

	string			m_Name;
	string			m_ParentName;
	string			m_strRelatedNodeNames;

	D3DXMATRIX		m_matBase;
	D3DXMATRIX		m_matLocal;	
	float			m_fBaseVisibility;

	bool			m_isHaveRotAni;
	bool			m_isHavePosAni;

	int				m_PointCnt;
	int				m_FaceCnt;
	int				m_PointColorCnt;	// 있다면 m_PointCnt 과 같다~
	int				m_PhysiqueCnt;
	int				m_VertexAniCnt;
	int				m_BoneTableCnt;
	int				m_VertexIndexTableCnt;
	int				m_MtrlTableInfoCnt;
	int				m_FaceIndexTableCnt;

	D3DXVECTOR3*	m_PointTable;
	vector<RMaxFaceData*>	m_FaceTable;	
	D3DXVECTOR3*	m_PointColorTable;
	RPhysiqueInfo*	m_PhysiqueTable;
	D3DXMATRIX*		m_BoneTable;
	WORD*			m_BoneTableIndex;
	MBox			m_BoundingBox;
	MBox			m_MaxAniBoundingBox;


	MCVectorTable	m_NormalTable;

	// tangent
	MCVector4Table	m_TangentTanTable;
	MCVectorTable	m_TangentBinTable;
	vector<RVector4> m_vecTangentTable;

	WORD*			m_pTangentIndex;
	MCVector4Table	m_TangentOrgTable;

	MCVectorTable	m_TexCoordTable;
	MCVectorTable	m_TexCoordTableExtra;

	D3DXVECTOR3**	m_VertexAniTable;
	DWORD*			m_VertexAniFrame;

	RVertexIndex*	m_VertexIndexTable;
	WORD*			m_FaceIndexTable;
	RMtrlTableInfo*	m_MtrlTableInfo;

	RMaxMaterial	*m_pMtrl;

	bool			m_NormalFlag;

	MCKeyTrack<MCPosKey>	m_positionKey;
	MCKeyTrack<MCQuatKey>	m_rotationKey;
	MCKeyTrack<MCScaleKey>	m_scaleKey;
	MCKeyTrack<MCVisKey>	m_visKey;

	int				m_nMaxFrame;

	DWORD					m_dwFlag;
	int						m_nAlign;

	EffectList				m_effectList;

	RSCID					m_rscShaderComponentFlag; // 특수효과(유령) 셰이더 사용 컴포넌트 Flag, 마테리얼에 넘겨준다.

	list<string>			m_userPropertyString;
	bool		FindPropertyString(const string &str)
	{
		for ( list<string>::iterator it = m_userPropertyString.begin(); it != m_userPropertyString.end(); ++it )
		{
			if ( *it == str )
				return true;
		}

		return false;
	}

	void		DeleteProcessedProperty(const string &strPropertyName)
	{
		list<string>::iterator it = m_userPropertyString.begin();
		while ( it != m_userPropertyString.end() )
		{
			// trim right
			string aLine = (*it);
			int n = aLine.find_last_not_of(" \n\r");
			if(n==string::npos) continue;
			string buf = aLine.substr(0,n+1);

			stringstream sstream(buf); // string을 stream에 넣는다
			sstream >> aLine;

			if( (strncmp(aLine.c_str(), strPropertyName.c_str(), aLine.length())==0) 
				&& (strncmp(aLine.c_str(), strPropertyName.c_str(), strlen(strPropertyName.c_str()))==0) ) 
			{
				m_userPropertyString.erase(it++);
			}
			else 
				it++;
		}

	}
};

typedef list<RMaxMeshNode*>		RMaxMeshList;
typedef RMaxMeshList::iterator	RMaxMeshListNode;

class RMaxMesh
{
public:
	RMaxMesh();
	virtual ~RMaxMesh();

	int	 Add(RMaxMeshNode* data);	// 메시 데이터 추가

	int  GetMeshID(string& name);

	void CalcPhysiquePID();
	void CalcBufferIndex();
	void CalcParentID();
	void CalcBoundBox();
	void CalcMaxAniBoundBox();

	void AttachMeshNodes();

	void DelAll();

	void OptimizeKeys();

	bool export_bin(char* filename);
	bool export_ani(char* filename,int mode);
	bool export_desc_xml(char* filename, bool bExporyUserProperty = true);
	bool export_obstacle(char *filename);

	int GetEffectCount();

	int  ani_node_cnt();

	bool IsEmpty();

public:

	int						m_nMaxFrame;

	vector<RMaxMeshNode*>	m_MeshTable;
	int						m_MeshCount;

	RMaxMeshList			m_ObstacleMeshList;

	RMaxMaterialKeeper		*m_pMaterialKeeper;
	vector<MCollision*>		m_collisionList;

	vector<vector<MVector3>> m_OccluderPointList;

	MBox					m_MaxAniBoundingBox;

	bool					m_bAlpha2Pass;
	bool					m_bAutoAnimation;

	// LOD, Culling
	static const int		LOD_LEVEL = 3;

	bool					m_bFogCulling;
	bool					m_bDistanceCulling;

private:
	void DeleteProcessedProperty(const string &str);
	void DeleteProcessedProperty(const string &strPropertyName, bool bValue);
	void ExportUserProperty(MXmlElement *pElement);
};

//////////////////////////////////////////////////////////////////////////
inline bool RMaxMesh::IsEmpty() { return m_MeshCount==0; }

#endif // !defined(AFX_EL_MESH_H__A5FA3593_78B0_4304_B36D_635328A82A04__INCLUDED_)
