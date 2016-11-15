// CXDC.h  Version 1.0
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
//              NAME                             DESCRIPTION
//   ---------------------------  ---------------------------------------------
//   Construction
//      CXDC()                    Ctor
//      CXDC(HWND)                Ctor from HWND
//   Attributes
//      HDC()                     Operator HDC - returns HDC
//      m_hDC                     HDC member variable
//   Operations
//      Attach()                  Attaches a HDC
//      Detach()                  Detaches HDC
//      GetPixel()                Gets RGB color at pixel
//      SetPixelV()               Sets RGB color
//      BitBlt()                  Copies bits from source DC to this DC
//      FillSolidRect()           Fills rect with RGB color
//      CreateCompatibleDC()      Creates a memory DC compatible with specified device
//      CreateCompatibleBitmap()  Creates a bitmap compatible with the device that 
//                                is associated with the specified DC
//      SelectObject()            Selects an object into the specified DC
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CXDC_H
#define CXDC_H

#include <crtdbg.h>

#pragma warning(disable : 4127)	// for _ASSERTE: conditional expression is constant

#ifndef NO_COLOR
#define NO_COLOR ((COLORREF)-1)
#endif

class CXDC
{
// Construction
public:
	CXDC() : m_hDC(0), m_hWnd(0), m_bRelease(FALSE), m_bDelete(FALSE) {}
	CXDC(HWND hWnd) : m_hDC(0), m_hWnd(0), m_bRelease(FALSE), m_bDelete(FALSE)
	{
		m_hWnd = hWnd;
		_ASSERTE(m_hWnd && IsWindow(m_hWnd));
		if (m_hWnd && IsWindow(m_hWnd))
		{
			m_hDC = ::GetDC(m_hWnd);
			_ASSERTE(m_hDC);
			m_bRelease = m_hDC != 0;
		}
	}
	CXDC(HDC hdc) : m_hDC(hdc), m_hWnd(0), m_bRelease(FALSE), m_bDelete(FALSE) {}

	virtual ~CXDC()
	{
		if (m_hDC && m_bRelease)
		{
			::ReleaseDC(m_hWnd, m_hDC);
		}
		if (m_hDC && m_bDelete)
		{
			::DeleteDC(m_hDC);
		}
		m_hDC = 0;
		m_hWnd = 0;
	}

// Attributes
public:
	HDC m_hDC;
	operator HDC() const
	{
		return this == 0 ? 0 : m_hDC;
	}

// Operations
public:
	BOOL Attach(HDC hdc)
	{
		BOOL rc = hdc != 0;
		_ASSERTE(hdc);
		_ASSERTE(m_hDC == 0);
		m_hDC = hdc;
		m_bRelease = FALSE;
		m_bDelete = FALSE;
		return rc;
	}

	HDC Detach()
	{
		HDC rc = m_hDC;
		m_hDC = 0;
		return rc;
	}

	COLORREF GetPixel(int x,	// x-coordinate of pixel
					  int y)	// y-coordinate of pixel
	{
		COLORREF cr = NO_COLOR;
		_ASSERTE(m_hDC);
		if (m_hDC)
			cr = ::GetPixel(m_hDC, x, y);
		return cr;
	}

	BOOL SetPixelV(int x,		// x-coordinate of pixel
				   int y,		// y-coordinate of pixel
				   COLORREF cr)	// new pixel color
	{
		BOOL rc = FALSE;
		_ASSERTE(m_hDC);
		if (m_hDC)
			rc = ::SetPixelV(m_hDC, x, y, cr);
		return rc;
	}

	BOOL BitBlt(int x,			// x-coord of destination upper-left corner
				int y,			// y-coord of destination upper-left corner
				int nWidth,		// width of destination rectangle
				int nHeight,	// height of destination rectangle
				CXDC *pSrcDC,	// pointer to source CXDC object
				int xSrc,		// x-coordinate of source upper-left corner
				int ySrc,		// y-coordinate of source upper-left corner
				DWORD dwRop)	// raster operation code
	{
		BOOL rc = FALSE;
		_ASSERTE(m_hDC);
		_ASSERTE(pSrcDC);
		if (m_hDC && pSrcDC && pSrcDC->m_hDC)
			rc = ::BitBlt(m_hDC, x, y, nWidth, nHeight, 
						pSrcDC->m_hDC, xSrc, ySrc, dwRop);
		return rc;
	}

	void FillSolidRect(int x, int y, int cx, int cy, COLORREF cr)
	{
		_ASSERTE(m_hDC);
		if (m_hDC)
		{
			::SetBkColor(m_hDC, cr);
			RECT rect = { x, y, x + cx, y + cy };
			::ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, &rect, 0, 0, 0);
		}
	}

	BOOL CreateCompatibleDC(HDC hdc)
	{
		BOOL rc = FALSE;
		_ASSERTE(hdc);
		_ASSERTE(m_hDC == 0);
		if (hdc)
		{
			m_hDC = ::CreateCompatibleDC(hdc);
			_ASSERTE(m_hDC);
			rc = m_hDC != 0;
			m_bDelete = TRUE;
			m_bRelease = FALSE;
		}
		return rc;
	}

	BOOL CreateCompatibleDC(CXDC *pDC)
	{
		BOOL rc = FALSE;
		_ASSERTE(pDC && pDC->m_hDC);
		_ASSERTE(m_hDC == 0);
		if (pDC && pDC->m_hDC)
		{
			m_hDC = ::CreateCompatibleDC(pDC->m_hDC);
			_ASSERTE(m_hDC);
			rc = m_hDC != 0;
			m_bDelete = TRUE;
			m_bRelease = FALSE;
		}
		return rc;
	}

	HBITMAP CreateCompatibleBitmap(int nWidth, int nHeight)
	{
		HBITMAP hbitmap = 0;
		_ASSERTE(m_hDC);
		if (m_hDC)
			hbitmap = ::CreateCompatibleBitmap(m_hDC, nWidth, nHeight);
		return hbitmap;
	}

	HGDIOBJ SelectObject(HGDIOBJ hgdiobj)
	{
		HGDIOBJ old = 0;
		_ASSERTE(m_hDC);
		if (m_hDC)
			old = ::SelectObject(m_hDC, hgdiobj);
		return old;
	}

// Implementation
protected:
	HWND	m_hWnd;
	BOOL	m_bRelease;
	BOOL	m_bDelete;
};

#endif //CXDC_H
