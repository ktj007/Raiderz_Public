#include "stdafx.h"
#include "MGroup.h"
#include "MColorTable.h"


namespace mint3
{




// MGroup
MGroup::MGroup( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	DECLARE_DEFAULT_LOOK( MGroupLook);

	m_bStatic = true;
}









// MGroupLook
MGroupLook::MGroupLook()
{
}

void MGroupLook::OnDrawBorder( MWidget* pWidget, MDrawContext* pDC)
{
	MLookBase::OnDrawBorder( pWidget, pDC);

	
	string strText = pWidget->GetText();
	if ( strText.empty() == false)
	{
		MRECT _r = GetBkgrndRect( pWidget);
		int _h = GetFont()->GetHeight();

		OnDrawText( (MGroup*)pWidget, MRECT( _r.x, _r.y - _h - 5, _r.w, _r.h + (_h << 1) + 5), pDC);
	}
}

void MGroupLook::OnDrawText( MGroup* pGroup, MRECT& r, MDrawContext* pDC)
{
	pDC->SetFont( GetFont());
	pDC->SetColor( GetFontColor( MWP_BORDER, pGroup->GetState(), MColorTable::TEXT[ pGroup->GetStateNum()]));
	pDC->Text( r, pGroup->GetText(), GetTextAlign());
}


} // namespace mint3