#include "stdafx.h"
#include "WS_WorkSpaceManager.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperResource.h"

#include "C_ControlCursor.h"
#include "C_MintManager.h"
#include "V_MFCHelper.h"

#include "mmsystem.h"
#include "ext_lib/BHTabedReBar.h"
#include "ext_lib/RollupCtrl.h"
#include "ext_lib/CChartCtrl/ChartCtrl.h"
#include "CorePlugin.h"

#include "VL_ToolBar.h"

#include "MainFrm.h"
#include "CoreToolRS3_Doc.h"
#include "MFileSystem.h"


static CorePlugin corePlugin;

//////////////////////////////////////////////////////////////////////////
// CWorkSpaceManager
CWorkSpaceManager::CWorkSpaceManager(CControlMediator* pMediator)
: m_pToolBar(NULL)
, m_pControlMediator_( pMediator )
{
	m_ImageList.Create( 16,16, ILC_COLOR32, 1, 1);

	m_pEntityActionHelper = NULL;
	m_pObjectRenderHelper = NULL;

	m_vecMenuRelatedFileNewIds.push_back(ID_FILE_NEW_1);
	m_vecMenuRelatedFileNewIds.push_back(ID_FILE_NEW_2);
	m_vecMenuRelatedFileNewIds.push_back(ID_FILE_NEW_3);
	m_vecMenuRelatedFileNewIds.push_back(ID_FILE_NEW_4);
	m_vecMenuRelatedFileNewIds.push_back(ID_FILE_NEW_5);

	m_vecMenuToolEtcIds.push_back(ID_TOOL_ETC_1);
	m_vecMenuToolEtcIds.push_back(ID_TOOL_ETC_2);
	m_vecMenuToolEtcIds.push_back(ID_TOOL_ETC_3);
	m_vecMenuToolEtcIds.push_back(ID_TOOL_ETC_4);
	m_vecMenuToolEtcIds.push_back(ID_TOOL_ETC_5);
}

CWorkSpaceManager::~CWorkSpaceManager()
{
	_ASSERT(m_mapWorkSpace.empty());
}

void CWorkSpaceManager::RegisterCommonControls(HINSTANCE hInstance)
{
	CChartCtrl::RegisterWindowClass( hInstance );
}

bool CWorkSpaceManager::LoadPlugin( SPluginInfo& pluginInfo, const std::string& strPluginName )
{
	typedef CPluginInterface* (*PFNCREATEPLUGIN) ();

	pluginInfo.m_strModuleName = strPluginName;
	pluginInfo.m_hModule = LoadLibrary( strPluginName.c_str() );
	if( !pluginInfo.m_hModule ) 
	{
		DWORD dwError = GetLastError();
		pluginInfo.m_state = SPluginInfo::UNLOADED;

		// 플러긴 로딩 실패
		// by pok. CutScene-ReiderZ 플러그인 경우 참조관계 때문에 처음 로딩시에 실패할수 있습니다.
		//_ASSERT( FALSE );
		return false;
	}

	PFNCREATEPLUGIN pfnCreatePlugin = (PFNCREATEPLUGIN)GetProcAddress(pluginInfo.m_hModule, "CreatePlugin");
	if( !pfnCreatePlugin )
	{ 
		pluginInfo.m_state = SPluginInfo::UNLOADED;
		return false;
	}

	CPluginInterface* pluginInstance = pfnCreatePlugin();
 
	if(!pluginInstance || !pluginInstance->Initialize(this) )
	{
		pluginInfo.m_state = SPluginInfo::FAILED;
		return false;
	}

	pluginInstance->m_pWorkSpaceManager = this;
	pluginInfo.m_pPluginInterface = pluginInstance;
	RegisterCommonControls(pluginInstance->GetInstanceHandle());
	pluginInfo.m_state = SPluginInfo::LOADED;

	return true;
}

bool CWorkSpaceManager::UnloadPlugin(SPluginInfo& pluginInfo)
{
	if( pluginInfo.m_state != SPluginInfo::LOADED ) return true;

	CPluginInterface* pluginInstance = pluginInfo.m_pPluginInterface;
	pluginInstance->Finalize(this);

	FreeLibrary( pluginInfo.m_hModule );
	pluginInfo.m_hModule = NULL;

	pluginInfo.m_state = SPluginInfo::UNLOADED;

	return true;
}

