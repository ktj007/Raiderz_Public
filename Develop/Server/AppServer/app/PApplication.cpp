#include "stdafx.h"
#include "PApplication.h"
#include "resource.h"
#include <commctrl.h>
#include <richedit.h>
#include "PConfig.h"
#include "MProfiler.h"
#include "PServer.h"
#include "PCmdHandler_App.h"
#include "SCommandTable_App.h"
#include "PCommandCenter.h"
#include "SAppHelper.h"
#include "MLocale.h"

#pragma comment(lib,"comctl32.lib")

HWND PApplication::m_hListBox = 0;

#define WM_SERVER_RELOAD		(WM_USER+6)
#define MAX_OUTPUT_COUNT		1000

// 리치에디트에 로그 출력한다. 옵션 미구현
void PApplication::AddLogString(const wchar_t* szString, COLORREF color, bool bCarrigeReturn)
{
	SetColorRichEdit(PApplication::GetInstance().m_hRichEdLog, color);

	SendMessage(PApplication::GetInstance().m_hRichEdLog, EM_REPLACESEL, FALSE, (LPARAM)szString);
	SendMessage (PApplication::GetInstance().m_hRichEdLog, EM_LINESCROLL, 0, 1) ;
}

void PApplication::AddErrorLogString(const wchar_t* szString, COLORREF color)
{
	SetColorRichEdit(m_hRichEdErrorLog, color);

	SendMessage(m_hRichEdErrorLog, EM_REPLACESEL, FALSE, (LPARAM)szString);
	SendMessage(m_hRichEdErrorLog, EM_LINESCROLL, 0, 1) ;
}

void PApplication::SetColorRichEdit(HWND hRichEdit, COLORREF color)
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

PApplication::PApplication(bool bMakeTrayIcon) : 
PBaseApplication(),
m_bMakeTrayIcon(bMakeTrayIcon)
{
	m_hRichEd = ::LoadLibrary(L"Riched20.dll");
	m_hRichEdLog = 0;

	m_pApplicationHandles = new SApplicationHandles(m_hWnd, APP_NAME, IDD_SERVER_DIALOG, IDC_RICHEDIT22, IDR_TRAY_MENU);
}

PApplication::~PApplication()
{
	delete m_pApplicationHandles;

	FreeLibrary(m_hRichEd);
	m_hRichEdLog = 0;
	m_hRichEd = 0;
	Destroy();
}

void PApplication::OnPaint()
{
	return;
}

void PApplication::OutputLogProc(const char* szText, MLogLevel nLevel)
{
	DWORD dwCurrentThreadID = GetCurrentThreadId();
	if (PApplication::GetInstance().IsMainThread(dwCurrentThreadID))
	{
		PApplication::GetInstance().OutputLog(MLocale::ConvAnsiToUCS2(szText).c_str(), nLevel);
	}
	else
	{
		if (NULL == gsys.pCommandCenter) return;
		if (NULL == gsys.pCommandCenter->GetNetServer()) return;

		MCommand* pNewCmd = gsys.pCommandCenter->NewCommand(MAPP_LOG_OUT_PUT);
		pNewCmd->AddParameter(NEW_WSTR(MLocale::ConvAnsiToUCS2(szText).c_str()));
		pNewCmd->AddParameter(NEW_INT(nLevel));

		pNewCmd->SetReceiverUID(gsys.pServer->GetUID());
		gsys.pCommandCenter->PostCommand(pNewCmd);
	}
}

void PApplication::OutputLog(const wchar_t* szText, MLogLevel nLevel)
{
	wstring strDateTime = SAppHelper::MakeCurrentDateTimeString();
	wstring strText = strDateTime + L" ";
	strText += szText;

	switch (nLevel)
	{
	case MLOG_LEVEL_DEBUG:			PApplication::GetInstance().AddLogString(strText.c_str(), RGB(130, 130, 130)); break;
	case MLOG_LEVEL_INFORMATION:	PApplication::GetInstance().AddLogString(strText.c_str()); break;
	case MLOG_LEVEL_WARNING:		PApplication::GetInstance().AddLogString(strText.c_str(), RGB(0, 0, 255)); break;
	case MLOG_LEVEL_ERROR:
		{
			PApplication::GetInstance().AddLogString(strText.c_str(), RGB(255, 0, 0));
			PApplication::GetInstance().AddErrorLogString(strText.c_str(), RGB(255, 0, 0));
		}
		break;
	default:
		PApplication::GetInstance().AddLogString(strText.c_str());	
	};
}

