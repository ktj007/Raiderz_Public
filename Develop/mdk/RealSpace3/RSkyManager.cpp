#include "stdafx.h"
#include "RSkyManager.h"
#include "RSceneSky.h"
#include "RCameraSceneNode.h"
#include "RWaterPassRenderer.h"
#include "RMaterialResource.h"
#include "RMesh.h"
#include "RShaderCompositeManager.h"

#include <algorithm>

#include "RXMLTemplate.h"

namespace rs3 {

using namespace std;

const char* RSkyManager::ELEMENT_FILENAME	= "FILENAME";
const char* RSkyManager::ELEMENT_SPEED		= "SPEED";
const char* RSkyManager::ELEMENT_FOGFACTOR	= "FOGFACTOR";
const char* RSkyManager::ELEMENT_COLOR		= "COLOR";

RSkyManager::RSkyManager() : RSceneNode(RRP_SKYBOX)
,m_vMultiplyColor(1.f, 1.f, 1.f, 1.f), m_nRenderOrder(eRENDERORDER_FIRST)
{
	m_pSkyArray = new SKYARRAY[LAYER_NUM];
	m_fVisibility = 1.0f;

	// 무조건 렌더링 되도록
	SetAABB( RBoundingBox::INFINITY );

	SetAutoUpdate(true);
}

RSkyManager::~RSkyManager()
{
	SetAutoUpdate(false);
	Destroy();
	SAFE_DELETE_ARRAY(m_pSkyArray);
}

void RSkyManager::Destroy()
{
	for ( UINT ui = 0; ui<LAYER_NUM; ++ui )
	{
		SKYARRAY &rArray = m_pSkyArray[ui];

		for ( SKYARRAY::iterator it = rArray.begin(); it != rArray.end(); ++it )
		{
			RSkyInfo &info = (*it);
			SAFE_DELETE(info.pSky);
		}

		rArray.clear();
	}
}

void RSkyManager::SetAnimationSpeed()
{
	for ( UINT ui = 0; ui<LAYER_NUM; ++ui )
	{
		SKYARRAY &rArray = m_pSkyArray[ui];
		for ( SKYARRAY::iterator it = rArray.begin(); it != rArray.end(); ++it )
		{
			RSkyInfo &info = (*it);
			if ( RSceneSky *pSky = info.pSky )
			{
				pSky->SetSpeed(info.fUVAniSpeed);
			}
		}
	}
}

bool RSkyManager::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	using namespace TXML;
	// SkyManager 는 Background loading을 하지 않습니다.
	// dub - 아래쪽에 AUTOCREATE 만들면서 실제로 백그라운드 로딩을 하는데, 
	// 어서션이 있는줄 몰랐네요, 문제생기면 리팩토링하겠습니다
//	_ASSERT( bBackgroundCreation == false );

	for ( MXmlElement *pElement = element.FirstChildElement("SKYMESH"); pElement != NULL
		; pElement = pElement->NextSiblingElement("SKYMESH") )
	{	
		int nLayer = 0;
		pElement->Attribute("layer",&nLayer);

		RSkyInfo kInfo;
		pElement->Attribute("order",&kInfo.nOrder);
		MXmlElement *pChild = pElement->FirstChildElement(ELEMENT_FILENAME);
		
		if ( pChild == NULL )
			continue;

		_Contents(kInfo.strFilename, pChild);
		
		RVector4 vColor;
		_GetElementValue(&vColor, pElement, ELEMENT_COLOR, "vec4");
		DWORD dwAlpha = (DWORD)(vColor.w * 255);
		DWORD dwRed = (DWORD)(vColor.x * 255);
		DWORD dwGreen = (DWORD)(vColor.y * 255);
		DWORD dwBlue = (DWORD)(vColor.z * 255);
		kInfo.dwColor = D3DCOLOR_ARGB( dwAlpha, dwRed, dwGreen, dwBlue);

		kInfo.fUVAniSpeed = 1.0f;
		if ( pElement->FirstChildElement(ELEMENT_SPEED))
			kInfo.fUVAniSpeed = _GETElement<float>::Value(pElement, ELEMENT_SPEED, "float");

		kInfo.fFogFactor = 1.0f;
		if ( pElement->FirstChildElement(ELEMENT_FOGFACTOR))
			kInfo.fFogFactor = _GETElement<float>::Value(pElement, ELEMENT_FOGFACTOR, "float");

		// create skyscene
		kInfo.pSky = NULL;
		AddSky(0, kInfo);
	}

