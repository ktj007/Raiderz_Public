#pragma once

#include "MTextFrame.h"

class XDebugNPCMonitor : public MTextFrame
{
private:
	friend class Mint4Game;

protected:
	virtual void OnDraw(MDrawContext* pDC);

	XDebugNPCMonitor(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

public:
	void SaveLog();

#define MINT_DEBUGNPCMONITOR	"debugNPCMonitor"
	virtual const char* GetClassName(void){ return MINT_DEBUGNPCMONITOR; }
};
