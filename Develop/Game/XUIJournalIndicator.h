#pragma once

#include "MWidget.h"
#include "MScrollBar.h"
#include "MTextView.h"
#include "MButton.h"


using namespace std;
using namespace mint3;




// XJournalObjType
enum XJOURNALTYPE
{
	XJT_NONE		= 0,
	XJT_QUEST,
	XJT_RECIPE
};





/// class : XJournalItem
class XJournalItem : public MTextView
{
protected:
	int			m_nID;
	MWidget*	m_pCloseBtn;
	bool		m_bActivate;


public:
	XJournalItem( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XJournalItem();

	virtual const char* GetState();
	virtual int GetStateNum();

	virtual XJOURNALTYPE GetType() const			{ return XJT_NONE;			}
	int GetID() const								{ return m_nID;				}
	MWidget* GetCloseBtn()							{ return m_pCloseBtn;		}

	void SetActivate( bool bActivate)				{ m_bActivate = bActivate;	}
	bool IsActivate()								{ return m_bActivate;		}

	virtual bool Create( int nID);
	virtual bool Destroy();
	virtual bool RefreshText()						{ return false;				}

	virtual bool OnRefresh()						{ return false;				}


/// Event handler
protected:
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs =NULL) override;


public:
#define MINT_JOURNALITEM	"JournalItem"
	virtual const char* GetClassName() override		{ return MINT_JOURNALITEM;		}
};





/// class : XJournalQuest
class XJournalQuest : public XJournalItem
{
public:
	XJournalQuest( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XJournalQuest()					{}

	virtual XJOURNALTYPE GetType() const		{ return XJT_QUEST;			}

	virtual bool RefreshText() override;

	virtual bool OnRefresh() override;
};





/// class : XJournalRecipe
class XJournalRecipe : public XJournalItem
{
public:
	XJournalRecipe( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XJournalRecipe()			{}

	virtual XJOURNALTYPE GetType() const		{ return XJT_RECIPE;		}

	virtual bool RefreshText() override;

	virtual bool OnRefresh() override;
};










/// class : XUIJournalIndicator
class XUIJournalIndicator : public MWidget
{
/// Member variables
protected:
	list< XJournalItem*>		m_Children;
	list< XJournalItem*>		m_DeleteItemQueue;			// 아이템 삭제 명령을 받으면 바로 지우는게 아니라 update 타임에 삭제한다
	static XJournalItem*		m_pFocusedItem;
	MScrollBar*					m_pScrollBar;
	MLookBase*					m_pTextLook;
	MLookBase*					m_pButtonLook;
	bool						m_bRefreshAll;
	bool						m_bUpdateScrollBar;
	bool						m_bArrangeChildren;


/// Member functions
public:
	XUIJournalIndicator( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XUIJournalIndicator();

	void SetTextLook( MLookBase* pLook)						{ m_pTextLook = pLook;							}
	void SetButtonLook( MLookBase* pLook)					{ m_pButtonLook = pLook;						}

	bool AddItem( int nType, int nID);
	bool RemoveItem( int nType, int nID);
	bool RefreshItem( int nType, int nID);
	bool IsEmpty() const									{ return m_Children.empty();					}
	void Clear();

	void RefreshAll()										{ m_bRefreshAll = true;							}

	static XJournalItem* GetFocused()						{ return XUIJournalIndicator::m_pFocusedItem;	}
	static bool GetFocusedTypeID( XJOURNALTYPE* pnType, int* pnID);


	virtual MScrollBar* GetScrollBar() override				{ return m_pScrollBar;							}
	void ScrollPage( int delta);



/// Event handler
protected:
	virtual MRESULT OnPreTranslateMsg( MEvent* pEvent) override;
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs =NULL) override;
	virtual void OnRun( DWORD dwCurrTime) override;
	virtual void OnSize( int w, int h) override;
	virtual void OnScrollBarChanged( int nPos) override;



/// LUA script
public:
	bool glueAddItem( int nType, int nID)					{ return AddItem( nType, nID);					}
	bool glueRemoveItem( int nType, int nID)				{ return RemoveItem( nType, nID);				}
	bool glueRefreshItem( int nType, int nID)				{ return RefreshItem( nType, nID);				}
	bool glueIsEmpty()										{ return IsEmpty();								}
	void glueClear()										{ Clear();										}
	void glueRefreshAll()									{ RefreshAll();									}
	MWLua::ret_int2 glueGetFocused();



#define MINT_JOURNALINDICATOR		"JournalIndicator"
	virtual const char* GetClassName() override				{ return MINT_JOURNALINDICATOR;					}
};

