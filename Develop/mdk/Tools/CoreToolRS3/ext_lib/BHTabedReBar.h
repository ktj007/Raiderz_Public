/********************************************************************
	Created:	5:6:2004   16:44
	Filename: 	BHTabedReBar
	File ext:	h
	Author:		Bahrudin Hrnjica
	e-mail:		bhrnjica@hotmail.com
	Web Page:	www.baha.dreamstation.com
	
	Disclaimer
	----------
	THIS SOFTWARE AND THE ACCOMPANYING FILES ARE DISTRIBUTED "AS IS" AND WITHOUT
	ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED. NO REPONSIBILITIES FOR POSSIBLE
	DAMAGES OR EVEN FUNCTIONALITY CAN BE TAKEN. THE USER MUST ASSUME THE ENTIRE
	RISK OF USING THIS SOFTWARE.
*********************************************************************/

#pragma once
#include "afximpl.h"
#include "afxpriv.h"    
#include <afxtempl.h>
#include <vector>

#include "WM_USER.h"

//main definitions
#define IDC_TABED_REBAR						0x5107
#define AFX_IDW_TABED_REBAR					0xE806

// CBHTabedReBar
//struct for tab items
typedef struct
{
	CControlBar *pWnd;
	CString  strTitle;
}TCB_ITEM;


class CBHTabedReBar : public CControlBar
{
	DECLARE_DYNAMIC(CBHTabedReBar)
	DECLARE_MESSAGE_MAP()
public:
	CBHTabedReBar();
	virtual ~CBHTabedReBar();

	// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBHTabedReBar)
	virtual void	OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual BOOL	Create(	CWnd* pParentWnd,
							DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
							UINT nID = AFX_IDW_TABED_REBAR);
	virtual CSize	CalcFixedLayout( BOOL bStretch, BOOL bHorz );
	virtual void	DoPaint(CDC* pDC);		 // overriden virtual member
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CBHTabedReBar)
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult) ;
	afx_msg void	OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	// Implementation
protected:
	void			DrawGripper(CDC* pDC);   //Draws griper on left side of	  the control
public:
	BOOL			AddBar(CControlBar* pBar, LPCTSTR pszText = NULL, bool _bRecalcLayout = false);//Adds any bars derived from CWnd
	void			ClearBar();
	CTabCtrl&		GetTabCtrl() const;						// for access to the tab control

	//extended by pok
	void			ChangeTabSelection(int _num);
	int				GetActiveTab();

	//Data Members
protected:
	int								m_nToobBarHeight;  //height of tab
	int								m_nActiveTab;	   // currently active tab
	CRect							m_rectClient;	   // client rect of the control
	CList <TCB_ITEM *,TCB_ITEM *>	m_Bars;			   // list of derived ControlBars
	CTabCtrl						m_tabctrl;		   // Tab Control
};

inline int CBHTabedReBar::GetActiveTab() { return m_nActiveTab; }