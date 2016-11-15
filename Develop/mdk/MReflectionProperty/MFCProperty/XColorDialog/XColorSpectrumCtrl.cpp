// XColorSpectrumCtrl.cpp  Version 1.1 - see article at CodeProject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// Description:
//     XColorSpectrumCtrl implements CXColorSpectrumCtrl, a control that mimics the
//     functionality of the "Standard Colors" color picker in MS Office
//
// History
//     Version 1.1 - 2008 April 4
//     - Bug fixes
//
//     Version 1.0 - 2008 March 12
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

#include "windows.h"
#include "windowsx.h"
#include <stdio.h>
#include <tchar.h>
#include <crtdbg.h>
#include "XColorSpectrumCtrl.h"
#include "rgbhsl.h"

#pragma warning(disable : 4127)	// for _ASSERTE: conditional expression is constant
#pragma warning(disable : 4996)	// disable bogus deprecation warning
// #define UNUSED(x) x

#ifdef _DEBUG
// extracted from mfc\src\afxmem.cpp
static void * operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
{
	return ::operator new(nSize, _NORMAL_BLOCK, lpszFileName, nLine);
}
static void operator delete(void *pData, LPCSTR /*lpszFileName*/, int /*nLine*/)
{
	::operator delete(pData);
}
#define DEBUG_NEW new(THIS_FILE, __LINE__)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
//#include "XTrace.h"


//=============================================================================
// Messages sent to parent window - 
//     wParam = RGB color value 
//     lParam = control id
UINT WM_XCOLORPICKER_SELCHANGE = ::RegisterWindowMessage(_T("WM_XCOLORPICKER_SELCHANGE"));
UINT WM_XCOLORPICKER_SELENDOK  = ::RegisterWindowMessage(_T("WM_XCOLORPICKER_SELENDOK"));


//=============================================================================
// constants used in drawing
static const int ARROW_WIDTH			= 10;		// slider arrow
static const int ARROW_HEIGHT			= 17;		// slider arrow
static const int HORIZONTAL_MARGIN		= 2;		// right margin only
static const int VERTICAL_MARGIN		= ARROW_HEIGHT/2 + 3;	
													// this is necessary to 
													// make room for arrow on
													// slider bar
static const int SLIDER_WIDTH			= 10;
static const int ARROW_DELTA			= 5;		// delta for arrow keys
static const int CROSSHAIR_SIZE			= 19;		// spectrum crosshair: 
													//    height = width
static const int LUMINOSITY_BAR_WIDTH	= 14;
// cx - SLIDER_OFFSET = left side of slider area
static const int SLIDER_OFFSET			= SLIDER_WIDTH + HORIZONTAL_MARGIN;	
// cx - LUMINOSITY_BAR_OFFSET = left side of luminosity bar
static const int LUMINOSITY_BAR_OFFSET	= SLIDER_OFFSET + LUMINOSITY_BAR_WIDTH + 3;	
// cx - SPECTRUM_OFFSET = right side of spectrum
static const int SPECTRUM_OFFSET		= LUMINOSITY_BAR_OFFSET + 6;


//=============================================================================
//
// CXColorSpectrumCtrl()
//
// Purpose:     Construct CXColorSpectrumCtrl object.
//
// Parameters:  None
//
// Returns:     None
//
// Notes:       Construction is a two-step process.  First, construct the
//              CXColorSpectrumCtrl object.  Second, call CXColorSpectrumCtrl::Create() 
//              to create the CXColorSpectrumCtrl window.
//
CXColorSpectrumCtrl::CXColorSpectrumCtrl()
 :	m_hWnd(0),
	m_OldBitmap(NULL),
	m_eToolTipFormat(XCOLOR_TOOLTIP_NONE),
	m_pToolTip(NULL),
	m_nLuminosity(0),
	m_Hue(0), 
	m_Sat(0), 
	m_Lum(0),
	m_crLastSent(NO_COLOR),
	m_crCurrent(NO_COLOR),
	m_crBackground(NO_COLOR),
	m_bSliderDrag(FALSE),
	m_bCrosshairDrag(FALSE),
	m_nDlgCode(DLGC_WANTARROWS|DLGC_WANTTAB),
	m_bIsSpectrumFocused(FALSE),
	m_bIsSliderFocused(FALSE)
{
	TRACE(_T("in CXColorSpectrumCtrl::CXColorSpectrumCtrl\n"));
	m_ptCurrent.x = 0;
	m_ptCurrent.y = VERTICAL_MARGIN;
}

//=============================================================================
//
// ~CXColorSpectrumCtrl()
//
// Purpose:     Destroy CXColorSpectrumCtrl object.
//
// Parameters:  None
//
// Returns:     None
//
CXColorSpectrumCtrl::~CXColorSpectrumCtrl()
{
	DeleteAll();

	if (m_pToolTip)
		delete m_pToolTip;
	m_pToolTip = 0;
}

