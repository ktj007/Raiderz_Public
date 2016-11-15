#pragma once

#include "MWidget.h"

using namespace mint3;



class XUIControlKey : public MWidget
{
protected:


public:
	XUIControlKey( const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~XUIControlKey();


protected:
	virtual void OnDraw( MDrawContext* pDC);
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);
	virtual bool OnSetCursor( MEvent* pEvent, string* pCursor) override;


#define MINT_CONTROLKEY		"ControlKey"
	virtual const char* GetClassName(void)		{ return MINT_CONTROLKEY; }
};