bool CWorkSpaceManager::LoadPlugins()
{
	// 기본 플러긴
	corePlugin.Initialize(this);

	// 플러긴 로드
	std::vector<std::string> vecPluginNames;

	std::vector< std::string > vecPluginDebugNotFiltered;
	CMFCHelper::GetFileFromPathWithFilter(vecPluginDebugNotFiltered, "plugins/", ".dll");
	for (std::vector<std::string>::iterator itr = vecPluginDebugNotFiltered.begin(); itr != vecPluginDebugNotFiltered.end(); ++itr)
	{
		#ifdef _DEBUG
		if (itr->find("_d.dll") == std::string::npos) continue;
		#else
		if (itr->find("_d.dll") != std::string::npos) continue;
		#endif

		vecPluginNames.push_back(*itr);
	}

	m_vecPlugins.resize( vecPluginNames.size() );
	std::vector<size_t> vecRetrial;
	for( size_t i=0; i< vecPluginNames.size(); i++ )
	{
		string strLibrary = vecPluginNames[i];
		
		if ( !LoadPlugin( m_vecPlugins[i], strLibrary ) )
		{
			vecRetrial.push_back(i);
		}
	}

	std::vector<std::string> vecPluginError;

	// 의존성에 의해 로드 실패할 경우가 생기므로 다시한번 시도한다.
	for( size_t i=0; i< vecRetrial.size(); i++ )
	{
		size_t idx = vecRetrial[i];
		string strLibrary = vecPluginNames[ idx ];
		if ( !LoadPlugin( m_vecPlugins[idx], strLibrary ) )
		{
			if (ERROR_MOD_NOT_FOUND == GetLastError())
			{
				// check current directory
				char buf[MAX_PATH];
				GetCurrentDirectory(MAX_PATH, buf);
			}
			_ASSERT( FALSE );	// 플러긴 로딩 실패
			mlog3(" %s load failed \n", strLibrary.c_str() );

			vecPluginError.push_back( vecPluginNames[ idx ] );
		}
	}

	if( vecPluginError.empty() == false )
	{
		string strMessage = "Failed to load plugins : \n\r\n\r";
		for( size_t i=0; i< vecPluginError.size(); i++ )
		{
			strMessage += vecPluginError[i] + "\r\n";
		}
		MessageBox( NULL, strMessage.c_str(), "Error", MB_OK );
	}

	// workspace 초기화
	m_pCurrentWorkSpace = GetWorkSpace( CorePlugin::CWorkSpaceDefault::DEFAULT_WORKSPACE_NAME );

	return true;
}

bool CWorkSpaceManager::UnloadPlugins()
{
	for( size_t i=0; i<m_vecPlugins.size(); i++ )
	{
		UnloadPlugin( m_vecPlugins[i] );
	}

	m_vecPlugins.clear();

	// 기본플러긴 해제
	corePlugin.Finalize(this);

	return true;
}

void CWorkSpaceManager::PostToPluginCreatedMainWindow()
{
	for( size_t i=0; i<m_vecPlugins.size(); i++ )
	{
		CPluginInterface* pInterface = m_vecPlugins[i].m_pPluginInterface;
		if(pInterface)
			pInterface->OnCreateMainWindow();
	}
}

std::string CWorkSpaceManager::GetSideTabName( int _nIndex )
{
	MAP_SIDETAB::iterator itr = m_mapSideTabs.begin();
	for( ; itr!= m_mapSideTabs.end(); ++itr )
	{
		if( itr->second.first == _nIndex )
		{
			return itr->first;
		}
	}
	return "";
}

// CPaneDialog* CWorkSpaceManager::GetTabPaneDialog(int nTabIndex)
// {
// 	MAP_SIDETAB::iterator itr = m_mapSideTabs.begin();
// 	for( ; itr!= m_mapSideTabs.end(); ++itr )
// 	{
// 		if( itr->second.first == nTabIndex )
// 		{
// 			return itr->second.second;
// 		}
// 	}
// 	return NULL;
// }

CPaneDialog* CWorkSpaceManager::GetTabPaneDialog( const char* szTabName )
{
	MAP_SIDETAB::iterator itr = m_mapSideTabs.find( szTabName );
	if( itr == m_mapSideTabs.end() )
		return NULL;

	return itr->second.second;

}

int	CWorkSpaceManager::GetSideTabIndex( const char* szName )
{
	MAP_SIDETAB::iterator itr = m_mapSideTabs.find( szName );
	if( itr == m_mapSideTabs.end() )
		return -1;

	return itr->second.first;
}


void CWorkSpaceManager::RegisterSideTab( int _nIndex, CPaneDialog* _pSideTab )
{
	MAP_SIDETAB::iterator itr = m_mapSideTabs.begin();
	for( ; itr!= m_mapSideTabs.end(); ++itr )
	{
		if( itr->second.first == _nIndex )
		{
			itr->second.second = _pSideTab;
			return;
		}
	}
	_ASSERT(FALSE);	// 인덱스를 가진 탭이 없다
}

