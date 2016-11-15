#include "StdAfx.h"
#include "RMaterialResourceMgr.h"

namespace rs3 {

void RMaterialResourceMgr::Destroy()
{
	Clear();
}

void RMaterialResourceMgr::Clear()
{
#ifndef MODIFY_LOADER
	// 캐싱된 리소스 지우기
	for (int i = 0; i < RResource::RRCP_MAX; ++i)
		DeepClearResourceContainer( m_resourcesCached[i] );

	// 현재 사용중인 리소스 지우기
	DeepClearResourceContainer( m_resources );
#else
	for (int i = 0; i < RResource::RRCP_MAX; ++i)
		m_CachedResources[i].DeepClear();

	m_Resources.DeepClear();
#endif

}

}	//namespace rs3