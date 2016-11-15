#pragma once

#include "Mint4Game.h"
#include "MTextFrame.h"

using namespace std;
using namespace mint3;

class XHelpFrm :	public MTextFrame
{
private:
	friend class Mint4Game;

protected:
	virtual void OnDraw(MDrawContext* pDC);

	XHelpFrm(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

public:

#define MINT_HELPFRM		"help"
	virtual const char* GetClassName(void){ return MINT_HELPFRM; }
};
