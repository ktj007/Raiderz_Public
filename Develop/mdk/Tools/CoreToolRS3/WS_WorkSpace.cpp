#include "stdafx.h"
#include "WS_WorkSpace.h"

#include "VL_ListenableView.h"
#include "C_ControlMediator.h"

//////////////////////////////////////////////////////////////////////////
// SWorkSpaceUIInfo
void SWorkSpaceUIInfo::AddRollup( int _nId, bool bExpandLayout, WORKBENCH_STATE::STATE _eRollupWB, bool bExclusiveOpen )
{
	m_vecRollups.push_back( std::pair<int, ROLLUI_EXPAND>(_nId, ROLLUI_EXPAND(bExclusiveOpen, _eRollupWB)) );
	if (bExpandLayout)
		m_vecExpandLayoutRollups.push_back(_nId);
}

int CWorkSpace::AddTab( const char* _pSzPanelName, WORKBENCH_STATE::STATE _eTabWB /*= WORKBENCH_STATE::STATE_NUM*/ )
{
	m_vecWorkSpaceUI.push_back(SWorkSpaceUIInfo());

	int nIdx = (int)m_vecWorkSpaceUI.size() - 1;
	SWorkSpaceUIInfo& info = m_vecWorkSpaceUI[nIdx];
	info.m_strSidePanel = _pSzPanelName;
	info.m_eTabWorkBenchState = _eTabWB;
	info.m_nTabIndex = nIdx;

	return nIdx;
}

//////////////////////////////////////////////////////////////////////////
// CWorkSpace
void CWorkSpace::BindWBState( WORKBENCH_STATE::STATE _eWbState, int _nTabID, int _nRollupId )
{
	_ASSERT( _nRollupId != -1 );	// 롤업이 없다

	VEC_BIND_CACHE& cached = m_mapBindCache[_eWbState];

	cached.push_back( make_pair(_nTabID, _nRollupId) );
}

void CWorkSpace::BindWorkBenchStateAndRollupFromUI(MAP_CURLISTENABLEVIEW& _rCurrRollups)
{
	m_mapWorkbenchBind.clear();
	for (MAP_BIND_CACHE::iterator itr = m_mapBindCache.begin(); itr != m_mapBindCache.end(); ++itr)
	{
		VEC_BIND_CACHE& bindCache = itr->second;
		VEC_WB_BIND binded;
		for (VEC_BIND_CACHE::iterator itr_cache = bindCache.begin(); itr_cache != bindCache.end(); ++itr_cache)
		{
			CListenableView* pListenableView = m_pControlMediator_->GetViewPtr(itr_cache->second);
			bool bRollup = false;
			for (MAP_CURLISTENABLEVIEW::iterator itr_curr = _rCurrRollups.begin(); itr_curr != _rCurrRollups.end(); ++itr_curr)
			{
				if (itr_curr->second->m_pListenableView == pListenableView)
				{
					WB_BIND bindItem;
					bindItem.nTabID = itr_cache->first;
					bindItem.pListenableView = pListenableView;
					bindItem.rollupBind = itr_curr->first;
					binded.push_back( bindItem );
					bRollup = true;
					break;
				}
			}

			if (!bRollup)
			{
				WB_BIND bindItem;
				bindItem.nTabID = itr_cache->first;
				bindItem.pListenableView = pListenableView;
				bindItem.rollupBind = ROLLUP_BIND(NULL, -1);
				binded.push_back( bindItem );
			}
		}
		
		m_mapWorkbenchBind.insert( MAP_WB_BIND::value_type(itr->first, binded) );
	}
}

void CWorkSpace::GetOpenFileFilter( std::string& _rOutFilter )
{
	std::string nullTerminator;
	nullTerminator.push_back('\0');

	_rOutFilter += GetFileExtensionDesc();
	_rOutFilter += "(";
	_rOutFilter += GetFileExtension();
	_rOutFilter += ")" + nullTerminator;
	_rOutFilter += GetFileExtension();
	_rOutFilter += nullTerminator;
}