void PApplication::InitWindowHandles()
{
	m_hRichEdLog = GetDlgItem(SApplicationHandles::GetDlgHandle(), IDC_RICHEDIT21);
	m_hRichEdErrorLog = GetDlgItem(SApplicationHandles::GetDlgHandle(), IDC_RICHEDIT_ERRORLOG);
}

bool PApplication::OnCreate()
{
	InitWindowHandles();

#ifndef _PUBLISH
	MGetDefaultLogger()->SetCustomOutputCallback(PApplication::OutputLogProc);
#endif

	AddLogString(L"");

	if (PBaseApplication::OnCreate() == false)
	{
		return false;
	}

	if (SetupTrayIcon() == false) return false;

	if (!PConfig::m_bWindowVisible)
	{
		Show(false);
	}

	m_pAppCmdHandler = new PCmdHandler_App(gsys.pCommandCenter->GetNetServer());

	return true;
}

void PApplication::OnDestroy()
{
	SAFE_DELETE(m_pAppCmdHandler);

	PBaseApplication::OnDestroy();

	if (m_bMakeTrayIcon)
		m_TrayIcon.RemoveIcon();
}

void PApplication::OnMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PBaseApplication::OnMsg(hWnd, uMsg, wParam, lParam);
	m_pApplicationHandles->OnMsg(hWnd, uMsg, wParam, lParam);


    switch (uMsg)
    {
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
			case ID_TOOLS_CMD_PROFILE_START:
				{
					gsys.pServer->StartCommandProfile();
				}
				break;
			case ID_TOOLS_CMD_PROFILE_FINISH:
				{
					gsys.pServer->FinishCommandProfile();
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
	}
}

void PApplication::OnServerStart()
{
}

void PApplication::OnServerStop()
{
}


void PApplication::LogCommand(const wchar_t* szCmd)
{
	wchar_t szMsg[512]= L"";
	wstring strDateTime = SAppHelper::MakeCurrentDateTimeString();

	swprintf_s(szMsg, L"%s %s\n", strDateTime.c_str(), szCmd);

	if (m_pApplicationHandles->GetCommandLogEditHandle())
	{
		SendMessage(m_pApplicationHandles->GetCommandLogEditHandle(), EM_REPLACESEL, FALSE, (LPARAM)szMsg);
		SendMessage (m_pApplicationHandles->GetCommandLogEditHandle(), EM_LINESCROLL, 0, 1) ;
	}
}

void PApplication::SetServerInfoView(PServerInfo& server_info) 
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

	wchar_t text[1024]=L"";
	wchar_t t1[1024]=L"";
	swprintf_s(text, L"FPS: %d(%I64d), BPS:R(%.2fk max=%.2fk avr=%.2fk) S(%.2fk max=%.2fk avr=%.2fk), CPS:R(%d) S(%d) L(%d), C: %d, S: %d", 
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

	m_pApplicationHandles->SetStatusText(0, (LPCTSTR)text);

	switch (server_info.nStat)
	{
	case SERVER_STAT_ACTIVE: wcscpy_s(t1, L"ACTIVE"); break;
	case SERVER_STAT_INCHECK: wcscpy_s(t1, L"INCHECK"); break;
	case SERVER_STAT_ERROR: wcscpy_s(t1, L"ERROR"); break;
	default: wcscpy_s(t1, L""); break;
	}
}

void PApplication::Show(bool bVisible)
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



bool PApplication::SetupTrayIcon()
{
	if (m_bMakeTrayIcon)
	{
		if(!m_TrayIcon.Create(m_hWnd, WM_ICON_NOTIFY, _T("AppServer"), NULL, IDR_MAINFRAME))
		{
			mlog3("Tray Icon Creation Failed.\n");
			return false;
		}

		HICON icon = ::LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME));
		m_TrayIcon.SetIcon(icon);
	}
	return true;
}

