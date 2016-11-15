#include "stdafx.h"
#include "MLabel.h"
#include "MColorTable.h"


namespace mint3
{



// MLabel
MLabel::MLabel( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MLabelLook);

	m_bStatic = true;
}







// MLabelLook
MLabelLook::MLabelLook()
{
	SetTextAlign( MAM_LEFT | MAM_TOP);
}

void MLabelLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	MRECT r = GetBkgrndRect( pWidget);
	Draw( pDC, r, MWP_BORDER, pWidget->GetState());
}

void MLabelLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	OnDrawText( (MLabel*)pWidget, pWidget->GetClientRect(), pDC);
}

void MLabelLook::OnDrawText( MLabel* pLabel, MRECT& r, MDrawContext* pDC)
{
	pDC->SetFont( GetFont());
	pDC->SetColor( GetFontColor( MWP_BORDER, pLabel->GetState(), MColorTable::TEXT[ pLabel->GetStateNum()]));
	pDC->Text( r, pLabel->GetText(), GetTextAlign());
}


} // namespace mint3