#ifndef _XWEAPONBINDINGMENU_H
#define _XWEAPONBINDINGMENU_H

#include "MComboBox.h"

class XWeaponListBox : public MListBox{
protected:
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);

public:
	XWeaponListBox(const char* szname, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~XWeaponListBox(void){}

//	DECLARE_LOOK(MListBoxLook)
//	DECLARE_LOOK_CLIENT()

#define MINT_WEAPONLISTBOX	"WeaponListBox"
	virtual const char* GetClassName(void){ return MINT_WEAPONLISTBOX; }
};

class XWeaponBindingMenu : public MWidget
{
	friend class Mint4Game;

protected:
	// ListBox
	XWeaponListBox*	m_pListBox;

	// 드롭박스 높이
	int				m_nDropHeight;

	// 페이드 타이머
	DWORD			m_dwFadeTimer;

protected:
	XWeaponBindingMenu(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);

public:
	virtual ~XWeaponBindingMenu(void);
	virtual void OnMouseDown(MEvent* pEvent);
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const wchar_t* szArgs=NULL);
	virtual void OnRun( void);
	virtual void OnDraw(MDrawContext* pDC);

	void OnSelectedItemChanged();

	bool SetSelIndex(int i);
	void glueSetSelectedIndex(int nIndex);

#define MINT_WEAPONBINDING_MENU "weaponbindingmenu"
	virtual const char* GetClassName(void){ return MINT_WEAPONBINDING_MENU; }
};

#endif	//_XWEAPONBINDINGMENU_H

