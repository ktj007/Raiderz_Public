#include "stdafx.h"
#include "GApplication.h"
#include "resource.h"
#include <commctrl.h>
#include <richedit.h>
#include "GConfig.h"
#include "MStatisticsProfiler.h" // DNC

#pragma comment(lib,"comctl32.lib")

HWND GApplication::m_hDlg = 0;
HWND GApplication::m_hListBox = 0;
vector<MUID> GApplication::m_vecSelectedList;

vector<int> g_qFieldID4Combobox;
int g_nFieldSelectedID=0;

#define MINI_MAP_UPDATE_TICK	(1.0f)		// 1초
#define WM_ICON_NOTIFY			(WM_USER+10)
#define WM_SERVER_RELOAD		(WM_USER+6)
#define MAX_OUTPUT_COUNT		1000

// 리치에디트에 로그 출력한다. 옵션 미구현
void GApplication::AddLogString(const char* szString, COLORREF color, bool bCarrigeReturn)
{
	SetColorRichEdit(GApplication::GetInstance().m_hRichEdLog, color);

	SendMessage(GApplication::GetInstance().m_hRichEdLog, EM_REPLACESEL, FALSE, (LPARAM)szString);
	SendMessage (GApplication::GetInstance().m_hRichEdLog, EM_LINESCROLL, 0, 1) ;
}

void GApplication::AddErrorLogString(const char* szString, COLORREF color)
{
	SetColorRichEdit(m_hRichEdErrorLog, color);

	SendMessage(m_hRichEdErrorLog, EM_REPLACESEL, FALSE, (LPARAM)szString);
	SendMessage(m_hRichEdErrorLog, EM_LINESCROLL, 0, 1) ;
}

void GApplication::SetColorRichEdit(HWND hRichEdit, COLORREF color)
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

GApplication::GApplication(bool bMakeTrayIcon) : 
CServerApplication(),
m_bDrawMinimap(false),
m_bMakeTrayIcon(bMakeTrayIcon)
{
	m_hRichEd = ::LoadLibrary("Riched20.dll");
	m_hCommandLogEdit = 0;
	m_hRichEdLog = 0;
	m_hStatus =0;

	m_nLastTime = 0;

	m_pMinimapRglt = new MRegulator(MINI_MAP_UPDATE_TICK);
}

GApplication::~GApplication()
{
	delete m_pMinimapRglt; m_pMinimapRglt = NULL;
	FreeLibrary(m_hRichEd);
	m_hRichEdLog = 0;
	m_hRichEd = 0;
	Destroy();
}

void GApplication::OnPaint()
{
	return;
}

void GApplication::OutputLogProc(const char* szText, MLogLevel nLevel)
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
	case MLOG_LEVEL_DEBUG:			GApplication::GetInstance().AddLogString(szMsg, RGB(130, 130, 130)); break;
	case MLOG_LEVEL_INFORMATION:	GApplication::GetInstance().AddLogString(szMsg); break;
	case MLOG_LEVEL_WARNING:		GApplication::GetInstance().AddLogString(szMsg, RGB(0, 0, 255)); break;
	case MLOG_LEVEL_ERROR:
		{
			GApplication::GetInstance().AddLogString(szMsg, RGB(255, 0, 0));
			GApplication::GetInstance().AddErrorLogString(szMsg, RGB(255, 0, 0));
		}
		break;
	default:
		GApplication::GetInstance().AddLogString(szMsg);	
	};
}

void GApplication::InitWindowHandles()
{
	m_hRichEdLog = GetDlgItem(GApplication::m_hDlg, IDC_RICHEDIT21);
	m_hRichEdErrorLog = GetDlgItem(GApplication::m_hDlg, IDC_RICHEDIT_ERRORLOG);
}

bool GApplication::OnCreate(LPSTR cmdLine)
{
	InitWindowHandles();

#ifndef _PUBLISH
	MGetDefaultLogger()->SetCustomOutputCallback(GApplication::OutputLogProc);
#endif

	AddLogString("");

	m_Systems.Create();
	m_Managers.Create();
	
	if (!gsys.pServer->Create()) return false;	

	if (SetupTrayIcon() == false) return false;

	if (!GConfig::m_bWindowVisible)
	{
		Show(false);
	}

	return true;
}

void GApplication::OnUpdate()
{
	if (m_nLastTime == 0)
	{
		m_nLastTime = gsys.pSystem->GetNowTime();
		return;
	}
	unsigned int thistime = gsys.pSystem->GetNowTime();
	unsigned int elapsed = thistime - m_nLastTime;
	m_nLastTime = thistime;
	float fDelta = .001f*(float)elapsed;
	if (fDelta > MAX_TICK_DELTA) fDelta = MAX_TICK_DELTA;

	PFI_B(10, "GApplication::OnUpdate - GServer:Update");
	gsys.pServer->Update(fDelta);
	PFI_E(10);
	
}

void GApplication::OnDestroy()
{
	mlog("GApplication > OnDestroy \n");

	if (gsys.pServer)
		gsys.pServer->Destroy();

	m_Managers.Destroy();
	m_Systems.Destroy();

	if (m_bMakeTrayIcon)
		m_TrayIcon.RemoveIcon();

}



// 메인 다이알로그 메세지 핸들러
BOOL CALLBACK GApplication::MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
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



void GApplication::OnMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
			sprintf_s(szTime, "LogServer [%02d/%02d %02d:%02d:%02d]"
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
void GApplication::SetStatusText(int nID, LPCTSTR str)
{
	::SendMessage(m_hStatus, SB_SETTEXT, nID, (LPARAM)str);
}


//////////////////////////////////////////////////////////////////////////
// Message Handler
//////////////////////////////////////////////////////////////////////////

LRESULT GApplication::OnTrayNotification(WPARAM wParam, LPARAM lParam)
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

void GApplication::OnServerStart()
{
}

void GApplication::OnServerStop()
{
}


void GApplication::LogCommand(const char* szCmd)
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

void GApplication::SetServerInfoView(GServerInfo& server_info) 
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
		server_info.nNowPlayer, 
		0);
	SetStatusText(0, (LPCTSTR)text);

	switch (server_info.nStat)
	{
	case SERVER_STAT_ACTIVE: strcpy_s(t1, "ACTIVE"); break;
	case SERVER_STAT_FULL: strcpy_s(t1, "FULL"); break;
	case SERVER_STAT_INCHECK: strcpy_s(t1, "INCHECK"); break;
	case SERVER_STAT_ERROR: strcpy_s(t1, "ERROR"); break;
	default: strcpy_s(t1, ""); break;
	}
}

void GApplication::Show(bool bVisible)
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


void GApplication::AnalysisProfiling()
{
	MSaveProfile(FILENAME_PROFILE, false);	
}

void GApplication::StartProfiling()
{
#ifdef _PUBLISH
	return;
#endif
	
	MInitProfile(true);
	MResetProfile();
}

bool GApplication::SetupTrayIcon()
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