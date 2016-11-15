#include "stdafx.h"

#include "max.h"
#include "RMaxMaterial.h"
#include "RShaderCompositeManager.h"
#include "MCplug3.h"
#include "ELMaxMesh.h"
//#include "RMaterialList.h"
#include "EffectList.h"
#include <sstream>
#include <algorithm>
#include "cs/bipexp.h"
#include "MDebug.h"
#include "MCollision.h"
#include "RMaterial.h"
#include "MFileSystem.h"

///////////////////////////////////////////////////////////////////////

RMaxFaceData::RMaxFaceData() : index(NULL), triangleIndices(NULL), nDeg(0), pMaterial(NULL), nSubMaterialID(0)
{
}

RMaxFaceData::~RMaxFaceData()
{
	SAFE_DELETE_ARRAY(index);
	SAFE_DELETE_ARRAY(triangleIndices);
}

void ComputeTangentSpace( float* vert1, float* vert2, float* vert3, 
						 float* t1, float* t2, float* t3, 
						 D3DXVECTOR3& tan,D3DXVECTOR3& bin)
{
	const float tol = 0.00001f;

	D3DXVECTOR3 edge1;
	D3DXVECTOR3 edge2;
	D3DXVECTOR3 crossP;

	edge1.x = vert2[0] - vert1[0];
	edge1.y = t2[0] - t1[0];    // s-vector - dont need to compute this multiple times
	edge1.z = t2[1] - t1[1];    // t-vector

	edge2.x = vert3[0] - vert1[0];
	edge2.y = t3[0] - t1[0];    // another s-vector
	edge2.z = t3[1] - t1[1];    // another t-vector

	D3DXVec3Cross(&crossP,&edge1,&edge2);
	D3DXVec3Normalize(&crossP,&crossP);

	if (fabs(crossP.x)>tol)
	{
		float tanX = -crossP.y/crossP.x;
		float biX  = -crossP.z/crossP.x;

		tan.x = tanX;
		bin.x = biX;
	}

	edge1.x = vert2[1] - vert1[1];
	edge1.y = t2[0] - t1[0];
	edge1.z = t2[1] - t1[1];

	edge2.x = vert3[1] - vert1[1];
	edge2.y = t3[0] - t1[0];
	edge2.z = t3[1] - t1[1];

	D3DXVec3Cross(&crossP,&edge1,&edge2);
	D3DXVec3Normalize(&crossP,&crossP);

	if (fabs(crossP.x)>tol)
	{
		float tanY = -crossP[1]/crossP[0];
		float biY  = -crossP[2]/crossP[0];

		tan.y = tanY;
		bin.y = biY;
	}

	edge1.x = vert2[2] - vert1[2];
	edge1.y = t2[0] - t1[0];
	edge1.z = t2[1] - t1[1];

	edge2.x = vert3[2] - vert1[2];
	edge2.y = t3[0] - t1[0];
	edge2.z = t3[1] - t1[1];

	D3DXVec3Cross(&crossP,&edge1,&edge2);
	D3DXVec3Normalize(&crossP,&crossP);

	if (fabs(crossP.x)>tol) 
	{
		float tanZ = -crossP[1]/crossP[0];
		float biZ  = -crossP[2]/crossP[0];

		tan.z = tanZ;
		bin.z = biZ;
	}

	D3DXVec3Normalize(&tan,&tan);
	D3DXVec3Normalize(&bin,&bin);
} 

void CreateTangentSpaceVectors( D3DXVECTOR3 *v1, D3DXVECTOR3 *v2, D3DXVECTOR3 *v3,
								float v1u, float v1v,
								float v2u, float v2v,
								float v3u, float v3v,
								RVector *vTangent,
								RVector *vBiNormal,
								RVector *vNormal )
{
	RVector2 v1uv(v1u,v1v);
	RVector2 v2uv(v2u,v2v);
	RVector2 v3uv(v3u,v3v);

	RVector2 deltaUV1 = v2uv - v1uv;
	RVector2 deltaUV2 = v3uv - v1uv;
	RVector3 deltaPos1 = *v2 - *v1;
	RVector3 deltaPos2 = *v3 - *v1;

	// face normal
	*vNormal = deltaPos1.CrossProduct(deltaPos2);
	(*vNormal).Normalize();

	RVector	TriangleNormal = *vNormal;

	RMatrix ParameterToLocal = RMatrix(
		deltaPos1.x,	deltaPos1.y,	deltaPos1.z,	0,
		deltaPos2.x,	deltaPos2.y,	deltaPos2.z,	0,
		(*v1).x,		(*v1).y,		(*v1).z,			0,
		0,				0,				0,				1 );
	
	RMatrix	ParameterToTexture = RMatrix(
		deltaUV1.x,	deltaUV1.y,	0,	0,
		deltaUV2.x,	deltaUV2.y,	0,	0,
		v1u,		v1v,		1,	0,
		0,			0,			0,	1 );

	RVector	TangentX;
	RVector TangentY;
	RVector TangentZ;

	RMatrix ParameterToTextureInv;
	ParameterToTexture.GetInverse(&ParameterToTextureInv);
	RMatrix	TextureToLocal = ParameterToTextureInv * ParameterToLocal;
	TextureToLocal.TransformNormal(RVector(1,0,0), TangentX);
	TextureToLocal.TransformNormal(RVector(0,1,0), TangentY);

	TangentX.Normalize();
	TangentY.Normalize();

	TangentX = TangentX - TriangleNormal * ( TangentX.DotProduct(TriangleNormal) );
	TangentY = TangentY - TriangleNormal * ( TangentY.DotProduct(TriangleNormal) ) - TangentX * ( TangentX.DotProduct(TangentY) );
	TangentZ = TriangleNormal;

	// 정규식
	//(*vTangent) = TangentX.Normalize();
	//(*vBiNormal) = TangentY.Normalize();

	(*vTangent) = TangentY.Normalize();
	(*vBiNormal) = TangentX.Normalize();

	TangentZ = TangentY.CrossProduct(TangentX).Normalize();
	TangentZ = TangentX.CrossProduct(TangentY).Normalize();
}

RMaxMeshNode::RMaxMeshNode() : m_pMtrl(NULL)
{
	D3DXMatrixIdentity(&m_matLocal);
	D3DXMatrixIdentity(&m_matBase);

	m_isHaveRotAni = false;
	m_isHavePosAni = false;

	// face info
	m_PointCnt			= 0;
	m_FaceCnt			= 0;
	m_PointColorCnt		= 0;
	m_PhysiqueCnt		= 0;
	m_VertexAniCnt		= 0;
	m_BoneTableCnt		= 0;

	m_VertexIndexTableCnt = 0;
	m_MtrlTableInfoCnt	  = 0;
	m_FaceIndexTableCnt	  = 0;

	m_PointTable		= NULL;
	m_FaceIndexTable	= NULL;

	m_PointColorTable	= NULL;
	m_PhysiqueTable		= NULL;
	m_BoneTable			= NULL;
	m_BoneTableIndex	= NULL;
	m_VertexIndexTable	= NULL;
	m_MtrlTableInfo		= NULL;

	m_VertexAniTable	= NULL;
	m_VertexAniFrame	= NULL;

	m_BoundingBox.Initialize();
	m_MaxAniBoundingBox.Initialize();

	m_nParentID			= -1;

	m_NormalFlag		= 0;

	m_nMaxFrame		= 0;

	// info

	m_nAlign			= 0;
	m_dwFlag			= 0;

	// tangent
	m_pTangentIndex		= NULL;
}

RMaxMeshNode::~RMaxMeshNode()
{
	SAFE_DELETE_ARRAY(m_PointTable);
	SAFE_DELETE_ARRAY(m_FaceIndexTable);
	for(int i=0;i<(int)m_FaceTable.size();i++)
	{
		delete m_FaceTable[i];
	}
	m_FaceTable.clear();

	SAFE_DELETE_ARRAY(m_PointColorTable);
	SAFE_DELETE_ARRAY(m_PhysiqueTable);
	SAFE_DELETE_ARRAY(m_BoneTable);
	SAFE_DELETE_ARRAY(m_BoneTableIndex);
	SAFE_DELETE_ARRAY(m_VertexIndexTable);
	SAFE_DELETE_ARRAY(m_MtrlTableInfo);

	for(int i=0;i<m_VertexAniCnt;i++)
		SAFE_DELETE_ARRAY(m_VertexAniTable[i]);
	SAFE_DELETE_ARRAY(m_VertexAniTable);
	SAFE_DELETE_ARRAY(m_pTangentIndex);
}

