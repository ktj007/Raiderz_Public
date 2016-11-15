
#include "stdafx.h"
#include "XUIControlKey.h"
#include "XController.h"
#include "XGameInterface.h"


XUIControlKey::XUIControlKey( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	SetEnableFocus( true);
}

XUIControlKey::~XUIControlKey()
{
}


void XUIControlKey::OnDraw( MDrawContext* pDC)
{
	XVirtualKey nVKey = gg.controller->GetVirtualKey( m_strText.c_str());
	if ( nVKey <= VKEY_NA  ||  nVKey >= VKEY_COUNT)
		return;
	
	MRECT r = GetClientRect();
	string text = gg.controller->GetDisplayString( nVKey);
	
	if ( IsFocus() == true)
	{
		pDC->SetColor( 128, 0, 0, 64.0 * sin( MGetMint()->GetUpdateTime() * 0.008) + 64.0);
		pDC->FillRectangle( r);


		if ( HasLookAndFill() == true)		pDC->SetColor( m_pLookAndFill->GetFontColor( MWP_BORDER, MWS_FOCUS, MCOLOR( 230, 230, 230)) );
		else								pDC->SetColor( 230, 230, 230);
	}
	else
	{
		if ( HasLookAndFill() == true)		pDC->SetColor( m_pLookAndFill->GetFontColor( MWP_BORDER, MWS_NORMAL, MCOLOR( 160, 160, 160)) );
		else								pDC->SetColor( 160, 160, 160);
	}

	pDC->SetFont( HasLookAndFill()  ?  GetLookAndFill()->GetFont() : MFontManager::Get( NULL));
	pDC->Text( GetClientRect(), text.c_str(), MAM_HCENTER | MAM_VCENTER);
}


bool XUIControlKey::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent( pEvent, pListener) == true)
		return true;


	if ( IsFocus() == false)
		return false;



	MRECT r = GetInitialClientRect();


	switch ( pEvent->nMessage)
	{
	case MWM_KEYDOWN :
		XGetGameInterface().OPTIONS.ChangeControlKey( m_strText.c_str(), pEvent->nKey, pEvent->bShift, pEvent->bCtrl, pEvent->bAlt);
		OnValueChanged();
		return true;

	case MWM_LBUTTONDBLCLK :
		{
			XGetGameInterface().OPTIONS.ChangeControlKey( m_strText.c_str(), KEY_LBUTTON);
			OnValueChanged();
			return true;
		}

	case MWM_MBUTTONDBLCLK :
		{
			XGetGameInterface().OPTIONS.ChangeControlKey( m_strText.c_str(), KEY_MBUTTON);
			OnValueChanged();
			return true;
		}

	case MWM_RBUTTONDBLCLK :
		{
			XGetGameInterface().OPTIONS.ChangeControlKey( m_strText.c_str(), KEY_RBUTTON);
			OnValueChanged();
			return true;
		}
	}


	return false;
}


bool XUIControlKey::OnSetCursor( MEvent* pEvent, string* pCursor)
{
	if ( IsEnabled() == true  &&  m_bStatic == false)
		*pCursor = MCURSOR_HAND;

	return MWidget::OnSetCursor( pEvent, pCursor);
}