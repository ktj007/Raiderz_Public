#pragma once

#include <list>
using namespace std;

#include "MWidget.h"
#include "MLookNFeel.h"
#include "MPanel.h"
#include "MButtonGroup.h"

namespace mint3 {


class MTabCtrl;
class MDrawContext;
//class MButton;


/// MTabCtrl의 Draw 코드가 있는 클래스, 이 클래스를 상속받아서 커스텀룩을 가질 수 있다.
class MTabCtrlLook
{
public:
	virtual void OnDraw(MTabCtrl* pTabCtrl, MDrawContext* pDC);
	virtual MRECT GetClientRect(MTabCtrl* pTabCtrl, MRECT& r);
};

class MNullTabCtrlLook : public MTabCtrlLook
{
public:
	virtual void OnDraw(MTabCtrl* pTabCtrl, MDrawContext* pDC) {}
};

//// MTabItem
class MTabItem 
{
public:
	MPanel		*pPanel;
	MButton		*pButton;
	MListener	*pButtonListener;		// 버튼의 원래 리스너

	MTabItem(MPanel *pPanel, MButton *pButton, MListener *pListener);
};

class MTabList : public list<MTabItem*> {};


//// MTabCtrl
class MTabCtrl : public MWidget 
{
protected:
	int				m_nCurrentSel;
	MTabList		m_TabList;
	MButtonGroup	m_ButtonGroup;

protected:
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);

public:
	MTabCtrl(const char* szName, MWidget* pParent=NULL, MListener* pListener=NULL);
	MTabCtrl(MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MTabCtrl(void);
		
	void Add(MButton *pButton, MPanel *pPanel);		// pButton 을 누르면 pFrame 이 보여진다
	void RemoveAll();

	int GetCount(void);
	int GetSelIndex(void);
	bool SetSelIndex(int i);

	void UpdateListeners();			// 만약 버튼의 리스너가 바뀐다면 이펑션을 불러주어야 한다.

	int		glueGetCount();
	int		glueGetSelIndex();
	bool	glueSetSelIndex(int index);
	bool	glueSetSelTabName(const char* szTabName);

	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
public:
	DECLARE_LOOK(MTabCtrlLook)
	DECLARE_NULL_LOOK(MNullTabCtrlLook)
	DECLARE_LOOK_CLIENT()
	

	#define MINT_TABCTRL	"TabCtrl"
	virtual const char* GetClassName(void){ return MINT_TABCTRL; }

public:
	
};


} // namespace mint3