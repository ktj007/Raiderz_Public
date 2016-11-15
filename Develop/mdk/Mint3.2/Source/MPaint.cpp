#include "stdafx.h"
#include "MPaint.h"


namespace mint3
{


// MPaint
MPaint::MPaint( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget(szName, pParent, pListener)
{
	m_bStatic = true;
}



// OnDraw
void MPaint::OnDraw( MDrawContext* pDC)
{
	pDC->SetColor( m_Color);
	pDC->FillRectangle( 0, 0, m_Rect.w, m_Rect.h);


	MWidget::m_EventArgs.SetFromEvent( this, NULL);
	m_EventHandler.OnScriptEvent( this, MEVENT_DRAW);
}



} // namespace mint3

