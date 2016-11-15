/// @file WS_Shared.h
#pragma once

#include <map>
#include <vector>
#include <deque>
#include <string>

#include "C_ControlMediator.h"
#include "C_ControlDoAction.h"

#include "M_ToolFunctor.h"
#include "MDebug.h"

#include "PluginInterface.h"
#include "WS_WorkSpace.h"

class CPaneDialog;

class CORETOOL_API CWorkSpaceManager
{
public:
								CWorkSpaceManager(CControlMediator* pMediator);
	virtual						~CWorkSpaceManager();

	void						RegisterCommonControls(HINSTANCE hInstance);
	bool						LoadPlugin( SPluginInfo& pluginInfo, const std::string& strPluginName );
	bool						UnloadPlugin( SPluginInfo& pluginInfo );
	bool						LoadPlugins();
	bool						UnloadPlugins();

	void						PostToPluginCreatedMainWindow();
	BOOL						OnShowPopupMenu(CMFCPopupMenu* pMenuPopup);

	int							GetSideTabCount(){ return (int)m_mapSideTabs.size(); }
	std::string					GetSideTabName( int _nIndex );
	int							GetSideTabIndex( const char* szName );
	void						RegisterSideTab(int _nIndex, CPaneDialog* _pSideTab);
	void						ChangeTabSelection( int nChangeToTab );
	int							GetActiveTabAt( int nTab );	///< nTab 탭이 속한 TabbedFrame 의 Active Tab을 얻는다

	void						SetToolbar( CCoreToolToolBar* pToolBar);

	MAP_LISTENABLEVIEW&			GetListenableViews(){ return m_mapViews; }

	MAP_EDIT_TYPE				OpenMapFileWithDoc(const char* _pSzMapName);
	void						ChagneWorkSpaceWithMainFrame(const char* _pSzWorkpsaceName, const char* _pSzMainFrameName);

	void						NotifyClickedFileNewMenu(UINT _resId);
	void						NotifyClickedToolEtcMenu(UINT _resId);

	bool						IsValidWorkSpaceKeyword(const std::string& _rStrKeyword);
	void						ChangeWorkSpace(CRollupCtrl* _arryRollups, const std::string& _rStrKeyword);
	CWorkSpace*					GetWorkSpace(const std::string& _rStrKeyword);
	CWorkSpace*					GetCurrentWorkSpace(){ return m_pCurrentWorkSpace; }
	void						GetWorkSpaceOpenFileFilter( std::string& _rOutFilter );
	std::deque<std::string>&	GetWorkSpaceErrorMessage(){ return m_deqWorkspaceErrorMessage_; }

	void						TriggerRollupExpand(CRollupCtrl* pCurrnetRollup, int nRollupIndex);
	void						TriggerSideTabExpand(int nTabIdx, bool bOpenTab);
	void						TriggerSideTab(const char* _pSzTabName);
	void						TriggerWBState(WORKBENCH_STATE::STATE _eState, bool bCheckToolbar);

	void						RunningWBDevAction(){ m_pCurrentWorkSpace->OnWBDevAction(); }
	void						RenderingWBHelper(){ m_pCurrentWorkSpace->OnWBHelperRender(); }
	void						RoutingWBObjectEditMouseMessage(VIEW_MESSAGE::ID message){ m_pCurrentWorkSpace->OnWBObjectEditMouseMessage(message); }
	bool						IsAlwaysPickingWhenMouseMoved(){ return m_pCurrentWorkSpace->OnCheckAlwaysPickingWhenMoused(); }

	void						GetNewMapList(std::vector<std::string>& _rNewMapList);
	void						NotifyClickedNewMap(const std::string& _rNewMapName);

	//Actions
	CDoActionObjectNew*			GetDoActionObjectNew(){ return m_pCurrentWorkSpace->m_WorkspaceHelpersAndActions_.m_pDoActionObjectNew; }
	CDoActionObjectSelect*		GetDoActionObjectSelect(){ return m_pCurrentWorkSpace->m_WorkspaceHelpersAndActions_.m_pDoActionObjectSelect; }
	CDoActionObjectMove*		GetDoActionObjectMove(){ return m_pCurrentWorkSpace->m_WorkspaceHelpersAndActions_.m_pDoActionObjectMove; }
	CDoActionObjectRotate*		GetDoActionObjectRotate(){ return m_pCurrentWorkSpace->m_WorkspaceHelpersAndActions_.m_pDoActionObjectRotate; }
	CDoActionObjectScale*		GetDoActionObjectScale(){ return m_pCurrentWorkSpace->m_WorkspaceHelpersAndActions_.m_pDoActionObjectScale; }
	CDoActionEtcPathSelect*		GetDoActionEtcPathSelect(){ return m_pCurrentWorkSpace->m_WorkspaceHelpersAndActions_.m_pDoActionEtcPathSelect; }

	// helper
	void						SetHelperEntityAction(CControlMediatorHelperEntityAction* _pEntityAction){ m_pEntityActionHelper = _pEntityAction; }
	void						SetHelperObjectRenderer(CControlMediatorHelperObjectRenderer* _pObjectRenderer){ m_pObjectRenderHelper = _pObjectRenderer; }

	CImageList*					GetImageList();	///< icon image list

	CControlMediatorHelperEntityAction*		GetHelperEntityAction(){ return m_pEntityActionHelper; }
	CControlMediatorHelperObjectRenderer*	GetHelperObjectRenderer(){ return m_pObjectRenderHelper; }
	CKeyEventHandlerHelper*					GetHelperKeyEventHandler(){ return m_pCurrentWorkSpace->m_WorkspaceHelpersAndActions_.m_pKeyEventHandlerHelper; }

