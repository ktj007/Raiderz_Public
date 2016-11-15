#pragma once

#include "resource.h"

// CViewControlSidebar 대화 상자입니다.

class CViewControlSidebar : public CDialog
{
	DECLARE_DYNAMIC(CViewControlSidebar)

public:
	//{{AFX_DATA(CViewControlSidebar)        
	enum { IDD = IDD_DIALOG_VIEWCONTROL };

	CSliderCtrl m_SliderVisibility;
	CSliderCtrl m_SliderBoneLOD;
	//}}AFX_DATA 


public:
	CViewControlSidebar(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CViewControlSidebar();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	//{{AFX_MSG(CViewControlSidebar)
	afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedBtnVisibilityReset();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
