// Page3.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EffectTool.h"
#include "PageParticle.h"
#include "RBaseEmitter.h"

using namespace Gdiplus;

extern CRView*		g_rView;

inline void _ui_SetSlider(CSliderCtrl *pCtrl, int iMin, int iMax, int iPos)
{
	pCtrl->ClearTics();
	pCtrl->ClearSel();

	pCtrl->SetRangeMin(iMin);
	pCtrl->SetRangeMax(iMax);
	pCtrl->SetPos(iPos);
}

// CPage3 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPage3, CPPage)
CPage3::CPage3()
: CPPage(CPage3::IDD)
, m_fTime(0.0f)
, m_fAlpha(1.0f)
, m_fScale(1.0f)
, m_fScaleA(0)
, m_fVelX(0)
, m_fVelY(0)
, m_fVelZ(0)
, m_fGRate(1)
, m_fScale2(1)
//, m_fPosX(0)
//, m_fPosY(0)
//, m_fPosZ(0)
//, m_bRandColor(FALSE)
//, m_bRandVel(FALSE)
, m_fMaxVelocity(0)
, m_fMinVelocity(0)
, m_fMinLife(0)
, m_fMaxLife(0)
, m_fWind(0)
, m_fWDirX(0)
, m_fWDirY(0)
, m_fWDirZ(0)
{
}

CPage3::~CPage3()
{
}

void CPage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MINLIFE, m_fMinLife);
	DDX_Text(pDX, IDC_EDIT_MAXLIFE, m_fMaxLife);

	DDX_Text(pDX, IDC_EDIT_TIME, m_fTime);
	DDX_Text(pDX, IDC_EDIT_COLORA, m_fAlpha);
	DDX_Text(pDX, IDC_EDIT_SCALE, m_fScale);
	DDX_Text(pDX, IDC_EDIT_SCALEA, m_fScaleA);
	DDX_Text(pDX, IDC_EDIT_VELX, m_fVelX);
	DDX_Text(pDX, IDC_EDIT_VELY, m_fVelY);
	DDX_Text(pDX, IDC_EDIT_VELZ, m_fVelZ);
	DDX_Text(pDX, IDC_EDIT_GRAVITY, m_fGRate);
	DDX_Text(pDX, IDC_EDIT_SCALE2, m_fScale2);
	//DDX_Text(pDX, IDC_EDIT_POSX, m_fPosX);
	//DDX_Text(pDX, IDC_EDIT_POSY, m_fPosY);
	//DDX_Text(pDX, IDC_EDIT_POSZ, m_fPosZ);
	//DDX_Check(pDX, IDC_CHECK_COLOR_RAND, m_bRandColor);
	//DDX_Check(pDX, IDC_CHECK_VEL_RAND, m_bRandVel);
	DDX_Text(pDX, IDC_EDIT_MAXVEL, m_fMaxVelocity);
	DDX_Text(pDX, IDC_EDIT_MINVEL, m_fMinVelocity);

	DDX_Control(pDX, IDC_SLIDER_MAXLIFE, m_ctlMaxLife);
	DDX_Control(pDX, IDC_SLIDER_MINLIFE, m_ctlMinLife);

	DDX_Control(pDX, IDC_SLIDER_TIME, m_ctlSlideTrack);
	DDX_Control(pDX, IDC_LIST_TRACK, m_ctlListTrack);
	DDX_Control(pDX, IDC_EDIT_COLOR, m_ctlColor);
	DDX_Control(pDX, IDC_SLIDER_MAXVEL, m_ctlMaxVelocity);
	DDX_Control(pDX, IDC_SLIDER_MINVEL, m_ctlMinVelocity);
	DDX_Control(pDX, IDC_SLIDER_GRAVITY, m_ctlGravity);
	DDX_Control(pDX, IDC_SLIDER_SCALE, m_ctlScale2);
	DDX_Text(pDX, IDC_EDIT_WIND, m_fWind);
	DDX_Text(pDX, IDC_EDIT_WDIRX, m_fWDirX);
	DDX_Text(pDX, IDC_EDIT_WDIRY, m_fWDirY);
	DDX_Text(pDX, IDC_EDIT_WDIRZ, m_fWDirZ);
}


