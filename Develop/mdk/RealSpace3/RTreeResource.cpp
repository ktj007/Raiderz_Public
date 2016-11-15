#include "stdafx.h"

#include "RResourceID.h"

#include "RTreeResource.h"
#include "RTreeResourceMgr.h"
#include "RTreeSceneNode.h"
#include "RTreeCollision.h"
#include "RSceneManager.h"
#include "RResourceLoader.h"


namespace rs3 {


RTreeInstancingBuffer	RTreeResource::m_InstancingBuffer;	// 우선은 모든 인스턴스가 하나 공유


const char* RTreeResource::GetTextureLayerName ( TREE_TEXTRUE eTexture )
{
	const char* szLayerNames[] = {
		"Branch",
		"BranchDetail",
		"Self_Shadow",
		"Composite",
		"Billboard",
		"BillBoardNormal",

		"_end"
	};

	return szLayerNames[eTexture];
}

RTreeResource::RTreeResource(const char* _pSzResourceName, RSceneManager* pSceneManager) : 
	RResource( _pSzResourceName, RRESOURCETYPE::RR_TREE ),
	m_pSpeedTree(NULL), 
	m_pTextureInfo(NULL), 
	m_pGeometryCache(NULL), 
	m_pCollisionInfo(NULL),
	m_pFirstRenderChain(NULL),
	m_pLastRenderChain(NULL),
	m_pSceneManager(pSceneManager),
	m_unBranchVertexCount(0),
	m_unFrondVertexCount(0),
	m_fLeafRockScalar(1.f),
	m_fLeafRustleScalar(1.f),
	m_nBillboardNum(0),
	m_pBranchBuffer(NULL),
	m_pFrondBuffer(NULL),
	m_pLeafCardBuffer(NULL),
	m_pLeafMeshBuffer(NULL)
{
	memset( m_arrayTreeTexture, 0, sizeof(m_arrayTreeTexture) );
	m_Box.Initialize();

	m_pBranchBuffer = new RTreeMeshBuffer<SFVFBranchVertex_GPU_Light>(REngine::GetDevicePtr(), BT_INDEXEDSTRIP);
	m_pFrondBuffer = new RTreeMeshBuffer<SFVFFrondVertex_GPU_Light>(REngine::GetDevicePtr(), BT_INDEXEDSTRIP);
	m_pLeafCardBuffer = new RTreeMeshBuffer<SFVFLeafVertex_GPU_Light>(REngine::GetDevicePtr(), BT_INDEXEDTRIANGLELIST);
	m_pLeafMeshBuffer = new RTreeMeshBuffer<SFVFLeafMeshVertex_GPU_Light>(REngine::GetDevicePtr(), BT_INDEXEDTRIANGLELIST);
}

RTreeResource::~RTreeResource()
{
	SAFE_DELETE(m_pCollisionInfo);
	SAFE_DELETE(m_pGeometryCache);
	SAFE_DELETE(m_pTextureInfo);

	SAFE_DELETE(m_pBranchBuffer);
	SAFE_DELETE(m_pFrondBuffer);
	SAFE_DELETE(m_pLeafCardBuffer);
	SAFE_DELETE(m_pLeafMeshBuffer);

	SAFE_DELETE(m_pSpeedTree);

	for( int i = 0; i < SPT_TEXTURE_NUM; ++i)
	{
// 		if(m_arrayTreeTexture[i])
// 			m_arrayTreeTexture[i]->RemoveLoadingController(this);
		REngine::GetDevice().DeleteTexture( m_arrayTreeTexture[ i ] );
	}
}

const char* RTreeResource::GetTextureFileName(const char* filename)
{
	int i=strlen(filename)-1;
	for (; i>=0; i--)
	{
		if (filename[i] == '\\') break;
	}

	return &(filename[i + 1]);
}

bool RTreeResource::Fill()
{
// 	_ASSERT( m_state==RRS_EMPTY );
	_ASSERT( REngine::GetSceneManager().GetResourceMgr< RTreeResourceMgr >(false) != NULL );

	// speed tree object
	m_pTextureInfo = new CSpeedTreeRT::SMapBank;
	m_pGeometryCache = new CSpeedTreeRT::SGeometry;
	m_pCollisionInfo = new RTreeCollision;

	// CSpeedTreeRT 생성
	_ASSERT( m_pSpeedTree == NULL && "Fill 은 한번만 해야하는건데, 여러번되고 있는건 아닌지 확인해보세요.");

#pragma push_macro ("new")
#ifdef new
#undef new
#endif
	m_pSpeedTree = new CSpeedTreeRT;
#pragma pop_macro ("new")

	m_pSpeedTree->SetTextureFlip(true);

	// IO 의존적인 파일 읽기
	MFile file;
	bool bRet = file.Open(GetName().c_str());

	if(!bRet)
	{
		_ASSERT(0);
// 		m_state = RRS_WRONG;
		return false;
	}

	int len = file.GetLength();
	unsigned char* buf = new unsigned char[len];

	bRet = file.ReadAll(buf, len);
	if( !bRet )
	{
		_ASSERT(0);
// 		m_state = RRS_WRONG;
		return false;
	}

	bool bLoaded = m_pSpeedTree->LoadTree(buf, len);

	delete[] buf;
	file.Close();

	// Device에 상관 없는 기본적인 설정
	if (bLoaded)
	{
		m_pSpeedTree->SetBranchLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
		m_pSpeedTree->SetFrondLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);
		m_pSpeedTree->SetLeafLightingMethod(CSpeedTreeRT::LIGHT_DYNAMIC);

		m_pSpeedTree->SetBranchWindMethod(CSpeedTreeRT::WIND_GPU);
		m_pSpeedTree->SetLeafWindMethod(CSpeedTreeRT::WIND_GPU);
		m_pSpeedTree->SetFrondWindMethod(CSpeedTreeRT::WIND_GPU);

		m_pSpeedTree->GetLightShaderParams(m_LightParam);
		m_pSpeedTree->SetNumLeafRockingGroups(1);	

		// override the size, if necessary
		if (m_fSize >= 0.0f && m_fSizeVariance >= 0.0f)
			m_pSpeedTree->SetTreeSize(m_fSize, m_fSizeVariance);

		float fTSize, fTSizeV;
		m_pSpeedTree->GetTreeSize(fTSize, fTSizeV);
		m_pSpeedTree->SetTreeSize(fTSize*1, fTSizeV*1);

		// generate tree geometry
		if (m_pSpeedTree->Compute(NULL, m_nSeed))
		{
			// get the dimensions
			m_pSpeedTree->GetBoundingBox(m_afBoundingBox);

			// make the leaves rock in the wind
			m_pSpeedTree->SetLeafRockingState(true);

			// query textures
			m_pSpeedTree->GetMapBank(*m_pTextureInfo);

			// read geometry cache
			m_pSpeedTree->GetGeometry(*m_pGeometryCache);

			// set tree material
			m_BranchMaterial.ScaleAmbient( GetLightAmbient() );
			m_FrondMaterial.ScaleAmbient( GetLightAmbient() );
			m_LeafMaterial.ScaleAmbient( GetLightAmbient() );
			m_pCollisionInfo->UpdateCollisionObject(this);

			LoadTextures();
		}
		else 
		{
			m_vErrorMessages.push_back( GetName() + " : 심각한 내부 오류 발생.");
			_ASSERT(0 && "Compute에 실패");

//			m_state = RRS_WRONG;
			return false;
		}
	}
	else // failed to load
	{
		//const char *szErrorString = CSpeedTreeRT::GetCurrentError( );
		m_vErrorMessages.push_back( GetName() + " : spt파일을 찾을 수 없음.");
		_ASSERT(0 && "spt 로딩에 실패, spt가 없거나 잘못된 파일. 혹은 스피드트리 인증이 안됨");

//		m_state = RRS_WRONG;
		return false;
	}

