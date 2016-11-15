#pragma once

#include "MWidget.h"


namespace mint3
{


/// class : MSheet
class MSheet : public MWidget
{
/// Member variables
private:
	bool	m_bDragAndDrop;



/// Member functions
public:
	MSheet( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	virtual bool IsLayered() const override;

	virtual void EnableDragAndDrop( bool bEnable) override		{ m_bDragAndDrop = bEnable;		}
	virtual bool IsEnableDragAndDrop() const override			{ return m_bDragAndDrop;		}



/// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);



#define MINT_SHEET	"Sheet"
	virtual const char* GetClassName()			{ return MINT_SHEET;			}
};

} // namespace mint3