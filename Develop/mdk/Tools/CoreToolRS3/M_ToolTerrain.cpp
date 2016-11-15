#include "stdafx.h"
#include "FileInfo.h"
#include "M_ToolTerrain.h"
#include "M_ToolSceneManager.h"
#include "RDeviceD3D.h"
#include "RTerrainPatch.h"
#include <ddraw.h>
#include "RUtil.h"
#include "RTerrainPatch.h"
#include "RTerrainPVS.h"
#include "RTerrainQuadTree.h"
#include "RGrassSystem.h"
#include "RRenderHelper.h"
#include "MXml.h"
#include "RPointLightSceneNode.h"
#include "RWaterSceneNode.h"
#include "MFileSystem.h"
#include "RTexture.h"
#include "RCollisionTree.h"
#include "RDeviceD3D.h"
#include "RFont.h"

//유틸리티 함수
#include "V_MFCHelper.h"

// Actor
#include "M_ToolActor.h"
#include "M_ToolStaticMeshSceneNode.h"

//  [6/22/2006]
#include "MDebug.h"
#include <algorithm>

#include "MFileLoader.h"

#include "RTreeSceneNode.h"
#include "RTreeResourceMgr.h"

#include "RShadowStrategy.h"

#include "d3dx9.h"

using namespace std;

