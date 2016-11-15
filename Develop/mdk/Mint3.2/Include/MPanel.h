#pragma once

#include "MWidget.h"



namespace mint3
{



/// class : Panel
class MPanel : public MWidget
{
/// Member functions
public:
	MPanel( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);



/// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnDraw( MDrawContext* pDC) override;



#define MINT_PANEL	"Panel"
	virtual const char* GetClassName()		{ return MINT_PANEL;		}
};

} // namespace mint3