	if ( element.FirstChildElement("AUTOCREATE") != NULL )
	{
		CreateScenes(bBackgroundCreation);
	}

	return true;
}

void RSkyManager::SaveToXML(MXmlElement &element)
{
	UpdateSkyLayer();

	for ( UINT ui = 0; ui<LAYER_NUM; ++ui )
	{
		SKYARRAY &rArray = m_pSkyArray[ui];
		for ( SKYARRAY::iterator it = rArray.begin(); it != rArray.end(); ++it )
		{
			RSkyInfo &info = (*it);

			MXmlElement *pElement = new MXmlElement("SKYMESH");
			pElement->SetAttribute("layer", (int)ui);
			pElement->SetAttribute("order", info.nOrder);

			{
				using namespace TXML;
				_SetElementValue(pElement, ELEMENT_FILENAME, "string", info.strFilename.c_str());

				RVector4 vColor;	// ARGB
				vColor.w = (( info.dwColor >> 24 ) & 0xff)/255.0f;
				vColor.x = (( info.dwColor >> 16 ) & 0xff)/255.0f;
				vColor.y = (( info.dwColor >> 8 ) & 0xff)/255.0f;
				vColor.z = (( info.dwColor ) & 0xff)/255.0f;

				_SetElementValue(pElement, ELEMENT_COLOR, "vec4", vColor);
				_SetElementValue(pElement, ELEMENT_SPEED, "float", info.fUVAniSpeed);
				_SetElementValue(pElement, ELEMENT_FOGFACTOR, "float", info.fFogFactor);
			}

			element.LinkEndChild(pElement);
		}
	}
}

void RSkyManager::UpdateSkyLayer()
{
	class Sort
	{
	public:
		static bool eval(RSkyInfo &a, RSkyInfo &b)
		{
			return a.nOrder < b.nOrder;
		}
	};
	for ( UINT ui = 0; ui<LAYER_NUM; ++ui )
		std::sort(m_pSkyArray[ui].begin(), m_pSkyArray[ui].end(), Sort::eval);
}

void RSkyManager::ReorderNumber()
{
	for ( UINT ui = 0; ui<LAYER_NUM; ++ui )
	{
		UINT uSize = m_pSkyArray[ui].size();
		if ( uSize >= 1 )
			(m_pSkyArray[ui])[0].nOrder = 0;

		for ( UINT un = 1; un < uSize; ++un )
			(m_pSkyArray[ui])[un].nOrder = (int)un;
	}
}

bool RSkyManager::CreateScenes(bool bBackgroundCreation)
{
	bool bResult = true;

	for ( UINT ui = 0; ui<LAYER_NUM; ++ui )
	{
		SKYARRAY &rArray = m_pSkyArray[ui];
		for ( SKYARRAY::iterator it = rArray.begin(); it != rArray.end(); ++it )
		{
			RSkyInfo &info = (*it);

			if ( info.pSky )
			{
				info.skyRefCount++;
			}
			else
			{
				info.pSky = new RSceneSky( &info );
				info.skyRefCount = 1;

				bool bCreatedSky = info.pSky->Create(info.strFilename.c_str(), bBackgroundCreation);
				bResult = bResult && bCreatedSky;
			}
		}
	}

	ReorderNumber();

	return bResult;
}

