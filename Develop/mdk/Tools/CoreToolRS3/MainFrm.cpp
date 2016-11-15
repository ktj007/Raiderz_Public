// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "CoreToolRS3_Doc.h"

#include "MProfiler.h"
#include "MCrashDump.h"
#include "MCrashReporter.h"

#include "RConfiguration.h"
#include "RWaterPassRenderer.h"
#include "REngine.h"
#include "RShaderCompositeManager.h"
#include "RRenderingStrategy.h"
#include "RPostEffectInterface.h"
#include "RShadowStrategy.h"

#include "MainFrm.h"

#include "WS_WorkSpaceManager.h"
#include "CorePlugin.h"

#include "C_ControlMediatorHelperRender.h"
#include "C_ControlMediatorHelperResource.h"
#include "C_MintManager.h"
#include "C_XMLConfig.h"
#include "C_XMLUserConfig.h"
#include "C_StatusBarHandler.h"

#include "M_ToolCmdMgr.h"
#include "M_ToolStaticMeshSceneNode.h"

#include "CoreToolRS3_View.h"
#include "V_MFCHelper.h"
#include "VD_DlgCommonFileOpen.h"
#include "VD_DlgNewMapTypeSelector.h"
#include "VR_EtcMapView.h"
#include "VL_ToolBar.h"
#include "VL_StatusBar.h"
#include "VL_ResourceInfo.h"
#include "VL_SceneNodeList.h"
#include "VD_DlgExportObj.h"
#include "VG_DefaultLight.h"

#include "C_RubyManager.h"
#include "ext_lib/RollupCtrl.h"
#include "ext_lib/WM_USER.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_SIDE_TABS		30

//IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_ACTIVATEAPP()
	ON_WM_SIZE()
	ON_WM_DESTROY()

// 	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
//	ON_COMMAND(ID_TIPS_OF_DAY, OnShowTipOfTheDay)

// TODO: 구현
// 	ON_MESSAGE( WM_USER_REBAR_EXPANDED, OnRebarExpaned)
 	ON_MESSAGE( WM_USER_SIDEPANEL_EXPANDED, OnSidePanelExpaned)

	//////////////////////////////////////////////////////////////////////////
	//파일
	//ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	//ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	// 파일
	ON_COMMAND(ID_FILE_NEW_0, &CMainFrame::OnFileNew0)
	ON_COMMAND(ID_FILE_NEW_1, &CMainFrame::OnFileNew1)
	ON_COMMAND(ID_FILE_NEW_2, &CMainFrame::OnFileNew2)
	ON_COMMAND(ID_FILE_NEW_3, &CMainFrame::OnFileNew3)
	ON_COMMAND(ID_FILE_NEW_4, &CMainFrame::OnFileNew4)
	ON_COMMAND(ID_FILE_NEW_5, &CMainFrame::OnFileNew5)

	ON_COMMAND(ID_FILE_OPENALL, OnFileOpenall)
	ON_COMMAND(ID_FILE_SAVEALL, OnFileSaveall)
	ON_COMMAND(ID_FILE_SAVE_AS_ALL, OnFileSaveAsAll)
	ON_COMMAND(ID_FILE_IMPORT, OnObjImport)
	ON_COMMAND(ID_FILE_EXPORT, OnObjExport)
	ON_COMMAND(ID_IMPORT_SCENE, OnImportScene)
	ON_COMMAND(ID_SETAUTOANIMATION, OnSetAutoAnimationProperty)
	ON_COMMAND(ID_EXPORT_SCENE, OnExportScene)

	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEALL, OnUpdateCmdUI) //로드되야 세이브를 합니다.
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS_ALL, OnUpdateCmdUI) //로드되야 세이브를 합니다.
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, OnUpdateCmdUI)

	//////////////////////////////////////////////////////////////////////////
	//편집
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_RESET, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_RESOURCE_REFRESH, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_RELAX_ALL, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_DIRECTIONAL_LIGHT_EDIT, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_WATER_PLANE_EDIT, OnUpdateCmdUI)
	ON_COMMAND(ID_SELECT_NONE, &CMainFrame::OnSelectNone)

	//////////////////////////////////////////////////////////////////////////
	//보기
	ON_COMMAND(ID_VIEW_WB_INFO_PANEL, OnWBInfoView)
	ON_COMMAND(ID_VIEW_PROFILE, OnViewProfileInfo)
	ON_COMMAND(ID_VIEW_TERRAIN, OnViewTerrainInfo)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TERRAIN, OnUpdateViewTerrainInfo)
	ON_COMMAND(ID_VIEW_ALPHA_SORTING, OnViewAlphaSorting)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ALPHA_SORTING, OnUpdateViewAlphaSorting)
	ON_COMMAND(ID_VIEW_LIGHTING, OnViewLighting)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIGHTING, OnUpdateViewLighting)
	ON_COMMAND(ID_VIEW_STATIC_RENDER_CELL, OnViewStaticRenderCell)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATIC_RENDER_CELL, OnUpdateViewStaticRenderCell)
	ON_COMMAND(ID_VIEW_STATIC_RENDER_ALL, OnViewStaticRenderAll)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATIC_RENDER_ALL, OnUpdateViewStaticRenderAll)
	ON_COMMAND(ID_VIEW_HDR, OnViewHDR)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HDR, OnUpdateViewHDR)
	ON_COMMAND(ID_VIEW_HDR_RENDER_TARGET, OnViewHDRRenderTarget)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HDR_RENDER_TARGET, OnUpdateViewHDRRenderTarget)
	ON_COMMAND(ID_VIEW_DOF, &CMainFrame::OnViewDof)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DOF, &CMainFrame::OnUpdateViewDof)
	ON_COMMAND(ID_VIEW_SCREEN_MOTION_BLUR, &CMainFrame::OnViewScreenMotionBlur)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SCREEN_MOTION_BLUR, &CMainFrame::OnUpdateViewScreenMotionBlur)
	ON_COMMAND(ID_VIEW_CLIPPING, &CMainFrame::OnViewClipping)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CLIPPING, &CMainFrame::OnUpdateViewClipping)
	ON_COMMAND(ID_VIEW_SSAO, &CMainFrame::OnViewSsao)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SSAO, &CMainFrame::OnUpdateViewSsao)
	ON_COMMAND(ID_TOOL_ETC_1, &CMainFrame::OnToolEtc1)
	ON_COMMAND(ID_TOOL_ETC_2, &CMainFrame::OnToolEtc2)
	ON_COMMAND(ID_TOOL_ETC_3, &CMainFrame::OnToolEtc3)
	ON_COMMAND(ID_TOOL_ETC_4, &CMainFrame::OnToolEtc4)
	ON_COMMAND(ID_TOOL_ETC_5, &CMainFrame::OnToolEtc5)
	ON_COMMAND(ID_VIEW_DISTORTION, &CMainFrame::OnViewDistortion)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DISTORTION, &CMainFrame::OnUpdateViewDistortion)
	ON_COMMAND(ID_VIEW_NORMALMAP, &CMainFrame::OnViewNormalMap)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NORMALMAP, &CMainFrame::OnUpdateViewNormalMap)
	ON_COMMAND(ID_VIEW_GAMMA_CORRECT, &CMainFrame::OnViewGammaCorrect)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GAMMA_CORRECT, &CMainFrame::OnUpdateGammaCorrect)
	ON_COMMAND(ID_VIEW_FXAA, &CMainFrame::OnViewFXAA)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FXAA, &CMainFrame::OnUpdateFXAA)
	ON_COMMAND(ID_VIEW_SHADOW, &CMainFrame::OnViewShadow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHADOW, &CMainFrame::OnUpdateViewShadow)
	ON_COMMAND_RANGE(ID_GRIDSIZE_NONE, ID_GRIDSIZE_50, &CMainFrame::OnSnapToGridSize)
	ON_UPDATE_COMMAND_UI_RANGE(ID_GRIDSIZE_NONE, ID_GRIDSIZE_50, &CMainFrame::OnUpdateUISnapToGridSize)
	ON_COMMAND_RANGE(ID_ANGLESNAP_NONE, ID_ANGLESNAP_45, &CMainFrame::OnAngleSnapSize)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ANGLESNAP_NONE, ID_ANGLESNAP_45, &CMainFrame::OnUpdateUIAngleSnapSize)

	ON_COMMAND_RANGE(ID_SIDE_PANEL_BASE, ID_SIDE_PANEL_BASE + MAX_SIDE_TABS, &CMainFrame::OnViewPane)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SIDE_PANEL_BASE, ID_SIDE_PANEL_BASE + MAX_SIDE_TABS, &CMainFrame::OnUpdateUIViewPane)

	ON_COMMAND(ID_VIEW_DEFAULT_LIGHT, &CMainFrame::OnViewDefaultLight)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEFAULT_LIGHT, &CMainFrame::OnUpdateViewDefaultLight)
	ON_COMMAND(ID_VIEW_DEFAULT_LIGHT_TO_CAMERA, &CMainFrame::OnViewDefaultLightToCamera)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEFAULT_LIGHT_TO_CAMERA, &CMainFrame::OnUpdateViewDefaultLight)

	//////////////////////////////////////////////////////////////////////////
	//Tool
	ON_COMMAND(ID_TOOLS_MAP_RES_MGR, OnToolsMapResMgr)
	ON_COMMAND(ID_TOOLS_OPTION, OnToolsOption)
	ON_COMMAND(ID_TOOLS_PAUSE, OnPause)
	ON_COMMAND(ID_TOOLS_SHORTCUT, OnToolsShortCut)

	ON_COMMAND(ID_XML_ZONE, OnToolsXmlZone)
	ON_COMMAND(ID_XML_FIELD, OnToolsXmlField)
	ON_COMMAND(ID_XML_NPC, OnToolsXmlNpc)
	ON_COMMAND(ID_XML_PROPERTY, OnToolsXmlProperty)

	ON_COMMAND(ID_TOOLS_OPEN_DATA, OnToolsDataFolderOpen)
	ON_COMMAND(ID_TOOLS_MAP_BATCH, OnToolsMapBatchScriptOpen)

	ON_COMMAND(ID_VIEW_OCCLUDER, &CMainFrame::OnViewOccluder)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OCCLUDER, &CMainFrame::OnUpdateViewOccluder)
	ON_COMMAND(ID_VIEW_ANISOTROPY, &CMainFrame::OnViewAnisotropy)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ANISOTROPY, &CMainFrame::OnUpdateViewAnisotropy)
	END_MESSAGE_MAP()

