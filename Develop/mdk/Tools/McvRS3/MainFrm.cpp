#include "stdafx.h"
#include "MainFrm.h"
#include "RDirectionalLightSceneNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_BKCOLOR,		OnBkcolor)
	ON_COMMAND(ID_PARTS_COLOR, OnPartsColor)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bActivate = FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP	
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||	
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	m_wndToolBar.ShowWindow( SW_HIDE);


	m_outputBar.Create(this,IDD_DIALOG_OUTPUT,CBRS_BOTTOM,0);
	m_outputBar.ShowWindow(SW_SHOW);

	m_cFrameControlBar.Create(this,IDD_DIALOG_FRAMECONTROL,CBRS_BOTTOM,0);
	m_cFrameControlBar.ShowWindow(SW_SHOW);

	m_controlBar.Create(this,IDD_DIALOG_CONTROL,CBRS_RIGHT,0);
	m_controlBar.ShowWindow( SW_SHOW);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.cx = 800;
	cs.cy = 800;
	cs.style &= ~FWS_ADDTOTITLE; 

	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	CString str("Maiet Charactor Viewer");
	m_strTitle = str;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

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

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
}

void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CFrameWnd::OnActivateApp(bActive, dwThreadID);
	m_bActivate = bActive;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if(m_outputBar.m_hWnd)
	{
		RECT rt;
		m_outputBar.GetClientRect(&rt);
		CWnd* pWnd = m_outputBar.GetDlgItem(IDC_EDIT_OUTPUT);
		pWnd->MoveWindow(&rt);
	}
	if(GetActiveView())
	{
		RECT rt;
		GetActiveView()->GetClientRect(&rt);

		GetMainView()->Resize(rt.right-rt.left, rt.bottom-rt.top);

		int a = 0;
	}
}

void CMainFrame::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	GetMainView()->ClearActor();
	CFrameWnd::OnClose();
}

void CMainFrame::OnPartsColor()
{
	CColorDialog dlg;
	COLORREF col;

	BYTE a,r,g,b;

	if( dlg.DoModal()==IDOK ) {
		col = dlg.GetColor();

		a = (col>>24)&0xff;
		b = (col>>16)&0xff;
		g = (col>> 8)&0xff;
		r = (col    )&0xff;

		col = D3DCOLOR_ARGB(255,r,g,b);

		RActor* pActor = GetMainView()->GetActor();
		for(int i=0;i<pActor->GetActorNodeCount();i++)
		{
			RActorNode* pActorNode = pActor->GetActorNodes()[i];
			pActorNode->SetPartsColor(col);
		}
	}
}

void CMainFrame::OnBkcolor()
{
	CColorDialog dlg;
	COLORREF col;

	BYTE a,r,g,b;

	if( dlg.DoModal()==IDOK ) {
		col = dlg.GetColor();

		a = (col>>24)&0xff;
		b = (col>>16)&0xff;
		g = (col>> 8)&0xff;
		r = (col    )&0xff;

		col = D3DCOLOR_ARGB(255,r,g,b);

		//SetClearColor((DWORD)col);
		GetMain()->m_Camera.SetClearColor((DWORD)col);
	}
}
