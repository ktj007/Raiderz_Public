// LightSidebar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LightSidebar.h"
#include "Mcv.h"

// CLightSidebar 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLightSidebar, CDialog)

CLightSidebar::CLightSidebar(CWnd* pParent /*=NULL*/)
	: CDialog(CLightSidebar::IDD, pParent)
	, m_dwDirLightDiffuseR(0)
	, m_dwDirLightDiffuseG(0)
	, m_dwDirLightDiffuseB(0)
	, m_dwDirLightAmbientR(255)
	, m_dwDirLightAmbientG(255)
	, m_dwDirLightAmbientB(255)
{
}

CLightSidebar::~CLightSidebar()
{
}

void CLightSidebar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_DIRLIGHT_DIFFUSE_R, m_dwDirLightDiffuseR);
	DDX_Text(pDX, IDC_EDIT_DIRLIGHT_DIFFUSE_G, m_dwDirLightDiffuseG);
	DDX_Text(pDX, IDC_EDIT_DIRLIGHT_DIFFUSE_B, m_dwDirLightDiffuseB);

	DDX_Text(pDX, IDC_EDIT_DIRLIGHT_AMBIENT_R, m_dwDirLightAmbientR);
	DDX_Text(pDX, IDC_EDIT_DIRLIGHT_AMBIENT_G, m_dwDirLightAmbientG);
	DDX_Text(pDX, IDC_EDIT_DIRLIGHT_AMBIENT_B, m_dwDirLightAmbientB);
}

BEGIN_MESSAGE_MAP(CLightSidebar, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CHECK_ENABLE_SPECULAR, &CLightSidebar::OnBnClickedCheckEnableSpecular)
	ON_BN_CLICKED(IDC_CHECK_LIGHTING_ONLY, &CLightSidebar::OnBnClickedCheckLightingOnly)
	ON_BN_CLICKED(IDC_CHECK_DIRLIGHT_MATCH_CAMERAPOS, &CLightSidebar::OnBnClickedCheckDirlightMatchCamerapos)
	ON_STN_CLICKED(IDC_COLOR_DIRLIGHT_DIFFUSE, &CLightSidebar::OnStnClickedColorDirlightDiffuse)
	ON_STN_CLICKED(IDC_COLOR_DIRLIGHT_AMBIENT, &CLightSidebar::OnStnClickedColorDirlightAmbient)
	ON_BN_CLICKED(IDC_CHECK_POINTLIGHT1_ENABLE, &CLightSidebar::OnBnClickedCheckPointlight1Enable)
	ON_BN_CLICKED(IDC_CHECK_POINTLIGHT2_ENABLE, &CLightSidebar::OnBnClickedCheckPointlight2Enable)
	ON_BN_CLICKED(IDC_CHECK_POINTLIGHT3_ENABLE, &CLightSidebar::OnBnClickedCheckPointlight3Enable)
END_MESSAGE_MAP()

void CLightSidebar::PaintIcon( CWnd *pWnd, DWORD dwColor )
{
	CClientDC dc( pWnd );
	CRect rect;
	pWnd->GetClientRect( &rect );
	CBrush brush, *pOldBrush;

	brush.CreateSolidBrush( dwColor );
	pOldBrush = (CBrush*)dc.SelectObject( &brush );
	dc.Rectangle( &rect );
	dc.SelectObject( pOldBrush );
}

void CLightSidebar::PaintIconDirLightDiffuse()
{
	PaintIcon( GetDlgItem(IDC_COLOR_DIRLIGHT_DIFFUSE),
		RGB( m_dwDirLightDiffuseR, m_dwDirLightDiffuseG, m_dwDirLightDiffuseB ) );
}

void CLightSidebar::PaintIconDirLightAmbient()
{
	PaintIcon( GetDlgItem(IDC_COLOR_DIRLIGHT_AMBIENT),
		RGB( m_dwDirLightAmbientR, m_dwDirLightAmbientG, m_dwDirLightAmbientB ) );
}

