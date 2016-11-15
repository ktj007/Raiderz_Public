#include "stdafx.h"
#include "MPanel.h"
#include "MColorTable.h"

namespace mint3 {

IMPLEMENT_LOOK(MPanel, MPanelLook)

MPanel::MPanel(const char* szName, MWidget* pParent, MListener* pListener) : MWidget(szName, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR()

	m_BorderColor = MColorTable::MPANEL_BORDER;
	m_BackgroundColor = MColorTable::MPANEL_PLANE;
	m_nBorderStyle = MBS_SINGLE;
}


void MPanel::SetBorderColor(MCOLOR color)
{	
	m_BorderColor = color;
}


MCOLOR MPanel::GetBorderColor(void)
{
	return m_BorderColor;
}

void MPanel::SetBorderStyle(MBorderStyle style)
{
	m_nBorderStyle = style;
}

MBorderStyle MPanel::GetBorderStyle()
{
	return m_nBorderStyle;
}

void MPanel::SetBackgroundColor(MCOLOR color)
{
	m_BackgroundColor = color;
}

MCOLOR MPanel::GetBackgroundColor(void)
{
	return m_BackgroundColor;
}

bool MPanel::OnEvent(MEvent* pEvent, MListener* pListener)
{
	MWidget::OnEvent(pEvent, pListener);
	
	//////////////////////////////////////////////////////////////////////////
	MRECT WidgetRect(0, 0, m_Rect.w, m_Rect.h);

	switch(pEvent->nMessage)
	{
	case MWM_LBUTTONDOWN:
		if(WidgetRect.InPoint(pEvent->Pos)==true)
		{
			return true;
		}
		break;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void MPanelLook::OnDraw(MPanel* pPanel, MDrawContext* pDC)
{
	MRECT r = pPanel->GetInitialClientRect();
	if (pPanel->GetBackgroundColor().a != 0)
	{
		pDC->SetColor(pPanel->GetBackgroundColor());
		pDC->FillRectangle(r);
	}

	if (pPanel->GetBorderStyle() == MBS_SINGLE)
	{
		pDC->SetColor(pPanel->GetBorderColor());
		pDC->Rectangle(r);
	}
}

MRECT MPanelLook::GetClientRect(MPanel* pLabel, MRECT& r)
{
	return r;
}


} // namespace mint3