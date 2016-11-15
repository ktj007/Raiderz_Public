#pragma once

#include "MTextFrame.h"

class XDebugCollisionFrm : public MTextFrame
{
private:
	friend class Mint4Game;

	MButton*				m_pTerrainCol;
	MButton*				m_pObjectCol;
	MButton*				m_pPlayerCol;

protected:
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
	virtual void OnShow( bool bShow);

	void AddControls();

	XDebugCollisionFrm(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

public:

#define MINT_DEBUGCOLLISION "debugcollision"
	virtual const char* GetClassName(void){ return MINT_DEBUGCOLLISION; }
};