BEGIN_MESSAGE_MAP(CPage3, CPPage)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_LBN_SELCHANGE(IDC_LIST_TRACK, OnLbnSelchangeListTrack)
	
	ON_EN_CHANGE(IDC_EDIT_MINLIFE, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_MAXLIFE, OnEnChangeEdits)

	ON_EN_CHANGE(IDC_EDIT_TIME, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_COLORA, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_SCALE, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_SCALEA, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_MINVEL, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_MAXVEL, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_VELX, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_VELY, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_VELZ, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_GRAVITY, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_POSX, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_POSY, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_POSZ, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_SCALE2, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_COLOR, OnEnChangeEditColor)
	ON_EN_CHANGE(IDC_EDIT_WIND, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_WDIRX, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_WDIRY, OnEnChangeEdits)
	ON_EN_CHANGE(IDC_EDIT_WDIRZ, OnEnChangeEdits)

	ON_BN_CLICKED(IDC_BUTTON_P1, OnBnClickedButtonP1)
	ON_BN_CLICKED(IDC_BUTTON_P2, OnBnClickedButtonP2)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnBnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VELX, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VELY, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VELZ, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POSX, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POSY, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POSZ, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WDIRX, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WDIRY, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WDIRZ, OnDeltaposSpin)
END_MESSAGE_MAP()


// CPage3 메시지 처리기입니다.

bool CPage3::ConfigFromXML()
{
	MXml kXml;
	if ( !kXml.LoadFile(FILENAME_CONFIG))
		return false;

	MXmlElement *pElement = kXml.Doc()->FirstChildElement("texturepath");
	if ( !pElement )
		return false;

	int nValue;

	_Contents(&nValue,kXml.Doc()->FirstChildElement("life"));
	_ui_SetSlider(&m_ctlMaxLife, 0, nValue*RATE, 0);
	_ui_SetSlider(&m_ctlMinLife, 0, nValue*RATE, 0);

	_Contents(&nValue,kXml.Doc()->FirstChildElement("velocity"));
	_ui_SetSlider(&m_ctlMaxVelocity, 0, nValue*RATE, 0);
	_ui_SetSlider(&m_ctlMinVelocity, 0, nValue*RATE, 0);

	_Contents(&m_fGRateMin,kXml.Doc()->FirstChildElement("gravityratemin"));
	_Contents(&m_fGRateMax,kXml.Doc()->FirstChildElement("gravityratemax"));

	int nRange = (int)(m_fGRateMax - m_fGRateMin + 0.5f );
	_ui_SetSlider(&m_ctlGravity, 0, nRange*RATE, 1);

	_Contents(&nValue,kXml.Doc()->FirstChildElement("scale"));
	_ui_SetSlider(&m_ctlScale2, 0, nValue*RATE, 1);

	return true;
}

void CPage3::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( m_ctlMaxLife.GetPos() < m_ctlMinLife.GetPos())
		m_ctlMinLife.SetPos( m_ctlMaxLife.GetPos());

	m_fMaxLife = (float)( m_ctlMaxLife.GetPos()/(float)RATE);
	m_fMinLife = (float)( m_ctlMinLife.GetPos()/(float)RATE);

	float fPos = m_ctlSlideTrack.GetPos()/100.0f;

	m_fTime = fPos;

	if ( m_ctlMaxVelocity.GetPos() < m_ctlMinVelocity.GetPos())
		m_ctlMinVelocity.SetPos( m_ctlMaxVelocity.GetPos());

	m_fMaxVelocity = (float)(m_ctlMaxVelocity.GetPos()/(float)RATE);
	m_fMinVelocity = (float)(m_ctlMinVelocity.GetPos()/(float)RATE);
	m_fScale2 = (float)(m_ctlScale2.GetPos()/(float)RATE);
	m_fGRate = m_fGRateMin + ((float)m_ctlGravity.GetPos()/(float)m_ctlGravity.GetRangeMax())*(m_fGRateMax - m_fGRateMin);

	UpdateData(FALSE);
	SendMessageParticleModyfy();

	//  [8/14/2006 madduck]
	{
		int iSel = m_ctlListTrack.GetCurSel();
		if ( iSel < 0 )
			return;
		SetVariables(g_rView->m_pEmitter->m_vecParticleVar, iSel);
	}

	CPPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPage3::GetVariables(const std::vector<RParticleVariable> &rVar)
{
	m_ctlListTrack.ResetContent();
	TCHAR tStr[128];
	memset(tStr, 0, sizeof(tStr));

	for ( UINT ui = 0; ui<rVar.size(); ++ui )
	{
		_stprintf(tStr,_T("+%f"), rVar[ui].fEventTime);
		m_ctlListTrack.InsertString(ui,tStr);
	}

	return;
}

