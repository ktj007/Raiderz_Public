#pragma once
#include "afxwin.h"
#include "afxcmn.h"


namespace rs3 {
	class RDevice;
};
// CDlgProfile 대화 상자입니다.

class CDlgProfile : public CDialog
{
	DECLARE_DYNAMIC(CDlgProfile)

public:
	CDlgProfile(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgProfile();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PROFILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	void UpdateProfile(rs3::RDevice *pDevice);

	CListCtrl m_ctlListDP;
	CListCtrl m_ctlListUR;

	virtual BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonUpdate();
	CEdit m_editURTip;
	afx_msg void OnLvnItemchangedListUsingresourceProfile(NMHDR *pNMHDR, LRESULT *pResult);
};