// 상태 줄 표시기
static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

class CoreToolCrashReporter : public MCrashReporter
{
public:
	CoreToolCrashReporter(const std::string &strAppName, const std::string &strTargetHost, const std::string &strID, const std::string &strPassword)
	: MCrashReporter(strAppName, strTargetHost, 21, strID, strPassword)
	{
	}

	void Report(const std::string &strLocalDumpName) const
	{
		std::string remote_prefix = GetRemotePrefix();
		std::string local_mlog = "mlog.txt";
		std::string local_dump = strLocalDumpName;
		std::string remote_mlog = remote_prefix + "_mlog.txt";
		std::string remote_dump = remote_prefix + ".dmp";
	 
		std::vector<MCrashReportFilePair> vec;
		vec.push_back(MCrashReportFilePair(local_mlog, remote_mlog));
		vec.push_back(MCrashReportFilePair(local_dump, remote_dump));
	 
		MCrashReporter::Report(vec);
	};
};

static void CrashDumpHandler(const char* szFileName)
{
	std::string& dumpPath = CMainFrame::GetDumpPathString();
	if (dumpPath.empty())
	{
		CoreToolCrashReporter Reporter("CoreTool", "icecream", "raiderz", "ihatebug");
		Reporter.Report(szFileName);
	}
	else
	{
		// tokenize by ';'
		vector<string> vecDumpPathToken;
		string::size_type nStart = 0;
		while (true)
		{
			string::size_type nEnd = dumpPath.find_first_of(';', nStart);
			if (nEnd == string::npos)
			{
				vecDumpPathToken.push_back(dumpPath.substr(nStart));
				break;
			}
			else
			{
				vecDumpPathToken.push_back(dumpPath.substr(nStart, nEnd-nStart));
				nStart = dumpPath.find_first_not_of(';', nEnd);
			}
		}

		CoreToolCrashReporter Reporter(vecDumpPathToken[0], vecDumpPathToken[1], vecDumpPathToken[2], vecDumpPathToken[3] );
		Reporter.Report(szFileName);
	}
}

// CMainFrame 생성/소멸
CMainFrame::CMainFrame()
: m_pToolBar_(NULL)
, m_pStatusBar_(NULL)
, m_nSnapGridSize( 0 )
, m_nAngleSnapSize( 0 )
, m_bHDREnable(false)
, m_bHDRViewRenderTargetEnable(false)
{
	m_pControlMediator_ = new CControlMediator;
	m_pWorkSpaceManager = m_pControlMediator_->GetWorkSpaceMgr();
	m_bMainFrameActived_ = false;
	m_nTabCount = 0;

	ilInit();
	iluInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

 	// 릴리즈일 경우 크래쉬 덤프 남기기
	 #ifndef _DEBUG
	 	MCrashDump::Init(NULL, NULL, false, &CrashDumpHandler);
	 #endif

	InitLog();
}

CMainFrame::~CMainFrame()
{
	mlog("Mainframe dtor\n");
}

void CMainFrame::OnUpdateCmdUI(CCmdUI* pCmdUI)
{
	const UINT cmd_id = pCmdUI->m_nID;

	if(   (cmd_id ==  ID_RELAX_ALL 
		|| cmd_id == ID_RESOURCE_REFRESH 
		|| cmd_id == ID_DIRECTIONAL_LIGHT_EDIT
		|| cmd_id == ID_WATER_PLANE_EDIT )
		&& !m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(m_pControlMediator_->m_mapInfo.m_bLoaded ? TRUE : FALSE);
	}
}

string CMainFrame::m_errorString;
string CMainFrame::m_dumpPathString;

MLogger* CMainFrame::CoreToolRS3MainLogger = NULL;

void CMainFrame::InitLog()
{
	CoreToolRS3MainLogger = MInitLog(MLOG_LEVEL_DEBUG, MLOG_FILE | MLOG_DEBUG | MLOG_PROG, MLogOutputCallback, "mlog.txt");
	MAddLogFilter("tool");
}

// mlog Callback
void CMainFrame::MLogOutputCallback(const char* szText, MLogLevel nLevel)
{
	if ( nLevel == MLOG_LEVEL_ERROR)
	{
		m_errorString += szText;
		m_errorString += '\n';
	}

	if(AfxGetMainWnd())
		((CMainFrame*)AfxGetMainWnd())->OutputText(szText);
}

string& CMainFrame::GetErrorString()
{
	return m_errorString;
}

string& CMainFrame::GetDumpPathString()
{
	return m_dumpPathString;
}

