#pragma once
#include "afxwin.h"

#include <string>
#include <vector>

// CDlgNewMapTypeSelector 대화 상자입니다.

class CDlgNewMapTypeSelector : public CDialog
{
	DECLARE_DYNAMIC(CDlgNewMapTypeSelector)

public:
	CDlgNewMapTypeSelector(std::vector<std::string>& _rInput, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgNewMapTypeSelector();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_NEW_MAP_TYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ctlComboMapType;
	std::vector<std::string> m_vecMapType;
	CString m_strSelected;
	afx_msg void OnCbnSelchangeComboMapType();
};
