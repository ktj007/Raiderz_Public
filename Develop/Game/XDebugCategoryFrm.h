#pragma once

#include "Mint4Game.h"
#include "MWidget.h"
#include "MComboBox.h"
using namespace std;
using namespace mint3;

class XDebugCategoryFrm :	public MFrame
{
private:
	friend class Mint4Game;

	MButton*				m_pCloseButton;
	MComboBox*				m_pCategory;

	MButton*				m_pFPSButton;
	MButton*				m_pObjListButton;
	MButton*				m_pHelpButton;
	MButton*				m_pNPCMonitorButton;
	MButton*				m_pNPCLogButton;
	MButton*				m_pCombatButton;
	MButton*				m_pRenderGraphButton;
	MButton*				m_pSoundButton;
	MButton*				m_pLootButton;
	MButton*				m_pMovableButton;
	MButton*				m_pMLogButton;
	MButton*				m_pCLogButton;
	MButton*				m_pCollisionButton;

protected:
	virtual void OnDraw(MDrawContext* pDC);
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
//	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
	virtual void OnShow( bool bShow);

	XDebugCategoryFrm(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

	void AddMenuButtons();
	void SelChange(int nIndex);

public:
	virtual ~XDebugCategoryFrm(void);

#define MINT_DEBUGCATEGORYFRM		"debugcategoryfrm"
	virtual const char* GetClassName(void){ return MINT_DEBUGCATEGORYFRM; }
};
