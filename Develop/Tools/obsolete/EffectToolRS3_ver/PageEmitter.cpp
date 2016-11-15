// Page5.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EffectTool.h"
#include "PageEmitter.h"
#include "RBaseEmitter.h"

extern CRView*		g_rView;

// CPageEmitter 대화 상자입니다.

inline void _ui_SetSlider(CSliderCtrl *pCtrl, int iMin, int iMax, int iPos)
{
	pCtrl->ClearTics();
	pCtrl->ClearSel();

	pCtrl->SetRange(iMin, iMax, TRUE);
	pCtrl->SetPos(iPos);
}

IMPLEMENT_DYNAMIC(CPageEmitter, CPPage)
CPageEmitter::CPageEmitter()
: CPPage(CPageEmitter::IDD)
, m_fTime(0.0f)
, m_uMaxCount(1000)
, m_uAmount(0)
, m_fLife(0)
, m_fAngleD(0)
, m_bLooping(FALSE)
, m_fPosX(0)
, m_fPosY(0)
, m_fPosZ(0)
, m_fDirX(1)
, m_fDirY(1)
, m_fDirZ(1)
, m_bSorting(FALSE)
, m_bRotation(FALSE)
, m_bColorRand(FALSE)
, m_bVelRand(FALSE)
, m_bGather(FALSE)
, m_bExplosion(FALSE)
, m_fMinRadian(-0.01f)
, m_fMaxRadian(0.01f)
{
}

CPageEmitter::~CPageEmitter()
{
}

void CPageEmitter::DoDataExchange(CDataExchange* pDX)
{
	CPPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ctlType);
	DDX_Text(pDX, IDC_EDIT_MAXCOUNT, m_uMaxCount);
	DDX_Text(pDX, IDC_EDIT_AMOUNT, m_uAmount);
	DDX_Text(pDX, IDC_EDIT_EMITLIFE, m_fLife);
	DDX_Text(pDX, IDC_EDIT_ANGLE, m_fAngleD);
	DDX_Check(pDX, IDC_CHECK_LOOPING, m_bLooping);
	DDX_Text(pDX, IDC_EDIT_POSX, m_fPosX);
	DDX_Text(pDX, IDC_EDIT_POSY, m_fPosY);
	DDX_Text(pDX, IDC_EDIT_POSZ, m_fPosZ);

	DDX_Text(pDX, IDC_EDIT_DIRX, m_fDirX);
	DDX_Text(pDX, IDC_EDIT_DIRY, m_fDirY);
	DDX_Text(pDX, IDC_EDIT_DIRZ, m_fDirZ);
	DDX_Check(pDX, IDC_CHECK_SORTING, m_bSorting);

	DDX_Check(pDX, IDC_CHECK_ROTATION, m_bRotation);
	DDX_Control(pDX, IDC_SLIDER_MAXPARTICLE, m_ctlMaxCount);
	DDX_Control(pDX, IDC_SLIDER_AMOUNT, m_ctlAmount);
	DDX_Control(pDX, IDC_SLIDER_EMITLIFE, m_ctlLife);
	DDX_Control(pDX, IDC_SLIDER_ANGLE, m_ctlAngle);
	DDX_Check(pDX, IDC_CHECK_COLOR_RAND, m_bColorRand);
	DDX_Check(pDX, IDC_CHECK_VEL_RAND, m_bVelRand);

	DDX_Text(pDX, IDC_EDIT_EMITTER_TIME, m_fTime);
	DDX_Control(pDX, IDC_SLIDER_EMITTER_TIME, m_ctlSlideTrack);
	DDX_Control(pDX, IDC_LIST_EMITTER_TRACK, m_ctlListTrack);

	DDX_Check(pDX, IDC_CHECK_GATHER, m_bGather);
	DDX_Check(pDX, IDC_CHECK_EXPLOSION, m_bExplosion);
	DDX_Text(pDX, IDC_EDIT_MINRADIAN, m_fMinRadian);
	DDX_Text(pDX, IDC_EDIT_MAXRADIAN, m_fMaxRadian);
	DDX_Control(pDX, IDC_SLIDER_MINRADIAN, m_ctlMinRadian);
	DDX_Control(pDX, IDC_SLIDER_MAXRADIAN, m_ctlMaxRadian);
	
	DDX_Control(pDX, IDC_RADIO_BILLBOARD_XYZ, m_ctlRadioXYZ);
	DDX_Control(pDX, IDC_RADIO_BILLBOARD_XY,  m_ctlRadioXY);
	DDX_Control(pDX, IDC_RADIO_PLANE_XY, m_ctlRadioPlaneXY);
	DDX_Control(pDX, IDC_RADIO_SPREAD, m_ctlRadioSpread);
}


