#pragma once

#include "MTextFrame.h"

class XDebugMovableFrm :	public MTextFrame
{
private:
	friend class Mint4Game;

protected:
	virtual void OnDraw(MDrawContext* pDC);
	virtual void OnShow( bool bShow);

	XDebugMovableFrm(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

public:

#define MINT_DEBUGMOVABLE		"debugMovable"
	virtual const char* GetClassName(void){ return MINT_DEBUGMOVABLE; }
};
