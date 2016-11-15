// VL_StatusBar.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "VL_StatusBar.h"
#include "C_ControlMediatorHelperAction.h"
#include "M_LogicObject.h"
#include "M_LogicObjectCommandBuffer.h"
#include "M_ToolCmdMgr.h"

// CCoreToolStatusBar 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCoreToolStatusBar, CMFCStatusBar)
CCoreToolStatusBar::CCoreToolStatusBar(CWnd* pParent /*=NULL*/)
// : m_strPaneText(_T("Ready"))
{
}

CCoreToolStatusBar::~CCoreToolStatusBar()
{
}

static UINT indicators[] =
{
	ID_SEPARATOR,
	IDS_STATUSBAR_POS_TEXT,
	IDS_STATUSBAR_X,
	IDS_STATUSBAR_Y,
	IDS_STATUSBAR_Z,
	IDS_STATUSBAR_SCALE_TEXT,
	IDS_STATUSBAR_SCALE
};

// edit 콘트롤이 들어갈 곳의 index
const int editIndicators[] = { 2,3,4,6 };

BOOL CCoreToolStatusBar::Create(CWnd* pWnd)
{
	if (!CMFCStatusBar::Create(pWnd) ||
		!SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	SetPaneStyle (0, SBPS_STRETCH );
	CMFCStatusBar::EnablePaneDoubleClick ();

	const int EDIT_SIZE = 70;
	for( int i = 0; i<_countof(editIndicators); i++ )
	{
		SetPaneWidth (editIndicators[i], EDIT_SIZE);
		
		CRect rt(0,0,0,0);
		m_editControls[i].Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP, rt, this, 0);
		m_editControls[i].SetFont( GetFont() );
	}

	return TRUE;
}

BOOL CCoreToolStatusBar::OnInitStatusBar()
{
	// register enter callback 
	CCoreToolEdit::EDIT_ENTER_HANDLER posHandler(this, &CCoreToolStatusBar::OnEnterPosEditCtrl );
	m_editControls[0].SetCallback( posHandler );
	m_editControls[1].SetCallback( posHandler );
	m_editControls[2].SetCallback( posHandler );

	CCoreToolEdit::EDIT_ENTER_HANDLER scaleHandler(this, &CCoreToolStatusBar::OnEnterScaleEditCtrl );
	m_editControls[3].SetCallback( scaleHandler );

	return TRUE;
}

void CCoreToolStatusBar::OnUpdateCmdUI(CCmdUI* pCmdUI)
{
}

BEGIN_MESSAGE_MAP(CCoreToolStatusBar, CMFCStatusBar)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CCoreToolStatusBar::Listen(CoreMessage& _message)
{
	switch( _message.nID)
	{
	case VIEW_MESSAGE::DEVICE_CREATED:
		OnInitStatusBar();
		break;
	case VIEW_MESSAGE::SELECTION_CHANGED:
		OnSelectionChanged( );
		break;
	}
}

void CCoreToolStatusBar::SetPaneText(LPCTSTR lpszNewText)
{
	__super::SetPaneText( 0, lpszNewText );
}

void CCoreToolStatusBar::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	for( int i = 0; i<_countof(editIndicators); i++ )
	{
		if( m_editControls[i].GetSafeHwnd() )
		{
			CRect rt;
			GetItemRect( editIndicators[i], rt );
			m_editControls[i].MoveWindow( &rt );
		}
	}
}

