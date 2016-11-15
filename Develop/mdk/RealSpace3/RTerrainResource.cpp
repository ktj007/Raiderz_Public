#include "StdAfx.h"
#include "RTerrainResource.h"
#include "MFileLoader.h"
#include "RGrassSystem.h"
#include "RTerrainPatch.h"
#include "RTerrainQuadTree.h"
#include "RTerrainPVS.h"
#include "RTerrain.h"
#include "RResourceID.h"
#include "RResourceLoader.h"
#include "RShaderFX.h"
#include "RRenderHelper.h"
#include "RRule.h"

namespace rs3 {

const float RDEFAULT_GRASS_NEAR	= 2000.f;
const float RDEFAULT_GRASS_FAR	= 3000.f;

const int GRID_Z_COUNT = 20;

RTerrainResource::RTerrainResource() 
: RResource("", RRESOURCETYPE::RR_TERRAIN)
, m_vStep(0, 0, 0)
, m_pPatches(NULL)
, m_pTerrain(NULL)
{
}

RTerrainResource::~RTerrainResource()
{
}

void RTerrainResource::Initialize()
{
	m_pHeightField = NULL;

	m_pNormalMap = NULL;

	m_iLayerNum = m_iPatchNum = 0;
	m_iIndexNum = m_iLODLevel = 0;
	m_nPatchSize = m_iCount = 0;

	m_phIndexBuffer = NULL;
	m_pPrimitiveNum = NULL;   

	m_pQuadTree = NULL;
	m_pLayerInfo = NULL;

	m_dwAmbientColor = 0xffffffff;

	m_hBaseMap = R_NONE;
	m_nBaseMapWidth = 0;
	m_nBaseMapHeight = 0;

	m_bAnisotropyFilter = true;
	m_pGrassSystem = NULL;
	m_nLODPatchCount = LOD_LEVEL;

	// 기본 광원값 = 1 = 0x808080
	m_dwDiffuseColor = D3DCOLOR_XRGB(128, 128, 128);
	m_dwAmbientColor = 0x00;

	m_pPVS = NULL;

	m_pTerrain = dynamic_cast< RTerrain*>( this);

	// create default texture color : alpha
	DWORD dwDefaultNullColor[64];
	for (int i=0; i<64; i++)
		dwDefaultNullColor[i] = 0x00000000;
}

bool RTerrainResource::Fill()
{
	// fill displacementmap
	if (m_InfoDisplacementAndGrass.m_bFillDone == false)
	{
		bool bFillResult = FillDisplacementMap(
			m_InfoDisplacementAndGrass.m_strDisplacementFileName.c_str(),
			m_InfoDisplacementAndGrass.m_vMapSize);

		m_InfoDisplacementAndGrass.m_bFillDone = true; 
		_ASSERT(bFillResult);
	}

	if (m_deqInfoAlphaMap.empty() == false)
	{
		for(std::deque< ALPHA_MAP_INFO >::iterator itr = m_deqInfoAlphaMap.begin(); itr != m_deqInfoAlphaMap.end(); ++itr )
		{
			bool bFillAlphaMap = FillLayerAlpha( itr->m_nIndex, itr->m_strAlphaMapFileName.c_str() );
			if (!bFillAlphaMap)
				mlog("can't create %s alpha map\n", itr->m_strAlphaMapFileName.c_str());
		}
	}

	if (m_deqInfoTileMap.empty() == false)
	{
		for(std::deque< TILE_MAP_INFO >::iterator itr = m_deqInfoTileMap.begin();
			itr != m_deqInfoTileMap.end(); ++itr )
		{
			FillLayerTile(
				itr->m_nIndex, itr->m_strTileMapFileName.c_str(), itr->m_strNormalMapFileName.c_str(),
				itr->m_vMapSize.x, itr->m_vMapSize.y, itr->m_bSpecularTest);
		}
	}

	return true;
}

RResource::LOAD_RESULT RTerrainResource::Load()
{
	bool bBackgroundLoading = IsBackgroundCreated();

	// 백그라운드 로딩중이면 대기
	if(bBackgroundLoading)
	{
		for(int i=0;i<m_iLayerNum;i++)
		{
			if(!m_pLayerInfo[i].hTileMap->IsLoadingCompleted())
				return WAITING;

			if(m_pLayerInfo[i].hNormalMap)
				if(!m_pLayerInfo[i].hNormalMap->IsLoadingCompleted())
					return WAITING;
		}
	}

	// 딸린 리소스들 로딩 완료

	// load displacementmap
	if (m_InfoDisplacementAndGrass.m_bLoadDone == false)
	{
		bool bCreateDisplacementMapResult = CreateDisplacementmapPatch(
			m_InfoDisplacementAndGrass.m_vMapSize.x,
			m_InfoDisplacementAndGrass.m_vMapSize.y,
			m_InfoDisplacementAndGrass.m_nPatchSize,
			m_InfoDisplacementAndGrass.m_nLodLevel);

		m_InfoDisplacementAndGrass.m_bLoadDone = true; 
		_ASSERT(bCreateDisplacementMapResult);
	}

	// Grass System 생성
	bool bCreateGrass = CreateGrassSystem();
	_ASSERT(bCreateGrass);

	/// 패치당 레이어의 알파 마스크 텍스쳐들은 랜더 되기 직전에 만들어지므로 이곳에서는 레이어 전체의 텍스쳐만 처리가 되면 큐를 비워주자.
	if (m_deqInfoAlphaMap.empty() == false)
	{
		m_deqInfoAlphaMap.clear();
	}

	if (m_deqInfoTileMap.empty() == false)
	{
		m_deqInfoTileMap.clear();
	}
	
	CreateAndLoadGrass( m_InfoDisplacementAndGrass.m_strGrassFileName );
	SetGrassLodParams(RDEFAULT_GRASS_NEAR, RDEFAULT_GRASS_FAR);

	return LOADED;
}

// ASync load 에서 사용하는 부분 ( REngine::GetDevicePtr()를 사용하지 않는 부분 )
bool RTerrainResource::FillDisplacementMap( const char* filename, const RVector& _refVecMapSize )
{
	_ASSERT(m_pHeightField==NULL);
	_ASSERT(NULL != filename);


	MVector2 vScale(_refVecMapSize.x,_refVecMapSize.y);

	m_pHeightField = new RHeightField();
	if ( m_pHeightField->LoadHeightFieldmapFromR32F(filename, vScale) == false )
	{
		SAFE_DELETE(m_pHeightField);
		return false;
	}

	bool bNormalMapCreate = CreateNormalFieldMap(m_pHeightField->GetWidthMap()+1, m_pHeightField->GetHeightMap()+1, vScale.x, vScale.y);
	_ASSERT(bNormalMapCreate);

	// Load desc file
	string descFileName = RRule::Terrain::GetTerrainDescFileNameByTerrainHeightmapFileName( string(filename) );
	if ( !m_pHeightField->LoadHeightFieldDescFile(descFileName.c_str()))
		mlog("terrain desc file is not found\n");

	return true;
}


bool RTerrainResource::IsAlphaTextureUsed( int nPatchIdx, int nLayerIdx)
{
	if( m_iLayerNum>nLayerIdx && nLayerIdx>=0 && m_pLayerInfo)
		return m_pLayerInfo[nLayerIdx].pbDataUsed[nPatchIdx];
	else 
		return false;
}

void RTerrainResource::ToDoWhenUpdateAlphaTextureRaw( int nLayerIdx)
{
	if( m_iLayerNum>nLayerIdx && nLayerIdx>=0 && m_pLayerInfo)
	{
		/// 알파 텍스쳐가 업데이트 되어야 한다고 일러준다.
		m_pLayerInfo[nLayerIdx].bNeedUpdateAlphaTexture = true;
		/// 패치별 레이어의 알파 사용 플래그를 재 계산.
		for ( int i = 0; i<m_iPatchNum; ++i )
		{
			if( m_pLayerInfo[nLayerIdx].pbDataUsed)
				m_pLayerInfo[nLayerIdx].pbDataUsed[i] = IsAlphaDataInTexture( i, nLayerIdx);
		}
	}
	
}

void RTerrainResource::DeleteAlphaTextureRaw( int nLayerIdx)
{
	if( m_iLayerNum>nLayerIdx && nLayerIdx>=0 && m_pLayerInfo)
	{
		SAFE_DELETE_ARRAY( m_pLayerInfo[nLayerIdx].pAlphaTextureRaw);
		/// Raw data가 바뀌면 햐아 하는 일들 .....
		ToDoWhenUpdateAlphaTextureRaw( nLayerIdx);
	}
}

void RTerrainResource::SetAlphaTextureRaw( int nLayerIdx, BYTE* pAlphaTextureRaw)
{
	if( m_iLayerNum>nLayerIdx && nLayerIdx>=0 && m_pLayerInfo)
	{
		m_pLayerInfo[nLayerIdx].pAlphaTextureRaw = pAlphaTextureRaw;
		/// Raw data가 바뀌면 햐아 하는 일들 .....
		ToDoWhenUpdateAlphaTextureRaw( nLayerIdx);
	}
}


bool RTerrainResource::FillLayerAlpha(int nIndex, const char *filename)
{
	_ASSERT(m_iLayerNum>nIndex);
	_ASSERT(nIndex>=0);
	_ASSERT(REngine::GetDevicePtr());
	_ASSERT(m_pLayerInfo);

	if ( !strstr(filename,".dds"))
		return false;


	// .dds를 직접 읽음
	int nImageWidth, nImageHeight;
	BYTE *pAlphaTextureRaw = MFileLoad::CreateARGBFromDDS(filename,&nImageWidth,&nImageHeight);
	if ( pAlphaTextureRaw == NULL )
		return false;

	m_pLayerInfo[nIndex].alphaMapName = filename;
	m_pLayerInfo[nIndex].dwAlphaWidth = (DWORD)nImageWidth;
	m_pLayerInfo[nIndex].dwAlphaHeight = (DWORD)nImageHeight;
	/// size of one patch's alpha texture
	DWORD dwSmallWidth = (m_pLayerInfo[nIndex].dwAlphaWidth/m_iCount);
	DWORD dwSmallHeight = (m_pLayerInfo[nIndex].dwAlphaHeight/m_iCount);
	/// maybe... margin of joint
	m_pLayerInfo[nIndex].dwAlphaPatchWidth = dwSmallWidth + 2;
	m_pLayerInfo[nIndex].dwAlphaPatchHeight = dwSmallHeight + 2;
	SetAlphaTextureRaw( nIndex, pAlphaTextureRaw);

	return true;
}

bool RTerrainResource::CreateAndLoadGrass(const std::string& _strFileName)
{
	_ASSERT(m_pGrassSystem);
	m_pGrassSystem->m_strGrassPath = this->m_strPathName;

	bool bRet = m_pGrassSystem->LoadFromFile( _strFileName, m_pTerrain);
	if( !bRet )
		return false;

	m_pGrassSystem->UpdateDXBuffers( m_pTerrain);
	//mlog("Grass data loading\n");
	return true;
}

void RTerrainResource::SetGrassLodParams(float _fNear, float _fFar)
{
	if( _fNear >= 0.f && _fFar >= 0.f )
	{
		if( _fFar < _fNear)
			_fNear = _fFar;
		m_fGrassNear = _fNear;
		m_fGrassFar = _fFar;
	}
	if( m_pGrassSystem )
	{
		RSpeedGrass::SetLodParams(m_fGrassFar, m_fGrassFar-m_fGrassNear);
	}
}

void RTerrainResource::SetPath(const char *pPathName)
{
 	int iLen = strlen(pPathName);
 	_ASSERT(iLen<MAX_PATH);
 
 	m_strPathName = pPathName;
 	_ASSERT(iLen==m_strPathName.length());
 
 	if ( iLen )
 		if ( m_strPathName[iLen-1] != '/' && m_strPathName[iLen-1] != '\\' )
 			m_strPathName[iLen] = '/';
}

void RTerrainResource::SetResourceName(const char *pName)
{
	m_strName = pName;
}

bool RTerrainResource::CreateIndexContact(rs3::HINDEXBUFFER *phIndexBuffer,
										  int *pPrimitiveNum, int nPatchWidth, int nPatchHeight, int nLevel, int nFlag )
{
	int pos = 0;
	//HRESULT hr;

	/** 
	Vertex Buffer의 구조에 따라, 뒤쪽 부분에 이음새를 위한 Vertex를 넣었다.
	*/
	int nLeftBaseIndex = ( nPatchWidth + 1 ) * ( nPatchHeight + 1 );
	int nRightBaseIndex = nLeftBaseIndex + ( nPatchHeight);
	int nTopBaseIndex = nRightBaseIndex + ( nPatchHeight);
	int nBottomBaseIndex = nTopBaseIndex + ( nPatchWidth);

	// TRIANGLESTRIP
	int nPitch = nPatchWidth + 1;
	int nIndexSize = ( nPitch ) * nPatchHeight * 2 /* 순수 Strip Index */ +  ( nPatchHeight - 1) * 2; /* 퇴화 삼각형(?) */

	bool bLeft = ( (nFlag & LEFT) && nLevel ? true : false );
	bool bRight = ( (nFlag & RIGHT) && nLevel ? true : false );
	bool bTop = ( (nFlag & TOP) && nLevel ? true : false );
	bool bBottom = ( (nFlag & BOTTOM) && nLevel ? true : false );
	if ( bLeft )
		nIndexSize += (nPatchHeight*2);
	if ( bRight )
		nIndexSize += (nPatchHeight*2);
	if ( bBottom )
		nIndexSize += (nPatchWidth*2);
	if ( bTop )
		nIndexSize += (nPatchWidth*2);

	WORD *pIndexArray = new WORD[nIndexSize];
	_ASSERT(pIndexArray);
	memset(pIndexArray, 0, sizeof(WORD)*nIndexSize);

	WORD *pIndex = pIndexArray;

	for ( int y = 0; y<nPatchHeight ; ++y )
	{
		if ( y != 0 )
		{
#if PATCH_CCW
			*pIndex = y * nPitch;			++pIndex;
#else
			*pIndex = y * nPitch + nPitch;	++pIndex;
#endif
		}

		for ( int x = 0; x<nPitch ; ++x )
		{
			pos = x + y*nPitch;
			// 중복조건을 먼저 !
			if ( bLeft && ( x == 0 ) && bTop && ( y  == 0 ))
			{
#if PATCH_CCW
				*pIndex = pos;	++pIndex;
				*pIndex = ( nLeftBaseIndex + y );	++pIndex;
				*pIndex = nTopBaseIndex + x;	++pIndex;
				*pIndex = pos+nPitch; ++pIndex;
				*pIndex = pos + 1;		++pIndex;
				*pIndex = pos+nPitch;	++pIndex;
#else
				// x == 0 && y == 0
				*pIndex = pos + nPitch;	++pIndex;
				*pIndex = ( nLeftBaseIndex);	++pIndex;
				*pIndex = pos + nPitch+1;	++pIndex;
				*pIndex = pos; ++pIndex;
				*pIndex = pos + nPitch+1;	++pIndex;
				*pIndex = nTopBaseIndex;	++pIndex;
#endif
			}
			else if ( bLeft && ( x == 0 ) && bBottom && ( y == nPatchHeight - 1))
			{
#if PATCH_CCW
				*pIndex = pos;	++pIndex;
				*pIndex = ( nLeftBaseIndex + y );	++pIndex;
				*pIndex = pos+1;	++pIndex;
				*pIndex = pos+nPitch; ++pIndex;
				*pIndex = pos+1;	++pIndex;
				*pIndex = nBottomBaseIndex + x;	++pIndex;
#else
				*pIndex = pos + nPitch;	++pIndex;
				*pIndex = (nLeftBaseIndex + y);	++pIndex;
				*pIndex = nBottomBaseIndex + x;	++pIndex;
				*pIndex = (nLeftBaseIndex + y);	++pIndex;
				*pIndex = pos+nPitch+1; ++pIndex;
				*pIndex = pos;	++pIndex;
#endif
			}
			else if ( bRight && ( x == nPitch - 1 ) && bBottom && ( y == nPatchHeight-1 ))
			{
#if PATCH_CCW
				*pIndex = pos;				   ++pIndex;
				*pIndex = nBottomBaseIndex + x-1;   ++pIndex;
				*pIndex = nRightBaseIndex + y; ++pIndex;
				*pIndex = pos + nPitch;		   ++pIndex;
#else
				*pIndex = nBottomBaseIndex + x-1;   ++pIndex;
				*pIndex = pos;				   ++pIndex;
				*pIndex = nRightBaseIndex + y; ++pIndex;
				*pIndex = pos + nPitch;		   ++pIndex;
#endif
			}
			// RIGHT && TOP 은 없음
			// 단일 조건은 뒤에 ...
			else if ( bLeft && ( x == 0 ))
			{
#if PATCH_CCW
				*pIndex = pos;	++pIndex;
				*pIndex = ( nLeftBaseIndex + y );	++pIndex;
				*pIndex = pos+1; ++pIndex;
				*pIndex = pos+nPitch; ++pIndex;
#else
				*pIndex = pos+nPitch; ++pIndex;
				*pIndex = ( nLeftBaseIndex + y );	++pIndex;
				*pIndex = pos+nPitch+1;	++pIndex;
				*pIndex = pos; ++pIndex;

#endif
			}
			else if ( bTop && ( y  == 0 ) && ( x < nPatchWidth ))
			{
#if PATCH_CCW
				*pIndex = pos;			++pIndex;
				*pIndex = pos + nPitch;	++pIndex;
				*pIndex = nTopBaseIndex + x;;	++pIndex;
				*pIndex = pos + nPitch;	++pIndex;
#else
				*pIndex = pos + nPitch;	++pIndex;
				*pIndex = pos;			++pIndex;
				*pIndex = pos + nPitch;	++pIndex;
				*pIndex = nTopBaseIndex + x;;	++pIndex;

#endif
			}
			else if ( bRight && ( x == nPitch-1 ))
			{
#if PATCH_CCW
				*pIndex = pos;				   ++pIndex;
				*pIndex = pos + nPitch -1;	   ++pIndex;
				*pIndex = nRightBaseIndex + y; ++pIndex;
				*pIndex = pos + nPitch;		   ++pIndex;
#else
				*pIndex = pos + nPitch;		   ++pIndex;
				*pIndex = pos-1;			   ++pIndex;
				*pIndex = nRightBaseIndex + y; ++pIndex;
				*pIndex = pos;				   ++pIndex;
#endif
			}
			else if ( bBottom && ( y == nPatchHeight - 1 ) && ( x < nPatchWidth ))
			{
#if PATCH_CCW
				*pIndex = pos;			++pIndex;
				*pIndex = pos + nPitch;	++pIndex;
				*pIndex = pos + 1;		++pIndex;
				*pIndex = nBottomBaseIndex + x;	++pIndex;
#else
				*pIndex = pos + nPitch;	++pIndex;
				*pIndex = pos;			++pIndex;
				*pIndex = nBottomBaseIndex + x;	++pIndex;
				*pIndex = pos;			++pIndex;
#endif
			}
			else
			{
#if PATCH_CCW
				*pIndex = pos;			++pIndex;
				*pIndex = pos + nPitch;	++pIndex;
#else
				*pIndex = pos + nPitch;	++pIndex;
				*pIndex = pos;			++pIndex;
#endif
			}
		}
		if ( y != nPatchHeight-1)		
		{
#if PATCH_CCW
			*pIndex = pos + nPitch;		++pIndex;		
#else
			*pIndex = pos;			++pIndex;
#endif
		}
	}

	UINT nLen = (UINT)(pIndex - pIndexArray);
	_ASSERT(nLen == nIndexSize);

	*phIndexBuffer = REngine::GetDevice().CreateIndexBuffer(nIndexSize, RFMT_INDEX16,(void*)pIndexArray);
	SAFE_DELETE_ARRAY(pIndexArray);

	*pPrimitiveNum = ( nPatchWidth ) * ( nPatchHeight ) * 2 + ( nPatchHeight - 1) * 4;
	*pPrimitiveNum +=( bLeft ? nPatchHeight*2 : 0 );
	*pPrimitiveNum +=( bRight ? nPatchHeight*2 : 0 );
	*pPrimitiveNum +=( bBottom ? nPatchWidth*2 : 0 );
	*pPrimitiveNum +=( bTop ? nPatchWidth*2: 0 );

	return true;
}

bool RTerrainResource::CreateIndexNonCont(rs3::HINDEXBUFFER *phIndexBuffer, int *pPrimitiveNum, int nPatchWidth, int nPatchHeight, int nLevel)
{
	int pos = 0;
	//HRESULT hr;

	// TRIANGLESTRIP
	int nPitch = nPatchWidth+1;
	int nIndexSize = ( nPitch ) * nPatchHeight * 2 /* 순수 Strip Index */ +  ( nPatchHeight - 1) * 2; /* 퇴화 삼각형(?) */

	WORD *pIndexArray = new WORD[nIndexSize];
	memset(pIndexArray, 0, sizeof(WORD)*nIndexSize);
	WORD *pIndex = pIndexArray;

	for ( int y = 0; y<nPatchHeight ; ++y )
	{
		if ( y != 0 )
		{
#if PATCH_CCW
			*pIndex = y * nPitch;			++pIndex;
#else
			*pIndex = y * nPitch+nPitch;	++pIndex;
#endif
		}
		for ( int x = 0; x<nPitch ; ++x )
		{
			pos = x + y*nPitch;
#if PATCH_CCW
			*pIndex = pos;			++pIndex;
			*pIndex = pos + nPitch;			++pIndex;
#else
			*pIndex = pos + nPitch;			++pIndex;
			*pIndex = pos;			++pIndex;
#endif
		}
		if ( y != nPatchHeight-1)
		{
#if PATCH_CCW
			*pIndex = pos + nPitch;			++pIndex;
#else
			*pIndex = pos;			++pIndex;
#endif
		}
	}

	//UINT nLen = (UINT)(pIndex - pIndexArray);
	*phIndexBuffer = REngine::GetDevice().CreateIndexBuffer(nIndexSize, RFMT_INDEX16, (void*)pIndexArray);
	SAFE_DELETE_ARRAY(pIndexArray);

	*pPrimitiveNum = ( nPatchWidth ) * ( nPatchHeight ) * 2 + ( nPatchHeight - 1) * 4;

	return true;
}

void RTerrainResource::CreateLayer(int nLayerNum, int nPatchNum)
{
	_ASSERT(m_pLayerInfo==NULL);
	if ( nLayerNum == 0 )
		return;

	m_pLayerInfo = new LayerInfo[nLayerNum];
	for ( int nIndex = 0; nIndex<nLayerNum; ++nIndex )
	{
		m_pLayerInfo[nIndex].dwAlphaWidth = 0;
		m_pLayerInfo[nIndex].dwAlphaHeight = 0;

		/// 레이어, 패치 당 알파 마스킹 텍스쳐
		m_pLayerInfo[nIndex].phAlphaMask = new RTexture*[nPatchNum];
		for ( int i = 0; i<nPatchNum; ++i )
			m_pLayerInfo[nIndex].phAlphaMask[i] = R_NONE;
		/// 레이어, 패치 당 알파 마스킹 텍스쳐 사용 유무
		m_pLayerInfo[nIndex].pbDataUsed = new bool[nPatchNum];
		for ( int i = 0; i<nPatchNum; ++i )
			m_pLayerInfo[nIndex].pbDataUsed[i] = false;
		/// 레이어, 패치 당 알파 마스킹 텍스쳐 생성 처리 유무
		m_pLayerInfo[nIndex].pbAlphaMaskCreated = new bool[nPatchNum];
		for ( int i = 0; i<nPatchNum; ++i )
			m_pLayerInfo[nIndex].pbAlphaMaskCreated[i] = false;
		

		m_pLayerInfo[nIndex].hTileMap = R_NONE;
		m_pLayerInfo[nIndex].hNormalMap = R_NONE;
		m_pLayerInfo[nIndex].vTilingUV = RVector(1.0f,1.0f,1.0f);

		m_pLayerInfo[nIndex].alphaMapName.clear();
		m_pLayerInfo[nIndex].tileMapName.clear();
		m_pLayerInfo[nIndex].normalMapName.clear();

		m_pLayerInfo[nIndex].dwAlphaPatchWidth = 0;
		m_pLayerInfo[nIndex].dwAlphaPatchHeight = 0;
	}

	m_iLayerNum = nLayerNum;
}

// UpdateHeightField : Rect, w, h ( = count )
// 높이맵 (float)m_pHeightMap 의 데이터를 기반으로 m_pPatch->m_pVertexBuffer를 생성해낸다.
void RTerrainResource::UpdateHeightField(RECT *pRect, int w, int h, int nPitch)
{
	_ASSERT(NULL!=m_pPatches);
	_ASSERT(NULL!=m_pQuadTree);
	_ASSERT(NULL!=m_pHeightField);

	int nIndex = w + h*m_iCount;
	RTerrainPatch *pPatch = &m_pPatches[nIndex];
	pPatch->SetXY(w, h);

	// vertexbuffer를 만든다.
	pPatch->SetDevice(REngine::GetDevicePtr());
	pPatch->CreateVertexBuffer(m_iLODLevel, GetWidthMap()/m_iCount);

	for ( int i = 0; i<m_iLODLevel; ++i )
		pPatch->CreateDisplacementMapFromMemory(m_pHeightField->GetPtr(), m_pNormalMap, nPitch, i /* Level */ ,pRect);

	pPatch->UpdateVertexBuffer();

	// quad tree에 패치를 추가
	m_pQuadTree->AddTerrainPatch(pPatch);
}

void RTerrainResource::UpdateHeightField(int w, int h)
{
	// w, h - index의 Patch를 업데이트 한다.
	int nWidth = m_pHeightField->GetWidthMap()/m_iCount;
	int nHeight = m_pHeightField->GetHeightMap()/m_iCount;

	RECT rt;
	rt.left = w * nWidth;
	rt.top = h * nHeight;
	rt.right = (w+1)*nWidth;
	rt.bottom = (h+1)*nHeight;

	UpdateHeightField( &rt, w, h, m_pHeightField->GetHeightMapPitch());
}


int RTerrainResource::GetVertexFromBound(std::vector<RVt_pos> &vecVertex, const RBoundingBox &aabb)
{
	if (m_pHeightField == NULL) return 0;

	// 일단 가로, 세로의 길이를 얻어 vecVertex의 크기를 resize한다. ( x, y 값만 ... )
	RBoundingBox box = aabb;

	int nX1 = (int)(box.vmin.x/GetXScale());
	int nY1 = (int)(box.vmin.y/GetYScale());
	int nX2 = (int)(box.vmax.x/GetXScale());
	int nY2 = (int)(box.vmax.y/GetYScale());

	if ( nX1 < 0 )	nX1 = 0;
	if ( nY1 < 0 )	nY1 = 0;
	if ( nX2 >= GetWidthMap())  nX2 = GetWidthMap() -1;
	if ( nY2 >= GetHeightMap()) nY2 = GetHeightMap() -1;

	if ( nX1 > nX2 )
		return 0;
	if ( nY1 > nY2 )
		return 0;

	int nSize = ( nX2 - nX1 + 1 ) * ( nY2 - nY1 + 1 );
	int nCount = 0;

	float *pHeightMap = m_pHeightField->GetPtr();
	int nPitch = m_pHeightField->GetHeightMapPitch();

	// 해당 영역을 순환하면서 해당하는 영역의 z를 검사하여 불 필요한 것은 skip 하고, 그렇지 않은 것은 triangle을 만든다.
	for ( int y = nY1; y <=nY2; ++y )
	{
		for ( int x = nX1; x <=nX2; ++x )
		{
			++nCount;

			RVt_pos vLeftTop, vRightTop, vLeftBottom, vRightBottom;

			vLeftTop.vPos = RVector(x*GetXScale(), y*GetYScale(), pHeightMap[x + y*nPitch]);
			vRightTop.vPos = RVector((x+1)*GetXScale(), y*GetYScale(), pHeightMap[x+1 + y*nPitch]);
			vLeftBottom.vPos = RVector(x*GetXScale(),(y+1)*GetYScale(), pHeightMap[x + (y+1)*nPitch]);
			vRightBottom.vPos = RVector((x+1)*GetXScale(),(y+1)*GetYScale(), pHeightMap[x+1 + (y+1)*nPitch]);

			vecVertex.push_back(vLeftTop);
			vecVertex.push_back(vRightTop);
			vecVertex.push_back(vRightBottom);
			vecVertex.push_back(vLeftTop);
			vecVertex.push_back(vRightBottom);
			vecVertex.push_back(vLeftBottom);
		}
	}

	_ASSERT(nSize==nCount);
	return vecVertex.size();
	// 남겨진 문제 : LOD 경계 부분에서 약간 깨지는 문제가 있음.
}

bool RTerrainResource::CreateNormalFieldMap(int nImageWidth, int nImageHeight, float fXSize, float fYSize)
{
	_ASSERT(m_pHeightField!=NULL);

	SAFE_DELETE_ARRAY(m_pNormalMap);

	float fXScale = fXSize/(float)GetWidthMap();
	float fYScale = fYSize/(float)GetHeightMap();

	float *pHeightMap = m_pHeightField->GetPtr();

	m_pNormalMap = new RVector[nImageWidth*nImageHeight];

	for ( int y = 0; y<nImageHeight; ++y )
	{
		for ( int x = 0; x<nImageWidth; ++x )
		{
			int nIndexCenter = x + y*nImageWidth;
			int nIndexUP = (x+1) + y*nImageWidth;
			int nIndexUN = (x-1) + y*nImageWidth;
			int nIndexVP = x + (y+1)*nImageWidth;
			int nIndexVN = x + (y-1)*nImageWidth;

			float cn, up, un, vp, vn;

			cn = pHeightMap[nIndexCenter];
			un = ( x == 0 ? cn : pHeightMap[nIndexUN]);
			up = ( x == nImageWidth-1 ? cn : pHeightMap[nIndexUP]);
			vn = ( y == 0 ? cn : pHeightMap[nIndexVN]);
			vp = ( y == nImageHeight-1 ? cn : pHeightMap[nIndexVP]);

			float fx = x * fXScale;
			float fy = y * fYScale;

			RVector vCenter(fx, fy, cn);
			RVector vUn = RVector( fx - fXScale, fy, un) - vCenter;
			RVector vUp = RVector( fx + fXScale, fy, up) - vCenter;
			RVector vVn = RVector( fx, fy - fYScale, vn) - vCenter;
			RVector vVp = RVector( fx, fy + fYScale, vp) - vCenter;

			RVector &vNormal = m_pNormalMap[nIndexCenter];
			vNormal = vVp.CrossProduct(vUn);
			vNormal += vUn.CrossProduct(vVn);
			vNormal += vVn.CrossProduct(vUp);
			vNormal += vUp.CrossProduct(vVp);
			vNormal.Normalize();
		}
	}

	return true;
}

bool RTerrainResource::CreateDisplacementmapPatch(float fXSize, float fYSize, int nPatchSize, int nLODLevel)
{
	// LOD 레벨 깊이는 nPatchSize가 충분히 넓어야 합니다.
	_ASSERT(pow(2.f, nLODLevel)<=static_cast<float>(nPatchSize));

	m_iLODLevel = nLODLevel;

	// Patch의 VertexBuffer에 데이터를 만들고, HeightField의 Min-Max를 따로 구한다.
	SAFE_DELETE_ARRAY(m_pPatches);
	m_pPatches = new RTerrainPatch[m_iPatchNum];
	for ( int i = 0; i<m_iPatchNum; ++i )
		m_pPatches[i].Init(this);

	// 원점에서 시작되는 QuadTree를 만든다.
	_ASSERT(m_pQuadTree==NULL);

	m_pQuadTree = new RTerrainQuadTree();
	m_pQuadTree->CreateNode(0, 0, fXSize, fYSize);

	// 수평선 컬링 클래스를 만든다.
	//m_pHorizonCull = new Horizon();
	//m_pHorizonCull->Init(width, height);

	int count = 0;
	for ( int h = 0; h<m_iCount; ++h )
	{
		for ( int w = 0; w<m_iCount; ++w )
		{
			_ASSERT(m_iPatchNum>count);
			UpdateHeightField(w, h);
			++count;
		}
	} // next h


	if ( 0 )
	{
		m_pPVS = new RTerrainPVS();
		DWORD dwStartTime = GetTickCount();

		int nSize = m_iCount * m_iCount;

		float *pMinArray = new float[nSize];
		float *pMaxArray = new float[nSize];

		float fmaxz = -FLT_MAX;
		float fminz = +FLT_MAX;

		for ( int h = 0; h<m_iCount; ++h )
			for ( int w = 0; w<m_iCount; ++w )
			{
				int nindex = w + h*m_iCount;
				RBoundingBox *pBBox = m_pPatches[nindex].GetAABBBoundBox();

				pMinArray[nindex] = pBBox->vmin.z;
				pMaxArray[nindex] = pBBox->vmax.z;

				fmaxz = max(pBBox->vmax.z, fmaxz);
				fminz = min(pBBox->vmin.z, fminz);
			}

			//
			float height = fmaxz - fminz;
			float deltaZ = height/(float)GRID_Z_COUNT;
			m_vStep.z = deltaZ;

			m_pPVS->calc(pMinArray, pMaxArray, m_iCount, m_iCount, GRID_Z_COUNT
				, fminz, fmaxz, deltaZ);

			delete [] pMinArray;
			delete [] pMaxArray;

			mlog("waste time for pvs build : %d ms\n", GetTickCount() - dwStartTime);

	}

	// create lod index buffer
	m_iIndexNum = 1;
	if ( nLODLevel > 0 )
		m_iIndexNum += 16 * (m_iLODLevel-1);

	_ASSERT(NULL==m_phIndexBuffer);
	_ASSERT(NULL==m_pPrimitiveNum);
	m_phIndexBuffer = new HINDEXBUFFER[m_iIndexNum];
	m_pPrimitiveNum = new int[m_iIndexNum];

	for ( int i = 0; i<m_iIndexNum; ++i )
	{
		m_phIndexBuffer[i] = R_NONE;
		m_pPrimitiveNum[i] = 0;
	}

	int nWidth = GetWidthMap()/m_iCount;
	int nHeight = GetHeightMap()/m_iCount;

	for ( int i = 0; i<nLODLevel; ++i )
	{
		int pw = (int)pow(2.f,i);
		int w = nWidth /pw;
		int h = nHeight/pw;
		int nIndex = GetLodIndex( i, 0);

		_ASSERT(m_phIndexBuffer[nIndex]==R_NONE);
		if ( !CreateIndexNonCont(&m_phIndexBuffer[nIndex],&m_pPrimitiveNum[nIndex], w, h, i))
			return false;

		if ( i > 0 )
		{
			// 하위 4비트를 비트 플래스를 사용한다.
			for ( int k = 0x01; k<=0x0f; ++k )
			{
				nIndex = GetLodIndex( i, k );
				_ASSERT(m_phIndexBuffer[nIndex]==R_NONE);
				if ( !CreateIndexContact(&m_phIndexBuffer[nIndex],&m_pPrimitiveNum[nIndex], w, h, i, k))
					return false;
			}
		}
	}

	m_nPatchSize = nPatchSize;

	_ASSERT(REngine::GetDevicePtr()!=NULL);

	m_vStep.x = m_pHeightField->GetWidthSize()/(float)m_iCount;
	m_vStep.y = m_pHeightField->GetHeightSize()/(float)m_iCount;

	//	SetUsable(true);
	return true;
}

bool RTerrainResource::LoadDisplacementmapFromR32File(bool _bMainThreadCall, const char *filename, int nPatchSize, int nLODLevel, float fXSize, float fYSize
													  , float fZScale, float fZOffset)
{
	_ASSERT(NULL==m_pPatches);
	_ASSERT(REngine::GetDevicePtr());

	if ( !FillDisplacementMap(filename, RVector3(fXSize, fYSize, fZScale)) )
		return false;

	if ( !CreateDisplacementmapPatch(fXSize, fYSize, nPatchSize, nLODLevel))
		return false;

	_ASSERT(m_pHeightField);

	return true;
}

void RTerrainResource::DeleteAlphaTexture(int nIndex)
{
	_ASSERT(nIndex<m_iLayerNum);
	_ASSERT(m_iPatchNum);
	_ASSERT(REngine::GetDevicePtr());

	for ( int i = 0; i<m_iPatchNum; ++i )
	{
		if ( m_pLayerInfo[nIndex].phAlphaMask )
		{
			REngine::GetDevice().DeleteTexture(m_pLayerInfo[nIndex].phAlphaMask[i]);
			m_pLayerInfo[nIndex].phAlphaMask[i] = R_NONE;
		}
	}

	if( m_pLayerInfo[nIndex].pAlphaTexture)
	{
		REngine::GetDevice().DeleteTexture( m_pLayerInfo[nIndex].pAlphaTexture);
		m_pLayerInfo[nIndex].pAlphaTexture = NULL;
	}
	m_pLayerInfo[nIndex].bNeedUpdateAlphaTexture = true;

	SAFE_DELETE_ARRAY(m_pLayerInfo[nIndex].phAlphaMask);
	m_pLayerInfo[nIndex].phAlphaMask = NULL;
	SAFE_DELETE_ARRAY(m_pLayerInfo[nIndex].pbAlphaMaskCreated);
	m_pLayerInfo[nIndex].pbAlphaMaskCreated = NULL;
	SAFE_DELETE_ARRAY(m_pLayerInfo[nIndex].pbDataUsed);
	m_pLayerInfo[nIndex].pbDataUsed = NULL;
}

bool RTerrainResource::IsAlphaDataInTexture(void *pData, int nWidth, int nHeight, RECT *pRect, int nPitch, int nBpp)
{
	if( !pData )
		return false;

	int nRectWidth = pRect->right - pRect->left;
	int nRectHeight = pRect->bottom - pRect->top;

	for (int nY = 0; nY<nRectHeight; ++nY)
		for (int nX = 0; nX<nRectWidth; ++nX)
		{
			// Clamp the src-data
			int x = pRect->left + nX;
			int y = pRect->top + nY;

			if ( x < 0 )	x = 0;
			if ( x >= nWidth )	x = nWidth-1;
			if ( y < 0 )	y = 0;
			if ( y >= nHeight )	y = nHeight-1;

			BYTE *pPixelData = (BYTE*)pData + y*nWidth*nBpp;
			pPixelData += (nBpp*x) + (nBpp-1);

			if ( *pPixelData != 0x00 )
				return true;
		}

		return false;
}


void RTerrainResource::NeedUpdateAlphaTexture( int nLayerIndex)
{
	for ( int nPatchIndex = 0; nPatchIndex<m_iPatchNum; ++nPatchIndex)
	{
		m_pLayerInfo[nLayerIndex].pbAlphaMaskCreated[nPatchIndex] = false;
		m_pLayerInfo[nLayerIndex].pbDataUsed[nPatchIndex] = false;
		// 알파텍스쳐가 변경되면 마스크 텍스쳐도 변경 되어야 한다.
		m_pPatches[nPatchIndex].NeedUpdateLayerMaskTexture(true);
		m_pLayerInfo[nLayerIndex].bNeedUpdateAlphaTexture = true;
	}
}

void RTerrainResource::InitAlphaTexture(int nIndex, int nWidth, int nHeight)
{
	_ASSERT(nIndex>=0);
	_ASSERT(nIndex<m_iLayerNum);
	_ASSERT(m_pLayerInfo[nIndex].phAlphaMask==NULL);
	_ASSERT(m_iPatchNum);

	/// 레이어, 패치 당 알파 마스킹 텍스쳐
	if(!m_pLayerInfo[nIndex].phAlphaMask)
		m_pLayerInfo[nIndex].phAlphaMask = new RTexture*[m_iPatchNum];
	for ( int i = 0; i<m_iPatchNum; ++i )
		m_pLayerInfo[nIndex].phAlphaMask[i] = R_NONE;
	/// 레이어, 패치 당 알파 마스킹 텍스쳐 생성 처리 유무
	if(!m_pLayerInfo[nIndex].pbAlphaMaskCreated)
		m_pLayerInfo[nIndex].pbAlphaMaskCreated = new bool[m_iPatchNum];
	for ( int i = 0; i<m_iPatchNum; ++i )
		m_pLayerInfo[nIndex].pbAlphaMaskCreated[i] = false;
	/// 레이어, 패치 당 알파 마스킹 텍스쳐 사용 유무
	if(!m_pLayerInfo[nIndex].pbDataUsed)
		m_pLayerInfo[nIndex].pbDataUsed = new bool[m_iPatchNum];
	for ( int i = 0; i<m_iPatchNum; ++i )
		m_pLayerInfo[nIndex].pbDataUsed[i] = false;

	m_pLayerInfo[nIndex].dwAlphaWidth = nWidth;
	m_pLayerInfo[nIndex].dwAlphaHeight = nHeight;
	/// size of one patch's alpha texture
	DWORD dwSmallWidth = (m_pLayerInfo[nIndex].dwAlphaWidth/m_iCount);
	DWORD dwSmallHeight = (m_pLayerInfo[nIndex].dwAlphaHeight/m_iCount);
	/// maybe... margin of joint
	m_pLayerInfo[nIndex].dwAlphaPatchWidth = dwSmallWidth + 2;
	m_pLayerInfo[nIndex].dwAlphaPatchHeight = dwSmallHeight + 2;
}


bool RTerrainResource::IsAlphaDataInTexture(int nPatchIndex, int nLayerIndex)
{
	_ASSERT(nPatchIndex>=0);
	_ASSERT(nPatchIndex<m_iPatchNum);
	_ASSERT(nLayerIndex<m_iLayerNum);

	if(!m_pLayerInfo[nLayerIndex].pAlphaTextureRaw)
		return false;
	/// Whole size of layer
	DWORD nLayerWidth = m_pLayerInfo[nLayerIndex].dwAlphaWidth;
	DWORD nLayerHeight = m_pLayerInfo[nLayerIndex].dwAlphaHeight;
	/// size of one patch's alpha texture. this size include margin of joint
	DWORD nTextureWidth = m_pLayerInfo[nLayerIndex].dwAlphaPatchWidth;
	DWORD nTextureHeight = m_pLayerInfo[nLayerIndex].dwAlphaPatchHeight;
	// valid size check
	if( nLayerWidth <= 0 || nLayerHeight <= 0)
		return false;
	/// exclude margin of joint. 
	DWORD dwSmallWidth = nTextureWidth-2;
	DWORD dwSmallHeight = nTextureHeight-2;
	/// calculate x,y index
	int x = nPatchIndex%m_iCount;
	int y = nPatchIndex/m_iCount;
	// rect
	RECT rt;
	rt.left = (x*dwSmallWidth)-1;
	rt.top = (y*dwSmallHeight)-1;
	rt.right = rt.left + nTextureWidth;
	rt.bottom = rt.top + nTextureHeight;

	return IsAlphaDataInTexture( (void*)(m_pLayerInfo[nLayerIndex].pAlphaTextureRaw), nLayerWidth, nLayerHeight, &rt, nLayerWidth*4, 4);
}


void RTerrainResource::UpdateAlphaTexture(int nPatchIndex, int nLayerIndex)
{
	_ASSERT(nPatchIndex>=0);
	_ASSERT(nPatchIndex<m_iPatchNum);
	_ASSERT(nLayerIndex<m_iLayerNum);

	/// 해당 알파 마스크 텍스쳐가 생성 처리가 되었는지 아닌지를 체크하여 생성 처리를 한다.
	/// 단순히 phAlphaMask NULL 유무만으로는 알 수 없다; 레이어가 사용되지 않아 생성을 안한건지 생성 처리가 안된것인지 확인이 불가능하다.
	if ( m_pLayerInfo[nLayerIndex].pbAlphaMaskCreated[nPatchIndex])
		return;

	/// 알파 마스크 텍스쳐는 정말로 정직하게 a채널만 사용하므로 bit를 아끼게 A8을 사용하도록 한다.
	/// 기본적으로 ARGB32를 쓰게 하고 지원 여부를 체크
	/// 막상 지원되는 카드가 없어 검증 불가. 진행 취소.
	RFORMAT alphaTextureFormat = RFMT_ARGB32;
	///if( REngine::GetDevice().QueryFeature(RQF_RGB16))
	///	alphaTextureFormat = RFMT_RGB16;

	// m_pLayerInfo[layerIdx].phAlphaMask[patchIdx]
	RTexture* *pArrayTexture = m_pLayerInfo[nLayerIndex].phAlphaMask;
	/// Whole size of layer
	DWORD nLayerWidth = m_pLayerInfo[nLayerIndex].dwAlphaWidth;
	DWORD nLayerHeight = m_pLayerInfo[nLayerIndex].dwAlphaHeight;
	/// size of one patch's alpha texture. this size include margin of joint
	DWORD nTextureWidth = m_pLayerInfo[nLayerIndex].dwAlphaPatchWidth;
	DWORD nTextureHeight = m_pLayerInfo[nLayerIndex].dwAlphaPatchHeight;
	/// exclude margin of joint. 
	DWORD dwSmallWidth = nTextureWidth-2;
	DWORD dwSmallHeight = nTextureHeight-2;
	/// calculate x,y index
	int x = nPatchIndex%m_iCount;
	int y = nPatchIndex/m_iCount;

	if ( !IsAlphaTextureUsed( nPatchIndex, nLayerIndex))
	{
		// 알파 값이 하나도 없다면 ...
		if (pArrayTexture[nPatchIndex] != R_NONE)
			REngine::GetDevice().DeleteTexture(pArrayTexture[nPatchIndex]);

		pArrayTexture[nPatchIndex] = R_NONE;
	}
	else
	{
		// 알파 값이 하나라도 있다면 ...
		if ( pArrayTexture[nPatchIndex] == R_NONE )
			pArrayTexture[nPatchIndex] = REngine::GetDevice().CreateTexture(nTextureWidth, nTextureHeight, alphaTextureFormat );

		_ASSERT(pArrayTexture[nPatchIndex]!=R_NONE);

		REngine::GetDevice().UpdateTexture(pArrayTexture[nPatchIndex], 0
			, 0, 0, nTextureWidth, nTextureHeight
			, (x*dwSmallWidth)-1, (y*dwSmallHeight)-1
			, nLayerWidth, nLayerHeight, (void*)(m_pLayerInfo[nLayerIndex].pAlphaTextureRaw));
	}

	/// 생성 처리가 되었음을 표시
	m_pLayerInfo[nLayerIndex].pbAlphaMaskCreated[nPatchIndex] = true;
}


void RTerrainResource::UpdateAlphaTextureOfPatch(int nPatchIndex)
{
	_ASSERT(nPatchIndex>=0);
	_ASSERT(nPatchIndex<m_iPatchNum);
	if ( m_pLayerInfo == NULL)
		return;

	for ( int nLayerIndex = 0; nLayerIndex<m_iLayerNum; ++nLayerIndex )
	{
		if( m_pLayerInfo[nLayerIndex].pAlphaTextureRaw)
			UpdateAlphaTexture( nPatchIndex, nLayerIndex);
	}
}


void RTerrainResource::UpdateAlphaTextureOfLayer(int nLayerIndex)
{
	_ASSERT(nLayerIndex>=0);
	_ASSERT(nLayerIndex<m_iLayerNum);
	if ( m_pLayerInfo == NULL
		|| m_pLayerInfo[nLayerIndex].phAlphaMask == NULL)
		return;

	for ( int y = 0; y<m_iCount; ++y )
	{
		for ( int x = 0; x<m_iCount; ++x )
		{
			int nPatchIndex = x + y*m_iCount;
			UpdateAlphaTexture( nPatchIndex, nLayerIndex);
		}
	}

	m_pLayerInfo[nLayerIndex].bNeedUpdateAlphaTexture = true;
}


// void RTerrainResource::UpdateMaskTexture()
// {
// 	if( !m_iPatchNum)
// 		return;
// 
// 	RShaderFX* pFX = REngine::GetDevice().CreateShaderFX("TerrainMask.fx");
// 	RTechnique technique = pFX->GetTechnique( "MainTechnique");
// 
// 	int nPatchIdx = 0;
// 	for( nPatchIdx = 0; nPatchIdx<m_iPatchNum; nPatchIdx++)
// 	{
// 		UpdateMaskTexture( nPatchIdx);
// 	}
// 
// 	REngine::GetDevice().DeleteShaderFX(pFX);
// }



void RTerrainResource::FillLayerTile( int iIndex, const char* pTileFileName, const char* pNormalFileName, float fXSize, float fYSzie, bool bUseToSpecular )
{
	_ASSERT(m_iLayerNum>iIndex);
	_ASSERT(iIndex>=0);
	_ASSERT(REngine::GetDevicePtr());
	_ASSERT(m_pLayerInfo);

	bool bBackgroundLoading = IsBackgroundCreated();

	RTexture* pTexture			= REngine::GetDevice().CreateTexture( pTileFileName, RTF_LINEAR, RCT_AUTOGENMIPMAP, RFMT_NONE, bBackgroundLoading);

	RTexture* pNormalTexture	= R_NONE;
	if( strlen(pNormalFileName) > 0 )
	{
		if( !(pNormalTexture = REngine::GetDevice().CreateTexture( pNormalFileName, RTF_LINEAR, RCT_AUTOGENMIPMAP, RFMT_NONE, bBackgroundLoading)))
		{
			mlog("%s 파일에 해당하는 노말맵 텍스쳐 %s가 없습니다.\n", pTileFileName, pNormalFileName );
		}
	}

	m_pLayerInfo[iIndex].hTileMap			= pTexture;
	m_pLayerInfo[iIndex].hNormalMap			= pNormalTexture;
	m_pLayerInfo[iIndex].tileMapName		= pTileFileName;
	m_pLayerInfo[iIndex].normalMapName		= pNormalFileName;
	m_pLayerInfo[iIndex].bAlphaMaskForSpec	= bUseToSpecular;

	m_pLayerInfo[iIndex].vTilingUV. x = fXSize;
	m_pLayerInfo[iIndex].vTilingUV. y = fYSzie;
}

void RTerrainResource::GetPatchXYZ(int *pX, int *pY, int *pZ, const RVector *pCameraPos)
{
	_ASSERT(m_pHeightField!=NULL);

	*pX = (int)(pCameraPos->x / m_vStep.x);
	*pY = (int)(pCameraPos->y / m_vStep.y);
	*pZ = (int)((pCameraPos->z - m_pHeightField->GetMinZ()) / m_vStep.z);
}

bool RTerrainResource::CreateGrassSystem()
{
	_ASSERT(m_pGrassSystem==NULL);
	m_pGrassSystem = new RGrassSystem();
	m_pGrassSystem->Create(REngine::GetDevicePtr(), REngine::GetSceneManagerPtr());
	m_pGrassSystem->CreateRegions(m_iCount, m_iCount, GetWidthSize(), GetHeightSize());

	return true;
}


const RTerrainResource::LayerInfo* RTerrainResource::GetLayerInfo( int nIndex )
{
	if ( m_pLayerInfo && nIndex >= 0 && nIndex < m_iLayerNum )
		return &m_pLayerInfo[nIndex];

	return NULL;
}

}