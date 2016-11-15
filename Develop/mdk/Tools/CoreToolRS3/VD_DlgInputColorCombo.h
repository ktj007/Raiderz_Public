#pragma once
#include "ext_lib/ColourPickerCB.h"

// CDlgInputColorCombo 대화 상자입니다.

class CORETOOL_API CDlgInputColorCombo : public CDialog
{
	DECLARE_DYNAMIC(CDlgInputColorCombo)

public:
	CDlgInputColorCombo(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CDlgInputColorCombo(CString& _refColorName);
	virtual ~CDlgInputColorCombo();

	const char* GetColorName();
	COLORREF GetColorValue(){ return m_crColor_; }

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_INPUT_COLOR_COMBOBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CColourPickerCB	m_ctlColorCombo_;
	COLORREF m_crColor_;
	CString m_cstrColorName_;
};
