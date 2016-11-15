#include "StdAfx.h"
#include "REngine.h"
#include "RDevice.h"
#include "RFont.h"
#include "RAABBTreeSceneNode.h"
#include "RSpotLightSceneNode.h"
#include "RTerrain.h"
#include "RLightManager.h"
#include "RActor.h"
#include "RClothNode.h"
#include "RResourceID.h"

#include "RBackgroundWorker.h"

namespace rs3 {

REngine::REngine(RDevice *pDevice)
{
	m_pBackgroundWorker = NULL;

	// 자원관리자 / pass renderer
	m_arrayResourceMgr = new RResourceManager*[ RRESOURCETYPE::RR_MAX ];
	memset( m_arrayResourceMgr, 0, sizeof( RResourceManager* ) * RRESOURCETYPE::RR_MAX );
}

REngine::~REngine()
{
	delete[]	m_arrayResourceMgr;

	_ASSERT( m_pBackgroundWorker == NULL );
	_ASSERT( RDevice::m_pDevice == NULL );
}

bool REngine::Create()
{
	_ASSERT(RDevice::m_pDevice != NULL);

	if(!RFont::Initialize(RDevice::m_pDevice))
	{
		mlog("Fail to call RFont::Initialize()\n");
		return false;
	}

	if(!RSpotLightSceneNode::Create())
	{
		mlog("Fail to call RSpotLightSceneNode::Create()\n");
		return false;
	}

	if(!RMesh::CreateVertexFormatHandles(RDevice::m_pDevice))
	{
		mlog("Fail to call RMesh::CreateVertexFormatHandles()\n");
		return false;
	}

	if(!RActor::CreateCheckShader())
	{
		mlog("Fail to call RActor::CreateCheckShader()\n");
		return false;
	}

	// 지형 Shader Processor
	RTerrain::CreatePipeProcessor( RDevice::m_pDevice->GetUsingShader() );

	return true;
}

void REngine::Destroy()
{
	// AsyncLoader Clear
	if( m_pBackgroundWorker )
	{
		m_pBackgroundWorker->Destroy();
		delete m_pBackgroundWorker;
		m_pBackgroundWorker = NULL;
	}

	RFont::Finalize();
	RSpotLightSceneNode::Destroy();
	RAABBTreeSceneNode::Destroy();
	RActor::DestroyShader();
	RClothNode::DestroyShader();
	RTerrain::DestroyPipeProcessor();

	// 자원 관리자
	_ASSERT( m_arrayResourceMgr );
	for( int i = 0; i < RRESOURCETYPE::RR_MAX; ++i )
	{
		if(m_arrayResourceMgr[ i ] != NULL )
		{
			m_arrayResourceMgr[ i ]->Destroy();
			delete m_arrayResourceMgr[ i ];
		}
	}

	RDevice::m_pDevice = NULL;
}

void REngine::Update()
{
	// 지움 예약 리소스 지우기
	if (m_resourcesToDelete.empty() == false)
	{
		for( std::vector< RResource* >::iterator itr = m_resourcesToDelete.begin(); itr != m_resourcesToDelete.end(); ++itr )
			delete *itr;
		m_resourcesToDelete.clear();
	}

	// timer
	timer.Update();

	// backgroundLoader, 어플리케이션 Update랑 연관된 작업이 있으므로 먼저해준다.
	if(m_pBackgroundWorker)
		m_pBackgroundWorker->UpdateCompleteQueue();
}

void REngine::Clear()
{
	// 리소스 지우기
	if (m_resourcesToDelete.empty() == false)
	{
		for( std::vector< RResource* >::iterator itr = m_resourcesToDelete.begin(); itr != m_resourcesToDelete.end(); ++itr )
			delete *itr;
		m_resourcesToDelete.clear();
	}

	// BackgroundWorkder
	if( m_pBackgroundWorker )
		m_pBackgroundWorker->ClearQueues();
}

void REngine::CreateBackgroundWorker()
{
	_ASSERT( m_pBackgroundWorker == NULL );
	m_pBackgroundWorker = new RBackgroundWorker;
	m_pBackgroundWorker->Init();
}

void REngine::ClearBackgroundWorker()
{
	_ASSERT( m_pBackgroundWorker != NULL );
	m_pBackgroundWorker->ClearQueues();
}

void REngine::ReleaseResource( RResource* _pResource)
{
	RResourceManager* pResourceMgr = GetResourceMgrFromResource<RRESOURCETYPE>(_pResource);
	_ASSERT(pResourceMgr);

	if(0 == pResourceMgr->ReleaseRef(_pResource))
	{
		bool bRet = pResourceMgr->RemoveResource( _pResource->GetName() );
		_ASSERT( bRet == true && "리소스 삭제에 문제가 있습니다.");

		// 지움요청. 각 리소스 매니저에 따라 캐싱될수 있다.
		pResourceMgr->RequestDeepDeletion( _pResource, NULL );
	}
}

void REngine::ReserveResourceToDelete( RResource* _pResource )
{
	_ASSERT(_pResource != NULL);
	_ASSERT( _pResource->GetRefCount() == 0 );
	RResourceManager* pResourceMgr = GetResourceMgrFromResource<RRESOURCETYPE>(_pResource);
	_ASSERT(pResourceMgr);

	// 리소스를 관리자에서 삭제
	bool bRet = pResourceMgr->RemoveResource( _pResource->GetName() );
	_ASSERT( bRet == true && "리소스 삭제에 문제가 있습니다.");

	pResourceMgr->RequestDeepDeletion( _pResource, &m_resourcesToDelete );
}

}
