#pragma once

#include "MFrame.h"
#include "MButton.h"
#include "MLabel.h"
#include "MTextArea.h"

namespace mint3 {

enum MMsgBoxType{
	MT_OK,
	MT_CANCEL,
	MT_OKCANCEL,
	MT_YESNO,

	MT_NOTDECIDED,	// 미리 만들어놓지 않기 위해
};

class MMsgBox : public MFrame{
protected:
	MTextArea*	m_pMessage;
	MButton*	m_pOK;
	MButton*	m_pCancel;
	MMsgBoxType	m_nType;

protected:
	/*
	virtual void OnDrawText(MDrawContext* pDC);
	virtual void OnDraw(MDrawContext* pDC);
	*/
	virtual void OnShow(void);
	virtual void OnSize(int w, int h);

	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);

public:
	MMsgBox(const char* szMessage, MWidget* pParent, MListener* pListener=NULL, MMsgBoxType nType=MT_NOTDECIDED );
	virtual ~MMsgBox(void);

	void SetType(MMsgBoxType nType);
	void SetTitle(const char* szTitle);
	void SetOKButtonText(const char* szText);
	void SetCancelButtonText(const char* szText);
	const char* GetTitle(void);
	void SetMessage(const char* szMessage);
	const char* GetMessage(void);
	virtual void SetText(const char* szText);
	virtual const char* GetText(void);

#define MINT_MSGBOX	"MsgBox"
	virtual const char* GetClassName(void){ return MINT_MSGBOX; }
};

} // namespace mint3