#include "stdafx.h"
#include "LApplication.h"
#include "resource.h"
#include <commctrl.h>
#include <richedit.h>
#include "LConfig.h"
#include "MProfiler.h"
#include "LServer.h"

#pragma comment(lib,"comctl32.lib")

HWND LApplication::m_hDlg = 0;
HWND LApplication::m_hListBox = 0;

#define WM_ICON_NOTIFY			(WM_USER+10)
#define WM_SERVER_RELOAD		(WM_USER+6)
#define MAX_OUTPUT_COUNT		1000

// 리치에디트에 로그 출력한다. 옵션 미구현
void LApplication::AddLogString(const char* szString, COLORREF color, bool bCarrigeReturn)
{
	SetColorRichEdit(LApplication::GetInstance().m_hRichEdLog, color);

	SendMessage(LApplication::GetInstance().m_hRichEdLog, EM_REPLACESEL, FALSE, (LPARAM)szString);
	SendMessage (LApplication::GetInstance().m_hRichEdLog, EM_LINESCROLL, 0, 1) ;
}

void LApplication::AddErrorLogString(const char* szString, COLORREF color)
{
	SetColorRichEdit(m_hRichEdErrorLog, color);

	SendMessage(m_hRichEdErrorLog, EM_REPLACESEL, FALSE, (LPARAM)szString);
	SendMessage(m_hRichEdErrorLog, EM_LINESCROLL, 0, 1) ;
}

void LApplication::SetColorRichEdit(HWND hRichEdit, COLORREF color)
{
#ifndef _PUBLISH
	CHARFORMAT2 cf;
	memset(&cf, 0, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(CHARFORMAT2);
	SendMessage(hRichEdit, EM_GETCHARFORMAT, 0, (LPARAM)&cf);

	cf.dwEffects = 0;
	cf.crTextColor = color;
	cf.dwMask = CFM_COLOR;
	SendMessage(hRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
#endif
}

LApplication::LApplication(bool bMakeTrayIcon) : 
CServerApplication(),
m_bMakeTrayIcon(bMakeTrayIcon)
{
	m_hRichEd = ::LoadLibrary("Riched20.dll");
	m_hCommandLogEdit = 0;
	m_hRichEdLog = 0;
	m_hStatus =0;

	m_nLastTime = 0;


}

LApplication::~LApplication()
{
	FreeLibrary(m_hRichEd);
	m_hRichEdLog = 0;
	m_hRichEd = 0;
	Destroy();
}

void LApplication::OnPaint()
{
	return;
}

void LApplication::OutputLogProc(const char* szText, MLogLevel nLevel)
{
	char szTime[64]="";
	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);
	sprintf_s(szTime, "[%02d/%02d %02d:%02d:%02d]:"
		, stCurTime.wMonth
		, stCurTime.wDay
		, stCurTime.wHour
		, stCurTime.wMinute
		, stCurTime.wSecond
		);

	char szMsg[1024]="";
	sprintf_s(szMsg, "%s %s", szTime, szText);

	switch (nLevel)
	{
	case MLOG_LEVEL_DEBUG:			LApplication::GetInstance().AddLogString(szMsg, RGB(130, 130, 130)); break;
	case MLOG_LEVEL_INFORMATION:	LApplication::GetInstance().AddLogString(szMsg); break;
	case MLOG_LEVEL_WARNING:		LApplication::GetInstance().AddLogString(szMsg, RGB(0, 0, 255)); break;
	case MLOG_LEVEL_ERROR:
		{
			LApplication::GetInstance().AddLogString(szMsg, RGB(255, 0, 0));
			LApplication::GetInstance().AddErrorLogString(szMsg, RGB(255, 0, 0));
		}
		break;
	default:
		LApplication::GetInstance().AddLogString(szMsg);	
	};
}

void LApplication::InitWindowHandles()
{
	m_hRichEdLog = GetDlgItem(LApplication::m_hDlg, IDC_RICHEDIT21);
	m_hRichEdErrorLog = GetDlgItem(LApplication::m_hDlg, IDC_RICHEDIT_ERRORLOG);
}

bool LApplication::OnCreate(LPSTR cmdLine)
{
	InitWindowHandles();

#ifndef _PUBLISH
	MGetDefaultLogger()->SetCustomOutputCallback(LApplication::OutputLogProc);
#endif

	AddLogString("");

	m_Systems.Create();
	m_Managers.Create();

	if (!m_Systems.pServer->Create()) return false;	

	if (SetupTrayIcon() == false) return false;

	if (!LConfig::m_bWindowVisible)
	{
		Show(false);
	}

	return true;
}

void LApplication::OnUpdate()
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
	if (fDelta > MAX_TICK_DELTA) fDelta = MAX_TICK_DELTA;

	m_Systems.pServer->Update(fDelta);
}