void RSkyManager::ReleaseScenes()
{
	for ( UINT ui = 0; ui<LAYER_NUM; ++ui )
	{
		SKYARRAY &rArray = m_pSkyArray[ui];
		for ( SKYARRAY::iterator it = rArray.begin(); it != rArray.end(); ++it )
		{
			RSkyInfo &info = (*it);

			_ASSERT( info.skyRefCount > 0 );
			info.skyRefCount--;
			if ( info.skyRefCount == 0 )
			{
				SAFE_DELETE(info.pSky);
				//info.nOrder = -1;		//환경탭에서 다른 날씨나 시간을 보기만 해도 여기에 들어와, 값을 바꾸어 버려, 저장시 -1값이들어가는 문제
			}
		}
	}
}

RRSTATE RSkyManager::GetScenesState()
{
	for ( UINT ui = 0; ui<LAYER_NUM; ++ui )
	{
		SKYARRAY &rArray = m_pSkyArray[ui];
		for ( SKYARRAY::iterator it = rArray.begin(); it != rArray.end(); ++it )
		{
			RSkyInfo &info = (*it);
			if ( info.pSky == NULL )
				return RRS_EMPTY;

			if ( info.pSky->m_pMesh == NULL)
				return RRS_EMPTY;

			if ( info.pSky->m_pMesh->GetState() == RRS_WRONG )
				return RRS_WRONG;
			else if ( info.pSky->m_pMesh->GetState() == RRS_EMPTY )
				return RRS_EMPTY;

			std::vector<RActorNode*> &nodes = info.pSky->m_actorNodes;
			for  (size_t i = 0; i<nodes.size(); ++i )
			{
				RActorNode *pActorNode = nodes[i];
				std::vector<RMaterialResource*,RStlCustomAlloc<RMaterialResource*>> &mtrlRes = pActorNode->m_vecMaterialTable;
				for ( size_t j = 0; j<mtrlRes.size(); ++j )
				{
					RResource *pResource = static_cast<RResource*>(mtrlRes[j]);
					if ( pResource->GetState() == RRS_WRONG )
						return RRS_WRONG;
					else if ( pResource->GetState() == RRS_EMPTY )
						return RRS_EMPTY;
				}
			}
		}
	}

	return RRS_LOADED;
}

bool RSkyManager::AddSky(int nLayer, RSkyInfo &info)
{
	_ASSERT(m_pSkyArray);

	if ( nLayer < 0 )	return false;
	if ( nLayer >= LAYER_NUM )	return false;

	ReorderNumber();

	if ( info.nOrder < 0 )
		info.nOrder = (int)m_pSkyArray[nLayer].size();

	m_pSkyArray[nLayer].push_back(info);
	
	UpdateSkyLayer();

	return true;
}

void RSkyManager::DelSky(int nLayer, int nIndex)
{
	_ASSERT(m_pSkyArray);

	if ( nLayer < 0 )	return;
	if ( nLayer >= LAYER_NUM )	return;
	if ( nIndex < 0 )	return;
	
	SKYARRAY &rArray = m_pSkyArray[nLayer];
	if ( (int)rArray.size() <= nIndex )
		return;

	delete rArray[nIndex].pSky;
	rArray.erase(rArray.begin()+nIndex);

	return;
}

int RSkyManager::FindSky(int nLayer, RSceneSky *pSkyNode)
{
	if ( nLayer < 0 )	return -1;
	if ( nLayer >= LAYER_NUM )	return -1;
	if ( pSkyNode == NULL )		return -1;

	SKYARRAY &rArray = m_pSkyArray[nLayer];
	for ( size_t si = 0; si<rArray.size(); ++si )
	{
		if ( rArray[si].pSky == pSkyNode )
			return (int)si;
	}

	return -1;
}

RSkyInfo* RSkyManager::GetSkyInfo(int nLayer, int nIndex)
{
	if ( nLayer < 0 )	return NULL;
	if ( nLayer >= LAYER_NUM )	return NULL;
	if ( nIndex < 0 )		return NULL;
	
	SKYARRAY &rArray = m_pSkyArray[nLayer];
	if ( nIndex >= (int)rArray.size())	return NULL;

	return &rArray[nIndex];
}

