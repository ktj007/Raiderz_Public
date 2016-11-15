#pragma once

#include ".\TreeListCtrl\ResourceAnalyzeTreeListCtrl.h"
#include "afxwin.h"

// CPerformanceViewerDialog 대화 상자입니다.

class RResourceAnalyzeDialog : public CDialog
{
	DECLARE_DYNAMIC(RResourceAnalyzeDialog)

public:
	RResourceAnalyzeDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~RResourceAnalyzeDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PERFORMANCE };

	void UpdateProfileInfo(char* pInputBuffer,int Size);
	void UpdateTreeItemData();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	void		 SetDefaultSize();

	DECLARE_MESSAGE_MAP()

private:
	CResourceAnalyzeTreeListCtrl m_PerformanceTreeListCtrl;
	CButton m_btnExport;
	CButton m_btnUpdate;

	bool m_bUpdated;
	bool m_bSortLess;

public:
	afx_msg void OnBnClickedUpdate();
	afx_msg void OnBnClickedExport();
	afx_msg void OnHdnItemclickMaterialList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