//////////////////////////////////////////////////////////////////////////////

RMaxMesh::RMaxMesh() : m_pMaterialKeeper(new RMaxMaterialKeeper)
{
	m_MeshCount		= 0;
	m_nMaxFrame		= 0;

	m_bAlpha2Pass	= false;
	m_bAutoAnimation= false;

	m_bFogCulling		= true;
	m_bDistanceCulling	= true;
}

RMaxMesh::~RMaxMesh()
{
	delete m_pMaterialKeeper;
	DelAll();
}

#define RVERTEX_MAX		1024*100*32			// 34133 face

void RMaxMeshNode::GetFaceTangentInfo(RMaxFaceData* pF,TangentInfo& info, int index)
{
	int p1,p2,p3;
	int uv1,uv2,uv3;

	p1 = pF->index[(index+0)%3].p;
	p2 = pF->index[(index+1)%3].p;
	p3 = pF->index[(index+2)%3].p;

	uv1 = pF->index[(index+0)%3].uv;
	uv2 = pF->index[(index+1)%3].uv;
	uv3 = pF->index[(index+2)%3].uv;

	CreateTangentSpaceVectors(
		&m_PointTable[p1],
		&m_PointTable[p2],
		&m_PointTable[p3],
		m_TexCoordTable[uv1].x,
		m_TexCoordTable[uv1].y,
		m_TexCoordTable[uv2].x,
		m_TexCoordTable[uv2].y,
		m_TexCoordTable[uv3].x,
		m_TexCoordTable[uv3].y,
		&info.tan,&info.bin,&info.nor );
}

// TODO:   求. dubble
void AccumTangentSpace( RVector* target,RVector* vector)
//void AccumTangentSpace( D3DXVECTOR3* target,D3DXVECTOR3* vector)
{
	*target += *vector;
//	D3DXVec3Normalize(target,target);
	target->Normalize();
}

float GetAngle(D3DXVECTOR3& v1,D3DXVECTOR3& v2)
{
	D3DXVECTOR3 v1a;
	D3DXVECTOR3 v2a;

	D3DXVec3Normalize(&v1a,&v1);
	D3DXVec3Normalize(&v2a,&v2);

	return D3DXVec3Dot(&v1a,&v2a);
}

int RMaxMeshNode::CalculateParity(TangentInfo& info)
{
	// Note that this parity is the reverse of what you'd expect - this is
	// because the 'V' texture coordinate is actually left handed
	if (info.tan.CrossProduct(info.bin).DotProduct(info.nor) >= 0.0f)
		return -1;
	else
		return 1;
}

float RMaxMeshNode::CalculateAngleWeight(RMaxFaceData* pF, size_t vidx0, size_t vidx1, size_t vidx2)
{
	int p1,p2,p3;

	p1 = pF->index[vidx0].p;
	p2 = pF->index[vidx1].p;
	p3 = pF->index[vidx2].p;

	const RVector v0 = m_PointTable[p1];
	const RVector v1 = m_PointTable[p2];
	const RVector v2 = m_PointTable[p3];

	RVector diff0 = v1 - v0;
	RVector diff1 = v2 - v1;

	// Weight is just the angle - larger == better
	return diff0.AngleBetween(diff1);

}

void RMaxMeshNode::AddFaceTangentSpaceToVertices( RMaxFaceData* pF, TangentInfo& info, size_t nFaceIndex ) 
{
	// Calculate parity for this triangle
	int nParity = CalculateParity(info);

	// Now add these to each vertex referenced by the face
	for (int v = 0; v < 3; ++v)
	{
		// index 0 is vertex we're calculating, 1 and 2 are the others
		// We want to re-weight these by the angle the face makes with the vertex
		// in order to obtain tesselation-independent results
		float fAngleWeight = CalculateAngleWeight(pF, v, (v+1)%3, (v+2)%3 ); 

		RVector v3Tangent  = info.tan * fAngleWeight;
		v3Tangent.Normalize();
		RVector4 vTangent = RVector4(v3Tangent, (float)nParity);
		m_pTangentIndex[ nFaceIndex * 3 + v ] = m_TangentOrgTable.Add(vTangent);
	}
}

