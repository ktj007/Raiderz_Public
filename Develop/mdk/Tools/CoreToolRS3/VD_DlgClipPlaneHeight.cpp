// V_DlgClipPlaneHeight.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgClipPlaneHeight.h"
#include "C_ControlMediatorHelperRender.h"
//#include "MainFrm.h"
#include "V_MFCHelper.h"

// CDlgClipPlaneHeight 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgClipPlaneHeight, CDialog)

CDlgClipPlaneHeight::CDlgClipPlaneHeight(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgClipPlaneHeight::IDD, pParent)
	, m_bShow_(false)
	, m_pControlMediator_(NULL)
	, m_bStaticMeshRender(false)
{

}

CDlgClipPlaneHeight::~CDlgClipPlaneHeight()
{
}

void CDlgClipPlaneHeight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MINZ, m_EditMinZ);
	DDX_Control(pDX, IDC_EDIT_MAXZ, m_EditMaxZ);
	DDX_Control(pDX, IDC_SLIDER_MIN, MinZ_Slider);
	DDX_Control(pDX, IDC_SLIDER_MAX, MaxZ_Slider);
}


BEGIN_MESSAGE_MAP(CDlgClipPlaneHeight, CDialog)
	ON_EN_CHANGE(IDC_EDIT_MINZ, &CDlgClipPlaneHeight::OnEnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_MAXZ, &CDlgClipPlaneHeight::OnEnChangeEdit)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CDlgClipPlaneHeight 메시지 처리기입니다.
BOOL CDlgClipPlaneHeight::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_EditMinZ.SetWindowText(_T("-10"));
	m_EditMaxZ.SetWindowText(_T("1000"));

	CMFCHelper::SetMFCSlider(&MinZ_Slider, -10000, 10000, 0);
	MinZ_Slider.SetPos(-10);
	CMFCHelper::SetMFCSlider(&MaxZ_Slider, -10000, 10000, 0);
	MaxZ_Slider.SetPos(1000);

	return TRUE;
}

void CDlgClipPlaneHeight::OnCancel()
{
	m_pControlMediator_->GetToolCamera()->SetUserClipPlane(0,0);

	if (m_bStaticMeshRender == false)
	{
		OnViewStaticRenderAll();
	}

	Hide();
}

void CDlgClipPlaneHeight::OnOK()
{
	OnEnChangeEdit();
}

void CDlgClipPlaneHeight::Hide()
{
	m_bShow_ = false;
	ShowWindow(SW_HIDE);
}

void CDlgClipPlaneHeight::ShowToggle()
{
	m_bShow_ = !m_bShow_;
	int nShowState = m_bShow_ ? SW_SHOW : SW_HIDE;
	ShowWindow(nShowState);

	if (m_bShow_)
	{
		if(m_pControlMediator_->GetHelperRender()->GetViewStaticMeshRenderAll() == false)
		{
			m_bStaticMeshRender = false;
			OnViewStaticRenderAll();
		}
		else
		{
			m_bStaticMeshRender = true;
		}

		OnEnChangeEdit();
	}
	else
	{
		OnCancel();
	}
}

void CDlgClipPlaneHeight::SetControl(CControlMediator* pControlMediator_)
{
	m_pControlMediator_ = pControlMediator_;
}

void CDlgClipPlaneHeight::OnEnChangeEdit()
{
	if(m_pControlMediator_ == NULL) return;

	CString cstrAutoEdit;

	m_EditMinZ.GetWindowText(cstrAutoEdit);
	float ClipNearZ= atof(cstrAutoEdit);

	m_EditMaxZ.GetWindowText(cstrAutoEdit);
	float ClipFarZ = atof(cstrAutoEdit);

	m_pControlMediator_->m_fClipNearZ = ClipNearZ;
	m_pControlMediator_->m_fClipFarZ = ClipFarZ;

	MPlane plane[2];
	plane[0].SetPlane( vec3::AXISZ, vec3(0.f, 0.f, ClipNearZ));
	plane[1].SetPlane(-vec3::AXISZ, vec3(0.f, 0.f, ClipFarZ));

	m_pControlMediator_->GetToolCamera()->SetUserClipPlane(&plane[0], 2);
}

void CDlgClipPlaneHeight::OnViewStaticRenderAll()
{
	m_pControlMediator_->GetHelperRender()->ToggleViewStaticMeshRenderAll();
}

void CDlgClipPlaneHeight::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	if(m_pControlMediator_ == NULL) return;

	CString cstrAutoEdit;

	float ClipNearZ = (float)MinZ_Slider.GetPos();
	float ClipFarZ = (float)MaxZ_Slider.GetPos();

	char buff[16];
	m_EditMinZ.SetWindowText(CString(itoa(ClipNearZ, buff, 10)));
	m_EditMaxZ.SetWindowText(CString(itoa(ClipFarZ, buff, 10)));

	m_pControlMediator_->m_fClipNearZ = ClipNearZ;
	m_pControlMediator_->m_fClipFarZ = ClipFarZ;

	MPlane plane[2];
	plane[0].SetPlane( vec3::AXISZ, vec3(0.f, 0.f, ClipNearZ));
	plane[1].SetPlane(-vec3::AXISZ, vec3(0.f, 0.f, ClipFarZ));

	m_pControlMediator_->GetToolCamera()->SetUserClipPlane(&plane[0], 2);
}
