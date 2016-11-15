#include "stdafx.h"
#include "MLabelEx.h"
#include "MColorTable.h"


namespace mint3
{



// MLabelEx
MLabelEx::MLabelEx( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	m_bStatic = true;

	m_pFont = NULL;
	m_nColor = MCOLOR( 255, 255, 255);
	m_nAlign = MAM_LEFT | MAM_TOP;
}


// OnChangedLookAndFill
void MLabelEx::OnChangedLookAndFill()
{
	if ( HasLookAndFill() == true)
	{
		m_pFont = m_pLookAndFill->GetFont();
		m_nColor = m_pLookAndFill->GetFontColor( MWP_BORDER, GetState(), MColorTable::TEXT[ GetStateNum()]);
		m_nAlign = m_pLookAndFill->GetTextAlign();
	}

	MWidget::OnChangedLookAndFill();
}


// OnDraw
void MLabelEx::OnDraw( MDrawContext* pDC)
{
	pDC->SetFont( m_pFont ? m_pFont : MFontManager::Get( NULL));
	pDC->SetColor( m_nColor);
	pDC->Text( GetClientRect(), m_strText.c_str(), m_nAlign);
}


// glueSetAlign
void MLabelEx::glueSetAlign( const char* szAlign)
{
	string strAlign( szAlign);

	int _align = 0;
	if ( strAlign.find( "left") != string::npos)			_align |= MAM_LEFT;
	if ( strAlign.find( "top") != string::npos)				_align |= MAM_TOP;
	if ( strAlign.find( "right") != string::npos)			_align |= MAM_RIGHT;
	if ( strAlign.find( "bottom") != string::npos)			_align |= MAM_BOTTOM;
	if ( strAlign.find( "hcenter") != string::npos)			_align |= MAM_HCENTER;
	if ( strAlign.find( "vcenter") != string::npos)			_align |= MAM_VCENTER;
	if ( strAlign.find( "justify") != string::npos)			_align |= MAM_JUSTIFY;

	SetAlign( _align);
}


} // namespace mint3