void CPage3::SetVariables(std::vector<RParticleVariable> &rVar, int iSel, bool btime)
{
	UpdateData(TRUE);

	if ( m_fTime < 0.0f )	m_fTime = 0.0f;
	if ( m_fTime > 1.0f )	m_fTime = 1.0f;

	if(btime)
		rVar[iSel].fEventTime = m_fTime;

	rVar[iSel].fColorA = m_fAlpha;
	rVar[iSel].fMulScale = m_fScale;
	rVar[iSel].fAddScale = m_fScaleA;

	float a;
	DWORD2RGBA( m_ctlColor.GetValue(), rVar[iSel].fColorB, rVar[iSel].fColorG, rVar[iSel].fColorR, a);

	//  [7/20/2006 madduck]
	rVar[iSel].fMaxVel	= m_fMaxVelocity;
	rVar[iSel].fMinVel	= m_fMinVelocity;

	rVar[iSel].vVel = RVector(m_fVelX, m_fVelY, m_fVelZ) * 0.1f;

	rVar[iSel].fGRate	= m_fGRate;
	rVar[iSel].fScale	= m_fScale2;

	rVar[iSel].fWind	= m_fWind;
	rVar[iSel].vWindDir = RVector(m_fWDirX, m_fWDirY, m_fWDirZ) * 0.1f;

	// [9/20/2006 madduck]
	// TODO : 이미터의 타임트랙으로 옮겨야 함.
	//rVar[iSel].vEmitterPos = RVector(m_fPosX, m_fPosY, m_fPosZ);

// 	rVar[iSel].bRandColor = m_bRandColor	? true : false;
// 	rVar[iSel].bRandVel	  = m_bRandVel		? true : false;

	UpdateData(FALSE);
}

void CPage3::UpdateGradation()
{
	CWnd *pWnd = GetDlgItem(IDC_GRAPHICS);
	ASSERT(pWnd);

	Graphics grp(pWnd->GetSafeHwnd());
	SolidBrush brushA(Color::DarkGray);
	SolidBrush brushB(Color::Gray);
	Pen pen(Color::Black);

	RECT rt;

	grp.Clear(Color(255, 255, 255, 255));
	pWnd->GetClientRect(&rt);

	int w = rt.right - rt.left;
	int h = rt.bottom - rt.top;

	// BackGround Check
	for ( int x = 0; x<w; x+=5 )
	{
		Brush *p = ( x%2 ? &brushA : &brushB );
		grp.FillRectangle( p, x, 0, x+5, h);
	}

// 	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
// 	CChildView *pView = (CChildView*)pFrame->GetView();
	std::vector<RParticleVariable> &rVar = g_rView->m_pEmitter->m_vecParticleVar;

	// Gradiant
	for ( UINT ui = 1; ui<rVar.size(); ++ui )
	{
		float r1, g1, b1, a1;
		float r2, g2, b2, a2;

		r1 = rVar[ui-1].fColorR;
		g1 = rVar[ui-1].fColorG;
		b1 = rVar[ui-1].fColorB;
		a1 = rVar[ui-1].fColorA;		

		r2 = rVar[ui].fColorR;
		g2 = rVar[ui].fColorG;
		b2 = rVar[ui].fColorB;
		a2 = rVar[ui].fColorA;

		int w1, w2, len;

		w1 = (int)(rVar[ui-1].fEventTime * w);
		w2 = (int)(rVar[ui].fEventTime * w);
		len = w2 - w1;

		for ( int x = w1, i = 0; x<w2; ++x, ++i )
		{
			float fA = (float)i/(float)len;

			BYTE r = (BYTE)(MMath::LinearInterpolation(r1, r2, fA)*255);
			BYTE g = (BYTE)(MMath::LinearInterpolation(g1, g2, fA)*255);
			BYTE b = (BYTE)(MMath::LinearInterpolation(b1, b2, fA)*255);
			BYTE a = (BYTE)(MMath::LinearInterpolation(a1, a2, fA)*255);

			pen.SetColor(Color(a, r, g, b));
			grp.DrawLine(&pen, x, 0, x, h/2);
			pen.SetColor(Color(255, r, g, b));
			grp.DrawLine(&pen, x, h/2, x, h);
		}
	}
}
void CPage3::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	if( g_rView->m_pEmitter == NULL)	return;

	UpdateGradation();

	// 그리기 메시지에 대해서는 CPPage::OnPaint()을(를) 호출하지 마십시오.
}

