#include "StdAfx.h"
#include "RSceneManager.h"
#include "RDevice.h"
#include "RFont.h"
#include "RSpotLightSceneNode.h"
#include "RTerrain.h"
#include "RStaticMeshSceneNode.h"
#include "RActor.h"

#include "RPassRenderer.h"
#include "REffectSceneNode.h"

#include "MProfiler.h"

#include "RResourceID.h"

#include "RProfileInfo.h"
#include "mmsystem.h"

//#include "REnvironmentSceneNode.h"
#include "RShadowStrategy.h"
#include "RLiSPSMShadowStrategy.h"
#include "RPSSMShadowStrategy.h"
#include "RProjectionShadowStrategy.h"
#include "RRenderingStrategy.h"
#include "RInferredRenderingStrategy.h"
#include "RDeferredRenderingStrategy.h"
#include "RNullRenderingStrategy.h"
#include "RActorPassRenderer.h"
#include "RVertexMotionBlurPassRenderer.h"
#include "REdgePassRender.h"
#include "RPixelMotionBlurPassRenderer.h"
#include "RAccumMotionBlurPassRenderer.h"
#include "REffectPassRenderer.h"
#include "RAlphaPassRenderer.h"
#include "RSkyPassRenderer.h"
#include "RAdditivePassRenderer.h"
#include "RTerrainPassRenderer.h"
#include "RTreePassRenderer.h"
#include "RStaticMeshPassRenderer.h"
#include "RWaterPassRenderer.h"
#include "RPointLightSceneNode.h"
#include "RShaderCompositeManager.h"
#include "RListener.h"
#include "RActorNodeConstraintController.h"
#include "REffectManager.h"
#include "RDecalProjectionPassRenderer.h"
#include "RDecalPolygonPassRenderer.h"
#include "RDistortionPassRenderer.h"

#include "REnvironmentSet.h"
#include "RMemoryAlloc.h"
#include "RPostEffectEdgePassRender.h"

#include "tbb/task_scheduler_init.h"

#ifdef BVH_CAMERA
	#define USE_CULL_BY_BVH
#endif

//#define DEFAULT_RENDERING_INFERRED
#define DEFAULT_RENDERING_DEFERRED

template < class F, class S>
class value_equals
{
private:
	S second;
public:
	value_equals( const S& s) : second(s)		{ }
	bool operator() ( pair< const F, S> elem) 	{ return elem.second == second;	}
};


