#include "stdafx.h"
#include "XSceneManager.h"
#include "XDef.h"
#include "XConst.h"
#include "XProfiler.h"
#include "RShaderCompositeManager.h"
#include "RBackgroundWorker.h"

#include "MPhysX.h"
#include "MPhysX4R3/MPhysX4R3.h"
#include "XCharacter.h"

const char *RSID_XCHARACTER	= "XCHARACTER";

MImplementRTTI(XSceneManager, RSceneManager);

XSceneManager::XSceneManager()
{
	m_vecReserveRemoveToDelete.clear();
}

XSceneManager::~XSceneManager()
{

}

bool XSceneManager::Create(RDevice* pDevice)
{
	if(!RSceneManager::Create(pDevice))
		return false;

	//////////////////////////////////////////////////////////////////////////
	// 쉐이더 미리 로딩 테스트
	//
	// 릴리즈 모드에서 864 개 27.5 초 -_-;;
	//
	// 쓸수 있는 레벨이 아니라 봉인. 차후에 미리 컴파일하는 식으로 가던가 해야겠음
	//

	// PreloadActorShaders();
	return true;
}

void XSceneManager::Clear()
{
	// 백그라운드 워커에 있는 큐 비우기

	// CLEAR_QUEUE_DEBUG
	//REngine::GetBackgroundWorker().ClearQueues();

	// 씬매니저 clear
	RSceneManager::Clear();
}

void XSceneManager::Update()
{
	PFI_BLOCK_THISFUNC( 1001 );
	RSceneManager::Update();

	//////////////////////////////////////////////////////////////////////////
	for( size_t i = 0; i < m_vecReserveRemoveToDelete.size(); ++i )
	{
		XCharacter* pDeleteActor = m_vecReserveRemoveToDelete[i];

		SAFE_DELETE(pDeleteActor);
	}

	m_vecReserveRemoveToDelete.clear();
	_ASSERT( m_vecReserveRemoveToDelete.size() == 0 );
}

void XSceneManager::RenderPrimaryCamera()
{
	PFI_BLOCK_THISFUNC( 1002 );
	RSceneManager::RenderPrimaryCamera();
}

//////////////////////////////////////////////////////////////////////////
// 씬 매니저 Factory
//
RSceneNode* XSceneManager::CreateSceneNode( const char* pChar )
{
	RSceneNode* pSceneNode = NULL;

	// XCharacter
	if (_stricmp(pChar, RSID_XCHARACTER) == 0)
	{
		return new XCharacter();
	}

	if (NULL == pSceneNode)
	{
		//CPU 클로스 
		pSceneNode = RSceneManager::CreateSceneNode(pChar);
	}

	return pSceneNode;
}

RSceneNodeController* XSceneManager::CreateSceneNodeController( const char* _pSzControllerName )
{
	RSceneNodeController* pController = NULL;

	//pController = physx::CreateRPhysXController(_pSzControllerName);

	if (NULL == pController)
	{
		pController = RSceneManager::CreateSceneNodeController(_pSzControllerName);
	}

	return pController;
}

void XSceneManager::DeleteActor(XCharacter* pActor)
{
	ReserveActorToDelete(pActor);
}

void XSceneManager::ReserveActorToDelete(XCharacter* pActor)
{
	m_vecReserveRemoveToDelete.push_back(pActor);
}
