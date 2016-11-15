#pragma once

#include "resource.h"


class CAnimationSidebar : public CDialog
{
	DECLARE_DYNAMIC(CAnimationSidebar)

public:
	//{{AFX_DATA(CAnimationSidebar)  
	enum { IDD = IDD_DIALOG_ANIMATION };

	CTreeCtrl	m_cAnimationTreeCtrl;
	CListBox	m_cEventListBox;
	//}}AFX_DATA 


public:
	CAnimationSidebar(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAnimationSidebar();

	void UpdateEventList();

protected:
	void _resize( int nID, int delta, bool top_anchor);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	//{{AFX_MSG(CAnimationSidebar)
	afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
	afx_msg void OnTvnSelchangedAnimationTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkAnimationTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonAddani();
	afx_msg void OnBnClickedButtonEditani();
	afx_msg void OnBnClickedButtonDelani();
	afx_msg void OnBnClickedButtonAddevent();
	afx_msg void OnBnClickedButtonEditevent();
	afx_msg void OnBnClickedButtonDelevent();
	afx_msg void OnLbnSelchangeEventList();
	afx_msg void OnLbnDblclkEventList();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