void LApplication::OnDestroy()
{
	m_Systems.pServer->Destroy();

	m_Managers.Destroy();
	m_Systems.Destroy();

	if (m_bMakeTrayIcon)
		m_TrayIcon.RemoveIcon();

}



// 메인 다이알로그 메세지 핸들러
BOOL CALLBACK LApplication::MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg,0);
			return TRUE;
		}break;
	}

	return FALSE;
}



void LApplication::OnMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CServerApplication::OnMsg(hWnd, uMsg, wParam, lParam);

    switch (uMsg)
    {
	case WM_CREATE:
		{
			// 서버 시작 시간 표시
			char szTime[64]="";
			SYSTEMTIME stCurTime;
			GetLocalTime(&stCurTime);
			sprintf_s(szTime, "%s [%02d/%02d %02d:%02d:%02d]"
				, APP_NAME
				, stCurTime.wMonth
				, stCurTime.wDay
				, stCurTime.wHour
				, stCurTime.wMinute
				, stCurTime.wSecond );
			SetWindowText(hWnd, szTime);

			// 메인 다이알로그 생성
			HWND hDlg = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SERVER_DIALOG), hWnd, MainDlgProc);
			m_hDlg = hDlg;
			ShowWindow(hDlg, SW_SHOW);

			// 상태창 생성
			m_hStatus = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "Status Line", hWnd, 0);
			
			RECT rt;
			GetWindowRect(m_hStatus, &rt);
			int arPart[2];
			arPart[0] = 750;
			arPart[1] = rt.right;

			SendMessage(m_hStatus, SB_SETPARTS, 2, (LPARAM)arPart);

			m_hCommandLogEdit = GetDlgItem(hDlg, IDC_RICHEDIT22);
			
			SetStatusText(0, "");
			SetStatusText(1, "Active");

			break;
		}
	case WM_KEYDOWN:
		{
#ifdef _DEBUG
			if(wParam==VK_ESCAPE) exit(0);
#endif
		}
		break;
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case ID_MENU_START:
				OnServerStart();
				break;
			case ID_MENU_STOP:
				OnServerStop();
				break;
			case ID_MENU_PROFILE_START:
				{
					StartProfiling();					
				}
				break;
			case ID_TOOLS_PROFILE_FINISH:
				{
					AnalysisProfiling();					
				}
				break;
			case ID_MENU_EXIT:
			case ID_POPUP_QUIT:
				PostQuitMessage(0);
				break;
			case ID_POPUP_OPEN:
				Show(true);
				break;
			case ID_POPUP_HIDE:
				Show(false);
				break;
			}
		}
		break;

	case WM_ICON_NOTIFY:
		{
			OnTrayNotification(wParam, lParam);	
		}
	case WM_PAINT:
		{
			OnPaint();
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Message Sender
//////////////////////////////////////////////////////////////////////////
void LApplication::SetStatusText(int nID, LPCTSTR str)
{
	::SendMessage(m_hStatus, SB_SETTEXT, nID, (LPARAM)str);
}


//////////////////////////////////////////////////////////////////////////
// Message Handler
//////////////////////////////////////////////////////////////////////////

LRESULT LApplication::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(lParam) == WM_RBUTTONUP)
	{
		HMENU menu = ::LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_TRAY_MENU));
		HMENU submenu = GetSubMenu(menu, 0);
		
		POINT pt;
		GetCursorPos(&pt);

		SetForegroundWindow(m_hWnd);

		TrackPopupMenu(submenu, TPM_LEFTALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
		PostMessage(m_hWnd, WM_NULL, 0, 0);
	}
	else if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
	{
		if (IsWindowVisible(m_hWnd)) ShowWindow(m_hWnd, SW_HIDE);
		else ShowWindow(m_hWnd, SW_SHOW);
	}
	return 1;
}

