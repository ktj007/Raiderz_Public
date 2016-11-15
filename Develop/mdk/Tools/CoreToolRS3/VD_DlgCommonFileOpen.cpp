// from MSDN Magazine -- November 2004, by Paul DiLascia
#pragma once
#include "stdafx.h"
#include "VD_DlgCommonFileOpen.h"
#include <dlgs.h>				 // for common dialog control IDs

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Self-initialization message posted.
const int MYWM_POSTINIT = WM_USER+1;
LPCTSTR PROFILE_SETTINGS = _T("settings");
LPCTSTR PROFILE_VIEWMODE = _T("ViewMode");

IMPLEMENT_DYNAMIC(CPersistOpenDlg, CFileDialog)
BEGIN_MESSAGE_MAP(CPersistOpenDlg, CFileDialog)
	ON_MESSAGE(MYWM_POSTINIT, OnPostInit)
END_MESSAGE_MAP()

CPersistOpenDlg::CPersistOpenDlg() : CFileDialog(TRUE)
{
	m_ofn.lpstrTitle = _T("Select a file or folder");
}

////////////////
// dtor saves current open dialog list view mode in profile settings.
//
CPersistOpenDlg::~CPersistOpenDlg()
{
}

//////////////////
// Handle WM_INITDIALOG: post myself a special message to set list view mode.
//
BOOL CPersistOpenDlg::OnInitDialog()
{
	CFileDialog::OnInitDialog();
	PostMessage(MYWM_POSTINIT,0,0);
	return TRUE;
}

//////////////////
// Handle MYWN_POSTINIT: not until now is the list view created, so I can set
// it to the remembered value from profile settings.
//
LRESULT CPersistOpenDlg::OnPostInit(WPARAM wp, LPARAM lp)
{
	// Subclass folder view shell window. Note real dialog is my parent, not me!
	VERIFY(m_wndListViewShell.SubclassDlgItem(lst2, GetParent()));

	// Set view mode based on value saved in profile settings.
	/*m_wndListViewShell.SetViewMode(AfxGetApp()->GetProfileInt(PROFILE_SETTINGS,
		PROFILE_VIEWMODE, 0));*/
	//by pok, 오로지 미리보기로만 열기
	m_wndListViewShell.SetViewMode(ODM_VIEW_THUMBS);

	return 0;
}

////////////////////////////////////////////////////////////////
// CListViewShellWnd - Used to subclass the "SHELLDLL_DefView" window in the
// file open dialog. This window contain the list view and processses
// commands to set different view modes.
//

IMPLEMENT_DYNAMIC(CListViewShellWnd, CWnd)
BEGIN_MESSAGE_MAP(CListViewShellWnd, CWnd)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

//////////////////
// Set the list view to desired mode.
//
BOOL CListViewShellWnd::SetViewMode(int cmd)
{
	if (m_hWnd && cmd) {

		// SHELLDLL_DefView window found: send it the command.
		SendMessage(WM_COMMAND, cmd);

		// Sent parent dialog a Refresh command (F5) to force repaint.
		// The command ID value was obtained by inspection using Spy++.
		//
		const int CMD_REFRESH = 40966; // from Spy++
		GetParent()->SendMessage(WM_COMMAND, CMD_REFRESH);

		return TRUE;
	}
	return FALSE;
}

//////////////////
// This override is necessary to bypass an ASSERT in CWnd::OnCommand that
// bombs with the list view shell window. Note that this bypass eliminates
// some MFC functionality such as message reflection.
//
BOOL CListViewShellWnd::OnCommand(WPARAM wp, LPARAM lp)
{
	UINT nID = LOWORD(wp);
	int nCode = HIWORD(wp);
	return OnCmdMsg(nID, nCode, NULL, NULL);
}


//////////////
// 새로운 다이어로그 형식을 정의 할때
//

// 일반적인 리스트 뷰
IMPLEMENT_DYNAMIC(CListViewOpenDlg, CPersistOpenDlg)
BEGIN_MESSAGE_MAP(CListViewOpenDlg, CPersistOpenDlg)
	ON_MESSAGE(MYWM_POSTINIT, OnPostInit)
END_MESSAGE_MAP()

LRESULT CListViewOpenDlg::OnPostInit(WPARAM wp, LPARAM lp)
{
	VERIFY(m_wndListViewShell.SubclassDlgItem(lst2, GetParent()));
	m_wndListViewShell.SetViewMode(ODM_VIEW_LIST);

	return 0;
}

// xml 파일
IMPLEMENT_DYNAMIC(CXMLOpenDlg, CPersistOpenDlg)
BEGIN_MESSAGE_MAP(CXMLOpenDlg, CPersistOpenDlg)
	ON_MESSAGE(MYWM_POSTINIT, OnPostInit)
END_MESSAGE_MAP()

LRESULT CXMLOpenDlg::OnPostInit(WPARAM wp, LPARAM lp)
{
	VERIFY(m_wndListViewShell.SubclassDlgItem(lst2, GetParent()));
	m_wndListViewShell.SetViewMode(ODM_VIEW_LIST);

	return 0;
}

// bmp 파일
IMPLEMENT_DYNAMIC(CBMPOpenDlg, CPersistOpenDlg)
BEGIN_MESSAGE_MAP(CBMPOpenDlg, CPersistOpenDlg)
	ON_MESSAGE(MYWM_POSTINIT, OnPostInit)
END_MESSAGE_MAP()

LRESULT CBMPOpenDlg::OnPostInit(WPARAM wp, LPARAM lp)
{
	VERIFY(m_wndListViewShell.SubclassDlgItem(lst2, GetParent()));
	m_wndListViewShell.SetViewMode(ODM_VIEW_THUMBS);

	return 0;
}
