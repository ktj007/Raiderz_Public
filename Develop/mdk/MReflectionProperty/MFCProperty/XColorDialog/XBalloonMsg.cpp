// XBalloonMsg.cpp  Version 1.0 - see article at CodeProject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// Description:
//     XBalloonMsg implements CXBalloonMsg, a wrapper for balloon tooltips.
//
// History
//     Version 1.0 - 2008 April 5
//     - Initial public release
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

#include "stdafx.h"

// #define _WIN32_WINNT 0x0400		// for WM_MOUSEWHEEL

#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include "XBalloonMsg.h"

#pragma warning(disable : 4127)	// conditional expression is constant (_ASSERTE)
#pragma warning(disable : 4996)	// disable bogus deprecation warning

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#undef TRACE
#define TRACE __noop
#undef TRACERECT
#define TRACERECT __noop

//=============================================================================
// if you want to see the TRACE output, uncomment this line:
#include "XTrace.h"

//=============================================================================
// class variables
//=============================================================================

BOOL	CXBalloonMsg::m_bInit = FALSE;
BOOL	CXBalloonMsg::m_bUseBalloonTips = TRUE;
BOOL	CXBalloonMsg::m_bBalloon = TRUE;
HINSTANCE CXBalloonMsg::m_hInstance = 0;
UINT	CXBalloonMsg::m_nIcon = TTI_INFO;
UINT	CXBalloonMsg::m_nLastMessage = 0;
UINT	CXBalloonMsg::m_nTimeOutSeconds = 0;
UINT	CXBalloonMsg::m_nTimerId = 0;
LPRECT	CXBalloonMsg::m_pRect = 0;
HWND	CXBalloonMsg::m_hWnd = 0;
HWND	CXBalloonMsg::m_hParent = 0;
HWND	CXBalloonMsg::m_hCtrl = 0;
HWND	CXBalloonMsg::m_hWndBalloon = 0;
WNDPROC	CXBalloonMsg::m_wndProc = 0;
HHOOK	CXBalloonMsg::m_hKeyboardHook = 0;
HHOOK	CXBalloonMsg::m_hMouseHook = 0;
TCHAR	CXBalloonMsg::m_szTitle[] = { 0 };
TCHAR	CXBalloonMsg::m_szMsg[]   = { 0 };


//=============================================================================
CXBalloonMsg::CXBalloonMsg()
//=============================================================================
{
}

//=============================================================================
CXBalloonMsg::~CXBalloonMsg()
//=============================================================================
{
	if (m_wndProc && IsWindow(m_hParent))
		::SetWindowLong(m_hParent, GWL_WNDPROC, (LONG)m_wndProc);
	m_wndProc = 0;
}

