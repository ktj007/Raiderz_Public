#pragma once

#include "RResource.h"

namespace rs3 {

////////////////////////////////////////////////////////////////////////////////////////////////////
class RS_API RResourceLoadingGroup
{
public:
	RResourceLoadingGroup();
	virtual ~RResourceLoadingGroup();

	int GetTotalResource() { return m_nTotalResource; }
	int GetLoadedResource() { return m_nLoadedResource; }

	void AddResourceForMonitoring( RResource* pResource );
	void IncrementLoadedResource( RResource* pResource );

	float GetPercent();
	void Clean()					{ m_bCleaned = true;		}	// 단순히 GetPercent 값을 0으로 만드는 역할만 한다. 주의.

	void AddLoadingList( RResource* pResource, RResource::LoadingController* pLoadingController = NULL );
	void StartLoading( bool bBackgroundLoading );

protected:
	int		m_nTotalResource;
	int		m_nLoadedResource;
	bool	m_bCleaned;

	vector<pair<RResource*, RResource::LoadingController*>>	m_vecResForBuild;

//	list<RResource*> m_listDebugResLoading;
};

}