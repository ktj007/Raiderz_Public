// V_EditTerrainUserMaterial.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VR_TerrainUserMaterial.h"

#include "V_MFCHelper.h"
#include "M_ToolTerrain.h"


// CRollupTerrainUserMaterial 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupTerrainUserMaterial, CDialog)

CRollupTerrainUserMaterial::CRollupTerrainUserMaterial(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupTerrainUserMaterial::IDD, pParent)
	, m_cstrViewSize(_T(""))
	, m_dwEditID(0)
{
	m_fEditSize = 0.0f;
	m_pTerrain_ = NULL;
}

CRollupTerrainUserMaterial::~CRollupTerrainUserMaterial()
{
}

void CRollupTerrainUserMaterial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_USERMATERIAL, m_sliderCtrlSize);

	DDX_Text(pDX, IDC_EDIT_USERMATERIAL_AREA, m_cstrViewSize);
	DDX_Text(pDX, IDC_EDIT_USERMATERIAL_ID, m_dwEditID);
	DDX_Control(pDX, IDC_CHECK_VIEW, m_btnView);
}


BEGIN_MESSAGE_MAP(CRollupTerrainUserMaterial, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_VIEW, &CRollupTerrainUserMaterial::OnBnClickedCheckView)
	ON_EN_CHANGE(IDC_EDIT_USERMATERIAL_ID, &CRollupTerrainUserMaterial::OnEnChangeEditUsermaterialId)
END_MESSAGE_MAP()


// CRollupTerrainUserMaterial 메시지 처리기입니다.
using namespace rs3;

BOOL CRollupTerrainUserMaterial::OnInitDialog()
{
	CDialog::OnInitDialog();

	// UI Initialize
	CMFCHelper::RegisterBrushWorkArea(&m_sliderCtrlSize, &m_fEditSize, &m_cstrViewSize);
	m_btnView.SetCheck(0);

	return TRUE;
}

void CRollupTerrainUserMaterial::Listen(CoreMessage& _message)
{
	switch( _message.nID )
	{
	case VIEW_MESSAGE::ACTIVE_VIEW :
		//m_pControlMediator_->SetCurrentWBState( WORKBENCH_STATE::TERRAIN_USERMATERIAL );
	break;
	case VIEW_MESSAGE::SCENE_LOADED :
	case VIEW_MESSAGE::LOGIC_LOADED:
		m_pTerrain_ = (RToolTerrain*) m_pControlMediator_->GetToolSceneManager()->GetCurrentTerrain();
	break;
	}
}

void CRollupTerrainUserMaterial::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//맵 편집 영역
	int size = m_sliderCtrlSize.GetPos();
	CMFCHelper::SetBrushWorkAreaSize(size);

	UpdateData(FALSE);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRollupTerrainUserMaterial::OnBnClickedCheckView()
{
	bool bRenderUserMaterial = m_btnView.GetCheck() ? true : false;
	
	//m_pTerrain_->SetUserMaterialFlag(bRenderUserMaterial);
	//if ( bRenderUserMaterial )
	//{
	//	UpdateData(TRUE);
	//	m_pTerrain_->UpdateUserMaterialMap(m_dwEditID);
	//}
}

void CRollupTerrainUserMaterial::OnEnChangeEditUsermaterialId()
{
	OnBnClickedCheckView();
}