BEGIN_MESSAGE_MAP(CPageEmitter, CPPage)
	ON_EN_CHANGE(IDC_EFFECT_NAME, OnEnChangeEdites)
	ON_EN_CHANGE(IDC_EDIT_MAXCOUNT, OnEnChangeEdites)
	ON_EN_CHANGE(IDC_EDIT_AMOUNT, OnEnChangeEdites)
	ON_EN_CHANGE(IDC_EDIT_EMITLIFE, OnEnChangeEdites)
	ON_EN_CHANGE(IDC_EDIT_DIRX, OnEnChangeEdites)
	ON_EN_CHANGE(IDC_EDIT_DIRY, OnEnChangeEdites)
	ON_EN_CHANGE(IDC_EDIT_DIRZ, OnEnChangeEdites)
	ON_EN_CHANGE(IDC_EDIT_POSX, OnEnChangeEdites)
	ON_EN_CHANGE(IDC_EDIT_POSY, OnEnChangeEdites)
	ON_EN_CHANGE(IDC_EDIT_POSZ, OnEnChangeEdites)
	ON_EN_CHANGE(IDC_EDIT_ANGLE, OnEnChangeEdites)
	ON_EN_CHANGE(IDC_EDIT_MINLIFE, OnEnChangeEdites)
	ON_EN_CHANGE(IDC_EDIT_MAXLIFE, OnEnChangeEdites)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DIRX, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DIRY, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DIRZ, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POSX, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POSY, OnDeltaposSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POSZ, OnDeltaposSpin)
	ON_WM_HSCROLL()

	ON_EN_CHANGE(IDC_EDIT_MINRADIAN, OnEnChangeEdites)
	ON_EN_CHANGE(IDC_EDIT_MAXRADIAN, OnEnChangeEdites)

	//ON_LBN_SELCHANGE(IDC_LIST_EMITTER2, OnLbnSelchangeListEmitter2)

	ON_BN_CLICKED(IDC_CHECK_LOOPING, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_SORTING, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_WORLDAXIS, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_RANDROT, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_COLOR_RAND, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_VEL_RAND, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_GATHER, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_EXPLOSION, OnBnClickedCheck)

	ON_LBN_SELCHANGE(IDC_LIST_EMITTER_TRACK, OnLbnSelchangeListEmitterTrack)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CPageEmitter::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &CPageEmitter::OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CPageEmitter::OnBnClickedButtonUpdate)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CPageEmitter::OnCbnSelchangeComboType)
	
	ON_BN_CLICKED(IDC_RADIO_BILLBOARD_XYZ, &CPageEmitter::OnBnClickedRadioBillboard)
	ON_BN_CLICKED(IDC_RADIO_BILLBOARD_XY, &CPageEmitter::OnBnClickedRadioBillboard)
	ON_BN_CLICKED(IDC_RADIO_PLANE_XY, &CPageEmitter::OnBnClickedRadioBillboard)
	ON_BN_CLICKED(IDC_RADIO_SPREAD, &CPageEmitter::OnBnClickedRadioBillboard)
END_MESSAGE_MAP()


// CPageEmitter메시지 처리기입니다.

