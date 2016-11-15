// AnimationDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AnimationSidebar.h"
#include "resource.h"
#include "EventDlg.h"
#include "AddAnimationDlg.h"
#include "RAnimation.h"
#include "RAnimationResource.h"

// CAnimationSidebar 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAnimationSidebar, CDialog)

CAnimationSidebar::CAnimationSidebar(CWnd* pParent /*=NULL*/)
	: CDialog()
{

}

CAnimationSidebar::~CAnimationSidebar()
{
}

LRESULT CAnimationSidebar::HandleInitDialog(WPARAM wParam, LPARAM lParam)
{
	OnInitDialog();

	return CDialog::HandleInitDialog(wParam, lParam);
}

BOOL CAnimationSidebar::OnInitDialog() 
{
	// TODO: Add extra initialization here
	UpdateData( FALSE);

	return TRUE;  
}

void CAnimationSidebar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CAnimationSidebar)
	DDX_Control(pDX, IDC_ANIMATION_TREE, m_cAnimationTreeCtrl);
	DDX_Control(pDX, IDC_EVENT_LIST, m_cEventListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnimationSidebar, CDialog)
	//{{AFX_MSG_MAP(CAnimationSidebar)
	ON_MESSAGE(WM_INITDIALOG, HandleInitDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_ANIMATION_TREE, &CAnimationSidebar::OnTvnSelchangedAnimationTree)
	ON_NOTIFY(NM_DBLCLK, IDC_ANIMATION_TREE, &CAnimationSidebar::OnNMDblclkAnimationTree)
	ON_BN_CLICKED(IDC_BUTTON_ADDANI, &CAnimationSidebar::OnBnClickedButtonAddani)
	ON_BN_CLICKED(IDC_BUTTON_EDITANI, &CAnimationSidebar::OnBnClickedButtonEditani)
	ON_BN_CLICKED(IDC_BUTTON_DELANI, &CAnimationSidebar::OnBnClickedButtonDelani)
	ON_BN_CLICKED(IDC_BUTTON_ADDEVENT, &CAnimationSidebar::OnBnClickedButtonAddevent)
	ON_BN_CLICKED(IDC_BUTTON_EDITEVENT, &CAnimationSidebar::OnBnClickedButtonEditevent)
	ON_BN_CLICKED(IDC_BUTTON_DELEVENT, &CAnimationSidebar::OnBnClickedButtonDelevent)
	ON_LBN_SELCHANGE(IDC_EVENT_LIST, &CAnimationSidebar::OnLbnSelchangeEventList)
	ON_LBN_DBLCLK(IDC_EVENT_LIST, &CAnimationSidebar::OnLbnDblclkEventList)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CAnimationSidebar::OnTvnSelchangedAnimationTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if(pNMTreeView->itemNew.hItem)
		GetMainView()->SelChangeAnimationList();

	*pResult = 0;
}

void CAnimationSidebar::OnNMDblclkAnimationTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if(pNMTreeView->itemNew.hItem)
		GetMainView()->SelChangeAnimationList();

	*pResult = 0;
}

void CAnimationSidebar::OnBnClickedButtonAddani()
{
	CAddAnimationDlg dlg;

	dlg.SetAniName( "");
	dlg.SetFileName( "");
	dlg.SetLoop( true);
	dlg.SetSpeed( 1.0f);
	dlg.SetEditMode( true);

	if ( dlg.DoModal() == IDCANCEL)
		return;

	CString strAniName = dlg.GetAniName();
	CString strFname = dlg.GetFileName();
	bool bLoop = dlg.GetLoop();
	float fSpeed = dlg.GetSpeed();


	if ( strAniName.IsEmpty() == true  ||  strFname.IsEmpty() == true)
		return;

	RActor* pActor = GetMainView()->GetActor();
	if ( pActor == NULL)
		return;


	pActor->Stop();

	RAnimation* pAni = pActor->m_pMesh->m_AniMgr.Add( strAniName.GetBuffer(), strFname.GetBuffer());
	pAni->GetResource()->BuildAll();
	if(pAni)
	{
		pAni->SetAnimationLoopType( ( bLoop == true)  ? RAniLoopType_Loop : RAniLoopType_HoldLastFrame);
		pAni->SetSpeed( fSpeed);
	}


	GetMainView()->ChangeAnimationListBox();
}