void CMainFrame::ClearErrorString()
{
	m_errorString.clear();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// 부모
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableDocking(CBRS_ALIGN_ANY);

	// Create menu bar:
	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	// do not allow modification of the dummy item in customize mode
	CList<UINT, UINT> lstProtectedCmds;
	lstProtectedCmds.AddTail (ID_FILE_NEW_1);
	lstProtectedCmds.AddTail (ID_TOOL_ETC_1);
	CMFCToolBarButton::SetProtectedCommands (lstProtectedCmds);
	//-----------------------------------------//


	// view
	return CreateViews_();
}

// CMainFrame 진단
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

class CPaneDialogCoreToolEx : public CPaneDialog
{
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
	{
		bool bUpdateRollupFlag = ( (!(lpwndpos->flags & SWP_NOSIZE)) || (lpwndpos->flags & SWP_NOACTIVATE) );
		if ( bUpdateRollupFlag && lpwndpos->cx > 0 && lpwndpos->cy > 0)
		{
			// 조건1 : 크기변화
			static int nOldCy = lpwndpos->cy;
			bool bUpdateRollupCondChangeSize = (nOldCy != lpwndpos->cy);

			// 조건2 : Activate 변화
			bool bUpdateRollupCondChangeActivity = (lpwndpos->flags & SWP_NOACTIVATE)?true:false;

			// 조건3 : 현재 활성화된 탭에 대해서만
			CTabbedPane* pTabbedPane = DYNAMIC_DOWNCAST (CTabbedPane, GetParentTabbedPane() );
			bool bUpdateRollupCondCurrentTab = ( (pTabbedPane == NULL ) || (pTabbedPane->GetUnderlyingWindow()->GetActiveWnd() == this) );

			// 롤업 크기 변경
			if ( (bUpdateRollupCondChangeSize || bUpdateRollupCondChangeActivity) && bUpdateRollupCondCurrentTab )
			{
				CRollupCtrl* pChild = dynamic_cast<CRollupCtrl*>(GetWindow(GW_CHILD ));
				if (pChild && AfxGetMainWnd())
				{
					// 변화된 크기를 원래 사이드바 크기에서의 Top-Gap 이라고 생각하고 계산
					RECT r;
					AfxGetMainWnd()->GetClientRect(&r);
					pChild->SetTopGap( r.bottom - r.top + 20 - lpwndpos->cy );
					pChild->RecalLayout();
				}
				nOldCy = lpwndpos->cy;
			}
		}
		CPaneDialog::OnWindowPosChanging(lpwndpos);
	}
	DECLARE_MESSAGE_MAP()
};
BEGIN_MESSAGE_MAP(CPaneDialogCoreToolEx, CPaneDialog)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

int CMainFrame::CreateViews_()
{
	// StatusBar 만들기
	m_pStatusBar_ = m_pControlMediator_->GetViewPtr<CCoreToolStatusBar>();
	if( m_pStatusBar_->Create( this ) == FALSE )
 	{
  		TRACE0("Failed to create views.\n");
 		return -1;
 	}

	// output log 창
	m_outputBar.Create( _T("Output"), this,FALSE, IDD_DIALOG_OUTPUT, CBRS_SIZE_FIXED | CBRS_BOTTOM, IDD_DIALOG_OUTPUT);
	m_outputBar.EnableDocking( CBRS_ALIGN_BOTTOM );
	DockPane( &m_outputBar, AFX_IDW_DOCKBAR_BOTTOM );

	//ToolBar 만들기
	m_pToolBar_ = m_pControlMediator_->GetViewPtr<CCoreToolToolBar>();
	if (!m_pToolBar_->Create( _T("ToolBar"), this, FALSE, IDD_VL_TOOL_BAR,
		CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_HIDE_INPLACE,
		IDD_VL_TOOL_BAR))
	{
		TRACE0("Failed to create TopBar\n");
		return -1;
	}
 	m_pToolBar_->EnableDocking( CBRS_ALIGN_ANY );
	DockPane( m_pToolBar_, AFX_IDW_DOCKBAR_TOP );

	// 다른 View 만들어지기전에 Helper 초기화
	CMFCHelper::InitMFCHelper(m_pControlMediator_, m_pStatusBar_ );

	// Side tab
	m_nTabCount = m_pWorkSpaceManager->GetSideTabCount();
	
	// MFC Framework 에서 OnCommand 를 바인딩 시켜줘야 메뉴가 활성화 되는것때문에 상수가 필요해서 정의해놨습니다
	_ASSERT( m_nTabCount < MAX_SIDE_TABS );

	m_arryWndRollupContainer_ = new CRollupCtrl[m_nTabCount];
	CDockablePane*	pTabbedBar = NULL;

	// Side Panel / Rollup Container 만들기
	for(int i = 0; i < m_nTabCount; i++)
	{
		string strTabName = m_pWorkSpaceManager->GetSideTabName(i);

		CPaneDialog* pNewPaneDialog = new CPaneDialogCoreToolEx;
 		m_arryWndSidePanels_.push_back( pNewPaneDialog );
		if(!m_arryWndSidePanels_[i]->Create( strTabName.c_str(), this, TRUE, IDD_SIDE_PANEL,
			CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_HIDE_INPLACE,
			ID_SIDE_PANEL_BASE+i))	// dlgCtrlID 에 고유 id 를 주어서 UI 저장시 문제없도록
		{
			TRACE0("Fail Side Panel\n");
			return -1;
		}

		m_arryWndSidePanels_[i]->EnableDocking( CBRS_ALIGN_ANY );
		if( pTabbedBar == NULL )
		{
			// 첫 패널은 오른쪽 도킹
			pTabbedBar = pNewPaneDialog;
			DockPane( pNewPaneDialog, AFX_IDW_DOCKBAR_RIGHT );
		}else
		{
			// 이후 패널은 tab으로 붙여주기
			pNewPaneDialog->AttachToTabWnd( pTabbedBar, DM_SHOW, TRUE, &pTabbedBar);
		}

		//사이드바 패널에 컨트롤 붙이기
		//library : CRollupCtrl, From CodeProject
		m_arryWndRollupContainer_[i].Create(WS_VISIBLE|WS_CHILD, CRect(0,10,285,1200), m_arryWndSidePanels_[i], 0);

		// shared workspace 에 등록
		m_pWorkSpaceManager->RegisterSideTab(i, m_arryWndSidePanels_[i]);
	}

	// Side Panel 만든 이후 보기 메뉴에추가
	//CreatePaneMenus();
	//m_wndMenuBar.CreateFromMenu( GetMenu()->m_hMenu );

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	DockPane( &m_wndMenuBar );


	HINSTANCE hInstOld = AfxGetResourceHandle();

	// Init views
	MAP_LISTENABLEVIEW& rollups = m_pWorkSpaceManager->GetListenableViews();
	for (MAP_LISTENABLEVIEW::iterator itr = rollups.begin(); itr != rollups.end(); ++itr)
	{
		SListenableViewInfo* pView = itr->second;
		//create view panel dialog
		if( pView->m_eType == CVT_ROLLUP )
		{
			AfxSetResourceHandle( pView->m_hResourceHandle );
			pView->m_pDialog->Create(MAKEINTRESOURCE(pView->m_nIDD), this );

			//put into SidePanel Container
			m_arryWndRollupContainer_[0].InsertPage(pView->m_pListenableView->GetViewName(), pView->m_pDialog, FALSE);
		}else
		if ( pView->m_eType == CVT_DIALOG )
		{
			AfxSetResourceHandle( pView->m_hResourceHandle );

			CDialog* pDlg = pView->m_pDialog;
			pDlg->Create(itr->second->m_nIDD, this);
			pDlg->ShowWindow(SW_HIDE);
		}
	}

	AfxSetResourceHandle( hInstOld  );

	// 툴바 초기화
 	m_pWorkSpaceManager->SetToolbar( m_pToolBar_);

	// snap to grid size - none
	OnSnapToGridSize(ID_GRIDSIZE_NONE);
	OnAngleSnapSize(ID_ANGLESNAP_NONE);

	return 0;
}

