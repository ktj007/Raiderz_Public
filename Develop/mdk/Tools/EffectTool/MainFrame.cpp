// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "MainFrame.h"
#include "MainPanel.h"
#include "EffectListPanel.h"
#include "EffectScenePanel.h"
#include "Config.h"

#include "res/EffectTool.xpm"

namespace EffectTool
{

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_MENU(wxID_EXIT, MainFrame::OnQuit)
	EVT_MENU(ID_MENU_VIEW_EFFECTLIST, MainFrame::OnViewEffectList)
	EVT_MENU(ID_MENU_VIEW_EFFECTSCENE, MainFrame::OnViewEffectScene)
END_EVENT_TABLE()

const wxString MAINPANEL_NAME = "MainPanel";
const wxString EFFECTLISTPANEL_NAME = "EffectListPanel";
const wxString EFFECTSCENEPANEL_NAME = "EffectScenePanel";

MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title,
					 const wxPoint& pos, const wxSize& size, long style) :
 wxFrame(parent, id, title, pos, size, style)
{
	wxIcon icon(EffectTool_xpm);
	SetIcon(icon);

	CConfig::GetInstance().Init();
//	CConfig::GetInstance().LoadConfig();
	MInitLog(MLOG_LEVEL_DEBUG, MLOG_FILE|MLOG_DEBUG|MLOG_PROG, MLogCallBack);

	wxAuiManager* auiManager = new wxAuiManager;
	auiManager->SetManagedWindow(this);

	SetMinSize(wxSize(800,600));

	CreateMenuBar();
	
	CreateStatusBar();
	GetStatusBar()->SetStatusText("준비");

	// 메인 패널
	MainPanel* mainPanel = new MainPanel(this, ID_MAINFRAME);
	auiManager->AddPane(mainPanel, wxAuiPaneInfo().Name(MAINPANEL_NAME).Show().CenterPane().Layer(0).Position(0));

	// 이펙트 리스트 패널
	EffectListPanel* effectListPanel = new EffectListPanel(this);
	auiManager->AddPane(effectListPanel, wxAuiPaneInfo().Name(EFFECTLISTPANEL_NAME).Show().PinButton().Left().Layer(0).Position(0).FloatingSize(250, 500));

	// 이펙트 씬 패널
	EffectScenePanel* effectScenePanel = new EffectScenePanel(this);
	auiManager->AddPane(effectScenePanel, wxAuiPaneInfo().Name(EFFECTSCENEPANEL_NAME).Show().PinButton().Right().Layer(0).Position(0).FloatingSize(250, 500));

	auiManager->Update();

	mainPanel->Init();
	effectListPanel->Init();
}

MainFrame::~MainFrame()
{
	wxAuiManager* mgr = GetAuiManager();
	mgr->UnInit();
	delete mgr;

	MFinalizeLog();
}

bool MainFrame::CreateEffectSceneNode(const wxString& fileName)
{
	MainPanel* mainPanel = wxDynamicCast(GetPanel(MAINPANEL_NAME).window, MainPanel);
	_ASSERT(mainPanel);

	if(NULL == mainPanel)
		return false;

	return mainPanel->CreateEffectSceneNode(fileName);
}

void MainFrame::RefreshEffectSceneTreeCtrl(RSceneNode* pSceneNode)
{
	_ASSERT(pSceneNode);

	EffectScenePanel* panel = wxDynamicCast(GetPanel(EFFECTSCENEPANEL_NAME).window, EffectScenePanel);
	_ASSERT(panel);

	panel->SetEffectSceneNode(pSceneNode);
}

void MainFrame::MLogCallBack(const char* szText, MLogLevel nLevel)
{

}

wxAuiManager* MainFrame::GetAuiManager()
{
	wxAuiManager* mgr = wxAuiManager::GetManager(this);
	_ASSERT(mgr);
	return mgr;
}

wxAuiPaneInfo& MainFrame::GetPanel(const wxString& name)
{
	wxAuiManager* mgr = GetAuiManager();
	return mgr->GetPane(name);
}

void MainFrame::ShowPanel(const wxString& name, bool show)
{
	wxAuiPaneInfo& paneInfo = GetPanel(name);
	paneInfo.Show(show);
	GetAuiManager()->Update();
}

void MainFrame::OnQuit(wxCommandEvent& event)
{
	Close(true);
}

void MainFrame::OnViewEffectList( wxCommandEvent& event )
{
	ShowPanel(EFFECTLISTPANEL_NAME, event.IsChecked());
}

void MainFrame::OnViewEffectScene( wxCommandEvent& event )
{
	ShowPanel(EFFECTSCENEPANEL_NAME, event.IsChecked());
}

void MainFrame::CreateMenuBar()
{
	wxMenuBar* menuBar = new wxMenuBar;

	// 파일
	{
		wxMenu* fileMenu = new wxMenu;
		fileMenu->Append(wxID_EXIT);
		menuBar->Append(fileMenu, "파일(&F)");
	}
	// 보기
	{
		wxMenu* viewMenu = new wxMenu;

		wxMenuItem* menuItemEffectList = viewMenu->Append(ID_MENU_VIEW_EFFECTLIST, "이펙트 리스트(&L)", wxEmptyString, wxITEM_CHECK );
		menuItemEffectList->Check(true);

		wxMenuItem* menuItemEffectScene = viewMenu->Append(ID_MENU_VIEW_EFFECTSCENE, "이펙트 씬(&S)", wxEmptyString, wxITEM_CHECK );
		menuItemEffectScene->Check(true);

		menuBar->Append(viewMenu, "보기(&V)");
	}

	SetMenuBar(menuBar);
}

}