void CAnimationSidebar::OnBnClickedButtonEditani()
{
	RActor* pActor = GetMainView()->GetActor();
	int nAniSel = m_cAnimationTreeCtrl.GetItemData( m_cAnimationTreeCtrl.GetSelectedItem());
	if ( pActor == NULL  ||  nAniSel < 0)
		return;

	RAnimation* pAni = pActor->m_pMesh->m_AniMgr.m_NodeHashList[ nAniSel];
	if ( pAni == NULL)
		return;


	CAddAnimationDlg dlg;

	dlg.SetAniName( pAni->GetAliasName().c_str());
	dlg.SetFileName( pAni->GetResource()->GetName().c_str());
	dlg.SetLoop( (pAni->GetAnimationLoopType() == RAniLoopType_Loop) ? true : false);
	dlg.SetSpeed( pAni->GetSpeed());
	dlg.SetEditMode( false);

	if ( dlg.DoModal() == IDCANCEL)
		return;

	pAni->SetAnimationLoopType( ( dlg.GetLoop() == true)  ? RAniLoopType_Loop : RAniLoopType_HoldLastFrame);
	pAni->SetSpeed( dlg.GetSpeed());

	pActor->SetAnimation( pAni);
}

void CAnimationSidebar::OnBnClickedButtonDelani()
{
	if ( MessageBox( "선택한 애니메이션을 정말로 삭제하시겠습니까?", "애니메이션 삭제 확인", MB_OKCANCEL) == IDCANCEL)
		return;


	RActor* pActor = GetMainView()->GetActor();
	int nAniSel = m_cAnimationTreeCtrl.GetItemData( m_cAnimationTreeCtrl.GetSelectedItem());
	if ( pActor == NULL  ||  nAniSel < 0)
		return;

	RAnimation* pAni = pActor->m_pMesh->m_AniMgr.m_NodeHashList[ nAniSel];
	if ( pAni == NULL)
		return;

	pActor->Stop();
	pActor->m_pMesh->m_AniMgr.Delete( pAni->GetAliasName().c_str());


	GetMainView()->ChangeAnimationListBox();
}

void CAnimationSidebar::OnBnClickedButtonAddevent()
{
	RActor* pActor = GetMainView()->GetActor();
	int nAniSel = m_cAnimationTreeCtrl.GetItemData( m_cAnimationTreeCtrl.GetSelectedItem());
	if ( pActor == NULL  ||  nAniSel < 0)
		return;

	RAnimation* pAni = pActor->m_pMesh->m_AniMgr.m_NodeHashList[ nAniSel];
	if ( pAni == NULL)
		return;

	CEventDlg dlg;
	dlg.SetFrame( GetFrameControlBar()->m_SliderAnimation.GetPos());
	dlg.SetMaxFrame( GetFrameControlBar()->m_SliderAnimation.GetRangeMax());
	dlg.SetName( "");
	dlg.SetParam1( "");
	dlg.SetParam2( "");
	dlg.SetParam3( "");
	dlg.SetUncondition( false);


	if ( dlg.DoModal() == IDCANCEL)
		return;


	if ( strlen( dlg.GetName()) == 0)
		return;

	RAnimationEvent* pEvent = new RAnimationEvent;
	pEvent->m_nFrame = dlg.GetFrame();
	pEvent->m_strEvent = dlg.GetName();
	pEvent->m_strParam1 = dlg.GetParam1();
	pEvent->m_strParam2 = dlg.GetParam2();
	pEvent->m_strParam3 = dlg.GetParam3();
	pEvent->m_bUnconditional = dlg.GetUncondition();

	pAni->m_animationEvents.push_back( pEvent);

	UpdateEventList();
}

