#include "StdAfx.h"
#include "XNarrationLabel.h"

#define NARRATION_COLOR			0xFFE5D018
#define NARRATION_REMAINTIME	4.f

XNarrationLabel::XNarrationLabel( const char* szName, MWidget* pParent, MListener* pListener )
: MWidget(szName, pParent, pListener)
{
	m_ShowRegulator.Stop();
	Init();
}

XNarrationLabel::~XNarrationLabel( void )
{
}

void XNarrationLabel::Init()
{
	m_fFadingAlpha	= 1.0f;
	m_bFadeIn = true;
}

void XNarrationLabel::SetText( const wchar_t* szText, float fRemainTime )
{
// 	MWidget::SetText(szText);
// 
// 	int len = strlen(szText);
// 	int width = m_pFont->GetWidth(szText);
// 	int x = (XGETCFG_VIDEO_WIDTH - width)/2;
// 
// 	SetRect(x, 140, width, 24);
// 
// 	SetTextColor(NARRATION_COLOR);
// 	Show( true);
// 
// 	if (fRemainTime == 0.f)
// 	{
// 		fRemainTime = NARRATION_REMAINTIME;
// 	}
// 
// 	m_ShowRegulator.SetTickTime(fRemainTime);
// 	m_ShowRegulator.Start();
}

//현재 남은 시간 동안 보여주고 2초동안 사라지도록 되어있다.
void XNarrationLabel::OnRun( void )
{
	m_Timer.Update();
	float fsp = m_Timer.GetElapsedTime() * 0.001f;

	{
		float t = fsp/2.f;

		if (!m_bFadeIn)
		{
			m_fFadingAlpha -= t;
		}

		m_fFadingAlpha = min(1.0f, max(m_fFadingAlpha, 0.0f));
		if (m_fFadingAlpha <= 0.f )
		{
			Hide();
			Init();
		}

		MCOLOR color = m_nColor;
		unsigned char alpha = (unsigned char)(255 * m_fFadingAlpha);
		SetTextColor(MCOLOR(color.r, color.g, color.b, alpha));
	}

	if(m_ShowRegulator.IsActive() && m_ShowRegulator.IsReady(fsp))
	{
		m_ShowRegulator.Stop();
		m_bFadeIn = false;
	}
}