void LApplication::OnServerStart()
{
}

void LApplication::OnServerStop()
{
}


void LApplication::LogCommand(const char* szCmd)
{
	char szMsg[512]= "";
	char szTime[256] = "";

	//시간
	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);

	sprintf_s(szTime, "[%02d:%02d:%02d.%03d]:"
		, stCurTime.wHour
		, stCurTime.wMinute
		, stCurTime.wSecond
		, stCurTime.wMilliseconds);

	sprintf_s(szMsg, "%s %s\n", szTime, szCmd);

	if (m_hCommandLogEdit)
	{
		SendMessage(m_hCommandLogEdit, EM_REPLACESEL, FALSE, (LPARAM)szMsg);
		SendMessage (m_hCommandLogEdit, EM_LINESCROLL, 0, 1) ;
	}
}

void LApplication::SetServerInfoView(LServerInfo& server_info) 
{
	server_info.nMaxRecvBPS = max(server_info.nMaxRecvBPS, server_info.nRecvBPS);
	server_info.nMaxSendBPS = max(server_info.nMaxSendBPS, server_info.nSendBPS);
	
	server_info.nTotalRecvBPS += server_info.nRecvBPS;
	server_info.nTotalSendBPS += server_info.nSendBPS;
	server_info.nFrameIndex++;

	float fRecvBPSk = server_info.nRecvBPS / (float)1024;
	float fSendBPSk = server_info.nSendBPS / (float)1024;
	float fMaxRecvBPSk = server_info.nMaxRecvBPS / (float)1024;
	float fMaxSendBPSk = server_info.nMaxSendBPS / (float)1024;
	float fAvrRecvBPSk = server_info.nTotalRecvBPS / (float)server_info.nFrameIndex / (float)1024;
	float fAvrSendBPSk = server_info.nTotalSendBPS / (float)server_info.nFrameIndex / (float)1024;

	char text[1024]="";
	char t1[1024]="";
	sprintf_s(text, "FPS: %d(%I64d), BPS:R(%.2fk max=%.2fk avr=%.2fk) S(%.2fk max=%.2fk avr=%.2fk), CPS:R(%d) S(%d) L(%d), C: %d, S: %d", 
		server_info.nFPS, 
		server_info.nFrameIndex,
		fRecvBPSk, 
		fMaxRecvBPSk,
		fAvrRecvBPSk,
		fSendBPSk,
		fMaxSendBPSk,
		fAvrSendBPSk,
		server_info.nRecvCPS, 
		server_info.nSendCPS, 
		server_info.nLocalCPS,
		server_info.nCurrGameServerCount, 
		0);
	SetStatusText(0, (LPCTSTR)text);

	switch (server_info.nStat)
	{
	case SERVER_STAT_ACTIVE: strcpy_s(t1, "ACTIVE"); break;
	case SERVER_STAT_INCHECK: strcpy_s(t1, "INCHECK"); break;
	case SERVER_STAT_ERROR: strcpy_s(t1, "ERROR"); break;
	default: strcpy_s(t1, ""); break;
	}
}

void LApplication::Show(bool bVisible)
{
	if (bVisible)
	{
		ShowWindow(m_hWnd, SW_SHOW);
	}
	else
	{
		ShowWindow(m_hWnd, SW_HIDE);
	}
}


void LApplication::AnalysisProfiling()
{
	MSaveProfile(FILENAME_PROFILE, false);	
}

void LApplication::StartProfiling()
{
#ifdef _PUBLISH
	return;
#endif
	
	MInitProfile(true);
	MResetProfile();
}

bool LApplication::SetupTrayIcon()
{
	if (m_bMakeTrayIcon)
	{
		if(!m_TrayIcon.Create(m_hWnd, WM_ICON_NOTIFY, _T("GameServer"), NULL, IDR_MAINFRAME))
		{
			mlog3("Tray Icon Creation Failed.\n");
			return false;
		}

		HICON icon = ::LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME));
		m_TrayIcon.SetIcon(icon);
	}
	return true;
}