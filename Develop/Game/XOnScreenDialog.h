#ifndef _XONSCREENDIALOG
#define _XONSCREENDIALOG

#include "MPanel.h"
#include "MLabel.h"
#include "MTextView.h"

#include "MintLoader.h"
#include "MLookManager.h"
#include "MTime.h"

using namespace rs3;

#define BALLON_DEFAULTl_WIDTH			300

class XOnScreenDialog : public mint3::MPanel
{
protected:
	MUID			m_UID;
	MRegulator		m_ShowRegulator;

	MLabel*			m_pLabel;
	MTextView*		m_pTextView;

	RTimer			m_Timer;
	
	int				m_WidthMrgin;
	int				m_HeightMargin;		///< 텍스트 그리는 영역외 height
	int				m_nLineCount;
	int				m_nChatWidth;

	void			Create();
	void			SetInfo(wstring szID, wstring szChat);
	void			CalculateSize( wstring szID, wstring szChat );

	int				GetWidgetFontWidth( MWidget* pWidget, const wchar_t* str);
	int				GetWidgetFontHeight( MWidget* pWidget);

public:
	XOnScreenDialog( MUID uid, const char* szName, MWidget* pParent, MListener* pListener );
	virtual ~XOnScreenDialog(void);
	virtual void	OnRun(void);

	void			OnAir(wstring szID, wstring szChat, float fMaintainTime );

#define MINT_ONSCREENPANEL "OnScreenDialog"
	virtual const char* GetClassName(void)	{ return MINT_ONSCREENPANEL;}
};

#endif // _XONSCREENDIALOG
