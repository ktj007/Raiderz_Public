#pragma once

#define MDEPRECATED
#ifdef MDEPRECATED

#include "MButton.h"
#include "MEdit.h"

namespace mint3 {

class MBmButton : public MButton{
protected:
	MBitmap*	m_pUpBitmap;
	MBitmap*	m_pDownBitmap;
	MBitmap*	m_pOverBitmap;
	MBitmap*	m_pDisableBitmap;
	bool			m_bStretch;
public:
	bool			m_bTextColor;
	MCOLOR	m_BmTextColor;

protected:
	virtual void OnDownDraw(MDrawContext* pDC);
	virtual void OnUpDraw(MDrawContext* pDC);
	virtual void OnOverDraw(MDrawContext* pDC);
	virtual void OnDisableDraw(MDrawContext* pDC);
	virtual void OnDraw(MDrawContext* pDC);

public:
	MBmButton(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

	void SetUpBitmap(MBitmap* pBitmap);
	void SetDownBitmap(MBitmap* pBitmap);
	void SetDisableBitmap(MBitmap* pBitmap);
	void SetOverBitmap(MBitmap* pBitmap);
#define MINT_BMBUTTON	"BmButton"
	virtual const char* GetClassName(void){ return MINT_BMBUTTON; }

	void SetStretch( bool b ){		m_bStretch = b;	}

	virtual MRECT GetClientRect(void){ return MWidget::GetClientRect(); }
};

} // namespace mint3
#endif