#ifndef _XDEBUGFPS_H_
#define _XDEBUGFPS_H_

#include "Mint4Game.h"
#include "MTextFrame.h"

class XDebugFPSFrm :	public MTextFrame
{
private:
	friend class Mint4Game;

	FrameCounter			m_FrameCounter;

protected:
	XDebugFPSFrm(const char* szName, MWidget* pParent=NULL, MListener* pListener=NULL);

	virtual void OnDraw(MDrawContext* pDC);

public:

#define MINT_DEBUGFPSFRM		"debugFPSfrm"
	virtual const char* GetClassName(void){ return MINT_DEBUGFPSFRM; }
};

#endif