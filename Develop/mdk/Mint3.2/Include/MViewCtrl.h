#pragma once

#include "MWidget.h"
#include "MPanel.h"
#include "MLookBase.h"


namespace mint3
{

class MViewCtrl;






// class : MViewCtrl
class MViewCtrl : public MWidget
{
/// Member variables
protected:
	MPanel*					m_pPanel;
	MScrollBar*				m_pScrollBar;
	int						m_nMinScrollRange;
	int						m_nMaxScrollRange;



/// Member functions
public:
	MViewCtrl( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MViewCtrl();

	// Panel
	MPanel* GetPanel()									{ return m_pPanel;							}

	// Scrollbar
	virtual MScrollBar* GetScrollBar() override			{ return m_pScrollBar;						}
	void ScrollPage( int delta);


	void SetScrollValue( int nValue);
	int GetScrollValue() const;

	void SetScrollRange( int min, int max);
	void GetScrollRange( int* min, int* max);

	void GetScrollBarRange( int* min, int* max);



/// Event handler
protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnSize( int w, int h) override;
	virtual void OnScrollBarChanged( int nPos) override;



// LUA script
public:
	void glueSetScrollValue( int nValue)				{ SetScrollValue( nValue);					}
	int glueGetScrollValue() const						{ return GetScrollValue();					}
	void glueSetScrollRange( int min, int max)			{ SetScrollRange( min, max);				}
	MWLua::ret_int2 glueGetScrollRange();
	MWLua::ret_int2 glueGetScrollBarRange();



#define MINT_VIEWCTRL	"ViewCtrl"
	virtual const char* GetClassName() override			{ return MINT_VIEWCTRL;						}
};

} // namespace mint3