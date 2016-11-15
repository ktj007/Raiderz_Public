// XColorSpectrumCtrl.h  Version 1.1 - article available at www.codeproject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// License:
//     This software is released into the public domain.  You are free to use
//     it in any way you like, except that you may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XCOLORSPECTRUMCTRL_H
#define XCOLORSPECTRUMCTRL_H

#include "CXDC.h"
#include "CXRect.h"
#include "CXToolTipCtrl.h"

#ifndef NO_COLOR
#define NO_COLOR ((COLORREF)-1)
#endif

extern UINT WM_XCOLORPICKER_SELCHANGE;
extern UINT WM_XCOLORPICKER_SELENDOK;

//=============================================================================
class CXColorSpectrumCtrl
//=============================================================================
{
// Construction
public:
	CXColorSpectrumCtrl();
	virtual ~CXColorSpectrumCtrl();


// Attributes
public:
	HWND m_hWnd;

	enum TOOLTIP_FORMAT
	{
		XCOLOR_TOOLTIP_NONE = 0,
		XCOLOR_TOOLTIP_RGB,
		XCOLOR_TOOLTIP_HTML,
		XCOLOR_TOOLTIP_VB,
		XCOLOR_TOOLTIP_HSL
	};

	COLORREF GetBackground() { return m_crBackground; }
	void GetHSL(BYTE* h, BYTE* s, BYTE* l);
	COLORREF GetRGB();
	TOOLTIP_FORMAT GetTooltipFormat() { return m_eToolTipFormat; }
	CXColorSpectrumCtrl& SetBackground(COLORREF cr) 
	{ 
		m_crBackground = cr; 
		return *this;
	}
	CXColorSpectrumCtrl& SetHSL(BYTE h, BYTE s, BYTE l);
	CXColorSpectrumCtrl& SetRGB(COLORREF cr);
	CXColorSpectrumCtrl& SetTooltipFormat(TOOLTIP_FORMAT eFormat);


// Operations
public:

// Overrides
public:
	virtual BOOL Create(HINSTANCE hInstance, DWORD dwStyle, const RECT& rect, 
		HWND hParent, UINT nID, COLORREF crInitialColor = RGB(0,0,0),
		TOOLTIP_FORMAT eToolTipFormat = XCOLOR_TOOLTIP_NONE);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult);


// Implementation
protected:
	HWND			m_hParent;			// parent window
	COLORREF		m_crBackground;		// background color; defaults to transparent
	CXDC			m_dcSpectrum;		// device context for control, used for 
										// color determination and to restore 
										// bitmap
	HBITMAP			m_bmpSpectrum;		// spectrum bitmap
	HBITMAP			m_OldBitmap;		// old bitmap
	CXRect			m_rectCtrl;			// rect of color control, passed in Create()
	CXRect			m_rectSlider;		// rect of luminosity slider
	CXRect			m_rectSpectrumClient;	// spectrum client rect
	CXRect			m_rectSliderClient;	// slider client rect
	POINT			m_ptCurrent;		// location in spectrum
	int				m_nLuminosity;		// luminosity y coord
	COLORREF		m_crLastSent;		// last color sent to parent
	COLORREF		m_crCurrent;		// current color
	BYTE			m_Hue, m_Sat, m_Lum;	// hsl values
	BOOL			m_bSliderDrag;		// TRUE = slider is being dragged
	BOOL			m_bCrosshairDrag;	// TRUE = crosshair is being dragged
	UINT			m_nDlgCode;			// code to return from OnGetDlgCode()
	BOOL			m_bIsSpectrumFocused; // TRUE = spectrum has focus
	BOOL			m_bIsSliderFocused;	// TRUE = slider has focus
	CXToolTipCtrl *	m_pToolTip;			// pointer to tooltip control
	TOOLTIP_FORMAT	m_eToolTipFormat;	// color value format

	void CreateTooltip();
	void DeleteAll();
	void DrawArrow(CXDC *pDC, int starty, BOOL bHasFocus, BOOL bSendColor);
	void DrawCrosshair(CXDC *pDC, int startx, int starty, BOOL bHasFocus, BOOL bSendColor);
	void DrawLuminosityBar(CXDC *pDC);
	void DrawSpectrum(CXDC *pDC);
	BOOL GetClientCursorPos(POINT& point);
	int GetLuminosity();
	POINT GetPointFromHsl();

	void GoDown(int nDelta);
	void GoLeft(int nDelta);
	void GoRight(int nDelta);
	void GoUp(int nDelta);
	void Internal_SetHSL(BYTE h, BYTE s, BYTE l);

	BOOL IsCtrlDown();
	BOOL IsFocused();
	BOOL IsLeftButtonDown();
	BOOL IsPointInSlider(POINT point);
	BOOL IsPointInSpectrum(POINT point);
	BOOL IsShiftDown();

	LRESULT KeyDown(WPARAM wParam, LPARAM lParam);
	void SendColorToParent(UINT nMessage, COLORREF cr);
	void SetHslFromPoint(POINT point);
	void SetLuminosity(int nLuminosity);
};

/////////////////////////////////////////////////////////////////////////////

#endif //XCOLORSPECTRUMCTRL_H
