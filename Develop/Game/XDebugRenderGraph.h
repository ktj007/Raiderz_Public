#pragma once

#include "Mint4Game.h"
#include "MTextFrame.h"

class XDebugRenderGraph : public MTextFrame
{
private:
	friend class Mint4Game;

protected:
	XDebugRenderGraph(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

	virtual void OnDraw(MDrawContext* pDC);
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);

public:

#define MINT_DEBUGRENDERGRAPH	"debugrendergraph"
	virtual const char* GetClassName(void){ return MINT_DEBUGRENDERGRAPH; }
};