void CAnimationSidebar::OnBnClickedButtonEditevent()
{
	RActor* pActor = GetMainView()->GetActor();
	int nAniSel = m_cAnimationTreeCtrl.GetItemData( m_cAnimationTreeCtrl.GetSelectedItem());
	if ( pActor == NULL  ||  nAniSel < 0)
		return;

	RAnimation* pAni = pActor->m_pMesh->m_AniMgr.m_NodeHashList[ nAniSel];
	if ( pAni == NULL)
		return;

	int nEventSel = m_cEventListBox.GetCurSel();
	if ( nEventSel < 0)
		return;

	CEventDlg dlg;
	dlg.SetFrame( pAni->m_animationEvents[ nEventSel]->m_nFrame);
	dlg.SetMaxFrame( GetFrameControlBar()->m_SliderAnimation.GetRangeMax());
	dlg.SetName( pAni->m_animationEvents[ nEventSel]->m_strEvent.c_str());
	dlg.SetParam1( pAni->m_animationEvents[ nEventSel]->m_strParam1.c_str());
	dlg.SetParam2( pAni->m_animationEvents[ nEventSel]->m_strParam2.c_str());
	dlg.SetParam3( pAni->m_animationEvents[ nEventSel]->m_strParam3.c_str());
	dlg.SetUncondition( pAni->m_animationEvents[ nEventSel]->m_bUnconditional);

	if ( dlg.DoModal() == IDOK)
	{
		if ( strlen( dlg.GetName()) == 0)
			return;

		pAni->m_animationEvents[ nEventSel]->m_nFrame = dlg.GetFrame();
		pAni->m_animationEvents[ nEventSel]->m_strEvent = dlg.GetName();
		pAni->m_animationEvents[ nEventSel]->m_strParam1 = dlg.GetParam1();
		pAni->m_animationEvents[ nEventSel]->m_strParam2 = dlg.GetParam2();
		pAni->m_animationEvents[ nEventSel]->m_strParam3 = dlg.GetParam3();
		pAni->m_animationEvents[ nEventSel]->m_bUnconditional = dlg.GetUncondition();

		UpdateEventList();
	}
}

void CAnimationSidebar::OnBnClickedButtonDelevent()
{
	if ( MessageBox( "선택한 이벤트를 정말로 삭제하시겠습니까?", "이벤트 삭제 확인", MB_OKCANCEL) == IDCANCEL)
		return;


	RActor* pActor = GetMainView()->GetActor();
	int nAniSel = m_cAnimationTreeCtrl.GetItemData( m_cAnimationTreeCtrl.GetSelectedItem());
	if ( pActor == NULL  ||  nAniSel < 0)
		return;

	RAnimation* pAni = pActor->m_pMesh->m_AniMgr.m_NodeHashList[ nAniSel];
	if ( pAni == NULL)
		return;

	int nEventSel = m_cEventListBox.GetCurSel();
	if ( nEventSel < 0)
		return;

	pAni->m_animationEvents.erase( pAni->m_animationEvents.begin() + nEventSel);

	UpdateEventList();
}

void CAnimationSidebar::OnLbnSelchangeEventList()
{
	RActor* pActor = GetMainView()->GetActor();
	int nAniSel = m_cAnimationTreeCtrl.GetItemData( m_cAnimationTreeCtrl.GetSelectedItem());
	if ( pActor == NULL  ||  nAniSel < 0)
		return;

	RAnimation* pAni = pActor->m_pMesh->m_AniMgr.m_NodeHashList[ nAniSel];
	if ( pAni == NULL)
		return;

	int nEventSel = m_cEventListBox.GetCurSel();
	if ( nEventSel < 0)
		return;

	GetMainView()->SetFrame( pAni->m_animationEvents[ nEventSel]->m_nFrame);

	GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_ADDEVENT)->EnableWindow( TRUE);
	GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_EDITEVENT)->EnableWindow( TRUE);
	GetAnimationSidebar()->GetDlgItem( IDC_BUTTON_DELEVENT)->EnableWindow( TRUE);
}

