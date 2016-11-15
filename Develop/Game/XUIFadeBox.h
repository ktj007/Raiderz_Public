#pragma once

#include "MWidget.h"


using namespace std;
using namespace mint3;




/// class : XUIFadeBox
class XUIFadeBox : public MWidget
{
protected:
	MWidget*	m_pFiltered;
	MTimer		m_tFade;


public:
	XUIFadeBox( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XUIFadeBox();


protected:
	virtual int GetDrawState() override;


protected:
	virtual bool OnPreFilterMessage( MEvent* pEvent) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnPreDraw( MDrawContext* pDC) override;


#define MINT_FADEBOX		"FadeBox"
	virtual const char* GetClassName() override				{ return MINT_FADEBOX;			}
};

