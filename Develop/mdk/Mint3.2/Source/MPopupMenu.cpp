#include "stdafx.h"
#include "MPopupMenu.h"
#include "Mint.h"


namespace mint3
{



// MPopupMenu
MPopupMenu::MPopupMenu( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, (MWidget*)MGetMint()->GetMainFrame(), pListener)
{
	DECLARE_DEFAULT_LOOK( MPopupMenuLook);

	SetEnableFocus( true);

	m_bShow = false;
	m_pOwner = NULL;
	m_bZOrderChangable = true;
}


// TrackPopupMenu
bool MPopupMenu::TrackPopupMenu( MPOINT& ptPoint, MWidget* pOwner)
{
	MWidget::m_PopupEventArgs.Set( ptPoint, pOwner);

	if ( pOwner == NULL)	return false;
	m_pOwner = pOwner;

	MPOINT pt( ptPoint);
	MSIZE r( MGetMint()->GetWorkspaceWidth(), MGetMint()->GetWorkspaceHeight());
	if ( pt.x < 0)								pt.x = 0;
	else if ( (pt.x + m_Rect.w) > (r.w - 10))	pt.x -= m_Rect.w;
	if ( pt.y < 0)								pt.y = 0;
	else if ( (pt.y + m_Rect.h) > (r.h - 10))	pt.y -= m_Rect.h;
	SetPosition( pt);

	pOwner->EventInitPopupMenu( this);

	MGetMint()->SetPopupMenu( this);

	return true;
}


// OnCommand
bool MPopupMenu::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	if ( pWidget == this  &&  ( nMsg == MEVENT_OK || nMsg == MEVENT_CANCEL))
	{
		MGetMint()->ClearPopupMenu();

		if ( m_pOwner != NULL)	m_pOwner->EventCommand( this, MEVENT_CANCEL);
	}

	else if ( pWidget != this  &&  (nMsg == MEVENT_CLICK  ||  nMsg == MEVENT_RCLICK))
	{
		MGetMint()->ClearPopupMenu();

		if ( m_pOwner != NULL)	m_pOwner->EventCommand( this, MEVENT_CLICK);
	}

	return MWidget::OnCommand( pWidget, nMsg, szArgs);
}


// OnRun
void MPopupMenu::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	if ( m_bShow == false)		return;


	// 자식을 포함해서 아무도 포커스를 가지고 있지 않으면 윈도우를 닫음
	MWidget* pWidget = MWidget::m_pFocusedWidget;
	while ( pWidget != NULL)
	{
		if ( pWidget == this)	return;
		pWidget = pWidget->GetParent();
	}

	MGetMint()->ClearPopupMenu();
}


// OnShow
void MPopupMenu::OnShow( bool bShow)
{
	MWidget::OnShow( bShow);

	if ( bShow == false  &&  MGetMint()->GetPopupMenu() == this)
		MGetMint()->ClearPopupMenu();
}


// OnCancel
void MPopupMenu::OnCancel()
{
	MWidget::OnCancel();

	Show( false);
}










// MPopupMenuLook
MPopupMenuLook::MPopupMenuLook()
{
}

void MPopupMenuLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	// to do...
}


} // namespace mint3