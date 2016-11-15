#if !defined(AFX_INFOWND_H_10662CE4_AFD2_11d4_869E_0050DA426F23_INCLUDED_)
#define AFX_INFOWND_H_10662CE4_AFD2_11d4_869E_0050DA426F23_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InfoWnd.h : header file
//

#define INFOWINDOW_CLASSNAME _T("ST_InfoWnd")

#define INFOWND_ATTACH_BOTTOM   1
#define INFOWND_ATTACH_LEFT     2
#define INFOWND_ATTACH_TOP      4
#define INFOWND_ATTACH_RIGHT    8

/////////////////////////////////////////////////////////////////////////////
// CInfoWnd window

class CInfoWnd : public CWnd
{
// Construction
public:
	CInfoWnd(COLORREF cr = RGB(255,129,129));
	virtual ~CInfoWnd();
	virtual BOOL Create( CWnd *pParentWnd);

// Attributes
public:

// Operations
public:
	void Show(CWnd* p, LPCTSTR lpszTitleText);
    void Hide();

    void AttachToSide(int nAttach) {m_nAttach = nAttach;};
    void SetOffsets(int nHorizontal, int nVertical) {m_nHorizontalOffset = nHorizontal;m_nVerticalOffset = nVertical;};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoWnd)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:

protected:
	CWnd *m_pParentWnd;
	CRect m_rectTitle;
    CRect m_rectDisplay;
    CString m_strTitle;		// for use in OnPaint()
    CBrush* m_pBkColor;
    int m_nHorizontalOffset;
    int m_nVerticalOffset;
    int m_nAttach; // INFOWND_ATTACH_TOP, INFOWND_ATTACH_BOTTOM, INFOWND_ATTACH_RIGHT, INFOWND_ATTACH_LEFT
	// Generated message map functions
protected:
	//{{AFX_MSG(CInfoWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFOWND_H_10662CE4_AFD2_11d4_869E_0050DA426F23_INCLUDED_)