//=============================================================================
//
// Show()
//
// Purpose:     The Show() function creates, displays, and operates a 
//              balloon message box. The message box contains an 
//              application-defined message and title, a close button, 
//              and an optional icon.
//
// Parameters:  lpszTitle       - [in] Pointer to a null-terminated string 
//                                that contains the balloon message title.  
//                                If this parameter is NULL, no title and  
//                                no icon will be displayed.  The special 
//                                symbol LPCTSTR_DEFAULT_TITLE may be used;  
//                                it will cause the executable module name 
//                                to be displayed.
//              lpszMsg         - [in] Pointer to a null-terminated string 
//                                that contains the balloon message to be 
//                                displayed.  Must not be null.
//              hCtrl           - [in] Handle to the control that the 
//                                message is being displayed for. Must not 
//                                be null.
//              hParent         - [in] Handle to the parent window of the 
//                                control. Must not be null.
//              hInstance       - [in] Handle to the instance of the module 
//                                that contains the string resource.  May 
//                                be null if string resource is not used.
//              nIcon           - [in] Specifies the icon to associate with 
//                                the balloon message.
//              bUseBalloonTips - [in] Specifies whether balloon tips are 
//                                to be used to display the message. If 
//                                this parameter is TRUE, balloon tips will 
//                                be used unless disabled in the registry.  
//                                If this parameter is FALSE, regular 
//                                tooltips will be used, regardless of value 
//                                of registry key. If not present, this 
//                                parameter defaults to TRUE.
//              nTimeOutSeconds - [in] Specifies number of seconds before the
//                                balloon message is automatically closed. If
//                                this parameter is zero, the balloon message
//                                will not be automatically closed.  If not
//                                present, this parameter defaults to zero.
//              pRect           - [in] Pointer to a RECT struct that contains 
//                                position where balloon message is to be 
//                                displayed.  May be null if default position 
//                                should be used. If not present, this 
//                                parameter defaults to null.
//
// Returns:     None
//
void CXBalloonMsg::Show(LPCTSTR lpszTitle, 
						LPCTSTR lpszMsg, 
						HWND hCtrl, 
						HWND hParent,
						HINSTANCE hInstance,
						UINT nIcon /*= TTI_INFO*/,
						BOOL bUseBalloonTips /*= TRUE*/,
						UINT nTimeOutSeconds /*= 0*/,
						LPRECT pRect /*= NULL*/)
{
	_ASSERTE(lpszMsg);
	_ASSERTE(hCtrl);
	_ASSERTE(hParent);

	if (!lpszMsg || !hCtrl || !hParent)
	{
		TRACE(_T("ERROR - bad parameter\n"));
		return;
	}

	if (!IsWindow(hCtrl) || !IsWindow(hParent))
	{
		TRACE(_T("ERROR - not a valid window handle\n"));
		return;
	}

	Destroy();

	m_hParent = hParent;
	m_hCtrl = hCtrl;
	m_hInstance = hInstance;
	m_nIcon = nIcon;
	m_pRect = pRect;
	m_bUseBalloonTips = bUseBalloonTips;
	m_nTimeOutSeconds = nTimeOutSeconds;
	m_nTimerId = 0;

	GetStrings(lpszTitle, lpszMsg, hInstance);

	// make sure there's a message
	_ASSERTE(m_szMsg[0] != _T('\0'));
	if (m_szMsg[0] == _T('\0'))
		return;

	m_bInit = FALSE;
	m_nLastMessage = 0;

	TCHAR * pszClassName = _T("XBalloonMsg");

	WNDCLASS wc =
	{
		CS_DBLCLKS,						// class style - want WM_LBUTTONDBLCLK
		DefWindowProcX,					// window proc
		0,								// class extra bytes
		0,								// window extra bytes
		hInstance,						// instance handle
		0,								// icon
		::LoadCursor(0, IDC_ARROW),		// cursor
		0,								// background brush
		0,								// menu name
		pszClassName					// class name
	};

	if (!::RegisterClass(&wc))
	{
		DWORD dwLastError = GetLastError();
		if (dwLastError != ERROR_CLASS_ALREADY_EXISTS)
		{
			TRACE(_T("ERROR - RegisterClass failed, GetLastError() returned %u\n"), dwLastError);
			_ASSERTE(FALSE);
			return;
		}
	}

	m_hWnd = ::CreateWindowEx(0, pszClassName, _T(""), WS_POPUP | WS_VISIBLE, 
		30000, 30000, 10, 10,
		m_hParent, (HMENU)0, hInstance, 0);

	if (!m_hWnd)
	{
		TRACE(_T("ERROR - CreateWindowEx failed\n"));
		_ASSERTE(m_hWnd);
	}
}

//=============================================================================
//
// Destroy()
//
// Purpose:     The Destroy() function removes the balloon tip from the
//              screen and destroys the tooltip window.
//
// Parameters:  None
//
// Returns:     None
//
void CXBalloonMsg::Destroy()
{
	TRACE(_T("in CXBalloonMsg::Destroy\n"));

	if (m_nTimerId)
		::KillTimer(0, m_nTimerId);
	m_nTimerId = 0;

	if (m_wndProc && IsWindow(m_hParent))
		::SetWindowLong(m_hParent, GWL_WNDPROC, (LONG)m_wndProc);
	m_wndProc = 0;

	if (IsWindow(m_hWndBalloon))
	{
		::SendMessage(m_hWndBalloon, TTM_POP, 0, 0);
		::PostMessage(m_hWndBalloon, WM_CLOSE, 0, 0);
	}
	m_hWndBalloon = 0;

	if (IsWindow(m_hWnd))
		::DestroyWindow(m_hWnd);
}