BOOL CPageEmitter::OnInitDialog()
{
	CPPage::OnInitDialog();

	m_ctlRadioXYZ.SetCheck(TRUE);
	m_ctlRadioXY.SetCheck(FALSE);
	m_ctlRadioPlaneXY.SetCheck(FALSE);

	InitUIState();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

bool CPageEmitter::ConfigFromXML()
{
	MXml kXml;
	if ( !kXml.LoadFile(FILENAME_CONFIG))
		return false;

	MXmlElement *pElement = kXml.Doc()->FirstChildElement("texturepath");
	if ( !pElement )
		return false;

	//int nValue;

	//_Contents(&nValue,kXml.Doc()->FirstChildElement("life"));
	//_ui_SetSlider(&m_ctlMaxLife, 0, nValue*RATE, 0);
	//_ui_SetSlider(&m_ctlMinLife, 0, nValue*RATE, 0);

	return true;
}

void CPageEmitter::InitUIState()
{
	UpdateData(FALSE);

	_ui_SetSlider(&m_ctlMaxRadian, -500, 500, (int)m_fMaxRadian);
	_ui_SetSlider(&m_ctlMinRadian, -500, 500, (int)m_fMinRadian);

	_ui_SetSlider(&m_ctlMaxCount, 0, 1000, m_uMaxCount);
	_ui_SetSlider(&m_ctlAmount, 0, 1000, m_uAmount);
	_ui_SetSlider(&m_ctlLife, 0, 15*RATE, m_fLife);
	_ui_SetSlider(&m_ctlAngle, 0, 360, m_fAngleD);

// 	m_ctlMaxRadian.SetPos((m_fMaxRadian*100.f));
// 	m_ctlMinRadian.SetPos((m_fMinRadian*100.f));

	// 생성자의 Init 이후 실행되므로, 값에 주의 !!!
	m_ctlType.Clear();
	for ( int i = 0; emitterTypes[i].pcName ; ++i )
		m_ctlType.InsertString( i, emitterTypes[i].pcName );

	m_ctlType.SetCurSel(0);

	m_ctlSlideTrack.ClearTics();
	m_ctlSlideTrack.ClearSel();
	m_ctlSlideTrack.SetRangeMin(0);
	m_ctlSlideTrack.SetRangeMax(100);
	m_ctlSlideTrack.SetPos(0);

	//OnEnChangeEdites();
}

void CPageEmitter::OnEnChangeEdites()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	if( m_uMaxCount > 1000) m_uMaxCount = 1000;

	m_ctlMaxCount.SetPos(m_uMaxCount);
	m_ctlAmount.SetPos(m_uAmount);
	m_ctlLife.SetPos((int)(m_fLife*RATE));
	m_ctlAngle.SetPos((int)(m_fAngleD));

	//if ( m_fMaxLife < m_fMinLife )
	//	m_fMaxLife = m_fMinLife;

	m_ctlMaxRadian.SetPos((m_fMaxRadian*100.f));
	m_ctlMinRadian.SetPos((m_fMinRadian*100.f));

	UpdateData(FALSE);

	{
		int iSel = m_ctlListTrack.GetCurSel();
		if ( iSel >= 0 )
			SetVariables(g_rView->m_pEmitter->m_vecEmitterVar, iSel);
	}

	SendMessageEmitterModify();
}

void CPageEmitter::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	const float STEP = 0.1f;
	UpdateData(TRUE);
	switch ( pNMHDR->idFrom )
	{
	case IDC_SPIN_POSX:
		m_fPosX += ( -pNMUpDown->iDelta * STEP );
		break;
	case IDC_SPIN_POSY:
		m_fPosY += ( -pNMUpDown->iDelta * STEP );
		break;
	case IDC_SPIN_POSZ:
		m_fPosZ += ( -pNMUpDown->iDelta * STEP );
		break;
	case IDC_SPIN_DIRX:
		m_fDirX += ( -pNMUpDown->iDelta * STEP );
		break;
	case IDC_SPIN_DIRY:
		m_fDirY += ( -pNMUpDown->iDelta * STEP );
		break;
	case IDC_SPIN_DIRZ:
		m_fDirZ += ( -pNMUpDown->iDelta * STEP );
		break;
	}

	if ( m_fDirX < 0.0f )	m_fDirX = 0.0f;
	if ( m_fDirY < 0.0f )	m_fDirY = 0.0f;
	if ( m_fDirZ < 0.0f )	m_fDirZ = 0.0f;

	UpdateData(FALSE);
	OnEnChangeEdites();
	*pResult = 0;
}