void CMainFrame::OnViewPane(UINT nID)
{
	unsigned int nSideTabID = nID - ID_SIDE_PANEL_BASE;
	if( nSideTabID >=0 && nSideTabID < m_arryWndSidePanels_.size() )
	{
		BOOL bVisible = m_arryWndSidePanels_[nSideTabID]->IsVisible();
		bVisible = !bVisible;
		m_arryWndSidePanels_[nSideTabID]->ShowPane( bVisible, FALSE, FALSE );
	}
}

void CMainFrame::OnUpdateUIViewPane(CCmdUI *pCmdUI)
{
	unsigned int nSideTabID = pCmdUI->m_nID - ID_SIDE_PANEL_BASE;
	if( nSideTabID >=0 && nSideTabID < m_arryWndSidePanels_.size() )
	{
		BOOL bVisible = m_arryWndSidePanels_[nSideTabID]->IsVisible();
		pCmdUI->SetCheck( bVisible );
	}
}

//프레임이 생성되고 난후에 app에 의해 호출되어서 초기화 되어야 할것들
bool CMainFrame::OnDocViewCreated_()
{
	RECT rt;
	m_pViewActive->GetClientRect(&rt); //워크밴치
	
	//장치만들기
	const UINT callTime = 3;
	bool bSuccess = m_pControlMediator_->CreateWBDeviceAndNotify(rt.right, rt.bottom, m_pViewActive->m_hWnd, callTime);
	
	CCoreToolRS3View* m_pWorkBenchView_ =  static_cast<CCoreToolRS3View*>(m_pViewActive);
	m_pWorkBenchView_->SetControlMediator(m_pControlMediator_);

	m_pWorkBenchView_->SetUserTimer(callTime);
	m_pViewActive->SetFocus();

	// user config 읽기
 	CXMLUserConfig::GetReference().LoadConfigRelatedUserTool( m_pControlMediator_ );

	// 초기 workspace 설정
	ChangeWorkSpace( CorePlugin::CWorkSpaceDefault::DEFAULT_WORKSPACE_NAME );

	m_pControlMediator_->GetWorkSpaceMgr()->PostToPluginCreatedMainWindow();

	return bSuccess;
}

void CMainFrame::ChangeWorkSpace( const std::string& _rWorkSpaceKeyword )
{
	// 이전 웍스페이스에서 바뀐 메뉴가 있을수 있으므로 복구한다
 	m_wndMenuBar.RestoreOriginalstate();

	m_pWorkSpaceManager->ChangeWorkSpace(m_arryWndRollupContainer_, _rWorkSpaceKeyword);
}

void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CFrameWnd::OnActivateApp(bActive, dwThreadID);
	TRACE( " OnActivateApp %s \n", bActive ? "true" : "false");
	if(bActive)
		m_bMainFrameActived_ = true;
	else
		m_bMainFrameActived_ = false;
}

void CMainFrame::OnClose()
{
	mlog("Closing application dialog START\n" );

	// TODO: CCoreToolRS3Doc::ConfirmSaveAndClose 와 중복된 코드인듯.
	// 문서 닫는 부분인 GetCurrentWorkSpace()->OnCloseFile 은 로직이 다르기까지 하다
	if(m_pControlMediator_->GetToolCmdMgr()->IsMapChanged())
	{
		int iResult = AfxMessageBox("Do you want to save your work?", MB_YESNOCANCEL);
		switch ( iResult )
		{
		case IDCANCEL:
			return;
		case IDYES:
			OnFileSaveall();
			break;
		case IDNO:
			break;
		}
	}


	PFC_FINALANALYSIS("profile_coretool.txt");

	CCoreToolRS3View* m_pWorkBenchView_ =  static_cast<CCoreToolRS3View*>(m_pViewActive);
	m_pWorkBenchView_->KillUserTimer();

	// 열려있는 문서 닫기
	m_pControlMediator_->BroadCastMessage(VIEW_MESSAGE::PRE_CLOSE_DOCUMENT);
	m_pWorkSpaceManager->GetCurrentWorkSpace()->OnCloseFile();

	// 특별히 신경써서 지워줘야할 객체들 지우기
	CRubyManager::GetReference().ClearManagedScriptFunctor();

	// mediator resource 지우기
	m_pControlMediator_->DestoryResource();

	m_pControlMediator_->BroadCastMessage(VIEW_MESSAGE::POST_CLOSE_DOCUMENT);

	// save coretool config
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	AfxGetApp()->WriteProfileBinary("CoreToolRS3", "WP", (LPBYTE)&wp, sizeof(wp));

	// save user config
	CXMLUserConfig::GetReference().SaveConfigRelatedUserTool( m_pControlMediator_ );

	// delete temp 파일
	m_pControlMediator_->GetHelperResource()->DeleteTempFile();

	/* 도킹되어있지 않은 pane 은 CFrameWnd::OnClose 에서 destroy 되어서 그 이전에 
	sidepanel 관련 윈도우들( m_arryWndRollupContainer_ ) 을 지워주어야 오류가 없다
	따라서 OnDestroy 가 아닌 OnClose 에서 view 윈도우들을 지운다

	m_arryWndSidePanels_ 들은 panel 이므로 onClose 에서 레지스트리에 마지막 스테이트를 기록해야 하므로
	Destroy 에서 지운다
	*/
	mlog( "destroying rollup containers\n" );
	for (int i = 0; i < m_nTabCount; ++i)
	{
		if( m_arryWndRollupContainer_[i].GetSafeHwnd() )
		{
			m_arryWndRollupContainer_[i].RemoveAllPages();
			m_arryWndRollupContainer_[i].DestroyWindow();
		}
	}
	delete[] m_arryWndRollupContainer_;

	CFrameWnd::OnClose();
}

void CMainFrame::OnDestroy()
{
	TRACE("CMainFrame::OnDestroy\n");

	DestroyViews();

	// 디바이스 해제
	m_pControlMediator_->DestroyDevice();

	mlog("Destroying Control Mediator\n");
	SAFE_DELETE(m_pControlMediator_);

	CFrameWnd::OnDestroy();
}

void CMainFrame::DestroyViews()
{
	mlog("Destroying views\n");

	// delete side panels
	for (int i = 0; i < m_nTabCount; ++i)
	{
		m_arryWndSidePanels_[i]->DestroyWindow();
		delete m_arryWndSidePanels_[i];
	}
	m_arryWndSidePanels_.clear();

	// CVT_FIXED 인것들. coreplugin 에서 생성/삭제하는데 create/destroy 는 여기서한다
	if(m_pToolBar_)
		m_pToolBar_->DestroyWindow();
	if(m_pStatusBar_)
		m_pStatusBar_->DestroyWindow();

	// unloading 시 listenable view 들은 해제된다
	mlog("unloading plugins\n");
	m_pWorkSpaceManager->UnloadPlugins();

	CMFCHelper::DestroyMFCHelper();
}

//리바(패널의 탭컨트롤)로 부터 트리깅되어, 리바의 탭이 열렸다고 메시지를 받았을때
LRESULT CMainFrame::OnRebarExpaned(WPARAM wParam, LPARAM lParam)
{
	m_pWorkSpaceManager->TriggerSideTabExpand((int)lParam, false);
	return 0;
}