	// fill buffer
	CSpeedWind* pSpeedWind = REngine::GetSceneManager().GetResourceMgr<RTreeResourceMgr>()->GetSpeedWind();
	if (! FillBranchBuffer(pSpeedWind) ) return false;
	if (! FillFrondBuffer(pSpeedWind) ) return false;
	if (! FillLeafCardBuffer(pSpeedWind) ) return false;
	if (! FillLeafMeshBuffer(pSpeedWind) )return false;
	if (! FillBillboardBuffer() ) return false;

//	m_state = RRS_FILLED;
	return true;
}

RResource::LOAD_RESULT RTreeResource::Load()
{
//	_ASSERT(m_state==RRS_FILLED || m_state==RRS_LOST);

	bool bBackgroundLoading = IsBackgroundCreated();

	// 아직 로딩중이라면 대기한다
	if(bBackgroundLoading)
	{
		for( int i = 0; i < SPT_TEXTURE_NUM; ++i )
		{
			RTexture* pTreeTexture = m_arrayTreeTexture[ i ];
			if( pTreeTexture && !pTreeTexture->IsLoadingCompleted()) 
				return WAITING;
		}
	}

	// 텍스쳐 로딩은 완료

	// 텍스쳐가 원래 없거나 잘못 로딩된 경우 기본텍스쳐로 대체
	for( int i = 0; i < SPT_TEXTURE_NUM; ++i )
	{
		RTexture* pTreeTexture = m_arrayTreeTexture[ i ];
		if( !pTreeTexture || pTreeTexture->GetState()==RRS_WRONG)
		{
			if(pTreeTexture)
//			{
//				pTreeTexture->RemoveLoadingController(this);
				REngine::GetDevice().DeleteTexture( pTreeTexture );
//			}

			m_vErrorMessages.push_back( GetName() + " : " + GetTextureLayerName((TREE_TEXTRUE)i) +" 텍스쳐 없음.");
			if(IsNormalMap((TREE_TEXTRUE)i))
				m_arrayTreeTexture[ i ] = REngine::GetDevice().CreateTexture( "tree_blue" );
			else
				m_arrayTreeTexture[ i ] = REngine::GetDevice().CreateTexture( "tree_gray" );
		}
	}

	// load buffer
	if (! LoadBranchBuffer()) return WRONG;
	if (! LoadFrondBuffer()) return WRONG;
	if (! LoadLeafCardBuffer()) return WRONG;
	if (! LoadLeafMeshBuffer()) return WRONG;

	// for debug
// 	for( size_t i = 0; i<m_vErrorMessages.size(); i++)
// 	{
// 		mlog("%s\n", m_vErrorMessages[i].c_str() );
// 	}

	return LOADED;
}