//=============================================================================
//
// DefWindowProcX()
//
// Purpose:     Initial window proc to dispatch messages to 
//              CXColorSpectrumCtrl::WindowProc().  This allows us to set up
//              the 'this' pointer to CXColorSpectrumCtrl instance.
//
// Parameters:  Standard windows message parameters.
//
// Returns:     LRESULT - The return value is the result of the message 
//                        processing and depends on the message.
//
static
LRESULT __stdcall DefWindowProcX(HWND hWnd,		// handle to window
								 UINT message,	// message identifier
								 WPARAM wParam,	// first message parameter
								 LPARAM lParam)	// second message parameter
{
	switch (message)
	{
		case WM_CREATE:
		{
			// save 'this' pointer in windows extra memory - the lParam
			// is set when ::CreateWindowEx() is called
			CREATESTRUCT* pcs = (CREATESTRUCT *) lParam;
			if (!pcs)
			{
				TRACE(_T("ERROR - CREATESTRUCT lParam is zero\n"));
				_ASSERTE(pcs);
				return -1;		// abort creation
			}
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pcs->lpCreateParams);
			return 0;
		}
		break;

		default:
		{
			// dispatch via saved 'this' pointer
			LONG_PTR lData = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (lData)
			{
				CXColorSpectrumCtrl *pCtrl = (CXColorSpectrumCtrl *) lData;
				return pCtrl->WindowProc(message, wParam, lParam);
			}
			else
			{
				// probably some WM_NCxxxx message
				TRACE(_T("GWLP_USERDATA = 0 for message = 0x%04X\n"), message);
			}
		}
		break;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

//=============================================================================
//
// Create()
//
// Purpose:     This virtual function creates the CXColorSpectrumCtrl window.
//
// Parameters:  hInstance      - handle to the instance that contains 
//                               the window procedure 
//              dwStyle        - specifies the window style attributes
//              rect           - the size and position of the window
//              hParent        - the parent window HWND
//              nID            - the ID of the child window
//              crInitialColor - initial color selection
//              eToolTipFormat - tooltip format
//
// Returns:     BOOL - TRUE = window created successfully
//
BOOL CXColorSpectrumCtrl::Create(HINSTANCE hInstance,
								 DWORD dwStyle, 
								 const RECT& rect, 
								 HWND hParent, 
								 UINT nID,
								 COLORREF crInitialColor /*= RGB(0,0,0)*/,
								 TOOLTIP_FORMAT eToolTipFormat /*= XCOLOR_TOOLTIP_NONE*/)
{
	TRACE(_T("in CXColorSpectrumCtrl::Create\n"));

	m_hWnd = 0;

	m_hParent = hParent;
	_ASSERTE(IsWindow(m_hParent));
	if (!IsWindow(m_hParent))
		return FALSE;

	m_rectCtrl = rect;
	TRACERECT(m_rectCtrl);
	TRACE(_T("width=%d  height=%d\n"), m_rectCtrl.Width(), m_rectCtrl.Height());

	m_rectSlider = m_rectCtrl;
	TRACERECT(m_rectSlider);

	TCHAR * pszClassName = _T("XColorSpectrumCtrl");

	WNDCLASS wc =
	{
		CS_DBLCLKS,									// class style - want WM_LBUTTONDBLCLK messages
		DefWindowProcX,								// window proc
		0,											// class extra bytes
		0,											// window extra bytes
		hInstance,									// instance handle
		0,											// icon
		::LoadCursor(0, IDC_ARROW),					// cursor
		0,											// background brush
		0,											// menu name
		pszClassName								// class name
	};

	if (!::RegisterClass(&wc))
	{
		DWORD dwLastError = GetLastError();
		if (dwLastError != ERROR_CLASS_ALREADY_EXISTS)
		{
			TRACE(_T("ERROR - RegisterClass failed, GetLastError() returned %u\n"), 
				dwLastError);
			_ASSERTE(FALSE);
			return FALSE;
		}
	}

	// we pass 'this' pointer as lpParam, so DefWindowProcX will see it 
	// in WM_CREATE message
	m_hWnd = ::CreateWindowEx(0, pszClassName, _T(""), dwStyle, 
		m_rectCtrl.left, m_rectCtrl.top, m_rectCtrl.Width(), m_rectCtrl.Height(),
		hParent, (HMENU)nID, hInstance, this);

	if (m_hWnd == 0)
	{
#ifdef _DEBUG
		DWORD dwLastError = GetLastError();
		UNUSED(dwLastError);
		TRACE(_T("ERROR - CreateWindowEx failed, GetLastError() returned %u\n"), 
			dwLastError);
		_ASSERTE(m_hWnd);
#endif
		return FALSE;
	}

	// set up rect for spectrum
	::GetClientRect(m_hWnd, &m_rectSpectrumClient);
	m_rectSliderClient = m_rectSpectrumClient;
	m_rectSpectrumClient.right -= SPECTRUM_OFFSET;
	m_rectSpectrumClient.top += VERTICAL_MARGIN;
	m_rectSpectrumClient.bottom -= VERTICAL_MARGIN;

	// set up rect for slider
	m_rectSliderClient.left = m_rectSliderClient.right - LUMINOSITY_BAR_OFFSET;
	m_rectSliderClient.right -= HORIZONTAL_MARGIN;
	m_rectSliderClient.top += VERTICAL_MARGIN - ARROW_HEIGHT/2;
	m_rectSliderClient.bottom -= VERTICAL_MARGIN - ARROW_HEIGHT/2;

	m_crLastSent = m_crCurrent = crInitialColor;
	RGBtoHSL(m_crLastSent, &m_Hue, &m_Sat, &m_Lum);
	TRACE(_T("m_crCurrent=%06X  h=%d  s=%d  l=%d\n"), 
		m_crCurrent, m_Hue, m_Sat, m_Lum);

	m_ptCurrent = GetPointFromHsl();
	m_nLuminosity = GetLuminosity();

	DrawCrosshair(NULL, m_ptCurrent.x, m_ptCurrent.y, FALSE, FALSE);
	DrawArrow(NULL, m_nLuminosity, FALSE, FALSE);

	m_eToolTipFormat = eToolTipFormat;

	CreateTooltip();

	return m_hWnd != 0;
}

//=============================================================================
//
// CreateTooltip()
//
// Purpose:     This function creates the CXColorSpectrumCtrl tooltip.
//
// Parameters:  None
//
// Returns:     None
//
void CXColorSpectrumCtrl::CreateTooltip()
{
	TRACE(_T("in CXColorSpectrumCtrl::CreateTooltip\n"));

	if (m_hWnd && (m_eToolTipFormat != XCOLOR_TOOLTIP_NONE))
	{
		_ASSERTE(m_pToolTip == NULL);

		m_pToolTip = new CXToolTipCtrl;
		_ASSERTE(m_pToolTip);
		if (m_pToolTip)
		{
			if (!m_pToolTip->Create(m_hWnd, TTS_ALWAYSTIP))
			{
				TRACE(_T("ERROR - failed to create tooltip control\n"));
				_ASSERTE(FALSE);
				return;
			}
			m_pToolTip->SetMaxTipWidth(40);

			// include entire rect
			CXRect rectTool(0, 0, m_rectCtrl.right-m_rectCtrl.left, 
				m_rectCtrl.bottom-m_rectCtrl.top);

			if (!m_pToolTip->AddTool(m_hWnd, LPSTR_TEXTCALLBACK, &rectTool, 10000))
			{
				TRACE(_T("ERROR - failed to add tool\n"));
				_ASSERTE(FALSE);
			}
		}
	}
}

//=============================================================================
//
// GetRGB()
//
// Purpose:     This function retrieves the current RGB value.
//
// Parameters:  None
//
// Returns:     COLORREF - the RGB value
//
COLORREF CXColorSpectrumCtrl::GetRGB()
{
	TRACE(_T("GetRGB: %06X\n"), m_crCurrent);

	return m_crCurrent;
}

//=============================================================================
//
// SetRGB()
//
// Purpose:     This function sets the RGB value.
//
// Parameters:  cr - RGB value to set
//
// Returns:     CXColorSpectrumCtrl& - reference to 'this'
//
CXColorSpectrumCtrl& CXColorSpectrumCtrl::SetRGB(COLORREF cr)
{
	TRACE(_T("in CXColorSpectrumCtrl::SetRGB\n"));

	m_crLastSent = m_crCurrent = cr;
	RGBtoHSL(m_crCurrent, &m_Hue, &m_Sat, &m_Lum);

	m_ptCurrent = GetPointFromHsl();
	m_nLuminosity = GetLuminosity();

	//TRACE(_T("SetRGB:  m_ptCurrent.y=%d  m_Hue=%d  m_Sat=%d  m_Lum=%d =====\n"), m_ptCurrent.y, m_Hue, m_Sat, m_Lum);

	// update color locators, don't tell parent

	DrawCrosshair(NULL, m_ptCurrent.x, m_ptCurrent.y, 
		IsFocused() && m_bIsSpectrumFocused, FALSE);

	DrawArrow(NULL, m_nLuminosity, IsFocused() && m_bIsSliderFocused, FALSE);

	return *this;
}

//=============================================================================
//
// GetHSL()
//
// Purpose:     This function retrieves the current HSL values.
//
// Parameters:  h - BYTE pointer to retrieved h value
//              s - BYTE pointer to retrieved s value
//              l - BYTE pointer to retrieved l value
//
// Returns:     None
//
void CXColorSpectrumCtrl::GetHSL(BYTE* h, BYTE* s, BYTE* l)
{
	*h = m_Hue;
	*s = m_Sat;
	*l = m_Lum;
	//TRACE(_T("GetHSL:  m_ptCurrent.y=%d  h=%d  s=%d  l=%d =====\n"), m_ptCurrent.y, *h, *s, *l);
}

//=============================================================================
//
// SetHslFromPoint()
//
// Purpose:     This function sets the hue and sat values from point coordinates.
//
// Parameters:  point - point for color coordinates
//
// Returns:     None
//
void CXColorSpectrumCtrl::SetHslFromPoint(POINT point)
{
	int y = point.y - VERTICAL_MARGIN;
	m_Hue = (BYTE)((point.x * 239) / (m_rectSpectrumClient.Width() - 1));
	m_Sat = (BYTE)(((m_rectSpectrumClient.Height() - y - 1) * 240) / 
				(m_rectSpectrumClient.Height() - 1));
	if (m_Hue > 239)
		m_Hue = 239;
	if (m_Sat > 240)
		m_Sat = 240;
	//TRACE(_T("SetHslFromPoint:  hue=%d sat=%d  height=%d  y = %d =====\n"), m_Hue, m_Sat, m_rectSpectrumClient.Height(), y);
}

//=============================================================================
//
// Internal_SetHSL()
//
// Purpose:     Internal function to set HSL values
//
// Parameters:  h - BYTE value of h value
//              s - BYTE value of s value
//              l - BYTE value of l value
//
// Returns:     None
//
void CXColorSpectrumCtrl::Internal_SetHSL(BYTE h, BYTE s, BYTE l)
{
	if (h > 239)
		h = 239;
	if (s > 240)
		s = 240;
	if (l > 240)
		l = 240;

	m_Hue = h;
	m_Sat = s;
	m_Lum = l;

	m_crCurrent = HSLtoRGB(h, s, l);
	//TRACE(_T("Internal_SetHSL:  m_crCurrent=%06X\n"), m_crCurrent);
	m_ptCurrent = GetPointFromHsl();
	m_nLuminosity = GetLuminosity();
}

//=============================================================================
//
// SetHSL()
//
// Purpose:     This function sets the HSL value.  Called *only* by parent.
//
// Parameters:  h - BYTE h value
//              s - BYTE s value
//              l - BYTE l value
//
// Returns:     CXColorSpectrumCtrl& - reference to 'this'
//
CXColorSpectrumCtrl& CXColorSpectrumCtrl::SetHSL(BYTE h, BYTE s, BYTE l)
{
	TRACE(_T("SetHSL: y=%d   %d,%d,%d =====\n"), m_ptCurrent.y, h, s, l);

	Internal_SetHSL(h, s, l);
	m_crLastSent = m_crCurrent;
	DrawCrosshair(NULL, m_ptCurrent.x, m_ptCurrent.y, m_bIsSpectrumFocused, FALSE);
	DrawArrow(NULL, m_nLuminosity, m_bIsSliderFocused, FALSE);

	//TRACE(_T("SetHSL:  m_ptCurrent.x=%d  m_ptCurrent.y=%d =====\n"), m_ptCurrent.x, m_ptCurrent.y);
	return *this;
}

//=============================================================================
//
// GoLeft()
//
// Purpose:     This function processes left arrow keystrokes.
//
// Parameters:  nDelta - number of pixels to move
//
// Returns:     None
//
void CXColorSpectrumCtrl::GoLeft(int nDelta)
{
	if (m_bIsSpectrumFocused)
	{
		int nHue = m_Hue - nDelta;
		if (nHue < 0)
			nHue = 0;
		if (nHue > 239)
			nHue = 239;

		Internal_SetHSL((BYTE)nHue, m_Sat, m_Lum);
		DrawCrosshair(NULL, m_ptCurrent.x, m_ptCurrent.y, TRUE, TRUE);
	}
}

//=============================================================================
//
// GoRight()
//
// Purpose:     This function processes right arrow keystrokes.
//
// Parameters:  nDelta - number of pixels to move
//
// Returns:     None
//
void CXColorSpectrumCtrl::GoRight(int nDelta)
{
	if (m_bIsSpectrumFocused)
	{
		int nHue = m_Hue + nDelta;
		if (nHue < 0)
			nHue = 0;
		if (nHue > 239)
			nHue = 239;

		Internal_SetHSL((BYTE)nHue, m_Sat, m_Lum);
		DrawCrosshair(NULL, m_ptCurrent.x, m_ptCurrent.y, TRUE, TRUE);
	}
}

//=============================================================================
//
// GoUp()
//
// Purpose:     This function processes up arrow keystrokes.
//
// Parameters:  nDelta - number of pixels to move
//
// Returns:     None
//
void CXColorSpectrumCtrl::GoUp(int nDelta)
{
	if (m_bIsSliderFocused)
	{
		int nLum = m_Lum + nDelta;
		if (nLum < 0)
			nLum = 0;
		if (nLum > 240)
			nLum = 240;
		Internal_SetHSL(m_Hue, m_Sat, (BYTE)nLum);
		DrawArrow(NULL, m_nLuminosity, TRUE, TRUE);
	}
	else
	{
		int nSat = m_Sat + nDelta;
		if (nSat < 0)
			nSat = 0;
		if (nSat > 240)
			nSat = 240;

		Internal_SetHSL(m_Hue, (BYTE)nSat, m_Lum);
		DrawCrosshair(NULL, m_ptCurrent.x, m_ptCurrent.y, TRUE, TRUE);
	}
}

//=============================================================================
//
// GoDown()
//
// Purpose:     This function processes down arrow keystrokes.
//
// Parameters:  nDelta - number of pixels to move
//
// Returns:     None
//
void CXColorSpectrumCtrl::GoDown(int nDelta)
{
	if (m_bIsSliderFocused)
	{
		int nLum = m_Lum - nDelta;
		if (nLum < 0)
			nLum = 0;
		if (nLum > 240)
			nLum = 240;
		Internal_SetHSL(m_Hue, m_Sat, (BYTE)nLum);
		DrawArrow(NULL, m_nLuminosity, TRUE, TRUE);
	}
	else
	{
		int nSat = m_Sat - nDelta;
		if (nSat < 0)
			nSat = 0;
		if (nSat > 240)
			nSat = 240;

		Internal_SetHSL(m_Hue, (BYTE)nSat, m_Lum);
		DrawCrosshair(NULL, m_ptCurrent.x, m_ptCurrent.y, TRUE, TRUE);
	}
}

//=============================================================================
//
// KeyDown()
//
// Purpose:     This function processes WM_KEYDOWN messages.
//
// Parameters:  wParam - virtual-key code
//              lParam - not used
//
// Returns:     LRESULT - zero = message was processed
//
LRESULT CXColorSpectrumCtrl::KeyDown(WPARAM wParam, LPARAM /*lParam*/)
{
	TRACE(_T("in CXColorSpectrumCtrl::KeyDown\n"));

	// for the arrow keys:  
	//     if the Ctrl key is down, advance by 1
	//     if the shift key is down, advance by twice the normal delta
	int nDelta = ARROW_DELTA;	// standard advance for arrow keys
	if (IsCtrlDown())			// Ctrl key:  advance by 1
		nDelta = 1;
	else if (IsShiftDown())		// Shift key:  advance by 2x
		nDelta *= 2;

	if (wParam == VK_TAB)
	{
		// ignore tab if we don't have focus
		if (::GetFocus() != m_hWnd)
			return 1;

		if (IsShiftDown())
		{
			// shift is down, tab backwards

			if (m_bIsSliderFocused)
			{
				// slider has focus, switch to spectrum
				m_bIsSpectrumFocused = TRUE;
				m_bIsSliderFocused = FALSE;
				DrawCrosshair(NULL, m_ptCurrent.x, m_ptCurrent.y, TRUE, FALSE);
				DrawArrow(NULL, m_nLuminosity, FALSE, FALSE);
				return 0;
			}
			else
			{
				// spectrum has focus, give focus to previous control
				::PostMessage(m_hParent, WM_NEXTDLGCTL, 1, 0);
			}
		}
		else
		{
			// shift is up, tab forwards
			if (m_bIsSliderFocused)
			{
				// slider has focus, give focus to next control
				::PostMessage(m_hParent, WM_NEXTDLGCTL, 0, 0);
				return 0;
			}
			else
			{
				// spectrum has focus, switch to slider
				m_bIsSpectrumFocused = FALSE;
				m_bIsSliderFocused = TRUE;
				DrawCrosshair(NULL, m_ptCurrent.x, m_ptCurrent.y, FALSE, FALSE);
				DrawArrow(NULL, m_nLuminosity, TRUE, FALSE);
				return 0;
			}
		}
	}
	else if (wParam == VK_END)
	{
		if (m_bIsSliderFocused)
			GoDown(20000);				// position to bottom
		else
			GoRight(20000);				// position to right
	}
	else if (wParam == VK_HOME)
	{
		if (m_bIsSliderFocused)
			GoUp(20000);				// position to top
		else
			GoLeft(20000);				// position to left
	}
	else if (wParam == VK_NEXT)
	{
		GoDown(20000);					// position to bottom
	}
	else if (wParam == VK_PRIOR)
	{
		GoUp(20000);					// position to top
	}
	else if (wParam == VK_DOWN)
	{
		GoDown(nDelta);
	}
	else if (wParam == VK_UP)
	{
		GoUp(nDelta);
	}
	else if (wParam == VK_RIGHT)
	{
		GoRight(nDelta);
	}
	else if (wParam == VK_LEFT)
	{
		GoLeft(nDelta);
	}
	
	return 1;		// let default processing continue
}

//=============================================================================
//
// IsShiftDown()
//
// Purpose:     This function returns TRUE if either shift key is down.
//
// Parameters:  None
//
// Returns:     BOOL - TRUE = shift key is down
//
BOOL CXColorSpectrumCtrl::IsShiftDown()
{
	return GetAsyncKeyState(VK_SHIFT) & 0x8000;
}

//=============================================================================
//
// IsCtrlDown()
//
// Purpose:     This function returns TRUE if either Ctrl key is down.
//
// Parameters:  None
//
// Returns:     BOOL - TRUE = Ctrl key is down
//
BOOL CXColorSpectrumCtrl::IsCtrlDown()
{
	return GetAsyncKeyState(VK_CONTROL) & 0x8000;
}

//=============================================================================
//
// IsLeftButtonDown()
//
// Purpose:     This function returns TRUE if left mouse button is down.
//
// Parameters:  None
//
// Returns:     BOOL - TRUE = left mouse button is down
//
BOOL CXColorSpectrumCtrl::IsLeftButtonDown()
{
	BOOL rc = FALSE;

	SHORT state = 0;
	if (GetSystemMetrics(SM_SWAPBUTTON))		// check if buttons have been swapped
		state = GetAsyncKeyState(VK_RBUTTON);	// buttons swapped, get right button state
	else
		state = GetAsyncKeyState(VK_LBUTTON);

	// if the most significant bit is set, the button is down
	if (state < 0)
		rc = TRUE;

	return rc;
}

//=============================================================================
//
// IsFocused()
//
// Purpose:     This function returns TRUE if control has focus.
//
// Parameters:  None
//
// Returns:     BOOL - TRUE = control has focus
//
BOOL CXColorSpectrumCtrl::IsFocused()
{
	BOOL rc = FALSE;

	HWND hFocus = ::GetFocus();
	if (hFocus == m_hWnd)
		rc = TRUE;

	return rc;
}

//=============================================================================
//
// GetClientCursorPos()
//
// Purpose:     This function retrieves the cursor location relative to the
//              control's client rect.
//
// Parameters:  point - reference to the variable that receives the location
//
// Returns:     BOOL - TRUE = location is valid
//
BOOL CXColorSpectrumCtrl::GetClientCursorPos(POINT& point)
{
	BOOL rc = FALSE;

	if (::GetCursorPos(&point))		// check if screensaver kicked in
	{
		::ScreenToClient(m_hWnd, &point);
		rc = TRUE;
	}

	return rc;
}

//=============================================================================
//
// IsPointInSpectrum()
//
// Purpose:     This function determines if point is in spectrum.
//
// Parameters:  point = location to check
//
// Returns:     BOOL - TRUE = point is in spectrum
//
BOOL CXColorSpectrumCtrl::IsPointInSpectrum(POINT point)
{
	BOOL rc = m_rectSpectrumClient.PtInRect(point);
	return rc;
}

//=============================================================================
//
// IsPointInSlider()
//
// Purpose:     This function determines if point is in slider.
//
// Parameters:  point = location to check
//
// Returns:     BOOL - TRUE = point is in slider
//
BOOL CXColorSpectrumCtrl::IsPointInSlider(POINT point)
{
	BOOL rc = m_rectSliderClient.PtInRect(point);
	return rc;
}

//=============================================================================
//
// DeleteAll()
//
// Purpose:     This function deletes the saved bitmap
//
// Parameters:  None
//
// Returns:     None
//
void CXColorSpectrumCtrl::DeleteAll()
{
	if (m_OldBitmap)
		m_dcSpectrum.SelectObject(m_OldBitmap);
	m_OldBitmap = NULL;

	if (m_bmpSpectrum)
		::DeleteObject(m_bmpSpectrum);
}

//=============================================================================
//
// SetLuminosity()
//
// Purpose:     This function sets the luminosity value for the specified 
//              luminosity slider position.
//
// Parameters:  nLuminosity - luminosity slider position
//
// Returns:     None
//
void CXColorSpectrumCtrl::SetLuminosity(int nLuminosity)
{
	if (nLuminosity < 0)
		nLuminosity = 0;
	if (nLuminosity > (m_rectSpectrumClient.Height() -1))
		nLuminosity = m_rectSpectrumClient.Height() -1;
	m_nLuminosity = nLuminosity;
	m_Lum = (BYTE)(((m_rectSpectrumClient.Height() - m_nLuminosity - 1) * 240) / (m_rectSpectrumClient.Height() - 1));
	if (m_Lum > 240)
		m_Lum = 240;
	m_crCurrent = HSLtoRGB(m_Hue, m_Sat, m_Lum);
}

//=============================================================================
//
// GetLuminosity()
//
// Purpose:     This function retrieves the luminosity value for the 
//              specified RGB value.
//
// Parameters:  cr - RGB color value
//
// Returns:     int - luminosity valie
//
int CXColorSpectrumCtrl::GetLuminosity()
{
	UINT l = m_Lum;

	int lum = m_rectSpectrumClient.Height() - ((m_rectSpectrumClient.Height() * l) / 240);

	if (lum < 0)
		lum = 0;

	if (lum > m_rectSpectrumClient.Height() - 1)
		lum = m_rectSpectrumClient.Height() - 1;

	return lum;
}

//=============================================================================
//
// GetPointFromHsl()
//
// Purpose:     This function retrieves the POINT location (in spectrum) for 
//              the specified RGB value.
//
// Parameters:  cr - RGB color value
//
// Returns:     POINT - location of color
//
POINT CXColorSpectrumCtrl::GetPointFromHsl()
{
	POINT point = { 0 };

	UINT hue = m_Hue;
	UINT sat = m_Sat;

	point.x = (hue * m_rectSpectrumClient.Width()) / 239;
	if (point.x > (m_rectSpectrumClient.right-1))
		point.x = m_rectSpectrumClient.right-1;

	point.y = m_rectSpectrumClient.Height() - ((sat * m_rectSpectrumClient.Height()) / 240);
	point.y += VERTICAL_MARGIN;
	if (point.y > (m_rectSpectrumClient.bottom-1))
		point.y = m_rectSpectrumClient.bottom-1;

	return point;
}

//=============================================================================
//
// DrawSpectrum()
//
// Purpose:     This function draws the spectrum and initializes the 
//              spectrum bitmap and memory dc.
//
// Parameters:  pDC - pointer to device context used for drawing
//
// Returns:     None
//
void CXColorSpectrumCtrl::DrawSpectrum(CXDC *pDC)
{
	TRACE(_T("in CXColorSpectrumCtrl::DrawSpectrum\n"));

	_ASSERTE(pDC);
	if (!pDC)
		return;

	TRACERECT(m_rectCtrl);

	if (m_OldBitmap == NULL)
	{
		// spectrum bitmap hasn't been drawn yet

		if (!m_dcSpectrum.CreateCompatibleDC(pDC))
		{
			TRACE(_T("ERROR - CreateCompatibleDC failed\n"));
			_ASSERTE(FALSE);
			return;
		}
		m_bmpSpectrum = pDC->CreateCompatibleBitmap(m_rectCtrl.Width(), m_rectCtrl.Height());
		m_OldBitmap = (HBITMAP) m_dcSpectrum.SelectObject(m_bmpSpectrum);
		if (m_crBackground == NO_COLOR)
		{
			// no background color specified, just copy current background
			m_dcSpectrum.BitBlt(0, 0, m_rectCtrl.Width(), m_rectCtrl.Height(),
				pDC, 0, 0, SRCCOPY);
		}
		else
		{
			m_dcSpectrum.FillSolidRect(0, 0, m_rectCtrl.Width(), m_rectCtrl.Height(),
				m_crBackground);
		}

		UINT cx = m_rectCtrl.Width() - SPECTRUM_OFFSET;
		UINT cy = m_rectCtrl.Height() - 2*VERTICAL_MARGIN;

		// The saturation and luminosity values must be in the range 0 
		// through 240, and the hue value must be in the range 0 through 239.

		double cxd = cx;
		double cyd = cy;
		for (UINT y = 0; y < cy; y++)
		{
			for (UINT x = 0; x < cx; x++)
			{
				double hue = x;
				hue *= 240.0;
				hue /= cxd - 1.0;
				double sat = y;
				sat *= 240.0;
				sat /= cyd - 1.0;
				COLORREF cr = HSLtoRGB(hue, sat, 120.0);
				m_dcSpectrum.SetPixelV(x, cy - y - 1 + VERTICAL_MARGIN, cr);
			}
		}
	}

	// write spectrum bitmap to control's dc
	pDC->BitBlt(0, 0, m_rectCtrl.Width(), m_rectCtrl.Height(),
		&m_dcSpectrum, 0, 0, SRCCOPY);			
}

//=============================================================================
//
// SendColorToParent()
//
// Purpose:     This function sends the specified color to the parent.
//
// Parameters:  nMessage - message ID
//              cr       - color to send
//
// Returns:     None
//
void CXColorSpectrumCtrl::SendColorToParent(UINT nMessage, COLORREF cr)
{
	TRACE(_T("in CXColorSpectrumCtrl::SendColorToParent:  cr=%06X  m_crLastSent=%06X\n"), cr, m_crLastSent);

	// don't send duplicate messages
	//if ((nMessage == WM_XCOLORPICKER_SELENDOK) || (cr != m_crLastSent))
	{
		m_crLastSent = cr;
		if (IsWindow(m_hParent) && (cr != NO_COLOR))
		{
			TRACE(_T(">>>>> sending %06X\n"), m_crLastSent);
			::SendMessage(m_hParent, nMessage,
				m_crLastSent, ::GetDlgCtrlID(m_hWnd));
		}
	}
}

//=============================================================================
//
// DrawCrosshair()
//
// Purpose:     This function draws spectrum crosshair.
//
// Parameters:  pDC        - pointer to device context to use for drawing
//              startx     - x coord of center point
//              starty     - y coord of center point
//              bHasFocus  - TRUE = control has focus; this determines how the 
//                           crosshair is drawn
//              bSendColor - TRUE = send WM_XCOLORPICKER_SELCHANGE to parent
//
// Returns:     None
//
void CXColorSpectrumCtrl::DrawCrosshair(CXDC *pDC, 
										int startx, 
										int starty, 
										BOOL bHasFocus,
										BOOL bSendColor)
{
	TRACE(_T("in CXColorSpectrumCtrl::DrawCrosshair\n"));
	BOOL bRelease = FALSE;

	if (pDC == NULL)
	{
		pDC = new CXDC(m_hWnd);
		bRelease = TRUE;
	}

	// restore bitmap - get rid of previous selection
	if (m_OldBitmap)
		pDC->BitBlt(0, 0, m_rectCtrl.Width()-SPECTRUM_OFFSET, m_rectCtrl.Height(),
			&m_dcSpectrum, 0, 0, SRCCOPY);

	// for each byte in this array:
	//     0 = skip
	//     1 = COLOR_WINDOWTEXT (COLOR_WINDOW if bHasFocus == FALSE)
	static BYTE pixels[CROSSHAIR_SIZE][CROSSHAIR_SIZE] = 
	{
		0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 1
		0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 2
		0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 3
		0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 4
		0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 5
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// 6
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// 7
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// 8
		1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,	// 9
		1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,	// 10
		1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,	// 11
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// 12
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// 13
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// 14
		0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 15
		0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 16
		0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 17
		0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,	// 18
		0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0	// 19
	};

	COLORREF crFill = GetSysColor(COLOR_WINDOWTEXT);
	if (!bHasFocus)
		crFill = GetSysColor(COLOR_WINDOW);

	POINT pt;
	for (int row = 0; row < CROSSHAIR_SIZE; row++)
	{
		for (int col = 0; col < CROSSHAIR_SIZE; col++)
		{
			if (pixels[row][col] != 1)
				continue;
			pt.x = startx - CROSSHAIR_SIZE/2 + col;
			pt.y = starty - CROSSHAIR_SIZE/2 + row;
			if (m_rectSpectrumClient.PtInRect(pt))
			{
				pDC->SetPixelV(pt.x, pt.y, crFill);
			}
		}
	}

	DrawLuminosityBar(pDC);

	if (bRelease)
		delete pDC;

	// let parent know color selection has changed
	if (bSendColor)
		SendColorToParent(WM_XCOLORPICKER_SELCHANGE, m_crCurrent);
}

//=============================================================================
//
// DrawLuminosityBar()
//
// Purpose:     This function draws luminosity bar based on the color under
//              the crosshair.
//
// Parameters:  pDC - pointer to device context used for drawing
//
// Returns:     None
//
void CXColorSpectrumCtrl::DrawLuminosityBar(CXDC *pDC)
{
	TRACE(_T("in CXColorSpectrumCtrl::DrawLuminosityBar\n"));

	BOOL bRelease = FALSE;

	if (pDC == NULL)
	{
		pDC = new CXDC(m_hWnd);
		_ASSERTE(pDC);
		bRelease = TRUE;
	}

	// set up for double buffering
	CXDC memDC;
	if (!memDC.CreateCompatibleDC(pDC))
	{
		TRACE(_T("ERROR - CreateCompatibleDC failed\n"));
		_ASSERTE(FALSE);
		return;
	}
	HBITMAP bmp = pDC->CreateCompatibleBitmap(m_rectSlider.Width(), m_rectSlider.Height());
	HBITMAP hOldBitmap = (HBITMAP) memDC.SelectObject(bmp);
	memDC.BitBlt(0, 0, m_rectSlider.Width(), m_rectSlider.Height(),
		pDC, 0, 0, SRCCOPY);

	COLORREF cr = m_crCurrent;//GetColorUnderCrosshair(); //RGB(255,255,0);
	if (cr == NO_COLOR)
		return;

	DOUBLE h, s, l;
	RGBtoHSL(cr, &h, &s, &l);
	//if (h > 239.0)
	//	h = 239.0;
	if (h > 240.0)
		h = 240.0;
	if (s > 240.0)
		s = 240.0;

	UINT cx = m_rectSlider.Width();
	UINT cy = m_rectSlider.Height() - 2*VERTICAL_MARGIN;
	double cyd = cy;

	for (UINT y = 0; y < cy; y++)
	{
		l = y;
		l /= cyd - 1.0;
		l *= 240.0;
		COLORREF rgb = HSLtoRGB(h, s, l);
		for (UINT x = cx-LUMINOSITY_BAR_OFFSET; x < cx-LUMINOSITY_BAR_OFFSET+LUMINOSITY_BAR_WIDTH; x++)
			memDC.SetPixelV(x, cy - y - 1 + VERTICAL_MARGIN, rgb);
	}

	// end double buffering
	pDC->BitBlt(0, 0, m_rectSlider.Width(), m_rectSlider.Height(),
		&memDC, 0, 0, SRCCOPY);			

	// swap back the original bitmap
	if (hOldBitmap)
		memDC.SelectObject(hOldBitmap);
	if (bmp)
		::DeleteObject(bmp);

	if (bRelease)
		delete pDC;
}

//=============================================================================
//
// DrawArrow()
//
// Purpose:     This function draws slider arrow.
//
// Parameters:  pDC        - pointer to device context to use for drawing
//              starty     - y coord of arror tip
//              bHasFocus  - TRUE = control has focus; this determines how the 
//                           arrow is drawn
//              bSendColor - TRUE = send WM_XCOLORPICKER_SELCHANGE to parent
//
// Returns:     None
//
void CXColorSpectrumCtrl::DrawArrow(CXDC *pDC, 
									int starty, 
									BOOL bHasFocus, 
									BOOL bSendColor)
{
	BOOL bRelease = FALSE;

	if (pDC == NULL)
	{
		pDC = new CXDC(m_hWnd);
		bRelease = TRUE;
	}

	// restore bitmap - get rid of previous selection
	if (m_OldBitmap)
		pDC->BitBlt(m_rectCtrl.Width()-SLIDER_OFFSET, 0, SLIDER_OFFSET, 
			m_rectCtrl.Height(), &m_dcSpectrum, m_rectCtrl.Width()-SLIDER_OFFSET, 
			0, SRCCOPY);

	// for each byte in this array:
	//     0 = skip
	//     1 = COLOR_WINDOWTEXT
	//     2 = COLOR_WINDOWTEXT (COLOR_BTNSHADOW if bHasFocus == FALSE)
	static BYTE pixels[ARROW_HEIGHT][ARROW_WIDTH] = 
	{
		0,0,0,0,0,0,0,0,1,0,	// 1
		0,0,0,0,0,0,0,1,1,0,	// 2
		0,0,0,0,0,0,1,2,1,0,	// 3
		0,0,0,0,0,1,2,2,1,0,	// 4
		0,0,0,0,1,2,2,2,1,0,	// 5
		0,0,0,1,2,2,2,2,1,0,	// 6
		0,0,1,2,2,2,2,2,1,0,	// 7
		0,1,2,2,2,2,2,2,1,0,	// 8
		1,2,2,2,2,2,2,2,1,0,	// 9
		0,1,2,2,2,2,2,2,1,0,	// 10
		0,0,1,2,2,2,2,2,1,0,	// 11
		0,0,0,1,2,2,2,2,1,0,	// 12
		0,0,0,0,1,2,2,2,1,0,	// 13
		0,0,0,0,0,1,2,2,1,0,	// 14
		0,0,0,0,0,0,1,2,1,0,	// 15
		0,0,0,0,0,0,0,1,1,0,	// 16
		0,0,0,0,0,0,0,0,1,0		// 17
	};

	COLORREF crWindowText = GetSysColor(COLOR_WINDOWTEXT);
	COLORREF crBtnShadow  = GetSysColor(COLOR_BTNSHADOW);
	COLORREF crFill = crWindowText;
	if (!bHasFocus)
		crFill = crBtnShadow;

	int startx = m_rectCtrl.Width() - SLIDER_OFFSET;
	if (starty < 0)
		starty = 0;
	if (starty > m_rectSliderClient.Height() - 2*(ARROW_HEIGHT/2) - 1)
		starty = m_rectSliderClient.Height() - 2*(ARROW_HEIGHT/2) - 1;

	for (int row = 0; row < ARROW_HEIGHT; row++)
	{
		for (int col = 0; col < ARROW_WIDTH; col++)
		{
			COLORREF crFill = crWindowText;
			if (pixels[row][col] == 1)
				crFill = crWindowText;
			else if (pixels[row][col] == 2)
				if (bHasFocus)
					crFill = crWindowText;
				else
					crFill = crBtnShadow;
			else
				continue;
			pDC->SetPixelV(startx + col, 
						   starty + row - ARROW_HEIGHT/2 + VERTICAL_MARGIN, 
						   crFill);
		}
	}
	if (bRelease)
		delete pDC;

	// let parent know color selection has changed
	if (bSendColor)
		SendColorToParent(WM_XCOLORPICKER_SELCHANGE, m_crCurrent);
}

//=============================================================================
// Helper functions extracted from mfc\src\strcore.cpp
#ifndef _UNICODE

static int _mbstowcsz(wchar_t* wcstr, const char* mbstr, size_t count)
{
	if (count == 0 && wcstr != NULL)
		return 0;

	int result = ::MultiByteToWideChar(CP_ACP, 0, mbstr, -1,
		wcstr, count);
	_ASSERTE(wcstr == NULL || result <= (int)count);
	if (result > 0)
		wcstr[result-1] = 0;
	return result;
}

#else

static int _wcstombsz(char* mbstr, const wchar_t* wcstr, size_t count)
{
	if (count == 0 && mbstr != NULL)
		return 0;

	int result = ::WideCharToMultiByte(CP_ACP, 0, wcstr, -1,
		mbstr, count, NULL, NULL);
	_ASSERTE(mbstr == NULL || result <= (int)count);
	if (result > 0)
		mbstr[result-1] = 0;
	return result;
}

#endif //_UNICODE

//=============================================================================
//
// OnToolTipText()
//
// Purpose:     This virtual function is called when a TTN_NEEDTEXT is received.
//              The tooltip control send this WM_NOTIFY message when a tooltip 
//              is about to be displayed, and LPSTR_TEXTCALLBACK has been 
//              specified as the tooltip text.
//
// Parameters:  id      - Identifier of the control that sent the notification
//                        (not used).
//              pNMHDR  - A pointer to a  NMHDR structure, which is either a
//                        TOOLTIPTEXTA or a TOOLTIPTEXTW struct.
//              pResult - A pointer to the result code.
//
// Returns:     BOOL - TRUE = message handled, do not display automatic tooltip;
//                     FALSE = message not handled, allow tooltip to be 
//                     displayed, using updated TOOLTIPTEXT struct.
//
// Notes:       The tooltip is the color of the cell that the cursor 
//              is over, displayed in the format that was selected when the 
//              control was created, or set by SetTooltipFormat().
//
BOOL CXColorSpectrumCtrl::OnToolTipText(UINT /*id*/, NMHDR * pNMHDR, LRESULT * pResult)
{
	TRACE(_T("in CXColorSpectrumCtrl::OnToolTipText\n"));

	UINT nID = (UINT)pNMHDR->idFrom;
	
	// check if this is the automatic tooltip of the control
	if (nID == 0) 
		return TRUE;	// do not allow display of automatic tooltip,
						// or our tooltip will disappear
	
	// handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	
	*pResult = 0;
	
	POINT point;
	::GetCursorPos(&point);
	::ScreenToClient(m_hWnd, &point);

	COLORREF cr = NO_COLOR;

	if (IsPointInSpectrum(point))
	{
		if (m_OldBitmap)
		{
			// get color of pixel under cursor
			cr = m_dcSpectrum.GetPixel(point.x, point.y);
		}
	}
	else
	{
		// set up rect for color slider bar (do not include arrow)
		CXRect rect = m_rectSliderClient;
		rect.right = rect.left + LUMINOSITY_BAR_WIDTH;
		rect.top = m_rectSpectrumClient.top;
		rect.bottom = m_rectSpectrumClient.bottom;

		if (rect.PtInRect(point))
		{
			HDC hdc = ::GetDC(m_hWnd);

			if (hdc)
			{
				// get color of pixel under cursor
				cr = ::GetPixel(hdc, point.x, point.y);
				::ReleaseDC(m_hWnd, hdc);
			}
		}
	}

	// the max tooltip string is hardcoded as 80 in commctrl.h
	// (for both ansi and unicode)
	const int nMaxTooltipString = sizeof(pTTTA->szText);

	TCHAR szToolTip[nMaxTooltipString+2];
	szToolTip[0] = _T('\0');

	if (cr != NO_COLOR)
	{
		BYTE r = GetRValue(cr);
		BYTE g = GetGValue(cr);
		BYTE b = GetBValue(cr);
		double H, S, L;

		switch (m_eToolTipFormat)
		{
			default:
			case XCOLOR_TOOLTIP_RGB:
				_sntprintf(szToolTip, nMaxTooltipString, _T("RGB(%d,%d,%d)"), r, g, b);
				break;

			case XCOLOR_TOOLTIP_HTML:
				_sntprintf(szToolTip, nMaxTooltipString, _T("#%02X%02X%02X"), r, g, b);
				break;

			case XCOLOR_TOOLTIP_VB:
				_sntprintf(szToolTip, nMaxTooltipString, _T("%02X%02X%02X"), b, g, r);
				break;

			case XCOLOR_TOOLTIP_HSL:
				RGBtoHSL(cr, &H, &S, &L);
				H += 0.5;
				S += 0.5;
				L += 0.5;
				_sntprintf(szToolTip, nMaxTooltipString, _T("HSL(%d,%d,%d)"), (BYTE)H, (BYTE)S, (BYTE)L);
				break;
		}
	}

	szToolTip[nMaxTooltipString-1] = _T('\0');

	if (szToolTip[0] == _T('\0'))
		return TRUE;

#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, szToolTip, sizeof(pTTTA->szText));
	else
		_mbstowcsz(pTTTW->szText, szToolTip, sizeof(pTTTW->szText)/sizeof(TCHAR));
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, szToolTip, sizeof(pTTTA->szText));
	else
		lstrcpyn(pTTTW->szText, szToolTip, sizeof(pTTTW->szText)/sizeof(TCHAR));
#endif // _UNICODE

	return FALSE;	// we didn't handle the message, let the 
					// framework continue propagating the message
}