//=============================================================================
//
// KeyboardProc()
//
// Purpose:     Callback function set by the SetWindowsHookEx() function.
//              The system calls this function whenever an application 
//              calls the GetMessage or PeekMessage function and there is a
//              keyboard message (WM_KEYUP or WM_KEYDOWN) to be processed. 
//
// Parameters:  nCode  - hook code
//              wParam - virtual-key code
//              lParam - keystroke-message information
//
// Returns:     LRESULT - 1 to prevent the system from passing the message on;
//                        or the return value from CallNextHookEx().
//
LRESULT CALLBACK CXBalloonMsg::KeyboardProc(int nCode, 
											WPARAM wParam, 
											LPARAM lParam)
{
	if ((nCode >= 0) && 
		((lParam & 0x80000000) == 0) &&		// ignore key releases
		(wParam != VK_CONTROL) &&
		(wParam != VK_SHIFT) &&
		(wParam != VK_MENU))
	{
		LRESULT rc = 1;

		if (wParam == VK_ESCAPE ||
			wParam == VK_RETURN ||
			wParam == VK_TAB ||
			wParam == VK_LEFT ||
			wParam == VK_UP ||
			wParam == VK_RIGHT ||
			wParam == VK_DOWN ||
			((wParam >= VK_F1) && (wParam <= VK_F24))
			)
		{
			// don't forward these keystrokes
		}
		else
		{
			rc = CallNextHookEx(m_hKeyboardHook, nCode, wParam, lParam);
		}
		Destroy();
		return rc;
	}

	return CallNextHookEx(m_hKeyboardHook, nCode, wParam, lParam);
}

//=============================================================================
//
// MouseProc()
//
// Purpose:     Callback function set by the SetWindowsHookEx() function.
//              The system calls this function whenever an application 
//              calls the GetMessage or PeekMessage function and there is a
//              mouse message to be processed. 
//
// Parameters:  nCode  - hook code
//              wParam - message identifier
//              lParam - mouse coordinates
//
// Returns:     LRESULT - the return value from CallNextHookEx().
//
LRESULT CALLBACK CXBalloonMsg::MouseProc(int nCode, 
										 WPARAM wParam, 
										 LPARAM lParam)
{
	LRESULT rc =  CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);

	if (nCode >= 0)
	{
		if (wParam == WM_LBUTTONDOWN ||
			wParam == WM_RBUTTONDOWN ||
			wParam == WM_MBUTTONDOWN ||
			wParam == WM_NCLBUTTONDOWN ||
			wParam == WM_NCRBUTTONDOWN ||
			wParam == WM_NCMBUTTONDOWN ||
			wParam == WM_MOUSEWHEEL)
		{
			Destroy();
		}
	}
	return rc;
}

//=============================================================================
//
// TimerProcX()
//
// Purpose:     Callback function that processes WM_TIMER messages.  It is set
//              by SetTimer() if caller has specified a message timeout.
//
// Parameters:  hwnd     - handle to window
//              message  - WM_TIMER message
//              nIdEvent - timer identifier
//              dwTime   - current system time
//
// Returns:     None
//
void CALLBACK CXBalloonMsg::TimerProcX(HWND /*hwnd*/, 
									   UINT /*message*/,
									   UINT_PTR /*nIdEvent*/,
									   DWORD /*dwTime*/)
{
	Destroy();
}

