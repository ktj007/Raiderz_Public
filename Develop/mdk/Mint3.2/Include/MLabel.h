#pragma once

#include "MWidget.h"
#include "MLookBase.h"


namespace mint3
{

class MLabel;



/// class : MLabelLook
class MLabelLook : public MLookBase
{
/// Member functions
public:
	MLabelLook();

	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;


protected:
	virtual void OnDrawText( MLabel* pLabel, MRECT& r, MDrawContext* pDC);
};




/// class : Label
class MLabel : public MWidget
{
/// Member functions
public:
	MLabel( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);



#define MINT_LABEL	"Label"
	virtual const char* GetClassName()		{ return MINT_LABEL;			}
};


} // namespace mint3