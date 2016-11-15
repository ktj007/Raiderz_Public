#include "stdafx.h"
#include "RGrassSystem.h"
#include "RShaderFX.h"
#include "RSceneManager.h"
#include "RDevice.h"
#include "RDeviceD3D.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "RHeightField.h"
#include "RSpeedGrassGPUInstance.h"

#include "RTreeResourceMgr.h"
#include "RTreePassRenderer.h"

#include "RProfileInfo.h"

#include "REnvironmentSet.h"
#include "RGrassLoader.h"

#include "RShadowStrategy.h"

#include "RRule.h"

namespace rs3
{

RGrassSystem::RGrassSystem() :
		m_pDevice(NULL),
		m_pGrass(NULL),

		m_hGrassCompositeTexture(R_NONE),
		m_fTime(0.0f),

		m_bRendering(true),
		m_nRenderGrassCount(0)
{
	m_bNeedUpdate = true;
}


RGrassSystem::~RGrassSystem()
{
}

bool RGrassSystem::Create(RDevice* dev, RSceneManager* pSceneManager)
{
	m_pDevice = dev;
	m_pSceneManager = pSceneManager;

	m_pGrass = new RSpeedGrassGPUInstance(dev);
	return m_pGrass->Create();
}

void RGrassSystem::Destroy()
{
	if ( m_pGrass )
	{
		m_pGrass->Destroy();
		delete m_pGrass;
		m_pGrass = NULL;
	}

	if ( m_hGrassCompositeTexture )
	{
		m_pDevice->DeleteTexture(m_hGrassCompositeTexture);
		m_hGrassCompositeTexture = NULL;
	}
}

void RGrassSystem::CreateRegions(int row, int col, float fWidth, float fHeight)
{
	if (m_pGrass == NULL) _ASSERT(0);
	m_pGrass->CreateRegions(row, col, fWidth, fHeight);
}

bool RGrassSystem::AddBlade(const RGrassBlade& b)	// 결과를 리턴하게 변경 - 090610, OZ
{
	if (m_pGrass == NULL) _ASSERT(0);
	bool bSuccess = m_pGrass->AddGrass(b);
	m_bNeedUpdate = true;

	return bSuccess;
}

void RGrassSystem::DeleteBlade(float x, float y, float fRadius)
{
	if (m_pGrass == NULL) _ASSERT(0);
	m_pGrass->DeleteGrass(x, y, fRadius);
	m_bNeedUpdate = true;
}

void RGrassSystem::GetBladesArray(vector<RGrassBlade> &outBlades)
{
	outBlades.clear();
	UINT uNum = 0;
	RSpeedGrass::SRegion *pRegions = m_pGrass->GetRegions(uNum);

	for ( UINT ui = 0; ui<uNum; ++ui )
		for ( UINT un = 0; un<pRegions[ui].m_vBlades.size(); ++un )
			outBlades.push_back( pRegions[ui].m_vBlades[un]);
}

void RGrassSystem::DeleteAllBlade()
{
	UINT uNum = 0;
	RSpeedGrass::SRegion *pRegions = m_pGrass->GetRegions(uNum);

	for ( UINT ui = 0; ui<uNum; ++ui )
		for ( UINT un = 0; un<pRegions[ui].m_vBlades.size(); ++un )
			pRegions[ui].m_vBlades.clear();

	m_bNeedUpdate = true;
}

void RGrassSystem::UpdateDXBuffers( RTerrain* pTerrain)
{
	if (m_pGrass == NULL) _ASSERT(0);
	if ( m_bNeedUpdate )
		m_pGrass->UpdateDXBuffers( pTerrain);

	m_bNeedUpdate = false;
}

void RGrassSystem::LoadTexture(const char* filename)
{
	if (m_pGrass == NULL) _ASSERT(0);

	if (m_hGrassCompositeTexture != R_NONE)
		m_pDevice->DeleteTexture(m_hGrassCompositeTexture);

	m_strTextureFilename.clear();

	std::string strFullname = m_strGrassPath + filename;
	m_hGrassCompositeTexture = m_pDevice->CreateTexture( strFullname.c_str(), RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE );
	//mlog("%s grass composite-texture load\n", strFullname.c_str());
	if (m_hGrassCompositeTexture == R_NONE) 
	{
		// default 값을 넣어준다. (이 값이 풀 크기 계산에 쓰이므로 적절한 값을 넣어주지 않으면, 제대로 나오지 않는다. )
		m_pGrass->m_nCompTexHeight = 1024;
		m_pGrass->m_nCompTexWidth = 1024;

		mlog("grass texture load failed...\n");
		return;
	}

	const RTextureInfo tInfo = *(m_pDevice->GetTextureInfo(m_hGrassCompositeTexture));
	m_pGrass->m_nCompTexHeight = tInfo.nTextureHeight;
	m_pGrass->m_nCompTexWidth = tInfo.nTextureWidth;

	m_strTextureFilename = filename;
}


void RGrassSystem::Render(RSceneManager* pManager, RCameraSceneNode* pCamera)
{
	if (!m_bRendering || CSpeedGrassRT::m_fLodFarDistance <= 0) return;

	RCameraSceneNode *cam = pCamera;
	if (cam == NULL) return;

	RVector m_CamPos = cam->GetPosition();
	RMatrix matViewProj = cam->GetViewMatrix();

	CSpeedGrassRT::SetPerspective(cam->GetAspectRatio(), cam->GetFovY());

	m_pGrass->SetCamera((const float*)&m_CamPos, (const float*)&matViewProj);
	m_pGrass->Cull( );

	m_pGrass->SetBasicRenderState(this, pCamera);

	m_nRenderGrassCount = m_pGrass->Render( NULL, NULL);
	m_pGrass->ResetRenderState();

	REngine::GetResourceProfiler().AddPolygon(PPT_GRASS,m_nRenderGrassCount);
}


void RGrassSystem::RenderMStage( RCameraSceneNode* pCamera)
{
	if (!m_bRendering || CSpeedGrassRT::m_fLodFarDistance <= 0) return;

	RCameraSceneNode *cam = pCamera;
	if (cam == NULL) return;

	RVector m_CamPos = cam->GetPosition();
	RMatrix matViewProj = cam->GetViewMatrix();

	CSpeedGrassRT::SetPerspective(cam->GetAspectRatio(), cam->GetFovY());

	m_pGrass->SetCamera((const float*)&m_CamPos, (const float*)&matViewProj);

	// G-Stage에서 컬링을 했으므로 이곳에서는 하지 않고 바로 랜더 한다 (m_pGrass->Cull();)
	m_pGrass->SetBasicRenderState(this, pCamera);

	m_nRenderGrassCount = m_pGrass->RenderMStage();
	m_pGrass->ResetRenderState();
	REngine::GetResourceProfiler().AddPolygon(PPT_GRASS,m_nRenderGrassCount);
}

void RGrassSystem::RenderGStage( RRENDERINGMETHOD method, RCameraSceneNode* pCamera)
{
	RPFC_THISFUNC;

	if (!m_bRendering || CSpeedGrassRT::m_fLodFarDistance <= 0) return;

	RCameraSceneNode *cam = pCamera;
	if (cam == NULL) return;

	RVector m_CamPos = cam->GetPosition();
	RMatrix matViewProj = cam->GetViewMatrix();

	CSpeedGrassRT::SetPerspective(cam->GetAspectRatio(), cam->GetFovY());

	m_pGrass->SetCamera((const float*)&m_CamPos, (const float*)&matViewProj);
	m_pGrass->Cull();

	m_pGrass->SetBasicRenderState(this, pCamera);

	m_nRenderGrassCount = m_pGrass->RenderGStage( method);
	m_pGrass->ResetRenderState();
	REngine::GetResourceProfiler().AddPolygon(PPT_GRASS,m_nRenderGrassCount);
}



void RGrassSystem::Update(RSceneManager* pManager)
{
	m_fTime += REngine::GetTimer().GetElapsedTime() / 1000.0f;
}


bool RGrassSystem::SaveToFile(const string& filename)
{
	// 텍스쳐가 없어도, grs는 저장한다.

	int nNum = m_pGrass->GrassTypeCount();
	if ( nNum == 0 )
		return false;

	FILE* fp = NULL;
	fp = fopen(filename.c_str(), "wb");

	if (fp == NULL) 
		return false;

	char szBuffer[256];
	szBuffer[0] = '\0';

	fwrite("GRS ", 4, 1, fp);	// HEADER
	DWORD dwVersion = RGrassLoader::GetCurrentVersion();
	fwrite(&dwVersion, sizeof(DWORD), 1, fp);	// VERSION

	// region 
	fwrite(&(m_pGrass->m_nNumRegionRows), 4, 1, fp);	
	fwrite(&(m_pGrass->m_nNumRegionCols), 4, 1, fp);	
	fwrite(&(m_pGrass->m_afBoundingBox[3]), 4, 1, fp);
	fwrite(&(m_pGrass->m_afBoundingBox[4]), 4, 1, fp);

	// .dds (compositemap filename) 공간은 사용되지 않음
	memset(szBuffer, 0x00, 256);
	fwrite(szBuffer, 256, 1, fp);

	fwrite(&nNum, 4, 1, fp);		// grass kind num
	for ( UINT ui = 0; ui<RSpeedGrass::GRASS_MAX_NUM; ++ui )
	{
		// Table 내용을 그대로 기록한다.
		RSpeedGrass::SCompositeMapInfo *pCInfo = m_pGrass->m_pGrassTable[ui];
		if ( pCInfo == NULL )
			continue;
		
		fwrite(&ui, sizeof(int), 1, fp);	// save grass id
		strncpy(szBuffer, pCInfo->strFilename.c_str(), 256);
		fwrite(szBuffer, 256, 1, fp);
		fwrite(&pCInfo->fWidth, sizeof(float), 1, fp);
		fwrite(&pCInfo->fHeight,sizeof(float), 1, fp);
		fwrite(&pCInfo->fUOffset, sizeof(float), 1, fp);
		fwrite(&pCInfo->fVOffset, sizeof(float), 1, fp);
	}

	unsigned int uiNumRegions = 0;
	const CSpeedGrassRT::SRegion* pRegions = m_pGrass->GetRegions(uiNumRegions);

	typedef CSpeedGrassRT::SPackedBlade6 GrassPackedBlade;
	GrassPackedBlade PackedBlade;

	int bnum = 0;
	for (int i = 0; i < (int)uiNumRegions; ++i)
	{
		bnum += int(pRegions[i].m_vBlades.size( )); 
	}
	fwrite(&bnum, 4, 1, fp);	// blade count

	// run through the regions and render those that aren't culled
	for (int i = 0; i < (int)uiNumRegions; ++i)
	{
		std::vector<CSpeedGrassRT::SBlade>::const_iterator it;
		for  ( it = pRegions[i].m_vBlades.begin(); it!= pRegions[i].m_vBlades.end(); ++it )
		{
			const CSpeedGrassRT::SBlade &b = (*it);
			PackedBlade.Pack(b);
			fwrite(&PackedBlade, sizeof(GrassPackedBlade), 1, fp);	// write packed-blade
		}
	}	

	fclose(fp);
	return true;
}


bool RGrassSystem::LoadFromFile( const string& filename, RTerrain* pTerrain)
{
	MFile file;
	if( file.Open( filename.c_str() ) == false )
		return false;

	char szHeader[5];
	memset(szHeader, 0, sizeof(szHeader));

	file.Read( szHeader, 4 );
	if( strcmp(szHeader,"GRS ") )
		return false;

	DWORD dwVersion;
	file.Read( &dwVersion, sizeof(DWORD) );

	RGrassLoader* pLoader = RGrassLoader::CreateLoader( dwVersion );
	if( !pLoader )
		return false;

	// save row, col, width, height
	struct
	{
		int rows;
		int cols;
		float fWidth;
		float fHeight;
	} sSizeChunk;

	file.Read( &sSizeChunk, sizeof(sSizeChunk) );

	char szBuffer[256];
	file.Read( szBuffer, 256 );		// 사용되지 않음.

	int nNum;
	file.Read( &nNum, 4 );			// grass num
	for ( int i = 0; i<nNum; ++i )
	{
		UINT ui;
		file.Read( &ui, sizeof(int) );	// grass id
		
		_ASSERT(m_pGrass->m_pGrassTable[ui] == NULL);
		m_pGrass->m_pGrassTable[ui] = new RSpeedGrass::SCompositeMapInfo();
		RSpeedGrass::SCompositeMapInfo *pCInfo = m_pGrass->m_pGrassTable[ui];
		file.Read( szBuffer, 256 );
		pCInfo->strFilename = szBuffer;
		file.Read( &pCInfo->fWidth, sizeof(float) );
		file.Read( &pCInfo->fHeight, sizeof(float) );
		file.Read( &pCInfo->fUOffset, sizeof(float) );
		file.Read( &pCInfo->fVOffset, sizeof(float) );
	}

	int bc;
	file.Read( &bc, 4 );	// blade num

	bool bLoadSuccess = pLoader->Load(file, bc, m_pGrass, this, pTerrain);

	delete pLoader;
	file.Close();

	// texture load
	string textureFileName = RRule::GrassSystem::GetGrassTextureFileNameByGrassFileName( filename );
	LoadTexture(textureFileName.c_str());

	return bLoadSuccess;
}

void RGrassSystem::UpdateOnCallback(ICallback *piCallback)
{
	UINT uNum = 0;
	RSpeedGrass::SRegion *pRegions = m_pGrass->GetRegions(uNum);

	for ( UINT ui = 0; ui<uNum; ++ui )
	{
		for ( UINT un = 0; un<pRegions[ui].m_vBlades.size(); ++un )
		{
			RGrassBlade &b = pRegions[ui].m_vBlades[un];
			piCallback->OnBlade(b);
		}
	}

	m_bNeedUpdate = true;
}

const char* RGrassSystem::GetLoadedTextureName()
{
	if ( m_strTextureFilename.empty() )
		return NULL;
	else
		return m_strTextureFilename.c_str();
}


void RGrassSystem::LoadGrassListFromXML(MXmlElement &xmlRoot, const char *szNameA, const char *szNameB)
{
	_ASSERT(m_pGrass!=NULL);
	
	const char *pcFilename = xmlRoot.Attribute("filename");
	_ASSERT(pcFilename!=NULL);

	if ( pcFilename )
		LoadTexture(pcFilename);

	m_pGrass->LoadGrass(xmlRoot, szNameA, szNameB);
	m_pGrass->UpdateGrass();

	m_bNeedUpdate = true;
}

int RGrassSystem::GetGrassID(const char *szFilename)
{
	_ASSERT(m_pGrass!=NULL);
	return m_pGrass->GetGrassID(szFilename);
}

int RGrassSystem::GetGrassWidth(int nIndex)
{
	_ASSERT(m_pGrass!=NULL);
	return m_pGrass->GetGrassWidth(nIndex);
}

int RGrassSystem::GetGrassHeight(int nIndex)
{
	_ASSERT(m_pGrass!=NULL);
	return m_pGrass->GetGrassHeight(nIndex);
}

void RGrassSystem::SetAmbientColor(RVector4 &vColor)
{
	_ASSERT(m_pGrass!=NULL);
	m_pGrass->SetAmbientColor(vColor);
}

RVector4 RGrassSystem::GetAmbientColor()
{
	_ASSERT(m_pGrass!=NULL);
	return m_pGrass->GetAmbientColor();
}

void RGrassSystem::SetDiffuseColor(RVector4 &vColor)
{
	_ASSERT(m_pGrass!=NULL);
	m_pGrass->SetDiffuseColor(vColor);
}

RVector4 RGrassSystem::GetDiffuseColor()
{
	_ASSERT(m_pGrass!=NULL);
	return m_pGrass->GetDiffuseColor();
}

int RGrassSystem::GetCurrentRenderGrassCount()
{
	return m_nRenderGrassCount/2;
}

//////////////////////////////////////////////////////////////////////////
// RGrassSystem_WindStrengthAmplifier

float RGrassSystem_WindStrengthAmplifier::GetNewAmplifier()
{
	return RandomNumber(1.5f, 3.5f);
}

float RGrassSystem_WindStrengthAmplifier::GetNewUpdateTime()
{
	return RandomNumber(3.0f, 8.0f);
}

RGrassSystem_WindStrengthAmplifier::RGrassSystem_WindStrengthAmplifier()
{
	const float DEFAULT_AMP = 3.0f;

	m_fElapsedTime = 0.0f;
	m_fNextUpdateTime = 0.0f;
	m_fAmplifier = DEFAULT_AMP;

	m_fStrengthCoefficient = 10.f;
}

float RGrassSystem_WindStrengthAmplifier::Strength( float fDeltaTime, float fWindStrength )
{
	m_fElapsedTime += fDeltaTime;

	if (fWindStrength > 0.01f)
	{
		if (m_fElapsedTime > m_fNextUpdateTime)
		{
			m_fAmplifier = GetNewAmplifier();
			m_fNextUpdateTime = GetNewUpdateTime();

			m_fElapsedTime = 0.0f;
		}
	}

	// by pok. 풀 흔들림 계수조정(*10배) http://dev:8181/projects/engine/ticket/537
	return (fDeltaTime * fWindStrength * m_fAmplifier * m_fStrengthCoefficient);
}
}