void CLightSidebar::RepaintAllIcon()
{
	PaintIconDirLightDiffuse();
	PaintIconDirLightAmbient();
}

bool CLightSidebar::IsButtonCheck( int controlID )
{
	CButton* pCheckBtn = (CButton*)GetDlgItem( controlID );
	switch( pCheckBtn->GetCheck() )
	{
	case BST_CHECKED: return true;
	case BST_UNCHECKED: return false;
	}

	return false;
}

void CLightSidebar::UpdateDirLightDiffuse( DWORD dwColor )
{
	m_dwDirLightDiffuseR = GetRValue(dwColor);
	m_dwDirLightDiffuseG = GetGValue(dwColor);
	m_dwDirLightDiffuseB = GetBValue(dwColor);

	UpdateData( FALSE );

	PaintIconDirLightDiffuse();

	GetMain()->SetDirLightDiffuseColor( m_dwDirLightDiffuseR, m_dwDirLightDiffuseG, m_dwDirLightDiffuseB );
}

void CLightSidebar::UpdateDirLightAmbient( DWORD dwColor )
{
	m_dwDirLightAmbientR = GetRValue(dwColor);
	m_dwDirLightAmbientG = GetGValue(dwColor);
	m_dwDirLightAmbientB = GetBValue(dwColor);

	UpdateData( FALSE );

	PaintIconDirLightAmbient();

	GetMain()->SetDirLightAmbientColor( m_dwDirLightAmbientR, m_dwDirLightAmbientG, m_dwDirLightAmbientB );
}

BOOL CLightSidebar::OnInitDialog()
{
	CDialog::OnInitDialog();

	CButton* pCheckBtn = (CButton*)GetDlgItem( IDC_CHECK_ENABLE_SPECULAR );
	pCheckBtn->SetCheck( BST_CHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CLightSidebar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.

	RepaintAllIcon();
}

void CLightSidebar::OnBnClickedCheckEnableSpecular()
{
	GetMain()->SetSpecularEnable( IsButtonCheck( IDC_CHECK_ENABLE_SPECULAR ) );
}

void CLightSidebar::OnBnClickedCheckLightingOnly()
{
	GetMain()->SetLightingOnlyEnable( IsButtonCheck( IDC_CHECK_LIGHTING_ONLY ) );
}

void CLightSidebar::OnBnClickedCheckDirlightMatchCamerapos()
{
	GetMain()->SetDirLightMatchCamera( IsButtonCheck( IDC_CHECK_DIRLIGHT_MATCH_CAMERAPOS ) );
}

void CLightSidebar::OnBnClickedCheckPointlight1Enable()
{
	GetMain()->SetPointLightEnable( IsButtonCheck( IDC_CHECK_POINTLIGHT1_ENABLE ), 0 );
}

void CLightSidebar::OnBnClickedCheckPointlight2Enable()
{
	GetMain()->SetPointLightEnable( IsButtonCheck( IDC_CHECK_POINTLIGHT1_ENABLE ), 1 );
}

void CLightSidebar::OnBnClickedCheckPointlight3Enable()
{
	GetMain()->SetPointLightEnable( IsButtonCheck( IDC_CHECK_POINTLIGHT1_ENABLE ), 2 );
}

void CLightSidebar::OnStnClickedColorDirlightDiffuse()
{
	DWORD color = RGB( m_dwDirLightDiffuseR, m_dwDirLightDiffuseG, m_dwDirLightDiffuseB );

	CColorDialog dlg( color, CC_FULLOPEN );
	if( dlg.DoModal() == IDOK )
	{
		UpdateDirLightDiffuse( dlg.GetColor() );
	}
}

void CLightSidebar::OnStnClickedColorDirlightAmbient()
{
	DWORD color = RGB( m_dwDirLightAmbientR, m_dwDirLightAmbientG, m_dwDirLightAmbientB );

	CColorDialog dlg( color, CC_FULLOPEN );
	if( dlg.DoModal() == IDOK )
	{
		UpdateDirLightAmbient( dlg.GetColor() );
	}
}