void CCoreToolStatusBar::OnSelectionChanged()
{
	rs3::RVector* pCenter = m_pControlMediator_->GetHelperAction()->GetSelectNodeCenter();
	if(pCenter)
	{
		CString strBuffer;
		strBuffer.Format( "%3.2f", pCenter->x );
		m_editControls[0].SetWindowText( strBuffer );
		strBuffer.Format( "%3.2f", pCenter->y );
		m_editControls[1].SetWindowText( strBuffer );
		strBuffer.Format( "%3.2f", pCenter->z );
		m_editControls[2].SetWindowText( strBuffer );
	}else
	{
		m_editControls[0].SetWindowText( "" );
		m_editControls[1].SetWindowText( "" );
		m_editControls[2].SetWindowText( "" );
	}

	LOGIC_OBJECTLIST *pSelectedLogicList = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject();
	if( pSelectedLogicList->size() == 1 )
	{
		CLogicObject* pLogicObject = pSelectedLogicList->front();

		float fScale;
		pLogicObject->GetScale( fScale );

		// 스케일 활성화
		CString strBuffer;
		strBuffer.Format( "%3.2f", fScale );
		m_editControls[3].SetWindowText( strBuffer );
	}else
		m_editControls[3].SetWindowText( "" );
}

void CCoreToolStatusBar::OnEnterPosEditCtrl()
{
	rs3::RVector editPosition;

	CString strBuffer;
	m_editControls[0].GetWindowText(strBuffer);
	editPosition.x = atof( strBuffer );
	m_editControls[1].GetWindowText(strBuffer);
	editPosition.y = atof( strBuffer );
	m_editControls[2].GetWindowText(strBuffer);
	editPosition.z = atof( strBuffer );

	LOGIC_OBJECTLIST *pSelectedLogicList = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject();

	if( pSelectedLogicList->empty() ) return;

	rs3::RVector* pCenter = m_pControlMediator_->GetHelperAction()->GetSelectNodeCenter();
	_ASSERT( pCenter );
	if(pCenter)
	{
		CToolCmdMgr* pCommandManager = m_pControlMediator_->GetToolCmdMgr();
		CGroupedCommandBuffer* pGroupCmdBuffer = new CGroupedCommandBuffer;

		rs3::RVector deltaPosition = editPosition - *pCenter;

		// 변경된 만큼 모든 오브젝트 이동
		for(LOGIC_OBJECTLIST::iterator it_logic = pSelectedLogicList->begin(); it_logic != pSelectedLogicList->end(); ++it_logic)
		{
			CLogicObject* pLogicObject = *it_logic;

			if( pLogicObject->GetEditable() == false ) continue;

			rs3::RVector newPosition;
			pLogicObject->GetPosition( newPosition );

			CLogicMoveCmdBuffer* pCmdBuffer = new CLogicMoveCmdBuffer(pLogicObject, newPosition);
			pGroupCmdBuffer->AddBuffer(pCmdBuffer);

			pLogicObject->SetPosition( newPosition + deltaPosition );

			m_pControlMediator_->BroadCastMessage( CoreMessage( VIEW_MESSAGE::LOGIC_MOVE, pLogicObject ) );
		}

		if(pGroupCmdBuffer->IsEmpty())
			delete pGroupCmdBuffer;
		else
			pCommandManager->RecordCommand(pGroupCmdBuffer);
	}
}

void CCoreToolStatusBar::OnEnterScaleEditCtrl()
{
	float fEditScale;

	CString strBuffer;
	m_editControls[3].GetWindowText(strBuffer);
	fEditScale = atof( strBuffer );

	LOGIC_OBJECTLIST *pSelectedLogicList = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject();
	if( pSelectedLogicList->size() == 1 )
	{
		CLogicObject* pLogicObject = pSelectedLogicList->front();

		float fScale;
		pLogicObject->GetScale( fScale );

		CToolCmdMgr* pCommandManager = m_pControlMediator_->GetToolCmdMgr();
		CLogicScaleCmdBuffer* pCmdBuffer = new CLogicScaleCmdBuffer(pLogicObject, fScale);


		pLogicObject->SetScale( fEditScale );

		m_pControlMediator_->BroadCastMessage( CoreMessage( VIEW_MESSAGE::LOGIC_MOVE, pLogicObject ) );

		pCommandManager->RecordCommand(pCmdBuffer);
	}
}