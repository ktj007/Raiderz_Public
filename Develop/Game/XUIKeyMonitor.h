#pragma once

#include "MWidget.h"

using namespace mint3;



class XUIKeyMonitor : public MWidget
{
protected:
	unsigned int	m_nTime;
	int				m_nViewSecond;


public:
	XUIKeyMonitor( const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~XUIKeyMonitor();
	

protected:
 	virtual void OnDraw( MDrawContext* pDC);
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);


#define MINT_KEYMONITOR		"KeyMonitor"
	virtual const char* GetClassName(void)		{ return MINT_KEYMONITOR; }
};