// 나무 모델이 로딩되었으면 우선순위를 높인다
int RTreeResource::CalcPriority()
{
	if( GetState()==RRS_EMPTY ) 
		return RBGWK_SLEEP;

	return RBGWK_HIGHEST+1;
}

bool RTreeResource::LoadTextures()
{
	bool bBackgroundLoading = IsBackgroundCreated();

	//load texture --
	m_strTextureNames[ SPT_BRAHCH ]			= m_pTextureInfo->m_pBranchMaps[CSpeedTreeRT::TL_DIFFUSE];
	m_strTextureNames[ SPT_BRANCH_DETAIL ]	= m_pTextureInfo->m_pBranchMaps[CSpeedTreeRT::TL_DETAIL];
	m_strTextureNames[ SPT_SELF_SHADOW ]	= m_pTextureInfo->m_pSelfShadowMap;
	m_strTextureNames[ SPT_COMPOSITE ]		= m_pTextureInfo->m_pCompositeMaps[CSpeedTreeRT::TL_DIFFUSE];
	m_strTextureNames[ SPT_BILLBOARD ]		= m_pTextureInfo->m_pBillboardMaps[CSpeedTreeRT::TL_DIFFUSE];
	m_strTextureNames[ SPT_BILLBOARD_NORMAL ] = m_pTextureInfo->m_pBillboardMaps[CSpeedTreeRT::TL_NORMAL];

	for(int i=0; i<SPT_TEXTURE_NUM; i++)
	{
		DWORD dwFlags = IsNormalMap((TREE_TEXTRUE)i) ? 0 : RCT_AUTOGENMIPMAP;
		// 경로 뗀 이름
		const char* szFileName = GetTextureFileName( m_strTextureNames[i].c_str() );
		m_arrayTreeTexture[i] = REngine::GetDevice().CreateTexture( szFileName, RTF_LINEAR, dwFlags, RFMT_NONE, bBackgroundLoading, this );
	}

	return true;
}

bool RTreeResource::FillBranchBuffer(CSpeedWind* wind)
{
	// 줄기를 나타내는 Alias 설정
	CSpeedTreeRT::SGeometry::SIndexed* pBranches = &( m_pGeometryCache->m_sBranches );
	m_unBranchVertexCount = pBranches->m_nNumVertices;		// 줄기의 버텍스 숫자

	if (m_unBranchVertexCount > 1)							// 줄기가 없는 나무도 있을 수 있다.
	{
		vector<SFVFBranchVertex_GPU_Light> pVertexBuffer(m_unBranchVertexCount);				// 버텍스 포맷을 맞추기 위한 임시 버퍼
		for (int i = 0; i < m_unBranchVertexCount; ++i)
		{
			// 위치 세팅
			memcpy(&pVertexBuffer[i].m_vPosition, &(pBranches->m_pCoords[i * 3]), 3 * sizeof(float));
			m_Box.Add(pVertexBuffer[i].m_vPosition);

			// 노멀 세팅
			memcpy(&(pVertexBuffer[i].m_vNormal), &(pBranches->m_pNormals[i * 3]), 3 * sizeof(float));

			// 텍스쳐좌표 세팅
			pVertexBuffer[i].m_fTexCoords[0] = pBranches->m_pTexCoords[CSpeedTreeRT::TL_DIFFUSE][i * 2];
			pVertexBuffer[i].m_fTexCoords[1] = pBranches->m_pTexCoords[CSpeedTreeRT::TL_DIFFUSE][i * 2 + 1];


			float fWindMatrixIndex1 = float(int(pBranches->m_pWindMatrixIndices[0][i] * 10.0f / wind->GetNumWindMatrices( )));
			float fWindMatrixWeight1 = pBranches->m_pWindWeights[0][i];
			float fWindMatrixIndex2 = float(int(pBranches->m_pWindMatrixIndices[1][i] * 10.0f / wind->GetNumWindMatrices( )));
			float fWindMatrixWeight2 = pBranches->m_pWindWeights[1][i];

			// 바람 파라미터 세팅
			pVertexBuffer[i].m_fWind[0] = fWindMatrixIndex1 + fWindMatrixWeight1;
			pVertexBuffer[i].m_fWind[1] = fWindMatrixIndex2 + fWindMatrixWeight2;

			if (pBranches->m_pTexCoords[CSpeedTreeRT::TL_DETAIL])
			{
				pVertexBuffer[i].m_fDetailCoords[0] = pBranches->m_pTexCoords[CSpeedTreeRT::TL_DETAIL][i * 2];
				pVertexBuffer[i].m_fDetailCoords[1] = pBranches->m_pTexCoords[CSpeedTreeRT::TL_DETAIL][i * 2 + 1];
			}
			else
			{
				pVertexBuffer[i].m_fDetailCoords[0] = 0.0f;
				pVertexBuffer[i].m_fDetailCoords[1] = 0.0f;
			}

			if (pBranches->m_pTexCoords[CSpeedTreeRT::TL_SHADOW])
			{
				pVertexBuffer[i].m_fShadowCoords[0] = pBranches->m_pTexCoords[CSpeedTreeRT::TL_SHADOW][i * 2];
				pVertexBuffer[i].m_fShadowCoords[1] = pBranches->m_pTexCoords[CSpeedTreeRT::TL_SHADOW][i * 2 + 1];
			}
			else
			{
				pVertexBuffer[i].m_fShadowCoords[0] = 0.0f;
				pVertexBuffer[i].m_fShadowCoords[1] = 0.0f;
			}
		}

		m_pBranchBuffer->SetVertexBuffer(pVertexBuffer);

		vector<int> ib;	

		for (int i = 0; i < pBranches->m_nNumLods; ++i)
		{
			ib.clear();
			if (pBranches->m_pNumStrips[i] > 0)
			{
				for (int j=0; j<pBranches->m_pStripLengths[i][0]; j++)
				{
					ib.push_back(pBranches->m_pStrips[i][0][j]);
				}
			}
			m_pBranchBuffer->AddIndex(ib);

		}
	}

	return true;
}

