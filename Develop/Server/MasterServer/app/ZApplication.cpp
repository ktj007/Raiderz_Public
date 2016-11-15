#include "stdafx.h"
#include "Resource.h"
#include "ZApplication.h"
#include <commctrl.h>
#include <richedit.h>
#include "ZConfig.h"
#include "MProfiler.h"
#include "ZServer.h"
#include "ZCmdHandler_App.h"
#include "SCommandTable_App.h"
#include "ZCommandCenter.h"
#include "ZSystem.h"
#include "SAppHelper.h"
#include <tchar.h>
#include <atlconv.h>
#include <atlexcept.h>
#include "MLocale.h"

#pragma comment(lib,"comctl32.lib")

HWND ZApplication::m_hDlg = 0;
HWND ZApplication::m_hListBox = 0;

#define WM_DESTROY_SERVER		(WM_USER+25)
#define WM_ICON_NOTIFY			(WM_USER+10)
#define WM_SERVER_RELOAD		(WM_USER+6)
#define MAX_OUTPUT_COUNT		1000

// 리치에디트에 로그 출력한다. 옵션 미구현
void ZApplication::AddLogString(const wchar_t* szString, COLORREF color, bool bCarrigeReturn)
{
	SetColorRichEdit(ZApplication::GetInstance().m_hRichEdLog, color);

	SendMessage(ZApplication::GetInstance().m_hRichEdLog, EM_REPLACESEL, FALSE, (LPARAM)szString);
	SendMessage (ZApplication::GetInstance().m_hRichEdLog, EM_LINESCROLL, 0, 1) ;
}

void ZApplication::AddErrorLogString(const wchar_t* szString, COLORREF color)
{
	SetColorRichEdit(m_hRichEdErrorLog, color);

	SendMessage(m_hRichEdErrorLog, EM_REPLACESEL, FALSE, (LPARAM)szString);
	SendMessage(m_hRichEdErrorLog, EM_LINESCROLL, 0, 1) ;
}

void ZApplication::SetColorRichEdit(HWND hRichEdit, COLORREF color)
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

ZApplication::ZApplication(bool bMakeTrayIcon) : 
ZBaseApplication(),
m_bMakeTrayIcon(bMakeTrayIcon),
m_pAppCmdHandler(NULL),
m_pMonitorEd(NULL)
{
	m_hRichEd = ::LoadLibrary(L"Riched20.dll");
	m_hCommandLogEdit = 0;
	m_hRichEdLog = 0;
	m_hStatus =0;
}

ZApplication::~ZApplication()
{
	FreeLibrary(m_hRichEd);
	m_hRichEdLog = 0;
	m_hRichEd = 0;
	Destroy();
}

void ZApplication::OnPaint()
{
	return;
}

void ZApplication::OutputLogProc(const char* szText, MLogLevel nLevel)
{
	DWORD dwCurrentThreadID = GetCurrentThreadId();
	if (ZApplication::GetInstance().IsMainThread(dwCurrentThreadID))
	{
		ZApplication::GetInstance().OutputLog(MLocale::ConvAnsiToUCS2(szText).c_str(), nLevel);
	}
	else
	{
		if (NULL == gsys.pCommandCenter) return;

		MCommand* pNewCmd = gsys.pCommandCenter->NewCommand(MAPP_LOG_OUT_PUT);
		pNewCmd->AddParameter(NEW_WSTR(MLocale::ConvAnsiToUCS2(szText).c_str()));
		pNewCmd->AddParameter(NEW_INT(nLevel));

		pNewCmd->SetReceiverUID(gsys.pServer->GetUID());
		gsys.pCommandCenter->PostCommand(pNewCmd);
	}
}

void ZApplication::OutputLog(const wchar_t* szText, MLogLevel nLevel)
{
	wstring strDateTime = SAppHelper::MakeCurrentDateTimeString();
	wstring strText = strDateTime + L" ";
	strText += szText;

	switch (nLevel)
	{
	case MLOG_LEVEL_DEBUG:			ZApplication::GetInstance().AddLogString(strText.c_str(), RGB(130, 130, 130)); break;
	case MLOG_LEVEL_INFORMATION:	ZApplication::GetInstance().AddLogString(strText.c_str()); break;
	case MLOG_LEVEL_WARNING:		ZApplication::GetInstance().AddLogString(strText.c_str(), RGB(0, 0, 255)); break;
	case MLOG_LEVEL_ERROR:
		{
			ZApplication::GetInstance().AddLogString(strText.c_str(), RGB(255, 0, 0));
			ZApplication::GetInstance().AddErrorLogString(strText.c_str(), RGB(255, 0, 0));
		}
		break;
	default:
		ZApplication::GetInstance().AddLogString(strText.c_str());	
	};
}