void CPage3::OnLbnSelchangeListTrack()
{
	int iSel = m_ctlListTrack.GetCurSel();
	if ( iSel < 0 )
		return;

	//CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	//CChildView *pView = (CChildView*)pFrame->GetView();

	if( g_rView->m_pEmitter == NULL)	return;

	std::vector<RParticleVariable> &rVar = g_rView->m_pEmitter->m_vecParticleVar;

	DWORD dwColor = D3DCOLOR_COLORVALUE(rVar[iSel].fColorB, rVar[iSel].fColorG, rVar[iSel].fColorR, 1.0f);
	dwColor &= 0x00ffffff;

	m_ctlColor.SetValue( dwColor, true);
	m_ctlColor.SetHexColor( dwColor, 0x00);

	m_fTime		= rVar[iSel].fEventTime;
	m_fAlpha	= rVar[iSel].fColorA;
	m_fScale	= rVar[iSel].fMulScale;
	m_fScaleA	= rVar[iSel].fAddScale;

	int iPos = (int)(m_fTime * 100.0f);
	if ( iPos < 0 )		iPos = 0;
	if ( iPos > 100 )	iPos = 100;
	m_ctlSlideTrack.SetPos(iPos);

	//  [7/20/2006 madduck]
	m_fMaxVelocity = rVar[iSel].fMaxVel;
	m_fMinVelocity = rVar[iSel].fMinVel;

	m_fVelX		   = rVar[iSel].vVel.x * 10.f;
	m_fVelY		   = rVar[iSel].vVel.y * 10.f;
	m_fVelZ		   = rVar[iSel].vVel.z * 10.f;

	m_fGRate	   = rVar[iSel].fGRate;
	m_fScale2	   = rVar[iSel].fScale;

	m_fWind			= rVar[iSel].fWind;
	m_fWDirX		= rVar[iSel].vWindDir.x * 10.f;
	m_fWDirY		= rVar[iSel].vWindDir.y * 10.f;
	m_fWDirZ		= rVar[iSel].vWindDir.z * 10.f;

	// [9/20/2006 madduck]
	// TODO : 이미터의 값으로...
	//m_fPosX		   = rVar[iSel].vEmitterPos.x;
	//m_fPosY		   = rVar[iSel].vEmitterPos.y;
	//m_fPosZ		   = rVar[iSel].vEmitterPos.z;

	if ( m_fMaxVelocity < m_fMinVelocity )
		m_fMaxVelocity = m_fMinVelocity;

	m_ctlMaxVelocity.SetPos((int)(m_fMaxVelocity*RATE));
	m_ctlMinVelocity.SetPos((int)(m_fMinVelocity*RATE));
	m_ctlGravity.SetPos((int)((m_fGRate - m_fGRateMin)*RATE));
	m_ctlScale2.SetPos((int)(m_fScale2*RATE));

// 	m_bRandColor	= rVar[iSel].bRandColor;
// 	m_bRandVel		= rVar[iSel].bRandVel;

	UpdateData(FALSE);
}

void CPage3::OnEnChangeEdits()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	UpdateData(TRUE);

