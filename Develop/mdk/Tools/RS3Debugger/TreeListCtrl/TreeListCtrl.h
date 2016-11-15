/*
 * Definition of the TreeListCtrl
 *
 * Copyright (c)1999 David Carballo, All Right Reserved
 *
 * Created June 1999
 * Revised: August 1999
 * Written by David Carballo
 * Internet: fibeto@redestb.es
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 */

#if !defined(AFX_TREELISTCTRL_H__36C21A6B_051D_11D3_96E7_000000000000__INCLUDED_)
#define AFX_TREELISTCTRL_H__36C21A6B_051D_11D3_96E7_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl window

#include <vector>

using namespace std;

struct ITEM_INDEX
{
	int nListIndex;
	int nTreeIndex;
};

class tree_list_info
{
public:
	int				m_nLevel;
	int				m_nImage;
	vector<CString>	m_lstCols;
	bool			m_bCollapsed;
};

class CTreeListCtrl: public CListCtrl
{
// Construction
public:
					CTreeListCtrl();
	BOOL			Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
public:

// Operations
public:
	ITEM_INDEX		AddItem(LPCTSTR lpszItem, int nImage, int nLevel);
	BOOL			SetItemText( ITEM_INDEX nItem, int nSubItem, LPCTSTR lpszText );
	BOOL			DeleteAllItems();

	CHeaderCtrl*	GetHeaderCtrl() const {return (CHeaderCtrl*)GetDlgItem(0);};
	int				GetColumnsCount() const {return GetHeaderCtrl()->GetItemCount();};
	LPSTR			GetColumnName(int nIndex);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListCtrl)
	protected:
	virtual void	PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual			~CTreeListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeListCtrl)
	afx_msg void	OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnDblClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	int				InsertItem(int nItem, int nPosArray, tree_list_info& info);
	void			Collapse(int nPos, int nItem);
	int				Expand(int nItem, bool bRecursive, int& nCounterInsert);
	
protected:
	CImageList				m_ilState;
	vector<tree_list_info>	m_tree;
	UINT					m_HScrollPos;
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREELISTCTRL_H__36C21A6B_051D_11D3_96E7_000000000000__INCLUDED_)
