// CXToolTipCtrl.h  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// History
//     Version 1.0 - 2008 March 12
//     - Initial public release
//
// License:
//     This software is released into the public domain.  You are free to use
//     it in any way you like, except that you may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
// Public Members:
//           NAME                           DESCRIPTION
//   -------------------  ----------------------------------------------------
//   Construction
//      CXToolTipCtrl()   Ctor
//   Attributes
//      GetToolCount()    Retrieves a count of the tools
//      SetDelayTime()    Sets the initial, pop-up, and reshow durations
//      SetMaxTipWidth()  Sets the maximum width for a tool tip window
//      SetToolRect()     Sets a new bounding rectangle for a tool
//   Operations
//      Create()          Creates a tool tip control
//      AddTool()         Registers a tool with the tool tip control
//      DelTool()         Removes a tool from the tool tip control
//      Activate()        Activates and deactivates the tool tip control
//      Pop()             Removes a displayed ToolTip window from view
//      Update()          Forces the current tool to be redrawn
//      UpdateTipText()   Sets the tool tip text for a tool
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CXTOOLTIPCTRL_H
#define CXTOOLTIPCTRL_H

#include "commctrl.h."
#include <crtdbg.h>

#pragma warning(disable : 4127)	// for _ASSERTE: conditional expression is constant

class CXToolTipCtrl
{
// Constructors
public:
	CXToolTipCtrl() : m_hWnd(0), m_hParentWnd(0) {}
	virtual ~CXToolTipCtrl()
	{
		if (IsWindow(m_hWnd))
			::DestroyWindow(m_hWnd);
		m_hWnd = 0;
	}

// Attributes
public:
	int GetToolCount() const
	{
		int rc = 0;
		if (IsWindow(m_hWnd))
		{
			rc = ::SendMessage(m_hWnd, TTM_GETTOOLCOUNT, 0, 0);
		}
		return rc;
	}

	void SetDelayTime(DWORD dwDuratiion, int nTime)
	{
		if (IsWindow(m_hWnd))
		{
			::SendMessage(m_hWnd, TTM_SETDELAYTIME, (WPARAM)dwDuratiion, 
				(LPARAM)MAKELONG(nTime,0));
		}
	}

	int SetMaxTipWidth(int nWidth)
	{
		int rc = 0;
		if (IsWindow(m_hWnd))
		{
			rc =  ::SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, (LPARAM)nWidth);
		}
		return rc;
	}

	void SetToolRect(HWND hWnd, UINT nIDTool, LPCRECT lpRect)
	{
		if (lpRect && IsWindow(m_hWnd))
		{
			TOOLINFO ti;
			FillInToolInfo(ti, hWnd, nIDTool);
			ti.rect = *lpRect;
			::SendMessage(m_hWnd, TTM_NEWTOOLRECT, 0, (LPARAM) &ti);
		}
	}

// Operations
public:
	BOOL Create(HWND hParentWnd, DWORD dwStyle = 0)
	{
		_ASSERTE(m_hWnd == 0);

		m_hParentWnd = hParentWnd;

		INITCOMMONCONTROLSEX icex;

		// Load the ToolTip class from the DLL.
		icex.dwSize = sizeof(icex);
		icex.dwICC  = ICC_BAR_CLASSES;

		if (!InitCommonControlsEx(&icex))
		{
			_ASSERTE(FALSE);
			return FALSE;
		}

		m_hWnd = ::CreateWindowEx(0,
								  TOOLTIPS_CLASS,		
								  0,		
								  dwStyle | WS_POPUP,
								  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,		
								  hParentWnd,		
								  0,		
								  0,
								  0);

		_ASSERTE(m_hWnd);

		if (m_hWnd)
		{
			::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, 
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

			::SendMessage(m_hWnd, TTM_ACTIVATE, 1, 0);
		}

		return m_hWnd != 0;
	}

	BOOL AddTool(HWND hWnd, 
				 LPCTSTR lpszText = LPSTR_TEXTCALLBACK, 
				 RECT *pRectTool = NULL, 
				 UINT nIDTool = 0)
	{
		BOOL rc = FALSE;

		_ASSERTE(m_hWnd);

		if (IsWindow(m_hWnd))
		{
			TOOLINFO ti;
			FillInToolInfo(ti, hWnd, nIDTool);

			ti.uFlags   = TTF_SUBCLASS;
			ti.lpszText = (LPTSTR)lpszText;
			if (pRectTool)
				ti.rect = *pRectTool;

			rc = (BOOL) ::SendMessage(m_hWnd, TTM_ADDTOOL, 0, (LPARAM) &ti);
		}

		return rc;
	}

	void DelTool(HWND hWnd, UINT nIDTool = 0)
	{
		if (IsWindow(m_hWnd))
		{
			TOOLINFO ti;
			FillInToolInfo(ti, hWnd, nIDTool);
			::SendMessage(m_hWnd, TTM_DELTOOL, 0, (LPARAM) &ti);
		}
	}

	void Activate(BOOL bActivate)
	{
		if (IsWindow(m_hWnd))
		{
			::SendMessage(m_hWnd, TTM_ACTIVATE, (WPARAM) bActivate, 0);
		}
	}

	void Update()
	{
		if (IsWindow(m_hWnd))
		{
			::SendMessage(m_hWnd, TTM_UPDATE, 0, 0);
		}
	}

	void Pop()
	{
		if (IsWindow(m_hWnd))
		{
			::SendMessage(m_hWnd, TTM_POP, 0, 0);
		}
	}

	void UpdateTipText(LPCTSTR lpszText, HWND hWnd, UINT nIDTool = 0)
	{
		if (lpszText && IsWindow(m_hWnd))
		{
			TOOLINFO ti;
			FillInToolInfo(ti, hWnd, nIDTool);
			ti.lpszText = (LPTSTR)lpszText;
			::SendMessage(m_hWnd, TTM_UPDATETIPTEXT, 0, (LPARAM) &ti);
		}
	}

// Implementation
public:
	HWND m_hWnd;
	HWND m_hParentWnd;

protected:
	void FillInToolInfo(TOOLINFO& ti, HWND hWnd, UINT nIDTool) const
	{
		memset(&ti, 0, sizeof(TOOLINFO));
		ti.cbSize = sizeof(TOOLINFO);
		if (nIDTool == 0)
		{
			ti.hwnd = ::GetParent(hWnd);
			ti.uFlags = TTF_IDISHWND;
			ti.uId = (UINT)hWnd;
		}
		else
		{
			ti.hwnd = hWnd;
			ti.uFlags = 0;
			ti.uId = nIDTool;
		}
	}

};

#endif //CXTOOLTIPCTRL_H
