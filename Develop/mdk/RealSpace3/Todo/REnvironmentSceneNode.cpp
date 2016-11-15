#include "stdafx.h"
#include "RDevice.h"
#include "REnvironmentSceneNode.h"
#include "RWaterPassRenderer.h"
#include "RWheatherEffect.h"

#include "RXMLTemplate.h"

namespace rs3 {

//////////////////////////////////////////////////////////////////////////
//
// REnvironmentSceneNode enum converter
//
//////////////////////////////////////////////////////////////////////////
char *REnvironmentSceneNode::m_pcTimeTable[TIME_NUM] =
{
	"dawn",
	"daytime",
	"sunset",
	"evening",
	"night"
};

char *REnvironmentSceneNode::m_pcWheatherTable[WHEATHER_NUM] =
{
	"sunny",
	"cloud",
	"rainy",
	"heavy_rainy",
	"snow",
	"heavy_snow",
	"clear",
};

const char *REnvironmentSceneNode::ETimeToString(ETime eTime)
{
	if ( eTime >= TIME_NUM )	return NULL;
	if ( eTime < 0 )			return NULL;

	return m_pcTimeTable[eTime];
}

const char *REnvironmentSceneNode::EWheatherToString(EWheather eWheather)
{
	if ( eWheather >= WHEATHER_NUM )	return NULL;
	if ( eWheather < 0 )				return NULL;

	return m_pcWheatherTable[eWheather];
}

REnvironmentSceneNode::ETime REnvironmentSceneNode::StringToETime(const char *szName)
{
	for ( int i = 0; i<TIME_NUM; ++i )
	{
		ETime ei = (ETime)i;
		if ( stricmp(szName,m_pcTimeTable[ei]) == 0 )
			return ei;
	}

	return TIME_NUM;
}

REnvironmentSceneNode::EWheather REnvironmentSceneNode::StringToEWheather(const char *szName)
{
	for ( int i = 0; i<WHEATHER_NUM; ++i )
	{
		EWheather ei = (EWheather)i;
		if ( stricmp(szName,m_pcWheatherTable[ei]) == 0 )
			return ei;
	}

	return WHEATHER_NUM;
}

//////////////////////////////////////////////////////////////////////////
//
// REnvironmentPiece
//
//////////////////////////////////////////////////////////////////////////

void REnvironmentSceneNode::REnvironmentPiece::ResetEnvPiece(bool bResetInitValue, bool bDeepDeleteSky)
{
	if (bResetInitValue)
	{
		vAmbient.Set(.4f, .4f, .4f, .4f);
		vDiffuse.Set(1,1,1,1);
		vCharacterAmbient.Set(.4f, .4f, .4f, .4f);
		vCharacterDiffuse.Set(1,1,1,1);
		fPower = 1.f;
		vFogColor.Set(1,1,1,1);
		bFogDisable = false;
		fNear = 100000.0f;
		fFar = 300000.0f;
		vWindDirection.Set(0, 0, 0);
		fWindStrength = 0.0f;
		m_bCloudVisible = false;
		m_vCloudShadowScale.Set( 10000.f, 10000.f, 0 );
		m_vCloudShadowTranslate.Set( 300.f, 300.f, 0 );
	}

	if (m_vecSkyArray.empty() == false)
	{
		for ( std::vector<RSkySceneNode*>::iterator it = m_vecSkyArray.begin(); it != m_vecSkyArray.end(); ++it )
		{
			RSkySceneNode* pSky = *it;
			pSky->RemoveFromParent();

			if (bDeepDeleteSky)
				delete pSky;
		}

		if (bDeepDeleteSky)
		{
			m_vecSkyArray.clear();
			if (m_pEnvSceneNode)
				m_pEnvSceneNode->m_bEnvPieceChanged;
		}
	}
}

void REnvironmentSceneNode::REnvironmentPiece::OnAddToCurrnetEnvironment(REnvironmentSceneNode* pEnvironmentSceneNode)
{
	m_pEnvSceneNode = pEnvironmentSceneNode;
	for ( std::vector<RSkySceneNode*>::iterator it = m_vecSkyArray.begin(); it != m_vecSkyArray.end(); ++it )
	{
		RSkySceneNode* pSky = *it;
		m_pEnvSceneNode->m_pSceneManager->AddSceneNode(pSky);
	}
}

void REnvironmentSceneNode::REnvironmentPiece::OnRemoveFromCurrentEnvironment()
{
	if (m_pEnvSceneNode)
	{
		ResetEnvPiece(false, false);
		m_pEnvSceneNode = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// Sky Layer
//
int REnvironmentSceneNode::REnvironmentPiece::AddSky(const char* _pSzFileName)
{
	RSkySceneNode *pSky = new RSkySceneNode;
	bool bRet = pSky->Create(_pSzFileName, false);
	if (bRet == false)
	{
		delete pSky;
		return -1;
	}

	m_vecSkyArray.push_back(pSky);
	int nSize = (int)m_vecSkyArray.size();
	pSky->m_nOrder = 2 * nSize; //order는 짝수로만 관리

	pSky->SetAnimation("idle");
	pSky->Play();

	if (m_pEnvSceneNode)
		m_pEnvSceneNode->m_bEnvPieceChanged = true;

	return nSize;
}

int REnvironmentSceneNode::REnvironmentPiece::AddSky(MXmlElement& xmlElementForSky)
{
	using namespace TXML;

	char szBuffer[256];
	MXmlElement *pElement = xmlElementForSky.FirstChildElement(ELEMENT_FILENAME);
	_ASSERT(pElement!=NULL);
	if (pElement == NULL) 
		return false;
	_Contents(szBuffer,pElement);

	int nIndex = AddSky(szBuffer);
	if (nIndex < 0)
		return -1;

	RVector4 vColor;
	_GetElementValue(&vColor,&xmlElementForSky,ELEMENT_COLOR,"vec4");

	DWORD dwAlpha = (DWORD)(vColor.w * 255);
	DWORD dwRed = (DWORD)(vColor.x * 255);
	DWORD dwGreen = (DWORD)(vColor.y * 255);
	DWORD dwBlue = (DWORD)(vColor.z * 255);

	DWORD dwColor = D3DCOLOR_ARGB( dwAlpha, dwRed, dwGreen, dwBlue);

	float fSpeed = 1.0f;
	if ( xmlElementForSky.FirstChildElement(ELEMENT_SPEED) != NULL )
		fSpeed = _GETElement<float>::Value(&xmlElementForSky,ELEMENT_SPEED,"float");

	float fFogFactor = 1.0f;
	if ( xmlElementForSky.FirstChildElement(ELEMENT_FOGFACTOR) != NULL )
		fFogFactor = _GETElement<float>::Value(&xmlElementForSky,ELEMENT_FOGFACTOR,"float");

	RSkySceneNode* pSky = m_vecSkyArray[nIndex];
	pSky->m_dwColor = dwColor;
	pSky->m_fUVAniSpeed = fSpeed;
	pSky->m_fFogFactor = fFogFactor;

	return nIndex;
}

void REnvironmentSceneNode::REnvironmentPiece::ReOrderSky()
{
	std::sort(m_vecSkyArray.begin(), m_vecSkyArray.end(), RSkySceneNode::eval_order);

	int nCount = (int)m_vecSkyArray.size();
	for( int i = 0; i < nCount; ++i )
	{
		RSkySceneNode* pSky = m_vecSkyArray[i];
		pSky->m_nOrder = 2 * i;
	}

	if (m_pEnvSceneNode)
		m_pEnvSceneNode->m_bEnvPieceChanged = true;
}

bool REnvironmentSceneNode::REnvironmentPiece::DeleteSky(int _nIndex)
{
	if ((int)m_vecSkyArray.size() < _nIndex)
		return false;

	std::vector<RSkySceneNode*>::iterator itr = m_vecSkyArray.begin() + _nIndex;
	RSkySceneNode* pSky = *itr;
	pSky->RemoveFromParent();
	delete pSky;

	m_vecSkyArray.erase(itr);
	ReOrderSky();
	return true;
}

void REnvironmentSceneNode::REnvironmentPiece::ChangeSkyOrder( int _nLayerToChange, bool bIncrease )
{
	_ASSERT( _nLayerToChange < (int)m_vecSkyArray.size() );

	int nReorder = bIncrease ? 3 : -3;
	RSkySceneNode* pSky = m_vecSkyArray[_nLayerToChange];
	pSky->m_nOrder += nReorder;

	ReOrderSky();
}

void REnvironmentSceneNode::REnvironmentPiece::SetSkyColor( int _nIndex, DWORD dwColor )
{
	_ASSERT( _nIndex < (int)m_vecSkyArray.size() );
	m_vecSkyArray[_nIndex]->m_dwColor = dwColor;
}

void REnvironmentSceneNode::REnvironmentPiece::SetSkySpeed( int _nIndex, float f )
{
	_ASSERT( _nIndex < (int)m_vecSkyArray.size() );
	m_vecSkyArray[_nIndex]->m_fUVAniSpeed = f;
}

void REnvironmentSceneNode::REnvironmentPiece::SetFogFactor( int _nIndex, float f )
{
	_ASSERT( _nIndex < (int)m_vecSkyArray.size() );
	m_vecSkyArray[_nIndex]->m_fFogFactor = f;
}

void REnvironmentSceneNode::REnvironmentPiece::GetSkyInfo(int _nIndex, REnvironmentSceneNode::REnvironmentPiece::SSKY_INFO& _refSkyInfo)
{
	_ASSERT( _nIndex < (int)m_vecSkyArray.size() );
	RSkySceneNode* pSkySceneNode = m_vecSkyArray[_nIndex];

	_refSkyInfo.nIndex = _nIndex;
	_refSkyInfo.strName = pSkySceneNode->GetFilename();
	_refSkyInfo.dwColor = pSkySceneNode->m_dwColor;
	_refSkyInfo.fSpeed = pSkySceneNode->m_fUVAniSpeed;
	_refSkyInfo.fFogFactor = pSkySceneNode->m_fFogFactor;
}

void REnvironmentSceneNode::REnvironmentPiece::GetAllSkiesInfo( std::vector< REnvironmentSceneNode::REnvironmentPiece::SSKY_INFO >& _refVecSkyInfo )
{
	int nCount = m_vecSkyArray.size();
	SSKY_INFO sky_info;

	for (int i = 0; i < nCount; ++i )
	{
		RSkySceneNode* pSkySceneNode = m_vecSkyArray[i];
		sky_info.nIndex = i;
		sky_info.strName = pSkySceneNode->GetFilename();
		sky_info.dwColor = pSkySceneNode->m_dwColor;
		sky_info.fSpeed = pSkySceneNode->m_fUVAniSpeed;
		sky_info.fFogFactor = pSkySceneNode->m_fFogFactor;

		_refVecSkyInfo.push_back(sky_info);
	}
}

//////////////////////////////////////////////////////////////////////////
// Sky
//
const char* REnvironmentSceneNode::REnvironmentPiece::ELEMENT_FILENAME  = "FILENAME";
const char* REnvironmentSceneNode::REnvironmentPiece::ELEMENT_SPEED	    = "SPEED";
const char* REnvironmentSceneNode::REnvironmentPiece::ELEMENT_FOGFACTOR = "FOGFACTOR";
const char* REnvironmentSceneNode::REnvironmentPiece::ELEMENT_COLOR	    = "COLOR";

bool REnvironmentSceneNode::REnvironmentPiece::RSkySceneNode::eval_order(RSkySceneNode *a, RSkySceneNode *b)
{
	return a->m_nOrder < b->m_nOrder;
}

bool REnvironmentSceneNode::REnvironmentPiece::RSkySceneNode::Create(const char *filename, bool bBackgroundCreation)
{
	// by pok_background, 하늘은 백그라운드 로딩하지 않는다.
	bool bResult = RActor::Create(filename, bBackgroundCreation);
	m_bDepthEnable = false;
	m_bDepthWriteEnable = false;

	TurnOffShade();
	return bResult;
}

void REnvironmentSceneNode::REnvironmentPiece::RSkySceneNode::RenderSky(const RVector4 &vMultiplyColor)
{
	//RActor::OnPreRender();
	UpdateForRender(); // 예전에 OnPreRender에서 했던일을 OnRegisterToRender쪽으로 옮깁니다.

	RDevice *pDevice = RDevice::GetDevice();
	pDevice->SetAlphaRef(0x00);
	pDevice->SetAlphaFunc(RCMP_GREATER);
	pDevice->SetAmbient(0);

	m_worldLightAmbient = RVector3(1, 1, 1);
	m_worldLightColor = RVector(vMultiplyColor.x,vMultiplyColor.y,vMultiplyColor.z);

	// (정책변경)부모에서 vis 를 세팅해주었기 때문에 각각 세팅할 필요없음. 
	// 단 액터노드는 manual update 정책이기때문에 UpdateVisibility를 해주어야 한다

	SetVisibility(vMultiplyColor.a);
	UpdateVisibility();

	RActor::m_cTextureFactor.SetVector4(vMultiplyColor);
	RActor::m_cFogFactor.SetVector4(0, 0, 0, m_fFogFactor);

	// 메쉬 렌더
	if ( !m_sortedRenderableActorNode.empty() )
	{
		for( int i=0; i<(int)m_sortedRenderableActorNode.size(); i++)
		{
			//			float fVis = m_sortedRenderableActorNode[i]->GetVisibility();
			//			m_sortedRenderableActorNode[i]->SetVisibility(vMultiplyColor.a * fVis);
			m_sortedRenderableActorNode[i]->UpdateVisibility();
			m_sortedRenderableActorNode[i]->RenderSingle(true, NULL);
			//			m_sortedRenderableActorNode[i]->SetVisibility(fVis);
		}
	}
}

const char* REnvironmentSceneNode::REnvironmentPiece::RSkySceneNode::GetFilename()
{
	return m_pMesh->GetName().c_str();
}

//////////////////////////////////////////////////////////////////////////
// Render / Update
//
void REnvironmentSceneNode::REnvironmentPiece::RenderEnvPiece()
{
	RMatrix matView = RDevice::GetDevice()->GetTransform( RST_VIEW );
	RCameraSceneNode *pCamera = RCameraSceneNode::GetCurrentCamera();
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

	// matrix
	bool bFog = RDevice::GetDevice()->GetFogEnable();
	RDevice::GetDevice()->SetFogEnable(true);
	RDevice::GetDevice()->SetDepthEnable(false, false);
	RDevice::GetDevice()->SetAlphaTestEnable(true);

	for ( std::vector<RSkySceneNode*>::iterator it = m_vecSkyArray.begin(); it != m_vecSkyArray.end(); ++it )
	{
		RSkySceneNode *pSceneSky = (*it);

		DWORD dwColor = pSceneSky->m_dwColor;
		BYTE r, g, b, a;
		a = ( dwColor >> 24 ) & 0xff;
		r = ( dwColor >> 16 ) & 0xff;
		g = ( dwColor >> 8  ) & 0xff;
		b = ( dwColor ) & 0xff;

		RVector4 vColor( pSceneSky->m_vMultiplyColor.x * (r/255.f)
			, pSceneSky->m_vMultiplyColor.y * (g/255.f)
			, pSceneSky->m_vMultiplyColor.z * (b/255.f)
			, pSceneSky->m_vMultiplyColor.w * (a/255.f));

		pSceneSky->RenderSky(vColor);
	}

	// restore state
	RDevice::GetDevice()->SetFogEnable( bFog);
	RDevice::GetDevice()->SetTransform(RST_VIEW, matView);
	RDevice::GetDevice()->SetDepthEnable(true, true);

	pCamera->SetNearFarZ( fPrevNearZ, fPrevFarZ);
	pCamera->SetPosition(vCameraPrevPos);
	pCamera->SetClipPlaneEnable(bClip);
	pCamera->UpdateViewFrustrum();
	RDevice::GetDevice()->SetTransform(RST_VIEW,matView);
}

void REnvironmentSceneNode::REnvironmentPiece::UpdateEnvPiece()
{
	for ( std::vector<RSkySceneNode*>::iterator it = m_vecSkyArray.begin(); it != m_vecSkyArray.end(); ++it )
		(*it)->UpdateSky();
}

bool REnvironmentSceneNode::REnvironmentPiece::ReloadEnvPiece(MXmlElement* pChildElement)
{
	Clear();

	// Sky 로딩
	for ( MXmlElement *pElement = pChildElement->FirstChildElement("SKYMESH"); pElement != NULL
		; pElement = pElement->NextSiblingElement("SKYMESH") )
		AddSky( *pElement );

	using namespace TXML;

	_GetElementValue(&vAmbient, pChildElement, "AMBIENT","vec4");
	_GetElementValue(&vDiffuse, pChildElement, "DIFFUSE","vec4");

	// 캐릭터 광원 설정이 없을경우 기본값.
	vCharacterAmbient = vAmbient;	
	vCharacterDiffuse = vDiffuse;
	_GetElementValue(&vCharacterAmbient, pChildElement, "CHARACTERAMBIENT","vec4");
	_GetElementValue(&vCharacterDiffuse, pChildElement, "CHARACTERDIFFUSE","vec4");

	// POWER 항목이 없을경우 기본값을 1로 주고싶은데 방법이 없어서,,, dubble
	if(NULL!=pChildElement->FirstChild("POWER"))
		fPower = _GETElement<float>::Value(pChildElement,"POWER" , "float");

	_GetElementValue(&vFogColor, pChildElement, "FOG_COLOR","vec4");
	fNear = _GETElement<float>::Value(pChildElement,"FOG_NEAR" , "float");
	fFar = _GETElement<float>::Value(pChildElement, "FOG_FAR", "float");
	bFogDisable = ( _GETElement<int>::Value(pChildElement, "FOG_DISABLE", "int") ? true : false);

	_GetElementValue(&vWindDirection, pChildElement, "WIND_DIRECTION", "vec3");
	fWindStrength = _GETElement<float>::Value(pChildElement,"WIND_STRENGTH", "float");

	// 구름 그림자 읽어들이는 부분	
	MXmlElement *pCloudElement = pChildElement->FirstChildElement("CLOUDSHADOW");
	if ( pCloudElement )
	{
		int num = 0;
		pCloudElement->Attribute("size",&num);
		_ASSERT(num==1);

		int nIndex = 0;
		MXmlElement *pElement = pCloudElement->FirstChildElement();
		_ASSERT(_stricmp(pElement->Value(),"LAYER")==0);
		{
			_GetElementValue(&m_vCloudShadowTranslate, pElement, "TRANSLATE","vec3");
			_GetElementValue(&m_vCloudShadowScale, pElement, "SCALE", "vec3");
		}
		m_bCloudVisible = ( _GETElement<int>::Value(pElement,"VISIBLE","int") ? true : false );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
// REnvironmentSceneNode
//
//////////////////////////////////////////////////////////////////////////

char *REnvironmentSceneNode::TILINGEFFECT_TEXTURE_FILENAME = "cloudshadow.tga";

REnvironmentSceneNode::REnvironmentSceneNode()
: m_bEnvPieceChanged(false), hTillingTexture(R_NONE), m_bRenderWeatherEffect(true), m_pCurrentEnvPiece(NULL)
{
	SetAutoUpdate(true);
}

REnvironmentSceneNode::~REnvironmentSceneNode()
{
}

void REnvironmentSceneNode::OnAddToSceneManager(RSingletonSceneManager* pSingletionSceneManager)
{
	m_pSceneManager = pSingletionSceneManager;

	// weather
	for ( int i = 0; i<WHEATHER_NUM; ++i )
		m_pWeatherEffectTable[i] = new RWeatherEffect;

	m_vecTilling.reserve(4);

	// default EnvironmentBunch 만들기
	AddEnvBunch("default");
	SetCurrentEnvPieceToDefault();
}

void REnvironmentSceneNode::OnRemoveFromSceneManager()
{
	if (RDevice::GetDevice())
		RDevice::GetDevice()->DeleteTexture(hTillingTexture);

	m_vecTilling.clear();

	// EnvironmentBunch 지우기
	DeleteAllEnvBunch();

	// Weather Effect 지우기
	for ( int i = 0; i<WHEATHER_NUM; ++i )
	{
		m_pWeatherEffectTable[i]->RemoveFromParent();
		delete m_pWeatherEffectTable[i];
	}
}

bool REnvironmentSceneNode::CreateFromXMLFile(const char *szFilename, bool bBackgroundCreation){ return true; }
void REnvironmentSceneNode::SaveToXMLFile(const char *szFilename){}

bool REnvironmentSceneNode::CreateFromXML(MXmlElement &element, bool bBackgroundCreation){ return true; }
void REnvironmentSceneNode::SaveToXML(MXmlElement &element){}

bool REnvironmentSceneNode::AddEnvBunch(const char* _pSzEnvBunchName)
{
	ENV_TABLE::iterator itr = m_kEnvTable.find(_pSzEnvBunchName);
	if (itr != m_kEnvTable.end())
		return false;

	ENV_BUNCH& bunch = m_kEnvTable[ _pSzEnvBunchName ];
	for ( int nTime = 0; nTime<TIME_NUM; ++nTime)
	{
		for ( int nWheather = 0; nWheather<WHEATHER_NUM; ++nWheather )
		{
			ENV_KEY key(nTime, nWheather);
			bunch[key] = new REnvironmentPiece;
		}
	}
	return true;
}

bool REnvironmentSceneNode::DeleteEnvBunch(const char* _pSzEnvBunchName)
{
	if (stricmp(_pSzEnvBunchName, "default") == 0)
	{
		_ASSERT(!"디폴트 환경묶음은 지울수 없습니다.");
		return false;
	}

	// 지우기를 원하는 EnvironmentBunch가 없는 경우
	ENV_TABLE::iterator itr = m_kEnvTable.find(_pSzEnvBunchName);
	if (itr == m_kEnvTable.end())
		return false;

	bool bNeedToReset = false;
	if (m_strCurrentEnvBunch == _pSzEnvBunchName)
		bNeedToReset = true;

	ENV_BUNCH& bunch = itr->second;
	for ( int nTime = 0; nTime<TIME_NUM; ++nTime)
	{
		for ( int nWheather = 0; nWheather<WHEATHER_NUM; ++nWheather )
		{
			ENV_KEY key(nTime, nWheather);
			delete bunch[key];
		}
	}
	m_kEnvTable.erase(itr);

	if (bNeedToReset)
		SetCurrentEnvPieceToDefault();

	return true;
}

void REnvironmentSceneNode::DeleteAllEnvBunch()
{
	// 지우기를 원하는 EnvironmentBunch가 없는 경우
	for(ENV_TABLE::iterator itr = m_kEnvTable.begin(); itr != m_kEnvTable.end(); ++itr)
	{
		ENV_BUNCH& bunch = itr->second;
		for ( int nTime = 0; nTime<TIME_NUM; ++nTime)
		{
			for ( int nWheather = 0; nWheather<WHEATHER_NUM; ++nWheather )
			{
				ENV_KEY key(nTime, nWheather);
				delete bunch[key];
			}
		}
	}
	m_kEnvTable.clear();
}

void REnvironmentSceneNode::GetAllEnvBunchesList(std::vector<std::string> &vecArray)
{
	for(ENV_TABLE::iterator itr = m_kEnvTable.begin(); itr != m_kEnvTable.end(); ++itr)
		vecArray.push_back(itr->first);
}

bool REnvironmentSceneNode::ClearAllEnvBunches()
{
	// 몽땅 지워주기
	DeleteAllEnvBunch();

	// default EnvironmentBunch 만들기
	AddEnvBunch("default");
	SetCurrentEnvPieceToDefault();
	return true;
}

REnvironmentSceneNode::REnvironmentPiece* REnvironmentSceneNode::GetEnvPiece(const char* pSzEnvBunchName, REnvironmentSceneNode::ETime eTime, REnvironmentSceneNode::EWheather eWeather)
{
	ENV_TABLE::iterator itr = m_kEnvTable.find(pSzEnvBunchName);
	if (itr == m_kEnvTable.end())
		return NULL;

	ENV_KEY key(eTime, eWeather);
	return itr->second[ key ];
}

bool REnvironmentSceneNode::SetCurrentEnvPiece(const char* pSzEnvBunchName, REnvironmentSceneNode::ETime eTime, REnvironmentSceneNode::EWheather eWeather, float fFadingTime)
{
	REnvironmentPiece* pEnvPieceToChange = GetEnvPiece(pSzEnvBunchName, eTime, eWeather);
	if (pEnvPieceToChange == NULL)
		return false;

	// TODO 뭔가 리셋할께 있다면 리셋을 해준다던가..
	// 혹은 페이드 옵션이 있으면 페이딩을 해준다던가...
	if ( m_pCurrentEnvPiece == pEnvPieceToChange )
	{
		// 현재랑 같은 경우에
	}

	m_pCurrentEnvPiece = pEnvPieceToChange;
	m_eCurrentTime = eTime;
	m_eCurrentWeather = eWeather;
	return true;
}

bool REnvironmentSceneNode::LoadWeatherFromXML(){ return true; }
void REnvironmentSceneNode::SetRenderWeather(bool bRenderWeatherEffect){}

void REnvironmentSceneNode::OnUpdate(){}
void REnvironmentSceneNode::OnRender(){}

void REnvironmentSceneNode::ResetEnvProcess(){}
void REnvironmentSceneNode::UpdateEnvProcess(){}

void REnvironmentSceneNode::RemoveQueue(){}
void REnvironmentSceneNode::RemoveCurrent(){}
void REnvironmentSceneNode::Release(){}

} // end of rs3