void RMaxMeshNode::CalcBufferIndex(RMaxMesh* pMesh)
{
	bool bExistUV = !m_TexCoordTable.empty();
	m_pTangentIndex = new WORD[m_FaceCnt * 3];//pFace->nDeg]
	
	RVector tan,bin,nor;
	for (int i = 0; i < m_FaceCnt; i ++) 
	{
		TangentInfo info;
		RMaxFaceData* pFace = NULL;
		pFace = m_FaceTable[i];

		_ASSERT(pFace->nDeg==3);

		// tangent binormal
		if(bExistUV)
		{
			GetFaceTangentInfo(pFace, info, 0);
			AddFaceTangentSpaceToVertices(pFace, info, i);
		}
	}

	int nTotal = 0;
	MCVertexIndexTable	_VertTable;
	static WORD _pIndexTemp[RVERTEX_MAX*3];
	if(m_FaceCnt==0) 
		return;//bone,dummy,bip ...etc

	int nTriangles = 0;
	for(int i=0;i<m_FaceCnt;i++) {
		nTriangles += m_FaceTable[i]->nDeg-2;
	}

	m_FaceIndexTableCnt = nTriangles * 3;
	m_FaceIndexTable = new WORD [m_FaceIndexTableCnt];
	_ASSERT(m_FaceIndexTableCnt<RVERTEX_MAX*3);

	RVertexIndex vi;
	RMaxFaceData* pFace = NULL;
	//int i,j;//,w2;
	TangentInfo info;	// tangent space info
	bool IsExist = false;
	int nPointPos = 0;
	int w = 0;

	for (int i = 0; i < m_FaceCnt; i ++) 
	{
		pFace = m_FaceTable[i];
		nTotal+= pFace->nDeg;

		for( int j=0;j<pFace->nDeg;j++) 
		{
			vi.p		= pFace->index[j].p;
			vi.n		= pFace->index[j].n;
			vi.uv		= pFace->index[j].uv;
			vi.uv2		= pFace->index[j].uv2;
			//vi.n_tan	= pFace->index[j].n_tan;

			if(bExistUV)
			{
				int nPosIndex = _VertTable.IsExist( vi );

				// Tangent Merge
				RVector4 vSrcTangent = m_TangentOrgTable[ m_pTangentIndex[ i * 3 + j ] ];

				// p ~ uv까지의 인덱스가 모두 같다
				if( nPosIndex > -1 )	// 기본 조건이 만족되면
				{
					// 그렇다면 Parity가 같은가
					RVector4 vDestTanent = m_vecTangentTable[_VertTable[nPosIndex].n_tan];
					if( vSrcTangent.w == vDestTanent.w )
					{
						RVector4 vResult(vSrcTangent.x + vDestTanent.x, vSrcTangent.y + vDestTanent.y, vSrcTangent.z + vDestTanent.z, vSrcTangent.w);

						pFace->index[j].n_tan = _VertTable[nPosIndex].n_tan;
						//m_TangentTanTable[pFace->index[j].n_tan] = vResult;
						m_vecTangentTable[pFace->index[j].n_tan] = vResult;
					}
				}
				else
				{
					//pFace->index[j].n_tan = m_TangentTanTable.Add(vSrcTangent);
					pFace->index[j].n_tan = m_vecTangentTable.size();
					m_vecTangentTable.push_back(vSrcTangent);
				}

				// vertexindex에도 정보를 추가한다.
				vi.n_tan			= pFace->index[j].n_tan;
			}
			else
				vi.n_tan			= 0;

			nPointPos			= _VertTable.Add( vi ,IsExist );
			_pIndexTemp[w+j]	= nPointPos;
		}

		w+= pFace->nDeg;
	}

	int nVertexCount = _VertTable.size();
	_ASSERT( nVertexCount <= 65535 );
	if( nVertexCount >= 65535 )
	{
		char szErrorMessage[MAX_PATH];
		sprintf(szErrorMessage, "정점의 개수가 65535개를 초과하였습니다. ObjectName : %s, VertexCount : %d\n", m_Name.c_str(), nVertexCount);
		MessageBox(NULL, szErrorMessage, "Error", MB_OK);
	}

	mlog("%s : %d vertices -> %d\n", m_Name.c_str(), nTotal, nVertexCount);

	m_VertexIndexTable = new RVertexIndex[nVertexCount];
	m_VertexIndexTableCnt = nVertexCount;

	// Tangent 정규화 하고 최종 Unique 버퍼에 담는다.
	for(i=0;i<nVertexCount;i++) {

		m_VertexIndexTable[i] = _VertTable[i];

		if(bExistUV)
		{
			RVector vTangent  = RVector(m_vecTangentTable[i].x, m_vecTangentTable[i].y, m_vecTangentTable[i].z);
			vTangent.Normalize();

			m_vecTangentTable[i].x = vTangent.x; 
			m_vecTangentTable[i].y = vTangent.y;
			m_vecTangentTable[i].z = vTangent.z;

			m_VertexIndexTable[i].n_tan = m_TangentTanTable.Add(m_vecTangentTable[i]);
		}
	}

	set<RMaxMaterial*> materialsUsed;
	map<RMaxMaterial*,int> subMaterialIDMap;
	for (int i = 0; i < m_FaceCnt; i++)
	{
		_ASSERT( pMesh->m_pMaterialKeeper->IsExistMaterial( m_FaceTable[i]->pMaterial ) );

		materialsUsed.insert(m_FaceTable[i]->pMaterial);
		map<RMaxMaterial*,int>::iterator itr = subMaterialIDMap.find(m_FaceTable[i]->pMaterial);
		if(itr!=subMaterialIDMap.end())
		{
			if(itr->second != m_FaceTable[i]->nSubMaterialID)	//  material 효 id 摸 퓜潔.
			{
				itr->second = 0;
			}
		}
		else
		{
			subMaterialIDMap.insert(map<RMaxMaterial*,int>::value_type(m_FaceTable[i]->pMaterial,m_FaceTable[i]->nSubMaterialID));
		}
	}

	m_MtrlTableInfoCnt = materialsUsed.size();
	m_MtrlTableInfo = new RMtrlTableInfo[m_MtrlTableInfoCnt];

	int nOffset  = 0;
	int nCurrentIndex = 0;
	int nTriangle = 0;
	int nTotalTriangles = 0;
//	int sub_mtrl = 0;

//	RMaxMtrlNode* pMtrl;

	int nMtrlTableID = 0;

	set<RMaxMaterial*>::iterator itor = materialsUsed.begin();
	for (int index = 0; index < m_MtrlTableInfoCnt ; index++, ++itor) 
	{
		RMaxMaterial* pMaterial = *itor;

		nTriangle	= 0;
//		nOffset = nAllFace*3;	// 치
		nOffset = nCurrentIndex;

		int nSrcIndex = 0;
		for (int i = 0; i < m_FaceCnt  ; i ++) {

			RMaxFaceData* pFace = m_FaceTable[i];
			if(pMaterial==pFace->pMaterial)
			{
				for(int j=0;j<pFace->nDeg-2;j++)
				{
					// 원래 triangle 쪼개짐을 유지한다
					m_FaceIndexTable[nCurrentIndex  ] = _pIndexTemp[nSrcIndex+pFace->triangleIndices[j*3]];
					m_FaceIndexTable[nCurrentIndex+1] = _pIndexTemp[nSrcIndex+pFace->triangleIndices[j*3+1]];
					m_FaceIndexTable[nCurrentIndex+2] = _pIndexTemp[nSrcIndex+pFace->triangleIndices[j*3+2]];
					nCurrentIndex+=3;
				}
				nTriangle+= pFace->nDeg-2;
			}

			nSrcIndex+= pFace->nDeg;

//			nFace++;
//			nAllFace++;
		}
		_ASSERT((nCurrentIndex-nOffset)/3 == nTriangle);

		m_MtrlTableInfo[index].mtrlid = pMaterial->GetIndex();
		m_MtrlTableInfo[index].offset = nOffset;
		m_MtrlTableInfo[index].count  = nTriangle;
		
		map<RMaxMaterial*,int>::iterator itr = subMaterialIDMap.find(pMaterial);
		_ASSERT(itr!=subMaterialIDMap.end());
		m_MtrlTableInfo[index].nSubMaterialIDForDrawMasking = itr->second;

		_ASSERT(nTriangle!=0);
		nTotalTriangles+=nTriangle;
	}

	_ASSERT(m_FaceIndexTableCnt==nTotalTriangles*3);
}

void RMaxMeshNode::CalcPhysiquePID(RMaxMesh* pMesh)
{
	if(m_PhysiqueCnt==0) return;

	string strWarningBone;
	bool bWarning = false;

	RPhysiqueInfo* pPInfo;

	int pid;

	// 참조하는 뼈 테이블
	MCIntTable table;

	for(int i=0;i<m_PhysiqueCnt;i++)
	{
		pPInfo = &m_PhysiqueTable[i];

		for(int j=0;j<pPInfo->m_num;j++) 
		{
			int pid = pMesh->GetMeshID(pPInfo->m_Data[j].name);

			if(pid != -1) 
			{
				pPInfo->m_Data[j].cid = table.Add(pid);	//
				pPInfo->m_Data[j].pid = pid;
			}
			else
			{
				bWarning = true;
				strWarningBone = pPInfo->m_Data[j].name.c_str();
			}
		}
	}

	if(bWarning)
	{
		mlog3("%s 가 잘못된 bone ( %s ) 을 참조하고 있습니다", m_Name.c_str(), strWarningBone.c_str() );
		m_PhysiqueCnt = 0;
		return;
	}

	// 상수는 한쪽으로 빼는것도 좋을듯.
	const int MAX_BONE_COUNT_PER_OBJECT = 55;	///< 오브젝트 하나당 최대 뼈 개수

	if(table.size()>=MAX_BONE_COUNT_PER_OBJECT)
	{
		mlog3("%s 가 %d 개 이상의 뼈를 참조하고 있습니다. rs3에서 제대로 보이지 않을 수 있습니다.", m_Name.c_str(), MAX_BONE_COUNT_PER_OBJECT );
		m_PhysiqueCnt = 0;
		return;
	}

	m_BoneTableCnt = table.size();

	m_BoneTable = new D3DXMATRIX[m_BoneTableCnt];
	m_BoneTableIndex = new WORD[m_BoneTableCnt];

	RMaxMeshNode* pMNode;
	D3DXMATRIX inv;

	int pos = 0;

	for(i=0;i<m_BoneTableCnt;i++) {
		pid = table[i];
		pMNode = pMesh->m_MeshTable[pid];
		D3DXMatrixInverse(&inv,NULL,&pMNode->m_matBase);
		m_BoneTable[i] = m_matBase * inv;
		m_BoneTableIndex[i] = pid;
	}
}

bool IsBone(INode* node)
{
	ObjectState os = node->EvalWorldState(0); 
	if(!os.obj) return false;
	if(os.obj->ClassID() == Class_ID(BONE_CLASS_ID,0)) return true;
	if(os.obj->ClassID() == BONE_OBJ_CLASSID) return true;

	// check for biped node
	Control *pControl;
	pControl = node->GetTMController();
	if((pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) || 
		(pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID) || 
		(pControl->ClassID() == FOOTPRINT_CLASS_ID) ) 
		return true;
	
	return false;
}


