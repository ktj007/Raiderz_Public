// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "HeightMapRS3.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	m_bIdle = FALSE;
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::Render()
{
	m_wndView.RenderScene();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// 프레임의 클라이언트 영역을 차지하는 뷰를 만듭니다.
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("보기 창을 만들지 못했습니다.\n");
		return -1;
	}
	
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
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

	cs.cx = 1024;
	cs.cy = 768;

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

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// 보기 창으로 포커스를 이동합니다.
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 뷰에서 첫째 크랙이 해당 명령에 나타나도록 합니다.
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// 그렇지 않으면 기본 처리합니다.
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

// DirectX의 초기화
HRESULT CMainFrame::InitDirect3D(void)
{
	HRESULT hr;

	//m_wndView.m_lpD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	//if ( m_wndView.m_lpD3D9 == NULL )
	//	return E_FAIL;

	//D3DPRESENT_PARAMETERS *pd3dpp = &m_wndView.m_kd3dpp;
	//memset(pd3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
	//pd3dpp->Windowed = TRUE;
	//pd3dpp->SwapEffect = D3DSWAPEFFECT_DISCARD;
	//pd3dpp->BackBufferFormat = D3DFMT_UNKNOWN;
	//pd3dpp->AutoDepthStencilFormat = D3DFMT_D16;
	//pd3dpp->EnableAutoDepthStencil = TRUE;

	//hr = m_wndView.m_lpD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_wndView.GetSafeHwnd(),
	//	D3DCREATE_HARDWARE_VERTEXPROCESSING, pd3dpp,&m_wndView.m_lpd3dDevice);
	//if ( FAILED(hr))
	//{
	//	hr = m_wndView.m_lpD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_wndView.GetSafeHwnd(),
	//		D3DCREATE_SOFTWARE_VERTEXPROCESSING, pd3dpp,&m_wndView.m_lpd3dDevice);
	//	if ( FAILED(hr))
	//		return hr;
	//}

	ASSERT(m_wndView.m_pdeviced3d == NULL);

	RECT rt;
	m_wndView.GetClientRect(&rt);

	m_wndView.m_pdeviced3d = new rs3::RDeviceD3D;
	m_wndView.m_pdeviced3d->Create( m_wndView.GetSafeHwnd(), ( rt.right - rt.left), (rt.bottom - rt.top), false, rs3::RFMT_ARGB32 );
	m_wndView.m_pdeviced3d->ResetDeivce();

	LPDIRECT3DDEVICE9 pd3dDevice = m_wndView.m_pdeviced3d->GetD3DDevice();
	// Create Font
	D3DXFONT_DESC kDesc;
	kDesc.CharSet = DEFAULT_CHARSET;
	kDesc.Height = 15;
	kDesc.MipLevels = 1;
	_tcscpy(kDesc.FaceName,_T("Arial"));

	hr = D3DXCreateFontIndirect( pd3dDevice,&kDesc,&m_wndView.m_pFont);
	if ( FAILED(hr))
		return hr;

	hr = D3DXCreateBox( pd3dDevice, 10.0f, 10.0f, 10.0f,&m_wndView.m_pMesh, NULL);
	if ( FAILED(hr))
		return hr;

	// Timer On
	//m_wndView.SetTimer( 1, 1000/30, NULL);
	m_wndView.InitState();
	m_bIdle = TRUE;

	return S_OK;
}


void CMainFrame::ReleaseDirect3D(void)
{
	m_bIdle = FALSE;

	//m_wndView.KillTimer(1);
	//SAFE_DELETE(m_wndView.m_pdeviced3d);

	//  if ( m_wndView.m_lpd3dDevice != NULL )
	//{
	//	m_wndView.m_lpd3dDevice->Release();
	//	m_wndView.m_lpd3dDevice = NULL;
	//}
	//if ( m_wndView.m_lpD3D9 != NULL )
	//{
	//	m_wndView.m_lpD3D9->Release();
	//	m_wndView.m_lpD3D9 = NULL;
	//}
}
BOOL CMainFrame::DestroyWindow()
{
	ReleaseDirect3D();

	return CFrameWnd::DestroyWindow();
}