/**
 TODO : m_nLightMapWidth, m_nLightMapHeight 에 대해 재정리 필요.
*/
namespace rs3 {

DWORD RToolTerrain::GRID_TEXTURE_COLOR = D3DCOLOR_XRGB(60, 80, 140);
DWORD RToolTerrain::MARKING_TEXTURE_COLOR = D3DCOLOR_XRGB(255, 1, 1);

MImplementRTTI(RToolTerrain, RTerrain);

FLOAT McUtil_DotAngle(D3DXVECTOR3* p0, D3DXVECTOR3* p1, D3DXVECTOR3* p2)
{
	D3DXVECTOR3 vcA = *p1 - *p0;
	D3DXVECTOR3 vcB = *p2 - *p0;

	D3DXVec3Normalize(&vcA,&vcA);
	D3DXVec3Normalize(&vcB,&vcB);

	FLOAT fDot = D3DXVec3Dot(&vcA, &vcB);

	if(fDot>0.9999f)
		return 0.f;
	else if(fDot<-0.9999f)
		return D3DX_PI;

	fDot = acosf(fDot);

	return fDot;
}

BOOL McMath_CollisionLineToTriangle(D3DXVECTOR3* vcOut, D3DXVECTOR3* pTri, D3DXVECTOR3* pLine)
{
	D3DXPLANE	plane;
	D3DXVECTOR3 vcN;
	FLOAT		fD;

	D3DXVECTOR3 vcA = pTri[1] - pTri[0];
	D3DXVECTOR3 vcB = pTri[2] - pTri[0];

	D3DXVec3Cross(&vcN, &vcA, &vcB);
	D3DXVec3Normalize(&vcN,&vcN);

	fD = -D3DXVec3Dot(&vcN, &pTri[0]);

	plane = D3DXPLANE(vcN.x, vcN.y, vcN.z, fD);

	FLOAT fD1 = D3DXPlaneDotCoord(&plane, &pLine[0]);
	FLOAT fD2 = D3DXPlaneDotCoord(&plane, &pLine[1]);

	if(fD1 * fD2>0)											// Not Collision
		return FALSE;

	D3DXVECTOR3 vcL;
	D3DXVECTOR3 vcP;
	FLOAT		fT;
	FLOAT		fA;
	FLOAT		fB;
	FLOAT		fC;

	vcL = pLine[1]-pLine[0];
	D3DXVec3Normalize(&vcL, &vcL);

	fT = D3DXVec3Dot(&vcL, &vcN);

	if( 0.f == fT)											//평면과 평행한 경우
	{
		fA = McUtil_DotAngle(&pLine[0], &pTri[0], &pTri[1]);
		fB = McUtil_DotAngle(&pLine[0], &pTri[1], &pTri[2]);
		fC = McUtil_DotAngle(&pLine[0], &pTri[2], &pTri[0]);

		if(fA+fB+fC>=(D3DX_PI*2-0.001f))
			return TRUE;

		fA = McUtil_DotAngle(&pLine[1], &pTri[0], &pTri[1]);
		fB = McUtil_DotAngle(&pLine[1], &pTri[1], &pTri[2]);
		fC = McUtil_DotAngle(&pLine[1], &pTri[2], &pTri[0]);

		if(fA+fB+fC>=(D3DX_PI*2-0.001f))
			return TRUE;
	}

	fT = -( fD+ D3DXVec3Dot(&vcN, &pLine[0]))/fT;

	vcP = pLine[0] + fT * vcL;

	fA = McUtil_DotAngle(&vcP, &pTri[0], &pTri[1]);
	fB = McUtil_DotAngle(&vcP, &pTri[1], &pTri[2]);
	fC = McUtil_DotAngle(&vcP, &pTri[2], &pTri[0]);

	*vcOut = vcP;

	if(fA+fB+fC>=(D3DX_PI*2-0.001f))
		return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//
std::string RToolTerrain::CreateName::DisplacementMapFile(RResource *pRes)
{
	_ASSERT(pRes);
	return pRes->GetName() + "_dis.dds";
}

std::string RToolTerrain::CreateName::BaseMapFile(RResource *pRes)
{
	_ASSERT(pRes);
	return pRes->GetName() + BAKED_MAP_FILENAME;
}

std::string RToolTerrain::CreateName::GrassFile(RResource *pRes)
{
	_ASSERT(pRes);
	return pRes->GetName() + "_grass.grs";
}

std::string RToolTerrain::CreateName::AlphaLayerFile(RResource *pRes, int nIndex)
{
	_ASSERT(nIndex>=0);

	const std::string &resourceName = pRes->GetName();
	_ASSERT(!resourceName.empty());

	char pcBuffer[MAX_PATH];
	pcBuffer[0] = '\0';

	sprintf_s(pcBuffer, sizeof(pcBuffer), "%s_mask%d.dds", resourceName.c_str(), nIndex);
	return std::string(pcBuffer);
}

std::string RToolTerrain::CreateName::LightMapFile(RResource *pRes, int nIndex)
{
	_ASSERT(nIndex>=0);
	_ASSERT(pRes);
	const std::string &resourceName = pRes->GetName();
	_ASSERT(!resourceName.empty());

	char pcBuffer[MAX_PATH];
	pcBuffer[0] = '\0';

	sprintf_s(pcBuffer, sizeof(pcBuffer), "%s_light%d.dds", resourceName.c_str(), nIndex);
	return std::string(pcBuffer);
}

//////////////////////////////////////////////////////////////////////////

RToolTerrain::RToolTerrain(RDevice *pDevice)
: RTerrain()
{
	m_pAttendInfo = NULL;

	m_hGuideVertex = R_NONE;

	m_bAnisotropyFilter = true;

	m_hGridTexture = R_NONE;
	m_hMarkingTexture = R_NONE;
	m_hNoneTexture = R_NONE;

	bool bResult = false;
	bResult = CreateUtlityTexture();
	_ASSERT(bResult);

	m_nTotalAlphaTextureNum = 0;

	m_hMaterialMap = R_NONE;
	m_pMaterialMap = NULL;

	m_bObjectLight = false;
	_ASSERT(bResult);
}

RToolTerrain::~RToolTerrain()
{
	assert(REngine::GetDevicePtr());

	RDevice &device = REngine::GetDevice();
	
	device.DeleteTexture(m_hGridTexture);
	m_hGridTexture = R_NONE;
	device.DeleteTexture(m_hMarkingTexture);
	m_hMarkingTexture = R_NONE;
	device.DeleteTexture(m_hNoneTexture);
	m_hNoneTexture = R_NONE;
	device.DeleteTexture(m_hMaterialMap);
	m_hMaterialMap = R_NONE;

	SAFE_DELETE_ARRAY(m_pMaterialMap);
	SAFE_DELETE_ARRAY(m_pAttendInfo);
}

bool RToolTerrain::CreateUtlityTexture()
{
	_ASSERT(REngine::GetDevicePtr()!=NULL);
	
	m_hGridTexture = REngine::GetDevice().CreateTexture(1, 1, RFMT_ARGB32, RTF_BILINEAR, 0,(void*)&GRID_TEXTURE_COLOR);
	if ( m_hGridTexture == R_NONE )
		return false;
	m_hMarkingTexture = REngine::GetDevice().CreateTexture(1, 1, RFMT_ARGB32, RTF_BILINEAR, 0, (void*)&MARKING_TEXTURE_COLOR);
	if ( m_hMarkingTexture == R_NONE )
		return false;

	DWORD texture4x4[2*2] = 
	{
		0xffFFD800, 0xff000000, 
		0xff000000, 0xffFFD800, 
	};

	m_hNoneTexture = REngine::GetDevice().CreateTexture(2, 2, RFMT_ARGB32, RTF_BILINEAR, 0, (void*)texture4x4);
	if ( m_hNoneTexture == R_NONE )
		return false;

	return true;
}

void RToolTerrain::SetBaseMapName()
{
	RTerrain::SetBaseMapName( CreateName::BaseMapFile(this));
}

void RToolTerrain::CreateLayer(int nNum, int nPatchNum)
{
	if ( nNum == 0 )	
		return;

	RTerrain::CreateLayer(nNum, nPatchNum);

	_ASSERT(m_pAttendInfo==NULL);
	m_pAttendInfo = new LayerInfoAttend[nNum];
	for ( int i = 0; i<nNum; ++i )
		m_pAttendInfo[i].bMapChanged = true;
}

const ILuint SCALE_METHOD = ILU_SCALE_LANCZOS3;

bool RToolTerrain::ResizeLayerAlpha(int nIndex ,int nPower, bool bMultiply /* = true */)
{
	if ( nIndex < 0 )	return false;
	if ( nIndex >= m_iLayerNum )	return false;

	LayerInfo *pLInfo = &m_pLayerInfo[nIndex];
	LayerInfoAttend *pAInfo = &m_pAttendInfo[nIndex];
	
	DWORD dwRWidth = ( bMultiply ? pLInfo->dwAlphaPatchWidth * nPower : pLInfo->dwAlphaPatchWidth / nPower);
	DWORD dwRHeight = ( bMultiply ? pLInfo->dwAlphaPatchHeight * nPower : pLInfo->dwAlphaPatchHeight / nPower);

	if ( dwRWidth > 256 || dwRHeight > 256 )
		return false;
	if ( dwRWidth < 4 || dwRHeight < 4 )
		return false;

	DWORD dwRAlphaWidth = (dwRWidth-2) * m_iCount;
	DWORD dwRAlphaHeight = (dwRHeight-2) * m_iCount;

	int nSize = dwRAlphaWidth*dwRAlphaHeight*4;
	BYTE *pNewAlphaMap = new BYTE[nSize];
	ZeroMemory(pNewAlphaMap, nSize);

	ILuint nID;
	ilGenImages(1,&nID);
	ilBindImage(nID);
	ilTexImage(m_pLayerInfo[nIndex].dwAlphaWidth, m_pLayerInfo[nIndex].dwAlphaHeight, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, (void*)m_pLayerInfo[nIndex].pAlphaTextureRaw);
	iluImageParameter(ILU_FILTER, SCALE_METHOD);
	iluScale(dwRAlphaWidth, dwRAlphaHeight, 1);

	ilCopyPixels(0, 0, 0, dwRAlphaWidth, dwRAlphaHeight, 1, IL_BGRA, IL_UNSIGNED_BYTE,(void*)pNewAlphaMap);
	ilDeleteImages(1,&nID);

	DeleteAlphaTexture(nIndex);
	RTerrain::InitAlphaTexture(nIndex, dwRAlphaWidth, dwRAlphaHeight);

	DeleteAlphaTextureRaw( nIndex);
	SetAlphaTextureRaw( nIndex, pNewAlphaMap);

	return true;
}

bool RToolTerrain::ResizeLayerAlphaMul2X(int nIndex)
{
	return ResizeLayerAlpha(nIndex, 2, true);
}

bool RToolTerrain::ResizeLayerAlphaDiv2X(int nIndex)
{
	return ResizeLayerAlpha(nIndex, 2, false);
}

bool RToolTerrain::LoadDisplacementmapFromR32File(bool _bMainThreadCall, const char *filename, int nPatchSize, int nLODLevel, float fXSize, float fYSize
												  ,float fZScale , float fZOffset)
{
	bool bDisplacementLoad = RTerrain::LoadDisplacementmapFromR32File(_bMainThreadCall, filename, nPatchSize, 3, fXSize, fYSize, fZScale, fZOffset);
	if ( bDisplacementLoad == false )
		return false;

	CreateGuideVertex();
	return true;
}

bool RToolTerrain::CreateNewTerrain(const char *pPath, const char *pResourceName, const char *pBaseTileName, int nWidth, int nHeight, float fXSize, float fYSize)
{
	// resource and path setting
	if ( pPath )
		SetPath(pPath);
	else
		SetPath("");

	if ( pResourceName )
		SetResourceName(pResourceName);
	else
		SetResourceName("");

	///////////////////////////////////////////////////////////////////////////

	SetBaseMapName();

	bool bCreateSuccess = CreateDisplacementmapPlane( nWidth, nHeight, fXSize, fYSize );
	if( false == bCreateSuccess )
		return false;

	bCreateSuccess = CreateGrass();
	if( false == bCreateSuccess )
		return false;

	bCreateSuccess = AddLayer(pBaseTileName, true);
	if( false == bCreateSuccess )
		return false;

	return true;
}

bool RToolTerrain::UpdateNormal(int nWidth, int nHeight, float fXSize, float fYSize)
{
	assert(NULL!=m_pHeightField);

	return CreateNormalFieldMap(nWidth, nHeight, fXSize, fYSize);
}

bool RToolTerrain::CreateDisplacementmapPlane(int nWidth, int nHeight, float fXSize, float fYSize)
{
	_ASSERT(m_pHeightField==NULL);

	int nPatchSize = 32;

	// displacement map name
	m_strDisplacementMapName = CreateName::DisplacementMapFile(this);

	m_pHeightField = new RHeightField();
	if ( m_pHeightField->Create(nWidth, nHeight, MVector2(fXSize, fYSize)) == false )
	{
		SAFE_DELETE(m_pHeightField);
		return false;
	}

	int nMapSize = m_pHeightField->GetWidthMap() * m_pHeightField->GetHeightMap();
	int nMapWidth = m_pHeightField->GetWidthMap();

	m_iCount = nMapWidth/nPatchSize;
	m_iPatchNum = m_iCount*m_iCount;

	assert(NULL!=m_pHeightField);

	CreateNormalFieldMap(nWidth, nHeight, fXSize, fYSize);
	bool bCreatePatch = CreateDisplacementmapPatch(fXSize, fYSize, PATCH_SIZE, LOD_LEVEL);
	if ( !bCreatePatch )
		return false;
	return true;
}

bool RToolTerrain::CreateGrass()
{
	if(!CreateGrassSystem())
		return false;

	_ASSERT(m_pGrassSystem);
	m_strGrassName = CreateName::GrassFile(this);

	return true;
}

bool RToolTerrain::SaveDisplacementmapToFile()
{
	if ( m_pHeightField == NULL )
		return false;

	// Path + Filename
	std::string strFileName = m_strPathName + m_strDisplacementMapName;
	m_pHeightField->SaveHeightFieldmapToR32F(strFileName.c_str());

	RECT rt;
	rt.left = 0;
	rt.top = 0;
	rt.right = GetWidthMap();
	rt.bottom = GetHeightMap();
	UpdateDescriptMapAndMaterial(rt);

	std::string descFileName = strFileName.substr(0, strFileName.find('.')) + ".desc";
	m_pHeightField->SaveHeightFieldDescFile(descFileName.c_str());
    OutputDebugString("Displacement Map Save\n");

	return true;
}

bool RToolTerrain::SaveAlphaMapToFile()
{
	RDeviceD3D *pd3dDevice = (RDeviceD3D*)REngine::GetDevicePtr();

	for ( int i = 0; i<m_iLayerNum; ++i )
	{
		LayerInfo *pLInfo = &m_pLayerInfo[i];
		LayerInfoAttend *pAInfo = &m_pAttendInfo[i];

		// 저장하기 전에 파일이름 재설정 - 레이어 삭제, 추가로 인한 이름 중복 방지
		pLInfo->alphaMapName = CreateName::AlphaLayerFile(this, i);

		std::string strFileName = m_strPathName + pLInfo->alphaMapName;

		if ( pAInfo->bMapChanged == false 
			&& CMFCHelper::FileExist(strFileName.c_str()) )
			continue;

		if ( pLInfo->phAlphaMask != NULL
			&& pLInfo->pAlphaTextureRaw != NULL )
		{
			bool bResult = MFileLoad::SaveFromNBitRawToDDSFile(strFileName.c_str(), pLInfo->dwAlphaWidth, pLInfo->dwAlphaHeight, 4, pLInfo->pAlphaTextureRaw);
			assert(bResult && "Texture->File");
			OutputDebugString("Alpha Layer Save\n");
		}
	}

	return true;
}

bool RToolTerrain::RecreateAndSaveBakedMap()
{
	if( false == CreateBakedMap() )
		return false;

	_ASSERT( false == m_strBaseMapName.empty() );
	string strBaseMapName = string(GetPath()) + m_strBaseMapName;

	RDeviceD3D* device = (RDeviceD3D*)REngine::GetDevicePtr();

	// D3D texture로 DXT1 포맷으로 저장
	if( device->GetD3DTexture(m_hBaseMap))
	{
		/// 만든 텍스쳐를 메모리에 저장해둔다
		LPD3DXBUFFER pBuffer = NULL;
		D3DXSaveTextureToFileInMemory(&pBuffer,D3DXIFF_DDS, device->GetD3DTexture(m_hBaseMap), NULL);
	
		/// 밉맵을 만들고 압축텍스쳐로 변환한다
		D3DXIMAGE_INFO diffuseInfo;
		LPDIRECT3DTEXTURE9 pTargetTexture;
		if( FAILED(D3DXCreateTextureFromFileInMemoryEx( device->GetD3DDevice(),pBuffer->GetBufferPointer(), pBuffer->GetBufferSize(), 
			D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT/*mipmap*/, 0/*usage*/,
			D3DFMT_DXT1 ,D3DPOOL_SYSTEMMEM,	D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR,
			0, &diffuseInfo, NULL, &pTargetTexture )))
		{
			_ASSERT(FALSE);
			return false;
		}

		bool bSuccess = (D3D_OK == D3DXSaveTextureToFile( strBaseMapName.c_str(),D3DXIFF_DDS, pTargetTexture, NULL));

		pTargetTexture->Release();
		pBuffer->Release();

		return bSuccess;
	}
	else
		return m_hBaseMap->SaveToFile( strBaseMapName.c_str() );
}

bool RToolTerrain::FloodFillMapValidMaker(int nX, int nY, float fSlopeDot)
{
	if (nX<0)	return false;
	if (nY<0)	return false;
	if (nX>=m_pHeightField->GetWidthMap())	return false;
	if (nY>=m_pHeightField->GetHeightMap()) return false;

	if ( m_pHeightField->GetAttribute(nX, nY, RHeightField::VALID_POS))	
		return false;

	// 기울기 검사
	MVector3 vNormal1, vNormal2;
	float fX = nX*GetXScale();
	float fY = nY*GetYScale();

	m_pHeightField->GetHeightAt(NULL,&vNormal1, fX, fY);
	m_pHeightField->GetHeightAt(NULL,&vNormal2, fX+ (GetXScale()*0.9f), fY+(GetYScale()*0.6f));

	MVector3 vNormal = vNormal1 + vNormal2;
	vNormal.Normalize();

	if (vNormal.DotProduct(MVector3::AXISZ) < fSlopeDot)	
		return false;

	m_pHeightField->SetAttribute(nX, nY, RHeightField::VALID_POS, true);
	
	return true;
}

void RToolTerrain::UpdateDescriptMapMakedFromSeed(const MVector3 seedPosition, float fSlopeDot)
{
	_ASSERT(m_pHeightField);

	int nX = (int)seedPosition.x/GetXScale();
	int nY = (int)seedPosition.y/GetYScale();

	struct IVec2
	{
		int nx, ny;
		IVec2()
		{
			nx = -1;
			ny = -1;
		}
		IVec2(int x, int y)
			: nx(x), ny(y) {}
	};

	queue<IVec2> dfsQueue;
	dfsQueue.push(IVec2(nX, nY));

	while(!dfsQueue.empty())
	{
		IVec2 vec;
		vec = dfsQueue.front();
		dfsQueue.pop();
		if ( FloodFillMapValidMaker(vec.nx, vec.ny, fSlopeDot))
		{
			IVec2 vec1(vec.nx+1, vec.ny);
			IVec2 vec2(vec.nx-1, vec.ny);
			IVec2 vec3(vec.nx, vec.ny+1);
			IVec2 vec4(vec.nx, vec.ny-1);

			dfsQueue.push(vec1);
			dfsQueue.push(vec2);
			dfsQueue.push(vec3);
			dfsQueue.push(vec4);
		}
		else
		{
		}
	}
}

void RToolTerrain::UpdateDescriptMapMakedAll(bool bFlag)
{
	for ( int nY = 0; nY<GetHeightMap(); ++nY)
		for  ( int nX = 0; nX<GetWidthMap(); ++nX)
			m_pHeightField->SetAttribute(nX, nY, RHeightField::VALID_POS, bFlag);
}

void RToolTerrain::UpdateDescriptMapMakedBySlope(float fSlopeDot)
{
	for ( int nY = 0; nY<GetHeightMap(); ++nY)
		for  ( int nX = 0; nX<GetWidthMap(); ++nX)
		{
			// 기울기 검사만 ...
			MVector3 vNormal1, vNormal2;
			float fX = nX*GetXScale();
			float fY = nY*GetYScale();

			m_pHeightField->GetHeightAt(NULL,&vNormal1, fX, fY);
			m_pHeightField->GetHeightAt(NULL,&vNormal2, fX+ (GetXScale()*0.9f), fY+(GetYScale()*0.6f));

			MVector3 vNormal = vNormal1 + vNormal2;
			vNormal.Normalize();

			if (vNormal.DotProduct(MVector3::AXISZ) < fSlopeDot)	
				m_pHeightField->SetAttribute(nX, nY, RHeightField::VALID_POS, false);
		}
}

/**
 참조 식 : u vs p
 u = (p(x)+.5)/width
 p(x) = (u*width)- .5
*/
void RToolTerrain::UpdateDescriptMapAndMaterial(RECT rect)
{
	_ASSERT(m_iLayerNum>0);
	_ASSERT(m_pLayerInfo);

	if ( rect.left < 0 )	rect.left = 0;
	if ( rect.top  < 0 )	rect.top = 0;
	if ( rect.right > GetWidthMap())	rect.right = GetWidthMap();
	if ( rect.bottom > GetHeightMap())	rect.bottom = GetHeightMap();

	float fOffsetSrcX = 1.0f/(float)GetWidthMap();
	float fOffsetSrcY = 1.0f/(float)GetHeightMap();

	for (int y = rect.top; y<rect.bottom; ++y)
		for (int x = rect.left; x<rect.right; ++x)
		{
			float left = (x*fOffsetSrcX);
			float top = (y*fOffsetSrcY);
			float right = left + fOffsetSrcX;
			float bottom = top + fOffsetSrcY;

			int nID = ActuallyAlphaMask(left, top, right, bottom);
			if ( nID >= 0 )
				m_pHeightField->SetMaterial( x, y, nID );
		}
}

bool RToolTerrain::GetAllWaterPlane(RSceneNode *pSceneNode)
{
	if ( _stricmp(pSceneNode->GetNodeIDString(), RSID_WATERPLANE) == 0 )
		return true;
	else
		return false;
}

void RToolTerrain::UpdateDescriptMapUnderWater()
{
	vector<RSceneNode*> vWaterArray;
	REngine::GetSceneManager().GetRootSceneNode().SerializeToVector(GetAllWaterPlane, vWaterArray);

	size_t nSize = vWaterArray.size();
	if ( nSize == 0 )
		return;

	int nWidth = GetWidthMap();
	int nHeight = GetHeightMap();
	float fMaxZ = GetAABB().vmax.z + 1000.f;

	const float WATER_DEPTH_LIMIT = 100.f;
	RPICKINFO kPickInfo;

	int nMapSize = (nWidth+1)*(nHeight+1);
	int *pMap = new int[nMapSize];
	memset(pMap, 0, sizeof(int)*nMapSize);

	// boolean table 맵을 만든다.
	for (int y = 0; y<=nHeight; ++y)
	{
		for (int x = 0; x<=nWidth; ++x)
		{
			float fX1 = x*GetXScale();
			float fY1 = y*GetYScale();
			RVector vPos = RVector(fX1, fY1, 0.0f);
			
			float fHeight;
			if ( !m_pHeightField->GetHeightAt(&fHeight, NULL, vPos.x, vPos.y))
				continue;

			vPos.z = fHeight;
			kPickInfo.inPickOrigin = vPos;
			kPickInfo.inPickTo = vPos + RVector(0, 0, fMaxZ);
			
			int nIndex = x + y*(nWidth+1);

			for ( size_t si = 0; si<nSize; ++si )
			{
				RSceneNode *pNode = vWaterArray[si];
				RWaterPlane *pPlane = (RWaterPlane*)pNode;

				if ( pPlane->PickPlane(kPickInfo))
					if ( kPickInfo.outPosition.z - fHeight > WATER_DEPTH_LIMIT )
					{
						pMap[nIndex] = 1;
						break;
					}
			}
		}
	}

	// 테이블을 참조하여, Desc에 넣는다.
	for (int y = 0; y<nHeight; ++y)
		for (int x = 0; x<nWidth; ++x)
		{
			int nIndex0 = x + y*(nWidth+1);
			int nIndex1 = x+1 + y*(nWidth+1);
			int nIndex2 = x + (y+1)*(nWidth+1);
			int nIndex3 = x+1 + (y+1)*(nWidth+1);
			bool bUnderWater = (pMap[nIndex0] && pMap[nIndex1] && pMap[nIndex2] && pMap[nIndex3]);

			m_pHeightField->SetAttribute(x, y, RHeightField::UNDER_WATER, bUnderWater);
		}

	SAFE_DELETE_ARRAY(pMap);
	pMap = NULL;

	return;
}

void RToolTerrain::SetTiling(int nIndex, rs3::RVector &uvw)
{
	assert(nIndex<m_iLayerNum);
	m_pLayerInfo[nIndex].vTilingUV = uvw;
}

void RToolTerrain::DeleteLayer(int nIndex)
{
	if ( nIndex < 0 )	return;
	if ( nIndex >= m_iLayerNum )	return;

	LayerInfo *pNewLayerInfo = new LayerInfo[m_iLayerNum-1];
	LayerInfoAttend *pNewAttendInfo = new LayerInfoAttend[m_iLayerNum-1];

	for ( int s = 0, d = 0; s<m_iLayerNum; ++s)
	{
		if ( s == nIndex )	continue;

		pNewLayerInfo[d] = m_pLayerInfo[s];
		pNewAttendInfo[d] = m_pAttendInfo[s];
		++d;
	}

	_ASSERT(m_pLayerInfo[nIndex].phAlphaMask);
	for ( int i = 0; i<m_iPatchNum; ++i )
	{
		REngine::GetDevice().DeleteTexture( m_pLayerInfo[nIndex].phAlphaMask[i] );
		m_pLayerInfo[nIndex].phAlphaMask[i] = R_NONE;
	}
	SAFE_DELETE_ARRAY(m_pLayerInfo[nIndex].phAlphaMask);

	REngine::GetDevice().DeleteTexture( m_pLayerInfo[nIndex].hTileMap );
	REngine::GetDevice().DeleteTexture( m_pLayerInfo[nIndex].hNormalMap );
	DeleteAlphaTextureRaw(nIndex);
	SAFE_DELETE_ARRAY(m_pLayerInfo);
	SAFE_DELETE_ARRAY(m_pAttendInfo);

	m_pLayerInfo = pNewLayerInfo;
	m_pAttendInfo = pNewAttendInfo;
	--m_iLayerNum;

	// Mask 검사, 그리지 않는 부분에 일단 Flag 체크
	assert(m_pPatches);
	for ( int i = nIndex; i < m_iLayerNum; ++i )
		//m_pAttendInfo[i].bMapChanged = true;
		UpdateAlphaTexture( i);

	CountAlphaTexture();
	return;
}

bool RToolTerrain::ChangeTileMap(int nIndex, const char *pTileName)
{
	if ( nIndex < 0 )	return false;
	if ( nIndex >= m_iLayerNum )	return false;

	string szPath;
	string szTileFileName;

	RTexture* hTexture = R_NONE;

	szTileFileName = MGetRemovePathName(pTileName);
	hTexture = REngine::GetDevice().CreateTexture(szTileFileName.c_str(), RTF_TRILINEAR);
	if ( hTexture == R_NONE )
		return false;

	LayerInfo *pLayerInfo = &m_pLayerInfo[nIndex];
	REngine::GetDevice().DeleteTexture( pLayerInfo->hTileMap );
	pLayerInfo->hTileMap		= hTexture;
	pLayerInfo->tileMapName		= szTileFileName;
	
	return true;
}

bool RToolTerrain::ChangeNormalMap(int nIndex, const char *pTileName)
{
	if ( nIndex < 0 )	return false;
	if ( nIndex >= m_iLayerNum )	return false;

	string szPath;
	string szTileFileName;

	RTexture* hTexture = R_NONE;

	szTileFileName = MGetRemovePathName(pTileName);
	hTexture = REngine::GetDevice().CreateTexture(szTileFileName.c_str(), RTF_TRILINEAR);
	if ( hTexture == R_NONE )
		return false;

	LayerInfo *pLayerInfo = &m_pLayerInfo[nIndex];
	REngine::GetDevice().DeleteTexture( pLayerInfo->hNormalMap );
	pLayerInfo->hNormalMap		= hTexture;
	pLayerInfo->normalMapName	= szTileFileName;

	return true;
}

bool RToolTerrain::DeleteNormalMap(int nIndex)
{
	if ( nIndex < 0 )	return false;
	if ( nIndex >= m_iLayerNum )	return false;

	string szTileFileName;
	RTexture* hTexture = R_NONE;

	LayerInfo *pLayerInfo = &m_pLayerInfo[nIndex];
	REngine::GetDevice().DeleteTexture( pLayerInfo->hNormalMap );
	pLayerInfo->hNormalMap		= hTexture;
	pLayerInfo->normalMapName	= szTileFileName;

	return true;
}


bool RToolTerrain::SwapLayer(int nIndex1, int nIndex2)
{
	if ( nIndex1 < 0 )	return false;
	if ( nIndex1 >= m_iLayerNum )	return false;
	if ( nIndex2 < 0 )	return false;
	if ( nIndex2 >= m_iLayerNum )	return false;

	LayerInfo &rLayerInfo1 = m_pLayerInfo[nIndex1];
	LayerInfoAttend &rAttendinfo1 = m_pAttendInfo[nIndex1];

	LayerInfo &rLayerInfo2 = m_pLayerInfo[nIndex2];
	LayerInfoAttend &rAttendinfo2 = m_pAttendInfo[nIndex2];

	swap(rLayerInfo1.alphaMapName	, rLayerInfo2.alphaMapName);						/// alphaMapName
	swap(rLayerInfo1.tileMapName	, rLayerInfo2.tileMapName);							/// tileMapName
	swap(rLayerInfo1.normalMapName	, rLayerInfo2.normalMapName);						/// normalMapName
	swap(rLayerInfo1.pAlphaTextureRaw	, rLayerInfo2.pAlphaTextureRaw);				/// pAlphaTextureRaw
	swap(rLayerInfo1.hTileMap		, rLayerInfo2.hTileMap);							/// hTileMap
	swap(rLayerInfo1.hNormalMap		, rLayerInfo2.hNormalMap);							/// hNormalMap
	swap(rLayerInfo1.vTilingUV		, rLayerInfo2.vTilingUV);							/// vTilingUV
	swap(rLayerInfo1.bAlphaMaskForSpec, rLayerInfo2.bAlphaMaskForSpec);					/// bAlphaMaskForSpec
	swap(rLayerInfo1.phAlphaMask	, rLayerInfo2.phAlphaMask);							/// phAlphaMask
	swap(rLayerInfo1.dwAlphaWidth	, rLayerInfo2.dwAlphaWidth);						/// dwAlphaWidth
	swap(rLayerInfo1.dwAlphaHeight	, rLayerInfo2.dwAlphaHeight);						/// dwAlphaHeight

	swap(rLayerInfo1.bNeedUpdateAlphaTexture	, rLayerInfo2.bNeedUpdateAlphaTexture);	/// bNeedUpdateAlphaTexture
	swap(rLayerInfo1.pAlphaTexture	, rLayerInfo2.pAlphaTexture);						/// pAlphaTexture
	swap(rLayerInfo1.dwAlphaPatchWidth	, rLayerInfo2.dwAlphaPatchWidth);				/// dwAlphaPatchWidth
	swap(rLayerInfo1.dwAlphaPatchHeight	, rLayerInfo2.dwAlphaPatchHeight);				/// dwAlphaPatchHeight
	swap(rLayerInfo1.pbAlphaMaskCreated	, rLayerInfo2.pbAlphaMaskCreated);				/// pbAlphaMaskCreated
	swap(rLayerInfo1.pbDataUsed, rLayerInfo2.pbDataUsed);								/// pbDataUsed

	//rAttendinfo1.bMapChanged = true;
	//rAttendinfo2.bMapChanged = true;
	UpdateAlphaTexture( nIndex1);
	UpdateAlphaTexture( nIndex2);

	return true;
}

bool RToolTerrain::AddLayer(RTexture *pTileTexture, RTexture* pNormalTexture, bool bFillAlpha)
{
	// 데이터 저장소 새로 생성
	LayerInfo *pNewLayerInfo = new LayerInfo[m_iLayerNum+1];
	LayerInfoAttend *pNewAttendInfo = new LayerInfoAttend[m_iLayerNum+1];

	char pcName[MAX_PATH];
	char pcBaseName[MAX_PATH];

	memset(pcName, 0, sizeof(pcName));
	memset(pcBaseName, 0, sizeof(pcBaseName));

	// 기존 데이터 복사
	for ( int i = 0; i<m_iLayerNum; ++i )
	{
		pNewLayerInfo[i] = m_pLayerInfo[i];
		pNewAttendInfo[i] = m_pAttendInfo[i];
	}

	// 다음 레이어( 레이어 증가)
	++m_iLayerNum;

	// 새로운 레이어 초기화
	int nMakeIndex = m_iLayerNum-1;
	char szTileFileName[MAX_PATH];
	szTileFileName[0] = '\0';

	if ( pTileTexture )
		SplitPath(pTileTexture->GetName().c_str(), NULL, szTileFileName);

	// Layer 정보
	pNewLayerInfo[nMakeIndex].alphaMapName = CreateName::AlphaLayerFile(this, nMakeIndex);
	pNewLayerInfo[nMakeIndex].tileMapName  = szTileFileName;
	pNewLayerInfo[nMakeIndex].vTilingUV	= RVector(500.f, 500.0f, 1.0f);
	pNewLayerInfo[nMakeIndex].hTileMap		= pTileTexture;
	// dwAlphaWidth, dwAlphaHeight, bMapChanged는 CreateAlphaTexture()에서 수행 됨.

	char szNormalFileName[MAX_PATH];
	szNormalFileName[0] = '\0';
	if ( pNormalTexture )
		SplitPath(pNormalTexture->GetName().c_str(), NULL, szNormalFileName);

	// normal
	pNewLayerInfo[nMakeIndex].normalMapName = szNormalFileName;
	pNewLayerInfo[nMakeIndex].hNormalMap	= pNormalTexture;

	// Alpha map 생성
	const UINT TEXTURE_SIZE = 32;
	DWORD dwWidth = (TEXTURE_SIZE-2)*m_iCount;
	DWORD dwHeight = (TEXTURE_SIZE-2)*m_iCount;

	// 기존 저장소 제거 후 새로운 저장소 적재
	SAFE_DELETE_ARRAY(m_pLayerInfo);
	SAFE_DELETE_ARRAY(m_pAttendInfo);
	m_pLayerInfo = pNewLayerInfo;
	m_pAttendInfo = pNewAttendInfo;

	// create Alpha texture, m_pLayerInfo가 설정된다음에 호출되어야한다.
	CreateAlphaTexture( nMakeIndex, bFillAlpha, dwWidth, dwHeight);
	InitAlphaTexture( nMakeIndex, dwWidth, dwHeight);
	UpdateAlphaTexture( nMakeIndex);

	return true;
}

void RToolTerrain::CreateAlphaTexture( int nIndex, bool bFillAlpha, DWORD dwWidth, DWORD dwHeight )
{
	if (dwWidth == 0 || dwHeight == 0)
	{
		const UINT TEXTURE_SIZE = 32;
		dwWidth = (TEXTURE_SIZE-2)*m_iCount;
		dwHeight = (TEXTURE_SIZE-2)*m_iCount;

		m_pLayerInfo[nIndex].dwAlphaWidth = dwWidth;
		m_pLayerInfo[nIndex].dwAlphaHeight = dwHeight;
	}
	m_pAttendInfo[nIndex].bMapChanged = true;

	DWORD dwSize = dwWidth*dwHeight*4;
	BYTE* pNewAlphaTexture = new BYTE[dwSize];
	BYTE nFillColor;
	if ( bFillAlpha )
		nFillColor = 0xff;
	else
		nFillColor = 0x00;

	DWORD *pAlphaTextureRaw = (DWORD*)pNewAlphaTexture;
	BYTE colorB = 0x00;
	BYTE colorG = 0x00;
	BYTE colorR = 0x00;
	for ( UINT ui = 0; ui<dwSize/4; ++ui )
		pAlphaTextureRaw[ui] = D3DCOLOR_ARGB( nFillColor, colorR, colorG, colorB );

	SetAlphaTextureRaw( nIndex, pNewAlphaTexture);
}

bool RToolTerrain::AddLayer(const char *pTileName, bool bFillAlpha /* = false */)
{
	RTexture* hNormalTexture = R_NONE;
	RTexture* hTileTexture = REngine::GetDevice().CreateTexture(pTileName, RTF_TRILINEAR);
	if ( hTileTexture == R_NONE )
		return false;

	mlog("Texture Alpha Channel : (%s)\n", ( hTileTexture->GetInfo().pixelFormat == RFMT_ARGB32 ? "true" : "false"));

	return AddLayer(hTileTexture, hNormalTexture, bFillAlpha);
}


int RToolTerrain::RenderPatchInfo(RFont* m_pFont)
{
	int nDPCallCount = 0;

	RMatrix viewMatrix(REngine::GetDevice().GetTransform(RST_VIEW));
	RMatrix matInverse;
	viewMatrix.GetInverse(&matInverse);
	matInverse._41 = 0.0f;
	matInverse._43 = 0.0f;
	matInverse._42 = 0.0f;

	RRenderHelper::SetRenderState();
	RRenderHelper::RenderBox((*m_matLocal), GetAABB(), D3DCOLOR_XRGB(0, 0, 255));

	int nTotalLayerNum = 0;
	for ( int si = 0; si<m_iPatchNum; ++si )
	{
		RTerrainPatch *pPatch = &m_pPatches[si];
		
		DWORD dwColor = 0xffffffff;	// 255, 255, 255, 255 색상

		// 필요한 정보를 써 넣자.
		int x = pPatch->GetX();
		int y = pPatch->GetY();
		int nIndex = x + y*GetWidthCount();

		wchar_t szLayerString[256];
		szLayerString[0] = '\0';

		int nLayerNum = 0;
		wchar_t *pBuffer = &szLayerString[0];
		for ( int wi = 0; wi<m_iLayerNum; ++wi )
		{
			LayerInfo *pInfo = &(m_pLayerInfo[wi]);
			if( IsAlphaTextureUsed( nIndex, wi))
			{
				++nLayerNum;

				if( GetMarkingLayerNumber() == wi)
				{
					*pBuffer = _T('>');
					++pBuffer;
					dwColor = 0xffff0000;
				}

				int nByte = wsprintfW(pBuffer,L"%2d,",wi+1);
				pBuffer += nByte;
			}
		}

		if (nLayerNum)
			pBuffer[-1] = _T('\0');

		// 바운딩 박스 경계를 그리도록 했다.
		RBoundingBox *pAABB = pPatch->GetAABBBoundBox();
		RBoundingSphere *pSphere = pPatch->GetBoundSphere();

		RRenderHelper::SetRenderState();
		RRenderHelper::RenderBox((*m_matLocal),*pAABB, dwColor);

		if (m_pFont)
		{
			float fScale = 25.0f;

			// 위치, 크기 계산
			RVector center = (pAABB->vmax + pAABB->vmin)*0.5f ;
			RMatrix matTranslate(RMatrix::IDENTITY), matScale(RMatrix::IDENTITY);

			matTranslate.SetTranslation(RVector(center.x, center.y, pAABB->vmax.z + 10.f));
			matScale._11 = fScale;
			matScale._22 = -fScale;
			matScale._33 = fScale;

			RMatrix mat = matScale * matTranslate;

			rs3::RFont::BeginFont(/*D3DXSPRITE_ALPHABLEND | D3DXSPRITE_OBJECTSPACE*/);
			REngine::GetDevice().SetTransform(RST_VIEW,viewMatrix);
			
			nTotalLayerNum += nLayerNum;

			// 레이어 갯수에 따른 메시지
			wchar_t szMsg[256];
			szMsg[0] = '\0';

			
			RVector3 ClrGood(1.0f, 1.f, 1.f);
			RVector3 ClrNutral(1.0f, 0.0f, 1.f);
			RVector3 ClrDanger(1.0f, 0.f, 0.f);
			DWORD dwFontColor = D3DCOLOR_XRGB(0, 0, 255);
			if ( nLayerNum > MAX_LAYER_PER_PATCH )		/// 패치당 최대 레이어 허용 수는 7장. 죽어랏~! 캬캬캬
			{
				BYTE byClr = 128 + (rand()%128);
				if (byClr >= 256 )
				{
					_ASSERT(false);
					byClr = 255;
				}
				dwFontColor = D3DCOLOR_XRGB(byClr, 0, 0);
				wsprintfW( szMsg,L"반드시 %d개 이하로 !!", MAX_LAYER_PER_PATCH);
			}
			else if ( nLayerNum > MAX_LAYER_BY_ONE_MASK )	/// 5장까지는 마스크 한장으로 커버되므로 so.. so..
			{
				float alpha = (float)(nLayerNum - MAX_LAYER_BY_ONE_MASK)/(float)(MAX_LAYER_PER_PATCH-MAX_LAYER_BY_ONE_MASK);
				RVector3 Color = MMath::LinearInterpolation(ClrNutral, ClrDanger, alpha);
				dwFontColor = D3DCOLOR_XRGB(int(Color.x*255), int(Color.y*255), int(Color.z)*255);
				wsprintfW( szMsg,L"되도록 %d개 이하로", MAX_LAYER_BY_ONE_MASK);
			}
			else
			{
				float alpha = (float)(nLayerNum-1)/(float)MAX_LAYER_BY_ONE_MASK;
				RVector3 Color = MMath::LinearInterpolation(ClrGood, ClrNutral, alpha);
				dwFontColor = D3DCOLOR_XRGB(int(Color.x*255), int(Color.y*255), int(Color.z)*255);
				wsprintfW( szMsg,L"적을수록 죠아요");
			}

			wchar_t szBuffer[256];
			szBuffer[0] = '\0';

			RMatrix matNextLine;
			matNextLine.SetTranslationMatrix(0,-400,0);

			wsprintfW(szBuffer,L"patch : %2d,%2d", x, y );
			m_pFont->DrawTextIn3DSpace(mat, szBuffer, -1, dwFontColor);
			mat = mat * matNextLine;

			wsprintfW(szBuffer,L"Layer Count: %d", nLayerNum);
			m_pFont->DrawTextIn3DSpace(mat, szBuffer, -1, dwFontColor);
			mat = mat * matNextLine;

			wsprintfW(szBuffer,L"(%s)", szMsg);
			m_pFont->DrawTextIn3DSpace(mat, szBuffer, -1, dwFontColor);
			mat = mat * matNextLine;

			wsprintfW(szBuffer,L"%s", szLayerString );
			m_pFont->DrawTextIn3DSpace(mat, szBuffer, -1, dwFontColor);
			mat = mat * matNextLine;

			wsprintfW(szBuffer,L"Lod Level:%d", pPatch->GetLevel());
			m_pFont->DrawTextIn3DSpace(mat, szBuffer, -1, dwFontColor);
			mat = mat * matNextLine;

			rs3::RFont::EndFont();
		}
		++nDPCallCount;
	}

	rs3::RFont::BeginFont(/*D3DXSPRITE_ALPHABLEND*/);
	TCHAR szBuffer[256];
	szBuffer[0] = 0;
	int nCount = m_iPatchNum;

	_stprintf(szBuffer,_T("Layer Average = %5.2f\n"), (float)m_nTotalAlphaTextureNum / (float)nCount);
	m_pFont->DrawText_UNICODE( 10, 10, MLocale::ConvAnsiToUTF16(szBuffer).c_str(), -1, D3DCOLOR_XRGB(242, 242, 242));
	rs3::RFont::EndFont();

	RRenderHelper::EndRenderState();

	return nDPCallCount;
}

void RToolTerrain::UpdateMaterialMap()
{
	RDevice *pDevice = REngine::GetDevicePtr();

	int nWidth = GetWidthMap();
	int nHeight = GetHeightMap();

	if ( m_hMaterialMap == R_NONE )
	{
		_ASSERT(m_pMaterialMap==NULL);

		int nSize = nWidth * nHeight;
		m_pMaterialMap = new DWORD[nSize];
		memset(m_pMaterialMap, 0, sizeof(DWORD)*nSize);

		m_hMaterialMap = pDevice->CreateTexture(nWidth, nHeight, RFMT_ARGB32, RTF_LINEAR, RUSAGE_DYNAMIC, m_pMaterialMap);
	}

	_ASSERT(m_pMaterialMap);
	DWORD *pMap = m_pMaterialMap;

	for ( int h = 0; h<nHeight; ++h )
		for ( int w = 0; w<nWidth; ++w )
		{
			DWORD dwMaterialID = m_pHeightField->GetMaterial(w, h);
			pMap[w + h*nWidth] = ( dwMaterialID == 0 ? D3DCOLOR_XRGB(128, 0, 128) : 0x00 );
		}

	pDevice->UpdateTexture(m_hMaterialMap, 0, 0, 0, nWidth, nHeight, (void*)pMap);

	return;
}

void RToolTerrain::OnUpdate()
{
	RTerrain::OnUpdate();
}


void RToolTerrain::RenderMStage( RCameraSceneNode* pCurrentCamera, RRENDERINGMETHOD renderMethod)
{
	RTerrain::RenderMStage( pCurrentCamera, renderMethod);
}


void RToolTerrain::ReloadTexture()
{
	assert(REngine::GetDevicePtr());

	char szTileFilename[MAX_PATH];
	char szNormalFilename[MAX_PATH];
	memset(szTileFilename, 0, sizeof(szTileFilename));
	memset(szNormalFilename, 0, sizeof(szNormalFilename));

	for ( int i = 0; i<m_iLayerNum; ++i )
	{
		strcpy(szTileFilename, m_pLayerInfo[i].tileMapName.c_str());
		strcpy(szNormalFilename, m_pLayerInfo[i].normalMapName.c_str());

		REngine::GetDevice().DeleteTexture(m_pLayerInfo[i].hTileMap);
		RTexture* hTexture = REngine::GetDevice().CreateTexture(szTileFilename, RTF_TRILINEAR);
       	m_pLayerInfo[i].hTileMap = hTexture;

		REngine::GetDevice().DeleteTexture(m_pLayerInfo[i].hNormalMap);
		if(szNormalFilename)
		{
			RTexture* hNormalMap = REngine::GetDevice().CreateTexture(szNormalFilename, RTF_TRILINEAR);
			m_pLayerInfo[i].hNormalMap = hNormalMap;
		}
	}
}

void RToolTerrain::AllChangedFlag(bool bFlag)
{
	m_bDisplacementMapChanged = bFlag;
	for ( int i = 0; i<m_iLayerNum; ++i )
		m_pAttendInfo[i].bMapChanged = true;

}

bool RToolTerrain::SaveBaseMapToFile()
{
	if ( m_hBaseMap == R_NONE )
		return false;

	const std::string &resourceName = GetName();
	std::string strFileName = m_strPathName + resourceName + "_base.dds";

	RDeviceD3D *pd3ddevice = (RDeviceD3D*)REngine::GetDevicePtr();
	LPDIRECT3DBASETEXTURE9 pTexture = pd3ddevice->GetD3DTexture(m_hBaseMap);
	ASSERT(pTexture!=NULL);

	HRESULT hr = D3DXSaveTextureToFile(strFileName.c_str(), D3DXIFF_DDS, pTexture, NULL);
	if ( FAILED(hr))
		return false;

	return true;
}

bool RToolTerrain::SaveGrassToFile()
{
	if ( m_pGrassSystem == NULL )
		return false;

	_ASSERT(!m_strGrassName.empty());
	return m_pGrassSystem->SaveToFile(m_strPathName + m_strGrassName);
}

void RToolTerrain::SaveDataFiles()
{
	bool bSaveDisplacement = SaveDisplacementmapToFile();
	_ASSERT(bSaveDisplacement);
	bool bSaveAlphamap = SaveAlphaMapToFile();
	_ASSERT(bSaveAlphamap);
	bool bGrassMap = SaveGrassToFile();
	//_ASSERT(bGrassMap);
}

void RToolTerrain::SaveToXML(MXmlElement &element)
{
	_ASSERT(m_strName.empty()==false);
	SaveDataFiles();
	RTerrain::SaveToXML(element);
}

bool RToolTerrain::ExportObjFile(int nLevel, int nWidthPartition , int nHeightPartition)
{
	_ASSERT(nLevel>0);
	_ASSERT(nWidthPartition>0);
	_ASSERT(nHeightPartition>0);

	DWORD dwWidth  = GetWidthMap();
	DWORD dwHeight = GetHeightMap();

	DWORD dwWidthUnit = dwWidth/nWidthPartition;
	DWORD dwHeightUnit = dwHeight/nHeightPartition;

	float *pHeightMap = GetHeightMapPtr();
	float fScaleX = GetXScale();
	float fScaleY = GetYScale();

	char strFilename[MAX_PATH];

	for ( DWORD hu = 0, hc = 0; hu<dwHeight; hu += dwHeightUnit, ++hc)
	{
		for ( DWORD wu = 0, wc = 0; wu<dwWidth; wu += dwWidthUnit, ++wc)
		{
			UINT wuBegin = wu;
			UINT wuEnd = wu + dwWidthUnit;
			UINT huBegin = hu;
			UINT huEnd = hu + dwHeightUnit;

			int nVertexCount = 0;
			int nFaceCount = 0;

			const std::string &resourceName = GetName();
			strFilename[0] = '\0';
			sprintf(strFilename,"%s_export_%d_%d_%d.obj", resourceName.c_str(), nLevel, wc, hc);

			FILE *fp = fopen(strFilename,"wt");

			// vertex write
			fprintf(fp,"# vertex begin { \n");
			for ( UINT h = huBegin; h<=huEnd; h+= nLevel )
			{
				for ( UINT w = wuBegin; w<=wuEnd; w+= nLevel )
				{
					UINT uIndex = w + h*GetHeightMapPitch();
					fprintf(fp,"v %f %f %f\n", w*fScaleX, h*fScaleY, pHeightMap[uIndex]);
					++nVertexCount;
				}
			}
			fprintf(fp,"# vertex end : %d} \n", nVertexCount);

			// index write ( ccw quad)
			fprintf(fp,"# face begin { \n");
			for ( UINT h = 0; h<dwHeightUnit/nLevel; ++h )
			{
				for ( UINT w = 0; w<dwWidthUnit/nLevel; ++w )
				{
					int nPitch = dwWidthUnit/nLevel + 1;
					int nIndexA = w + h*nPitch + 1;
					int nIndexB = nIndexA + 1;
					int nIndexC = w + (h+1)*nPitch + 1;
					int nIndexD = nIndexC + 1;
					fprintf(fp,"f %d %d %d %d\n", nIndexA, nIndexB, nIndexD, nIndexC);
					++nFaceCount;

					_ASSERT(nIndexA<=nVertexCount);
					_ASSERT(nIndexB<=nVertexCount);
					_ASSERT(nIndexC<=nVertexCount);
					_ASSERT(nIndexD<=nVertexCount);
				}
			}
			fprintf(fp,"# face end : %d{ \n", nFaceCount);
			fclose(fp);
		}
	}

	return true;
}

bool RToolTerrain::ImportObjFile(const char *filename, std::string *pErrMessage)
{
	FILE *fp = fopen(filename, "rt");
	if ( fp == NULL )
	{
		if ( pErrMessage )	*pErrMessage = "파일을 읽지 못했습니다.";
		return false;
	}

	char szBuffer[256];
	memset(szBuffer, 0, sizeof(szBuffer));

	std::vector<RVector> vecVertex;
	std::vector<int>	 vecFaceIndex;

	RBoundingBox kAABB;
	// 원점은 항상 0, 0, 0이므로 - 이렇게 초기화 했다.
	kAABB.vmin = RVector(0, 0, 0);
	kAABB.vmax = RVector(0, 0, 0);
    
	// .obj 읽기. 정점과 Face Index만 읽는다.
	while ( !feof(fp))
	{
		fgets(szBuffer, 255, fp);
		if ( szBuffer[0] == 'v' )
		{
			char szType;
			RVector vec;

			sscanf(szBuffer,"%c %f %f %f",&szType,&vec.x,&vec.y,&vec.z);

			kAABB.Add(vec);
			vecVertex.push_back(vec);
		}
		else if ( szBuffer[0] =='f' )
		{
			char szIndex[4][64];
			char szType;

			memset(szIndex, 0, sizeof(szIndex));
			sscanf(szBuffer,"%c %s %s %s %s",&szType, szIndex[0], szIndex[1], szIndex[2], szIndex[3]);

            for ( int i = 0; i<4; ++i )
			{
				if ( szIndex[i][0] == '\0' )
					continue;

				int nIndex = atoi(szIndex[i]);
				if ( nIndex == 0 )
					continue;

				vecFaceIndex.push_back(nIndex-1);
			}
		}
	}
	
	// .obj 파일은 quad이어야 한다.
	if ( vecFaceIndex.size()%4 != 0 )
	{
		if ( pErrMessage )	*pErrMessage = ".obj 파일이 4의 배수가 아닙니다.";
		return false;
	}

	_ASSERT(m_pHeightField);
	float *pHeightMap = m_pHeightField->GetPtr();
	_ASSERT(pHeightMap);
	
	// 지형의 넓이가 같아야 한다.
	if ( kAABB.vmin.x < -1.f 
		|| kAABB.vmin.y < -1.f 
		|| abs( GetWidthSize() - kAABB.vmax.x ) > 1.f
		|| abs( GetHeightSize() - kAABB.vmax.y ) > 1.f )
	{
		char szBuffer[128];
		szBuffer[0] = '\0';
		sprintf(szBuffer,"지형과 크기가 다릅니다. .obj 크기 %7.2f,%7.2f", kAABB.vmax.x, kAABB.vmax.y);

		if ( pErrMessage )	*pErrMessage = szBuffer;
		return false;
	}

	for ( int i = 0; i<(GetWidthMap()+1)*(GetHeightMap()+1); ++i )
		pHeightMap[i] = kAABB.vmin.z;

	for ( std::vector<int>::iterator it = vecFaceIndex.begin(); it != vecFaceIndex.end(); )
	{
		RBoundingBox kBox;
		RVector vecA[4], vecB[4];

		vecA[0] = vecVertex[(*it)]; ++it;
		vecA[1] = vecVertex[(*it)]; ++it;
		vecA[2] = vecVertex[(*it)]; ++it;
		vecA[3] = vecVertex[(*it)]; ++it;

		kBox.vmin = kBox.vmax = vecA[0];
		kBox.Add(vecA[1]);
		kBox.Add(vecA[2]);
		kBox.Add(vecA[3]);

		RVector vCenter = kBox.vmin + ( kBox.vmax - kBox.vmin )*0.5f;
		for ( int i = 0; i<4; ++i )
		{
			if ( vCenter.x > vecA[i].x && vCenter.y > vecA[i].y )
				vecB[0] = vecA[i];
			else if ( vCenter.x < vecA[i].x && vCenter.y > vecA[i].y )
				vecB[1] = vecA[i];
			else if ( vCenter.x > vecA[i].x && vCenter.y < vecA[i].y )
				vecB[2] = vecA[i];
			else if ( vCenter.x < vecA[i].x && vCenter.y < vecA[i].y )
				vecB[3] = vecA[i];
		}

		int nX1 = (int)(kBox.vmin.x / GetXScale() + 0.4f);
		int nX2 = (int)(kBox.vmax.x / GetXScale() + 0.4f);
		int nY1 = (int)(kBox.vmin.y / GetYScale() + 0.4f);
		int nY2 = (int)(kBox.vmax.y / GetYScale() + 0.4f);

		float dx = ( nX2 - nX1 );
		float dy = ( nY2 - nY1 );

		for ( int y = nY1; y<=nY2; ++y )
		{
			for ( int x = nX1; x<=nX2; ++x )
			{
				float sx = ( dx <= 0.0f ? 0.0f : float(x - nX1)/dx );
				float sy = ( dy <= 0.0f ? 0.0f : float(y - nY1)/dy );

				float fHeight = CMFCHelper::BiLinearInterpolation( sx, sy
					, vecB[0].z, vecB[1].z, vecB[2].z, vecB[3].z );
				
				float *pHeight = &pHeightMap[ x + y*GetHeightMapPitch() ];
				*pHeight = max(*pHeight, fHeight);
			}
		}
	}

	for ( int h = 0; h<m_iCount; ++h )
		for ( int w = 0; w<m_iCount; ++w )
            UpdateHeightField(w, h);

	m_pHeightField->UpdateAABB();
	return true;
}

void RToolTerrain::CreateGuideVertex()
{
	int nSize = ((GetWidthMap()+1) + (GetHeightMap()*2) + GetWidthMap())*2;
	int nCount = 0;
	
	if ( m_hGuideVertex != R_NONE )
	{
		REngine::GetDevice().DeleteVertexBuffer(m_hGuideVertex);
		m_hGuideVertex = R_NONE;
	}

	_ASSERT(m_pHeightField);
	float *pHeightMap = m_pHeightField->GetPtr();

	RVt_pos_tex1 *pVertexArray = new RVt_pos_tex1[nSize];
	// bottom
	for ( int i = 0; i<=GetWidthMap(); ++i )
	{
		float fz = pHeightMap[i];

		pVertexArray[nCount].u = 0;
		pVertexArray[nCount].v = 0;
		pVertexArray[nCount++].vPos = RVector( i*GetXScale(), 0, fz);

		pVertexArray[nCount].u = 0;
		pVertexArray[nCount].v = 0;
		pVertexArray[nCount++].vPos = RVector( i*GetXScale(), 0, m_pHeightField->GetMinZ());
	}
	// right
	for ( int i = 1; i<=GetHeightMap(); ++i )
	{
		float fz = pHeightMap[GetWidthMap() + i*GetHeightMapPitch()];

		pVertexArray[nCount].u = 0;
		pVertexArray[nCount].v = 0;
		pVertexArray[nCount++].vPos = RVector( GetWidthSize(), i*GetYScale(), fz);

		pVertexArray[nCount].u = 0;
		pVertexArray[nCount].v = 0;
		pVertexArray[nCount++].vPos = RVector( GetWidthSize(), i*GetYScale(), m_pHeightField->GetMinZ());
	}
	// top
	for ( int i = GetWidthMap() - 1; i>=0 ; --i )
	{
		float fz = pHeightMap[i + GetHeightMap()*GetHeightMapPitch()];

		pVertexArray[nCount].u = 0;
		pVertexArray[nCount].v = 0;
		pVertexArray[nCount++].vPos = RVector( i*GetXScale(), GetHeightSize(), fz);

		pVertexArray[nCount].u = 0;
		pVertexArray[nCount].v = 0;
		pVertexArray[nCount++].vPos = RVector( i*GetXScale(), GetHeightSize(), m_pHeightField->GetMinZ());
	}
	// left
	for ( int i = GetHeightMap() -1 ; i>=0 ; --i )
	{
		float fz = pHeightMap[0 + i*GetHeightMapPitch()];

		pVertexArray[nCount].u = 0;
		pVertexArray[nCount].v = 0;
		pVertexArray[nCount++].vPos = RVector( 0, i*GetYScale(), fz);

		pVertexArray[nCount].u = 0;
		pVertexArray[nCount].v = 0;
		pVertexArray[nCount++].vPos = RVector( 0, i*GetYScale(), m_pHeightField->GetMinZ());
	}

	assert(m_hGuideVertex==R_NONE);
	m_hGuideVertex = REngine::GetDevice().CreateVertexBuffer( sizeof(RVt_pos_tex1)*nCount, sizeof(RVt_pos_tex1), pVertexArray, 0);

	SAFE_DELETE_ARRAY(pVertexArray);
}

int RToolTerrain::RenderGuideMap()
{
	REngine::GetDevice().SetFvF( RVt_pos_normal_tex1::FVF );
	REngine::GetDevice().SetVertexBuffer( m_hGuideVertex, 0, 0);
	REngine::GetDevice().DrawPrimitive( RPT_TRIANGLESTRIP, 0, GetWidthMap()*4 + GetHeightMap()*4);

	return 0;
}

void RToolTerrain::ApplyCircleActionWithFuncPtr(
	const rs3::RVector* pVecCenterPos, float fSize, float fStrength, float fFactor,
	float (*funcPtrAction)(float, float, float, float), bool bTerrainVertexUpdate)
{
	// RECT 를 구한다.
	int nX = (int)(pVecCenterPos->x / GetXScale() + 0.5f);
	int nY = (int)(pVecCenterPos->y / GetYScale() + 0.5f);

	RECT rt;
	int sizepp = int(fSize+0.5f) + int(fSize/2.0f);
	if ( fSize < 2.5f )
		sizepp += 5;

	rt.left = nX - sizepp;
	rt.right = nX + sizepp;
	rt.top = nY - sizepp;
	rt.bottom = nY + sizepp;

	float lx = fSize * GetXScale();
	float ly = fSize * GetYScale();
	float minlen = min(lx,ly);

	// RECT 값을 검사하고,
	if ( rt.left < 0 )	rt.left = 0;
	if ( rt.right > GetWidthMap() )	rt.right = GetWidthMap();
	if ( rt.top < 0 )	rt.top = 0;
	if ( rt.bottom > GetHeightMap() )	rt.bottom = GetHeightMap();

	// RECT 영역을 순환하여, 원 안에 있는지를 검사한 다음 원 안의 부분은 funcPtrAction 메서드를 이용하여 조작한다.
	float *pHeightMap = m_pHeightField->GetPtr();

	for ( int y = rt.top; y<=rt.bottom; ++y )
	{
		for ( int x = rt.left; x<=rt.right; ++x )
		{
			int nIndex = x + y * GetHeightMapPitch();
			float fx = x*GetXScale();
			float fy = y*GetYScale();

			float dx = fx - pVecCenterPos->x;
			float dy = fy - pVecCenterPos->y;

			float f = sqrtf(dx*dx+dy*dy) / minlen;
			float HALF_PI = MMath::PI/2.0f;
			f = f*HALF_PI;
			if ( f < -HALF_PI )
				f = -HALF_PI;
			else if ( f > HALF_PI )
				f = HALF_PI;

			f = cosf(f);

			if ( f < 0.0f )
				f = 0.0f;
			if ( f > 1.0f )		
				f = 1.0f;

			pHeightMap[nIndex] = funcPtrAction(pHeightMap[nIndex], f, fStrength, fFactor);
		}
	}

	if ( bTerrainVertexUpdate )
		ApplyTerrainVertex_(&rt);
}

void RToolTerrain::ApplyTerrainVertex_(RECT *pRect)
{
	_ASSERT(m_pHeightField);

	int h1, h2;
	int w1, w2;
	
	if(pRect == NULL) //전체영역
	{
		h1 = 0;
		h2 = GetHeightCount();

		w1 = 0;
		w2 = GetWidthCount();
	}
	else
	{
		int widthMap = GetWidthMap();
		int heightMap = GetHeightMap();
		int wUnit =  widthMap / GetWidthCount();
		int hUnit =  heightMap / GetHeightCount();

		// Safety
		if( pRect->right >= widthMap)
			pRect->right = widthMap-1;
		if( pRect->bottom >= heightMap)
			pRect->bottom = heightMap-1;

		// m_fHeigtMap의 내용을 버텍스 버퍼에 반영한다.
		h1 = pRect->top/hUnit;
		h2 = pRect->bottom/hUnit + 1;
		w1 = pRect->left/wUnit;
		w2 = pRect->right/wUnit + 1;
	}
	
	for ( int h = h1; h < h2; ++h )
		for ( int w = w1; w < w2; ++w )
			UpdateHeightField(w, h);

	m_pHeightField->UpdateAABB();
}

void RToolTerrain::ApplyRelaxAction(float _fStrength,
	float _fSizeFactor, RECT* pRect)
{
	int nTop, nBottom;
	int nLeft, nRight;
	if(pRect == NULL)
	{
		nLeft = 0;
		nTop = 0;
		nRight = GetWidthMap();
		nBottom = GetHeightMap();
	}
	else
	{
		// RECT 값을 검사하고,
		if ( pRect->left < 0 )	pRect->left = 0;
		if ( pRect->right > GetWidthMap() )	pRect->right = GetWidthMap();
		if ( pRect->top < 0 )	pRect->top = 0;
		if ( pRect->bottom > GetHeightMap())	pRect->bottom = GetHeightMap();

		nTop = pRect->top + _fSizeFactor / 3 + 1;
		nBottom = pRect->bottom - _fSizeFactor / 3 - 1;
		nLeft = pRect->left + _fSizeFactor / 3 + 1;
		nRight = pRect->right - _fSizeFactor / 3 - 1;
	}

	float *pHeightMap = m_pHeightField->GetPtr();

	//////////// RECT 영역의 순환
	// 가로로 한번, 세로로 한번 이렇게 1번씩 총2번을 교차로 순환한다.
	//
	// RECT 영역 1차 순환
	for ( int y = nTop; y<= nBottom; ++y )
	{
		for ( int x = nLeft; x<= nRight; ++x )
		{
			//3개를 비교해서 중간값으로, 맨 마지막 값이 범위 밖에면 다음순환을 한다.
			if(x+2 > nRight)
				break;

			int offset = y * GetHeightMapPitch();
			int index_start = x + offset;
			int index_middle = x + 1 + offset;
			int index_end = x + 2 + offset;

			float goal_value = (pHeightMap[index_start] + pHeightMap[index_end])/2;
			float gap_of_goal_value = pHeightMap[index_middle] - goal_value;
			if(fabs(gap_of_goal_value) > _fStrength)
			{
				if(gap_of_goal_value > 0 )
					pHeightMap[index_middle] -= _fStrength;
				else
					pHeightMap[index_middle] += _fStrength;
			}
			else
				pHeightMap[index_middle] = goal_value;
		}
	}

	//2차 순환
	//마지막 바로 전줄이 마지막줄과 마지막 전전줄과의 중간값을 가지므로
	//마지막 바로 전전줄까지만 순환하면 된다.
	for ( int y = nTop; y<= nBottom - 2; ++y )
	{
		for ( int x = nLeft; x<= nRight; ++x )
		{
			//offset 재조정, 1차 순환과 offset 이 다르다.
			int offset_start = y * GetHeightMapPitch();
			int offset_middle = (y+1) * GetHeightMapPitch();
			int offset_end = (y+2) * GetHeightMapPitch();

			int index_start = x + offset_start;
			int index_middle = x + offset_middle;
			int index_end = x + offset_end;

			float goal_value = (pHeightMap[index_start] + pHeightMap[index_end])/2;
			float gap_of_goal_value = pHeightMap[index_middle] - goal_value;
			if(fabs(gap_of_goal_value) > _fStrength)
			{
				if(gap_of_goal_value > 0 )
					pHeightMap[index_middle] -= _fStrength;
				else
					pHeightMap[index_middle] += _fStrength;
			}
			else
				pHeightMap[index_middle] = goal_value;
		}
	}
	
	// RECT 영역 2차 순환
	//최종 업데이트
	ApplyTerrainVertex_(pRect);
}

#if 0 
void RToolTerrain::ApplyUserMaterial(const rs3::RVector* pVecCenterPos, float fSize, DWORD dwID)
{
	// RECT 를 구한다.
	int nX = (int)(pVecCenterPos->x / GetXScale() + 0.5f);
	int nY = (int)(pVecCenterPos->y / GetYScale() + 0.5f);

	RECT rt;
	int sizepp = int(fSize+0.5f) + int(fSize/2.0f);
	if ( fSize < 2.5f )
		sizepp += 5;

	rt.left = nX - sizepp;
	rt.right = nX + sizepp;
	rt.top = nY - sizepp;
	rt.bottom = nY + sizepp;

	float lx = fSize * GetXScale();
	float ly = fSize * GetYScale();
	float minlen = min(lx,ly);

	// RECT 값을 검사하고,
	if ( rt.left < 0 )	rt.left = 0;
	if ( rt.right > GetWidthMap() )	rt.right = GetWidthMap();
	if ( rt.top < 0 )	rt.top = 0;
	if ( rt.bottom > GetHeightMap() )	rt.bottom = GetHeightMap();

	// RECT 영역을 순환하여, 원 안에 있는지를 검사한 다음 원 안의 부분은 funcPtrAction 메서드를 이용하여 조작한다.
	float *pHeightMap = m_pHeightField->GetPtr();

	for ( int y = rt.top; y<=rt.bottom; ++y )
	{
		for ( int x = rt.left; x<=rt.right; ++x )
		{
			int nIndex = x + y * GetHeightMapPitch();
			float fx = x*GetXScale();
			float fy = y*GetYScale();

			float dx = fx - pVecCenterPos->x;
			float dy = fy - pVecCenterPos->y;

			float f = sqrtf(dx*dx+dy*dy) / minlen;
			if ( f > -1.0f && f < 1.0f )
			{
				// 지형 UserData 영역에 저장
				m_pHeightField->SetMaterial(x, y, dwID);
			}
		}
	}
}
#endif

void RToolTerrain::UpdateAlphaTexture(int nIndex)
{
	_ASSERT(nIndex>=0);
	_ASSERT(nIndex<m_iLayerNum);

	/// 텍스쳐들을 비워주거나, 정보 초기화 필요 플래그 등을 둬야 함.
	NeedUpdateAlphaTexture( nIndex);

	RTerrain::UpdateAlphaTextureOfLayer(nIndex);
	m_pAttendInfo[nIndex].bMapChanged = true;

	ToDoWhenUpdateAlphaTextureRaw(nIndex);

	CountAlphaTexture();
}

bool RToolTerrain::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	// ToolTerrain은 오직 Foreground loading
	_ASSERT(bBackgroundCreation==false);

	bool bResult = RTerrain::CreateFromXML(element, bBackgroundCreation);
	if ( bResult )
	{
		/// 툴 터레인에서는 베이크드맵을 사용하지는 않는다. 다만 만들어낼 뿐이다. 고로 베이크드맵 이름을 강제 지정 한다.
		SetBaseMapName();
		CountAlphaTexture();
		SetRenderDetailMap(ONLY_SPLATTING);
	}

	return bResult;
}

void RToolTerrain::CountAlphaTexture()
{
	m_nTotalAlphaTextureNum = 0;
	for (int n = 0; n<m_iLayerNum; ++n)
		for ( int i = 0; i<m_iPatchNum; ++i )
			if ( m_pLayerInfo[n].phAlphaMask[i] != R_NONE )
				++m_nTotalAlphaTextureNum;
}

int RToolTerrain::ActuallyAlphaMask(float left, float top, float right, float bottom)
{
	_ASSERT(m_pLayerInfo);
	_ASSERT(m_pAttendInfo);

	if ( left < 0.0f )	left = 0.0f;
	if ( top < 0.0 )	top = 0.0f;
	if ( right > 1.0f )	right = 1.0f;
	if ( bottom > 1.0f) bottom = 1.0f;
	
	float fFactor = 1.0f;
	for ( int nIndex = GetLayerNum()-1; nIndex >= 0; --nIndex )
	{
		int nAlphaWidth = m_pLayerInfo[nIndex].dwAlphaWidth;
		int nAlphaHeight = m_pLayerInfo[nIndex].dwAlphaHeight;

		int nLeft = floor(left*nAlphaWidth);
		int nRight = ceil(right*nAlphaWidth);
		int nTop = floor(top*nAlphaHeight);
		int nBottom = ceil(bottom*nAlphaHeight);

		_ASSERT(nLeft>=0);
		_ASSERT(nTop>=0);
		_ASSERT(nRight<=nAlphaWidth);
		_ASSERT(nBottom<=nAlphaHeight);

		float fValue, fCount, fAvrValue;

		fValue = 0.0f;
		fCount = 0.0f;

		for (int y = nTop; y<nBottom; ++y)
			for (int x = nLeft; x<nRight; ++x)
			{
				fValue += (float)m_pLayerInfo[nIndex].pAlphaTextureRaw[(x + y*nAlphaWidth)*4 + 3]/255.f;
				fCount += 1.0f;
			}	// next x
		// next y

		fAvrValue = ( fValue / fCount ) * fFactor;
		if ( fAvrValue > 0.5f )
			return nIndex;

		fFactor = 1.0f - fAvrValue;
	}	// next nIndex

	return 0;
}

bool RToolTerrain::IsAlphaPixel(int nIndex, float left, float top, float right, float bottom, long ref)
{
	_ASSERT(m_pLayerInfo);
	_ASSERT(m_pAttendInfo);

	if ( left < 0.0f )	left = 0.0f;
	if ( top < 0.0 )	top = 0.0f;
	if ( right > 1.0f )	right = 1.0f;
	if ( bottom > 1.0f) bottom = 1.0f;

	int nAlphaWidth = m_pLayerInfo[nIndex].dwAlphaWidth;
	int nAlphaHeight = m_pLayerInfo[nIndex].dwAlphaHeight;
	
	int nLeft = floor(left*nAlphaWidth);
	int nRight = ceil(right*nAlphaWidth);
	int nTop = floor(top*nAlphaHeight);
	int nBottom = ceil(bottom*nAlphaHeight);

	_ASSERT(nLeft>=0);
	_ASSERT(nTop>=0);
	_ASSERT(nRight<=nAlphaWidth);
	_ASSERT(nBottom<=nAlphaHeight);

	for (int y = nTop; y<nBottom; ++y)
		for (int x = nLeft; x<nRight; ++x)
		{
			if ( m_pLayerInfo[nIndex].pAlphaTextureRaw[(x + y*nAlphaWidth)*4 + 3] < ref )
				return false;
		}

	return true;
}

int RToolTerrain::RebuildAlphaMask()
{
	if (m_iLayerNum < 2)
		return 0;

	int nErasePixelCount = 0;

	// 3중 for 문이 필요할듯 ( 순환해서 검사하는 방식이라, 뭔가 더 좋은 방법 있으면 고쳐주세요 )
	for (int index = m_iLayerNum-2; index>=0; --index)
	{
		int nAlphaWidth = m_pLayerInfo[index].dwAlphaWidth;
		int nAlphaHeight = m_pLayerInfo[index].dwAlphaHeight;

		float fOffsetSrcX = 1.0f/(float)nAlphaWidth;
		float fOffsetSrcY = 1.0f/(float)nAlphaHeight;

		for ( int y = 0; y<nAlphaHeight; ++y )
			for ( int x = 0; x<nAlphaWidth; ++x )
			{
				bool bFlag = false;
				for ( int n = index+1; n<m_iLayerNum; ++n )
				{
					if (m_pLayerInfo[n].hTileMap->GetInfo().pixelFormat == RFMT_ARGB32 
						&& m_pLayerInfo[n].bAlphaMaskForSpec == false )
						continue;

					float fOffsetDstX = 1.0f/(float)m_pLayerInfo[n].dwAlphaWidth;
					float fOffsetDstY = 1.0f/(float)m_pLayerInfo[n].dwAlphaHeight;
					
					// 현재 Pixel을 u,v 로 환산하고 ( 0 ~ 1 사이의 )
					float left = (x*fOffsetSrcX);
					float top = (y*fOffsetSrcY);
					float right = left + fOffsetSrcX;
					float bottom = top + fOffsetSrcY;

					float addX = (fOffsetSrcX + fOffsetDstX);
					float addY = (fOffsetSrcY + fOffsetDstY);
					
					// 삭제되는 픽셀로 인해, 옆 픽셀이 블렌딩 되어 변화되므로 옆 픽셀 역시 완전히 가려져야 하고 ...
					// 윗 픽셀 역시 인접되는 픽셀에 의해 블렌딩 되지 않아 완전히 가려져야 하므로,
					// 아랫픽셀 영역 + 윗픽셀 영역 을 더한 만큼 좌우 크기를 + 해주는 것이라고 생각됩니다.
					left -= addX;
					right += addX;
					top -= addY;
					bottom += addY;
					
					// IsAlphaPixel에서는 left, top 은 버리고, right, bottom은 올림으로서 사각영역에 걸치는 픽셀을 모두 검사합니다.
					// 전 상위 레이어에 걸쳐 true(완전히 가려진다면), 삭제해도 괜찮은 것이겠죠.
					bFlag = bFlag || IsAlphaPixel(n, left, top, right, bottom);
				}

				BYTE *pAlphaPixel = &m_pLayerInfo[index].pAlphaTextureRaw[(x+y*nAlphaWidth)*4 + 3];
				if (bFlag)
				{
					// 알파 픽셀 삭제
					*pAlphaPixel = 0x00;
					++nErasePixelCount;
				}
			}
		// end for statement
		UpdateAlphaTexture(index);
	}

	return nErasePixelCount;
}

bool RToolTerrain::GetPickLine(RVector &rOut, RVector *pNormal, const RVector &p, const RVector &q)
{
	// Custom ...
	if(!GetUsable()) return false;
	if(m_pHeightField == NULL ) return false;

	if ( _isnan(p.x) || _isnan(p.y) || _isnan(p.z))
		return false;

	if ( _isnan(q.x) || _isnan(q.y) || _isnan(q.z))
		return false;

	bool bResult = m_pHeightField->GetPickLine(&rOut, pNormal, NULL, p, q, RHeightField::NONE);
	return bResult;
}

void RToolTerrain::UpdateGrassOnTerrain()
{
	class Impl : public RGrassSystem::ICallback
	{
	private:
		RToolTerrain *m_pTerrain;
	public:
		Impl(RToolTerrain *pTerrain)
			: m_pTerrain(pTerrain)
		{
		}
		virtual void OnBlade(RGrassBlade &b)
		{
			RVector vPos(b.m_afPos[0], b.m_afPos[1], b.m_afPos[2]);
			RVector vOut;
			
			if ( m_pTerrain->GetPickVertical(vOut, NULL, vPos.x, vPos.y))
			{
				b.m_afPos[0] = vOut.x;
				b.m_afPos[1] = vOut.y;
				b.m_afPos[2] = vOut.z;

				RVector4 outColor;
				//m_pTerrain->GetPickLightMapColor(outColor, vOut);
				outColor.x = outColor.y = outColor.z = outColor.a = 1.0f;

				// b, g, r
				DWORD dwColor = D3DCOLOR_COLORVALUE(outColor.x, outColor.y, outColor.z, outColor.a );
				b.m_dwTopColor = dwColor;
				b.m_dwBottomColor = dwColor;
			}
		}
	};

	Impl impl(this);

	if ( m_pGrassSystem )
		m_pGrassSystem->UpdateOnCallback(&impl);

}

}