//RollupPanel로부터 트리깅되어, RollupPanel이 열렸다고..
LRESULT CMainFrame::OnSidePanelExpaned(WPARAM wParam, LPARAM lParam)
{
	CRollupCtrl* pRollupContainer = (CRollupCtrl*)lParam;
	if(pRollupContainer != NULL)
	{
		int rollup_element = (int)wParam;
		m_pWorkSpaceManager->TriggerRollupExpand(pRollupContainer, rollup_element);
	}
	return 0;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	if(m_outputBar.m_hWnd)
	{
		RECT rt;
		m_outputBar.GetClientRect(&rt);
		CWnd* pWnd = m_outputBar.GetDlgItem(IDC_EDIT_OUTPUT);
		pWnd->MoveWindow(&rt);
	}
}

void CMainFrame::OutputText(const char* szText)
{
	if (FALSE == ::IsWindow(m_outputBar.GetSafeHwnd()))
		return;

	// log output 창
	CEdit* pEdit = (CEdit*)m_outputBar.GetDlgItem(IDC_EDIT_OUTPUT);

	if(NULL == pEdit)
		return;
	if (FALSE == ::IsWindow(pEdit->GetSafeHwnd()))
		return;

	int len = pEdit->GetWindowTextLength();
	pEdit->SetSel(len,len);

	char buffer[8192],*pBuf = buffer;
	const char* src = szText;
	_ASSERT(strlen(szText)<sizeof(buffer));

	while(*src!=0)	// edit control 이기 때문에 \n 을 \r\n 으로 바꾼다
	{
		if(*src=='\n')
		{
			*pBuf++ = '\r';
			*pBuf++ = '\n';
		}else
			*pBuf++ = *src;
		src++;
	}
	*pBuf=0;

	pEdit->ReplaceSel(buffer,false);
	pEdit->Invalidate(FALSE);

}

////////////// 보기 메뉴
//
void CMainFrame::OnViewTerrainInfo()
{
	m_pControlMediator_->GetHelperRender()->ToggleMapDetailInfo();
	m_pControlMediator_->GetViewPtr< CRollupEtcMapView >()->UpdateViewCheckerButton();
}

void CMainFrame::OnUpdateViewTerrainInfo(CCmdUI *pCmdUI)
{
	bool bState = m_pControlMediator_->GetHelperRender()->GetMapDetailInfoState();
	pCmdUI->SetCheck(bState ? 1 : 0);
}

void CMainFrame::OnViewAlphaSorting()
{
	m_pControlMediator_->GetHelperRender()->ToggleViewAlphaSorting();
}

void CMainFrame::OnUpdateViewAlphaSorting(CCmdUI *pCmdUI)
{
	bool bState = m_pControlMediator_->GetHelperRender()->GetViewAlphaSorting();
	pCmdUI->SetCheck(bState ? 1 : 0);
}

void CMainFrame::OnWBInfoView()
{
	m_pControlMediator_->GetHelperRender()->ToggleViewSideInfoPanel();
}

void CMainFrame::OnViewProfileInfo()
{
	m_pControlMediator_->GetHelperRender()->SetVisibleProfile();
}

void CMainFrame::OnViewLighting()
{
	m_pControlMediator_->GetHelperRender()->ToggleViewLightingOnly();

	using namespace rs3;
	RShaderCompositeManager* pManager = REngine::GetSceneManager().GetShaderCompositeManager();
	pManager->SetViewLightingOnly( m_pControlMediator_->GetHelperRender()->GetViewLightingOnly() );
}

void CMainFrame::OnUpdateViewLighting(CCmdUI *pCmdUI)
{
	bool bState = m_pControlMediator_->GetHelperRender()->GetViewLightingOnly();
	pCmdUI->SetCheck(bState ? 1 : 0);
}

void CMainFrame::OnViewStaticRenderCell()
{
	m_pControlMediator_->GetHelperRender()->ToggleViewStaticMeshRenderCell();

	rs3::RToolStaticMeshSceneNode* pStaticMesh = static_cast<rs3::RToolStaticMeshSceneNode*>(rs3::REngine::GetSceneManager().GetCurrentStaticMeshSceneNode());
	if( pStaticMesh )
	{
		pStaticMesh->SetRenderDebug( m_pControlMediator_->GetHelperRender()->GetViewStaticMeshRenderCell() );
	}
}
void CMainFrame::OnUpdateViewStaticRenderCell(CCmdUI *pCmdUI)
{
	bool bState = m_pControlMediator_->GetHelperRender()->GetViewStaticMeshRenderCell();
	pCmdUI->SetCheck(bState ? 1 : 0);
}

void CMainFrame::OnViewStaticRenderAll()
{
	m_pControlMediator_->GetHelperRender()->ToggleViewStaticMeshRenderAll();

	rs3::RToolStaticMeshSceneNode* pStaticMesh = static_cast<rs3::RToolStaticMeshSceneNode*>(rs3::REngine::GetSceneManager().GetCurrentStaticMeshSceneNode());
	if( pStaticMesh )
	{
		pStaticMesh->SetRenderAll( m_pControlMediator_->GetHelperRender()->GetViewStaticMeshRenderAll() );
	}
}

void CMainFrame::OnUpdateViewStaticRenderAll(CCmdUI *pCmdUI)
{
	bool bState = m_pControlMediator_->GetHelperRender()->GetViewStaticMeshRenderAll();
	pCmdUI->SetCheck(bState ? 1 : 0);
}

void CMainFrame::OnViewHDR()
{
	m_bHDREnable = !m_bHDREnable;
	rs3::REngine::GetSceneManager().SetHDREnable(m_bHDREnable);

	if( m_bHDREnable )
	{
		m_bHDRViewRenderTargetEnable = !m_bHDRViewRenderTargetEnable;
		OnViewHDRRenderTarget();
	}
}

void CMainFrame::OnUpdateViewHDR(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( rs3::REngine::GetSceneManager().IsEnableHDR() ? 1 : 0);
}

void CMainFrame::OnViewHDRRenderTarget()
{
	m_bHDRViewRenderTargetEnable = !m_bHDRViewRenderTargetEnable;

	if( rs3::REngine::GetSceneManager().IsEnableHDR() )
		rs3::REngine::GetSceneManager().SetHDRViewRenderTarget( m_bHDRViewRenderTargetEnable );
}

void CMainFrame::OnUpdateViewHDRRenderTarget(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_bHDRViewRenderTargetEnable ? 1 : 0);
}

void CMainFrame::OnViewDof()
{
	rs3::RToolSceneManager* pMng = m_pControlMediator_->GetToolSceneManager();
	if(!pMng) return;

	static bool bDOFEnable = false;
	bDOFEnable = !bDOFEnable;
	pMng->SetDOFEnable(bDOFEnable);
}

void CMainFrame::OnUpdateViewDof(CCmdUI *pCmdUI)
{
	rs3::RToolSceneManager* pMng = m_pControlMediator_->GetToolSceneManager();
	if(!pMng) return;

	pCmdUI->SetCheck( pMng->IsEnableDOF() ? 1 : 0);

}


void CMainFrame::OnViewScreenMotionBlur()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	rs3::RToolSceneManager* pMng = m_pControlMediator_->GetToolSceneManager();
	if(!pMng) return;
	
	static bool bScreenMotionBlurEnable = false;
	bScreenMotionBlurEnable = !bScreenMotionBlurEnable;
	pMng->SetScreenMotionBlurEnable( bScreenMotionBlurEnable);
	pMng->SetScreenMotionBlurActive( bScreenMotionBlurEnable);
}

