// CXRect.h  Version 1.0
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
//           NAME                             DESCRIPTION
//   ---------------------   -------------------------------------------------
//   Construction
//      CXRect()             Ctor
//      CXRect(RECT)         Ctor from RECT
//      CXRect(LPCRECT)      Ctor from LPCRECT
//   Attributes
//      IsRectEmpty()        Determines whether CXRect is empty. CXRect is empty 
//                           if the width and/or height are 0
//      IsRectNull()         Determines whether the top, bottom, left, and right 
//                           member variables are all equal to 0
//      Width()              Returns width of rect
//      Height()             Returns height of rect
//      PtInRect()           Determines if point is in rect
//   Operations
//      CopyRect()           Copies the dimensions of a source rectangle to CXRect
//      InflateRect()        Increases the width and height of rect
//      DeflateRect()        Decreases the width and height of rect
//      OffsetRect()         Moves rect by the specified offsets
//      SetRect()            Sets the dimensions of CXRect
//      SetRectEmpty()       Sets CXRect to an empty rectangle (all coordinates equal to 0)
//      SwapLeftRight()      Swaps left and right
//      operator LPRECT      Converts a CXRect to an LPRECT
//      operator LPCRECT     Converts a CXRect to an LPCRECT
//      operator=            Copies the dimensions of a rectangle to CXRect
//      operator==           Determines whether CXRect is equal to a rectangle
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CXRECT_H
#define CXRECT_H


class CXRect : public tagRECT
{
// Construction
public:
	CXRect(int l = 0, int t = 0, int r = 0, int b = 0) 
	{ 
		left = l; 
		top = t; 
		right = r; 
		bottom = b; 
	}
	CXRect(const RECT& srcRect)	{ ::CopyRect(this, &srcRect); }
	CXRect(LPCRECT lpSrcRect) { ::CopyRect(this, lpSrcRect); }

// Attributes
public:
	BOOL IsRectEmpty() const { return ::IsRectEmpty(this); }
	BOOL IsRectNull() const { return (left == 0 && right == 0 && top == 0 && bottom == 0); }
	int Width() const { return right - left; }
	int Height() const { return bottom - top; }
	BOOL PtInRect(POINT point) const { return ::PtInRect(this, point); }

// Operations
public:
	void CopyRect(LPCRECT lpSrcRect) { ::CopyRect(this, lpSrcRect); }
	void InflateRect(int x, int y) { ::InflateRect(this, x, y); }
	void InflateRect(SIZE size) { ::InflateRect(this, size.cx, size.cy); }
	void DeflateRect(int x, int y) { ::InflateRect(this, -x, -y); }
	void DeflateRect(SIZE size) { ::InflateRect(this, -size.cx, -size.cy); }
	void OffsetRect(int x, int y) { ::OffsetRect(this, x, y); }
	void OffsetRect(POINT point) { ::OffsetRect(this, point.x, point.y); }
	void OffsetRect(SIZE size) { ::OffsetRect(this, size.cx, size.cy); }
	void SetRect(int x1, int y1, int x2, int y2) { ::SetRect(this, x1, y1, x2, y2); }
	void SetRectEmpty() { ::SetRectEmpty(this); }
	void SwapLeftRight() { SwapLeftRight(LPRECT(this)); }
	static void SwapLeftRight(LPRECT lpRect) { LONG temp = lpRect->left; 
											   lpRect->left = lpRect->right; 
											   lpRect->right = temp; }

	operator LPRECT() { return this; }
	operator LPCRECT() const { return this; }
	void operator=(const RECT& srcRect) { ::CopyRect(this, &srcRect); }
	BOOL operator==(const RECT& rect) const { return ::EqualRect(this, &rect); }
};

#endif //CXRECT_H
