#ifndef _XMSG_BOX_H
#define _XMSG_BOX_H

#include "MMsgBox.h"

class XMsgBox : public MMsgBox{
protected:
// 	bool				m_bExclusive;
// 	unsigned long int	m_nShowTime;
// 	bool				m_bNextVisible;
// 	MListener*			m_pCustomListener;
// 
// 	virtual void OnShow(void);
// 	virtual void OnDraw(MDrawContext* pDC);

public:
	XMsgBox(const char* szMessage, MWidget* pParent, MListener* pListener=NULL, MMsgBoxType nType=MT_NOTDECIDED );
	virtual ~XMsgBox(void);

// 	void SetCustomListener(MListener* pListener) { m_pCustomListener = pListener; }
// 	MListener* GetCustomListener() { return m_pCustomListener; }
// 
// 	void Show(bool bVisible, bool bModal=false);
// 	virtual void SetText(const char* szText);
};


#endif // _XMSG_BOX_H