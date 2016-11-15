#pragma once

#include "WinDef.h"
#include "SServerApplication.h"
#include "MTstring.h"

#define WM_ICON_NOTIFY			(WM_USER+10)
#define WM_DESTROY_SERVER		(WM_USER+25)

class SApplicationHandles
{
public:
	static BOOL CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
public:
	SApplicationHandles(HWND hWnd, wstring strAppName, WORD nAppDialogId, WORD nLogEditID, WORD nTrayMenuID);
	virtual ~SApplicationHandles();

	virtual void OnMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	void SetStatusText(int nID, const wchar_t* str);
	LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);

	static HWND GetDlgHandle() { return m_hDlg; }
	HWND GetCommandLogEditHandle() { return m_hCommandLogEdit; }
protected:
	SServerApplication*		m_pServerApplication;

	static HWND m_hDlg;
	
	HWND m_hStatus;	
	HWND m_hCommandLogEdit;

	HWND	m_hWnd;
	wstring m_strAppName;
	WORD	m_nAppDialogId;
	WORD	m_nLogEditID;
	WORD	m_nTrayMenuID;

	wstring GetAppName(void)		{ return m_strAppName; }
	WORD GetAppDialogId(void)	{ return m_nAppDialogId; }
	WORD GetLogEditID(void)		{ return m_nLogEditID; }
	WORD GetTrayMenuID(void)	{ return m_nTrayMenuID; }
};
