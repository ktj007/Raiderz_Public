// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "EffectTool.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CRView*			g_rView;
extern CControlView*	g_ControlView;

BEGIN_MESSAGE_MAP(CSplitterX, CSplitterWnd)
	//{{AFX_MSG_MAP(CSplitterX)
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSplitterX::CSplitterX() {}
CSplitterX::~CSplitterX(){}

LRESULT CSplitterX::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	return HTNOWHERE;
	//return CSplitterWnd::OnNcHitTest(point);

}

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE( WM_MODIFY_MSG, OnModifyParticle)

	ON_COMMAND(ID_VIEW_START, OnViewStart)
	ON_COMMAND(ID_VIEW_STOP, OnViewStop)
	ON_COMMAND(ID_VIEW_PAUSE, OnViewPause)
	ON_COMMAND(ID_VIEW_BGCOLOR, OnViewBgcolor)

	ON_COMMAND(ID_TB_WIREFRAME, OnWireFrame)
	ON_COMMAND(ID_TB_AXIS, OnViewAxis)
	ON_COMMAND(ID_TB_GRID, OnViewGrid)
	ON_COMMAND(ID_TB_EMITTER, OnViewEmitter)
	ON_COMMAND(ID_VIEW_PATH, OnViewPath)

	ON_UPDATE_COMMAND_UI(ID_TB_WIREFRAME, OnWireFrameUpdateUI)
	ON_UPDATE_COMMAND_UI(ID_TB_AXIS, OnViewAxisUpdateUI)
	ON_UPDATE_COMMAND_UI(ID_TB_GRID, OnViewGridUpdateUI)
	ON_UPDATE_COMMAND_UI(ID_TB_EMITTER, OnViewEmitterUpdateUI)

	ON_COMMAND(ID_FILE_NEW, OnNew)
	ON_COMMAND(ID_FILE_OPEN, OnEffectFileOpen)
	ON_COMMAND(ID_FILE_SAVEQ, OnEffectFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnEffectFileSaveAs)

	ON_COMMAND(ID_FILE_OPEN_MESH, OnMeshEffectFileOpen)
	ON_COMMAND(ID_FILE_OPEN_ELU, OnCharFileOpen)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

ULONG_PTR m_gdiplusToken;

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	//_CrtSetBreakAlloc(679);
}

CMainFrame::~CMainFrame()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
		
//	ShowWindow(SW_SHOWMAXIMIZED);
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// 
//	cs.style ^= WS_MAXIMIZEBOX;
	
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	cs.cx = 1024;
	cs.cy = 900;

	return TRUE;
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


// CMainFrame 메시지 처리기
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{

	HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	int width = GetDeviceCaps(hDC, HORZRES);

	// TODO: Add your specialized code here and/or call the base class
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE("분할 윈도우를 생성하는데 실패 했습니다.\n");
		return FALSE;
	}
	//왼쪽 Size 800, 0
	if (!m_wndSplitter.CreateView(0,0, RUNTIME_CLASS(CRView), CSize(620,0), pContext))
	{
		//생성하다 실패하면 종료
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}
	//오른쪽  size 0,0 -> 생성된 윈도우의 나머지 부분을 차지한다.
	if (!m_wndSplitter.CreateView(0,1, RUNTIME_CLASS(CControlView), CSize(0,0), pContext))
	{
		// 생성하다 실패하면 종료..
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	//	아래와 같이 변경	
	return TRUE;
}

LRESULT CMainFrame::OnModifyParticle(WPARAM wParam, LPARAM lParam)
{
	g_rView->ModifyParticle(wParam, lParam);
	return 0;
}

void CMainFrame::OnViewStart()
{
	g_rView->OnViewStart();
}

void CMainFrame::OnViewStop()
{
	g_rView->OnViewStop();
}

void CMainFrame::OnViewPause()
{
	g_rView->OnViewPause();
}

void CMainFrame::OnViewBgcolor()
{
	g_rView->OnViewBgcolor();
}

void CMainFrame::OnWireFrame()
{
	g_rView->OnWireFrame();
}

void CMainFrame::OnWireFrameUpdateUI(CCmdUI *pCmdUI)
{
	g_rView->OnWireFrameUpdateUI(pCmdUI);
}
void CMainFrame::OnViewAxis()
{
	g_rView->OnViewAxis();
}

void CMainFrame::OnViewAxisUpdateUI(CCmdUI *pCmdUI)
{
	g_rView->OnViewAxisUpdateUI(pCmdUI);
}

void CMainFrame::OnViewGrid()
{
	g_rView->OnViewGrid();
}

void CMainFrame::OnViewGridUpdateUI(CCmdUI *pCmdUI)
{
	g_rView->OnViewGridUpdateUI(pCmdUI);
}

void CMainFrame::OnViewEmitter()
{
	g_rView->OnViewEmitter();
}

void CMainFrame::OnViewPath()
{
	g_rView->OnViewPath();
}

void CMainFrame::OnViewEmitterUpdateUI(CCmdUI *pCmdUI)
{
	g_rView->OnViewEmitterUpdateUI(pCmdUI);
}

void CMainFrame::OnNew()
{
	g_rView->OnNew();
}

void CMainFrame::OnEffectFileOpen()
{
	g_rView->OnEffectFileOpen();
}

void CMainFrame::OnEffectFileSave()
{
	g_rView->OnEffectFileSave();
}

void CMainFrame::OnEffectFileSaveAs()
{
	g_rView->OnEffectFileSaveAs();
}

void CMainFrame::OnMeshEffectFileOpen()
{
	g_rView->OnMeshEffectFileOpen();
}

void CMainFrame::OnCharFileOpen()
{
	g_rView->OnCharFileOpen();
}
