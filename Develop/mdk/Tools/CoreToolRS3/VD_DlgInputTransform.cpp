// VD_DlgInputTransform.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgInputTransform.h"
#include "VD_DlgInputCommon.h"


// CDlgInputTransform 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInputTransform, CDialog)

CDlgInputTransform::CDlgInputTransform(const char* _pObjectName, const rs3::RVector3* _pPos, const rs3::RVector3* _pDir, const rs3::RVector3* _pUp, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputTransform::IDD, pParent)
{
	CDlgInputCommon::m_isActive = true;

	if (_pObjectName)
		m_cstrWindowName_ = _pObjectName;

	using namespace rs3;

	if (_pPos)
	{
		m_fPos_[0] = _pPos->x;
		m_fPos_[1] = _pPos->y;
		m_fPos_[2] = _pPos->z;
		m_bUsingPos_ = true;
	}
	else
	{
		m_fPos_[0] = 0.f;
		m_fPos_[1] = 0.f;
		m_fPos_[2] = 0.f;
		m_bUsingPos_ = false;
	}

	if (_pDir)
	{
		m_fDir_[0] = _pDir->x;
		m_fDir_[1] = _pDir->y;
		m_fDir_[2] = _pDir->z;
		m_bUsingDir_ = true;
	}
	else
	{
		m_fDir_[0] = 0.f;
		m_fDir_[1] = 1.f;
		m_fDir_[2] = 0.f;
		m_bUsingDir_ = false;
	}

	if (_pUp)
	{
		m_fUp_[0] = _pUp->x;
		m_fUp_[1] = _pUp->y;
		m_fUp_[2] = _pUp->z;
		m_bUsingUp_ = true;
	}
	else
	{
		m_fUp_[0] = 0.f;
		m_fUp_[1] = 0.f;
		m_fUp_[2] = 1.f;
		m_bUsingUp_ = false;
	}
}

CDlgInputTransform::~CDlgInputTransform()
{
	CDlgInputCommon::m_isActive = false;
}

void CDlgInputTransform::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_TM_POS_X, m_fPos_[0]);
	DDX_Text(pDX, IDC_EDIT_TM_POS_Y, m_fPos_[1]);
	DDX_Text(pDX, IDC_EDIT_TM_POS_Z, m_fPos_[2]);

	DDX_Text(pDX, IDC_EDIT_TM_DIR_X, m_fDir_[0]);
	DDX_Text(pDX, IDC_EDIT_TM_DIR_Y, m_fDir_[1]);
	DDX_Text(pDX, IDC_EDIT_TM_DIR_Z, m_fDir_[2]);

	DDX_Text(pDX, IDC_EDIT_TM_UP_X, m_fUp_[0]);
	DDX_Text(pDX, IDC_EDIT_TM_UP_Y, m_fUp_[1]);
	DDX_Text(pDX, IDC_EDIT_TM_UP_Z, m_fUp_[2]);
}

BOOL CDlgInputTransform::OnInitDialog()
{
	CDialog::OnInitDialog();

	// SOMETHING
	if(!m_cstrWindowName_.IsEmpty())
		SetWindowText(m_cstrWindowName_);

	if (!m_bUsingPos_)
	{
		GetDlgItem(IDC_EDIT_TM_POS_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TM_POS_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TM_POS_Z)->EnableWindow(FALSE);
	}

	if (!m_bUsingDir_)
	{
		GetDlgItem(IDC_EDIT_TM_DIR_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TM_DIR_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TM_DIR_Z)->EnableWindow(FALSE);
	}

	if (!m_bUsingUp_)
	{
		GetDlgItem(IDC_EDIT_TM_UP_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TM_UP_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TM_UP_Z)->EnableWindow(FALSE);
	}

	return TRUE;
}

void CDlgInputTransform::OnOK()
{
	CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(CDlgInputTransform, CDialog)
END_MESSAGE_MAP()

// CDlgInputTransform 메시지 처리기입니다.