//=============================================================================
//
// DefWindowProcX()
//
// Purpose:     Window proc to dispatch messages for CXBalloonMsg.  Also 
//              used to subclass parent window to trap UI messages.
//
// Parameters:  Standard windows message parameters.
//
// Returns:     LRESULT - The return value is the result of the message 
//                        processing and depends on the message.
//
LRESULT __stdcall CXBalloonMsg::DefWindowProcX(HWND hWnd,
											   UINT message,
											   WPARAM wParam,
											   LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
			if (m_hKeyboardHook)
				::UnhookWindowsHookEx(m_hKeyboardHook);
			m_hKeyboardHook = 0;

			if (m_hMouseHook)
				::UnhookWindowsHookEx(m_hMouseHook);
			m_hMouseHook = 0;

			if (IsWindow(m_hParent))
			{
				::UpdateWindow(m_hParent);

				// check if another app now is active
				if (m_nLastMessage == WM_ACTIVATE)
				{
					// this restores "active caption" to parent
					::SetWindowPos(m_hParent, 0, 0, 0, 0, 0, 
						SWP_DRAWFRAME|SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE);
				}
			}
			break;

		case WM_WINDOWPOSCHANGED:
		{
			if (!m_bInit && (m_hWndBalloon == 0))
			{
				DWORD dwStyle = WS_POPUP | WS_VISIBLE | TTS_ALWAYSTIP | TTS_NOPREFIX | 
								TTS_CLOSE | TTS_NOFADE | TTS_NOANIMATE;
				m_bBalloon = GetEnableBalloonTips();
				if (m_bBalloon && m_bUseBalloonTips)
					dwStyle |= TTS_BALLOON;

				m_hWndBalloon = ::CreateWindowEx(0, TOOLTIPS_CLASS, 0, dwStyle,
					CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
					m_hParent, 0, 0, 0);

				_ASSERTE(m_hWndBalloon);

				if (m_hWndBalloon)
				{
					if (m_wndProc == 0 && IsWindow(m_hParent))
					{
						m_wndProc = (WNDPROC)::SetWindowLong(m_hParent, GWL_WNDPROC, (LONG)DefWindowProcX);
						if (m_wndProc)
						{
							TRACE(_T("parent successfully subclassed\n"));
						}
						else
						{
							TRACE(_T("ERROR - parent subclass failed\n"));
						}
					}

					TOOLINFO ti;
					::ZeroMemory(&ti, sizeof(ti));
					ti.cbSize   = sizeof(ti);
					// using TTF_TRACK causes balloon to be displayed underneath control
					ti.uFlags   = TTF_TRACK;
					if (m_bBalloon && m_bUseBalloonTips)
						ti.uFlags |= TTF_CENTERTIP;
					ti.hwnd     = m_hParent;
					ti.lpszText = (LPTSTR)m_szMsg;
					ti.uId      = 10001;
					ti.hinst    = m_hInstance;

					if (IsWindow(m_hWndBalloon))
					{
						::SendMessage(m_hWndBalloon, TTM_SETMAXTIPWIDTH, 0, 32000);
						::SendMessage(m_hWndBalloon, TTM_SETDELAYTIME, TTDT_AUTOPOP, MAKELONG(1,0));
						::SendMessage(m_hWndBalloon, TTM_SETDELAYTIME, TTDT_INITIAL, MAKELONG(1,0));
						::SendMessage(m_hWndBalloon, TTM_SETDELAYTIME, TTDT_RESHOW,  MAKELONG(1,0));
						::SendMessage(m_hWndBalloon, TTM_ADDTOOL, 0, (LPARAM) &ti);
						::SendMessage(m_hWndBalloon, TTM_SETTITLE, (WPARAM)m_nIcon, (LPARAM)m_szTitle);

						RECT rectTip;
						PositionBalloon(m_pRect, &rectTip, m_bBalloon & m_bUseBalloonTips);

						::SendMessage(m_hWndBalloon, TTM_TRACKPOSITION, (WPARAM)0, 
							(LPARAM)MAKELONG(rectTip.left, rectTip.top));

						::SendMessage(m_hWndBalloon, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&ti);

						if (!m_hKeyboardHook)
						{
							m_hKeyboardHook = ::SetWindowsHookEx(WH_KEYBOARD,
													KeyboardProc,
													NULL, ::GetCurrentThreadId());
							_ASSERTE(m_hKeyboardHook);
						}

						if (!m_hMouseHook)
						{
							m_hMouseHook = ::SetWindowsHookEx(WH_MOUSE,
													MouseProc,
													NULL, ::GetCurrentThreadId());
							_ASSERTE(m_hMouseHook);
						}

						if (m_nTimeOutSeconds)
						{
							m_nTimerId = ::SetTimer(0, 1, m_nTimeOutSeconds*1000, TimerProcX);
						}

						m_bInit = TRUE;
					}
				}
			}
			return 0;
			break;
		}

		case WM_ACTIVATE:
			// lParam will be zero if another app is now active
			m_nLastMessage = (lParam != 0) ? WM_ACTIVATE : 0;

		case WM_KILLFOCUS:
// 		case WM_MENUCOMMAND:
		case WM_SYSCOMMAND:
		case WM_MOUSEACTIVATE:
		case WM_ENTERMENULOOP:
		case WM_HSCROLL:
		case WM_VSCROLL:
			TRACE(_T("potential terminating message 0x%X\n"), message);

			if (m_bInit && IsWindow(m_hWnd) && IsWindow(m_hWndBalloon))
			{
				// remove tip from screen
				Destroy();
			}
			return 0;
			break;
	};

	if (hWnd == m_hParent)
	{
		if (m_wndProc && IsWindow(m_hParent))
			return (*m_wndProc)(hWnd, message, wParam, lParam);
		else
			return 0;
	}
	else
	{
		if (IsWindow(hWnd))
			return ::DefWindowProc(hWnd, message, wParam, lParam);
		else
			return 0;
	}
}

