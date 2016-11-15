#ifndef _XONDEBUGSCREENDIALOG
#define _XONDEBUGSCREENDIALOG

#include "XOnScreenDialog.h"

class XDebugScreenDialog : public mint3::MPanel
{
protected:
	XObject*		m_pOwner;

	MLabel*			m_pLabel;
//	MTextArea*		m_pTextArea;

	int				m_nLine;
	int				m_nChatSize;

	void		SetInfo(wstring szID, wstring szChat);
	bool		CheckVisible();

public:
	XDebugScreenDialog( XObject* pOwner, const char* szName, MWidget* pParent, MListener* pListener );
	virtual ~XDebugScreenDialog(void);

	virtual void	OnRun(void);

	void			Init(wstring szID, wstring szChat);


	int GetLineWidth(const wchar_t* str)
	{
//		MFont* pFont = m_pTextArea->GetFont();
		MFont* pFont = MFontManager::Get( NULL);
		if (pFont == NULL) return 1;
		return pFont->GetWidth(MLocale::ConvUTF16ToAnsi(str).c_str());
	}
	void			OnAir( wstring szID, wstring szChat);

#define MINT_ONDEBUGSCREENPANEL "OnDebugScreenDialog"
	virtual const char* GetClassName(void)	{ return MINT_ONDEBUGSCREENPANEL;}
};

#endif // _XONDEBUGSCREENDIALOG