void ZApplication::InitWindowHandles()
{
	m_hRichEdLog = GetDlgItem(ZApplication::m_hDlg, IDC_RICHEDIT21);
	m_hRichEdErrorLog = GetDlgItem(ZApplication::m_hDlg, IDC_RICHEDIT_ERRORLOG);

	m_pMonitorEd = new SAppMonitorEditBox(GetDlgItem(ZApplication::m_hDlg, IDC_RICHEDIT_MONITOR));
}

bool ZApplication::OnCreate()
{
	InitWindowHandles();

#ifndef _PUBLISH
	MGetDefaultLogger()->SetCustomOutputCallback(ZApplication::OutputLogProc);
#endif

	AddLogString(L"");

	if (ZBaseApplication::OnCreate() == false)
	{
		return false;
	}

	if (SetupTrayIcon() == false) return false;

	if (!ZConfig::m_bWindowVisible)
	{
		Show(false);
	}

	m_pAppCmdHandler = new ZCmdHandler_App(gsys.pCommandCenter->GetNetServer());

	return true;
}

bool ZApplication::OnUpdate(float fDelta)
{
	if (!ZBaseApplication::OnUpdate(fDelta))
		return false;

	UpdateServerInfo();

	m_pMonitorEd->Update(fDelta);

	return true;
}

void ZApplication::OnDestroy()
{
	SAFE_DELETE(m_pAppCmdHandler);

	ZBaseApplication::OnDestroy();

	if (m_bMakeTrayIcon)
		m_TrayIcon.RemoveIcon();

	SAFE_DELETE(m_pMonitorEd);
}



// 메인 다이알로그 메세지 핸들러
BOOL CALLBACK ZApplication::MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
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



