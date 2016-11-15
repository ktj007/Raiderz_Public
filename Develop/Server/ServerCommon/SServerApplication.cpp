#include "stdafx.h"
#include "SServerApplication.h"
#include "conio.h"
#include <mmsystem.h>
#include <signal.h>
#include "SServerMonitorEventListener.h"
#include "MLocale.h"
#include "MIOCP.h"
#include "MCommand.h"

SServerApplication* SServerApplication::m_pInstance = NULL;

LRESULT CALLBACK SServerApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (m_pInstance) m_pInstance->OnMsg( hWnd, uMsg, wParam, lParam );
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

SServerApplication::SServerApplication()
: m_hWnd(0)
, m_nAppType(APPTYPE_WINDOW)
, m_bRun(true)
, m_nLastTime(0)
, m_pServerMonitorEventListener(NULL)
, m_iocp(NULL)
{
	m_pInstance = this;
	m_dwMainThreadID = GetCurrentThreadId();
}

SServerApplication::~SServerApplication()
{
	m_pInstance = NULL;
}

bool SServerApplication::Create(int x, int y, int width, int height, const wchar_t* szClassName, const wchar_t* szName, UINT nMenuID, UINT nIconID)
{
	m_nAppType = APPTYPE_WINDOW;
	m_CommandLine.ParseFromCommandLine();

	
	if (!InitMainWnd(x, y, width, height, szClassName, szName, nMenuID, nIconID)) return false;

	bool ret = OnCreate();
	return ret;
}

void SServerApplication::Destroy()
{
	OnDestroy();

	SAFE_DELETE(m_pServerMonitorEventListener);
}



bool SServerApplication::Update()
{
	if (m_nLastTime == 0)
	{
		m_nLastTime = timeGetTime();
		return true;
	}
	unsigned int thistime = timeGetTime();
	unsigned int elapsed = thistime - m_nLastTime;
	m_nLastTime = thistime;
	float fDelta = .001f*(float)elapsed;
	if (fDelta > MAX_SERVER_TICK_DELTA) fDelta = MAX_SERVER_TICK_DELTA;

	if (!OnUpdate(fDelta))
		return false;

	CheckServerMonitorEvent();

	return true;
}

int SServerApplication::Run(MIOCP* const iocp)
{
	m_iocp = iocp;
	switch (m_nAppType)
	{
	case APPTYPE_WINDOW: return Run_Window();
	case APPTYPE_CONSOLE: return Run_Console();
	case APPTYPE_SERVICE: return Run_Console();
	}
	return 0;
}

bool SServerApplication::InitMainWnd(int x, int y, int width, int height, const wchar_t* szClassName, const wchar_t* szName, UINT nMenuID, UINT nIconID)
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


void SServerApplication::OnMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		CloseServer();
		break;
    }
}

bool SServerApplication::CreateConsole(const wchar_t* szName, wstring strStartParameter)
{
	CatchTerminationSignal(SServerApplication::ConsoleCloseHandler);

	m_nAppType = APPTYPE_CONSOLE;

	if (strStartParameter.empty())
	{
		m_CommandLine.ParseFromCommandLine();
	}
	else
	{
		m_CommandLine.ParseFromString(MLocale::ConvUTF16ToAnsi(strStartParameter.c_str()));
	}

	bool ret = OnCreate();
	return ret;
}

int SServerApplication::Run_Window()
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
			if (NULL != m_iocp)
			{
				if (false == m_iocp->Process())
					return 0;
			}

			if (!Update())
				return 0;
			//Sleep(1);
		}
	}

	return (int) msg.wParam;
}


int SServerApplication::Run_Console()
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
			case 27:		// esc 키
				{
					CloseServer();
					break;
				}
			}
		};
#endif

		if (NULL != m_iocp)
		{
			if (false == m_iocp->Process())
				return 0;
		}

		if (!Update())
			return 0;

	} while (m_bRun);

	return 1;
}

BOOL WINAPI SServerApplication::ConsoleCloseHandler(DWORD CtrlType)
{
	if (m_pInstance)
	{
		m_pInstance->CloseServer();
	}

	return TRUE;
}

void SServerApplication::CatchTerminationSignal(PHANDLER_ROUTINE handler)
{
	// TODO: 콘솔 애플리케이션의 닫기버튼을 막아야한다. MSDN에서 시키는데로 했는데 안된다. [2010/9/2 praptor]

	//signal( SIGINT,		SServerApplication::ConsoleCloseHandler);
	//signal( SIGBREAK,	SServerApplication::ConsoleCloseHandler);	

	SetConsoleCtrlHandler(handler, TRUE);
}

bool SServerApplication::StartServerMonitorEventListener(const wchar_t* szServerName)
{
	m_pServerMonitorEventListener = new SServerMonitorEventListener();
	if (false == m_pServerMonitorEventListener->Create(szServerName))
		return false;

	return true;
}

void SServerApplication::CheckServerMonitorEvent()
{
#ifdef _PUBLISH
	return;
#endif

	if (NULL == m_pServerMonitorEventListener) return;
	if (!m_bRun) return;

	if (m_pServerMonitorEventListener->CheckSignal_Terminate())
	{
		CloseServer();
	}
}

void SServerApplication::Quit()
{ 
	if (m_nAppType == APPTYPE_WINDOW)
	{
		DestroyWindow( m_hWnd );
		PostQuitMessage(0);
	}

	m_bRun = false;	
}

void SServerApplication::CloseServer()
{
	Quit();
}

void SServerApplication::MsgHandler::OnIO(const DWORD& numOfByteTransfered,OVERLAPPED* const overlapped)
{
	IOMSG* const msg = static_cast<IOMSG*>(overlapped);
	if (NULL == msg)
	{
		mlog("SServerApplication::MsgHandler::OnIO() - error null message\n");
		return;
	}
	if (NULL == msg->cmd)
	{
		mlog("SServerApplication::MsgHandler::OnIO() - error null cmd\n");
		return;
	}
	OnRecvCmd(msg->cmd);

	delete msg->cmd;
	delete msg;
}

void SServerApplication::MsgHandler::OnError(const int& errCode)
{
	mlog("error in SServerApplication::MsgHandler::OnError()\n");	// 에러 날 일이없다(아마도)
}

void SServerApplication::RegisterMsgHandler(const MsgHandler& handler) const
{
	if (NULL == m_iocp)
		return;
	m_iocp->Register(handler);
}

void SServerApplication::SendCmd(const MsgHandler& handler,minet::MCommand* const cmd) const
{
	if (NULL == m_iocp)
		return;
	IOMSG* msg = new IOMSG;
	msg->cmd = cmd;
	m_iocp->Post(handler, msg);
}