void CWorkSpaceManager::SetToolbar( CCoreToolToolBar* pToolBar )
{
	m_pToolBar = pToolBar;
}

bool CWorkSpaceManager::IsValidWorkSpaceKeyword( const std::string& _rStrKeyword )
{
	return ( m_mapWorkSpace.find(_rStrKeyword) != m_mapWorkSpace.end() );
}

void CWorkSpaceManager::ChangeWorkSpace( CRollupCtrl* _arryRollups, const std::string& _rStrKeyword )
{
	// clear side panel
	int nSideTabCount = GetSideTabCount();
	for (int i = 1; i < nSideTabCount; ++i)
	{
		_arryRollups[i].RemoveAllPages();
	}

	// clear roll up
	for (MAP_CURLISTENABLEVIEW::iterator itr = m_mapCurrentRollups.begin(); itr != m_mapCurrentRollups.end(); ++itr)
	{
		SListenableViewInfo* pInfo = itr->second;
		_arryRollups[0].InsertPage(pInfo->m_pListenableView->GetViewName(), pInfo->m_pDialog, FALSE);
	}
	m_mapCurrentRollups.clear();

	// reset side panel
	MAP_SIDETAB::iterator itr = m_mapSideTabs.begin();
	for( ; itr!= m_mapSideTabs.end(); ++itr )
	{
		itr->second.second->ShowPane( FALSE, FALSE, FALSE);
	}

	MAP_WS::iterator itr_ws = m_mapWorkSpace.find(_rStrKeyword);
	_ASSERT(itr_ws != m_mapWorkSpace.end());

	m_pCurrentWorkSpace = itr_ws->second;
	VEC_WS_UI& ws_ui = m_pCurrentWorkSpace->GetWorkSpaceUI();

	for (VEC_WS_UI::iterator itr_ws = ws_ui.begin(); itr_ws != ws_ui.end(); ++itr_ws)
	{
		SWorkSpaceUIInfo& rWsUiInfo = *itr_ws;
		std::pair<int, CPaneDialog*>& sideTab = m_mapSideTabs[ rWsUiInfo.m_strSidePanel ];
		for (SWorkSpaceUIInfo::ROLLUPS::iterator itr_rollup_info = rWsUiInfo.m_vecRollups.begin(); itr_rollup_info != rWsUiInfo.m_vecRollups.end(); ++itr_rollup_info)
		{
			MAP_LISTENABLEVIEW::iterator itr_managed_rollup = m_mapViews.find(itr_rollup_info->first);
			
			_ASSERT( itr_managed_rollup != m_mapViews.end() );	// 롤업이 없다

			SListenableViewInfo* pInfo = itr_managed_rollup->second;
			if( pInfo->m_eType != CVT_ROLLUP ) continue;

			_arryRollups[0].RemovePage(pInfo->m_pDialog);
			pInfo->m_pDialog->SetParent(&_arryRollups[ sideTab.first ]);

			pInfo->m_bExclusiveOpen = itr_rollup_info->second.first;
			pInfo->m_eWorkBenchState = itr_rollup_info->second.second;

			int nIdx = _arryRollups[ sideTab.first ].InsertPage(pInfo->m_pListenableView->GetViewName(), pInfo->m_pDialog, FALSE);
			m_mapCurrentRollups[ std::pair<CRollupCtrl*, int>(&_arryRollups[ sideTab.first ], nIdx) ] = pInfo;
		}

		CPaneDialog* pPaneDialog = sideTab.second;
		pPaneDialog->ShowPane( TRUE, FALSE, TRUE );
	}

	// bind ui
	m_pCurrentWorkSpace->BindWorkBenchStateAndRollupFromUI(m_mapCurrentRollups);

	// reset helpers
	SetHelperEntityAction( m_pCurrentWorkSpace->m_WorkspaceHelpersAndActions_.m_pEntityActionHelper );
	SetHelperObjectRenderer( m_pCurrentWorkSpace->m_WorkspaceHelpersAndActions_.m_pObjectRenderHelper );

	if( m_pToolBar )
	{
	// reset toolbar
	for (int i = 0; i < WORKBENCH_STATE::STATE_NUM; ++i)
	{
		bool bEnable = false;
		if (m_pCurrentWorkSpace->IsBindedWB(WORKBENCH_STATE::STATE(i)))
			bEnable = true;

		m_pToolBar->m_btnGroup[i].enable(bEnable);
	}
	m_pToolBar->SetEnableAxis(m_pCurrentWorkSpace->m_sToolBarButton_.m_bAxis);
	m_pToolBar->m_btnWeather.enable(m_pCurrentWorkSpace->m_sToolBarButton_.m_bWeather);
	m_pToolBar->m_btnOnTerrain.enable(m_pCurrentWorkSpace->m_sToolBarButton_.m_bOnTerrain);
	m_pToolBar->m_btnObjectLock.enable(m_pCurrentWorkSpace->m_sToolBarButton_.m_bLock);
	m_pToolBar->m_btnObjectList.enable(m_pCurrentWorkSpace->m_sToolBarButton_.m_bSceneList);
	m_pToolBar->m_btnLayerManager.enable(m_pCurrentWorkSpace->m_sToolBarButton_.m_bLayerManager);
	m_pToolBar->m_btnClip.enable(m_pCurrentWorkSpace->m_sToolBarButton_.m_bClip);

	}

	// reset menu
	m_mapMenuRelatedFunctors.clear();
	m_mapMenuToolEtcFunctors.clear();

	// file menu
	CControlPopupActionRepository* pPopupActionRepo = m_pCurrentWorkSpace->m_WorkspaceHelpersAndActions_.m_pPopupActionRepository;
	if (pPopupActionRepo)
	{
		int nSize = (int)pPopupActionRepo->m_vecResFunctor.size();
		int nMenuSize = (int)m_vecMenuRelatedFileNewIds.size();
		if (nSize > nMenuSize)
			nSize = nMenuSize;

		for (int i = 0; i < nSize; ++i)
		{
			CCoreToolFunctor* pFunctor = pPopupActionRepo->m_vecResFunctor[i];
			m_mapMenuRelatedFunctors[ m_vecMenuRelatedFileNewIds[i] ] = pFunctor;
		}
	}

	// tool menu
	if (pPopupActionRepo)
	{
		int nSize = (int)pPopupActionRepo->m_vecToolEtcFunctor.size();
		int nMenuSize = (int)m_vecMenuToolEtcIds.size();
		if (nSize > nMenuSize)
			nSize = nMenuSize;

		for (int i = 0; i < nSize; ++i)
		{
			CCoreToolFunctor* pFunctor = pPopupActionRepo->m_vecToolEtcFunctor[i];
			m_mapMenuRelatedFunctors[ m_vecMenuToolEtcIds[i] ] = pFunctor;
		}
	}


	// 초기 WorkBench State
	TriggerWBState(m_pCurrentWorkSpace->m_eDefaultWB_, true);
}

