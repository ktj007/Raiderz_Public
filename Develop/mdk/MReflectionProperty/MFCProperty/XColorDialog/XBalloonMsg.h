// XBalloonMsg.h  Version 1.0 - see article at CodeProject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// License:
//     This software is released under the Code Project Open License (CPOL),
//     which may be found here:  http://www.codeproject.com/info/eula.aspx
//     You are free to use this software in any way you like, except that you 
//     may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XBALLOONMSG_H
#define XBALLOONMSG_H

#include <windows.h>
#include "commctrl.h"

//=============================================================================
// special symbol for title string - replaced with module name
#define LPCTSTR_DEFAULT_TITLE	((LPCTSTR)-1L)

//=============================================================================
class CXBalloonMsg
//=============================================================================
{
// Construction
public:
	CXBalloonMsg();
	virtual ~CXBalloonMsg();

// Attributes
public:
	static HWND m_hWndBalloon;

// Operations
public:
	static void Show(LPCTSTR lpszTitle, 
					 LPCTSTR lpszMsg, 
					 HWND hCtrl, 
					 HWND hParent,
					 HINSTANCE hInstance,
					 UINT nIcon = TTI_INFO,
					 BOOL bUseBalloonTips = TRUE,
					 UINT nTimeOutSeconds = 0,
					 LPRECT pRect = NULL);
	static void Destroy();

// Implementation
protected:
	static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT __stdcall DefWindowProcX(HWND hWnd,
								UINT message,
								WPARAM wParam,
								LPARAM lParam);
	static void CALLBACK TimerProcX(HWND hwnd,
								UINT message,
								UINT_PTR nIdEvent,
								DWORD dwTime);
	static BOOL GetEnableBalloonTips();
	static void GetStrings(LPCTSTR lpszTitle, LPCTSTR lpszMsg, HINSTANCE hInstance);
	static TCHAR * GetModuleName(HINSTANCE hInstance);
	static void PositionBalloon(LPRECT pRect, LPRECT pNewRect, BOOL bBalloon);

	static BOOL			m_bInit;			// TRUE = initialization finished
	static BOOL			m_bUseBalloonTips;	// TRUE = use balloon tips if possible
	static BOOL			m_bBalloon;			// TRUE = balloon tips not disallowed in 
											// registry
	static HINSTANCE	m_hInstance;		// app instance
	static UINT			m_nIcon;			// icon to use
	static UINT			m_nLastMessage;		// last message received
	static UINT			m_nTimeOutSeconds;	// message timeout in seconds;
											// 0 = no timeout
	static UINT			m_nTimerId;			// timer id returned by SetTimer()
	static LPRECT		m_pRect;			// NULL = use control rect
	static HWND			m_hWnd;				// hwnd of CXBalloonMsg
	static HWND			m_hParent;			// parent hwnd
	static HWND			m_hCtrl;			// control hwnd
	static WNDPROC		m_wndProc;			// window proc for parent subclassing
	static HHOOK		m_hKeyboardHook;	// keyboard hook
	static HHOOK		m_hMouseHook;		// mouse hook
	static TCHAR		m_szTitle[500];		// title string
	static TCHAR		m_szMsg[4000];		// message string
};

#endif // XBALLOONMSG_H