void RMaxMeshNode::CalcName2Type(INode* node)
{
	char* pName = (char*)m_Name.c_str();

	bool bBone = IsBone(node);
	if(bBone)
		AddFlag(RM_FLAG_BONE_MESH);

#define NCMPNAME(name,n)	(_strnicmp(pName,name,n)==0)
#define CMPNAME(name)		(_stricmp(pName,name)==0)

	if(NCMPNAME("Bip",3)) {		
		AddFlag(RM_FLAG_DUMMY_MESH);
	}
	else if(NCMPNAME("Bone",4))	{
		AddFlag(RM_FLAG_DUMMY_MESH);
	}
	else if(NCMPNAME("Dummy",5)) {
		AddFlag(RM_FLAG_DUMMY_MESH);
	}
	else if(NCMPNAME(RTOK_MAX_CELL,strlen(RTOK_MAX_CELL))) {
		AddFlag(RM_FLAG_DUMMY_MESH);
	}

#undef NCMPNAME


	const char *SET_ALIGN = "set_align";
	const char *SET_ALIGN_Z = "set_align_z";
	const char *LIGHTMAP = "lightmap";
	const char *SET_FX_GHOST = "set_fx_ghost";

	TSTR userPropBuffer;
	node->GetUserPropBuffer(userPropBuffer);
	AddFlag(RM_FLAG_COLLISION_MESH);

	istringstream iStringBuffer(userPropBuffer.data());
	string aLine;
	string aValue;
	while(getline(iStringBuffer,aLine))
	{
		// trim right
		int n = aLine.find_last_not_of(" \n\r");
		if(n==string::npos) continue;
		string buf = aLine.substr(0,n+1);

		stringstream::pos_type spos;
		stringstream sstream(buf); // string을 stream에 넣는다
		sstream >> aLine;
		
		// 이전 버젼 -------------------------------------------------
		if(strcmp(aLine.c_str(),"set_cloth_mesh")==0)
			AddFlag(RM_FLAG_CLOTH_MESH);
		if(strcmp(aLine.c_str(),"set_collision_cloth_mesh")==0)
			AddFlag(RM_FLAG_COLLISION_CLOTH_MESH);

		// 새 포멧 ---------------------------------------------------
		if(strcmp(aLine.c_str(),"set_cloth")==0)
		{
			sstream >> aLine;	// = 
			sstream >> aLine;	// true / false
			if(strcmp(aLine.c_str(), "true") == 0)
				AddFlag(RM_FLAG_CLOTH_MESH);
		}
		if(strcmp(aLine.c_str(),"set_cloth_collision")==0)
		{
			sstream >> aLine;	// = 
			sstream >> aLine;	// true / false
			if(strcmp(aLine.c_str(), "true") == 0)
				AddFlag(RM_FLAG_COLLISION_CLOTH_MESH);
		}

		if(strcmp(aLine.c_str(),"set_collision")==0)
		{
			sstream >> aLine;	// = 
			sstream >> aLine;	// true / false
			if(strcmp(aLine.c_str(),"false")==0)
				DelFlag(RM_FLAG_COLLISION_MESH);
		}

		if(strcmp(aLine.c_str(),"set_collisiononly")==0)
		{
			sstream >> aLine;	// = 
			sstream >> aLine;	// true / false
			if(strcmp(aLine.c_str(),"true")==0)
				AddFlag(RM_FLAG_COLLISION_MESHONLY);
		}

		if(strcmp(aLine.c_str(),"set_occluder")==0)
		{
			sstream >> aLine;	// = 
			sstream >> aLine;	// true / false
			if(strcmp(aLine.c_str(),"true")==0)
				AddFlag(RM_FLAG_OCCLUDER);
		}

		if(strcmp(aLine.c_str(),"set_visible")==0)	// false 일때만 숨김
		{
			sstream >> aLine;	// = 
			sstream >> aLine;	// true / false
			if(strcmp(aLine.c_str(),"false")==0)
				AddFlag(RM_FLAG_HIDE);
		}

		if(strcmp(aLine.c_str(),"set_shade")==0)
		{
			sstream >> aLine;	// = 
			sstream >> aLine;	// true / false
			if(strcmp(aLine.c_str(),"false")==0)
				AddFlag(RM_FLAG_NOSHADE);
		}
		
		// SET_ALIGN_Z 에 SET_ALIGN 이 포함되어있어서 문자열 비교 순서가 이렇다.
		else if(strncmp(aLine.c_str(),SET_ALIGN_Z,strlen(SET_ALIGN_Z))==0) 
		{
			sstream >> aLine;	// =

			// set_align = true // 새 룰
			if(strcmp(aLine.c_str(), "=") == 0)	
			{
				sstream >> aLine;
				if(strcmp(aLine.c_str(), "true") == 0)
					m_nAlign = 2;
			}
			else 	// 이전 포멧
				m_nAlign = 2;	// RMA_Z_FIXED
		}
		else if(strncmp(aLine.c_str(),SET_ALIGN,strlen(SET_ALIGN))==0) 
		{
			sstream >> aLine;	// =

			// set_align = true // 새 룰
			if(strcmp(aLine.c_str(), "=") == 0)	
			{
				sstream >> aLine;
				if(strcmp(aLine.c_str(), "true") == 0)
					m_nAlign = 1;
			}
			else 	// 이전 포멧
				m_nAlign = 1;	// RMA_NORMAL
		}
		else if(strncmp(aLine.c_str(),LIGHTMAP,strlen(LIGHTMAP))==0) 
		{
			// 처리는 mesh 익스포트할때 이미 했다.
			// user property 가 아니므로 넘긴다
		}
		// 유령 효과 셰이더 이펙트 사용 여부
		else if(strncmp(aLine.c_str(),SET_FX_GHOST,strlen(SET_FX_GHOST))==0) 
		{
			sstream >> aLine;	// =

			if(strcmp(aLine.c_str(), "=") == 0)	
			{
				sstream >> aLine;
				if(strcmp(aLine.c_str(), "true") == 0)
					m_rscShaderComponentFlag |= RSC_PPL_GHOST;
			}
		}
		else{
			m_userPropertyString.push_back(buf);
		}
	}
}

void RMaxMeshNode::AddKeys(Matrix3 tm, int nTime, BOOL pos, BOOL rot, BOOL scale)
{
	AffineParts ap;
	decomp_affine(tm, &ap);

	//////////////////////////////////////////////////////////////////////////
	// Position Key
	if(pos)
	{
		Point3 pos = ap.t;

		MCPosKey key;
		key.frame = nTime;
		key.x = pos.x;
		key.y = pos.y;
		key.z = pos.z;
		m_positionKey.push_back(key);
	}

	//////////////////////////////////////////////////////////////////////////
	// Rotation Key
	if(rot)
	{
		_ASSERT(ap.f>0);

		Quat q = ap.q;

		MCQuatKey rotKey;
		rotKey.frame = nTime;

		// 기본적으로 max 는 왼손 좌표계, rs3 의 애니메이션은 오른손 좌표계
		rotKey.x = -q.x;
		rotKey.y = -q.y;
		rotKey.z = -q.z;
		rotKey.w = q.w;
		m_rotationKey.push_back(rotKey);
	}

	//////////////////////////////////////////////////////////////////////////
	// Scale Key
	if(scale)
	{
		MCScaleKey scaleKey;
		scaleKey.frame = nTime;
		scaleKey.x = ap.k.x;
		scaleKey.y = ap.k.y;
		scaleKey.z = ap.k.z;
		m_scaleKey.push_back(scaleKey);
	}
}

bool RMaxMeshNode::IsAttachableMeshNode()
{
	if( m_isHaveRotAni ) return false;
	if( m_isHavePosAni ) return false;

	if( m_PointCnt <= 0 ) return false;
	if( m_FaceCnt <= 0 ) return false;
	if( m_PhysiqueCnt > 0 ) return false;
	if( m_VertexAniCnt > 0 ) return false;
	if( m_BoneTableCnt > 0 ) return false;

	if( m_PointColorCnt > 0 ) return false; // 나중에 제거.

	if( !m_TexCoordTable.size() ) return false;

	if( CheckFlag(RM_FLAG_HIDE) ) return false;
	if( CheckFlag(RM_FLAG_NOSHADE) ) return false;
//	if( CheckFlag(RM_FLAG_COLLISION_MESH) ) return false;
	if( CheckFlag(RM_FLAG_DUMMY_MESH) ) return false;
	if( CheckFlag(RM_FLAG_BONE_MESH) ) return false;
	if( CheckFlag(RM_FLAG_CLOTH_MESH) ) return false;
	if( CheckFlag(RM_FLAG_COLLISION_CLOTH_MESH) ) return false;
	if( CheckFlag(RM_FLAG_COLLISION_MESHONLY) ) return false;
	if( CheckFlag(RM_FLAG_OCCLUDER) ) return false;

	if( m_nAlign != 0 ) return false;

	if( !m_pMtrl->GetRMaterial()->CheckFlag(RM_FLAG_ADDITIVE) &&
		m_pMtrl->GetRMaterial()->CheckFlag(RM_FLAG_USEOPACITY) ) return false;

	return true;
}

