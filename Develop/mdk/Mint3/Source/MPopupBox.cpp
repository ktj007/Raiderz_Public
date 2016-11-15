

#include "stdafx.h"
#include "MPopupBox.h"


// namespace : mint3
namespace mint3
{



// MPopupBoxLook
MPopupBoxLook::MPopupBoxLook()
{
	m_ptOffset.x = m_ptOffset.y = 0;
}


// OnDraw
void MPopupBoxLook::OnDraw( MWidget* pWidget, MDrawContext* pDC)
{
	MRECT client = pWidget->GetClientRect();

	pDC->SetColor( 255, 255, 255);
	pDC->Rectangle( client);
}


// GetClientRect
MRECT MPopupBoxLook::GetClientRect( MWidget* pWidget, MRECT& client)
{
	return client;
}





} // namespace mint3

