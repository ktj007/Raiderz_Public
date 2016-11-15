#include "StdAfx.h"
#include "RResourceManager.h"
#include "RResource.h"
#include "RResourceID.h"
#include "MDebug.h"

namespace rs3 {

//////////////////////////////////////////////////
// RResourceManager
//
RResourceManager::RResourceManager() :
	m_pSceneManager(NULL),
	m_bCacheEnable(false)
{
	// 기본 정책
	m_cacheDuration[ RResource::RRCP_ALWAYS ] = m_cacheRemainedTime[ RResource::RRCP_ALWAYS ] = 0.f;		// 영원
	m_cacheDuration[ RResource::RRCP_HIGH ]   = m_cacheRemainedTime[ RResource::RRCP_HIGH ]   = 300.f;		// 300초, 5분
	m_cacheDuration[ RResource::RRCP_MIDDLE ] = m_cacheRemainedTime[ RResource::RRCP_MIDDLE ] = 60.f;		// 60초, 1분
	m_cacheDuration[ RResource::RRCP_LOW ]    = m_cacheRemainedTime[ RResource::RRCP_LOW ]    = 10.f;		// 10초
}

RResourceManager::~RResourceManager()
{
#ifndef MODIFY_LOADER
	#ifdef _DEBUG
		_ASSERT( m_resources.empty() == true );
		for (int i = 0; i < RResource::RRCP_MAX; ++i)
			_ASSERT( m_resourcesCached[i].empty() == true);
	#endif
#else
	#ifdef _DEBUG
		_ASSERT( m_Resources.Empty() );
		for (int i = 0; i < RResource::RRCP_MAX; ++i)
			_ASSERT( m_CachedResources[i].Empty() );
	#endif
#endif

}

void RResourceManager::AddResource(RResource* pResource)
{
	_ASSERT( pResource != NULL );
	_ASSERT( GetResource( pResource->GetName() ) == NULL );

#ifndef MODIFY_LOADER
	cml2::MCriticalSection::Guard guard(&m_resourceMapLock);
	m_resources.insert(pair<std::string, RResource*>(pResource->GetName(),pResource));
#else
	m_Resources.Add(pResource);
#endif

}

// RefCount 증가는 GetResource를 호출하는 쪽에서 해준다.
RResource* RResourceManager::GetResource(const string &strName)
{
#ifndef MODIFY_LOADER
	cml2::MCriticalSection::Guard guard(&m_resourceMapLock);

	// 리소스맵에서 찾기
	RRESOURCEHASHMAP::const_iterator itr = m_resources.find(strName);
	if( itr != m_resources.end() )
		return itr->second;

	// 캐시맵에서 찾고 캐시맵과 리소스맵을 동기화 하기
	return FindFromCacheMapAndSyncToResourceMap(strName);
#else
	//리소스와 캐쉬리소스와의 동기화가 더필요
	RResource* pResource = NULL;
	pResource = m_Resources.Get(strName);
	if( NULL == pResource )
	{
		pResource = FindFromCacheMapAndSyncToResourceMap(strName);
	}
	return pResource;
#endif

}

bool RResourceManager::RemoveResource(const string &strName)
{
#ifndef MODIFY_LOADER
	cml2::MCriticalSection::Guard guard(&m_resourceMapLock);

	RRESOURCEHASHMAP::iterator itr = m_resources.find(strName);
	if(itr==m_resources.end())
		return false;

	RResource* pResource = itr->second;
	m_resources.erase(itr);

	return true;
#else
	return m_Resources.Remove(strName);
#endif
}

// void RResourceManager::SetPath(const string& strPath)	// ;으로 구분된 path를 설정한다
// {
// 	m_path.SetPath(strPath);
// }

#ifndef MODIFY_LOADER
void RResourceManager::DeepClearResourceContainer( RRESOURCEHASHMAP& _refResourceContainer )
{
	for( RRESOURCEHASHMAP::iterator itr = _refResourceContainer.begin(); itr != _refResourceContainer.end(); ++itr )
	{
		RResource* pResource = itr->second;
		if( pResource->GetLoader() )
			pResource->GetLoader()->InvalidateWork();
		delete pResource;
	}
	_refResourceContainer.clear();
}
#endif


void RResourceManager::RequestDeepDeletion( RResource* pResource, std::vector<RResource*>* pDeleteQueue )
{
	// 지움 요청일때는 RefCount 가 0이어야 한다.
	_ASSERT( pResource->GetRefCount() == 0 );

	// 지움 요청이 있을때는 Workable 상태가 아니어야한다.
	if( pResource->GetLoader() )
		pResource->GetLoader()->InvalidateWork();

	// 캐시사용여부 : 캐시를 사용하면 지우지 않고 캐시맵에 보관한다.
	bool bCaching = InsertResourceToCacheMap(pResource);

	// 캐시되지 않는것을 지우기 요청했다면 지워준다
	// 만일 캐시맵에 보관되어 있다면, 지우지 않는다.
	// 캐시맵은 보관주기가 있어서 보관기간내에 외부에서 찾지 않으면 캐시맵을 비워준다.
	// 만일 보관기간내서 외부에서 할당요청이 있다면 캐시맵에서 꺼내 리소스맵으로 이동해준다.
	if ( false == bCaching )
	{
		if (pDeleteQueue)
			pDeleteQueue->push_back(pResource);
		else
			delete pResource;
	}
}

void RResourceManager::UpdateCache(float _fDelta)
{
	_ASSERT( m_bCacheEnable == true );
	for( int i = RResource::RRCP_HIGH; i < RResource::RRCP_MAX; ++i )
	{
		m_cacheRemainedTime[i] -= _fDelta;
		if ( m_cacheRemainedTime[i] < 0 )
		{
			m_cacheRemainedTime[i] = m_cacheDuration[i];
#ifndef MODIFY_LOADER
			if (m_resourcesCached[i].empty() == false)
			{
			#ifdef _DEBUG
				const char* pSzPriority[] =  {
					"Always Cache",
					"Priority High",
					"Priority Middle",
					"Priority Low"
				};
				mlog("%s 리소스 %s 캐시(%.3f sec) 클리어 :%d개\n",
					RRESOURCETYPE::GetResoruceIDString(m_resourcesCached[i].begin()->second->GetResID()),
					pSzPriority[i],
					m_cacheDuration[i],
					(int)m_resourcesCached[i].size() );
			#endif
				DeepClearResourceContainer( m_resourcesCached[i] );
			}
#else
			m_CachedResources[i].DeepClear();
#endif
		}
	}
}

void RResourceManager::ClearCache()
{
	_ASSERT( m_bCacheEnable == true );
	for( int i = 0; i < RResource::RRCP_MAX; ++i )
	{
#ifndef MODIFY_LOADER
		DeepClearResourceContainer( m_resourcesCached[i] );
#else
		m_CachedResources[i].DeepClear();
#endif
		m_cacheRemainedTime[i] = m_cacheDuration[i];
	}
}

int RResourceManager::GetUsingVideoMemory()
{
#ifndef MODIFY_LOADER
	int nUsingVideoMemory = 0;

	for( RRESOURCEHASHMAP::iterator itr = m_resources.begin(); itr != m_resources.end(); ++itr )
		nUsingVideoMemory += itr->second->GetUsingVideoMemory();


	for (int i = 0; i <  RResource::RRCP_MAX; ++i )
		for( RRESOURCEHASHMAP::iterator itr = m_resourcesCached[i].begin(); itr != m_resourcesCached[i].end(); ++itr )
			nUsingVideoMemory += itr->second->GetUsingVideoMemory();

	return nUsingVideoMemory;
#else
	class VideoMemoryFunctor : public RResourceContainerFunctor
	{
	public:
		VideoMemoryFunctor(void):m_UsingVideoMemory(0) {}
	public:
		void operator() (RRESOURCEHASHMAP& mapResources)
		{
			for( RRESOURCEHASHMAP::iterator itr = mapResources.begin(); itr != mapResources.end(); ++itr )
				m_UsingVideoMemory += itr->second->GetUsingVideoMemory();			
		}
	public:
		int		m_UsingVideoMemory;
	};

	VideoMemoryFunctor VideoMemory;
	m_Resources.Safe_Operation(VideoMemory);
	for (int i = 0; i <  RResource::RRCP_MAX; ++i )
		m_CachedResources[i].Safe_Operation(VideoMemory);
	return VideoMemory.m_UsingVideoMemory;
#endif

}

int RResourceManager::GetUsingSystemMemory()
{
#ifndef MODIFY_LOADER
	int nUsingSystemMemory = 0;

	for( RRESOURCEHASHMAP::iterator itr = m_resources.begin(); itr != m_resources.end(); ++itr )
		nUsingSystemMemory += itr->second->GetUsingSystemMemory();


	for (int i = 0; i <  RResource::RRCP_MAX; ++i )
		for( RRESOURCEHASHMAP::iterator itr = m_resourcesCached[i].begin(); itr != m_resourcesCached[i].end(); ++itr )
			nUsingSystemMemory += itr->second->GetUsingSystemMemory();

	return nUsingSystemMemory;
#else
	class SystemMemoryFunctor : public RResourceContainerFunctor
	{
	public:
		SystemMemoryFunctor(void):m_UsingSystemMemory(0) {}
	public:
		void operator() (RRESOURCEHASHMAP& mapResources)
		{
			for( RRESOURCEHASHMAP::iterator itr = mapResources.begin(); itr != mapResources.end(); ++itr )
				m_UsingSystemMemory += itr->second->GetUsingSystemMemory();			
		}
	public:
		int		m_UsingSystemMemory;
	};

	SystemMemoryFunctor SystemMemory;
	m_Resources.Safe_Operation(SystemMemory);
	for (int i = 0; i <  RResource::RRCP_MAX; ++i )
		m_CachedResources[i].Safe_Operation(SystemMemory);
	return SystemMemory.m_UsingSystemMemory;
#endif
}


void RResourceContainer::Add(RResource* pResource )
{
	MCriticalSectionSpinLock::Guard guard(m_LockForResources);
	m_mapResources.insert(pair<std::string, RResource*>(pResource->GetName(),pResource));
}

RResource* RResourceContainer::Get( std::string ResourceName )
{
	MCriticalSectionSpinLock::Guard guard(m_LockForResources);
	RRESOURCEHASHMAP::const_iterator itr = m_mapResources.find(ResourceName);
	if( itr != m_mapResources.end() )
		return itr->second;
	return NULL;
}

bool RResourceContainer::Remove( std::string ResourceName )
{
	MCriticalSectionSpinLock::Guard guard(m_LockForResources);
	RRESOURCEHASHMAP::iterator itr = m_mapResources.find(ResourceName);
	if(itr==m_mapResources.end())
		return false;
	RResource* pResource = itr->second;
	m_mapResources.erase(itr);
	return true;
}

void RResourceContainer::Safe_Operation( RResourceContainerFunctor& Functor )
{
	MCriticalSectionSpinLock::Guard guard(m_LockForResources);
	Functor.operator()(m_mapResources);

}

void RResourceContainer::DeepClear( void )
{
	/*
	MCriticalSectionSpinLock::Guard guard(m_LockForResources);
	for( RRESOURCEHASHMAP::iterator itor = m_mapResources.begin(); itor != m_mapResources.end(); itor++ )
	{
		RResource* pResource = itor->second;
		if( pResource->GetLoader() )
			pResource->GetLoader()->InvalidateWork();
		delete pResource;
	}
	m_mapResources.clear();
	*/

	//http://dev/redmine/issues/16287와 같은 데드락이 생기지 않게,
	//m_csTicket락 -> m_LockForResources락 순으로 순서를 맞추어줌
	REngine::GetBackgroundWorker().LockTicket();
	MCriticalSectionSpinLock::Guard guard(m_LockForResources);
	for( RRESOURCEHASHMAP::iterator itor = m_mapResources.begin(); itor != m_mapResources.end(); itor++ )
	{
		RResource* pResource = itor->second;
		if( pResource->GetLoader() )
			pResource->GetLoader()->InvalidateWorkWithNoLock();
		delete pResource;
	}
	m_mapResources.clear();
	REngine::GetBackgroundWorker().UnLockTicket();
}

bool RResourceContainer::Empty( void )
{
	MCriticalSectionSpinLock::Guard guard(m_LockForResources);
	return m_mapResources.empty();
}
}
