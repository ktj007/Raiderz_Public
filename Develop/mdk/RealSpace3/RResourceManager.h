#pragma once

#include "RPreDefine.h"
#include "RTypes.h"
#include "MHandle.h"
#include "RResource.h"
#include <map>

using namespace std;

namespace rs3 {

typedef map<std::string, RResource*> RRESOURCEHASHMAP;
class RSceneManager;

class RResourceContainerFunctor
{
public:
	void operator() (RRESOURCEHASHMAP& mapResources) {}
};

class RResourceManager;
class RS_API RResourceContainer
{
	friend RResourceManager;
public:
	void			Add(RResource* pResource);
	RResource*		Get(std::string ResourceName);
	bool			Remove(std::string ResourceName);
	void			Safe_Operation(RResourceContainerFunctor& Functor);
	void			DeepClear(void);
	bool			Empty(void);

private:
	RRESOURCEHASHMAP			m_mapResources;
	MCriticalSectionSpinLock	m_LockForResources;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// base resource manager
//
class RS_API RResourceManager
{
	friend class RSceneManager;
	friend class REngine;

public:

	RResourceManager();
	virtual ~RResourceManager();


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//
	virtual void				Init( RSceneManager* pSceneManager ){ m_pSceneManager = pSceneManager; }
	virtual void				Destroy()							{}
	virtual void				Clear()								{}


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 메모리 관리 정책
	//
	virtual int					GetUsingVideoMemory();	///< 비디오 메모리
	virtual int					GetUsingSystemMemory();	///< 시스템 메모리


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 캐쉬
	//
	void						SetCacheEnable(bool _bEnable);													///< 캐시 활성화
	void						SetCacheDuration( RResource::CACHE_PRIORITY _eCacheIndex, float _fDuration );	///< 각 캐시큐별 시간. 초.

	void						UpdateCache(float _fDelta);										///< 캐쉬를 정해진 정책에 의해 청소(Sweep)해준다.
	void						ClearCache();													///< 전체 캐쉬를 지워준다.

#ifndef MODIFY_LOADER
	const RRESOURCEHASHMAP&		GetResourceMap()	const {	return m_resources; }
#else
	const RRESOURCEHASHMAP&		GetResourceMap()	const {	return m_Resources.m_mapResources; }
#endif

protected:
	// resource map
#ifndef MODIFY_LOADER
	mutable RRESOURCEHASHMAP	m_resources;
	mutable RRESOURCEHASHMAP	m_resourcesCached[ RResource::RRCP_MAX ]; ///< Cache Queue
	cml2::MCriticalSection		m_resourceMapLock;
#else
	RResourceContainer			m_Resources;
	RResourceContainer			m_CachedResources[RResource::RRCP_MAX];
#endif


	// cache
	bool						m_bCacheEnable;
	float						m_cacheDuration[ RResource::RRCP_MAX ];
	float						m_cacheRemainedTime[ RResource::RRCP_MAX ];

	RSceneManager*				m_pSceneManager;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// resource management
	//
	RResource*					GetResource(const string &strName);				///< 관리되는 리소스 얻기
	void						AddResource(RResource* pResource);				///< 관리되는 리소스 더하기. 중복검사를 한다
	bool						RemoveResource(const string &strName);			///< 관리되는 리소스 삭제
	int							ReleaseRef( RResource* pResource );				///< RefCount만 떨어뜨려준다.

	// 캐시
	RResource*					FindFromCacheMapAndSyncToResourceMap(const string &strName);
	bool						InsertResourceToCacheMap(RResource* pResource);

