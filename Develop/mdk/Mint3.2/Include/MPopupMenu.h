#pragma once

#include "MWidget.h"
#include "MLookBase.h"


namespace mint3
{

class MPopupMenu;



/// class : MPopupMenuLook
class MPopupMenuLook : public MLookBase
{
/// Member functions
public:
	MPopupMenuLook();

	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;
};




/// class : PopupMenu
class MPopupMenu : public MWidget
{
/// Member variables
protected:
	MWidget*	m_pOwner;


/// Member functions
public:
	MPopupMenu( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);

	bool TrackPopupMenu( MPOINT& ptPoint, MWidget* pOwner);


/// Event handler
protected:
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs =NULL);
	virtual void OnRun( DWORD dwCurrTime);
	virtual void OnShow( bool bShow);
	virtual void OnCancel();


#define MINT_POPUPMENU	"PopupMenu"
	virtual const char* GetClassName()		{ return MINT_POPUPMENU;			}
};


} // namespace mint3