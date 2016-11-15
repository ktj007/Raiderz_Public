#pragma once

#include "MWidget.h"
#include "MLookBase.h"



namespace mint3
{


class MGroup;




/// class : MGroupLook
class MGroupLook : public MLookBase
{
/// Member functions
public:
	MGroupLook();

	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;

protected:
	virtual void OnDrawText( MGroup* pGroup, MRECT& r, MDrawContext* pDC);
};







/// class : MGroup
class MGroup : public MWidget
{
/// Member functions
public:
	MGroup( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);


#define MINT_GROUP	"Group"
	virtual const char* GetClassName()			{ return MINT_GROUP;			}
};


} // namespace mint3