	CControlMediatorHelperObjectRenderer**	GetHelperObjectRenderers(int& _nCnt);
	CControlMediatorHelperNewMapResource**	GetHelperAllNewMapResources(int& _nCnt);

	template< typename T_ListenableView >
	SListenableViewInfo*  RegisterView( HINSTANCE hResourceHandle )
	{
		T_ListenableView* pListenableView = new T_ListenableView;
		int nID = m_IDGenerator.GetNewID();
		_ASSERT(m_mapViews.find(nID) == m_mapViews.end());

		int& rID = const_cast<int&>(T_ListenableView::GetID());
		_ASSERT(rID == -1);
		rID = nID;

		m_pControlMediator_->AddView(pListenableView);
		pListenableView->m_pControlMediator_ = m_pControlMediator_;
		pListenableView->m_hResourceHandle = hResourceHandle;

		CORE_VIEW_TYPE eType = T_ListenableView::GetType();

		CDialog* pDialog = ( eType == CVT_DIALOG || eType == CVT_ROLLUP ) ? (CDialog*)pListenableView : NULL;
		SListenableViewInfo* pInfo = new SListenableViewInfo(pDialog, hResourceHandle, T_ListenableView::IDD, pListenableView, eType);
		m_mapViews.insert( std::map<int, SListenableViewInfo*>::value_type(nID, pInfo) );
		return pInfo;
	}

	template< typename T_ListenableView >
	void UnregisterView()
	{
		MAP_LISTENABLEVIEW::iterator itr = m_mapViews.find( T_ListenableView::GetID() );
		if( itr == m_mapViews.end() )
		{
			mlog3("view %d Not found\n", T_ListenableView::GetID());
			_ASSERT(FALSE);
		}

		CDialog* pDlg = itr->second->m_pDialog;
		if(pDlg)
			pDlg->DestroyWindow();
		delete itr->second->m_pListenableView;
		delete itr->second;

		m_mapViews.erase( itr );
	}

	template< typename T_WorkSpace >
	void RegisterWorkspace( )
	{
		CWorkSpace* pWorkSpace = new T_WorkSpace;

		_ASSERT(m_mapWorkSpace.find(pWorkSpace->GetWorkSpaceKeyword()) == m_mapWorkSpace.end());
		m_mapWorkSpace.insert( std::map<std::string, CWorkSpace*>::value_type(pWorkSpace->GetWorkSpaceKeyword(), pWorkSpace) );
		T_WorkSpace::WS_KEYWORD = pWorkSpace->GetWorkSpaceKeyword();

		pWorkSpace->m_pControlMediator_ = m_pControlMediator_;

		CWorkSpaceUiSetter uiSetter(pWorkSpace);
		pWorkSpace->InitWorkSpace(uiSetter, pWorkSpace->m_WorkspaceHelpersAndActions_);
	}

	template< typename T_WorkSpace >
	void UnregisterWorkspace()
	{
		MAP_WS::iterator itr = m_mapWorkSpace.find(T_WorkSpace::WS_KEYWORD);
		if( itr == m_mapWorkSpace.end() )
		{
			mlog3("Workspace %s Not found\n", T_WorkSpace::WS_KEYWORD );
			_ASSERT(FALSE);
		}

		CWorkSpace* pWorkSpace = itr->second;
		pWorkSpace->HaltWorkSpace(pWorkSpace->m_WorkspaceHelpersAndActions_);
		delete pWorkSpace;

		m_mapWorkSpace.erase( itr );
	}

	void RegisterSideTab(const char* szTabName, int nIndex);
	int RegisterIconImage( const char* szFilename );

protected:
	std::vector<SPluginInfo>	m_vecPlugins;
	std::deque<std::string>		m_deqWorkspaceErrorMessage_;

private:
	// workspace
	typedef						std::map<std::string, CWorkSpace*> MAP_WS;
	MAP_WS						m_mapWorkSpace;

	// side tab
	typedef						std::map<std::string, std::pair<int, CPaneDialog*> > MAP_SIDETAB; // 사이드탭이름, 전체에서의 인덱스, 포인터
	MAP_SIDETAB					m_mapSideTabs;

	// Listenable View
	MAP_LISTENABLEVIEW			m_mapViews;
	
	MAP_CURLISTENABLEVIEW		m_mapCurrentRollups;
	CWorkSpace*					m_pCurrentWorkSpace;

	CCoreToolToolBar*			m_pToolBar;

	CControlMediator*			m_pControlMediator_;

	ViewIDGenerator				m_IDGenerator;

	CImageList					m_ImageList;	///< icon image list


	void						SetAfterTriggered();
// 	CPaneDialog*				GetTabPaneDialog(int nTabIndex);
	CPaneDialog*				GetTabPaneDialog( const char* szTabName );

	Gdiplus::Bitmap*			GdiplusLoadImage(LPBYTE lpData, DWORD dwSize);

	CControlMediatorHelperEntityAction*		m_pEntityActionHelper;
	CControlMediatorHelperObjectRenderer*	m_pObjectRenderHelper;

	std::vector<UINT>						m_vecMenuRelatedFileNewIds;
	std::map< UINT, CCoreToolFunctor* >		m_mapMenuRelatedFunctors;

	std::vector<UINT>						m_vecMenuToolEtcIds;
	std::map< UINT, CCoreToolFunctor* >		m_mapMenuToolEtcFunctors;
};
