#include "StdAfx.h"
#include "RD3DApplication.h"
#include "RDeviceD3D.h"
#include "RProfileInfo.h"

namespace rs3 {

#define RS3_CLASS_NAME	"RealSpace3"

RD3DApplication::RD3DApplication(void) : m_nIconResID(0)
{
}

RD3DApplication::~RD3DApplication(void)
{
}

//#define TRACE_ACTIVATE_MESSAGE
LRESULT CALLBACK RD3DApplication::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result;

	RD3DApplication* pd3dApplication = (RD3DApplication*)RD3DApplication::GetInstancePtr();
	if(pd3dApplication && pd3dApplication->OnWndMessage(hWnd,message,wParam,lParam,&result))
		return result;

    // Handle messages
    switch (message)
    {
		case WM_SYSCHAR:
			if(wParam==VK_RETURN)
			{
				((RD3DApplication*)RD3DApplication::GetInstancePtr())->ToggleFullScreen();
			}
			return 0;

        case WM_SYSCOMMAND:
            // Prevent moving/sizing and power loss in fullscreen mode
            switch( wParam )
            {
                case SC_MOVE:
                case SC_SIZE:
                case SC_MAXIMIZE:
                case SC_KEYMENU:
                case SC_MONITORPOWER:
					if( RApplication::GetInstance().IsFullScreen() )
                        return 1;
                    break;
            }
            break;
#ifdef TRACE_ACTIVATE_MESSAGE
		case WM_ACTIVATE:
			mlog("WM_ACTIVATE %s\n", wParam != WA_INACTIVE ? "true" : "false");
			break;
#endif
		case WM_NCACTIVATE:

#ifdef TRACE_ACTIVATE_MESSAGE
			mlog("WM_NCACTIVATE %s\n", wParam != FALSE ? "true" : "false");
#endif			
			// 처음 deactive 인 상태로 실행되서 생기는 경우이다 
			if( !RApplication::GetInstance().IsActive() && wParam != FALSE )
				RApplication::GetInstance().Activate( TRUE );

// 			return FALSE;
			break;
		case WM_ACTIVATEAPP:
		{
#ifdef TRACE_ACTIVATE_MESSAGE
			mlog("WM_ACTIVATEAPP %s\n", wParam != FALSE ? "true" : "false");
#endif
			RApplication::GetInstance().Activate( wParam != FALSE );
//			return 0;
		}
		break;
		case WM_DESTROY:
		break;

		case WM_CLOSE:
            PostQuitMessage(0);
		break;

		case WM_SETCURSOR:{
			// 디바이스 커서를 사용중이면
			if(REngine::GetDevicePtr()->IsCursorVisible())
			{
				// Turn off window cursor. 
				SetCursor( NULL );
				REngine::GetDevicePtr()->ShowCursor( true );
				return TRUE; // Prevent Windows from setting cursor to window class cursor.
			}
			break;
		  }

		default:
			break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void RD3DApplication::AdjustWindowSizeAndShowWindow( int x, int y, int nWidth, int nHeight, bool bFullScreen)
{
	DWORD dwWindowStyle = GetWindowStyle(bFullScreen);
	SetWindowLong( m_hWnd, GWL_STYLE, dwWindowStyle );

	RECT r = { x, y, x + nWidth, y + nHeight};
	AdjustWindowRect( &r, dwWindowStyle, FALSE);

	MoveWindow( m_hWnd, x, y, r.right - r.left, r.bottom - r.top, TRUE);


	ShowWindow( m_hWnd, SW_SHOWNOACTIVATE );
	UpdateWindow( m_hWnd);
}

RSceneManager* RD3DApplication::CreateSceneManager()
{
	return RApplication::CreateSceneManager();
}

bool RD3DApplication::CreateDevice()
{
	// 이미 윈도우가 생성되어있으면 실패.
	if(m_hWnd)
	{
		mlog_setlasterror("Window is already created.\n");
		return false;
	}

	HINSTANCE hInstance = GetModuleHandle(0);

	// make a window
    WNDCLASS    wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(DWORD);
    wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE(m_nIconResID));
    wc.hCursor = 0;
	wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = RS3_CLASS_NAME;
	if(!RegisterClass(&wc))
	{
		mlog_setlasterror("Fail to register class.\n");
		return false;
	}

	DWORD dwWindowStyle = GetWindowStyle(REngine::GetConfig().m_bFullScreen);

	// Set the window's initial width
	RECT rc;
	SetRect( &rc, 0, 0, REngine::GetConfig().m_nWidth, REngine::GetConfig().m_nHeight );
	AdjustWindowRect( &rc, dwWindowStyle, FALSE );

    m_hWnd = CreateWindow( RS3_CLASS_NAME, GetName() , dwWindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, 
			(rc.right-rc.left), (rc.bottom-rc.top), NULL, NULL, hInstance , NULL );

	// 숨겨진채로 윈도우 생성, 크기 조정후 보여준다

	GetWindowRect(m_hWnd,&m_rcWindowBounds);
	AdjustWindowSizeAndShowWindow(REngine::GetConfig().m_nInitX, REngine::GetConfig().m_nInitY, 
			REngine::GetConfig().m_nWidth,REngine::GetConfig().m_nHeight,REngine::GetConfig().m_bFullScreen);

	m_bActive = (GetForegroundWindow() == m_hWnd);

	_ASSERT(m_pDevice==NULL);

	RDeviceD3D *pd3dDevice = new RDeviceD3D;
	m_pDevice = pd3dDevice;

	// by pok, 씬매니저 변경중
	if(!pd3dDevice->Create(m_hWnd))
		return false;

	REngine::RegisterDevice(m_pDevice);

	return true;
}

void RD3DApplication::DestroyDevice()
{
	if (m_pDevice)
	{
		m_pDevice->Destroy();
		SAFE_DELETE(m_pDevice);
		REngine::RegisterDevice(NULL);
	}

	// by pok, 씬매니저 변경중
	if(m_hWnd) {
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

bool RD3DApplication::ProcessMessage(MSG&  msg)
{
	BOOL bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
	if( bGotMsg ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
		return true;
	}

	return false;
}

int RD3DApplication::MainLoop()
{
	RPFC_RESET;
	RPFC_GPU_RESET;

    MSG  msg;

	do
    {
		RPFC_B("rs3::RD3DApplication::MainLoop");

		RPFC_B("rs3::RD3DApplication::MainLoop::Sleep");
		if(!m_bActive)
			Sleep(10);
		RPFC_E;

		bool bExistMessage = true;
		if( !ProcessMessage(msg) )
		{
			Update();
			Render();
			bExistMessage = false;
		}
		RPFC_E;

		if( false == bExistMessage )
		{
		#ifdef _USE_ENGINE_CPU_PROFILE
			m_DebuggingInfo.SendOut(m_hWnd);
		#endif
		#ifdef _USE_ENGINE_RESOURCE_PROFILE
			m_DebuggingInfo.SendOut(m_hWnd);
		#endif
		}

		RPFC_REST_CURRENTINFO;

	}while( WM_QUIT != msg.message );

    return (INT)msg.wParam;
}

void RD3DApplication ::Activate(bool bActive)
{
	m_bActive = bActive;
	OnActive(bActive);

	if ((!m_bActive) && (IsFullScreen())) {
		ShowWindow(m_hWnd, SW_MINIMIZE);
		UpdateWindow(m_hWnd);
	}
}

void RD3DApplication::OnLostDevice()
{
	mlog("RD3DApplication::OnLostDevice\n");
}

void RD3DApplication::OnResetDevice()
{
	mlog("RD3DApplication::OnResetDevice\n");
	AdjustWindowSizeAndShowWindow(0, 0, REngine::GetConfig().m_nWidth,REngine::GetConfig().m_nHeight, REngine::GetConfig().m_bFullScreen);
}

void RD3DApplication::ToggleFullScreen()
{
	REngine::GetConfig().m_bFullScreen =! REngine::GetConfig().m_bFullScreen;

	// 풀 스크린 모드로 전환 될 경우 디스플레이가 지원하는 해상도인지 체크한다.
	_ASSERT( m_pDevice );
	if( REngine::GetConfig().m_bFullScreen == true )
	{
		if( false == m_pDevice->IsCurrentDisplayModeSupportFullScreenMode() )
		{
			REngine::GetConfig().m_bFullScreen = false;
			mlog( "Full screen mode change failed.\nCurrent display mode is not support full screen mode.\n" );
			return;
		}
	}

	ResetDevice();

	AdjustWindowSizeAndShowWindow(0, 0, REngine::GetConfig().m_nWidth,REngine::GetConfig().m_nHeight, REngine::GetConfig().m_bFullScreen);

	if( REngine::GetConfig().m_bFullScreen == false )
	{
		// 풀스크린 -> 윈도우로 전환하고 나면 "항상 위" 속성이 켜지는것을 해제한다
		// "항상 위" 스타일을 토글 할 때는 SetWindowLong?으로 SWL_EXSTYLE을 조작하지 말고 반드시 SetWindowPos 함수를 사용해야 한다.
		SetWindowPos(m_hWnd, HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
}

DWORD RD3DApplication::GetWindowStyle(bool bFullScreen)
{
	DWORD dwWindowStyle;

	if (bFullScreen)
	{
		dwWindowStyle = WS_POPUP | WS_SYSMENU | WS_VISIBLE;
	}
	else
	{
		dwWindowStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU;
	}

	return dwWindowStyle;
}

} // namespace rs3 {