void CMainFrame::OnUpdateViewScreenMotionBlur(CCmdUI *pCmdUI)
{
	rs3::RToolSceneManager* pMng = m_pControlMediator_->GetToolSceneManager();
	if(!pMng) return;

	pCmdUI->SetCheck( pMng->IsEnableScreenMotionBlur() ? 1 : 0);
}

void CMainFrame::OnViewClipping()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_pControlMediator_->GetHelperRender()->ShowToggleClipPlane();
}

void CMainFrame::OnUpdateViewClipping(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck(m_pControlMediator_->GetHelperRender()->IsClipPlane() ? 1 : 0);
}

void CMainFrame::OnViewSsao()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	rs3::RToolSceneManager* pManager = m_pControlMediator_->GetToolSceneManager();
	if(!pManager) return;

	static bool bSSAOEnable = false;
	bSSAOEnable = !bSSAOEnable;
	pManager->UseSSAO( bSSAOEnable ? 3 : 0);
}

void CMainFrame::OnUpdateViewSsao(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	rs3::RToolSceneManager* pManager = m_pControlMediator_->GetToolSceneManager();
	if(!pManager) return;

	pCmdUI->SetCheck( pManager->IsUsingSSAO() ? 1 : 0);
}

void CMainFrame::OnFileOpenall()
{
	char szfull[256] = "";
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );

	string strFilter;
	m_pWorkSpaceManager->GetWorkSpaceOpenFileFilter( strFilter );

	CXMLOpenDlg dlg(szfull, strFilter.c_str() );
	CString sFile;
	if (dlg.DoModal()==IDOK )
	{
		POSITION pos = dlg.GetStartPosition();
		sFile = dlg.GetNextPathName(pos);
	}

	//모래시계 시작
	HCURSOR hOld = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));

	//오픈
	CCoreToolRS3Doc* pDoc_ = static_cast<CCoreToolRS3Doc*>(GetActiveDocument());
	pDoc_->OpenMapFile(sFile);
	//모래시계 끝
	::SetCursor(hOld);
}

void CMainFrame::OnFileSaveall()
{
	CCoreToolRS3Doc* pDoc_ = static_cast<CCoreToolRS3Doc*>(GetActiveDocument());
	pDoc_->SaveMap();
	CStatusBarHandler::GetInstance()->ShowStatusBarMessage("Saving all..");
}

void CMainFrame::OnFileSaveAsAll()
{
	if( m_pControlMediator_->m_mapInfo.m_bLoaded == false )
		return;

	CWorkSpace* pWorkSpace = m_pControlMediator_->GetWorkSpaceMgr()->GetCurrentWorkSpace();
	if( !pWorkSpace ) return;

	CCoreToolRS3Doc* pDoc_ = static_cast<CCoreToolRS3Doc*>(GetActiveDocument());
	CFileDialog dlg(FALSE, pWorkSpace->GetFileExtension(), pDoc_->GetPathName() );

	// 확장자 필터 구하기
	string strFilter;
	pWorkSpace->GetOpenFileFilter( strFilter );
	dlg.m_ofn.lpstrFilter = strFilter.c_str();

	dlg.m_ofn.Flags |= OFN_NOCHANGEDIR;

	if (dlg.DoModal() == IDOK)
	{
		CString fileName(dlg.GetPathName());

		CCoreToolRS3Doc* pDoc_ = static_cast<CCoreToolRS3Doc*>(GetActiveDocument());
		pDoc_->SaveMap( fileName.GetString() );
		CStatusBarHandler::GetInstance()->ShowStatusBarMessage("Saving filename as...");
	}
}

void CMainFrame::OnToolsOption()
{
	CMFCHelper::OpenToolOptionDlg();
}

void CMainFrame::OnToolsShortCut()
{
	CMFCHelper::OpenShortCutList();
}

void CMainFrame::OnPause()
{
	m_pControlMediator_->GetHelperAction()->SetPauseToggle();
}

void CMainFrame::OnToolsDataFolderOpen()
{
	char szfull[256]= "";
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	ShellExecute(NULL, "explore", szfull, NULL, NULL, SW_SHOWNORMAL);
}

void CMainFrame::OnToolsMapResMgr()
{
	CDlgResourceInfo* pDlg = m_pControlMediator_->GetViewPtr<CDlgResourceInfo>();
	pDlg->SetResourceType(CDlgResourceInfo::RES_ALL);
	pDlg->Listen( CoreMessage(VIEW_MESSAGE::ACTIVE_VIEW) );
}

void CMainFrame::OnToolsXmlZone()
{
	CMFCHelper::OpenZoneListEditor();
}

void CMainFrame::OnToolsXmlField()
{
	CMFCHelper::OpenFieldListEditor();
}

void CMainFrame::OnToolsXmlNpc()
{
	CMFCHelper::OpenNpcListEditor();
}

void CMainFrame::OnToolsXmlProperty()
{
	CMFCHelper::OpenPropertyListEditor();
}

void CMainFrame::OnObjExport()
{
	CDlgExportObj Dlg;

	if ( Dlg.DoModal()==IDOK )
	{
		int nLevel = max(Dlg.m_nLODLevel, 0);
		int nWPart = max(Dlg.m_nWPart, 0);
		int nHPart = max(Dlg.m_nHPart, 0);

		nLevel = (int)pow(2.f,nLevel);
		nWPart = (int)pow(2.f,nWPart);
		nHPart = (int)pow(2.f,nHPart);

		CCoreToolRS3Doc* pDoc_ = static_cast<CCoreToolRS3Doc*>(GetActiveDocument());
		pDoc_->ExportObj(nLevel, nWPart, nHPart);
	}
}

void CMainFrame::OnObjImport()
{
	CFileDialog Dlg(TRUE);
	Dlg.GetOFN().lpstrTitle = _T(".obj Files");
	Dlg.GetOFN().lpstrFilter = "obj file(*.obj)\0*.obj";
	Dlg.GetOFN().Flags |= OFN_NOCHANGEDIR | OFN_READONLY;

	if ( Dlg.DoModal()==IDOK )
	{
		CCoreToolRS3Doc* pDoc_ = static_cast<CCoreToolRS3Doc*>(GetActiveDocument());
		pDoc_->ImportObj(Dlg.GetPathName());
	}
}

void CMainFrame::OnEditUndo()
{
	CCoreToolRS3Doc* pDoc_ = static_cast<CCoreToolRS3Doc*>(GetActiveDocument());
	pDoc_->EditUndo();
}

void CMainFrame::OnToolsMapBatchScriptOpen()
{
	CMFCHelper::OpenMapBatchScrip();
}

// void CMainFrame::ShowTipAtStartup_(void)
// {
// 	CCommandLineInfo cmdInfo;
// 	ParseCommandLine(cmdInfo);
// 	if (cmdInfo.m_bShowSplash)
// 	{
// 		CTipDlg dlg;
// 		if (dlg.m_bStartup)
// 			dlg.DoModal();
// 	}
// }

// void CMainFrame::OnShowTipOfTheDay(void)
// {
// 	CTipDlg dlg;
// 	dlg.DoModal();
// }

void CMainFrame::OnImportScene()
{
	CFileDialog Dlg(TRUE);
	Dlg.GetOFN().lpstrTitle = _T(".scene.xml 파일을 선택하세요");
	Dlg.GetOFN().lpstrFilter = "scene.xml파일(*.scene.xml)\0*.scene.xml";
	Dlg.GetOFN().Flags |= OFN_NOCHANGEDIR | OFN_READONLY;

	if ( Dlg.DoModal()==IDOK )
	{
		CString file = Dlg.GetPathName();
		m_pControlMediator_->GetToolSceneManager()->ImportScene(file);
	}
}

void CMainFrame::OnExportScene()
{
	
}