BOOL CWorkSpaceManager::OnShowPopupMenu(CMFCPopupMenu* pMenuPopup)
{
	int iIndex = -1;

	// New Resource menu
	if (!CMFCToolBar::IsCustomizeMode () &&
		(iIndex = pMenuPopup->GetMenuBar ()->CommandToIndex (ID_FILE_NEW_1)) >= 0)
	{
		// remove the {Dynamic Command} item in noncustomize mode
		pMenuPopup->RemoveItem (iIndex);

		CControlPopupActionRepository* pPopupActionRepo = m_pCurrentWorkSpace->m_WorkspaceHelpersAndActions_.m_pPopupActionRepository;
		if (pPopupActionRepo)
		{
			int nSize = (int)pPopupActionRepo->m_vecResFunctor.size();
			int nMenuSize = (int)m_vecMenuRelatedFileNewIds.size();
			if (nSize > nMenuSize)
				nSize = nMenuSize;

			for (int i = 0; i < nSize; ++i)
			{
				CCoreToolFunctor* pFunctor = pPopupActionRepo->m_vecResFunctor[i];
				m_mapMenuRelatedFunctors[ m_vecMenuRelatedFileNewIds[i] ] = pFunctor;
//				pFileMenu->InsertMenu(i+1, MF_BYPOSITION, m_vecMenuRelatedFileNewIds[i], pFunctor->GetFuncName());
				pMenuPopup->InsertItem (
					CMFCToolBarMenuButton (ID_FILE_NEW_1 + i , NULL, -1, pFunctor->GetFuncName() ), iIndex + i);
			}
		}
	}

	// Tool Etc menu
	if (!CMFCToolBar::IsCustomizeMode () &&
		(iIndex = pMenuPopup->GetMenuBar ()->CommandToIndex (ID_TOOL_ETC_1)) >= 0)
	{
		// remove the {Dynamic Command} item in noncustomize mode
		pMenuPopup->RemoveItem (iIndex);

		CControlPopupActionRepository* pPopupActionRepo = m_pCurrentWorkSpace->m_WorkspaceHelpersAndActions_.m_pPopupActionRepository;
		if (pPopupActionRepo)
		{
			int nSize = (int)pPopupActionRepo->m_vecToolEtcFunctor.size();
			int nMenuSize = (int)m_vecMenuToolEtcIds.size();
			if (nSize > nMenuSize)
				nSize = nMenuSize;

			for (int i = 0; i < nSize; ++i)
			{
				CCoreToolFunctor* pFunctor = pPopupActionRepo->m_vecToolEtcFunctor[i];
				m_mapMenuRelatedFunctors[ m_vecMenuToolEtcIds[i] ] = pFunctor;
//				pToolMenu->InsertMenu(i+5, MF_BYPOSITION, m_vecMenuToolEtcIds[i], pFunctor->GetFuncName());
				pMenuPopup->InsertItem (
					CMFCToolBarMenuButton (ID_TOOL_ETC_1 + i , NULL, -1, pFunctor->GetFuncName() ), iIndex + i);
			}

			pMenuPopup->InsertSeparator (iIndex + nSize); // insert the separator at the end
		}
	}

	return TRUE;
}

