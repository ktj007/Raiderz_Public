#include "stdafx.h"
#include "MLabel.h"
#include "MColorTable.h"

namespace mint3 {

#define MLABEL_DEFAULT_ALIGNMENT_MODE	MAM_LEFT


IMPLEMENT_LOOK(MLabel, MLabelLook)

MLabel::MLabel(const char* szName, MWidget* pParent, MListener* pListener) : MWidget(szName, pParent, pListener)
{
	LOOK_IN_CONSTRUCTOR()

	m_TextColor		= MColorTable::MLABEL_TEXT;
	m_AlignmentMode	= MLABEL_DEFAULT_ALIGNMENT_MODE;

	MFont* pFont = GetFont();
	int w = 100;
	if(szName!=NULL) w = pFont->GetWidth(szName);
	int h = pFont->GetHeight();
	SetSize(w, h);
}

void MLabel::SetTextColor(MCOLOR color)
{
	m_TextColor = color;
}

MCOLOR MLabel::GetTextColor(void)
{
	return m_TextColor;
}

MAlignmentMode MLabel::GetAlignment(void)
{
	return m_AlignmentMode;
}

MAlignmentMode MLabel::SetAlignment(MAlignmentMode am)
{
	MAlignmentMode temp = m_AlignmentMode;
	m_AlignmentMode = am;
	return temp;
}

// ARGB 값이 제대로 안넘어와서.
void MLabel::glueSetTextColor(unsigned int color )
{
	unsigned long int t = ((((unsigned long int)255)&0xFF)<<24) | color;

	SetTextColor(t);
}

bool MLabel::OnEvent(MEvent* pEvent, MListener* pListener)
{
	if (MWidget::OnEvent(pEvent, pListener) == true) return true;

	return false;
}

void MLabelLook::OnDraw(MLabel* pLabel, MDrawContext* pDC)
{
	// client edge
#ifdef _DEBUG
	// 텍스트가 "show rect"이면 윈도우 영역을 표시한다
	if ( string( pLabel->GetText()) == string( "show rect"))
	{
		pDC->SetColor( 255, 255, 255, (int)(25.0 * sin( (double)timeGetTime() * 0.005) + 25.0));
		pDC->Rectangle( 0, 0, pLabel->GetRect().w, pLabel->GetRect().h);
		return;
	}
#endif

	if (strlen(pLabel->GetText()) <= 0) return;

//	if (pDC->BeginFont())
	{
		pDC->SetColor(pLabel->GetTextColor());
		MCOLOR PrevHCol = pDC->SetHighlightColor(MColorTable::PEN_HIGHLIGHT);
		MRECT r = pLabel->GetClientRect();
		pDC->TextWithHighlight(r, pLabel->GetText(), pLabel->GetAlignment());
		pDC->SetHighlightColor(PrevHCol);
//		pDC->EndFont();
	}
}

MRECT MLabelLook::GetClientRect(MLabel* pLabel, MRECT& r)
{
	MRECT t = r;
	t.EnLarge(-1);
	return t;
}


} // namespace mint3