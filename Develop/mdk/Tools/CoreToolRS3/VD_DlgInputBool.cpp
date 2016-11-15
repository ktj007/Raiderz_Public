// VD_DlgInputBool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgInputBool.h"


// CDlgInputBool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInputBool, CDialog)

CDlgInputBool::CDlgInputBool(const char* _pSzWindowName, int _nTrueChecked, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputBool::IDD, pParent), m_cstrWindowName_(_pSzWindowName)
{
	m_nBoolValue = _nTrueChecked;
}

CDlgInputBool::~CDlgInputBool()
{
}

BOOL CDlgInputBool::OnInitDialog()
{
	CDialog::OnInitDialog();
	if( !m_cstrWindowName_.IsEmpty() )
		SetWindowText(m_cstrWindowName_);

	return TRUE;
}

void CDlgInputBool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_FALSE, m_nBoolValue);
}


BEGIN_MESSAGE_MAP(CDlgInputBool, CDialog)
END_MESSAGE_MAP()

// CDlgInputBool 메시지 처리기입니다.
