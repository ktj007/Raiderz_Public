#include "stdafx.h"
#include "MButton.h"
#include "MButtonGroup.h"
#include "MColorTable.h"
#include "Mint.h"
#include "MWLua.h"
#include "MMainFrame.h"
#include "MEventHandler.h"
#include "MScriptManager.h"



namespace mint3
{


#define DEFAULT_BLANKINGTIME			1000



// MButton
MButton::MButton( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MButtonLook);

	SetEnableFocus( true);

	m_Type = MBT_NORMAL;
	m_bChecked = false;
	m_nHotKey = 0;
	m_pIcon = NULL;
	m_pButtonGroup = NULL;
	m_nIndexInGroup = 0;
	m_bButtonDown = false;
	m_dwUserData = 0;
	m_bBlanking = false;
	m_bEnableEnter = true;
}

MButton::~MButton()
{
	if ( m_pButtonGroup)
	{
		if (m_pButtonGroup->m_pPrevious == this)
			m_pButtonGroup->m_pPrevious = NULL;
	}

	ReleaseButtonGroup();
}


// GetState
const char* MButton::GetState()
{
	switch ( GetType())
	{
	case MBT_NORMAL :
		{
			if ( IsEnabled() == false)		return MWS_DISABLE;
			else if ( IsButtonDown())		return MWS_DOWN;
			else if ( IsMouseOver())		return MWS_FOCUS;
			else
			{
				if ( m_bBlanking == true  &&  (timeGetTime() % DEFAULT_BLANKINGTIME) > (DEFAULT_BLANKINGTIME >> 1))
					return MWS_FOCUS;
				else
					return MWS_NORMAL;
			}
			break;
		}

	case MBT_CHECKBTN :
		{
			if ( IsEnabled() == false)		return MWS_DISABLE;
			else if ( GetCheck())			return MWS_DOWN;
			else if ( IsMouseOver())		return MWS_FOCUS;
			else if ( IsButtonDown())		return MWS_FOCUS;
			else							return MWS_NORMAL;
			break;
		}

	case MBT_PUSHBTN :
		{
			if ( IsEnabled() == false)		return MWS_DISABLE;
			else if ( GetCheck())			return MWS_DOWN;
			else if ( IsButtonDown())		return MWS_DOWN;
			else if ( IsMouseOver())		return MWS_FOCUS;
			else							return MWS_NORMAL;
			break;
		}
	}

	return MWS_NORMAL;
}

int MButton::GetStateNum()
{
	switch ( GetType())
	{
	case MBT_NORMAL :
		{
			if ( GetEnable() == false)		return MWSN_DISABLE;
			else if ( IsButtonDown())		return MWSN_DOWN;
			else if ( IsMouseOver())		return MWSN_FOCUS;
			else
			{
				if ( m_bBlanking == true  &&  (timeGetTime() % DEFAULT_BLANKINGTIME) > (DEFAULT_BLANKINGTIME >> 1))
					return MWSN_FOCUS;
				else
					return MWSN_NORMAL;
			}
			break;
		}

	case MBT_CHECKBTN :
		{
			if ( GetEnable() == false)		return MWSN_DISABLE;
			else if ( GetCheck())			return MWSN_DOWN;
			else if ( IsMouseOver())		return MWSN_FOCUS;
			else if ( IsButtonDown())		return MWSN_FOCUS;
			else							return MWSN_NORMAL;
			break;
		}

	case MBT_PUSHBTN :
		{
			if ( GetEnable() == false)		return MWSN_DISABLE;
			else if ( GetCheck())			return MWSN_DOWN;
			else if ( IsButtonDown())		return MWSN_DOWN;
			else if ( IsMouseOver())		return MWSN_FOCUS;
			else							return MWSN_NORMAL;
			break;
		}
	}

	return MWSN_NORMAL;
}


// SetCheck
void MButton::SetCheck( bool bCheck)
{
	if (m_bChecked == bCheck)	return;

	if (m_pButtonGroup)
	{
		if ( bCheck && m_pButtonGroup->m_pPrevious)
		{
			m_pButtonGroup->m_pPrevious->SetCheck( false);
		}

		m_pButtonGroup->m_pPrevious = this;
	}

	m_bChecked = bCheck;
}


