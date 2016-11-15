#pragma once

#include "Mint4Game.h"
#include "MTextFrame.h"

using namespace std;
using namespace mint3;

class XDebugObjListFrm :	public MTextFrame
{
private:
	friend class Mint4Game;

protected:
	virtual void OnRun( DWORD dwCurrTime) override;

	XDebugObjListFrm(const char* szName, MWidget* pParent=NULL, MListener* pListener=NULL);

public:

#define MINT_DEBUGOBJLISTFRM		"debugObjList"
	virtual const char* GetClassName(void){ return MINT_DEBUGOBJLISTFRM; }
};