//=============================================================================
//
// SetTooltipFormat()
//
// Purpose:     This function sets the tooltip format.
//
// Parameters:  eFormat - tooltip format
//
// Returns:     CXColorSpectrumCtrl& - reference to 'this'
//
CXColorSpectrumCtrl& CXColorSpectrumCtrl::SetTooltipFormat(TOOLTIP_FORMAT eFormat)
{
	m_eToolTipFormat = eFormat;

	if (m_eToolTipFormat == XCOLOR_TOOLTIP_NONE)
	{
		if (m_pToolTip)
		{
			delete m_pToolTip;
		}
		m_pToolTip = 0;
	}
	else
	{
		if (!m_pToolTip)
			CreateTooltip();
	}
	return *this;
}

//=============================================================================
//
// WindowProc()
//
// Purpose:     This function is the window proc for CXColorSpectrumCtrl object.  
//              Messages are forwarded to this function from DefWindowProcX().
//
// Parameters:  message - message identifier
//              wParam  - first message parameter
//              lParam  - second message parameter
//
// Returns:     LRESULT - The return value is the result of the message 
//                        processing and depends on the message.
//
LRESULT CXColorSpectrumCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message)
	{
		case WM_CREATE:
			// will never see this message - it's handled by DefWindowProcX
			break;

		case WM_ERASEBKGND:
			// nothing to erase, since we draw the entire client area
			return TRUE;

		case WM_PAINT:
		{
			TRACE(_T("in WM_PAINT\n"));
			PAINTSTRUCT ps; 
			HDC hdc = (wParam != NULL) ? (HDC) wParam : ::BeginPaint(m_hWnd, &ps);
			if (hdc == 0)
				return 0;
			CXDC dc(hdc);
			DrawSpectrum(&dc);
			DrawCrosshair(&dc, m_ptCurrent.x, m_ptCurrent.y, 
				IsFocused() && m_bIsSpectrumFocused, FALSE);
			DrawArrow(&dc, m_nLuminosity, IsFocused() && m_bIsSliderFocused, FALSE);
			if (wParam == NULL)
				::EndPaint(m_hWnd, &ps);
			return 0;
		}

		case WM_KEYDOWN:
			if (!KeyDown(wParam, lParam))
				return 0;
			//::SendMessage(m_hParent, message, wParam, lParam);
			break;

		case WM_GETDLGCODE:
			return m_nDlgCode;
			break;

		case WM_LBUTTONDOWN:
		{
			POINT point;
			point.x = GET_X_LPARAM(lParam);
			point.y = GET_Y_LPARAM(lParam);
			TRACE(_T("in WM_LBUTTONDOWN:  point.x=%d  point.y=%d\n"), point.x, point.y);

			if (IsPointInSpectrum(point))
			{
				SetHslFromPoint(point);
				//TRACE(_T("m_Hue=%d  m_Sat=%d  m_Lum=%d =====\n"), m_Hue, m_Sat, m_Lum);
				m_crCurrent = HSLtoRGB(m_Hue, m_Sat, m_Lum);
				//TRACE(_T("m_crCurrent=%06X\n"), m_crCurrent);
				m_ptCurrent = point;
			}
			else if (IsPointInSlider(point))
			{
				SetLuminosity(point.y - VERTICAL_MARGIN);
				m_crCurrent = HSLtoRGB(m_Hue, m_Sat, m_Lum);
			}

			HWND hFocus = ::GetFocus();
			if (hFocus != m_hWnd)
			{
				::SetFocus(m_hWnd);		// WM_SETFOCUS draws crosshair
			}

			if (IsPointInSpectrum(point))
			{
				m_bIsSpectrumFocused = TRUE;
				m_bIsSliderFocused = FALSE;
				DrawCrosshair(NULL, m_ptCurrent.x, m_ptCurrent.y, TRUE, TRUE);
				DrawArrow(NULL, m_nLuminosity, FALSE, FALSE);
				m_bCrosshairDrag = TRUE;
			}
			else if (IsPointInSlider(point))
			{
				m_bIsSpectrumFocused = FALSE;
				m_bIsSliderFocused = TRUE;
				DrawCrosshair(NULL, m_ptCurrent.x, m_ptCurrent.y, FALSE, FALSE);
				DrawArrow(NULL, m_nLuminosity, TRUE, TRUE);
				m_bSliderDrag = TRUE;
			}
			TRACE(_T("WM_LBUTTONDOWN: m_crCurrent=%06X\n"), m_crCurrent);
			break;
		}

		case WM_LBUTTONUP:
			TRACE(_T("in WM_LBUTTONUP\n"));
			m_bSliderDrag = FALSE;
			m_bCrosshairDrag = FALSE;
			break;

		case WM_LBUTTONDBLCLK:
		{
			TRACE(_T("in CXColorSpectrumCtrl::OnLButtonDblClk\n"));

			POINT point;
			point.x = GET_X_LPARAM(lParam);
			point.y = GET_Y_LPARAM(lParam);
			// let parent know a color was selected
			if (IsPointInSlider(point) || IsPointInSpectrum(point))
				SendColorToParent(WM_XCOLORPICKER_SELENDOK, m_crCurrent);
			break;
		}

		case WM_SETFOCUS:
		{
			TRACE(_T("in WM_SETFOCUS\n"));
			LRESULT lResult = ::DefWindowProc(m_hWnd, message, wParam, lParam);

			if (IsShiftDown())
				m_bIsSliderFocused = TRUE;
			else
				m_bIsSpectrumFocused = TRUE;

			DrawCrosshair(NULL, m_ptCurrent.x, m_ptCurrent.y, 
				IsFocused() && m_bIsSpectrumFocused, FALSE);
			DrawArrow(NULL, m_nLuminosity, IsFocused() && m_bIsSliderFocused, FALSE);
			return lResult;
		}

		case WM_KILLFOCUS:
		{
			TRACE(_T("in WM_KILLFOCUS\n"));
			LRESULT lResult = ::DefWindowProc(m_hWnd, message, wParam, lParam);
			DrawCrosshair(NULL, m_ptCurrent.x, m_ptCurrent.y, FALSE, FALSE);
			DrawArrow(NULL, m_nLuminosity, FALSE, FALSE);
			m_bIsSpectrumFocused = FALSE;
			m_bIsSliderFocused = FALSE;
			return lResult;
		}

		case WM_MOUSEMOVE:
		{
			POINT point;
			point.x = GET_X_LPARAM(lParam);
			point.y = GET_Y_LPARAM(lParam);

			if (!IsLeftButtonDown())
			{
				m_bSliderDrag = FALSE;
				m_bCrosshairDrag = FALSE;
			}

			if (m_bSliderDrag)
			{
				if (IsPointInSlider(point))
				{
					SetLuminosity(point.y - VERTICAL_MARGIN);
					DrawArrow(NULL, m_nLuminosity, TRUE, TRUE);
				}
			}
			else if (m_bCrosshairDrag)
			{
				if (IsPointInSpectrum(point))
				{
					m_ptCurrent = point;
					SetHslFromPoint(m_ptCurrent);
					m_crCurrent = HSLtoRGB(m_Hue, m_Sat, m_Lum);
					DrawCrosshair(NULL, point.x, point.y, TRUE, TRUE);
				}
			}

			if (m_pToolTip)
				m_pToolTip->Update();

			break;
		}

		case WM_NOTIFY:
		{
			// for tooltips
			NMHDR *pNMHDR = (NMHDR *) lParam;
			if (pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW)
			{
				LRESULT lResult = 0;
				return OnToolTipText(0, pNMHDR, &lResult);
			}
			break;
		}
	}

	return ::DefWindowProc(m_hWnd, message, wParam, lParam);
}
