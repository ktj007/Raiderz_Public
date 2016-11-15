// V_EidtTerrainHeight.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VR_TerrainHeight.h"

//#include "CoreToolRS3.h"
#include "C_ControlMediator.h"
#include "V_MFCHelper.h"

// CRollupTerrainHeight 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupTerrainHeight, CDialog)
CRollupTerrainHeight::CRollupTerrainHeight(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupTerrainHeight::IDD, pParent)
{
	m_fEditSize = 0.0f;
}

CRollupTerrainHeight::~CRollupTerrainHeight()
{
}

BOOL CRollupTerrainHeight::OnInitDialog()
{
	CDialog::OnInitDialog();

	// UI Initialize
	//작업영역
	CMFCHelper::RegisterBrushWorkArea(&m_sliderCtrlSize, &m_fEditSize, &m_cstrViewSize);
	CMFCHelper::SetMFCSlider(&m_sliderCtrlStr, 1, 200, 0);
	m_fStr = (float)m_sliderCtrlStr.GetPos()*0.5f;

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CRollupTerrainHeight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SLIDER_HEIGHT_AREA, m_sliderCtrlSize);
	//DDX_Text(pDX, IDC_EDIT_HEIGHT_AREA, m_fEditSize);
	DDX_Text(pDX, IDC_EDIT_HEIGHT_AREA, m_cstrViewSize);
	DDX_Control(pDX, IDC_SLIDER_HEIGHT_STR, m_sliderCtrlStr);
	DDX_Text(pDX, IDC_EDIT_HEIGHT_STR, m_fStr);
}


BEGIN_MESSAGE_MAP(CRollupTerrainHeight, CDialog)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


void CRollupTerrainHeight::Listen(CoreMessage& _message){
	if(_message.nID == VIEW_MESSAGE::ACTIVE_VIEW){
		m_pControlMediator_->SetCurrentWBState(WORKBENCH_STATE::TERRAIN_HEIGHT);
	}
}

// CRollupTerrainHeight 메시지 처리기입니다.

void CRollupTerrainHeight::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar){

	//맵 편집 영역
	int size = m_sliderCtrlSize.GetPos();
	CMFCHelper::SetBrushWorkAreaSize(size);

	//세기
	m_fStr = (float)m_sliderCtrlStr.GetPos() * 0.5f;

	UpdateData(FALSE);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRollupTerrainHeight::OnCancel()
{
}
void CRollupTerrainHeight::OnOK()
{
}
