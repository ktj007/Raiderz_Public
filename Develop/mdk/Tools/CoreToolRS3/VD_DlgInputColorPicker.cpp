// VD_DlgInputColorPicker.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgInputColorPicker.h"
#include "V_MFCHelper.h"


// CDlgInputColorPicker 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInputColorPicker, CDialog)

CDlgInputColorPicker::CDlgInputColorPicker(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputColorPicker::IDD, pParent)
{

}

CDlgInputColorPicker::CDlgInputColorPicker(COLORREF _color, BOOL _bAutoColor)
	: CDialog(CDlgInputColorPicker::IDD, NULL)
{
	BYTE byteR = CMFCHelper::GetRed( _color );
	BYTE byteG = CMFCHelper::GetGreen( _color );
	BYTE byteB = CMFCHelper::GetBlue( _color );
	m_crColor_ = RGB( byteR, byteG, byteB );

	m_nAutoColor = _bAutoColor;
}

CDlgInputColorPicker::~CDlgInputColorPicker()
{
}

BOOL CDlgInputColorPicker::OnInitDialog(){
	CDialog::OnInitDialog();

	m_ctlAutoEditR_.SetMaxChars(3);
	m_ctlAutoEditR_.SetValidChar(_T("0123456789"));
	m_ctlAutoEditR_.SetValidationHandler(&m_validatorColorPicker_);
    m_ctlAutoEditR_.SetDefaultValue(_T("0"));

	m_ctlAutoEditG_.SetMaxChars(3);
	m_ctlAutoEditG_.SetValidChar(_T("0123456789"));
	m_ctlAutoEditG_.SetValidationHandler(&m_validatorColorPicker_);
    m_ctlAutoEditG_.SetDefaultValue(_T("0"));

	m_ctlAutoEditB_.SetMaxChars(3);
	m_ctlAutoEditB_.SetValidChar(_T("0123456789"));
	m_ctlAutoEditB_.SetValidationHandler(&m_validatorColorPicker_);
    m_ctlAutoEditB_.SetDefaultValue(_T("0"));

	UpdateColorEditBox_();
	UpdateData(FALSE);
	EnableManualColor_();

	return TRUE;
}

void CDlgInputColorPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_COLOR_PICK_R, m_ctlAutoEditR_);
	DDX_Control(pDX, IDC_EDIT_COLOR_PICK_G, m_ctlAutoEditG_);
	DDX_Control(pDX, IDC_EDIT_COLOR_PICK_B, m_ctlAutoEditB_);

	DDX_Check(pDX, IDC_CHECK_AUTO_COLOR, m_nAutoColor);
}


BEGIN_MESSAGE_MAP(CDlgInputColorPicker, CDialog)
	ON_STN_CLICKED(IDC_COLOR_PICK_COLOR, &CDlgInputColorPicker::OnStnClickedColorPickColor)
	ON_BN_CLICKED(IDC_CHECK_AUTO_COLOR, &CDlgInputColorPicker::OnBnClickedCheckAutoColor)
END_MESSAGE_MAP()

// CDlgInputColorPicker 메시지 처리기입니다.

void CDlgInputColorPicker::OnStnClickedColorPickColor()
{
	CColorDialog dlg( m_crColor_ , CC_FULLOPEN);
	if ( dlg.DoModal() == IDOK )
	{
		m_crColor_ = dlg.GetColor();
		UpdateColorEditBox_();
		CMFCHelper::FillWnd( GetDlgItem(IDC_COLOR_PICK_COLOR), m_crColor_);
	}
}

void CDlgInputColorPicker::UpdateColorEditBox_()
{
	CString cstrColor;
	cstrColor.Format("%d", GetRValue( m_crColor_ ) );
    m_ctlAutoEditR_.SetText(cstrColor);

	cstrColor.Format("%d", GetGValue( m_crColor_ ) );
    m_ctlAutoEditG_.SetText(cstrColor);

	cstrColor.Format("%d", GetBValue( m_crColor_ ) );
    m_ctlAutoEditB_.SetText(cstrColor);
}

void CDlgInputColorPicker::EnableManualColor_()
{
	if(m_nAutoColor)
	{
		GetDlgItem(IDC_EDIT_COLOR_PICK_R)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_COLOR_PICK_G)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_COLOR_PICK_B)->EnableWindow(FALSE);
		GetDlgItem(IDC_COLOR_PICK_COLOR)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_COLOR_PICK_R)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_COLOR_PICK_G)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_COLOR_PICK_B)->EnableWindow(TRUE);
		GetDlgItem(IDC_COLOR_PICK_COLOR)->EnableWindow(TRUE);
	}
}

LRESULT CDlgInputColorPicker::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = CDialog::WindowProc(message, wParam, lParam);

	switch(message) {
		case WM_PAINT:
		{
			CMFCHelper::FillWnd( GetDlgItem(IDC_COLOR_PICK_COLOR), m_crColor_);
		}
		break;
		case WM_USER_AUTOEDIT_VALID:
		{
			CString cstrColor;

			m_ctlAutoEditR_.GetWindowText(cstrColor);
			int nR = atoi(cstrColor);

			m_ctlAutoEditG_.GetWindowText(cstrColor);
			int nG = atoi(cstrColor);

			m_ctlAutoEditB_.GetWindowText(cstrColor);
			int nB = atoi(cstrColor);

			m_crColor_ = RGB( nR, nG, nB );
			CMFCHelper::FillWnd( GetDlgItem(IDC_COLOR_PICK_COLOR), m_crColor_);
		}
		break;
	}
	return result;
}

void CDlgInputColorPicker::OnBnClickedCheckAutoColor()
{
	UpdateData(TRUE);
	EnableManualColor_();
}