void ZApplication::OnMsg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SServerApplication::OnMsg(hWnd, uMsg, wParam, lParam);

    switch (uMsg)
    {
	case WM_CREATE:
		{
			// 서버 시작 시간 표시
			wchar_t szTime[64] = L"";
			SYSTEMTIME stCurTime;
			GetLocalTime(&stCurTime);
			swprintf_s(szTime, L"MasterServer [%02d/%02d %02d:%02d:%02d]"
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
			m_hStatus = CreateStatusWindow(WS_CHILD | WS_VISIBLE, L"Status Line", hWnd, 0);
			
			RECT rt;
			GetWindowRect(m_hStatus, &rt);
			int arPart[2];
			arPart[0] = 750;
			arPart[1] = rt.right;

			SendMessage(m_hStatus, SB_SETPARTS, 2, (LPARAM)arPart);

			m_hCommandLogEdit = GetDlgItem(hDlg, IDC_RICHEDIT22);
			
			SetStatusText(0, L"");
			SetStatusText(1, L"Active");

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
#ifndef _PUBLISH
	case WM_DESTROY_SERVER:
		{
			PostQuitMessage(0);
		}
		break;
#endif
	case WM_ICON_NOTIFY:
		{
			OnTrayNotification(wParam, lParam);	
		}
	case WM_PAINT:
		{
			OnPaint();
		}
		break;
	case (WM_USER+23):
		{
			DestroyWindow( m_hWnd );
			PostQuitMessage(0);
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Message Sender
//////////////////////////////////////////////////////////////////////////
void ZApplication::SetStatusText(int nID, LPCTSTR str)
{
	::SendMessage(m_hStatus, SB_SETTEXT, nID, (LPARAM)str);
}


//////////////////////////////////////////////////////////////////////////
// Message Handler
//////////////////////////////////////////////////////////////////////////

LRESULT ZApplication::OnTrayNotification(WPARAM wParam, LPARAM lParam)
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

void ZApplication::OnServerStart()
{
}

void ZApplication::OnServerStop()
{
}


void ZApplication::LogCommand(const wchar_t* szCmd)
{
	wchar_t szMsg[512] = L"";
	wstring strDateTime = SAppHelper::MakeCurrentDateTimeString();

	swprintf_s(szMsg, L"%s %s\n", strDateTime.c_str(), szCmd);

	if (m_hCommandLogEdit)
	{
		SendMessage(m_hCommandLogEdit, EM_REPLACESEL, FALSE, (LPARAM)szMsg);
		SendMessage (m_hCommandLogEdit, EM_LINESCROLL, 0, 1) ;
	}
}

void ZApplication::SetServerInfoView(ZServerInfo& server_info) 
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

	wchar_t text[1024] = L"";
	wchar_t t1[1024] = L"";
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
	SetStatusText(0, (LPCTSTR)text);


	switch (server_info.nStat)
	{
	case SERVER_STAT_ACTIVE: wcscpy_s(t1, L"ACTIVE"); break;
	case SERVER_STAT_INCHECK: wcscpy_s(t1, L"INCHECK"); break;
	case SERVER_STAT_INACTIVE: wcscpy_s(t1, L"INACTIVE"); break;
	case SERVER_STAT_ERROR: wcscpy_s(t1, L"ERROR"); break;
	default: wcscpy_s(t1, L""); break;
	}

	int nLineIndex = 0;
	swprintf_s(text, L"Master Status = %s", t1);
	if (SERVER_STAT_ACTIVE != server_info.nStat)
		wcscat_s(text, L" (Not Ready)");
	SetMonitorText(nLineIndex++, text);


	uint32 globaltime = gsys.pSystem->GetNowTime();

	swprintf_s(text, L"GlobalTime = %u", globaltime);
	SetMonitorText(nLineIndex++, text);

	nLineIndex++;

	swprintf_s(text, L"Servers:");
	SetMonitorText(nLineIndex++, text);

	wstring strAcceptedServerOK = L"OK";
	if (server_info.nIsLoginServerConnected != server_info.nMaxRequiredLoginServerCount) strAcceptedServerOK = L"Accepting..";

	swprintf_s(text, L"Login Server\t= %d / %d (%s)", server_info.nIsLoginServerConnected, server_info.nMaxRequiredLoginServerCount, strAcceptedServerOK.c_str());
	SetMonitorText(nLineIndex++, text);

	strAcceptedServerOK = L"OK";
	if (server_info.nCurrGameServerCount != server_info.nMaxRequiredGameServerCount) strAcceptedServerOK = L"Accepting..";

	swprintf_s(text, L"Game Server\t= %d / %d (%s)", server_info.nCurrGameServerCount, server_info.nMaxRequiredGameServerCount, strAcceptedServerOK.c_str());
	SetMonitorText(nLineIndex++, text);

	strAcceptedServerOK = L"OK";
	if (server_info.nCurrAppServerCount != server_info.nMaxRequiredAppServerCount) strAcceptedServerOK = L"Accepting..";

	swprintf_s(text, L"App Server\t= %d / %d (%s)", server_info.nCurrAppServerCount, server_info.nMaxRequiredAppServerCount, strAcceptedServerOK.c_str());
	SetMonitorText(nLineIndex++, text);

	nLineIndex++;

	swprintf_s(text, L"SharedFieldInfo Count\t= %d", server_info.nSharedFieldInfoCount);
	SetMonitorText(nLineIndex++, text);

	swprintf_s(text, L"All SharedField Count\t= %d", server_info.nAllSharedFieldCount);
	SetMonitorText(nLineIndex++, text);

	nLineIndex++;

	swprintf_s(text, L"World Player Count\t= %d", server_info.nCurrPlayerCount);
	SetMonitorText(nLineIndex++, text);

}

void ZApplication::SetMonitorText(int nLine, const wchar_t* szMsg)
{
	if (!m_pMonitorEd) return;
	m_pMonitorEd->SetText(nLine, szMsg);
	return;
}


void ZApplication::Show(bool bVisible)
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




bool ZApplication::SetupTrayIcon()
{
	if (m_bMakeTrayIcon)
	{
		if(!m_TrayIcon.Create(m_hWnd, WM_ICON_NOTIFY, _T("MasterServer"), NULL, IDR_MAINFRAME))
		{
			mlog3("Tray Icon Creation Failed.\n");
			return false;
		}

		HICON icon = ::LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME));
		m_TrayIcon.SetIcon(icon);
	}
	return true;
}

void ZApplication::UpdateServerInfo()
{
	if (gsys.pServer)
	{
		SetServerInfoView(gsys.pServer->GetServerInfo());
	}
}
