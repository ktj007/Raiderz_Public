#ifndef _XNARRATIONLABEL_H
#define _XNARRATIONLABEL_H

#include "MTime.h"

class XNarrationLabel : public MWidget
{
private:
	MFont*			m_pFont;
	MCOLOR			m_nColor;
	RTimer			m_Timer;
	MRegulator		m_ShowRegulator;
	float			m_fFadingAlpha;
	bool			m_bFadeIn;

	void			Init();

public:
	XNarrationLabel(const char* szName, MWidget* pParent, MListener* pListener);

	virtual ~XNarrationLabel(void);

	virtual void	OnRun(void);

	void			SetFont( MFont* pFont)				{ m_pFont = pFont;		}
	void			SetTextColor( MCOLOR& color)		{ m_nColor = color;		}
	void			SetText( const wchar_t* szText, float fRemainTime );

#define MINT_NARRATIONPANEL		"NarrationPanel"
	virtual const char* GetClassName(void)		{ return MINT_NARRATIONPANEL; }
};

#endif // _XNARRATIONLABEL_H
