// testShaderDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CtestShaderDlg dialog
class CtestShaderDlg : public CDialog
{
// Construction
public:
	CtestShaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTSHADER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_lbAllComponents;
	CListBox m_lbAddedComponents;

	void RefreshAllComponents();
	afx_msg void OnLbnDblclkListAll();
	afx_msg void OnBnClickedOk();
};