void RMaxMeshNode::Attach( vector<RMaxMeshNode*>* pSrcList, vector<RMaxMeshNode*>* pOutDeleteList )
{
	_ASSERT( pSrcList );
	_ASSERT( pSrcList->size() );
	_ASSERT( pOutDeleteList );

	int pointCnt = m_PointCnt;
	int faceCnt = m_FaceCnt;
	for( size_t i = 0; i < pSrcList->size(); ++i )
	{
		RMaxMeshNode* pSrc = (*pSrcList)[i];

		pointCnt += pSrc->m_PointCnt;
		faceCnt += pSrc->m_FaceCnt;
	}

	int faceIndexCnt = faceCnt * 3;
	if( faceIndexCnt > 65535 )
	{
		// 서브 마테리얼 별로 리스트를 나누기 위한 전처리
		set<int> subMtrlKeySet;
		for( size_t i = 0; i < pSrcList->size(); ++i )
		{
			RMaxMeshNode* pSrc = (*pSrcList)[i];

			// SrcList의 FaceTable에 할당 된 SubMtrlID를 조회
			// TODO: 모든 Face를 순회할 필요가 있을까?
			// TODO: 하나의 메시노드가 여러개의 FaceID를 가지고 있는 경우도 있을 수 있다. 일단 무시함.
			_ASSERT( pSrc->m_FaceTable.size() );
			int nSubMaterialID = pSrc->m_FaceTable[0]->nSubMaterialID;
			subMtrlKeySet.insert( nSubMaterialID );
		}
		vector<int> subMtrlKeyList;
		subMtrlKeyList.assign( subMtrlKeySet.begin(), subMtrlKeySet.end() );

		RMaxMeshNode* dest0 = this;
		RMaxMeshNode* dest1 = NULL;
		vector<RMaxMeshNode*> srcList0, srcList1;

		// 모두 같은 서브 마테리얼이면 갯수의 반씩 합치기
		// TODO: 버텍스 수가 균형이 잡히게 나눠야 할까?
		_ASSERT( subMtrlKeyList.size() );
		if( subMtrlKeyList.size() == 1 )
		{
			size_t divideIndex = pSrcList->size() / 2;

			for( size_t i = 0; i < divideIndex; ++i )
			{
				srcList0.push_back( (*pSrcList)[i] );
			}
			for( size_t i = divideIndex; i < pSrcList->size(); ++i )
			{
				if( dest1 == NULL )
					dest1 = (*pSrcList)[i];
				else
					srcList1.push_back( (*pSrcList)[i] );
			}
		}
		// 서브 마테리얼 별로 합치기
		else
		{
			size_t divideIndex = subMtrlKeyList.size() / 2;

			for( size_t i = 0; i < pSrcList->size(); ++i )
			{
				RMaxMeshNode* pSrc = (*pSrcList)[i];
				int nSubMaterialID = pSrc->m_FaceTable[0]->nSubMaterialID;

				for( size_t j = 0; j < divideIndex; ++j )
				{
					if( nSubMaterialID == subMtrlKeyList[j] )
						srcList0.push_back( pSrc );
				}

				for( size_t j = divideIndex; j < subMtrlKeyList.size(); ++j )
				{
					if( nSubMaterialID == subMtrlKeyList[j] )
					{
						if( dest1 == NULL )
							dest1 = pSrc;
						else
							srcList1.push_back( pSrc );
					}
				}
			}
		}

		_ASSERT( dest0 );
		_ASSERT( dest1 );
		dest0->Attach( &srcList0, pOutDeleteList );
		dest1->Attach( &srcList1, pOutDeleteList );
		return;
	}

	// PointTable
	{
		D3DXVECTOR3* pMergePointTable = new D3DXVECTOR3[pointCnt];
		D3DXVECTOR3* pCopyPtr = pMergePointTable;
		memcpy( pCopyPtr, m_PointTable, sizeof(D3DXVECTOR3) * m_PointCnt );
		pCopyPtr += m_PointCnt;

		for( size_t i = 0; i < pSrcList->size(); ++i )
		{
			RMaxMeshNode* pSrc = (*pSrcList)[i];

			memcpy( pCopyPtr, pSrc->m_PointTable, sizeof(D3DXVECTOR3) * pSrc->m_PointCnt );

			D3DXMATRIX matLocalInv;
			D3DXMatrixInverse( &matLocalInv, NULL, &m_matLocal );
			D3DXMATRIX mat = pSrc->m_matLocal * matLocalInv;
			for( int j = 0; j < pSrc->m_PointCnt; ++j )
			{
				D3DXVECTOR4 v;
				D3DXVec3Transform( &v, &pCopyPtr[j], &mat );

				pCopyPtr[j].x = v.x;
				pCopyPtr[j].y = v.y;
				pCopyPtr[j].z = v.z;
			}

			pCopyPtr += pSrc->m_PointCnt;
		}

		SAFE_DELETE_ARRAY( m_PointTable );
		m_PointTable = pMergePointTable;
	}

	// FaceTable
	{
		int pointOffset = m_PointCnt;

		for( size_t i = 0; i < pSrcList->size(); ++i )
		{
			RMaxMeshNode* pSrc = (*pSrcList)[i];

			D3DXMATRIX matLocalInv;
			D3DXMatrixInverse( &matLocalInv, NULL, &m_matLocal );
			D3DXMATRIX mat = pSrc->m_matLocal * matLocalInv;

			for( int j = 0; j < pSrc->m_FaceCnt; ++j )
			{
				// 인덱스 다시 계산
				RMaxFaceData* pFaceData = pSrc->m_FaceTable[j];
				for( int k = 0; k < pFaceData->nDeg; ++k )
				{
					RFaceSubData* pFaceSubData = &pFaceData->index[k];
					pFaceSubData->p += pointOffset;

					// TexCoordTable
					pFaceSubData->uv = m_TexCoordTable.Add( pSrc->m_TexCoordTable[pFaceSubData->uv] );

					// Extra TexCoordTable
					if( pSrc->m_TexCoordTableExtra.size() )
						pFaceSubData->uv2 = m_TexCoordTableExtra.Add( pSrc->m_TexCoordTableExtra[pFaceSubData->uv2] );

					// NormalTable
					D3DXVECTOR3 normal( pSrc->m_NormalTable[pFaceSubData->n] );
					D3DXVec3TransformNormal( &normal, &normal, &mat );
					pFaceSubData->n = m_NormalTable.Add( (float*)&normal );
				}
				m_FaceTable.push_back( pFaceData );
			}
			pSrc->m_FaceTable.clear();

			pointOffset += pSrc->m_PointCnt;
		}
	}

	m_PointCnt = pointCnt;
	m_FaceCnt = faceCnt;

	// 삭제할 메시노드 목록 업데이트
	for( size_t i = 0; i < pSrcList->size(); ++i )
	{
		pOutDeleteList->push_back( (*pSrcList)[i] );
	}
}

void RMaxMesh::CalcBufferIndex()
{
	for(int i=0;i<m_MeshCount;i++) {
		m_MeshTable[i]->CalcBufferIndex(this);
	}

}

void RMaxMesh::CalcPhysiquePID()
{
	for(int i=0;i<m_MeshCount;i++) {
		m_MeshTable[i]->CalcPhysiquePID(this);
	}
}

void RMaxMesh::CalcParentID()
{
	RMaxMeshNode* pNode;

	for(int i=0;i<m_MeshCount;i++) {

		pNode = m_MeshTable[i];

		if( pNode )
			pNode->m_nParentID = GetMeshID( pNode->m_ParentName );
		
	}
}

int RMaxMesh::Add(RMaxMeshNode* data)
{
	_ASSERT(m_MeshTable.size()==m_MeshCount);
	m_MeshTable.push_back(data);
	data->m_nID = m_MeshCount;
	m_MeshCount++;

	return m_MeshCount-1;
}