// 	m_ctlMaxCount.SetPos(m_uMaxCount);
// 	m_ctlAmount.SetPos(m_uAmount);
// 	m_ctlLife.SetPos((int)(m_fLife*RATE));
// 	m_ctlAngle.SetPos((int)(m_fAngleD));

	if ( m_fMaxLife < m_fMinLife )
		m_fMaxLife = m_fMinLife;

	m_ctlMaxLife.SetPos((int)(m_fMaxLife*RATE));
	m_ctlMinLife.SetPos((int)(m_fMinLife*RATE));

	if ( m_fMaxVelocity < m_fMinVelocity )
		m_fMaxVelocity = m_fMinVelocity;

	m_ctlMaxVelocity.SetPos((int)(m_fMaxVelocity*RATE));
	m_ctlMinVelocity.SetPos((int)(m_fMinVelocity*RATE));
	m_ctlGravity.SetPos((int)((m_fGRate - m_fGRateMin)*RATE));
// 	m_ctlScale2.SetPos((int)(m_fScale2*RATE));

	UpdateData(FALSE);

	{
		int iSel = m_ctlListTrack.GetCurSel();
		if ( iSel < 0 )
			return;
		SetVariables(g_rView->m_pEmitter->m_vecParticleVar, iSel);
	}

	SendMessageParticleModyfy();
}

void CPage3::OnBnClickedButtonP1()
{
	CColorDialog Dlg( m_ctlColor.GetValue(), CC_FULLOPEN);

	if ( Dlg.DoModal() == IDOK )
	{
		COLORREF color = Dlg.GetColor();
		m_ctlColor.SetHexColor( color, 0x00);
		m_ctlColor.SetValue( color, true);

		//  [8/14/2006 madduck]
		{
			int iSel = m_ctlListTrack.GetCurSel();
			if ( iSel < 0 )
				return;
			SetVariables(g_rView->m_pEmitter->m_vecParticleVar, iSel);
			UpdateGradation();
		}
	}
}

void CPage3::OnBnClickedButtonP2()
{
	CColorDialog Dlg( m_ctlColor.GetValue(), CC_FULLOPEN);

	if ( Dlg.DoModal() == IDOK )
	{
		DWORD dwColor;
		dwColor = Dlg.GetColor() & 0xff;
		m_fAlpha = dwColor/255.0f;

		UpdateData(FALSE);
		//  [8/14/2006 madduck]
		{
			int iSel = m_ctlListTrack.GetCurSel();
			if ( iSel < 0 )
				return;
			SetVariables(g_rView->m_pEmitter->m_vecParticleVar, iSel);
			UpdateGradation();
		}
	}
}

void CPage3::OnBnClickedButtonUpdate()
{
	int iSel = m_ctlListTrack.GetCurSel();
	if ( iSel < 0 )
		return;

	UpdateData(TRUE);

	if ( m_fTime != g_rView->m_pEmitter->m_vecParticleVar[iSel].fEventTime )
	{
		g_rView->m_pEmitter->Stop();
		g_rView->m_pEmitter->DeleteAllParticles();
	}

	SetVariables(g_rView->m_pEmitter->m_vecParticleVar, iSel);
	g_rView->m_pEmitter->RefreshParticleVar();
	GetVariables(g_rView->m_pEmitter->m_vecParticleVar);

	UpdateGradation();

	if ( m_fMaxVelocity < m_fMinVelocity )
		m_fMaxVelocity = m_fMinVelocity;

	m_ctlMaxVelocity.SetPos((int)(m_fMaxVelocity*RATE));
	m_ctlMinVelocity.SetPos((int)(m_fMinVelocity*RATE));
	m_ctlGravity.SetPos((int)((m_fGRate - m_fGRateMin)*RATE));
	m_ctlScale2.SetPos((int)(m_fScale2*RATE));

	m_ctlListTrack.SetCurSel(iSel);
}

void CPage3::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	const float STEP = 1.0f;
	UpdateData(TRUE);
	switch ( pNMHDR->idFrom )
	{
	case IDC_SPIN_VELX:
		m_fVelX += ( -pNMUpDown->iDelta * STEP );
		break;
	case IDC_SPIN_VELY:
		m_fVelY += ( -pNMUpDown->iDelta * STEP );
		break;
	case IDC_SPIN_VELZ:
		m_fVelZ += ( -pNMUpDown->iDelta * STEP );
		break;
	case IDC_SPIN_WDIRX:
		m_fWDirX += ( -pNMUpDown->iDelta * STEP );
		break;
	case IDC_SPIN_WDIRY:
		m_fWDirY += ( -pNMUpDown->iDelta * STEP );
		break;
	case IDC_SPIN_WDIRZ:
		m_fWDirZ += ( -pNMUpDown->iDelta * STEP );
		break;
	}

	UpdateData(FALSE);
	OnEnChangeEdits();
	*pResult = 0;
}