bool RTreeResource::FillFrondBuffer(CSpeedWind* wind)
{
	CSpeedTreeRT::SGeometry::SIndexed* pFrondes = &(m_pGeometryCache->m_sFronds);
	m_unFrondVertexCount = pFrondes->m_nNumVertices;		

	if (m_unFrondVertexCount > 1)							
	{
		vector<SFVFFrondVertex_GPU_Light> pVertexBuffer(m_unFrondVertexCount);				// 버텍스 포맷을 맞추기 위한 임시 버퍼
		for (int i = 0; i < m_unFrondVertexCount; ++i)
		{
			// 위치 세팅
			memcpy(&pVertexBuffer[i].m_vPosition, &(pFrondes->m_pCoords[i * 3]), 3 * sizeof(float));
			m_Box.Add(pVertexBuffer[i].m_vPosition);

			// 노멀 세팅
			memcpy(&(pVertexBuffer[i].m_vNormal), &(pFrondes->m_pNormals[i * 3]), 3 * sizeof(float));

			// 텍스쳐좌표 세팅
			pVertexBuffer[i].m_fTexCoords[0] = pFrondes->m_pTexCoords[CSpeedTreeRT::TL_DIFFUSE][i * 2];
			pVertexBuffer[i].m_fTexCoords[1] = pFrondes->m_pTexCoords[CSpeedTreeRT::TL_DIFFUSE][i * 2 + 1];

			// 바람 파라미터 세팅
			float fWindMatrixIndex1 = float(int(pFrondes->m_pWindMatrixIndices[0][i] * 10.0f / wind->GetNumWindMatrices( )));
			float fWindMatrixWeight1 = pFrondes->m_pWindWeights[0][i];
			float fWindMatrixIndex2 = float(int(pFrondes->m_pWindMatrixIndices[1][i] * 10.0f / wind->GetNumWindMatrices( )));
			float fWindMatrixWeight2 = pFrondes->m_pWindWeights[1][i];

			pVertexBuffer[i].m_fWind[0] = fWindMatrixIndex1 + fWindMatrixWeight1;
			pVertexBuffer[i].m_fWind[1] = fWindMatrixIndex2 + fWindMatrixWeight2;

			if (pFrondes->m_pTexCoords[CSpeedTreeRT::TL_SHADOW])
			{
				pVertexBuffer[i].m_fShadowCoords[0] = pFrondes->m_pTexCoords[CSpeedTreeRT::TL_SHADOW][i * 2];
				pVertexBuffer[i].m_fShadowCoords[1] = pFrondes->m_pTexCoords[CSpeedTreeRT::TL_SHADOW][i * 2 + 1];
			}
			else
			{
				pVertexBuffer[i].m_fShadowCoords[0] = 0.0f;
				pVertexBuffer[i].m_fShadowCoords[1] = 0.0f;
			}
		}

		m_pFrondBuffer->SetVertexBuffer(pVertexBuffer);

		vector<int> ib;

		for (int i = 0; i < pFrondes->m_nNumLods; ++i)
		{
			ib.clear();
			if (pFrondes->m_pNumStrips[i] > 0)
			{
				for (int j=0; j<pFrondes->m_pStripLengths[i][0]; j++)
				{
					ib.push_back(pFrondes->m_pStrips[i][0][j]);
				}
			}
			m_pFrondBuffer->AddIndex(ib);

		}
	}

	return true;
}

