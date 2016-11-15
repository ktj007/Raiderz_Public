#include "stdafx.h"
#include "MComboBox.h"
#include "MScrollBar.h"
#include "Mint.h"



namespace mint3
{


// MComboListBox
MComboListBox::MComboListBox( const char* szName , MWidget* pParent, MListener* pListener)
: MListBox( szName, pParent, pListener)
{
	m_bShow = false;
	m_Rect = MRECT( 0, 0, m_Rect.w, m_Rect.h);
	m_bZOrderChangable = true;

	m_pComboBox = (MComboBox*)pListener;
	m_nLastSel = -1;

	EnableMultipleSel( false);
}


// ~MComboListBox
MComboListBox::~MComboListBox()
{
	if ( m_pComboBox != NULL)
		m_pComboBox->m_pListBox = NULL;
}


// GetDrawState
int MComboListBox::GetDrawState()
{
	if ( m_bShow == false  &&  m_tFade.IsArrival() == false)		return true;
	return MListBox::GetDrawState();
}


// OnPreDraw
void MComboListBox::OnPreDraw( MDrawContext* pDC)
{
	if ( m_tFade.IsEnabled() == false)		return;

	float ratio = (float)m_tFade.GetRatio();
	pDC->SetOpacity( pDC->GetOpacity() * ratio);
}


// OnShow
void MComboListBox::OnShow( bool bShow)
{
	MListBox::OnShow( bShow);

	if ( m_bShow == true)	m_nLastSel = GetCurSel();

	m_tFade.SetReverse( !bShow);
}


// OnRun
void MComboListBox::OnRun( DWORD dwCurrTime)
{
	MListBox::OnRun( dwCurrTime);

	MWidget* pFocus = MWidget::m_pFocusedWidget;
	if ( pFocus != this  &&  pFocus != m_pScrollBar)
		m_pComboBox->ShowDropDown( false);
}


// OnSelChanged
void MComboListBox::OnSelChanged()
{
	MListBox::OnSelChanged();

	m_pComboBox->ShowDropDown( false);

	int nCurSel = GetCurSel();
	if ( nCurSel >= 0)
	{
		m_pComboBox->SetText( GetItemText( nCurSel));
		m_pComboBox->OnSelChanged();
	}
	else if ( m_nLastSel >= 0)
		SetCurSel( m_nLastSel);
}













// MComboBox
MComboBox::MComboBox( const char* szName /*=NULL*/, MWidget* pParent /*=NULL*/, MListener* pListener /*=NULL*/)
: MButton( szName, pParent, pListener)
{
	m_bEnableFocus = true;
	m_nDroppedWidth = 50;

	SetType( MBT_PUSHBTN);

	m_pListBox = new MComboListBox( NULL, pParent, this);
}


// ~MComboBox
MComboBox::~MComboBox()
{
	if ( m_pListBox != NULL)
	{
		delete m_pListBox;
		m_pListBox = NULL;
	}
}


// DeleteString
int MComboBox::DeleteString( size_t nIndex)
{
	int _index = m_pListBox->DeleteString( nIndex);
	if ( _index < 0)	SetText( "");
	else				SetText( m_pListBox->GetItemText( m_pListBox->GetCurSel()));

	return _index;
}


// InsertString
int MComboBox::InsertString( size_t nIndex, const char* szString)
{
	int _index = m_pListBox->InsertString( nIndex, szString);
	if ( _index < 0)	SetText( "");
	else				SetText( m_pListBox->GetItemText( m_pListBox->GetCurSel()));

	return _index;
}


// ResetContent
void MComboBox::ResetContent()
{
	m_pListBox->ResetContent();

	SetText( "");
}


// SetCurSel
int MComboBox::SetCurSel( int nIndex)
{
	int nResult = m_pListBox->SetCurSel( nIndex);
	if ( nResult < 0)	SetText( "");
	else				SetText( m_pListBox->GetItemText( m_pListBox->GetCurSel()));
	return nResult;
}


// ShowDropDown
void MComboBox::ShowDropDown( bool bShow /*=true*/)
{
	if ( bShow == true)
	{
		MRECT r( m_Rect.x, m_Rect.y + m_Rect.h + 1, m_Rect.w, m_nDroppedWidth);
		
		MRECT wr = m_pParent->GetClientRect();
		if ( (r.y + r.h + 10) > wr.h)
			r.y = m_Rect.y - m_nDroppedWidth - 1;

		m_pListBox->SetRect( r);
		m_pListBox->BringToTop();
		m_pListBox->Show( true);
		m_pListBox->SetFocus();

		m_bChecked = true;
	}
	else
	{
		m_pListBox->Show( false);

		m_bChecked = false;
	}
}


// OnEvent
bool MComboBox::OnEvent( MEvent* pEvent, MListener* pListener)
{
	if ( MWidget::OnEvent(pEvent, pListener) == true)	return true;

	if ( m_bChecked == true)		return false;

	switch ( pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN :
		ShowDropDown( true);
		return true;
	}

	return false;
}



} // namespace mint3