	// 지우기
#ifndef MODIFY_LOADER
	void						DeepClearResourceContainer( RRESOURCEHASHMAP& _refResourceContainer );
#endif
	void						RequestDeepDeletion( RResource* _pResource, std::vector<RResource*>* pDeleteQueue );
};
//----------------------------------------------------------------------------------------------------
inline int RResourceManager::ReleaseRef( RResource* pResource )
{
	// 앞에서 이미 다 지워진듯합니다.
	_ASSERT(pResource->m_nRefCount>0);
	return --pResource->m_nRefCount; 
}
//----------------------------------------------------------------------------------------------------
inline bool RResourceManager::InsertResourceToCacheMap( RResource* pResource )
{
	_ASSERT(pResource != NULL);

	// 캐시 사용하지 않는 경우
	// 1. 캐시 사용을 매니저 단에서 안하는 경우
	if ( false == m_bCacheEnable )
		return false;
	// 2. 캐시 사용을 리소스단에서 안하는 경우
	if ( RResource::RRCP_NOT_USING == pResource->GetCachePriority() )
		return false;
	// 3. 리소스 로딩이 실패한 경우
	if (pResource->GetState() != RRS_LOADED)
		return false;

#ifndef MODIFY_LOADER
	// 캐시사용
	cml2::MCriticalSection::Guard guard(&m_resourceMapLock);

	RRESOURCEHASHMAP& resourceMap = m_resourcesCached[ pResource->GetCachePriority() ];
	_ASSERT( resourceMap.find(pResource->GetName()) == resourceMap.end() );
	resourceMap.insert( RRESOURCEHASHMAP::value_type( pResource->GetName(), pResource ) );
	return true;
#else
	m_CachedResources[pResource->GetCachePriority()].Add(pResource);
	return true;
#endif

}
//----------------------------------------------------------------------------------------------------
inline RResource* RResourceManager::FindFromCacheMapAndSyncToResourceMap( const string &strName )
{
#ifndef MODIFY_LOADER
	// 캐시 활성화가 되어있으면 캐시맵에서 찾고 캐시맵과 리소스맵을 동기화 해주기
	if (m_bCacheEnable)
	{
		for (int i = 0; i < RResource::RRCP_MAX; ++i)
		{
			RRESOURCEHASHMAP::iterator itr_cached = m_resourcesCached[i].find(strName);
			if (itr_cached != m_resourcesCached[i].end())
			{
				// 캐시맵에서 꺼내면서 캐시맵에 있는 리소스는 지워주기
				RResource* pResource = itr_cached->second;
				m_resourcesCached[i].erase(itr_cached);

				// resouce에 추가
				m_resources.insert( RRESOURCEHASHMAP::value_type( pResource->GetName(), pResource ) );
				return pResource;
			}
		}
	}

	// 캐시가 활성화 되어 있지 않거나 찾을수 없다.
	return NULL;
#else
	//리소스와 캐쉬리소스와의 동기화 필요
	RResource* pResource = NULL;
	if (m_bCacheEnable)
	{
		for (int i = 0; i < RResource::RRCP_MAX; ++i)
		{
			pResource = m_CachedResources[i].Get(strName);
			if( NULL != pResource )
			{
				m_CachedResources[i].Remove(strName);
				break;
			}
		}

		if( pResource )
		{
			m_Resources.Add(pResource);
		}
	}

	return pResource;
#endif
}

//----------------------------------------------------------------------------------------------------
inline void RResourceManager::SetCacheDuration( RResource::CACHE_PRIORITY _eCacheIndex, float _fDuration )
{
	_ASSERT( _eCacheIndex < RResource::RRCP_MAX && _eCacheIndex != 0 );
	m_cacheDuration[ _eCacheIndex ] = _fDuration;
	m_cacheRemainedTime[ _eCacheIndex ] = _fDuration;

	_ASSERT(m_cacheDuration[ RResource::RRCP_HIGH ] >= m_cacheDuration[ RResource::RRCP_MIDDLE ]);
	_ASSERT(m_cacheDuration[ RResource::RRCP_MIDDLE ] >= m_cacheDuration[ RResource::RRCP_LOW ]);
}
//----------------------------------------------------------------------------------------------------
inline void RResourceManager::SetCacheEnable(bool _bEnable)
{
	// 비활성화 할때 캐쉬 지워주기
	if (_bEnable == false)
		ClearCache();

	m_bCacheEnable = _bEnable;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// general resource manager
//
class RS_API RGeneralResourceManager : public RResourceManager
{
public:
	DECLARE_RES_MGR_ID( RRESOURCETYPE::RR_GENERAL);
};

}