bool RTreeResource::FillLeafCardBuffer(CSpeedWind* wind)
{
	const int nNumLeafLods = m_pSpeedTree->GetNumLeafLodLevels( );
	int nLod = 0;

	vector<int> vecLeavsCnt;
	int nTotalLeavsCnt = 0;
	for( nLod=0; nLod<nNumLeafLods; nLod++)
	{
		const CSpeedTreeRT::SGeometry::SLeaf& sLeaves = m_pGeometryCache->m_pLeaves[nLod];
		const int nNumLeaves = sLeaves.m_nNumLeaves;
		vecLeavsCnt.push_back( nNumLeaves);
		nTotalLeavsCnt += nNumLeaves;
	}
	// 각각의 LOD 가 버텍스를 공유하지 않는다고 보고 LOD당 버텍스 정보를 따로 가지고 있는다
	// v0 of lod0, v1 of lod0, v2 of lod0, v3 of lod0,,,, v0 of lod1, v1 of lod1, v2 of lod1, v3 of lod1,,,,,  v0 of lod2, v1 of lod2
	vector<SFVFLeafVertex_GPU_Light> vb(nTotalLeavsCnt*4);
	// 인덱스 버퍼가 LOD당 따로 존재 하고 LOD에 맞는 오프셋을 가지게 함.
	vector<int> ib;
	// LOD 단계 당 시작 인덱스 오프셋
	vector<int> vecLodOffsetIdx(nNumLeafLods);

	int nLodIdxStartOffset = 0;
	int vertexOffset = 0;
	for( nLod = 0; nLod < nNumLeafLods; ++nLod)
	{
		bool bHasLeafCardGeometry = false;
		nLodIdxStartOffset = nLodIdxStartOffset + ib.size();
		vecLodOffsetIdx.push_back( nLodIdxStartOffset);
		ib.clear();

		const CSpeedTreeRT::SGeometry::SLeaf& sLeaves = m_pGeometryCache->m_pLeaves[nLod];

		// grab these parameters for shader uploading later
		if (nLod == 0)
		{
			m_fLeafRockScalar = sLeaves.m_fLeafRockScalar;
			m_fLeafRustleScalar = sLeaves.m_fLeafRustleScalar;
		}

		const int nNumLeaves = sLeaves.m_nNumLeaves;

		for (int nLeaf = 0; nLeaf < nNumLeaves; ++nLeaf)
		{
			const CSpeedTreeRT::SGeometry::SLeaf::SCard* pCard = sLeaves.m_pCards + sLeaves.m_pLeafCardIndices[nLeaf];
			if (!pCard->m_pMesh)
			{
				bHasLeafCardGeometry = true;
				// 사각형의 시작 오프 셋
				int nStartOffset = vertexOffset;

				// 잎 당 사각형의 네 점
				for (int nVert = 0; nVert < 4; ++nVert)
				{
					int nCorner = nVert; //anVertIndices[nVert];

					// xy = diffuse texcoords
					// zw = compressed wind parameters
					float fWindMatrixIndex1 = float(int(sLeaves.m_pWindMatrixIndices[0][nLeaf] * 10.0f / wind->GetNumWindMatrices()));
					float fWindMatrixWeight1 = sLeaves.m_pWindWeights[0][nLeaf];
					float fWindMatrixIndex2 = float(int(sLeaves.m_pWindMatrixIndices[1][nLeaf] * 10.0f / wind->GetNumWindMatrices()));
					float fWindMatrixWeight2 = sLeaves.m_pWindWeights[1][nLeaf];

					vb[vertexOffset].m_vPosition.x = sLeaves.m_pCenterCoords[nLeaf * 3 + 0];
					vb[vertexOffset].m_vPosition.y = sLeaves.m_pCenterCoords[nLeaf * 3 + 1];
					vb[vertexOffset].m_vPosition.z = sLeaves.m_pCenterCoords[nLeaf * 3 + 2];
					m_Box.Add(RVector(&(sLeaves.m_pCenterCoords[nLeaf * 3 + 0])));

					vb[vertexOffset].m_vPosition.w = (float)nCorner;

					vb[vertexOffset].m_vNormal.x = *(sLeaves.m_pNormals + nLeaf * 12 + nCorner * 3 + 0);
					vb[vertexOffset].m_vNormal.y = *(sLeaves.m_pNormals + nLeaf * 12 + nCorner * 3 + 1);
					vb[vertexOffset].m_vNormal.z = *(sLeaves.m_pNormals + nLeaf * 12 + nCorner * 3 + 2);

					vb[vertexOffset].m_fTexCoords[0] = pCard->m_pTexCoords[nCorner * 2 + 0];
					vb[vertexOffset].m_fTexCoords[1] = pCard->m_pTexCoords[nCorner * 2 + 1];
					vb[vertexOffset].m_fTexCoords[2] = fWindMatrixIndex1 + fWindMatrixWeight1;
					vb[vertexOffset].m_fTexCoords[3] = fWindMatrixIndex2 + fWindMatrixWeight2;

					vb[vertexOffset].m_fTexCoords2[0] = pCard->m_fWidth;
					vb[vertexOffset].m_fTexCoords2[1] = pCard->m_fHeight;
					vb[vertexOffset].m_fTexCoords2[2] = pCard->m_afPivotPoint[0] - 0.5f;
					vb[vertexOffset].m_fTexCoords2[3] = pCard->m_afPivotPoint[1] - 0.5f;

					vb[vertexOffset].m_fTexCoords3[0] = DegreeToRadian(pCard->m_afAngleOffsets[0]);
					vb[vertexOffset].m_fTexCoords3[1] = DegreeToRadian(pCard->m_afAngleOffsets[1]);
					vb[vertexOffset].m_fTexCoords3[2] = float(nLeaf % wind->GetNumLeafAngleMatrices());
					vb[vertexOffset].m_fTexCoords3[3] = sLeaves.m_pDimming[nLeaf];

					vertexOffset++;
				}

				// 인덱스 정보 채움
				const int anVertIndices[6] = { 0, 1, 2, 0, 2, 3 };
				for( int i=0; i<6; i++)
				{
					ib.push_back( nStartOffset + anVertIndices[i]);
				}
			}
		}
		// 새겨진 지오메트리 정보가 있을 경우만
		if (bHasLeafCardGeometry)
			m_pLeafCardBuffer->AddIndex(ib);
	}

	// 새겨진 버텍스가 있을 경우만
	if( vertexOffset > 0)
		m_pLeafCardBuffer->SetVertexBuffer(vb);


	return true;
}

