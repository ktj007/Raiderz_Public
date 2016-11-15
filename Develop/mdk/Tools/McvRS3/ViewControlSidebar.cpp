// AnimationDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ViewControlSidebar.h"
#include "resource.h"
#include "Mcv.h"


// CViewControlSidebar 대화 상자입니다.

IMPLEMENT_DYNAMIC(CViewControlSidebar, CDialog)

CViewControlSidebar::CViewControlSidebar(CWnd* pParent /*=NULL*/)
	: CDialog()
{

}

CViewControlSidebar::~CViewControlSidebar()
{
}

LRESULT CViewControlSidebar::HandleInitDialog(WPARAM wParam, LPARAM lParam)
{
	OnInitDialog();

	return CDialog::HandleInitDialog(wParam, lParam);
}

BOOL CViewControlSidebar::OnInitDialog() 
{
	// TODO: Add extra initialization here
	UpdateData( FALSE);

	m_SliderVisibility.SetRange( 0, 10);
	m_SliderVisibility.SetPos( 10);

	m_SliderBoneLOD.SetRange( 0, 10);
	m_SliderBoneLOD.SetPos( 10);

	return TRUE;  
}

void CViewControlSidebar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CViewControlSidebar)
	DDX_Control(pDX, IDC_SLIDER_VISIBILITY, m_SliderVisibility);
	DDX_Control(pDX, IDC_SLIDER_BONE_LOD, m_SliderBoneLOD);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewControlSidebar, CDialog)
	//{{AFX_MSG_MAP(CViewControlSidebar)
	ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_VISIBILITY_RESET, &CViewControlSidebar::OnBnClickedBtnVisibilityReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CViewControlSidebar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if((void*)pScrollBar==(void*)&m_SliderVisibility)
	{
		float fVisibility = (float)m_SliderVisibility.GetPos() * 0.1f;

		RActor* pActor = GetMainView()->GetActor();
		if( pActor )
			pActor->SetVisibility( fVisibility );

		CString str;
		str.Format( "Visibility : %.1f", fVisibility);
		GetDlgItem( IDC_STATIC_VISIBILITY)->SetWindowText( str);

		return;
	}

// 	else if((void*)pScrollBar==(void*)&m_SliderBoneLOD)
// 	{
// 		float fLODLevel = (float)m_SliderBoneLOD.GetPos() * 0.1f;
// 		// bone lod update
// 		GetMainView()->GetActor()->GetAnimationController().SetBoneLODLevel(fLODLevel);
// 
// 		char buffer[256];
// 		sprintf( buffer,"LOD : %d / %d / %d",
// 			GetMainView()->GetActor()->GetAnimationController().GetActiveBoneCount(),
// 			GetMainView()->GetActor()->GetAnimationController().GetBoneCount(),
// 			GetMainView()->GetActor()->GetActorNodeCount());
// 
// 		GetDlgItem(IDC_STATIC_LOD_INFO)->SetWindowText(buffer);
// 
// 		return;
// 	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CViewControlSidebar::OnBnClickedBtnVisibilityReset()
{
	RActor* pActor = GetMainView()->GetActor();
	if( pActor )
	{
		pActor->SetVisibility( 1.0f );

		m_SliderVisibility.SetPos( 100 );
		GetDlgItem( IDC_STATIC_VISIBILITY)->SetWindowText( "Visibility : 1.0");
	}
}
