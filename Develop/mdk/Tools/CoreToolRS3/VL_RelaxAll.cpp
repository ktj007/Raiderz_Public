// V_DlgRelaxAll.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VL_RelaxAll.h"

//#include "CoreToolRS3.h"
#include "C_ControlMediatorHelperResource.h"
#include "M_ToolTerrain.h"
#include "M_ToolCmdMgr.h"
#include "M_CommandBuffer.h"

// CDlgRelaxAll 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgRelaxAll, CDialog)

CDlgRelaxAll::CDlgRelaxAll(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRelaxAll::IDD, pParent)
{
	m_nRelaxFactor_ = 30;
	m_pTerrain_ = NULL;
}

CDlgRelaxAll::~CDlgRelaxAll()
{
}

BOOL CDlgRelaxAll::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgRelaxAll::Listen(CoreMessage& _message)
{
	if(_message.nID == VIEW_MESSAGE::ACTIVE_VIEW)
	{
		if(m_pTerrain_ == NULL)
		{
			AfxMessageBox("Terrain is NULL!");
			return;
		}
		UpdateData(FALSE);
		ShowWindow(SW_SHOW);
	}
	else if(_message.nID == VIEW_MESSAGE::SCENE_LOADED)
	{
		//편집할 지형 검사
		m_pTerrain_ = (rs3::RToolTerrain*) m_pControlMediator_->GetToolSceneManager()->GetCurrentTerrain();
	}
}

void CDlgRelaxAll::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RELAX_ALL, m_nRelaxFactor_);
}

void CDlgRelaxAll::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void CDlgRelaxAll::OnOK()
{
	if(m_pTerrain_ == NULL) return;

	UpdateData(TRUE);

	//모래시계 시작
	HCURSOR hOld = SetCursor(LoadCursor(NULL, IDC_WAIT));

	//Record command
	CTerrainHeightCmdBuffer* pCmdBuffer = new CTerrainHeightCmdBuffer(m_pTerrain_);
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(pCmdBuffer);

	//act relax all
	m_pTerrain_->ApplyRelaxAction(m_nRelaxFactor_);

	//모래시계 끝
	SetCursor(hOld);
}

BEGIN_MESSAGE_MAP(CDlgRelaxAll, CDialog)
END_MESSAGE_MAP()

// CDlgRelaxAll 메시지 처리기입니다.