//=============================================================================
//
// GetEnableBalloonTips()
//
// Purpose:     The GetEnableBalloonTips() function retrieves the value of
//              the registry key EnableBalloonTips.
//
// Parameters:  None
//
// Returns:     BOOL - TRUE = key value is non-zero, or key does not exist;
//                     FALSE = key value is zero.
//
BOOL CXBalloonMsg::GetEnableBalloonTips()
{
	BOOL rc = TRUE;		// assume balloon tips are enabled
	HKEY hkey = 0;
	if (RegOpenKeyEx(HKEY_CURRENT_USER,
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),
			0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS)
	{
		DWORD dwEnable = 0;
		DWORD dwSize = sizeof(DWORD);
		if (RegQueryValueEx(hkey, _T("EnableBalloonTips"), 0, 0,
			(LPBYTE) &dwEnable, &dwSize) == ERROR_SUCCESS)
		{
			rc = dwEnable != 0;
		}
		RegCloseKey(hkey);
	}

	return rc;
}

//=============================================================================
//
// GetStrings()
//
// Purpose:     The GetStrings() function sets the class variables m_szMsg 
//              and m_szTitle.
//
// Parameters:  lpszTitle - [in] pointer to title string or resource id;  may
//                          be NULL or special symbol LPCTSTR_DEFAULT_TITLE.
//              lpszMsg   - [in] pointer to message string or resource id; 
//                          must not be NULL.
//              hInstance - [in] app instance handle
//
// Returns:     None
//
void CXBalloonMsg::GetStrings(LPCTSTR lpszTitle, LPCTSTR lpszMsg, HINSTANCE hInstance)
{
	// get message and title - they may be pointers to strings or resource IDs

	memset(m_szMsg, 0, sizeof(m_szMsg));
	memset(m_szTitle, 0, sizeof(m_szTitle));

	// is the message a string or a resource id?
	if (HIWORD(lpszMsg) == 0)
	{
		// id
		UINT nID = LOWORD((UINT)(UINT_PTR)lpszMsg);
		if (!::LoadString(hInstance, nID, m_szMsg, sizeof(m_szMsg)/sizeof(TCHAR)-2))
		{
			TRACE(_T("ERROR - failed to load message string %d\n"), nID);
			_ASSERTE(FALSE);
		}
	}
	else
	{
		// string
		_tcsncpy(m_szMsg, lpszMsg, sizeof(m_szMsg)/sizeof(TCHAR)-2);
	}

#ifdef _DEBUG
	size_t nLen = _tcslen(m_szMsg);
	size_t nBufLen = sizeof(m_szMsg)/sizeof(TCHAR);
	if (nLen > (nBufLen - nBufLen/10))
	{
		TRACE(_T("WARNING - message size near maximum of %d\n"), nBufLen);
	}
#endif

	if (lpszTitle == LPCTSTR_DEFAULT_TITLE)
	{
		// set title to module name
		_tcsncpy(m_szTitle, GetModuleName(hInstance), 
			sizeof(m_szTitle)/sizeof(TCHAR)-2);
	}
	else if (lpszTitle)		// title is optional
	{
		// is the title a string or an id?
		if (HIWORD(lpszTitle) == 0)
		{
			// id
			UINT nID = LOWORD((UINT)(UINT_PTR)lpszTitle);
			if (!::LoadString(hInstance, nID, m_szTitle, sizeof(m_szTitle)/sizeof(TCHAR)-2))
			{
				TRACE(_T("ERROR - failed to load title string %d\n"), nID);
				_ASSERTE(FALSE);
			}
		}
		else
		{
			// string
			_tcsncpy(m_szTitle, lpszTitle, sizeof(m_szTitle)/sizeof(TCHAR)-2);
		}
	}

#ifdef _DEBUG
	nLen = _tcslen(m_szTitle);
	nBufLen = sizeof(m_szTitle)/sizeof(TCHAR);
	if (nLen > (nBufLen - nBufLen/10))
	{
		TRACE(_T("WARNING - title size near maximum of %d\n"), nBufLen);
	}
#endif
}