void RSkyManager::OnUpdate()
{
	RPFC_THISFUNC;

	for ( UINT ui = 0; ui<LAYER_NUM; ++ui )
	{
		SKYARRAY &rArray = m_pSkyArray[ui];
		for ( SKYARRAY::iterator it = rArray.begin(); it != rArray.end(); ++it )
		{
			RSkyInfo &info = (*it);
			if ( info.pSky )
				info.pSky->Update();

		}
	}
}


void RSkyManager::RenderSky( RCameraSceneNode* pCurrentCamera, RRENDERINGMETHOD renderMothod)
{
	RPFC_THISFUNC;

	// matrix
	RDevice * const pDevice = REngine::GetDevicePtr();
	_ASSERT(pDevice);

	RCameraSceneNode *pCamera = pCurrentCamera;
	float fPrevNearZ = pCamera->GetNearZ();
	float fPrevFarZ = pCamera->GetFarZ();
	bool bClip = pCamera->GetClipPlaneEnable();
	RVector vCameraPrevPos = pCamera->GetPosition();

	// set render state
	const float SKY_FARZ	= 5000000.0f;
	const float SKY_NEARZ	= 5000;
	pCamera->SetNearFarZ(SKY_NEARZ, SKY_FARZ);
	pCamera->SetPosition(0, 0, 0);
	pCamera->SetClipPlaneEnable(false);
	pCamera->UpdateViewFrustrum();


	for ( UINT ui = 0; ui<LAYER_NUM; ++ui )
	{
		SKYARRAY &rArray = m_pSkyArray[ui];
		for ( SKYARRAY::iterator it = rArray.begin(); it != rArray.end(); ++it )
		{
			RSkyInfo &info = (*it);
			if ( info.pSky )
			{
				DWORD dwColor = info.dwColor;

				BYTE r, g, b, a;
				a = ( dwColor >> 24 ) & 0xff;
				r = ( dwColor >> 16 ) & 0xff;
				g = ( dwColor >> 8  ) & 0xff;
				b = ( dwColor ) & 0xff;

				RVector4 vColor( 
					m_vMultiplyColor.x * (r/255.f)
					, m_vMultiplyColor.y * (g/255.f)
					, m_vMultiplyColor.z * (b/255.f)
					, m_vMultiplyColor.w * (a/255.f));

				if ( info.pSky->GetUsable())
					info.pSky->Render( pCurrentCamera, vColor, info.fFogFactor, renderMothod);
			}
		}
	}

	// restore state
	pCamera->SetNearFarZ( fPrevNearZ, fPrevFarZ);
	pCamera->SetPosition(vCameraPrevPos);
	pCamera->SetClipPlaneEnable(bClip);
	pCamera->UpdateViewFrustrum();
}


void RSkyManager::OnRenderGStage( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod)
{
	RPFC_THISFUNC;

	// G stage에서 스카이 랜더는 deferred만이 가능
	if( renderMothod != RENDERING_DEFERRED)
		return;

	RenderSky( pCurrentCamera, renderMothod);	
}

void RSkyManager::OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod)
{
	RPFC_THISFUNC;

	if( renderMothod == RENDERING_DEFERRED)
		return;

	RenderSky( pCurrentCamera, renderMothod);
}

SKYARRAY* RSkyManager::GetArray(int nLayer)
{
	if ( nLayer < 0 )	return NULL;
	if ( nLayer >= LAYER_NUM )	return NULL;

	return &m_pSkyArray[nLayer];
}

void RSkyManager::SetMultiplyColor(RVector4 &vec4)
{
	m_vMultiplyColor = vec4;
}

const RVector4& RSkyManager::GetMutiplyColor()
{
	return m_vMultiplyColor;
}

};