void CPage3::InitUIState()
{
	UpdateData(FALSE);

	if ( !ConfigFromXML())
	{
		_ui_SetSlider(&m_ctlMaxLife, 0, 20*RATE, 0);
		_ui_SetSlider(&m_ctlMinLife, 0, 20*RATE, 0);

		_ui_SetSlider(&m_ctlMaxVelocity, 0, 100*RATE, 0);
		_ui_SetSlider(&m_ctlMinVelocity, 0, 100*RATE, 0);
		_ui_SetSlider(&m_ctlScale2, 0, 100*RATE, 1);

		m_fGRateMin = -10.0f;
		m_fGRateMax = 10.0f;
		int nRange = (int)(m_fGRateMax - m_fGRateMin + 0.5f );
		_ui_SetSlider(&m_ctlGravity, 0, nRange*RATE, 1);
	}

//	OnEnChangeEdits();
}

BOOL CPage3::OnInitDialog()
{
	CPPage::OnInitDialog();

	InitUIState();

	if ( g_rView->m_pEmitter )
	{
		// Emitter의 값으로 다이얼로그를 초기화 한다.
		GetVariables(g_rView->m_pEmitter->m_vecParticleVar);
	}

	m_ctlColor.SetHexColor( 0xffffff, 0x00);
	m_ctlColor.SetValue( 0xffffff, true);

	m_ctlSlideTrack.ClearTics();
	m_ctlSlideTrack.ClearSel();
	m_ctlSlideTrack.SetRangeMin(0);
	m_ctlSlideTrack.SetRangeMax(100);
	m_ctlSlideTrack.SetPos(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CPage3::OnBnClickedButtonAdd()
{
	RParticleVariable ptVar;

	UpdateData(TRUE);

	std::vector<RParticleVariable> &rVar = g_rView->m_pEmitter->m_vecParticleVar;

	for ( UINT ui = 0; ui<rVar.size(); ++ui )
	{
		if ( rVar[ui].fEventTime == m_fTime )
		{
			MessageBox("동일한 시간은 추가할 수 없습니다.","Error");
			return;
		}
	}

	ptVar.fEventTime = m_fTime;
	ptVar.fColorA = m_fAlpha;
	ptVar.fMulScale = m_fScale;
	ptVar.fAddScale = m_fScaleA;

	float a;
	DWORD2RGBA( m_ctlColor.GetValue(), ptVar.fColorB, ptVar.fColorG, ptVar.fColorR, a);

	g_rView->m_pEmitter->Stop();
	g_rView->m_pEmitter->DeleteAllParticles();

	rVar.push_back(ptVar);
	g_rView->m_pEmitter->RefreshParticleVar();

	UpdateData(FALSE);
	GetVariables(g_rView->m_pEmitter->m_vecParticleVar);
}

void CPage3::OnBnClickedButtonRemove()
{
	int iSel = m_ctlListTrack.GetCurSel();
	if ( iSel < 1 || (iSel >= m_ctlListTrack.GetCount() -1))
	{
		MessageBox("처음과 끝은 삭제할 수 없습니다.","에러");
		return;
	}

// 	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
// 	CChildView *pView = (CChildView*)pFrame->GetView();
	std::vector<RParticleVariable> &rVar = g_rView->m_pEmitter->m_vecParticleVar;

	g_rView->m_pEmitter->Stop();
	g_rView->m_pEmitter->DeleteAllParticles();

	rVar.erase( rVar.begin() + iSel);
	ASSERT(g_rView->m_pEmitter->RefreshParticleVar());
	GetVariables(g_rView->m_pEmitter->m_vecParticleVar);
}

void CPage3::OnEnChangeEditColor()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CPPage::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