// int CMainFrame::ExitInstance()
// {
// 	#ifndef _GUNZ2
// 	// 유닛테스트 리턴
// 	if( strcmp(m_lpCmdLine,"/test")==0 ) 
// 		return RunUnitTest();
// 	#endif
// 
// 	CMFCVisualManager::DestroyInstance();
// 
// 	int nExitCode = CWinApp::ExitInstance();
// 	if (0 != nExitCode)
// 		return nExitCode;
// 
// 	return m_pMainFrame_->ExitCode;
// }

void CMainFrame::OnSetAutoAnimationProperty()
{
	m_pControlMediator_->GetHelperResource()->SetAutoAnimationProperty();
}


BOOL CMainFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle , const RECT& rect , CWnd* pParentWnd , LPCTSTR lpszMenuName , DWORD dwExStyle , CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	BOOL bCreated = CFrameWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, lpszMenuName, dwExStyle, pContext);

	CWinApp* pApp = AfxGetApp();

	// 예전의 위치 기억해서 보여주기
	WINDOWPLACEMENT *lwp;
	UINT nl;
	if(pApp->GetProfileBinary("CoreToolRS3", "WP", (LPBYTE*)&lwp, &nl))
	{
		lwp->showCmd = SW_HIDE;
		SetWindowPlacement(lwp);
		delete [] lwp;
	}
	return bCreated;
}

void CMainFrame::OnFileNew0()
{
	std::vector<std::string> vList;
	m_pControlMediator_->GetWorkSpaceMgr()->GetNewMapList(vList);

	CDlgNewMapTypeSelector dlg(vList);
	if(dlg.DoModal() == IDOK)
	{
		const char* _pSzNewMapName = dlg.m_strSelected.GetString();
		m_pControlMediator_->GetWorkSpaceMgr()->NotifyClickedNewMap(_pSzNewMapName);
	}
}

void CMainFrame::OnFileNew1()
{
	m_pControlMediator_->GetWorkSpaceMgr()->NotifyClickedFileNewMenu(ID_FILE_NEW_1);
}

void CMainFrame::OnFileNew2()
{
	m_pControlMediator_->GetWorkSpaceMgr()->NotifyClickedFileNewMenu(ID_FILE_NEW_2);
}

void CMainFrame::OnFileNew3()
{
	m_pControlMediator_->GetWorkSpaceMgr()->NotifyClickedFileNewMenu(ID_FILE_NEW_3);
}

void CMainFrame::OnFileNew4()
{
	m_pControlMediator_->GetWorkSpaceMgr()->NotifyClickedFileNewMenu(ID_FILE_NEW_4);
}

void CMainFrame::OnFileNew5()
{
	m_pControlMediator_->GetWorkSpaceMgr()->NotifyClickedFileNewMenu(ID_FILE_NEW_5);
}

void CMainFrame::OnToolEtc1()
{
	m_pControlMediator_->GetWorkSpaceMgr()->NotifyClickedFileNewMenu(ID_TOOL_ETC_1);
}

void CMainFrame::OnToolEtc2()
{
	m_pControlMediator_->GetWorkSpaceMgr()->NotifyClickedFileNewMenu(ID_TOOL_ETC_2);
}

void CMainFrame::OnToolEtc3()
{
	m_pControlMediator_->GetWorkSpaceMgr()->NotifyClickedFileNewMenu(ID_TOOL_ETC_3);
}

void CMainFrame::OnToolEtc4()
{
	m_pControlMediator_->GetWorkSpaceMgr()->NotifyClickedFileNewMenu(ID_TOOL_ETC_4);
}

void CMainFrame::OnToolEtc5()
{
	m_pControlMediator_->GetWorkSpaceMgr()->NotifyClickedFileNewMenu(ID_TOOL_ETC_5);
}

void CMainFrame::OnViewDistortion()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	static bool bDistortionEnable = true;
	
	//rs3::REngine::GetSceneManager().SetDistortionEnable(bDistortionEnable);

	rs3::RPostEffectInterface* pDistortion =rs3::REngine::GetSceneManager().GetPostEffect(rs3::PE_DISTORTION);
	if(pDistortion)
	{
		bDistortionEnable = !bDistortionEnable;
		pDistortion->SetActive(bDistortionEnable);
	}
}

void CMainFrame::OnViewNormalMap()
{
	bool bNormalMapEnable( rs3::REngine::GetSceneManager().GetShaderCompositeManager()->GetUseNormalMap() );
	bNormalMapEnable = !bNormalMapEnable;
	rs3::REngine::GetSceneManager().GetShaderCompositeManager()->SetUseNormalMap( bNormalMapEnable );
}

void CMainFrame::OnUpdateViewNormalMap(CCmdUI *pCmdUI)
{
	bool bActive( rs3::REngine::GetSceneManager().GetShaderCompositeManager()->GetUseNormalMap() );
	pCmdUI->SetCheck(bActive);
}


void CMainFrame::OnViewGammaCorrect()
{
	bool bGammaCorrectEnable( rs3::REngine::GetSceneManager().GetRenderingStrategy()->GetGammaCorrect() );
	bGammaCorrectEnable = !bGammaCorrectEnable;
	rs3::REngine::GetSceneManager().GetRenderingStrategy()->SetGammaCorrect( bGammaCorrectEnable );
}

void CMainFrame::OnUpdateGammaCorrect(CCmdUI *pCmdUI)
{
	bool bActive( rs3::REngine::GetSceneManager().GetRenderingStrategy()->GetGammaCorrect() );
	pCmdUI->SetCheck(bActive);
}


void CMainFrame::OnViewFXAA()
{
	bool bFXAAEnable( false );
	if( 0 < rs3::REngine::GetSceneManager().GetRenderingStrategy()->GetFXAA_Level() )
		bFXAAEnable = true;

	bFXAAEnable = !bFXAAEnable;

	if( bFXAAEnable )
		rs3::REngine::GetSceneManager().GetRenderingStrategy()->SetFXAA_Level( 5 );
	else
		rs3::REngine::GetSceneManager().GetRenderingStrategy()->SetFXAA_Level( 0 );
}

void CMainFrame::OnUpdateFXAA(CCmdUI *pCmdUI)
{
	bool bActive( false );
	if( 0 < rs3::REngine::GetSceneManager().GetRenderingStrategy()->GetFXAA_Level() )
		bActive = true;
	pCmdUI->SetCheck(bActive);
}



void CMainFrame::OnUpdateViewDistortion(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	bool bActive =false;

	rs3::RPostEffectInterface* pDistortion =rs3::REngine::GetSceneManager().GetPostEffect(rs3::PE_DISTORTION);
	if(pDistortion)
		bActive =pDistortion->IsActive();
	
	//pCmdUI->SetCheck( rs3::REngine::GetSceneManager().IsEnableDistortion() ? 1 : 0);
	pCmdUI->SetCheck(bActive);
}

void CMainFrame::OnViewShadow()
{
	static bool bEnable = (rs3::REngine::GetSceneManager().GetRealtimeShadowMethod() != rs3::SHADOW_NONE);

	bEnable = !bEnable;
	rs3::RShadowDesc desc;
	if(!bEnable)
		desc.eMethod = rs3::SHADOW_NONE;

	rs3::REngine::GetSceneManager().SetRealtimeShadowMethod(desc);
}

void CMainFrame::OnUpdateViewShadow(CCmdUI *pCmdUI)
{
	bool bActive =false;

	bActive = (rs3::REngine::GetSceneManager().GetRealtimeShadowMethod() != rs3::SHADOW_NONE);

	pCmdUI->SetCheck(bActive);
}

