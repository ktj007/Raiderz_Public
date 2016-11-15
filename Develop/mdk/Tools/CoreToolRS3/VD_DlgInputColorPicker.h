#pragma once

#include "ext_lib/CAutoEdit/AutoEdit.h"
#include "ext_lib/CAutoEdit/AutoEditValidator.h"


// CDlgInputColorPicker 대화 상자입니다.

class CORETOOL_API CDlgInputColorPicker : public CDialog
{
	DECLARE_DYNAMIC(CDlgInputColorPicker)

public:
	CDlgInputColorPicker(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CDlgInputColorPicker(DWORD _color, BOOL _bAutoColor);
	virtual ~CDlgInputColorPicker();

	COLORREF GetColorValue(){ return m_crColor_; }
	int m_nAutoColor;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_INPUT_COLOR_PICKER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg void OnStnClickedColorPickColor();
	afx_msg void OnBnClickedCheckAutoColor();

	DECLARE_MESSAGE_MAP()

private:

	void UpdateColorEditBox_();
	void EnableManualColor_();

	CString m_cstrColorValue_;
	COLORREF m_crColor_;

	CAutoEdit m_ctlAutoEditR_;
	CAutoEdit m_ctlAutoEditG_;
	CAutoEdit m_ctlAutoEditB_;

	CColorPickerValidate m_validatorColorPicker_;
};
