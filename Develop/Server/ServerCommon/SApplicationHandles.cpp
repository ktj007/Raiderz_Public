#include "stdafx.h"
#include "SApplicationHandles.h"
#include <commctrl.h>

HWND SApplicationHandles::m_hDlg = 0;

BOOL SApplicationHandles::MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
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

SApplicationHandles::SApplicationHandles(HWND hWnd, wstring strAppName, WORD nAppDialogId, WORD nLogEditID, WORD nTrayMenuID)
: m_hStatus(NULL)
, m_hCommandLogEdit(NULL)
, m_strAppName(strAppName)
, m_nAppDialogId(nAppDialogId)
, m_nLogEditID(nLogEditID)
, m_nTrayMenuID(nTrayMenuID)
, m_hWnd(hWnd)
{
	// do nothing
}

SApplicationHandles::~SApplicationHandles()
{
	// do nothing
}

void SApplicationHandles::OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
	case WM_CREATE:
		{
			// 서버 시작 시간 표시
			wchar_t szTime[64] = L"";
			SYSTEMTIME stCurTime;
			GetLocalTime(&stCurTime);
			swprintf_s(szTime, L"%s [%02d/%02d %02d:%02d:%02d]"
				, GetAppName().c_str()
				, stCurTime.wMonth
				, stCurTime.wDay
				, stCurTime.wHour
				, stCurTime.wMinute
				, stCurTime.wSecond );
			SetWindowText(hWnd, szTime);

			// 메인 다이알로그 생성
			HWND hDlg = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(GetAppDialogId()), hWnd, MainDlgProc);
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

			m_hCommandLogEdit = GetDlgItem(hDlg, GetLogEditID());
			
			SetStatusText(0, L"");
			SetStatusText(1, L"Active");
		}
		break;		
	case WM_KEYDOWN:
		{
#ifdef _DEBUG
			if (wParam == VK_ESCAPE) exit(0);
#endif
		}
		break;
#ifndef _PUBLISH
	case WM_DESTROY_SERVER:
		{
			if (m_pServerApplication != NULL)
			{
				m_pServerApplication->CloseServer();
			}
			else
			{
				PostQuitMessage(0);
			}
		}
		break;
#endif
	case WM_ICON_NOTIFY:
		{
			OnTrayNotification(wParam, lParam);	
		}
	}
}

void SApplicationHandles::SetStatusText(int nID, const wchar_t* str)
{
	::SendMessage(m_hStatus, SB_SETTEXT, nID, (LPARAM)str);
}

LRESULT SApplicationHandles::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(lParam) == WM_RBUTTONUP)
	{
		HMENU menu = ::LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(GetTrayMenuID()));
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