void CMainFrame::SetSnapToGridSize(int nSnapGridSize)
{
	m_nSnapGridSize = nSnapGridSize;
}

void CMainFrame::IncreaseSnapToGridSize()
{
	UINT nID = GetSnapToGridID( m_nSnapGridSize );
	UINT nNextID = min( nID + 1 , ID_GRIDSIZE_50 );
	OnSnapToGridSize( nNextID );

	// status bar 에 프롬프트 메시지 출력 :
	// m_wndMenuBar.ShowCommandMessageString( nNextID );
	// 위 함수가 protected 라 hack 스럽지만 이렇게 처리
	SendMessage(WM_SETMESSAGESTRING, (WPARAM) nNextID);
}

void CMainFrame::DecreaseSnapToGridSize()
{
	UINT nID = GetSnapToGridID( m_nSnapGridSize );
	UINT nPrevID = max( nID - 1 , ID_GRIDSIZE_NONE );
	OnSnapToGridSize( nPrevID );

	SendMessage(WM_SETMESSAGESTRING, (WPARAM) nPrevID);
}

UINT CMainFrame::GetSnapToGridID( int nSize )
{
	UINT nID = ID_GRIDSIZE_NONE;
	switch( m_nSnapGridSize )
	{
	case 1: nID = ID_GRIDSIZE_1 ;break;
	case 5: nID = ID_GRIDSIZE_5 ;break;
	case 10: nID = ID_GRIDSIZE_10 ;break;
	case 25: nID = ID_GRIDSIZE_25 ;break;
	case 50: nID = ID_GRIDSIZE_50 ;break;
	}
	return nID;
}

void CMainFrame::OnUpdateUISnapToGridSize(CCmdUI *pCmdUI)
{
	UINT nID = GetSnapToGridID(m_nSnapGridSize);

	BOOL bCheck = ( nID == pCmdUI->m_nID );
	pCmdUI->SetCheck( bCheck );
}

void CMainFrame::OnSnapToGridSize(UINT nID)
{
	int nSnapGridSize = 0;
	switch( nID )
	{
	case ID_GRIDSIZE_1 : nSnapGridSize = 1;break;
	case ID_GRIDSIZE_5 : nSnapGridSize = 5;break;
	case ID_GRIDSIZE_10 : nSnapGridSize = 10;break;
	case ID_GRIDSIZE_25 : nSnapGridSize = 25;break;
	case ID_GRIDSIZE_50 : nSnapGridSize = 50;break;
	default: nSnapGridSize = 0;
	}

	SetSnapToGridSize( nSnapGridSize );
}

void CMainFrame::SetAngleSnapSize(int nAngleSnapSize)
{
	m_nAngleSnapSize = nAngleSnapSize;
}

void CMainFrame::OnUpdateUIAngleSnapSize(CCmdUI *pCmdUI)
{
	UINT nID = ID_ANGLESNAP_NONE;
	switch( m_nAngleSnapSize )
	{
	case 1: nID = ID_ANGLESNAP_1 ;break;
	case 5: nID = ID_ANGLESNAP_5 ;break;
	case 10: nID = ID_ANGLESNAP_10 ;break;
	case 45: nID = ID_ANGLESNAP_45 ;break;
	}

	BOOL bCheck = ( nID == pCmdUI->m_nID );
	pCmdUI->SetCheck( bCheck );
}

void CMainFrame::OnAngleSnapSize(UINT nID)
{
	int nAngleSnapSize = 0;
	switch( nID )
	{
	case ID_ANGLESNAP_1 : nAngleSnapSize = 1;break;
	case ID_ANGLESNAP_5 : nAngleSnapSize = 5;break;
	case ID_ANGLESNAP_10 : nAngleSnapSize = 10;break;
	case ID_ANGLESNAP_45 : nAngleSnapSize = 45;break;
	default: nAngleSnapSize = 0;
	}

	SetAngleSnapSize( nAngleSnapSize );
}

BOOL CMainFrame::OnShowPopupMenu (CMFCPopupMenu* pMenuPopup)
{
	CFrameWnd::OnShowPopupMenu (pMenuPopup);

	if (pMenuPopup == NULL)
	{
		return TRUE;
	}

	if (pMenuPopup->GetMenuBar ()->CommandToIndex (ID_VIEW_TOOLBARS) >= 0)
	{
		pMenuPopup->RemoveAllItems ();

		for(int i = 0; i < m_nTabCount; i++)
		{
			string strTabName = m_pWorkSpaceManager->GetSideTabName(i);
			pMenuPopup->InsertItem (
				CMFCToolBarMenuButton (ID_SIDE_PANEL_BASE + i , NULL, -1, strTabName.c_str() ), i );
		}
	}

	return m_pWorkSpaceManager->OnShowPopupMenu( pMenuPopup );
}

void CMainFrame::OnSelectNone()
{
	m_pControlMediator_->GetHelperAction()->RemoveAllSelection(true);
}

void CMainFrame::OnViewDefaultLight()
{
	VG_DefaultLight* pDefaultLight = m_pControlMediator_->GetViewPtr< VG_DefaultLight >();
	pDefaultLight->ToggleLight();
}

void CMainFrame::OnViewDefaultLightToCamera()
{
	VG_DefaultLight* pDefaultLight = m_pControlMediator_->GetViewPtr< VG_DefaultLight >();
	pDefaultLight->ToggleLightToCamera();
}

void CMainFrame::OnUpdateViewDefaultLight(CCmdUI *pCmdUI)
{
	VG_DefaultLight* pDefaultLight = m_pControlMediator_->GetViewPtr< VG_DefaultLight >();

	switch( pCmdUI->m_nID ) 
	{
	case ID_VIEW_DEFAULT_LIGHT :
		pCmdUI->SetCheck( pDefaultLight->IsEnable() );
		break;
	case ID_VIEW_DEFAULT_LIGHT_TO_CAMERA :
		pCmdUI->SetCheck( pDefaultLight->IsLightToCamera() );
		break;
	}

	rs3::RDirectionalLightSceneNode* pLight = m_pControlMediator_->GetToolSceneManager()->GetDefaultLight();
	bool bLightExist = ( pLight && pDefaultLight->GetDefaultLight() != pLight );
	
	// 디렉셔널 라이트가 이미 있으면 비 활성화
	pCmdUI->Enable( bLightExist == false );
}

void CMainFrame::OnViewOccluder()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	rs3::RToolCamera* pCamera = m_pControlMediator_->GetToolCamera();
	pCamera->SetEnableOcclusionCulling(!pCamera->GetEnableOcclusionCulling());
}

void CMainFrame::OnUpdateViewOccluder(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.

	rs3::RToolCamera* pCamera = m_pControlMediator_->GetToolCamera();
	bool bActive	= pCamera->GetEnableOcclusionCulling();
	pCmdUI->SetCheck(bActive);
}

void CMainFrame::OnViewAnisotropy()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	switch( rs3::REngine::GetConfig().GetTextureFilter())
	{
	case rs3::RTF_TRILINEAR_ANISO:
		rs3::REngine::GetConfig().SetTextureFilter( rs3::RTF_TRILINEAR);
		rs3::REngine::GetConfig().SetTextureMaxAnisotropy(1);
		break;
	default:
		rs3::REngine::GetConfig().SetTextureFilter( rs3::RTF_TRILINEAR_ANISO);
		rs3::REngine::GetConfig().SetTextureMaxAnisotropy(3);
		break;
	}
}

void CMainFrame::OnUpdateViewAnisotropy(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( rs3::REngine::GetConfig().GetTextureFilter() == rs3::RTF_TRILINEAR_ANISO);
}