int RMaxMesh::GetMeshID(string& name)
{
	for(int i=0;i<m_MeshCount;i++) {
		if( m_MeshTable[i]->m_Name == name)
			return m_MeshTable[i]->m_nID;
	}
	return -1;
}

void RMaxMesh::DelAll()
{
	for( size_t i = 0; i < m_MeshTable.size(); ++i )
	{
		delete m_MeshTable[i];
	}
	m_MeshTable.clear();

	RMaxMeshListNode it;
	RMaxMeshNode* pMesh=NULL;
	it =  m_ObstacleMeshList.begin();
	while (it !=  m_ObstacleMeshList.end()) {
		pMesh = (*it);
		delete pMesh;
		it =  m_ObstacleMeshList.erase(it);
	}
}

int RMaxMesh::ani_node_cnt()
{
	int cnt = 0;
	for( size_t i = 0; i < m_MeshTable.size(); ++i )
	{
		RMaxMeshNode* meshNode = m_MeshTable[i];
		if( meshNode->m_isHavePosAni || meshNode->m_isHaveRotAni )
			cnt++;
	}
	return cnt;
}

extern BOOL g_bip_mesh_out;

/**
.obstacle 파일 포맷(XML)
<obstacle version = 1>
  <mesh count = 10>
    <polygon count = 5>
      <vertex>0 0</vertex>
      ... (5 times)
    </polygon>
	<polygon count = 3>
      <vertex>0 0</vertex>
	  ... (3 times)
	</polygon>
  </mesh>
</obstacle>

2008/12/26
.elu에서는 연관되어 있는 SceneNode의 XYZ 값을 이용해서 생성할 수 있지만,
.rs(static mesh)에서는 그렇게 할 수 없다. world position이 원점인 경우가 대부분일 것이므로, .obstacle 정보는 3d 가 되어야 한다.
*/

bool RMaxMesh::export_obstacle(char *filename)
{
	using namespace std;

	if ( m_ObstacleMeshList.empty())
		return false;

	const int OBSTACLE_VERSION = 0;
	vector<vector<MVector3> > vMeshArray;

	vMeshArray.clear();
	for ( RMaxMeshList::iterator i = m_ObstacleMeshList.begin(); i != m_ObstacleMeshList.end(); ++i )
	{
		RMaxMeshNode *pMesnNode = (*i);
		pMesnNode->GetPolygons(vMeshArray);
	}

	int nPolygonNum = static_cast<int>(vMeshArray.size());
	if ( nPolygonNum == 0 )
		return false;

	// xml data 로 저장
	MXml kXml;

	MXmlElement *pRootElement = new MXmlElement("obstacle");
	pRootElement->SetAttribute("version",OBSTACLE_VERSION);
	kXml.Doc()->LinkEndChild(pRootElement);

	MXmlElement *pMeshElement = new MXmlElement("mesh");
	_SetContents(pMeshElement, "");
	pMeshElement->SetAttribute("count", nPolygonNum);
	pRootElement->LinkEndChild(pMeshElement);

	for ( int i = 0; i<nPolygonNum; ++i )
	{
		int nPolygonSize = static_cast<int>(vMeshArray[i].size());

		MXmlElement *pPolygonElement = new MXmlElement("polygon");
		_SetContents(pPolygonElement, "");
		pPolygonElement->SetAttribute("count", nPolygonSize);
		pMeshElement->LinkEndChild(pPolygonElement);

		for ( int j = 0; j<nPolygonSize; ++j )
		{
			MXmlElement *pVertexElement = new MXmlElement("vertex");
			char pcBuffer[128];
			pcBuffer[0] = 0;
			
			const MVector3 &v = vMeshArray[i][j];
			sprintf_s(pcBuffer, sizeof(pcBuffer), "%f %f %f", v.x, v.y, v.z);

			_SetContents(pVertexElement, pcBuffer);
			pPolygonElement->LinkEndChild(pVertexElement);
		}
	}

	kXml.SaveFile(filename);
	return true;
}

int RMaxMesh::GetEffectCount()
{
	int nCount = 0;
	for(int i=0;i<(int)m_MeshTable.size();i++)
	{
		RMaxMeshNode* pMeshNode = m_MeshTable[i];
		nCount+= pMeshNode->m_effectList.size();
	}
	return nCount;
}

void RMaxMesh::DeleteProcessedProperty(const string &strPropertyName)
{
	for(int i=0;i<(int)m_MeshTable.size();i++)
	{
		RMaxMeshNode* pMeshNode = m_MeshTable[i];
		list<string>::iterator it = pMeshNode->m_userPropertyString.begin();
		while ( it != pMeshNode->m_userPropertyString.end() )
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
				pMeshNode->m_userPropertyString.erase(it++);
			}
			else 
				it++;
		}
	}
}

void RMaxMesh::DeleteProcessedProperty(const string &strPropertyName, bool bValue)
{
	for(int i=0;i<(int)m_MeshTable.size();i++)
	{
		RMaxMeshNode* pMeshNode = m_MeshTable[i];
		list<string>::iterator it = pMeshNode->m_userPropertyString.begin();
		while ( it != pMeshNode->m_userPropertyString.end() )
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
				sstream >> aLine;	// =
				sstream >> aLine;	// bool

				// 값까지 같을때만 삭제
				bool bRet = false;
				if( strncmp(aLine.c_str(), "true", aLine.length()) == 0 )
					bRet = true;
				
				if( bRet == bValue )
					pMeshNode->m_userPropertyString.erase(it++);
				else
					it++;
			}
			else
				it++;
		}
	}
}


void RMaxMesh::ExportUserProperty(MXmlElement *pElement)
{
	for(int i=0;i<(int)m_MeshTable.size();i++)
	{
		RMaxMeshNode* pMeshNode = m_MeshTable[i];
		for(list<string>::iterator ip = pMeshNode->m_userPropertyString.begin();
			ip != pMeshNode->m_userPropertyString.end(); ip++)
		{
			MXmlElement* pUserPropertyElement = new MXmlElement("USERPROPERTY");
			pElement->LinkEndChild(pUserPropertyElement);
			pUserPropertyElement->SetAttribute("node",pMeshNode->m_Name);
			pUserPropertyElement->SetAttribute("property",ip->c_str());
		}
	}
}

bool RMaxMesh::export_desc_xml(char* filename, bool bExporyUserProperty)
{
	MXml xml;

	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	MXmlElement* pRootElement = new MXmlElement("XML");
	xml.Doc()->LinkEndChild(pRootElement);

	m_pMaterialKeeper->ExportXML(pRootElement);

	// collision
	MXmlElement* pCollisionElement = new MXmlElement("COLLISION");
	pRootElement->LinkEndChild(pCollisionElement);
	for(int i=0;i<(int)m_collisionList.size();i++)
	{
		MCollision* pCollision = m_collisionList[i];
		pCollision->Save(pCollisionElement);
	}

	if( bExporyUserProperty )
	{
		// 처리된 / 출력할 필요 없는 Property 정리
		DeleteProcessedProperty("set_cloth");		// Flag로 기록 되므로 출력할 필요는 없다
		DeleteProcessedProperty("set_cloth_mesh");
		DeleteProcessedProperty("set_cloth_related_name");
		DeleteProcessedProperty("set_cloth_collision");
		DeleteProcessedProperty("set_export_ignore");
		DeleteProcessedProperty("set_static");
		DeleteProcessedProperty("set_vertexcolor");
		DeleteProcessedProperty("set_shade");
		DeleteProcessedProperty("set_collision");
		DeleteProcessedProperty("set_collisiononly");
		DeleteProcessedProperty("set_water");
		DeleteProcessedProperty("set_visible");
		DeleteProcessedProperty("set_visible_at_night", false);
		DeleteProcessedProperty("add_effect");
		DeleteProcessedProperty("set_addeffect");
		DeleteProcessedProperty("set_effect_name");
		DeleteProcessedProperty("set_effect_scale");
		DeleteProcessedProperty("set_occluder");

		for(int i=0;i<(int)m_MeshTable.size();i++)
		{
			RMaxMeshNode* pMeshNode = m_MeshTable[i];
			if( pMeshNode->FindPropertyString("set_sound = false") )
			{
				pMeshNode->DeleteProcessedProperty("set_sound_name");
				pMeshNode->DeleteProcessedProperty("set_sound_minradius");
				pMeshNode->DeleteProcessedProperty("set_sound_maxradius");
			}
		}

		MXmlElement* pUserPropertyElement = new MXmlElement("USERPROPERTYLIST");
		pRootElement->LinkEndChild(pUserPropertyElement);
		ExportUserProperty(pUserPropertyElement);
	}

	// Alpha2Pass
	if( m_bAlpha2Pass )
	{
		MXmlElement* pAlpha2PassElement = new MXmlElement("ALPHA2PASS");
		pRootElement->LinkEndChild(pAlpha2PassElement);
	}

	// AutoAnimation
	if( m_bAutoAnimation )
	{
		MXmlElement* pAutoAnimationElement = new MXmlElement("AUTOANIMATION");
		pRootElement->LinkEndChild(pAutoAnimationElement);
	}

	char xml_filename[1024];
	strcpy(xml_filename,filename);
	strcat(xml_filename,".xml");

	xml.Doc()->SaveFile(xml_filename);

	return true;
}