void CPageEmitter::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_uMaxCount = (UINT)m_ctlMaxCount.GetPos();
	m_uAmount = (UINT)m_ctlAmount.GetPos();

	float fPos = m_ctlSlideTrack.GetPos()/100.0f;
	m_fTime = fPos;


	m_fAngleD = (float)(m_ctlAngle.GetPos());
	//m_fMaxLife = (float)( m_ctlMaxLife.GetPos()/(float)RATE);
	//m_fMinLife = (float)( m_ctlMinLife.GetPos()/(float)RATE);
	m_fLife = (float(m_ctlLife.GetPos()/(float)RATE));

	if ( m_ctlMaxRadian.GetPos() < m_ctlMinRadian.GetPos())
		m_ctlMinRadian.SetPos( m_ctlMaxRadian.GetPos());
	m_fMinRadian = (float)( m_ctlMinRadian.GetPos()/(float)100);
	m_fMaxRadian = (float)( m_ctlMaxRadian.GetPos()/(float)100);

	UpdateData(FALSE);

	{
		int iSel = m_ctlListTrack.GetCurSel();
		if ( (iSel >= 0) && (g_rView->m_pEmitter != NULL) )
			SetVariables(g_rView->m_pEmitter->m_vecEmitterVar, iSel);
	}

	SendMessageEmitterModify();

	CPPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPageEmitter::OnBnClickedCheck()
{
	UpdateData(TRUE);

	SendMessageEmitterModify();
}

void CPageEmitter::OnLbnSelchangeListEmitterTrack()
{
	int iSel = m_ctlListTrack.GetCurSel();
	if ( iSel < 0 )
		return;

	if( g_rView->m_pEmitter == NULL)	return;

	std::vector<REmitterVariable> &rVar = g_rView->m_pEmitter->m_vecEmitterVar;

	m_fTime		= rVar[iSel].fEventTime;
	m_uAmount	= rVar[iSel].uAmount;
	m_ctlAmount.SetPos(m_uAmount);

	RVector vRange = rVar[iSel].vRange;
	m_fDirX		= vRange.x;	m_fDirY		= vRange.y;	m_fDirZ		= vRange.z;
	RVector vPos= rVar[iSel].vPos;
	m_fPosX		= vPos.x;	m_fPosY		= vPos.y;	m_fPosZ		= vPos.z;

	int iPos = (int)(m_fTime * 100.0f);
	if ( iPos < 0 )		iPos = 0;
	if ( iPos > 100 )	iPos = 100;
	m_ctlSlideTrack.SetPos(iPos);

	m_fAngleD = rVar[iSel].fAngle;
	m_ctlAngle.SetPos(m_fAngleD);

	UpdateData(FALSE);

	// 바로 갱신이 되도록. 흠 꼬일수도. [10/16/2006 madduck]
	SendMessageEmitterModify();
}

void CPageEmitter::OnBnClickedButtonAdd()
{
	if(g_rView->m_pEmitter == NULL)
	{
		MessageBox("이미터를 선택해 주세요.","Error");
		return;
	}

	REmitterVariable ptVar;

	UpdateData(TRUE);

	vector<REmitterVariable> &rVar = g_rView->m_pEmitter->m_vecEmitterVar;

	for ( UINT ui = 0; ui<rVar.size(); ++ui )
	{
		if ( rVar[ui].fEventTime == m_fTime )
		{
			MessageBox("동일한 시간은 추가할 수 없습니다.","Error");
			return;
		}
	}

	ptVar.fEventTime= m_fTime;
	ptVar.uAmount	= m_uAmount;
	ptVar.vRange	= RVector(m_fDirX, m_fDirY, m_fDirZ);
	ptVar.vPos		= RVector(m_fPosX, m_fPosY, m_fPosZ);
	ptVar.fAngle	= m_fAngleD;

	g_rView->m_pEmitter->Stop();
	g_rView->m_pEmitter->DeleteAllParticles();

	rVar.push_back(ptVar);
	g_rView->m_pEmitter->RefreshEmitterVar();

	UpdateData(FALSE);
	GetVariables(g_rView->m_pEmitter->m_vecEmitterVar);
}

