#pragma once

#include "Mint4Game.h"
#include "MTextFrame.h"

class XDebugLootFrm : public MTextFrame
{
private:
	friend class Mint4Game;

protected:
	virtual void OnDraw(MDrawContext* pDC);

	XDebugLootFrm(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

public:

#define MINT_DEBUGLOOT	"debugloot"
	virtual const char* GetClassName(void){ return MINT_DEBUGLOOT; }
};