//=============================================================================
//
// GetModuleName()
//
// Purpose:     The GetModuleName() function returns the executable module
//              name.
//
// Parameters:  hInstance - [in] app instance handle
//
// Returns:     TCHAR *   - pointer to name (static)
//
TCHAR * CXBalloonMsg::GetModuleName(HINSTANCE hInstance)
{
	static TCHAR szName[MAX_PATH*2];
	szName[0] = _T('\0');
	::GetModuleFileName(hInstance, szName, sizeof(szName)/sizeof(TCHAR)-2);
	szName[sizeof(szName)/sizeof(TCHAR)-1] = _T('\0');
	TCHAR *cp = _tcsrchr(szName, _T('\\'));
	if (cp)
	{
		_tcscpy(&szName[0], cp+1);
	}
	cp = _tcsrchr(szName, _T('.'));
	if (cp)
	{
		*cp = _T('\0');
	}
	return szName;
}

//=============================================================================
//
// PositionBalloon()
//
// Purpose:     The PositionBalloon() function returns the executable module
//              name.
//
// Parameters:  pRect    - [in] optional pointer to caller-specified RECT;
//                         may be NULL.
//              pNewRect - [out] pointer to RECT that receives balloon 
//                         location (left and top values only are vaild).
//              bBalloon - [in] TRUE = balloon will be displayed;  
//                         FALSE = regular tooltip will be displayed.
//
// Returns:     None
//
void CXBalloonMsg::PositionBalloon(LPRECT pRect, LPRECT pNewRect, BOOL bBalloon)
{
	RECT rect = { 0 };

	if (pRect)
	{
		// use specified position
		rect = *pRect;
	}
	else
	{
		// use control position
		RECT rectCtrl = { 0 };
		if (IsWindow(m_hCtrl))
			::GetWindowRect(m_hCtrl, &rectCtrl);

		rect = rectCtrl;
		rect.top  = rectCtrl.bottom;

		if (bBalloon)
		{
			rect.left = rectCtrl.left + (rectCtrl.right-rectCtrl.left)/2;
			rect.top  = rectCtrl.top +  (rectCtrl.bottom-rectCtrl.top)/2;
		}
	}

	_ASSERTE(pNewRect);
	if (pNewRect)
		*pNewRect = rect;
}
