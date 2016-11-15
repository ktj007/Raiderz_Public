#include "stdafx.h"

#include "RMeshNode.h"
#include "RMesh.h"

#include "RDeviceD3D.h"

#ifdef MESH_STRIP_TEST
#include "../MaxPlugins/NvTriStrip/NvTriStrip.h"
#endif


namespace rs3 {

//////////////////////////////////////////////////////////////////////////////////
// RMeshNodeInfo Class


#if PHYSIQUE_MAX_WEIGHT == 4
 #define	VERTEX_MAX_SIZE	36								// 웨이트 1개 4Byte 추가
#else
 #define	VERTEX_MAX_SIZE	32								
#endif
#define RVERTEX_MAX		1024*100*VERTEX_MAX_SIZE			// 34133 face
#define RMAX_BONE		64									// 한 파츠당 최대64개

bool RBatch::m_pUseHWShader = true;
bool RBatch::m_bisToolApp   = false;
RVector3 RBatch::m_vToolLightPos = RVector3(0,0,0);

MAP_VERTEX_FORMAT RMeshNode::m_mapVertexFormat;
static RMatrix g_matBone[RMAX_BONE]; 


///////////////////////////////////////////////////////////////////////////////////

RMeshNodeInfo::RMeshNodeInfo() : m_nAlign(RMA_NONE)
{
	m_dwFlag = 0;
}

RMeshNodeInfo::~RMeshNodeInfo()
{

}

///////////////////////////////////////////////////////////////////////////////////

RBatch::RBatch()
{
	m_bIsMakeVertexBuffer = false;

	m_pIb = NULL;
	m_pVb = NULL;

#ifdef MESH_STRIP_TEST
	m_pIbDefault = NULL;
	m_pIbOptimized = NULL;
	m_pIbStrips = NULL;
#endif
}

RBatch::~RBatch()
{
	m_bIsMakeVertexBuffer = false;

	SAFE_DELETE(m_pVb);

#ifdef MESH_STRIP_TEST
	SAFE_DELETE(m_pIbDefault);
	SAFE_DELETE(m_pIbOptimized);
	SAFE_DELETE(m_pIbStrips);
#else
	SAFE_DELETE(m_pIb);
#endif


}

bool RBatch::CreateVertexBuffer(char* pVert, const RVertexOffsetInfo& info, HVERTEXFORMAT hvf, HVERTEXFORMAT hvfOnlyStream0, int vert_num, DWORD flag)
{
	if( m_pVb == NULL )
	{
		m_pVb = new RMeshVertexBuffer;
		m_pVb->Create( (void*)pVert, hvf, hvfOnlyStream0, info.pos, info.normal, info.size, vert_num, flag );
	}
	return true;
}

bool RBatch::UpdateVertexBuffer(char* pVert)
{
	if(m_pVb==NULL) return false;

	m_pVb->UpdateData(pVert);

	return true;
}

bool RBatch::UpdateVertexBufferSoft(char* pVert)
{
	if(m_pVb==NULL) return false;

	m_pVb->UpdateDataSW(pVert);

	return true;
}

bool RBatch::CreateIndexBuffer(WORD* pIndex,int _size)
{
	if(m_pIb==NULL) {
		m_pIb = new RMeshIndexBuffer;
		m_pIb->Create(_size,pIndex);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
// RMeshNode Class

RMeshNode::RMeshNode()
// 	: m_pFirstRenderChain(NULL)
// 	, m_pLastRenderChain(NULL)
{
	m_pParentMesh = NULL;
	m_pBaseMesh = NULL;

	m_nParentNodeID = -1;
	m_pParent = NULL;

	m_matBase.MakeIdentity();
	m_matInvBase.MakeIdentity();

	m_bipID = RBIPID_UNKNOWN;
}

RMeshNode::~RMeshNode()
{

}

///////////////////////////////////////////////////////////
bool RMeshNode::ConnectMtrl()
{
	if(m_nFaceCnt==0) return false;//면이 없다면 연결할 필요도 없다..

	if(m_pParentMesh) 
	{

		//pMtrlList = m_pParentMesh->m_pMtrlList;

		m_nMtrlCnt = m_nMtrlTableInfoCnt;

		m_pMtrlTable = new RMaterial*[m_nMtrlCnt];

		for(int i=0;i<m_nMtrlCnt;i++) 
		{
			int nMaterialID = m_pMtrlTableInfo[i].mtrlid;

			// 마테리얼 ID가 잘못되었다.
			if((int)m_pParentMesh->m_vecMtrlList.size() <= nMaterialID || nMaterialID < -1)
			{
				_ASSERT( false );
				mlog("invalid material id , %s - %s\n",m_pParentMesh->GetName().c_str(),m_Name.c_str());
				m_pMtrlTable[i] = RMesh::m_pDefaultMaterial->m_pMaterial;
			}
			// 마테리얼이 연결되지 않은 메시노드다.
			// 마테리얼이 없는 메시노드가 존재 할 수 있다. 이런 경우에 디폴트 마테리얼을 지정해준다.
			else
			if(nMaterialID == -1)
			{
				m_pMtrlTable[i] = RMesh::m_pDefaultMaterial->m_pMaterial;
			}
			else
			{
				m_pMtrlTable[i] = m_pParentMesh->m_vecMtrlList.at(nMaterialID)->m_pMaterial;
			}
		}
	}

	return true;
}

void RMeshNode::UpdateNodeBuffer() // 얼마나 자주불리는지 빈도 조사와 성능조사 필요..
{
	// soft buffer 만 업데이트.. 버텍스 에니메이션이나.. vertex_ani 중에 uv 에니가 있는가? 위치값만 있다...

	RPFC_THISFUNC;
	if(!m_pVb ) 		_ASSERT(0);
	if(!m_pVb->m_pVert)	_ASSERT(0);

	char* vert = m_pVb->m_pVert;
	const int vertexSize = m_pVb->m_nVertexSize;
	int cursor = m_pVb->m_nPosOffset;
	for( int i = 0; i < m_nVertexIndexTableCnt; ++i )
	{
		memcpy( &vert[cursor], m_pPointTable[m_pVertexIndexTable[i].p], RFVF[RFVF_XYZ].size );
		cursor += vertexSize;
	}
}

//////////////////////////////////////////////////////////////////////

void RMeshNode::MakeVertexInfo(RVertexOffsetInfo& info, HVERTEXFORMAT* pVF, HVERTEXFORMAT* pVFOnlyStream0)
{
	_ASSERT(pVF);

	std::vector< RVertexAttributeEx> vecAttributes;
	string keyStr;

	// position
	info.pos = info.size;
	info.size += sizeof(float)*3;
	vecAttributes.push_back( RVertexAttributeEx( 0, TYPE_VERTEX, 0, FORMAT_FLOAT, 3));
	keyStr = "p;";

	// cloth 는 vb 만들때 physique를 만들 필요가 없다
	if( m_nPhysiqueCnt && RBatch::m_pUseHWShader && !isClothMesh() )
	{
		info.weight = info.size;
	#if PHYSIQUE_MAX_WEIGHT == 4
		info.size += sizeof(float)*3 + sizeof(unsigned char)*4;
	#else
		info.size += sizeof(float)*2 + sizeof(unsigned char)*4;
	#endif

		// Weight
	#if PHYSIQUE_MAX_WEIGHT == 4
		vecAttributes.push_back( RVertexAttributeEx( 0, TYPE_BLENDWEIGHT, 0, FORMAT_FLOAT, 3));
	#else
		vecAttributes.push_back( RVertexAttributeEx( 0, TYPE_BLENDWEIGHT, 0, FORMAT_FLOAT, 2));
	#endif
		// index
		vecAttributes.push_back( RVertexAttributeEx( 0, TYPE_BLENDINDICES, 0, FORMAT_UBYTEUN, 4));

		keyStr += "ph;";
	}

	if( m_nNormalTableCnt )
	{
		info.normal = info.size;
		info.size += sizeof(float)*3;

		// normal
		vecAttributes.push_back( RVertexAttributeEx( 0, TYPE_NORMAL, 0, FORMAT_FLOAT, 3));

		keyStr += "n;";
	}

	// cloth 는 vb 만들때 vcolor를 만들 필요가 없다
	if( m_nPointColorCnt && !isClothMesh() )
	{
		info.color = info.size;
		info.size += sizeof(DWORD);

		vecAttributes.push_back( RVertexAttributeEx( 0, TYPE_COLOR, 0, FORMAT_D3DCOLOR, 4));

		keyStr += "c;";
	}

	if( m_nTexCoordTableCnt )
	{
		bool bNormalMap = false;
		for( int i = 0; i < m_nMtrlCnt; i++ )
		{
			if( m_pMtrlTable[i]->GetTexLayerByType(RTT_NORMAL) )
			{
				bNormalMap = true;
				break;
			}
		}

		bool bExtraUV = false;
		if( m_nTexCoordExtraTableCnt )
		{
			bExtraUV = true;
		}

		// Tex0
		info.texCoord = info.size;
		info.size += sizeof(float)*2;
		vecAttributes.push_back( RVertexAttributeEx( 0, TYPE_TEXCOORD, 0, FORMAT_FLOAT, 2));
		keyStr += "uv;";

		if( bExtraUV )
		{
			info.size += sizeof(float)*2;

			// Tex1
			vecAttributes.push_back( RVertexAttributeEx( 0, TYPE_TEXCOORD, 1, FORMAT_FLOAT, 2));
			keyStr += "uv2;";
		}

		if( bNormalMap )
		{
			info.size += sizeof(float)*4;

			vecAttributes.push_back( RVertexAttributeEx( 0, TYPE_TEXCOORD, 2, FORMAT_FLOAT, 4));
			keyStr += "uvn;";
		}
	}

	// 인스턴싱 정보 없이 생성
	MAP_VERTEX_FORMAT::iterator itr = m_mapVertexFormat.find( keyStr);
	if( itr != m_mapVertexFormat.end() )
	{
		*pVFOnlyStream0 = m_mapVertexFormat[ keyStr];
	}
	else
	{
		HVERTEXFORMAT hVFOnlyStream0 = REngine::GetDevice().CreateVertexFormat( (RVertexAttributeEx*)(&(vecAttributes[0])), vecAttributes.size());
		m_mapVertexFormat.insert( MAP_VERTEX_FORMAT::value_type( keyStr, hVFOnlyStream0));
		*pVFOnlyStream0 = hVFOnlyStream0;
	}

	// 인스턴싱용 정보 추가
	vecAttributes.push_back( RVertexAttributeEx( 1, TYPE_TEXCOORD, 3, FORMAT_FLOAT, 1));
	keyStr += "inst;";

	// 중복 체크를 하여 중복되면 새로 생성하지 않고... 스태틱이므로...
	itr = m_mapVertexFormat.find( keyStr);
	if( itr != m_mapVertexFormat.end() )
	{
		*pVF = m_mapVertexFormat[ keyStr];
	}
	else
	{
		HVERTEXFORMAT hVF = REngine::GetDevice().CreateVertexFormat( (RVertexAttributeEx*)(&(vecAttributes[0])), vecAttributes.size());
		m_mapVertexFormat.insert( MAP_VERTEX_FORMAT::value_type( keyStr, hVF));
		*pVF = hVF;
	}
}

bool RMeshNode::MakeNodeBuffer(DWORD flag)
{
	static char	_pVert[RVERTEX_MAX];// 34133 face
//	static WORD _pIndex[RVERTEX_MAX*3];
//	static WORD _pIndexTemp[RVERTEX_MAX*3];

	_ASSERT(m_nFaceCnt*3 * VERTEX_MAX_SIZE <= RVERTEX_MAX - 20 );
	if(m_nFaceCnt*3 * VERTEX_MAX_SIZE > RVERTEX_MAX - 20 )
	{
		mlog("elu 단일 오브젝트 버텍스 갯수가 %d 개가 넘습니다\n", RVERTEX_MAX-20 );
		return false;
	}

	RVertexOffsetInfo vertexInfo;
	HVERTEXFORMAT hVF;
	HVERTEXFORMAT hVFOnlyStream0;
	MakeVertexInfo( vertexInfo, &hVF, &hVFOnlyStream0);

	//////////////////////////////////////////////////////////////////////////

	RVector* pP  = m_pPointTable;
	RVector* pN  = m_pNormalTable;
	RVector* pUV = m_pTexCoordTable; // uv 는 없을수도 있다?
	RVector* pUVExtra = m_pTexCoordExtraTable;
	RVector4* pTan = m_pTangentTanTable;

	if( vertexInfo.pos >= 0 )
	{
		int cursor = vertexInfo.pos;
		for( int i = 0; i < m_nVertexIndexTableCnt; ++i )
		{
			memcpy( &_pVert[cursor], &pP[m_pVertexIndexTable[i].p], RFVF[RFVF_XYZ].size );
			cursor += vertexInfo.size;
		}
	}

	if( vertexInfo.weight >= 0 )
	{
#if PHYSIQUE_MAX_WEIGHT == 4
		// weight가 4개인 경우는 마지막 하나는 1-a로 유추 가능하므로 vector3에 저장 
		RVector3 w( 1.f, 0.f, 0.f);
#else
		RVector2 w( 1.f, 0.f );
#endif

		RVector n;

		int nPosIndex;
		BYTE matIndex;

		RPhysiqueInfo* pPhysique;

		m_pPointNormalTable = new RVector[m_nVertexIndexTableCnt];

		int cursor = vertexInfo.weight;
		for( int i = 0; i < m_nVertexIndexTableCnt; ++i )
		{
			nPosIndex = m_pVertexIndexTable[i].p;

			pPhysique = &m_pPhysique[nPosIndex];
			// 현재 웨이트가 3개로 제한 되어 있는데 4개로 늘리면 여기 수정 필요 
			w.x = 1.f;
			w.y = 0.f;
#if PHYSIQUE_MAX_WEIGHT == 4
			w.z = 0.f;	
#endif
// 			if( pPhysique->m_num > 1 )
// 			{
// 				w.x = pPhysique->GetWeight(0);
// 				w.y = pPhysique->GetWeight(1);
// #if PHYSIQUE_MAX_WEIGHT == 4
// 				if( pPhysique->m_num > 3)	// m_num의 갯수 체크 필요한가 
// 					w.z = pPhysique->GetWeight(2);
// #endif
// 			}

			if( pPhysique->m_num > 1)
			{
				for( int k=0; k<pPhysique->m_num; k++)
				{
					switch( k)
					{
					case 0:	w.x = pPhysique->GetWeight(0);	break;
					case 1:	w.y = pPhysique->GetWeight(1);	break;
#if PHYSIQUE_MAX_WEIGHT == 4
					case 2:	w.z = pPhysique->GetWeight(2);	break;
#endif
					}
				}
			}


			memcpy( &_pVert[cursor], &w, sizeof(w) );

			n = pN[m_pVertexIndexTable[i].n];
			m_pPointNormalTable[i] = n;

			memset( &_pVert[cursor + sizeof(w)], 0, sizeof(float) );
			for( int k = 0 ; k < pPhysique->m_num; ++k )
			{
				matIndex = pPhysique->GetBoneID(k);
				memcpy( &_pVert[cursor + sizeof(w) + sizeof(BYTE)*k], &matIndex, sizeof(BYTE) );
			}

			cursor += vertexInfo.size;
		}
	}

	if( vertexInfo.normal >= 0 )
	{
		int cursor = vertexInfo.normal;
		for( int i = 0; i < m_nVertexIndexTableCnt; ++i )
		{
			memcpy( &_pVert[cursor], &pN[m_pVertexIndexTable[i].n], RFVF[RFVF_NORMAL].size );
			cursor += vertexInfo.size;
		}
	}

	if( vertexInfo.color >= 0 )
	{
		int cursor = vertexInfo.color;
		for( int i = 0; i < m_nVertexIndexTableCnt; ++i )
		{
			RVector* pColor = &m_pPointColorTable[m_pVertexIndexTable[i].p];

			float f = (pColor->x + pColor->y + pColor->z) / 3.f;
			int gray = f * 255;
			if( gray > 255 )
				gray = 255;

			DWORD dwColor = D3DCOLOR_ARGB( gray, 255, 255, 255);

			memcpy( &_pVert[cursor], &dwColor, RFVF[RFVF_DIFFUSE].size );
			cursor += vertexInfo.size;
		}
	}

	if( vertexInfo.texCoord >= 0  )
	{
		bool bNormalMap = false;
		for( int i = 0; i < m_nMtrlCnt; i++ )  
		{
			if( m_pMtrlTable[i]->GetTexLayerByType(RTT_NORMAL) )
			{
				bNormalMap = true;
				break;
			}
		}

		bool bExtraUV = false;
		if( m_nTexCoordExtraTableCnt )
		{
			bExtraUV = true;
		}

		if( bNormalMap && bExtraUV )
		{
			int cursor = vertexInfo.texCoord;
			for( int i = 0; i < m_nVertexIndexTableCnt; ++i )
			{
				WORD uvIndex = m_pVertexIndexTable[i].uv;
				_ASSERT( uvIndex < m_nTexCoordTableCnt );
				memcpy( &_pVert[cursor], &pUV[uvIndex], RFVF[RFVF_TEX1].size );

				WORD uv2Index = m_pVertexIndexTable[i].uv2;
				_ASSERT( uv2Index < m_nTexCoordExtraTableCnt );
				memcpy( &_pVert[cursor + RFVF[RFVF_TEX1].size], &pUVExtra[uv2Index], RFVF[RFVF_TEX2].size );

				WORD tanIndex = m_pVertexIndexTable[i].n_tan;
				_ASSERT( tanIndex < m_nTangentTanTableCnt );
				memcpy( &_pVert[cursor + RFVF[RFVF_TEX1].size + RFVF[RFVF_TEX2].size], &pTan[tanIndex], RFVF[RFVF_TEX3_TEX3SIZE4].size );

				cursor += vertexInfo.size;
			}
		}
		else if( bNormalMap )
		{
			int cursor = vertexInfo.texCoord;
			for( int i = 0; i < m_nVertexIndexTableCnt; ++i )
			{
				WORD uvIndex = m_pVertexIndexTable[i].uv;
				_ASSERT( uvIndex < m_nTexCoordTableCnt );
				memcpy( &_pVert[cursor], &pUV[uvIndex], RFVF[RFVF_TEX1].size );

				WORD tanIndex = m_pVertexIndexTable[i].n_tan;
				_ASSERT( tanIndex < m_nTangentTanTableCnt );
				memcpy( &_pVert[cursor + RFVF[RFVF_TEX1].size], &pTan[tanIndex], RFVF[RFVF_TEX2_TEX2SIZE4].size );

				cursor += vertexInfo.size;
			}
		}
		else if( bExtraUV )
		{
			int cursor = vertexInfo.texCoord;
			for( int i = 0; i < m_nVertexIndexTableCnt; ++i )
			{
				WORD uvIndex = m_pVertexIndexTable[i].uv;
				_ASSERT( uvIndex < m_nTexCoordTableCnt );
				memcpy( &_pVert[cursor], &pUV[uvIndex], RFVF[RFVF_TEX1].size );

				WORD uv2Index = m_pVertexIndexTable[i].uv2;
				_ASSERT( uv2Index < m_nTexCoordExtraTableCnt );
				memcpy( &_pVert[cursor + RFVF[RFVF_TEX1].size], &pUVExtra[uv2Index], RFVF[RFVF_TEX2].size );

				cursor += vertexInfo.size;
			}
		}
		else
		{
			int cursor = vertexInfo.texCoord;
			for( int i = 0; i < m_nVertexIndexTableCnt; ++i )
			{
				WORD uvIndex = m_pVertexIndexTable[i].uv;
				_ASSERT( uvIndex < m_nTexCoordTableCnt );
				memcpy( &_pVert[cursor], &pUV[uvIndex], RFVF[RFVF_TEX1].size );

				cursor += vertexInfo.size;
			}
		}
	}

	// 원본 보관
	_ASSERT(m_pPointTableOrg==NULL);
	m_pPointTableOrg = new RVector[m_nPointCnt];
	memcpy(m_pPointTableOrg, m_pPointTable,sizeof(RVector) * m_nPointCnt);

	bool bCreateVertexBuffer = CreateVertexBuffer( _pVert, vertexInfo, hVF, hVFOnlyStream0, m_nVertexIndexTableCnt, flag );
	_ASSERT(bCreateVertexBuffer);

	if( m_nFaceIndexTableCnt )
		CreateIndexBuffer( m_pFaceIndexTable , m_nFaceIndexTableCnt );

	m_bIsMakeVertexBuffer = true;

#ifdef MESH_STRIP_TEST

	m_pIbDefault = m_pIb;

	WORD* pOptimizedIndicies = new WORD[m_pIb->m_size];
	
	int nCurrentStripOffset = 0;
	vector<PrimitiveGroup*> primitiveGroupsStrips;

	m_pIb->Lock();
	for(int i=0;i<m_nMtrlCnt;i++)
	{
		// optimized list

		PrimitiveGroup* g_pPrimitiveGroupsList;
		PrimitiveGroup* g_pPrimitiveGroupsStrip;

		/////////////////////////////////////////////////////////////////////////
		// NVTriStrip setup code
		SetCacheSize(16);
		SetStitchStrips(true);
		SetMinStripSize(0);

		//First, we create the optimized list indices
		SetListsOnly(true);

		unsigned short* pIndices = m_pIb->m_i + m_pMtrlTableInfo[i].offset;

		unsigned short numSections;
		GenerateStrips(pIndices, m_pMtrlTableInfo[i].count * 3 , &g_pPrimitiveGroupsList, &numSections);

		//copy optimized data to index buffer
		memcpy(pOptimizedIndicies+m_pMtrlTableInfo[i].offset,g_pPrimitiveGroupsList[0].indices,sizeof(WORD)*m_pMtrlTableInfo[i].count*3);
		delete []g_pPrimitiveGroupsList;

		//Next, we create the optimized strip indices
		SetListsOnly(false);

		GenerateStrips(pIndices, m_pMtrlTableInfo[i].count * 3, &g_pPrimitiveGroupsStrip, &numSections);
		primitiveGroupsStrips.push_back(g_pPrimitiveGroupsStrip);
		
		m_pMtrlTableInfoStrips[i].offset = nCurrentStripOffset;
		m_pMtrlTableInfoStrips[i].count = g_pPrimitiveGroupsStrip[0].numIndices-2;
		m_pMtrlTableInfoStrips[i].mtrlid = m_pMtrlTableInfo[i].mtrlid;
		m_pMtrlTableInfoStrips[i].nSubMaterialIDForDrawMasking = m_pMtrlTableInfo[i].nSubMaterialIDForDrawMasking;

		nCurrentStripOffset+=g_pPrimitiveGroupsStrip[0].numIndices;

		/*
		//copy optimized data to index buffer
		pd3dDevice->CreateIndexBuffer(g_pPrimitiveGroupsStrip[0].numIndices * sizeof(unsigned short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
		D3DPOOL_DEFAULT, &g_pIndexBufferOptStrip, NULL);
		g_pIndexBufferOptStrip->Lock(0, g_pPrimitiveGroupsStrip[0].numIndices * sizeof(unsigned short), (void**)&pDest, 0);
		memcpy(pDest, g_pPrimitiveGroupsStrip[0].indices, g_pPrimitiveGroupsStrip[0].numIndices * sizeof(unsigned short));
		g_pIndexBufferOptStrip->Unlock();
		*/
	}
	m_pIb->Unlock();

	m_pIbOptimized = new RMeshIndexBuffer;
	m_pIbOptimized->Create(m_pIb->m_size,pOptimizedIndicies);
	delete[] pOptimizedIndicies;

	int nStripIndexSize = nCurrentStripOffset;
	m_pIbStrips = new RMeshIndexBuffer;
	m_pIbStrips->Create(nStripIndexSize,NULL);
	m_pIbStrips->Lock();
	for(int i=0;i<m_nMtrlCnt;i++)
	{
		PrimitiveGroup* pPrimitiveGroup = primitiveGroupsStrips[i];

		memcpy(m_pIbStrips->m_i+ m_pMtrlTableInfoStrips[i].offset , pPrimitiveGroup[0].indices, sizeof(WORD)*(m_pMtrlTableInfoStrips[i].count+2) );
		delete []pPrimitiveGroup;
	}
	m_pIbStrips->Unlock();
	m_pIbStrips->m_PrimitiveType = RPT_TRIANGLESTRIP;

	mlog("%s , %d trianges , %d strips polygons\n",m_Name.c_str(), m_pIb->m_size, nStripIndexSize);
#endif
	return true;
}

bool RMeshNode::CheckMaterialFlag(DWORD dwFlag)
{
	for (int index = 0; index < m_nMtrlCnt ; ++index) 
	{
		RMaterial* pMtrl = GetMtrl(index);
		if(	pMtrl && pMtrl->CheckFlag(dwFlag))
			return true;
	}
	return false;
}

bool RMeshNode::hasAlphaMaterial()
{
	for (int index = 0; index < m_nMtrlCnt ; index ++) {
		RMaterial* pMtrl = GetMtrl(index);
		if(	pMtrl && 
			(pMtrl->CheckFlag(RM_FLAG_USEOPACITY) ||
			pMtrl->CheckFlag(RM_FLAG_ADDITIVE) ))
			return true;
	}
	return false;
}

bool RMeshNode::hasAdditiveMaterial()
{
	return CheckMaterialFlag(RM_FLAG_ADDITIVE);
}

bool RMeshNode::hasTranslucentMaterial()
{
	return CheckMaterialFlag(RM_FLAG_USEOPACITY);
}

}