void CPageEmitter::OnBnClickedButtonRemove()
{
	int iSel = m_ctlListTrack.GetCurSel();
	if ( iSel < 1 || (iSel >= m_ctlListTrack.GetCount() -1))
	{
		MessageBox("처음과 끝은 삭제할 수 없습니다.","에러");
		return;
	}

	std::vector<REmitterVariable> &rVar = g_rView->m_pEmitter->m_vecEmitterVar;

	g_rView->m_pEmitter->Stop();
	g_rView->m_pEmitter->DeleteAllParticles();

 	rVar.erase( rVar.begin() + iSel);
 	ASSERT(g_rView->m_pEmitter->RefreshEmitterVar());
 	GetVariables(g_rView->m_pEmitter->m_vecEmitterVar);
}

void CPageEmitter::OnBnClickedButtonUpdate()
{
	int iSel = m_ctlListTrack.GetCurSel();
	if ( iSel < 0 )
		return;

	UpdateData(TRUE);

	//CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	//CChildView *pView = g (CChildView*)pFrame->GetView();

	if ( m_fTime != g_rView->m_pEmitter->m_vecEmitterVar[iSel].fEventTime )
	{
		g_rView->m_pEmitter->Stop();
		g_rView->m_pEmitter->DeleteAllParticles();
	}

	SetVariables(g_rView->m_pEmitter->m_vecEmitterVar, iSel);
	g_rView->m_pEmitter->RefreshParticleVar();
	GetVariables(g_rView->m_pEmitter->m_vecEmitterVar);

	//if ( m_fMaxVelocity < m_fMinVelocity )
	//	m_fMaxVelocity = m_fMinVelocity;

	//m_ctlMaxVelocity.SetPos((int)(m_fMaxVelocity*RATE));
	//m_ctlMinVelocity.SetPos((int)(m_fMinVelocity*RATE));
	//m_ctlGravity.SetPos((int)((m_fGRate - m_fGRateMin)*RATE));
	//m_ctlScale2.SetPos((int)(m_fScale2*RATE));

	m_ctlListTrack.SetCurSel(iSel);
}

void CPageEmitter::GetVariables(const std::vector<REmitterVariable> &rVar)
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

void CPageEmitter::SetVariables(std::vector<REmitterVariable> &rVar, int iSel, bool btime)
{
	UpdateData(TRUE);

	if ( m_fTime < 0.0f )	m_fTime = 0.0f;
	if ( m_fTime > 1.0f )	m_fTime = 1.0f;

	if(btime)
		rVar[iSel].fEventTime = m_fTime;

	rVar[iSel].uAmount	= m_uAmount;
	rVar[iSel].vRange	= RVector(m_fDirX, m_fDirY, m_fDirZ);
	rVar[iSel].vPos		= RVector(m_fPosX, m_fPosY, m_fPosZ);
	rVar[iSel].fAngle	= m_fAngleD;

	//float a;
	//DWORD2RGBA( m_ctlColor.GetValue(), rVar[iSel].fColorB, rVar[iSel].fColorG, rVar[iSel].fColorR, a);

	////  [7/20/2006 madduck]
	//rVar[iSel].fMaxVel	= m_fMaxVelocity;
	//rVar[iSel].fMinVel	= m_fMinVelocity;

	//rVar[iSel].vVel = RVector(m_fVelX, m_fVelY, m_fVelZ);

	//rVar[iSel].fGRate	= m_fGRate;
	//rVar[iSel].fScale	= m_fScale2;

	// 	rVar[iSel].bRandColor = m_bRandColor	? true : false;
	// 	rVar[iSel].bRandVel	  = m_bRandVel		? true : false;

	UpdateData(FALSE);
}
void CPageEmitter::OnCbnSelchangeComboType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 중간에 type를 바꿀필요가 있을까...
}

void CPageEmitter::OnBnClickedRadioBillboard()
{
	SendMessageEmitterModify();
}
