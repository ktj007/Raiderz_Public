// VD_DlgNpcNew.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgInputNpc.h"
#include "VL_ResourceInfo.h"

// CDlgInputNpc 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInputNpc, CDialog)

bool CDlgInputNpc::m_isActive = false;

CDlgInputNpc::CDlgInputNpc(CControlMediator* _pControlMediator,
	int _nID, const char* _pSzWindowName, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputNpc::IDD, pParent), m_cstrWindowName_(_pSzWindowName)
{
	CDlgInputNpc::m_isActive = true;
	m_pControlMediator_ = _pControlMediator;
	m_nNpcID = _nID;
}

CDlgInputNpc::~CDlgInputNpc()
{
	CDlgInputNpc::m_isActive = false;
}

void CDlgInputNpc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NPC_ID, m_nNpcID);
}

BOOL CDlgInputNpc::OnInitDialog()
{
	CDialog::OnInitDialog();
	if( !m_cstrWindowName_.IsEmpty() )
		SetWindowText(m_cstrWindowName_);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgInputNpc, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_NPC_MGR, &CDlgInputNpc::OnBnClickedButtonOpenNpcMgr)
END_MESSAGE_MAP()


// CDlgInputNpc 메시지 처리기입니다.

void CDlgInputNpc::OnBnClickedButtonOpenNpcMgr()
{
	UpdateData(TRUE);
	CDlgResourceInfo dlg(m_pControlMediator_, CDlgResourceInfo::RES_NPC);
	if (dlg.DoModal()==IDOK )
	{
		m_nNpcID = dlg.m_nSelectedNpcID;
		UpdateData(FALSE);
	}
}