bool RTreeResource::FillLeafMeshBuffer(CSpeedWind* wind)
{
	const int nNumSpeedWindAngles = wind->GetNumLeafAngleMatrices( );

	const int nNumLeafLods = m_pSpeedTree->GetNumLeafLodLevels( );

	int numWindMat = wind->GetNumWindMatrices();

	if (nNumLeafLods > 0)
	{
		vector<SFVFLeafMeshVertex_GPU_Light> vb;
		for (int nLod = 0; nLod < m_pGeometryCache->m_nNumLeafLods; ++nLod)
		{
			vector<int>	ib;

			const CSpeedTreeRT::SGeometry::SLeaf* pLod = m_pGeometryCache->m_pLeaves + nLod;

			for (int nLeaf = 0; nLeaf < pLod->m_nNumLeaves; ++nLeaf)
			{
				const CSpeedTreeRT::SGeometry::SLeaf::SCard* pCard = pLod->m_pCards + pLod->m_pLeafCardIndices[nLeaf];
				const CSpeedTreeRT::SGeometry::SLeaf::SMesh* pMesh = pCard->m_pMesh;

				// does this cluster have a mesh associated with it?
				if (pMesh)
				{
					const int c_nVertexTableStart = vb.size();

					// setup the vertex attributes
					for (int nVertex = 0; nVertex < pMesh->m_nNumVertices; ++nVertex)
					{
						vb.push_back(SFVFLeafMeshVertex_GPU_Light());
						SFVFLeafMeshVertex_GPU_Light& now = vb.back();

						float fWindMatrixIndex1 = float(int(pLod->m_pWindMatrixIndices[0][nLeaf] * 10.0f / numWindMat));
						float fWindMatrixWeight1 = pLod->m_pWindWeights[0][nLeaf];
						float fWindMatrixIndex2 = float(int(pLod->m_pWindMatrixIndices[1][nLeaf] * 10.0f / numWindMat));
						float fWindMatrixWeight2 = pLod->m_pWindWeights[1][nLeaf];


						now.m_vPosition.x = pMesh->m_pCoords[nVertex * 3 + 0];
						now.m_vPosition.y = pMesh->m_pCoords[nVertex * 3 + 1];
						now.m_vPosition.z = pMesh->m_pCoords[nVertex * 3 + 2];
						m_Box.Add(RVector(&(pMesh->m_pCoords[nVertex * 3 + 0])));

						now.m_vPosition.w = fWindMatrixIndex1 + fWindMatrixWeight1;

						now.m_vNormal.x = *(pMesh->m_pNormals + nVertex * 3 + 0);
						now.m_vNormal.y = *(pMesh->m_pNormals + nVertex * 3 + 1);
						now.m_vNormal.z = *(pMesh->m_pNormals + nVertex * 3 + 2);

						now.m_fTexCoords[0] = pMesh->m_pTexCoords[nVertex * 2];
						now.m_fTexCoords[1] = pMesh->m_pTexCoords[nVertex * 2 + 1];
						now.m_fTexCoords[2] = float(nLeaf % nNumSpeedWindAngles);
						now.m_fTexCoords[3] = pLod->m_pDimming[nLeaf];

 						now.m_fTexCoords2[0] = pLod->m_pTangents[nLeaf * 12 + 0];
						now.m_fTexCoords2[1] = pLod->m_pBinormals[nLeaf * 12 + 0];
						now.m_fTexCoords2[2] = pLod->m_pNormals[nLeaf * 12 + 0];

						now.m_fTexCoords3[0] = pLod->m_pTangents[nLeaf * 12 + 2];
						now.m_fTexCoords3[1] = pLod->m_pBinormals[nLeaf * 12 + 2];
						now.m_fTexCoords3[2] = pLod->m_pNormals[nLeaf * 12 + 2];

						now.m_fTexCoords4[0] = pLod->m_pCenterCoords[nLeaf * 3 + 0];
						now.m_fTexCoords4[1] = pLod->m_pCenterCoords[nLeaf * 3 + 1];
						now.m_fTexCoords4[2] = pLod->m_pCenterCoords[nLeaf * 3 + 2];
						now.m_fTexCoords4[3] = fWindMatrixIndex2 + fWindMatrixWeight2;
					}

					// setup the triangle indices
					for (int nIndex = 0; nIndex < pMesh->m_nNumIndices; ++nIndex)
						 ib.push_back(c_nVertexTableStart + pMesh->m_pIndices[nIndex]);
				}
			}
			m_pLeafMeshBuffer->AddIndex(ib);
		}
		m_pLeafMeshBuffer->SetVertexBuffer(vb);
	}
	return true;
}