// SetIcon
void MButton::SetIcon( MBitmap* pIcon, MSIZE& size)
{
	m_pIcon = pIcon;
	m_IconSize = size;
}


// SetButtonGroup
void MButton::SetButtonGroup( MButtonGroup *pGroup)
{
	m_pButtonGroup = pGroup;
	m_nIndexInGroup = pGroup->m_nCount;

	pGroup->m_nCount++;

	if ( GetCheck() == true)	pGroup->m_pPrevious = this;
}


// ReleaseButtonGroup
void MButton::ReleaseButtonGroup()
{
	m_pButtonGroup = NULL;
	m_nIndexInGroup = 0;
}


// OnEvent
bool MButton::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent(pEvent, pListener) == true)	return true;

	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN :
		m_bButtonDown = true;
		break;

	case MWM_LBUTTONUP:
		if ( m_bButtonDown == true)
		{
			if ( GetType() == MBT_CHECKBTN  ||  GetType() == MBT_PUSHBTN)
			{
				MRECT r = GetClientRect();
				if ( r.InPoint( pEvent->Pos) == true)
				{
					if ( m_pButtonGroup)		SetCheck( true);
					else						SetCheck( !m_bChecked);

					OnValueChanged();
				}
			}
		}
		m_bButtonDown = false;
		break;

	case MWM_LBUTTONDBLCLK:
		if ( pListener)		pListener->OnCommand( this, MEVENT_DBLCLICK);
		break;

	case MWM_KEYDOWN:
		if ( m_bEnableEnter == true  &&  IsFocus() == true)
		{
			if ( pEvent->nKey == VK_RETURN)
				OnClick( pEvent);
		}
		break;
	}

	return false;
}


// OnQueryHotKey
bool MButton::OnQueryHotKey( unsigned int nHotKey)
{
	return ( (m_nHotKey == nHotKey) ? true : false);
}


// OnHotKey
void MButton::OnHotKey( unsigned int nHotKey)
{
	MWidget::OnHotKey( nHotKey);

	MEvent e;
	OnClick( &e);
}


// OnCommand
bool MButton::OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
{
	return false;
}


// OnSetCursor
bool MButton::OnSetCursor( MEvent* pEvent, string* pCursor)
{
	if ( IsEnabled() == true  &&  m_bStatic == false)
		*pCursor = MCURSOR_HAND;

	return MWidget::OnSetCursor( pEvent, pCursor);
}


// glueSetIcon
void MButton::glueSetIcon( const char* szIcon)
{
	if ( szIcon == NULL  ||  szIcon[ 0] == 0)
	{
		m_pIcon = NULL;
		return;
	}

	MBitmap* pIcon = MBitmapManager::Get( szIcon);
	if ( pIcon != NULL)		SetIcon( pIcon);
}


// glueGetIcon
const char* MButton::glueGetIcon()
{
	if ( m_pIcon == NULL)		return "";
	return m_pIcon->GetName();
}











// MButtonLook
MButtonLook::MButtonLook()
{
	m_ptTextOffset = MPOINT( 0, 1);
	m_bShowHotKey = true;

	SetTextAlign( MAM_HCENTER | MAM_VCENTER);
}

void MButtonLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	MButton* pButton = (MButton*)pWidget;
	MRECT r = GetBkgrndRect( pButton);

	if ( pButton->GetType() == MBT_CHECKBTN)
	{
		r.w = r.h;

		if ( Draw( pDC, r, MWP_BORDER, pButton->GetState()) == false)
		{
			r.x += 3;
			r.y += 3;
			r.w -= 6;
			r.h -= 6;

			pDC->SetColor( MColorTable::FACE[ pButton->GetStateNum()]);
			pDC->FillRectangle( r);

			pDC->SetColor( MColorTable::BORDER[ pButton->GetStateNum()]);
			pDC->Rectangle( r);

			if ( pButton->GetCheck() == true)
			{
				pDC->Line( r.x, r.y, r.x + (r.w >> 1), r.y + r.h);
				pDC->Line( r.x + (r.w >> 1), r.y + r.h, r.x + r.w, r.y);
			}
		}
	}

	else if ( Draw( pDC, r, MWP_BORDER, pButton->GetState()) == false)
	{
		pDC->SetColor( MColorTable::FACE[ pButton->GetStateNum()]);
		pDC->FillRectangle( r);

		pDC->SetColor( MColorTable::BORDER[ pButton->GetStateNum()]);
		pDC->Rectangle( r);
	}
}

void MButtonLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	MButton* pButton = (MButton*)pWidget;
	MRECT r = pButton->GetClientRect();


	// Draw select bar
	if ( pButton->IsMouseOver() == true)	OnDrawSelectbar( pButton, r, pDC);


	// Draw text
	switch ( pButton->GetType())
	{
	case MBT_NORMAL :
		if ( pButton->IsButtonDown())
		{
			r.x += m_ptTextOffset.x;
			r.y += m_ptTextOffset.y;
		}
		break;

	case MBT_CHECKBTN :
		r.x += r.h;
		if ( pButton->IsButtonDown())
		{
			r.x += m_ptTextOffset.x;
			r.y += m_ptTextOffset.y;
		}
		break;

	case MBT_PUSHBTN :
		if ( pButton->GetCheck() == true)
		{
			r.x += m_ptTextOffset.x;
			r.y += m_ptTextOffset.y;
		}
		break;
	}

	OnDrawText( pButton, r, pDC);
}

void MButtonLook::OnDrawText( MButton* pButton, MRECT& r, MDrawContext* pDC)
{
	MRECT tr( r);
	int nAlign = GetTextAlign();
	MFont* pFont = GetFont();

	string strText = pButton->GetText();
	if ( m_bShowHotKey == true)
	{
		int nHotKey = pButton->GetHotKey();
		if ( ( nHotKey >= (int)'a'  &&  nHotKey <= (int)'z')  ||
			( nHotKey >= (int)'A'  &&  nHotKey <= (int)'Z'))
		{
			strText += string( "(");
			strText += nHotKey;
			strText += string( ")");
		}
	}


	// Draw icon
	MBitmap* pIcon = pButton->GetIcon();
	if ( pIcon != NULL)
	{
		MSIZE _size = pButton->GetIconSize();
		MRECT ir( r.x, r.y, _size.w, _size.h);

		if ( (nAlign & MAM_LEFT) == MAM_LEFT)
		{
			tr.x += _size.w;
		}
		else if ( (nAlign & MAM_RIGHT) == MAM_RIGHT)
		{
			ir.x += r.w - pFont->GetWidth( strText.c_str()) - _size.w;
		}
		else if ( (nAlign & MAM_HCENTER) == MAM_HCENTER)
		{
			ir.x += ( r.w - pFont->GetWidth( strText.c_str()) - _size.w) >> 1;
			tr.x += _size.w >> 1;
		}

		if ( (nAlign & MAM_BOTTOM) == MAM_BOTTOM)
		{
			ir.y += r.h - _size.h;
		}
		else if ( (nAlign & MAM_VCENTER) == MAM_VCENTER)
		{
			ir.y += (r.h - _size.h) >> 1;
		}


		pDC->SetBitmap( pIcon);

		if ( pButton->IsEnabled() == false)
		{
			MCOLOR _color = pDC->SetBitmapColor( 100, 100, 100);
			pDC->Draw( ir.x, ir.y, ir.w, ir.h);
			pDC->SetBitmapColor( _color);
		}
		else
		{
			pDC->Draw( ir.x, ir.y, ir.w, ir.h);
		}
	}



	MCOLOR _color;
	if ( pButton->GetType() == MBT_CHECKBTN)
	{
		if ( pButton->IsMouseOver())
			_color = GetFontColor( MWP_BORDER, MWS_FOCUS, MColorTable::TEXT[ MWSN_FOCUS]);
		else
			_color = GetFontColor( MWP_BORDER, pButton->GetState(), MColorTable::TEXT[ pButton->GetStateNum()]);
	}
	else
		_color = GetFontColor( MWP_BORDER, pButton->GetState(), MColorTable::TITLE[ pButton->GetStateNum()]);


	pDC->SetColor( _color);
	pDC->SetFont( pFont);
	pDC->Text( tr, strText.c_str(), GetTextAlign());
}

void MButtonLook::OnDrawSelectbar( MButton* pButton, MRECT& r, MDrawContext* pDC)
{
	Draw( pDC, r, MWP_SELECTBAR, pButton->GetState());
}


} // namespace mint3
