#pragma once

#include "Mint4Game.h"
#include "MTextFrame.h"

class XDebugCombat : public MTextFrame
{
private:
	friend class Mint4Game;

protected:
	virtual void OnDraw(MDrawContext* pDC);

	XDebugCombat(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

public:

#define MINT_DEBUGCOMBAT	"debugcombat"
	virtual const char* GetClassName(void){ return MINT_DEBUGCOMBAT; }
};