void RMaxMesh::CalcBoundBox()
{
	// Physique 데이터를 이용하여 Bone mesh-node 의 AABB를 만든다.
	for (int i = 0; i<m_MeshCount; ++i)
	{
		RMaxMeshNode* pMeshNode = m_MeshTable[i];
		if (pMeshNode == NULL) 
			continue;

		for (int n = 0; n<pMeshNode->m_PhysiqueCnt; ++n)
		{
			RPhysiqueInfo *pPhysiqueInfo = &pMeshNode->m_PhysiqueTable[n];
			int physiqueNum = pPhysiqueInfo->m_num;

			D3DXVECTOR3 v = pMeshNode->m_PointTable[n];

			for (int pn = 0; pn<physiqueNum; ++pn)
			{
				if ( pPhysiqueInfo->GetWeight(pn) < 0.1f )
					continue;

				RMaxMeshNode *pBoneNode = NULL;
				for ( size_t si = 0; si<m_MeshTable.size(); ++si )
				{
					if ( m_MeshTable[si]->m_Name == pPhysiqueInfo->GetName(pn))
						pBoneNode = m_MeshTable[si];
				}
				if ( pBoneNode == NULL)
					continue;

				D3DXMATRIX matInv;
				D3DXMatrixInverse(&matInv, NULL,&pBoneNode->m_matBase);

				D3DXMATRIX mat = /*pMeshNode->m_matBase **/ matInv;
				D3DXVECTOR3 vertex;
				D3DXVec3TransformCoord(&vertex,&v,&mat);

				pBoneNode->m_BoundingBox.Add(RVector(vertex.x, vertex.y, vertex.z));
			}
		}

		for ( int n = 0; n<pMeshNode->m_PointCnt; ++n)
		{
			D3DXVECTOR3 v = pMeshNode->m_PointTable[n];
			pMeshNode->m_BoundingBox.Add(RVector(v.x, v.y, v.z));
		}
	}
}