namespace rs3 {

MImplementRTTI(RSceneManager, RSceneNode);

RSceneManager::RSceneManager() 
: m_pBeforeResourceLoadingGroup(NULL)
, m_pCurrentResourceLoadingGroup(NULL)
, m_pPrimaryCamera(NULL)
, m_nLastUpdateFrameCount(0)
, m_pShadowStrategy( NULL )
, m_pRenderingStrategy( NULL )
, m_eFilterType(SF_NONE)
, m_pCurrentRootSceneNode( NULL)
, m_TurnOffToUpdateAndRender( false )
{
	RHeapAlloc::InitHeap();
	m_pMemoryPoolReleaser = new RMemPoolReleaser;

	// 자원관리자
	m_arrayResourceMgr = new RResourceManager*[ RRESOURCETYPE::RR_MAX ];
	memset( m_arrayResourceMgr, 0, sizeof( RResourceManager* ) * RRESOURCETYPE::RR_MAX );

	m_pShaderCompositeManager = new RShaderCompositeManager;

	m_pEffectManager = new REffectManager;

	m_bTimerUpdated = false;

	// default pass renderer
	m_arrayPassRenderer = new RPassRenderer*[ RRP_MAX ];
	memset( m_arrayPassRenderer, 0, sizeof( RPassRenderer* ) * RRP_MAX );
	m_defaultPassRenderers = new RPassRenderer[ RRP_MAX ];
}

RSceneManager::~RSceneManager()
{

	SAFE_DELETE(m_pShaderCompositeManager);
	SAFE_DELETE(m_pEffectManager);

	delete[] m_arrayResourceMgr;

	Clear();

	SAFE_DELETE(m_pMemoryPoolReleaser);
	RHeapAlloc::ReleaseHeap();
}

const char* PROF_PRI_CAM_UP = "RSceneManager::UpdateAndCull() - primary camera update";
const char* PROF_AUTO_UPDATE = "RSceneManager::UpdateAndCull() - RAutoUpdateNodeList update";
const char* PROF_UPDATE_BOUNDING_VOLUME = "RSceneManager::UpdateAndCull() - bounding volume update";
const char* PROF_PRI_CAM_CULL = "RSceneManager::UpdateAndCull() - primary camera cull";
const char* PROF_PASS_RENDERER_UP = "RSceneManager::UpdateAndCull() - pass renderer update";
const char* PROF_LIGHT_MGR_UPDATE = "RSceneManager::UpdateAndCull() - light manager update";

void RSceneManager::UpdateDeletionQueue()
{
	// 지움 예약 리소스 지우기
	if (m_resourcesToDelete.empty() == false)
	{
		for( std::vector< RResource* >::iterator itr = m_resourcesToDelete.begin(); itr != m_resourcesToDelete.end(); ++itr )
			delete *itr;

		m_resourcesToDelete.clear();
	}
}

void RSceneManager::AddResourceLoadMonitoring( RResource* pResource, bool bBackgroundLoading )
{
	_ASSERT( pResource );

	if( m_pCurrentResourceLoadingGroup && bBackgroundLoading )
		m_pCurrentResourceLoadingGroup->AddResourceForMonitoring( pResource );
}

bool RSceneManager::AddLoadingList( RResource* pResource, bool bBackgroundLoading, RResource::LoadingController* pLoadingController )
{
	_ASSERT( pResource );

	if( m_pCurrentResourceLoadingGroup && bBackgroundLoading )
		m_pCurrentResourceLoadingGroup->AddLoadingList( pResource, pLoadingController );
	else
		pResource->BuildAll( bBackgroundLoading, pLoadingController );

	return true;
}


void RSceneManager::GatherAABB( RBoundingBox* _pGatherAABB )
{
	for( RWorldSceneNodeMap::iterator itr = m_rootSceneNodeMap.begin(); itr != m_rootSceneNodeMap.end(); itr++)
	{
		RWorldSceneNode* pWorldSceneNode = *itr;
		pWorldSceneNode->GatherAABB( _pGatherAABB );
	}
}


void RSceneManager::Update( unsigned int frameCount /* = 0 */ )
{
	RPFC_THISFUNC;

	if( m_TurnOffToUpdateAndRender )
	{
		m_bTimerUpdated = false;
		return;
	}

	DWORD dwStartTime = timeGetTime();
	_ASSERT( m_bTimerUpdated == true );

	if( frameCount != 0 )
	{
		_ASSERT( m_nLastUpdateFrameCount < frameCount ); // 두번업데이트되었나요 ?
		m_nLastUpdateFrameCount = frameCount;
	}

	UpdateDeletionQueue();

	// 업데이트 해주어야 할 노드들 업데이트
	RPFC_B(PROF_AUTO_UPDATE);
	for( RWorldSceneNodeMap::iterator itr = m_rootSceneNodeMap.begin(); itr != m_rootSceneNodeMap.end(); itr++)
	{
		RWorldSceneNode* pWorldSceneNode = *itr;
		pWorldSceneNode->UpdateAutoUpdateSceneNode();
	}
	RPFC_E;

	// 바운딩 볼륨 업데이트
	RPFC_B(PROF_UPDATE_BOUNDING_VOLUME);
	for( RWorldSceneNodeMap::iterator itr = m_rootSceneNodeMap.begin(); itr != m_rootSceneNodeMap.end(); itr++)
	{
		RWorldSceneNode* pWorldSceneNode = *itr;
		pWorldSceneNode->UpdateBoundingVolumeAllSceneNodes();
	}
	RPFC_E;

	// get elapsed time for update
	float fElapsedTime = GetTimer().GetElapsedTime() / 1000.0f;

	// update pass renderer
	if(m_pPrimaryCamera)
	{
		RPFC_B(PROF_PASS_RENDERER_UP);
		for( int i = 0; i < RRP_MAX; ++i)
		{
			RPassRenderer* pPassRenderer = GetPassRenderer( (RRENDERPASS)i );
			if( pPassRenderer)
				pPassRenderer->Update( fElapsedTime );
		}
		RPFC_E;
	}

	m_bTimerUpdated = false;
	return;
}

void RSceneManager::RenderPrimaryCamera( RRenderingCallbackInterface* pRenderingCallback)
{
	if( m_TurnOffToUpdateAndRender ) return;
	if( m_pPrimaryCamera)
		RenderCamera( m_pPrimaryCamera, pRenderingCallback);
}


void RSceneManager::RenderCamera( RCameraSceneNode* pCamera, RRenderingCallbackInterface* pRenderingCallback /*= NULL*/ )
{
	TIMEPROFILE_THIS_FUNCTION;
	RPFC_THISFUNC;

	DWORD dwStartTime = timeGetTime();

	if( pCamera && GetCurrentRootSceneNode(false) )
	{
		pCamera->UpdateViewFrustrum();

		if(!m_pRenderingStrategy)
		{
#ifdef DEFAULT_RENDERING_INFERRED
			SetRealtimeRenderingMethod( RENDERING_INFERRED);
#else
 #ifdef DEFAULT_RENDERING_DEFERRED
			SetRealtimeRenderingMethod( RENDERING_DEFERRED);
 #else
			SetRealtimeRenderingMethod( RENDERING_FORWARD);
 #endif
#endif
		}

		m_pRenderingStrategy->RenderScene( pCamera, this->GetCurrentRootSceneNode(false), m_pShadowStrategy, pRenderingCallback);
	}

	DWORD dwElapsedTime = timeGetTime() - dwStartTime;
}


void RSceneManager::DisablePEEdge()
{
	RSceneManager* pSceneManager = REngine::GetSceneManagerPtr();
	RRenderingStrategy* pRenderingStrategy = pSceneManager->GetRenderingStrategy();
	RPostEffectManager* pPostEffectManager = pRenderingStrategy->GetPostEffectManager();
	pPostEffectManager->PEEdgeEnable( false );
}


void RSceneManager::EnablePEEdge()
{
	RRenderingStrategy* pRenderingStrategy = GetRenderingStrategy();
	RPostEffectManager* pPostEffectManager = pRenderingStrategy->GetPostEffectManager();
	pPostEffectManager->PEEdgeSet();
}

void RSceneManager::AddPEEdgeCount()
{
	RRenderingStrategy* pRenderingStrategy = GetRenderingStrategy();
	RPostEffectManager* pPostEffectManager = pRenderingStrategy->GetPostEffectManager();
	pPostEffectManager->AddPEEdgeCount();
}


void RSceneManager::MinusPEEdgeCount()
{
	RRenderingStrategy* pRenderingStrategy = GetRenderingStrategy();
	RPostEffectManager* pPostEffectManager = pRenderingStrategy->GetPostEffectManager();
	pPostEffectManager->MinusPEEdgeCount();
}


RCameraSceneNode* RSceneManager::SetPrimaryCamera(RCameraSceneNode *pCamera)
{
	RCameraSceneNode *pPreviousCamera = m_pPrimaryCamera;
	m_pPrimaryCamera = pCamera;

	// 이전 카메라를 Active F로 만드므로 현재 설정하려는 카메라와 현재 설정 되어 있는(이전 카메라가 될)카메라가 같은떄를 대비하여 이전 카메라의 Active상태부터 변경
	if (pPreviousCamera != NULL)
		pPreviousCamera->SetActive(false);

	if( m_pPrimaryCamera != NULL )
		m_pPrimaryCamera->SetActive(true);

	return pPreviousCamera;
}

bool RSceneManager::LoadXML(RSceneNode *pSceneNodeToLoad, MXmlElement &element, bool bBackgroundCreation,RZoneLoadingQuery* pZoneLoadingQuery)
{
	_ASSERT( pSceneNodeToLoad && "pSceneNode must not be null");

	// common
	MXmlElement* pCommon = element.FirstChildElement("COMMON");
	if (pCommon)
	{
		LoadCommonProperty(pSceneNodeToLoad,*pCommon);
	}

	// 초기 트랜스폼 업데이트
	pSceneNodeToLoad->NeedUpdateTransform();
	pSceneNodeToLoad->UpdateTransform();

	// controllers
	MXmlElement* pControllers = element.FirstChildElement("CONTROLLERS");
	if (pControllers)
	{
		MXmlElement* pControllerElement = pControllers->FirstChildElement();
		for (; pControllerElement != NULL; pControllerElement = pControllerElement->NextSiblingElement())
		{
			RSceneNodeController *pNodeController = CreateSceneNodeController(pControllerElement->Value());
			if ( pNodeController != NULL )
			{
				pNodeController->CreateFromXML( *pControllerElement );
				pSceneNodeToLoad->AddController( pNodeController );
			}
		}
	}

	// 엔진외부에서 셋팅해줄 수 있는 조작가능한 정보를 담음
	MXmlElement* pUserProperty = element.FirstChildElement("USER_PROPERTY");
	if( pUserProperty )
	{
		for (MXmlElement* pChildElement = pUserProperty->FirstChildElement() ; pChildElement != NULL; pChildElement = pChildElement->NextSiblingElement())
		{
			string strContents;
			_Contents( strContents, pChildElement );
			pSceneNodeToLoad->PushUserProperty(pChildElement->Value(), strContents);
		}
	}

	// property
	MXmlElement* pProperty = element.FirstChildElement("PROPERTY");
	if (pProperty)
	{
		if(!pSceneNodeToLoad->CreateFromXML(*pProperty, bBackgroundCreation))
			return false;
	}

	// create children
	MXmlElement* pChildSceneElement = element.FirstChildElement("SCENE");
	if ( pChildSceneElement )
	{
		MXmlElement* pChildElement = pChildSceneElement->FirstChildElement();
		for (; pChildElement != NULL; pChildElement = pChildElement->NextSiblingElement())
		{
			//OutputDebugString(pChildElement->Value());
			//OutputDebugString(" node create");
			RSceneNode *pNode = CreateSceneNode(pChildElement->Value());
			if ( pNode != NULL )
			{
				pSceneNodeToLoad->AddChild(pNode);
				pSceneNodeToLoad->AddToSceneNodeGroup(pNode);

				//OutputDebugString("Successed. addchild a node\n");

				if ( LoadXML(pNode,*pChildElement, bBackgroundCreation) == false )
				{
					pSceneNodeToLoad->RemoveChild(pNode);
					delete pNode;
					pNode = NULL;
				}
			}
			if( NULL != pZoneLoadingQuery && NULL != pNode)
			{
				pZoneLoadingQuery->Add(pNode);
			}
		} // for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement())
	}

	pSceneNodeToLoad->NeedUpdateBoundingVolume();
	pSceneNodeToLoad->UpdateBoundingVolume();

	return true;
}

bool RSceneManager::SetCurrentRootSceneNode( RWorldSceneNode* pNode)
{
	m_pCurrentRootSceneNode = pNode;

	// CommitEnvSet()을 따로 호출 해 주어 루트 씬이 바뀔 때 마다 환경 설정을 적용 시켜주어야 한다.
	if( m_pCurrentRootSceneNode->GetEnvironmentSet())
	{
		m_pCurrentRootSceneNode->GetEnvironmentSet()->CommitEnvSet();
		// 기존 클라이언트 코드와의 호환을 위해 ...
		m_pCurrentRootSceneNode->GetEnvironmentSet()->SetCurrent();
	}

	return true;
}

RWorldSceneNode* RSceneManager::GetCurrentRootSceneNode( bool bCreation)
{
	// 루트 씬이 생성 되어 있지 않으면 요청 시점에서 하나 만들어 준다.
	if(!m_pCurrentRootSceneNode && bCreation)
	{
		RWorldSceneNode* pWorldSceneNode = CreateRootSceneNode();
		SetCurrentRootSceneNode( pWorldSceneNode );
	}

	return m_pCurrentRootSceneNode;
}


RWorldSceneNode* RSceneManager::CreateRootSceneNode(void)
{
	RWorldSceneNode* pWorldSceneNode = new RWorldSceneNode;
	m_rootSceneNodeMap.insert(pWorldSceneNode);
	return pWorldSceneNode;
}

bool RSceneManager::DeleteRootSceneNode( RWorldSceneNode* pNode)
{
	_ASSERT( m_rootSceneNodeMap.end() != m_rootSceneNodeMap.find(pNode) );

	m_rootSceneNodeMap.erase(pNode);

	// 지우는 행위
	pNode->Clear();
	pNode->SetAttached(false);	//clear()후에 SetAttached(false)해야함. 왜냐하면 m_bAttached변수를 사용하는 루틴이 이안에 있음
	_ASSERT(true == pNode->GetChildren().empty() );
	SAFE_DELETE( pNode);

	if( GetCurrentRootSceneNode(false) == pNode )
	{
		SetCurrentRootSceneNode(NULL);
	}

	return true;
}

void RSceneManager::SaveXML(RSceneNode *pSceneNodeToSave, MXmlNode *pXMLNode )
{
	// 딸려있는 노드들은 저장하지 않는다
	if(pSceneNodeToSave->QueryAttribute(RSNA_DO_NOT_SAVE))
		return;

	// 붙어있는 컨트롤러를 비활성화 해줘서 변경된 부분(로컬 트랜스폼 등)을 복구 할 수 있는 기회를 준다.
	pSceneNodeToSave->EnableControllers(false);

	MXmlElement *pElement;
	if ( pSceneNodeToSave == m_pCurrentRootSceneNode)	// 루트 신 노드
	{
		pElement = new MXmlElement("ROOT");
		pXMLNode->LinkEndChild(pElement);

		char szBuffer[128];
		szBuffer[0] = '\0';
	}
	else			// 루트 신 노드 아니다.
	{
		pElement = new MXmlElement(pSceneNodeToSave->GetNodeIDString());
		pXMLNode->LinkEndChild(pElement);

		MXmlElement *pCommonElement = new MXmlElement("COMMON");
		SaveCommonProperty(pSceneNodeToSave,*pCommonElement);
		pElement->LinkEndChild(pCommonElement);

		// Controllers
		MXmlElement* pControllersElement = NULL;

		for( int i = 0; i < RSNCTRL_MAX; ++i )
		{
			RSCENENODECONTROLLERLIST* pControllers = pSceneNodeToSave->GetControllers( RSNCTRL_TYPE(i));

			for( RSCENENODECONTROLLERLIST::iterator it = pControllers->begin(); it != pControllers->end(); ++it )
			{
				RSceneNodeController* pController = *it;
				if( !pController->QueryAttribute( RCA_STICKY ) )
				{
					// 하나라도 추가할게 있으면 그때서야 만든다
					if( !pControllersElement )
					{
						pControllersElement = new MXmlElement("CONTROLLERS");
						pElement->LinkEndChild(pControllersElement);
					}

					MXmlElement *pControllerElement = new MXmlElement( pController->GetNodeIDString() );
					pControllersElement->LinkEndChild(pControllerElement);
					pController->SaveToXML(*pControllerElement);
				}
			}
		}


		// 각 SceneNode의 UserProperty
		if(pSceneNodeToSave->GetUserPropertyCount())
		{
			MXmlElement* pUserPropertyElement = new MXmlElement("USER_PROPERTY");
			for( unsigned int i=0; i<pSceneNodeToSave->GetUserPropertyCount() ; i++)
			{
				MXmlElement *pUserPropertyItemElement = new MXmlElement(pSceneNodeToSave->GetUserPropertyName(i).c_str());
				_SetContents( pUserPropertyItemElement, pSceneNodeToSave->GetUserPropertyValue(i) );
				pUserPropertyElement->LinkEndChild(pUserPropertyItemElement);
			}
			pElement->LinkEndChild(pUserPropertyElement);
		}

		// 각 SceneNode의 Property
		MXmlElement* pPropertyElement = new MXmlElement("PROPERTY");
		pSceneNodeToSave->SaveToXML(*pPropertyElement);			// 각 액터별 저장하러 들어간다.

		if ( pPropertyElement->NoChildren() )
		{
			delete pPropertyElement;	
		}
		else
		{
			pElement->LinkEndChild(pPropertyElement);
		}
	}

	if ( pSceneNodeToSave->GetChildren().empty() == false )		// 자식이 하나라도 있을 때
	{
		MXmlElement* pChildrenElement = new MXmlElement("SCENE");
		pElement->LinkEndChild(pChildrenElement);

		for (RSceneNodeList::const_iterator i = pSceneNodeToSave->GetChildren().begin(); i!=pSceneNodeToSave->GetChildren().end(); i++)
			SaveXML((*i), pChildrenElement);		// 재귀 호출
	}

	// 컨트롤러 동작 복구
	pSceneNodeToSave->EnableControllers(true);
}

bool RSceneManager::SaveSceneNode(RSceneNode *pSceneNodeToSave, const char *szFileName)
{
	MXml xml;
	MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
	xml.Doc()->LinkEndChild(pDec);

	SaveXML(pSceneNodeToSave,xml.Doc());
	return xml.SaveFile(szFileName);
}

bool RSceneManager::SaveRootSceneToXML(const char *szFileName)
{
	return SaveSceneNode( m_pCurrentRootSceneNode, szFileName );
}

bool RSceneManager::LoadCommonProperty(RSceneNode *pNode, MXmlElement &element)
{
	return pNode->LoadCommonProperty(element);
}

void RSceneManager::SaveCommonProperty(RSceneNode *pNode, MXmlElement &element)
{
	char szContents[256];

	if ( pNode->GetNodeName().length() > 0 )
		element.SetAttribute("name", pNode->GetNodeName().c_str());

	MXmlElement *pElement = new MXmlElement("POSITION");

	RVector position = pNode->GetPosition();
	RVector direction = pNode->GetDirection();
	RVector up = pNode->GetUp();
	RVector scale = pNode->GetScale();

	sprintf(szContents,"%f %f %f", position.x, position.y, position.z);
	_SetContents(pElement, szContents);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("DIRECTION");
	sprintf(szContents,"%f %f %f", direction.x, direction.y, direction.z);
	_SetContents(pElement, szContents);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("UP");
	sprintf(szContents,"%f %f %f", up.x, up.y, up.z);
	_SetContents(pElement, szContents);
	element.LinkEndChild(pElement);

	pElement = new MXmlElement("SCALE");
	sprintf(szContents,"%f %f %f", scale.x, scale.y, scale.z);
	_SetContents(pElement, szContents);
	element.LinkEndChild(pElement);
}

void RSceneManager::SetRealtimeShadowMethod(const RShadowDesc& desc)
{
	if( m_pShadowStrategy )
	{
		if( m_pShadowStrategy->GetShadowDesc() == desc )
			return;

		m_pShadowStrategy->Destroy();
		SAFE_DELETE( m_pShadowStrategy );
	}

	RSHADOWMETHOD shadowMethod = desc.eMethod;
	if ( !REngine::GetConfig().m_bUsingShader || shadowMethod == SHADOW_NONE )
	{
		m_pShadowStrategy = new RShadowStrategy();
	}
	else if ( shadowMethod == SHADOW_LISPSM )
	{
		m_pShadowStrategy = new RLispsmShadowStrategy(desc);
	}
	else if ( shadowMethod == SHADOW_PSSM )
	{
		m_pShadowStrategy = new RPSSMShadowStrategy(desc);
	}
	else if ( shadowMethod == SHADOW_ESM )
	{
		m_pShadowStrategy = new RExponentialShadowStrategy(desc);
	}
	else if ( shadowMethod == SHADOW_PROJECTION )
	{
		m_pShadowStrategy = new RProjectionShadowStrategy(desc);
	}
	else
	{
		m_pShadowStrategy = new RShadowStrategy();
	}

	m_pShadowStrategy->Init();
	REngine::BroadcastEvent(RE_SHADOW_CHANGE);
}

RSHADOWMETHOD RSceneManager::GetRealtimeShadowMethod()
{
	if( m_pShadowStrategy )
		return m_pShadowStrategy->GetShadowDesc().eMethod;

	return SHADOW_NONE;
}

void RSceneManager::SetRealtimeRenderingMethod(RRENDERINGMETHOD eMethod, DWORD dwWidth, DWORD dwHeight)
{
	if ( m_pRenderingStrategy )
	{
		SAFE_DELETE( m_pRenderingStrategy);
	}

	if( dwWidth == 0)
		dwWidth	= (DWORD)REngine::GetDevice().GetScreenWidth();
	if( dwHeight == 0)
		dwHeight = (DWORD)REngine::GetDevice().GetScreenHeight();

	if ( eMethod == RENDERING_NULL )
	{
		m_pRenderingStrategy = new RNullRenderingStrategy();
	}
	else if ( eMethod == RENDERING_INFERRED )
	{
		m_pRenderingStrategy = new RInferredRenderingStrategy();
	}
	else if ( eMethod == RENDERING_DEFERRED )
	{
		m_pRenderingStrategy = new RDeferredRenderingStrategy();
	}

	_ASSERT(m_pRenderingStrategy);
	m_pRenderingStrategy->Init( dwWidth, dwHeight);
	REngine::BroadcastEvent( RE_RENDERING_CHANGE);
}

RRENDERINGMETHOD RSceneManager::GetRealtimeRenderingMethod()
{
	if( m_pRenderingStrategy )
		return m_pRenderingStrategy->getMethod();

	return RENDERING_NULL;
}

bool RSceneManager::Create(RDevice* pDevice)
{
// 	_ASSERT( m_pShadowStrategy==NULL );
// 	SetRealTimeShadow(bRealTimeShadow);

	// 유닛테스트때는 아무것도 생성하지 않고 사용할수 있도록
	// 그림자 관련 코드는 명시적으로 호출해주어야 생성하도록 합니다.
	m_pShadowStrategy = new RShadowStrategy;

	if(!m_pRenderingStrategy)
	{
		// 외부에서 만들지 않은 경우를 대비 하여 랜더링 전략 새로 생성.
#ifdef DEFAULT_RENDERING_INFERRED
		SetRealtimeRenderingMethod( RENDERING_INFERRED);
#else
 #ifdef DEFAULT_RENDERING_DEFERRED
		SetRealtimeRenderingMethod( RENDERING_DEFERRED);
 #else
		SetRealtimeRenderingMethod( RENDERING_FORWARD);
 #endif
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// pass renderers

	// 생성하기 위해 한번씩 불렀다.
	GetPassRenderer< RActorPassRenderer >();
	GetPassRenderer< RAlphaPassRenderer >();
	GetPassRenderer< RSkyPassRenderer >();
	GetPassRenderer< RAdditivePassRenderer >();
	GetPassRenderer< REffectPassRenderer >();
	GetPassRenderer< RTerrainPassRenderer >();
	GetPassRenderer< RStaticMeshPassRenderer >();
	GetPassRenderer< RVertexMotionBlurPassRenderer >();
	GetPassRenderer< REdgePassRenderer >();
	GetPassRenderer< RPixelMotionBlurPassRenderer >();
	GetPassRenderer< RAccumMotionBlurPassRenderer >();
	GetPassRenderer< RWaterPassRenderer >();
	GetPassRenderer< RTreePassRenderer >();
	GetPassRenderer< RPostEffectEdgePassRender >();
	GetPassRenderer< RDecalProjectionPassRenderer >();
	GetPassRenderer< RDecalPolygonPassRenderer >();
	GetPassRenderer< RDistortionPassRenderer >();
	return true;
}

void RSceneManager::Destroy()	// TODO: 공유자원 해제,, static 이 나을듯.
{
	if(m_pShadowStrategy)
	{
		m_pShadowStrategy->Destroy();
		SAFE_DELETE( m_pShadowStrategy);
	}

	if(m_pRenderingStrategy)
	{
		m_pRenderingStrategy->Destroy();
		SAFE_DELETE( m_pRenderingStrategy);
	}

	for( RWorldSceneNodeMap::iterator itr = m_rootSceneNodeMap.begin(); itr != m_rootSceneNodeMap.end(); itr++)
	{
		RWorldSceneNode* pNode = *itr;
		if (pNode->GetChildren().empty() == false)
		{
			mlog("m_rootSceneNode가 비어있지 않은데 씬매니저를 지우려 합니다. Clear를 하셨는지 확인해주세요.\n");
		}
	}
	Clear();	

	if(m_pEffectManager)
		m_pEffectManager->Destroy();

	UpdateDeletionQueue();

	// 자원 관리자
	_ASSERT( m_arrayResourceMgr );
	for( int i = 0; i < RRESOURCETYPE::RR_MAX; ++i )
	{
		if(m_arrayResourceMgr[ i ] != NULL )
		{
			m_arrayResourceMgr[ i ]->Destroy();
			delete m_arrayResourceMgr[ i ];
			m_arrayResourceMgr[ i ] = NULL;
		}
	}

	// 랜더 패스 랜더러
	if(m_arrayPassRenderer)
	{
		for( int j = 0; j < RRP_MAX; ++j )
		{
			if( m_arrayPassRenderer[ j ] != NULL )
			{
				m_arrayPassRenderer[ j ]->Destroy();
				delete m_arrayPassRenderer[ j ];
				m_arrayPassRenderer[ j ] = NULL;
			}
		}
		delete[] m_arrayPassRenderer;
		m_arrayPassRenderer = NULL;
	}
	SAFE_DELETE_ARRAY( m_defaultPassRenderers );
}

bool RSceneManager::InitSharedResources()
{
	_ASSERT( REngine::GetDevicePtr() );
	_ASSERT( m_pShaderCompositeManager );
	return m_pShaderCompositeManager->Create();
}

void RSceneManager::DestroySharedResources()
{
	if( m_pShaderCompositeManager )
		m_pShaderCompositeManager->Destroy();
}

void RSceneManager::Clear()
{
	// 리소스 지우기
	if (m_resourcesToDelete.empty() == false)
	{
		for( std::vector< RResource* >::iterator itr = m_resourcesToDelete.begin(); itr != m_resourcesToDelete.end(); ++itr )
			delete *itr;
		m_resourcesToDelete.clear();
	}

	if( m_pEffectManager )
		m_pEffectManager->ClearEffectSource();

	for( RWorldSceneNodeMap::iterator itr = m_rootSceneNodeMap.begin(); itr != m_rootSceneNodeMap.end(); itr++)
	{
		RWorldSceneNode* pNode = *itr;

		pNode->Clear();
		pNode->SetAttached(false);	//clear()후에 SetAttached(false)해야함. 왜냐하면 m_bAttached변수를 사용하는 루틴이 이안에 있음
		_ASSERT(true == pNode->GetChildren().empty() );
		SAFE_DELETE( pNode);
	}
	m_rootSceneNodeMap.clear();
	m_pCurrentRootSceneNode = NULL;
}

bool RSceneManager::AddSceneNode(RSceneNode *pChildOfRoot, bool bAddToSceneNodeGroup)
{
	_ASSERT(pChildOfRoot);
	_ASSERT(pChildOfRoot->GetParent()==NULL);	// 이미 addChild 되어있는 노드입니다. 의도한바입니까 ?

	// 루트 씬이 생성 되지 않은 경우 GetCurrentRootSceneNode()에서 추가 처리를 하므로 예약 걸기 안함.
	if ( GetCurrentRootSceneNode()->AddChild(pChildOfRoot) == false )
		return false;

	if( bAddToSceneNodeGroup)
		m_pCurrentRootSceneNode->AddToSceneNodeGroup(pChildOfRoot);

	return true;
	
}

bool RSceneManager::AddSceneNode(RWorldSceneNode* pWorld, RSceneNode *pChildOfRoot, bool bAddToSceneNodeGroup)
{
	_ASSERT( pChildOfRoot->GetParent()==NULL);	// 이미 addChild 되어있는 노드입니다. 의도한바입니까 ?

	if( m_rootSceneNodeMap.find( pWorld) == m_rootSceneNodeMap.end())
	{
		return false;
	}

	if ( pWorld->AddChild(pChildOfRoot) == false )
		return false;

	if (bAddToSceneNodeGroup)
		pWorld->AddToSceneNodeGroup(pChildOfRoot);


	return true;
}

bool RSceneManager::RemoveSceneNode(RSceneNode *pChildOfRoot, bool bRemoveFromSceneNodeGroup)
{
	if ( m_pCurrentRootSceneNode->RemoveChild(pChildOfRoot) == false )
		return false;
	if (bRemoveFromSceneNodeGroup)
		m_pCurrentRootSceneNode->RemoveFromSceneNodeGroup(pChildOfRoot);
	return true;
}

bool RSceneManager::AddCameraSceneNode(RCameraSceneNode *pChildOfRoot, bool bAddToSceneNodeGroup)
{
	return AddSceneNode( pChildOfRoot, bAddToSceneNodeGroup);
}


RDirectionalLightSceneNode* RSceneManager::GetDefaultLight()
{
	if( NULL == GetCurrentRootSceneNode(false))		return  NULL;

	return GetCurrentRootSceneNode(false)->GetDirectionalLight();
}

void RSceneManager::ReleaseResource( RResource* _pResource)
{
	if (_pResource == NULL) return;

	 RResourceManager* pResourceMgr = GetResourceMgrFromID<RRESOURCETYPE>(_pResource->GetResID());
	_ASSERT(pResourceMgr);

	if(0 == pResourceMgr->ReleaseRef(_pResource))
	{
		bool bRet = pResourceMgr->RemoveResource( _pResource->GetName() );
		_ASSERT( bRet == true && "리소스 삭제에 문제가 있습니다.");

		// 지움요청. 각 리소스 매니저에 따라 캐싱될수 있다.
		pResourceMgr->RequestDeepDeletion( _pResource, NULL );
	}
}

void RSceneManager::ReserveResourceToDelete( RResource* _pResource )
{
	_ASSERT(_pResource != NULL);
	_ASSERT( _pResource->GetRefCount() == 0 );
	RResourceManager* pResourceMgr = GetResourceMgrFromID<RRESOURCETYPE>(_pResource->GetResID());
	_ASSERT(pResourceMgr);

	// 리소스를 관리자에서 삭제
	bool bRet = pResourceMgr->RemoveResource( _pResource->GetName() );
	_ASSERT( bRet == true && "리소스 삭제에 문제가 있습니다.");

	pResourceMgr->RequestDeepDeletion( _pResource, &m_resourcesToDelete );
}

bool RSceneManager::PickSceneNode( RPICKINFO& pickInfo, RWorldSceneNode* pRootNode)
{
	if( pRootNode)
		return pRootNode->Pick( pickInfo );
	else
	{
		_ASSERT( m_pCurrentRootSceneNode);
		return m_pCurrentRootSceneNode->Pick( pickInfo );
	}
}


//REffectInstance* RSceneManager::CreateEffect( const char* _pSzEffectName )
//{
//	REffectInstance* pEffectInstance = NULL;
//	if (m_pEffectMgr)
//		pEffectInstance = m_pEffectMgr->CreateEffect( _pSzEffectName );
//
//	return pEffectInstance;
//}
//
//bool RSceneManager::DeleteEffect( REffectInstance* pEffectInst )
//{
//	bool bRet = false;
//	if (m_pEffectMgr)
//		bRet = m_pEffectMgr->DeleteEffect( pEffectInst );
//
//	return bRet;
//}
//

// TODO: 클라이언트로 옮겨간다
// TODO: pActorNode -> strActorNode
void RSceneManager::AttachEffect( REffectSceneNode* pEffectInst, RSceneNode* pSceneNode, RActorNode* pActorNode , const RMatrix& _rBaseFrame, bool bIgnoreDir )
{
	_ASSERT( pEffectInst );


	//	pEffectInst->SetParent( pSceneNode );
	//	pEffectInst->SetParent( pActorNode );

	if (pSceneNode)
	{
		pSceneNode->AddChild(pEffectInst);
		if (pActorNode)
		{
			RActorNodeConstraintController* pConstraintController = new RActorNodeConstraintController;
			pConstraintController->SetTargetActorNode(pActorNode->GetNodeName());
			pConstraintController->SetIgnoreActorNodeDirection( bIgnoreDir );
			pEffectInst->AddController(pConstraintController);
		}
	}
	else
	{
		// root에 붙이기
		_ASSERT( pSceneNode == NULL );
		_ASSERT( pActorNode == NULL );	//만일 액터노드를 붙이려면 RSceneNode에 RActor를 채워주세요
		GetCurrentRootSceneNode(false)->AddChild(pEffectInst);
	}

	// 스케일이 적용된 로컬 매트릭스를 구한다.
	RMatrix matLocal;
	RMatrix matLocalScale;
	matLocalScale.SetScaleMatrix( pEffectInst->GetScale() );
	matLocalScale.MultiplyTo( pEffectInst->GetLocalTransform(), matLocal);

	pEffectInst->SetTransform( matLocal * _rBaseFrame );
	pEffectInst->UpdateTransform();
	pEffectInst->Start();
}


// RWorldSceneNode* RSceneManager::GetNodesRootSceneNode( RSceneNode* pSceneNode)
// {
// 	RSceneNode* pNode = pSceneNode;
// 	while(true)
// 	{
// 		if(!pNode)
// 			return NULL;
// 
// 		// 루트 씬들 중 대상 노드와 일치하면 그놈이다!!
// 		RWorldSceneNodeMap::iterator itr = find_if( m_rootSceneNodeMap.begin(), m_rootSceneNodeMap.end(), value_equals< int, RWorldSceneNode*>((RWorldSceneNode*)pSceneNode));
// 		if( itr != m_rootSceneNodeMap.end())
// 		{
// 			_ASSERT(false);
// 			return itr->second;
// 		}
// 
// 		// 일치 하는 놈이 없으면 그놈의 부모를 탐색
// 		pNode = pNode->m_pParent;
// 	}
// 
// 	// 속도를 더 증가 시킬 방법이 없을까...
// 	return NULL;
// }

int RSceneManager::GetSSAO()
{ 
	if(!m_pRenderingStrategy)
		return false;
	
	return m_pRenderingStrategy->IsUsingSSAO();
}

void RSceneManager::SetSSAO( int nQuality)
{
	if(!m_pRenderingStrategy)
		return;

	m_pRenderingStrategy->UseSSAO(nQuality);
} 

bool RSceneManager::IsUsingNormalMap()
{ 
	return (m_pShaderCompositeManager && m_pShaderCompositeManager->GetUseNormalMap());
}

void RSceneManager::UseNormalMap( bool b)
{
	if( m_pShaderCompositeManager)
		m_pShaderCompositeManager->SetUseNormalMap( b);
} 


RTerrain* RSceneManager::GetCurrentTerrain()
{
	if( !GetCurrentRootSceneNode(false) )	return NULL;
	return GetCurrentRootSceneNode(false)->GetTerrain();
}

RStaticMeshSceneNode* RSceneManager::GetCurrentStaticMeshSceneNode()
{
	if( !GetCurrentRootSceneNode(false) )	return NULL;
	return GetCurrentRootSceneNode(false)->GetStaticMeshSceneNode();
}


/// 총 랜더 타겟 수는 언제든지 변경 될 수 있다. 그러므로 네이밍을 Count로 하지 말고 Grade로 하자.
void RSceneManager::SetRenderTargetGrade( int nGrade)
{
	RRENDERING_MODE mode = RRM_FULL_LIGHT;

	switch( nGrade)
	{
	case 2 : 
		mode = RRM_FULL_LIGHT;
		break;
	case 1 : 
		mode = RRM_NO_SPECULAR_RIM_LIGHT;
		break;
	case 0 : 
		mode = RRM_DIFFUSE_ONLY;
		break;
	}

	GetRenderingStrategy()->SetRenderingMode(mode);
}

/// 총 랜더 타겟 수는 언제든지 변경 될 수 있다. 그러므로 네이밍을 Count로 하지 말고 Grade로 하자.
int RSceneManager::GetRenderTargetGrade()
{
	RRENDERING_MODE mode = GetRenderingStrategy()->GetRenderingMode();

	switch(mode)
	{
	case RRM_FULL_LIGHT : 
		return 2;
	case RRM_NO_SPECULAR_RIM_LIGHT : 
		return 1;
	case RRM_DIFFUSE_ONLY : 
		return 0;
	default: 
		return RRM_FULL_LIGHT;
	}
}


bool RZoneLoadingQuery::IsLoadingCompleted( void )
{
	for( TYPE_SCENENODES::iterator itor = m_SceneNodesToLoad.begin() ; itor != m_SceneNodesToLoad.end() ;  )
	{
		RSceneNode* pSceneNode = *itor;
		if( pSceneNode->IsLoadingCompleted() )
		{
			m_SceneNodesToLoad.erase(itor);
		}
		else
		{
			itor++;
		}	
	}

	if( m_SceneNodesToLoad.empty() ) return true;

	return false;
}

void RZoneLoadingQuery::Add( RSceneNode* pSceneNode )
{
	m_SceneNodesToLoad.push_back(pSceneNode);
}

}
