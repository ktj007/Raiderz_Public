#pragma once

#include "Mint4Game.h"
#include "MTextFrame.h"

class XDebugSoundFrm : public MTextFrame
{
private:
	friend class Mint4Game;

protected:
	virtual void OnDraw(MDrawContext* pDC);

	XDebugSoundFrm(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

public:

#define MINT_DEBUGSOUND	"debugsound"
		virtual const char* GetClassName(void){ return MINT_DEBUGSOUND; }
};
