#pragma once

#include "MWidget.h"

using namespace mint3;

class XUIHDRMonitor : public MWidget
{
public:
	float m_fBucketSize;
	float m_fBucketBlankSize;

	XUIHDRMonitor( const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~XUIHDRMonitor();
	
	void RenderBarrier(MDrawContext* pDC, MRECT& rect, MCOLOR color, int nBucketFrame, float fCountPixel);

protected:
 	virtual void OnDraw( MDrawContext* pDC);
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);


#define MINT_HDRMONITOR		"HDRMonitor"
	virtual const char* GetClassName(void)		{ return MINT_HDRMONITOR; }
};