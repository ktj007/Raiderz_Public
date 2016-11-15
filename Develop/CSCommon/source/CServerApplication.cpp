#include "stdafx.h"
#include "CServerApplication.h"
#include "conio.h"
#include <mmsystem.h>
#include <signal.h>

CServerApplication* CServerApplication::m_pInstance = NULL;

LRESULT CALLBACK CServerApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (m_pInstance) m_pInstance->OnMsg( hWnd, uMsg, wParam, lParam );
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

CServerApplication::CServerApplication()
: m_hWnd(0)
, m_nAppType(APPTYPE_WINDOW)
, m_bRun(true)
, m_nLastTime(0)
{
	m_pInstance = this;
	m_dwMainThreadID = GetCurrentThreadId();
}

CServerApplication::~CServerApplication()
{
	m_pInstance = NULL;
}

bool CServerApplication::Create(int x, int y, int width, int height, const char* szClassName, const char* szName, UINT nMenuID, UINT nIconID)
{
	m_nAppType = APPTYPE_WINDOW;
	m_CommandLine.ParseFromCommandLine();

	if (!InitMainWnd(x, y, width, height, szClassName, szName, nMenuID, nIconID)) return false;

	bool ret = OnCreate();
	return ret;
}

void CServerApplication::Destroy()
{
	OnDestroy();
}



void CServerApplication::Update()
{
	if (m_nLastTime == 0)
	{
		m_nLastTime = timeGetTime();
		return;
	}
	unsigned int thistime = timeGetTime();
	unsigned int elapsed = thistime - m_nLastTime;
	m_nLastTime = thistime;
	float fDelta = .001f*(float)elapsed;
	if (fDelta > MAX_SERVER_TICK_DELTA) fDelta = MAX_SERVER_TICK_DELTA;

	OnUpdate(fDelta);
}

int CServerApplication::Run()
{
	switch (m_nAppType)
	{
	case APPTYPE_WINDOW: return Run_Window();
	case APPTYPE_CONSOLE: return Run_Console();
	case APPTYPE_SERVICE: return Run_Console();
	}
	return 0;
}

bool CServerApplication::InitMainWnd(int x, int y, int width, int height, const char* szClassName, const char* szName, UINT nMenuID, UINT nIconID)
{
	if(m_hWnd) return false;

	HINSTANCE hInstance = GetModuleHandle(0);

	// make a window
    WNDCLASS    wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MsgProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(DWORD);
    wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE(nIconID));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szClassName;
	wc.lpszMenuName = MAKEINTRESOURCE(nMenuID);
	if(!RegisterClass(&wc)) return FALSE;

	DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    // Set the window's initial width
    RECT rc;
    SetRect( &rc, 0, 0, width, height );
    AdjustWindowRect( &rc, dwStyle, FALSE );


    m_hWnd = CreateWindow( szClassName, szName , dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, 
			width, height, NULL, NULL, hInstance , NULL );

	DWORD hr = GetLastError();

    ShowWindow( m_hWnd, SW_SHOWNORMAL );
    UpdateWindow( m_hWnd );

	return true;
}


void CServerApplication::OnMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch( uMsg )
    {
	case WM_PAINT:
		OnPaint();
		break;
	case WM_SIZING:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		break;
	case WM_CLOSE:
		DestroyWindow( m_hWnd );
		PostQuitMessage(0);
		break;
    }
}

bool CServerApplication::CreateConsole( const char* szName )
{
	CatchTerminationSignal(CServerApplication::SignalHandler);

	m_nAppType = APPTYPE_CONSOLE;
	m_CommandLine.ParseFromCommandLine();

	bool ret = OnCreate();
	return ret;
}

int CServerApplication::Run_Window()
{
	MSG msg;
	BOOL bGotMsg = FALSE;

	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

	while (WM_QUIT != msg.message && m_bRun)
	{
		bGotMsg = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

		if (bGotMsg)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			//Sleep(1);
		}
	}

	return (int) msg.wParam;
}


int CServerApplication::Run_Console()
{
	int key = 0;
	do 
	{
#ifdef _DEBUG
		if (_kbhit())
		{
			key = _getch();

			switch (key)
			{
			case 27:		// esc Ű
				{
					Quit();
					break;
				}
			}
		};
#endif

		Update();
	} while (m_bRun);

	return 1;
}

void CServerApplication::SignalHandler( int signo )
{
	if (m_pInstance)
	{
		m_pInstance->Quit();
	}
}

void CServerApplication::CatchTerminationSignal( void handler(int) )
{
	signal( SIGINT,		CServerApplication::SignalHandler);
	signal( SIGBREAK,	CServerApplication::SignalHandler);
}