void CWorkSpaceManager::TriggerRollupExpand( CRollupCtrl* pCurrnetRollup, int nRollupIndex)
{
	MAP_CURLISTENABLEVIEW::iterator itr = m_mapCurrentRollups.find( std::pair<CRollupCtrl*, int>(pCurrnetRollup, nRollupIndex) );
	_ASSERT( itr != m_mapCurrentRollups.end() );

	SListenableViewInfo* pInfo = itr->second;
	if(pInfo->m_bExclusiveOpen)
		pCurrnetRollup->FoldAllExcept(nRollupIndex);

	WORKBENCH_STATE::STATE wbState = pInfo->m_eWorkBenchState;
	if(wbState != WORKBENCH_STATE::STATE_NUM)
		m_pToolBar->m_btnGroup[ (int) wbState].check(true);

	// notify avtive view and expand clicked
	pInfo->m_pListenableView->Listen( CoreMessage(VIEW_MESSAGE::ACTIVE_VIEW) );
	_ASSERT(m_pCurrentWorkSpace);
	m_pCurrentWorkSpace->OnRollupExpandClicked(pInfo->m_pListenableView);

	SetAfterTriggered();
}

void CWorkSpaceManager::TriggerSideTabExpand(int nTabIdx, bool bOpenTab)
{
	if (NULL == m_pCurrentWorkSpace)
		return;

	if (bOpenTab)
		ChangeTabSelection(nTabIdx);

	VEC_WS_UI& ws_ui = m_pCurrentWorkSpace->GetWorkSpaceUI();
	SWorkSpaceUIInfo& ws_ui_tab = ws_ui[nTabIdx];

	// check tool bar button
	WORKBENCH_STATE::STATE wbState = ws_ui_tab.m_eTabWorkBenchState;
	if(wbState != WORKBENCH_STATE::STATE_NUM)
	{
		m_pToolBar->m_btnGroup[ (int) wbState].check(true);
		m_pControlMediator_->SetCurrentWBState(wbState);
	}

	// check edit mode
	if (m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
	{
		m_pControlMediator_->m_mapInfo.m_bEditSceneObject = true;
	}

	bool bFirstCollapseAll = false;
	for (std::vector<int>::iterator itr = ws_ui_tab.m_vecExpandLayoutRollups.begin(); itr != ws_ui_tab.m_vecExpandLayoutRollups.end(); ++itr)
	{
		for (MAP_CURLISTENABLEVIEW::iterator itr_rollup_info = m_mapCurrentRollups.begin(); itr_rollup_info != m_mapCurrentRollups.end(); ++itr_rollup_info)
		{
			if (itr_rollup_info->second->m_pListenableView->GetViewID() == *itr)
			{
				if (bFirstCollapseAll == false)
				{
					itr_rollup_info->first.first->ExpandAllPages(FALSE);
					bFirstCollapseAll = true;
				}

				itr_rollup_info->first.first->ExpandPage(itr_rollup_info->first.second, TRUE, TRUE, TRUE);

				// notify avtive view and expand clicked
				itr_rollup_info->second->m_pListenableView->Listen( CoreMessage(VIEW_MESSAGE::ACTIVE_VIEW) );
				m_pCurrentWorkSpace->OnRollupExpandClicked(itr_rollup_info->second->m_pListenableView);

				break;
			}
		}
	}

	SetAfterTriggered();
}

void CWorkSpaceManager::TriggerSideTab( const char* _pSzTabName )
{
	if (NULL == m_pCurrentWorkSpace)
		return;

	VEC_WS_UI& ws_ui = m_pCurrentWorkSpace->GetWorkSpaceUI();
	int nSize = (int)ws_ui.size();
	for (int i = 0; i < nSize; ++i)
	{
		if (ws_ui[i].m_strSidePanel == _pSzTabName)
		{
			TriggerSideTabExpand(i, true);
			break;
		}
	}
}

int CWorkSpaceManager::GetActiveTabAt( int nTab )	///< nTab 탭이 속한 TabbedFrame 의 Active Tab을 얻는다
{
	VEC_WS_UI& ws_ui = m_pCurrentWorkSpace->GetWorkSpaceUI();
	string& strTabName = ws_ui[nTab].m_strSidePanel;

	CPaneDialog* pTabPaneDialog = GetTabPaneDialog( strTabName.c_str() );
	if( pTabPaneDialog == NULL ) return -1;

	CBaseTabbedPane* pPane = pTabPaneDialog->GetParentTabbedPane();
	CTabbedPane*	pTabbedPane = DYNAMIC_DOWNCAST (CTabbedPane, pTabPaneDialog->GetParentTabbedPane() );
	if( pTabbedPane )
	{
		CMFCBaseTabCtrl* pTabCtrl = pTabbedPane->GetUnderlyingWindow();
		int nActiveTab = pTabCtrl->GetActiveTab();

		CString cstrTabName;
		pTabCtrl->GetTabLabel( nActiveTab, cstrTabName );

		// ws_ui 에서의 tab index 를 얻어서 리턴
		for( unsigned int i=0; i<ws_ui.size(); i++)
		{
			if( strcmp( ws_ui[i].m_strSidePanel.c_str(), cstrTabName ) == 0 )
				return i;
		}
	}

	return -1;
}

void CWorkSpaceManager::ChangeTabSelection( int nChangeToTab )
{
	VEC_WS_UI& ws_ui = m_pCurrentWorkSpace->GetWorkSpaceUI();
	string& strTabName = ws_ui[nChangeToTab].m_strSidePanel;

	CPaneDialog* pTabPaneDialog = GetTabPaneDialog( strTabName.c_str() );
	if( pTabPaneDialog == NULL ) return;

	CBaseTabbedPane* pPane = pTabPaneDialog->GetParentTabbedPane();
	CTabbedPane*	pTabbedPane = DYNAMIC_DOWNCAST (CTabbedPane, pTabPaneDialog->GetParentTabbedPane() );
	if( pTabbedPane )
	{
		CMFCBaseTabCtrl* pTabCtrl = pTabbedPane->GetUnderlyingWindow();
		for( int i=0; i<pTabCtrl->GetTabsNum(); i++ )
		{
			CString cstrTabName;
			pTabCtrl->GetTabLabel( i, cstrTabName );
			if( strcmp( (LPCSTR)cstrTabName, strTabName.c_str() ) == 0 )
			{
				pTabCtrl->SetActiveTab( i );
				break;
			}
		}
	}
//	pParentFrame->ShowPane( pTabPaneDialog, TRUE, FALSE, TRUE );

}

void CWorkSpaceManager::TriggerWBState(WORKBENCH_STATE::STATE _eState, bool bCheckToolbar)
{
	if (!m_pCurrentWorkSpace)
		return;

	CWorkSpace::MAP_WB_BIND::iterator itr = m_pCurrentWorkSpace->m_mapWorkbenchBind.find(_eState);
	if (itr == m_pCurrentWorkSpace->m_mapWorkbenchBind.end())
		return;

	// check tool bar button
	if (bCheckToolbar && m_pToolBar)
		m_pToolBar->m_btnGroup[(int)_eState].check(true);

	// check edit mode
	if (m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
	{
		m_pControlMediator_->m_mapInfo.m_bEditSceneObject = true;
	}

	VEC_WB_BIND& binder = itr->second;

	// 관련된 탭(panedialog)들을 foreground 로 열어준다
	for (VEC_WB_BIND::iterator itr_rollup = binder.begin(); itr_rollup != binder.end(); ++itr_rollup)
	{
		WB_BIND& bindItem = *itr_rollup;

		int nActiveTabAt = GetActiveTabAt( bindItem.nTabID );

		// 바뀔 스테이트와 관련된 탭이 이미 선택되어있다면 안바꿔줘도 된다
		bool bNeedToChange = true;

		if( nActiveTabAt != -1 )
		{
			for (VEC_WB_BIND::iterator itr_to_find = binder.begin(); itr_to_find != binder.end(); ++itr_to_find)
			{
				if( (*itr_to_find).nTabID == nActiveTabAt )
				{
					bNeedToChange = false;
					break;
				}
			}
		}

		if(bNeedToChange)
			ChangeTabSelection( bindItem.nTabID );
	}

	bool bFirstCollapseAll = false;
	for (VEC_WB_BIND::iterator itr_rollup = binder.begin(); itr_rollup != binder.end(); ++itr_rollup)
	{
		WB_BIND& bindItem = *itr_rollup;
		CRollupCtrl* pRollupMgr = bindItem.rollupBind.first;
		if (bFirstCollapseAll == false)
		{
			if (pRollupMgr)
				pRollupMgr->ExpandAllPages(FALSE);
			bFirstCollapseAll = true;
		}

		if (pRollupMgr)
		{
			pRollupMgr->ExpandPage(bindItem.rollupBind.second, TRUE, TRUE, TRUE);
		}

		m_pControlMediator_->SetCurrentWBState(_eState);

		// notify avtive view and expand clicked
		if(bindItem.pListenableView)
		{
			bindItem.pListenableView->Listen( CoreMessage(VIEW_MESSAGE::ACTIVE_VIEW) );
			m_pCurrentWorkSpace->OnRollupExpandClicked(bindItem.pListenableView);
		}

		_ASSERT(m_pControlMediator_->GetCurrentWBState() == _eState);
	}

	SetAfterTriggered();
}

void CWorkSpaceManager::SetAfterTriggered()
{
	//마우스 상태 청소
	L3DCursor::GetInstance()->SetType(L3DCursor::NONE, false);

	// 쿼드 메뉴가 활성화상태 였다면 취소 시켜주기
	if(CMintManager::GetReference().IsActiveQuadPopup())
		CMintManager::GetReference().ActivateQuadPopup(false);

	// 결정된 사항을 가지고 설정하는 부분
	if(m_pToolBar)
		m_pToolBar->SetAxis();
}

CWorkSpace* CWorkSpaceManager::GetWorkSpace( const std::string& _rStrKeyword )
{
	MAP_WS::iterator itr = m_mapWorkSpace.find(_rStrKeyword);
	if (itr == m_mapWorkSpace.end())
		return NULL;

	return itr->second;
}

void CWorkSpaceManager::GetWorkSpaceOpenFileFilter( std::string& _rOutFilter )
{
	// 형식
	//	"존파일(*.zone.xml )\0*.zone.xml\0필드파일(*.field.xml)\0*.field.xml\0XML파일\0*.xml";

	std::string nullTerminator;
	nullTerminator.push_back('\0');

	string allTypes;

	MAP_WS::iterator itr = m_mapWorkSpace.begin();
	for( ; itr!= m_mapWorkSpace.end(); ++itr )
	{
		CWorkSpace* pWorkSpace = itr->second;

		if( pWorkSpace->GetFileExtension() == NULL) continue;
		pWorkSpace->GetOpenFileFilter( _rOutFilter );

		if( !allTypes.empty() )
			allTypes += ";";
		allTypes += pWorkSpace->GetFileExtension();
	}

	// 모든 형식 ( 앞쪽에 추가 )
	if( !allTypes.empty() )
		_rOutFilter = "모든 형식" + nullTerminator + allTypes + nullTerminator + _rOutFilter;

	// 모든 파일
	_rOutFilter += "모든 XML 파일" + nullTerminator + "*.XML" + nullTerminator;
}

CControlMediatorHelperNewMapResource** CWorkSpaceManager::GetHelperAllNewMapResources( int& _nCnt )
{
	static std::vector<CControlMediatorHelperNewMapResource*> resHelpers;
	resHelpers.clear();

	for( MAP_WS::iterator itr = m_mapWorkSpace.begin(); itr!= m_mapWorkSpace.end(); ++itr )
	{
		CWorkSpace* pWorkSpace = itr->second;
		if (pWorkSpace->m_WorkspaceHelpersAndActions_.m_pNewMapResourceHelper)
			resHelpers.push_back(pWorkSpace->m_WorkspaceHelpersAndActions_.m_pNewMapResourceHelper);
	}

	_nCnt = (int)resHelpers.size();
	if (_nCnt == 0)
		return NULL;

	return &(resHelpers[0]);
}

CControlMediatorHelperObjectRenderer** CWorkSpaceManager::GetHelperObjectRenderers( int& _nCnt )
{
	static std::vector<CControlMediatorHelperObjectRenderer*> renderHelpers;
	renderHelpers.clear();

	for( MAP_WS::iterator itr = m_mapWorkSpace.begin(); itr!= m_mapWorkSpace.end(); ++itr )
	{
		CWorkSpace* pWorkSpace = itr->second;
		if (pWorkSpace->m_WorkspaceHelpersAndActions_.m_pObjectRenderHelper)
			renderHelpers.push_back(pWorkSpace->m_WorkspaceHelpersAndActions_.m_pObjectRenderHelper);
	}

	_nCnt = (int)renderHelpers.size();
	if (_nCnt == 0)
		return NULL;

	return &(renderHelpers[0]);
}

void CWorkSpaceManager::RegisterSideTab(const char* szTabName, int nIndex)
{
	m_mapSideTabs.insert( MAP_SIDETAB::value_type(szTabName, std::pair<int, CPaneDialog*>(nIndex, NULL) ) );
}

MAP_EDIT_TYPE CWorkSpaceManager::OpenMapFileWithDoc( const char* _pSzMapName )
{
	return ((CCoreToolRS3Doc*)((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument())->OpenMapFile(_pSzMapName);
}

void CWorkSpaceManager::ChagneWorkSpaceWithMainFrame( const char* _pSzWorkpsaceName, const char* _pSzMainFrameName )
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	pMainFrame->ChangeWorkSpace(_pSzWorkpsaceName);
	if (_pSzMainFrameName)
		pMainFrame->SetWindowText(_pSzMainFrameName);
}

void CWorkSpaceManager::NotifyClickedFileNewMenu( UINT _resId )
{
	std::map< UINT, CCoreToolFunctor* >::iterator itr = m_mapMenuRelatedFunctors.find(_resId);
	if (itr != m_mapMenuRelatedFunctors.end())
	{
		bool bRet = itr->second->Run();
		if (!bRet)
			AfxMessageBox("Failed to create new!");
	}
}

void CWorkSpaceManager::NotifyClickedToolEtcMenu( UINT _resId )
{
	std::map< UINT, CCoreToolFunctor* >::iterator itr = m_mapMenuToolEtcFunctors.find(_resId);
	if (itr != m_mapMenuToolEtcFunctors.end())
	{
		bool bRet = itr->second->Run();
		if (!bRet)
			AfxMessageBox("도구관련 명령실행 실패");
	}
}

void CWorkSpaceManager::GetNewMapList( std::vector<std::string>& _rNewMapList )
{
	for( MAP_WS::iterator itr = m_mapWorkSpace.begin(); itr!= m_mapWorkSpace.end(); ++itr )
	{
		CWorkSpace* pWorkSpace = itr->second;

		CControlMediatorHelperNewMapResource* pResHelper = pWorkSpace->m_WorkspaceHelpersAndActions_.m_pNewMapResourceHelper;
		if (pResHelper && pResHelper->GetNewMapName() != NULL)
		{
			_rNewMapList.push_back(pResHelper->GetNewMapName());
		}
	}
}

void CWorkSpaceManager::NotifyClickedNewMap( const std::string& _rNewMapName )
{
	for( MAP_WS::iterator itr = m_mapWorkSpace.begin(); itr!= m_mapWorkSpace.end(); ++itr )
	{
		CWorkSpace* pWorkSpace = itr->second;

		CControlMediatorHelperNewMapResource* pResHelper = pWorkSpace->m_WorkspaceHelpersAndActions_.m_pNewMapResourceHelper;
		if (pResHelper && pResHelper->GetNewMapName() != NULL)
		{
			if (_rNewMapName == pResHelper->GetNewMapName())
			{
				pResHelper->OpenNewMapDlg();
			}
		}
	}
}

// Load Images From Data - Napalm @ NetCore2K
// BMP, GIF, JPEG, PNG and all the rest;-)
Gdiplus::Bitmap* CWorkSpaceManager::GdiplusLoadImage(LPBYTE lpData, DWORD dwSize) 
// by Napalm
{ 
	HGLOBAL hgImage;    
	IStream *isImage = NULL;    
	Gdiplus::Bitmap *pbmImage = NULL;       
	hgImage = GlobalAlloc(GMEM_MOVEABLE, dwSize);   
	if(hgImage){        
		CopyMemory(GlobalLock(hgImage), lpData, dwSize);        
		GlobalUnlock(hgImage);      
		CreateStreamOnHGlobal(hgImage, TRUE, &isImage);     
		if(isImage){            
			pbmImage = new Gdiplus::Bitmap(isImage);            
			isImage->Release();      
		}       
		GlobalFree(hgImage);    
	}       
	return pbmImage;
}// .. somewhere in your code

int CWorkSpaceManager::RegisterIconImage( const char* szFilename )
{
	MFile file;
	if( file.Open(szFilename) == false )
		return -1;

	char* pBuffer;
	size_t nBytes;
	file.ReadBufferAll( &pBuffer, &nBytes );

	Gdiplus::Bitmap* pBitmap = GdiplusLoadImage( (LPBYTE) pBuffer, nBytes );

	int nIndex = -1;
	if( pBitmap )
	{
		HBITMAP hbmpImage = NULL;
		if( pBitmap->GetHBITMAP( Gdiplus::Color::Black, &hbmpImage ) == Gdiplus::Ok )
		{
			CBitmap* bitmap = CBitmap::FromHandle( hbmpImage );
			nIndex = m_ImageList.Add( bitmap, RGB(255, 255, 255) );
		}
	}

	delete pBuffer;
	return m_ImageList.GetImageCount() -1;
}

CImageList* CWorkSpaceManager::GetImageList()
{
	return &m_ImageList;
}