#pragma once
#include "afxwin.h"

#include "resource.h"

// CDlgInputCommon 대화 상자입니다.

class CORETOOL_API CDlgInputCommon : public CDialog
{
	DECLARE_DYNAMIC(CDlgInputCommon)

public:
	CDlgInputCommon(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CDlgInputCommon(const char* _pSzValue, const char* _pSzWindowName, const char* _pSzExtrType);
	virtual ~CDlgInputCommon();

	const char* GetInputData();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_INPUT_COMMON };
	static bool m_isActive;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CString m_cstrComboType_;
	CString m_cstrCommonValue_;
	CString m_cstrComBoValue_;
	CString m_cstrWindowName_;
	CComboBox m_ctrlComboBox_;
	afx_msg void OnCbnSelchangeEditCommonCombo();
};
