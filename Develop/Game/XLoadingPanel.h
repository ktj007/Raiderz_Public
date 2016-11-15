#pragma once

#include "Mint4Game.h"
#include "MPanel.h"
#include "MWidget.h"
#include "MPicture.h"

using namespace std;
using namespace mint3;

class XLoadingPanel : public MPanel
{
private:
	friend class Mint4Game;
	MPicture*		m_pBackGround;
	MBitmapR3*		m_pBitmap;
//	MTextArea*		m_pTipTextArea;
	MPanel*			m_pBackGroundPanel;
public:
	XLoadingPanel(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~XLoadingPanel(void);
	void SetTip(const wchar_t* szTip);

#define MINT_LOADING_PANEL		"loadingpanel"
	virtual const char* GetClassName(void){ return MINT_LOADING_PANEL; }

};








