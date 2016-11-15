#include "stdafx.h"
#include "MSheet.h"
#include "Mint.h"



namespace mint3
{


// MSheet
MSheet::MSheet( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	m_Rect = MRECT( 0, 0, MGetMint()->GetStandardWidth(), MGetMint()->GetStandardHeight());
	m_bDragAndDrop = false;

	SetAnchor( MAnchorStyle( true, true, true, true));
}


// IsLayered
bool MSheet::IsLayered() const
{
	if ( m_pParent == NULL)			return false;
	if ( m_pParent->IsLayered())	return true;
	return false;
}


// OnEvent
bool MSheet::OnEvent( MEvent* pEvent, MListener* pListener)
{
	return false;
}



} // namespace mint3