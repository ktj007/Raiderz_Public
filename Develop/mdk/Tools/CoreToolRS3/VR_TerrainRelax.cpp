// V_EditTerrainRelax.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VR_TerrainRelax.h"
#include "V_MFCHelper.h"


// CRollupTerrainRelax 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupTerrainRelax, CDialog)

CRollupTerrainRelax::CRollupTerrainRelax(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupTerrainRelax::IDD, pParent)
{

}

CRollupTerrainRelax::~CRollupTerrainRelax()
{
}

BOOL CRollupTerrainRelax::OnInitDialog()
{
	CDialog::OnInitDialog();

	// UI Initialize
	//작업영역
	CMFCHelper::RegisterBrushWorkArea(&m_sliderCtrlSize, &m_fEditSize, &m_cstrViewSize);

	CMFCHelper::SetMFCSlider(&m_sliderCtrlStr, 1, 100, 0);
	m_fStr = (float)m_sliderCtrlStr.GetPos() * 0.2f;

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CRollupTerrainRelax::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//컨트롤 이름
	DDX_Control(pDX, IDC_SLIDER_RELAX_AREA, m_sliderCtrlSize);
	//DDX_Text(pDX, IDC_EDIT_RELAX_AREA, m_fEditSize);
	DDX_Text(pDX, IDC_EDIT_RELAX_AREA, m_cstrViewSize);
	DDX_Control(pDX, IDC_SLIDER_RELAX_STR, m_sliderCtrlStr);
	DDX_Text(pDX, IDC_EDIT_RELAX_STR, m_fStr);
}

void CRollupTerrainRelax::OnCancel()
{
}
void CRollupTerrainRelax::OnOK()
{
}

BEGIN_MESSAGE_MAP(CRollupTerrainRelax, CDialog)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CRollupTerrainRelax 메시지 처리기입니다.

void CRollupTerrainRelax::Listen(CoreMessage& _message)
{
	if(_message.nID == VIEW_MESSAGE::ACTIVE_VIEW){
		m_pControlMediator_->SetCurrentWBState(WORKBENCH_STATE::TERRAIN_RELAX);
	}
}

void CRollupTerrainRelax::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar){

	//맵 편집 영역
	int size = m_sliderCtrlSize.GetPos();
	CMFCHelper::SetBrushWorkAreaSize(size);

	//세기
	m_fStr = (float)m_sliderCtrlStr.GetPos() * 0.2f;

	UpdateData(FALSE);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
