// VD_DlgInputColorCombo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgInputColorCombo.h"

// CDlgInputColorCombo 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInputColorCombo, CDialog)

CDlgInputColorCombo::CDlgInputColorCombo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputColorCombo::IDD, pParent)
{
}

CDlgInputColorCombo::CDlgInputColorCombo(CString& _refColorName)
	: CDialog(CDlgInputColorCombo::IDD, NULL), m_cstrColorName_(_refColorName)
{
}

CDlgInputColorCombo::~CDlgInputColorCombo()
{
}

BOOL CDlgInputColorCombo::OnInitDialog()
{
	CDialog::OnInitDialog();
	int nSelected = m_ctlColorCombo_.SelectString(0, m_cstrColorName_);
	if( nSelected > -1 ) m_ctlColorCombo_.SetCurSel(nSelected);

	return TRUE;
}

void CDlgInputColorCombo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COLOR, m_ctlColorCombo_);
	DDX_ColourPickerCB( pDX, IDC_COMBO_COLOR, m_crColor_);
}

const char* CDlgInputColorCombo::GetColorName()
{
	return  CColourPickerCB::GetColorName(m_crColor_);
}

BEGIN_MESSAGE_MAP(CDlgInputColorCombo, CDialog)
END_MESSAGE_MAP()


// CDlgInputColorCombo 메시지 처리기입니다.