void CAnimationSidebar::OnLbnDblclkEventList()
{
	RActor* pActor = GetMainView()->GetActor();
	int nAniSel = m_cAnimationTreeCtrl.GetItemData( m_cAnimationTreeCtrl.GetSelectedItem());
	if ( pActor == NULL  ||  nAniSel < 0)
		return;

	RAnimation* pAni = pActor->m_pMesh->m_AniMgr.m_NodeHashList[ nAniSel];
	if ( pAni == NULL)
		return;

	int nEventSel = m_cEventListBox.GetCurSel();
	if ( nEventSel < 0)
		return;

	CEventDlg dlg;
	dlg.SetFrame( pAni->m_animationEvents[ nEventSel]->m_nFrame);
	dlg.SetName( pAni->m_animationEvents[ nEventSel]->m_strEvent.c_str());
	dlg.SetParam1( pAni->m_animationEvents[ nEventSel]->m_strParam1.c_str());
	dlg.SetParam2( pAni->m_animationEvents[ nEventSel]->m_strParam2.c_str());
	dlg.SetParam3( pAni->m_animationEvents[ nEventSel]->m_strParam3.c_str());
	dlg.SetUncondition( pAni->m_animationEvents[ nEventSel]->m_bUnconditional);

	if ( dlg.DoModal() == IDOK)
	{
		if ( strlen( dlg.GetName()) == 0)
			return;

		pAni->m_animationEvents[ nEventSel]->m_nFrame = dlg.GetFrame();
		pAni->m_animationEvents[ nEventSel]->m_strEvent = dlg.GetName();
		pAni->m_animationEvents[ nEventSel]->m_strParam1 = dlg.GetParam1();
		pAni->m_animationEvents[ nEventSel]->m_strParam2 = dlg.GetParam2();
		pAni->m_animationEvents[ nEventSel]->m_strParam3 = dlg.GetParam3();
		pAni->m_animationEvents[ nEventSel]->m_bUnconditional = dlg.GetUncondition();

		UpdateEventList();
	}
}

void CAnimationSidebar::UpdateEventList()
{
	RActor* pActor = GetMainView()->GetActor();
	int nAniSel = m_cAnimationTreeCtrl.GetItemData( m_cAnimationTreeCtrl.GetSelectedItem());
	if ( pActor == NULL  ||  nAniSel < 0)
		return;

	RAnimation* pAni = pActor->m_pMesh->m_AniMgr.m_NodeHashList[ nAniSel];
	if ( pAni == NULL)
		return;

	m_cEventListBox.ResetContent();
	for ( size_t i = 0;  i < pAni->m_animationEvents.size();  i++)
	{
		CString str;
		str.Format( "(%d) %s - %s",
			pAni->m_animationEvents[ i]->m_nFrame,
			pAni->m_animationEvents[ i]->m_strEvent.c_str(),
			pAni->m_animationEvents[ i]->m_strParam1.c_str());
		m_cEventListBox.AddString( str);
	}
}

void CAnimationSidebar::_resize( int nID, int delta, bool top_anchor)
{
	CWnd* pWnd = GetDlgItem( nID);
	if ( pWnd == NULL)
		return;

	CRect rect;
	pWnd->GetWindowRect( &rect);
	ScreenToClient( rect);

	if ( top_anchor == false)
		rect.top += delta;

	rect.bottom += delta;

	GetDlgItem( nID)->MoveWindow( rect);
}

void CAnimationSidebar::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize( nType, cx, cy);

	if ( 1)
	{
		CWnd* pWnd = GetDlgItem( IDC_GROUP1);
		if ( pWnd == NULL)	return;
		CRect rectGroup1;
		pWnd->GetWindowRect( &rectGroup1);
		ScreenToClient( rectGroup1);

		pWnd = GetDlgItem( IDC_GROUP2);
		if ( pWnd == NULL)	return;
		CRect rectGroup2;
		pWnd->GetWindowRect( &rectGroup2);
		ScreenToClient( rectGroup2);

		int dy =  max( cy, 300) - rectGroup1.top - rectGroup2.bottom;

 		_resize( IDC_GROUP1, dy, true);
 		_resize( IDC_ANIMATION_TREE, dy, true);
 		_resize( IDC_BUTTON_ADDANI, dy, false);
		_resize( IDC_BUTTON_EDITANI, dy, false);
		_resize( IDC_BUTTON_DELANI, dy, false);
 
 		_resize( IDC_GROUP2, dy, false);
 		_resize( IDC_EVENT_LIST, dy, false);
 		_resize( IDC_BUTTON_ADDEVENT, dy, false);
 		_resize( IDC_BUTTON_EDITEVENT, dy, false);
 		_resize( IDC_BUTTON_DELEVENT, dy, false);
	}
}
