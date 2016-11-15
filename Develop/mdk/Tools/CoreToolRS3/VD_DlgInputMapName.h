#pragma once
#include <string>

// CDlgInputMapName 대화 상자입니다.

class CORETOOL_API CDlgInputMapName : public CDialog
{
	DECLARE_DYNAMIC(CDlgInputMapName)

public:
	CDlgInputMapName(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgInputMapName();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_INPUT_MAP_NAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTestDirectory();
	std::string m_strDlgCaption;
	CString m_cstrTestDirectory;
	CString m_cstrTestFile;
public:
	afx_msg void OnBnClickedOk();
};