bool RMaxMesh::export_bin(char* filename)
{
	ex_hd_t t_hd;

	FILE* fp;
	fp = fopen(filename,"wb");
	if(fp==NULL) return false;

	// export 직전에 모든 자료가 모여있으므로, 일단 이 시점에서 만들었다.
	CalcBoundBox();

	RMaxMeshFile mfp(fp);

	int nMtrlIndexCount = 0;

	t_hd.ver = EXPORTER_CURRENT_MESH_VER;
	t_hd.sig = EXPORTER_SIG;
	t_hd.mtrl_num = nMtrlIndexCount;
	t_hd.mesh_num = m_MeshCount;

	mfp.Save((void*)&t_hd,sizeof(ex_hd_t));

	for (int i = 0; i<m_MeshCount; ++i)
	{
		RMaxMeshNode* pMeshNode = m_MeshTable[i];

		if(pMeshNode == NULL) continue;
		pMeshNode->Save(mfp);

		// 옷감 메쉬일 때 정점은 법선과 텍스쳐 좌표가 동일 하여야 한다.
		// 정점이 늘어나게 되면 PhysX의 옷감 생성에서 다른 정점으로 취급된다.
		if( pMeshNode->CheckFlag(RM_FLAG_CLOTH_MESH) == true )
		{
			if( pMeshNode->m_PointCnt != pMeshNode->m_VertexIndexTableCnt )
			{
				char szErrorMessage[MAX_PATH];
				if( pMeshNode->m_PointCnt != pMeshNode->m_NormalTable.size())
				{
					int nDiff = pMeshNode->m_NormalTable.size() - pMeshNode->m_PointCnt;
					sprintf(szErrorMessage, "The vertex modification of the cloth mesh was changed\n 1. To confirm normal : Is more as %d\n", nDiff);
					MessageBox(NULL, szErrorMessage, "Error", MB_OK);
				}
				else
				{
					int nDiff = pMeshNode->m_VertexIndexTableCnt - pMeshNode->m_PointCnt; 
					sprintf(szErrorMessage, "The vertex modification of the cloth mesh was changed\n 1. To confirm Texture UV : Is more as %d\n 2. To confirm mapchannel count\n", nDiff);
					MessageBox(NULL, szErrorMessage, "Error", MB_OK);
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// #514 / 맥스에서 파일을 로드 하고 Focus나 별도의 작업 없이 바로 익스포트시 노말 데이터가 깨지는 현상
		// 일단 에러 메시지만 뜨게 해놓음
		//////////////////////////////////////////////////////////////////////////
		//if( ( pMeshNode->m_NormalTable.size() < pMeshNode->m_TangentTanTable.size() / 3) || ( pMeshNode->m_NormalTable.size() < pMeshNode->m_TangentBinTable.size() / 3 ) )
		//	mlog3("%d NormalTable이 정상적이지 않습니다.", i);
	}

	fclose(fp);

	return true;
}

void rot2quat( RQuatKey& q,RQuatKey& v )
{
	D3DXQUATERNION out;
	D3DXVECTOR3 vec;

	vec.x = v.x;	
	vec.y = v.y;	
	vec.z = v.z;

	D3DXQuaternionRotationAxis(&out,&vec,v.w);

	q.x = out.x;	
	q.y = out.y;	
	q.z = out.z;	
	q.w = out.w;
	q.frame = v.frame;
}

void RMaxMeshNode::OptimizeAnimation()
{
	int m_nDeletedPosKey = m_positionKey.Optimize();
	int m_nDeletedRotKey = m_rotationKey.Optimize();
	int m_nDeletedScaleKey = m_scaleKey.Optimize();
	int m_nDeletedVisKey = m_visKey.Optimize();

	mlog("%s , %d pos key  %d rot key  %d scale key  %d vis key, deleted \n",m_Name.c_str(), 
		m_nDeletedPosKey, m_nDeletedRotKey,m_nDeletedScaleKey, m_nDeletedVisKey);
}

void RMaxMesh::OptimizeKeys()
{
	RMaxMeshNode* pMeshNode;
	for(int i=0;i<m_MeshCount;i++) {
		pMeshNode = m_MeshTable[i];
		if(pMeshNode == NULL) continue;
		pMeshNode->OptimizeAnimation();
	}
}

void RMaxMesh::CalcMaxAniBoundBox()
{
	m_MaxAniBoundingBox.Initialize();

	for (int i = 0; i<m_MeshCount; ++i)
	{
		RMaxMeshNode* pMeshNode = m_MeshTable[i];
		if (pMeshNode == NULL) 
			continue;

		m_MaxAniBoundingBox.Add(pMeshNode->m_MaxAniBoundingBox);
	}
}

void RMaxMesh::AttachMeshNodes()
{
	struct GROUP_KEY
	{
		GROUP_KEY( RMaxMeshNode* pMeshNode ) :
			m_nParentID(pMeshNode->m_nParentID),
			m_pMtrl(pMeshNode->m_pMtrl),
			m_fBaseVisibility(pMeshNode->m_fBaseVisibility),
			m_bCollisionMesh(pMeshNode->CheckFlag(RM_FLAG_COLLISION_MESH) ) {}

		bool IsEqual( const GROUP_KEY& other ) const
		{
			if( m_nParentID == other.m_nParentID &&
				m_pMtrl == other.m_pMtrl &&
				m_fBaseVisibility == other.m_fBaseVisibility &&
				m_bCollisionMesh == other.m_bCollisionMesh )
				return true;

			return false;
		}

	private:
		int				m_nParentID;
		RMaxMaterial*	m_pMtrl;
		float			m_fBaseVisibility;
		bool			m_bCollisionMesh;
	};
	typedef vector<GROUP_KEY> GROUP_KEYS;
	typedef vector<RMaxMeshNode*> ATTACH_LIST;

	GROUP_KEYS	groupKeys;
	ATTACH_LIST	attachList;

	// 합칠 수 있는 메시노드 모으기
	for( size_t i = 0; i < m_MeshTable.size(); ++i )
	{
		RMaxMeshNode* pMeshNode = m_MeshTable[i];

		if( pMeshNode->IsAttachableMeshNode() )
		{
			GROUP_KEY groupKey( pMeshNode );

			bool isNewKey = true;
			for( size_t j = 0; j < groupKeys.size(); ++j )
			{
				if( groupKey.IsEqual( groupKeys[j] ) )
				{
					isNewKey = false;
					break;
				}
			}
			if( isNewKey )
			{
				groupKeys.push_back( groupKey );
			}

			attachList.push_back( pMeshNode );
		}
	}

	vector<RMaxMeshNode*> deleteList;

	// TODO: 일단 마테리얼이 같은 것만.
	// 마테리얼 구별 없이 같은 부모에 속한 메시노드를 다 합칠 수도 있을 것 같다.
	for( size_t i = 0; i < groupKeys.size(); ++i )
	{
		vector<RMaxMeshNode*> srcList;
		RMaxMeshNode* pDest = NULL;

		for( size_t j = 0; j < attachList.size(); ++j )
		{
			RMaxMeshNode* pMeshNode = attachList[j];
			GROUP_KEY targetKey( pMeshNode );

			if( groupKeys[i].IsEqual( targetKey ) )
			{
				if( pDest == NULL )
				{
					pDest = pMeshNode;
				}
				else
				{
					srcList.push_back( pMeshNode );
				}
			}
		}

		if( srcList.size() )
			pDest->Attach( &srcList, &deleteList );
	}

	// 합쳐진 노드들은 삭제
	for( size_t i = 0; i < deleteList.size(); ++i )
	{
		vector<RMaxMeshNode*>::iterator deleteIt = find( m_MeshTable.begin(), m_MeshTable.end(), deleteList[i] );
		if( deleteIt != m_MeshTable.end() )
		{
			delete (*deleteIt);
			m_MeshTable.erase( deleteIt );
			m_MeshCount--;
		}
	}

	// ID 재지정
	for( size_t i = 0; i < m_MeshTable.size(); ++i )
	{
		m_MeshTable[i]->m_nID = i;
	}

	// 부모 ID 재지정
	CalcParentID();
}

bool RMaxMesh::export_ani(char* filename,int mode)
{
	char ani_file_name[256];
	strcpy(ani_file_name, filename);

	strcat(ani_file_name,".ani");

	FILE* fp;
	fp  = fopen(ani_file_name,"wb");
	if(fp==NULL) return false;

	// 모든 메쉬노드의 MaxAniBoundingBox 정보를 모아 애니메이션 전체의 최대 BoundingBox를 만든다.
	CalcMaxAniBoundBox();

	RMaxMeshFile mfp(fp);

	ex_ani_t t_hd;

	t_hd.ver		= EXPORTER_CURRENT_ANI_VER;
	t_hd.sig		= EXPORTER_SIG;
	t_hd.maxframe	= m_nMaxFrame;
	t_hd.model_num	= m_MeshCount;
	t_hd.ani_type	= mode;

	mfp.Save((void*)&t_hd,sizeof(ex_ani_t));

	RMaxMeshNode* pMeshNode = NULL;

	if( mode == RAniType_Vertex) {//Vertex Animation

		int i=0,j=0;

		for(i=0;i<m_MeshCount;i++) { // node 큼

			pMeshNode = m_MeshTable[i];

			if(pMeshNode == NULL) continue;

			mfp.Save( pMeshNode->m_Name );

			mfp.Save(pMeshNode->m_VertexAniCnt);
			mfp.Save(pMeshNode->m_PointCnt);

			mfp.Save((void*)pMeshNode->m_VertexAniFrame,sizeof(DWORD)*pMeshNode->m_VertexAniCnt);

			for(j=0;j<pMeshNode->m_VertexAniCnt;j++) {
				mfp.Save(pMeshNode->m_VertexAniTable[j],pMeshNode->m_PointCnt);
			}

			// AABB 계산
			RBoundingBox aabb;
			aabb.Initialize();
			for ( int count = 0; count <pMeshNode->m_VertexAniCnt; ++count )
				for ( int vindex = 0; vindex<pMeshNode->m_PointCnt; ++vindex)
				{
					D3DXVECTOR3 &v = pMeshNode->m_VertexAniTable[count][vindex];
					aabb.Add(RVector(v.x, v.y, v.z));
				}

			// AABB 저장
			mfp.Save(aabb.vmin.v, sizeof(float)*3);
			mfp.Save(aabb.vmax.v, sizeof(float)*3);

			mfp.Save( (int)pMeshNode->m_visKey.size() );
			for(MCKeyTrack<MCVisKey>::iterator itr = pMeshNode->m_visKey.begin();itr!=pMeshNode->m_visKey.end();itr++) {
				MCVisKey& key = *itr;
				mfp.Save((void*)&key,sizeof(MCVisKey));
			}
		}
	} 
	else {// trans + bone
	
		for(int i=0;i<m_MeshCount;i++) {

			pMeshNode = m_MeshTable[i];
			if(pMeshNode == NULL) continue;

			mfp.Save( pMeshNode->m_Name );
			mfp.Save( pMeshNode->m_ParentName );

//			mfp.Save( pMeshNode->m_matBase );
			mfp.Save( pMeshNode->m_matLocal );

			mfp.Save( (int)pMeshNode->m_positionKey.size() );

			if(pMeshNode->m_positionKey.size())
			{
				for(MCKeyTrack<MCPosKey>::iterator itr = pMeshNode->m_positionKey.begin();itr!=pMeshNode->m_positionKey.end();itr++) {
					const MCPosKey& key = *itr;
					mfp.Save((void*)&key,sizeof(MCPosKey));
				}
			}

			mfp.Save( (int)pMeshNode->m_rotationKey.size() );
			for(MCKeyTrack<MCQuatKey>::iterator itr = pMeshNode->m_rotationKey.begin();itr!=pMeshNode->m_rotationKey.end();itr++) {
				MCQuatKey& key = *itr;
				mfp.Save((void*)&key,sizeof(MCQuatKey));
			}

			mfp.Save( (int)pMeshNode->m_scaleKey.size() );
			for(MCKeyTrack<MCScaleKey>::iterator itr = pMeshNode->m_scaleKey.begin();itr!=pMeshNode->m_scaleKey.end();itr++) {
				MCScaleKey& key = *itr;
				mfp.Save((void*)&key,sizeof(MCScaleKey));
			}

			mfp.Save( (int)pMeshNode->m_visKey.size() );
			for(MCKeyTrack<MCVisKey>::iterator itr = pMeshNode->m_visKey.begin();itr!=pMeshNode->m_visKey.end();itr++) {
				MCVisKey& key = *itr;
				mfp.Save((void*)&key,sizeof(MCVisKey));
			}
		}
	}

	// Max Ani AABB - EXPORTER_ANI_VER8 추가 사항
	mfp.Save((void*)m_MaxAniBoundingBox.vmin.v, sizeof(float)*3);
	mfp.Save((void*)m_MaxAniBoundingBox.vmax.v, sizeof(float)*3);

	if( !m_MaxAniBoundingBox.IsValid() )
		mlog3("애니메이션 AABB 정보가 잘못 익스포트 되었습니다.\n");

	fclose(fp);
	return true;
}