bool RTreeResource::FillBillboardBuffer()
{
	int nTexCoordsTableOffset = 0;

	CSpeedTreeRT::SGeometry sGeometry;
	m_pSpeedTree->GetGeometry(sGeometry, SpeedTree_BillboardGeometry);

	// init the static parts of the billboard struct
	SSpeedTreeBillboard::SVertex* pVertex = &m_cBillboardVertices.m_sVert0;

	const int anVertIndices[6] = { 0, 1, 2, 0, 2, 3 };

	for (int i = 0; i < 6; ++i)
	{
		// cluster corner
		pVertex->m_afPos[3] = float(anVertIndices[i]);

		// width & height
		pVertex->m_afTexCoord0[0] = sGeometry.m_s360Billboard.m_fWidth;
		pVertex->m_afTexCoord0[1] = sGeometry.m_s360Billboard.m_fHeight;

		// 360 billboard texcoord table offset
		pVertex->m_afMiscParams[1] = float(nTexCoordsTableOffset);
		pVertex->m_afMiscParams[2] = float(sGeometry.m_s360Billboard.m_nNumImages);
		pVertex->m_afMiscParams[3] = sGeometry.m_s360Billboard.m_fTransitionPercent;

		// light adjustments
		CSpeedTreeRT::SLightShaderParams& lightParam = m_LightParam;
		pVertex->m_afLightAdjusts[0] = lightParam.m_fBillboardBrightSideLightScalar * lightParam.m_fGlobalLightScalar;
		pVertex->m_afLightAdjusts[1] = lightParam.m_fBillboardDarkSideLightScalar * lightParam.m_fGlobalLightScalar;
		pVertex->m_afLightAdjusts[2] = lightParam.m_fBillboardAmbientScalar;

		++pVertex;
	}

	// init the dynamic parts of the billboard struct with something
	const float afOrigin[3] = { 0.0f, 0.0f, 0.0f };
	m_cBillboardVertices.UpdateTree(afOrigin, 0.0f, 0.0f, 1.0f);

	// setup the 360 texcoord table for quick shader uploads
	m_nBillboardNum = sGeometry.m_s360Billboard.m_nNumImages;
	if (m_nBillboardNum > 0)
	{
		m_vTexCoord360.clear();
		for (int i = 0; i < m_nBillboardNum; ++i)
		{
			m_vTexCoord360.push_back(sGeometry.m_s360Billboard.m_pTexCoordTable[i * 8]);
			m_vTexCoord360.push_back(sGeometry.m_s360Billboard.m_pTexCoordTable[i * 8 + 1]);

			m_vTexCoord360.push_back(sGeometry.m_s360Billboard.m_pTexCoordTable[i * 8] - sGeometry.m_s360Billboard.m_pTexCoordTable[i * 8 + 4]);
			m_vTexCoord360.push_back(sGeometry.m_s360Billboard.m_pTexCoordTable[i * 8 + 1] - sGeometry.m_s360Billboard.m_pTexCoordTable[i * 8 + 5]);
		}
	}
	return true;
}

