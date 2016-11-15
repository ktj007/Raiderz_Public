#include "StdAfx.h"
#include "RResourceLoadingGroup.h"

namespace rs3 {

////////////////////////////////////////////////////////////////////////////////////////////////////
RResourceLoadingGroup::RResourceLoadingGroup() :
	m_nTotalResource(0),
	m_nLoadedResource(0),
	m_bCleaned( false)
{
}

RResourceLoadingGroup::~RResourceLoadingGroup()
{
	// 반드시 StartLoading()을 해야 한다.
	_ASSERT( !m_vecResForBuild.size() );
}

void RResourceLoadingGroup::AddResourceForMonitoring( RResource* pResource )
{
	pResource->SetResourceLoadingGroup( this );
//	m_listDebugResLoading.push_back( pResource );
	m_nTotalResource++;
}

void RResourceLoadingGroup::IncrementLoadedResource( RResource* pResource )
{
//	m_listDebugResLoading.remove( pResource );
	m_nLoadedResource++;
}

float RResourceLoadingGroup::GetPercent()
{
	if ( m_nTotalResource <= 0)		return 0.0f;
	if ( m_bCleaned == true)		return 0.0f;

	return ((float)m_nLoadedResource / (float)m_nTotalResource) * 100.0f;
}

void RResourceLoadingGroup::AddLoadingList( RResource* pResource, RResource::LoadingController* pLoadingController )
{
	m_vecResForBuild.push_back( pair<RResource*, RResource::LoadingController*>( pResource, pLoadingController ) );
}

void RResourceLoadingGroup::StartLoading( bool bBackgroundLoading )
{
	for( size_t i = 0; i < m_vecResForBuild.size(); ++i )
	{
		m_vecResForBuild[i].first->BuildAll( bBackgroundLoading, m_vecResForBuild[i].second );
	}
	m_vecResForBuild.clear();
}

}