bool RTreeResource::LoadBranchBuffer()
{
	if (m_unBranchVertexCount > 1) // 줄기가 없는 나무도 있을 수 있다.
		m_pBranchBuffer->CreateDXBuffers();

	return true;
}

bool RTreeResource::LoadFrondBuffer()
{
	if (m_unFrondVertexCount > 1)
		m_pFrondBuffer->CreateDXBuffers();

	return true;
}

bool RTreeResource::LoadLeafCardBuffer()
{
	m_pLeafCardBuffer->CreateDXBuffers();
	return true;
}

bool RTreeResource::LoadLeafMeshBuffer()
{
	m_pLeafMeshBuffer->CreateDXBuffers();
	return true;
}

bool RTreeResource::SetupForRenderBranch()
{
	if (m_pBranchBuffer->m_vMeshInfo.empty())
		return false;

	REngine::GetDevice().SetVertexBuffer(m_pBranchBuffer->m_hVertexBuffer, 0, 0);
	REngine::GetDevice().SetIndexBuffer(m_pBranchBuffer->m_hIndexBuffer);

	return true;
}

bool RTreeResource::RestoreForRenderInstancing()
{
	RPFC_THISFUNC;
	REngine::GetDevice().RestoreForRenderInstancing();

	return true;
}

bool RTreeResource::SetupForRenderFrond()
{
	if (m_pFrondBuffer->m_vMeshInfo.empty())
		return false;

	REngine::GetDevice().SetVertexBuffer(m_pFrondBuffer->m_hVertexBuffer, 0, 0);
	REngine::GetDevice().SetIndexBuffer(m_pFrondBuffer->m_hIndexBuffer);
	return true;
}

bool RTreeResource::SetupForRenderLeafCard()
{
	if (m_pLeafCardBuffer->m_vMeshInfo.empty())
		return false;

	REngine::GetDevice().SetVertexBuffer(m_pLeafCardBuffer->m_hVertexBuffer, 0, 0);
	REngine::GetDevice().SetIndexBuffer(m_pLeafCardBuffer->m_hIndexBuffer);

	return true;
}

bool RTreeResource::SetupForRenderLeafMesh()
{
	if (m_pLeafMeshBuffer->m_vMeshInfo.empty())
		return false;

	REngine::GetDevice().SetVertexBuffer(m_pLeafMeshBuffer->m_hVertexBuffer, 0, 0);
	REngine::GetDevice().SetIndexBuffer(m_pLeafMeshBuffer->m_hIndexBuffer);

	return true;
}

bool RTreeResource::SetupForRenderInstancing( RTreeInstancingBuffer* pTreeInstancingBuffer, UINT nCnt)
{
	RPFC_THISFUNC;
	REngine::GetDevice().SetupForRenderInstancing( pTreeInstancingBuffer->GetBuffer(), nCnt);
	return true;
}

bool RTreeResource::SetupForRenderBillboard()
{
	return true;
}

int RTreeResource::RenderBranch( int nBranchLodLevel)
{
	RPFC_THISFUNC;
	return m_pBranchBuffer->Render(nBranchLodLevel);
}

int RTreeResource::RenderBranchInstancing( RTreeInstancingBuffer* pTreeInstancingBuffer, int nCnt)
{
	return m_pBranchBuffer->Render( pTreeInstancingBuffer, nCnt);
}

int RTreeResource::RenderFrond( int nFrondLodLevel)
{
	return m_pFrondBuffer->Render(nFrondLodLevel);
}

int RTreeResource::RenderFrondInstancing( RTreeInstancingBuffer* pTreeInstancingBuffer, int nCnt)
{
	return m_pFrondBuffer->Render( pTreeInstancingBuffer, nCnt);
}

int RTreeResource::RenderLeafCard( int nLeafLodLevel)
{
	return m_pLeafCardBuffer->Render(nLeafLodLevel);
}

int RTreeResource::RenderLeafMesh( int nLeafLodLevel)
{
	return m_pLeafMeshBuffer->Render(nLeafLodLevel);
}

SSpeedTreeBillboard::SVertex* RTreeResource::RenderBillboard( const float* pPos, float fAzimuth, float fFade, float fScale)
{
	m_cBillboardVertices.UpdateTree(pPos, fAzimuth, fFade, fScale);

	return &m_cBillboardVertices.m_sVert0;
}

RTreeCollisionObject* RTreeResource::GetTreeCollisionInfo(RTreeCollisionType type)
{
	// 어떻게 해야하는지 잘 모르겠다.
	_ASSERT(m_pSpeedTree);

	for ( vector<RTreeCollisionObject>::iterator it = m_pCollisionInfo->m_vCollisions.begin(); it != m_pCollisionInfo->m_vCollisions.end(); ++it )
	{
		RTreeCollisionObject &rObject = (*it);
		if ( rObject.m_Type == type )
			return &rObject;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
}
