#pragma once


#include "MWLua.h"
#include <set>
#include <string>
using namespace std;

namespace mint3 {


class MWidget;
class MDragData;


/// Listener에게 전달되는 메세지
enum MEventMsg
{
	MEVENT_UNKNOWN		= -1,
	MEVENT_LOADED,
	MEVENT_UPDATE,
	MEVENT_DRAWITEMBKGRND,
	MEVENT_PREDRAWITEM,
	MEVENT_DRAWITEM,
	MEVENT_PREDRAW,
	MEVENT_DRAW,
	MEVENT_SHOW,
	MEVENT_ENABLE,
	MEVENT_SETFOCUS,
	MEVENT_KILLFOCUS,
	MEVENT_POSITION,
	MEVENT_SIZE,
	MEVENT_LBUTTONDOWN,
	MEVENT_LBUTTONUP,
	MEVENT_RBUTTONDOWN,
	MEVENT_RBUTTONUP,
	MEVENT_MOUSEMOVE,
	MEVENT_MOUSEWHEEL,
	MEVENT_CLICK,
	MEVENT_RCLICK,
	MEVENT_DBLCLICK,
	MEVENT_KEYDOWN,
	MEVENT_KEYUP,
	MEVENT_CHAR,
	MEVENT_SELCHANGE,
	MEVENT_VALUECHANGED,
	MEVENT_ITEMCLICK,
	MEVENT_ITEMDBLCLICK,
	MEVENT_ITEMRCLICK,
	MEVENT_DRAGBEGIN,
	MEVENT_DRAGEND,
	MEVENT_DROP,
	MEVENT_TOOLTIP,
	MEVENT_OK,
	MEVENT_CANCEL,
	MEVENT_ENTER,
	MEVENT_LEAVE,
	MEVENT_SETCURSOR,
	MEVENT_INITPOPUPMENU,
	MEVENT_TIMER,
	MEVENT_NCHITTEST,
	MEVENT_USERARGUMENT,

	MEVENT_MAX
};



/// 위젯들의 메세지를 받을 수 있는 Listener(Pure Virtual Class)
class MListener{
public:
	/// 메세지를 받기위한 Virtual Function
	/// @param	pWidget		메세지를 보내는 위젯 포인터
	/// @param	szMessage	메세지(스트링)
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL) = 0;
};

// 루아에서 사용하는 이벤트 인자값 - 이벤트 인자는 전역적으로 한 객체만 가진다.
class MEventArgs
{
public:
	MWidget*		m_pOwner;
	int				m_nPosX;
	int				m_nPosY;
	int				m_nWheel;
	unsigned int	m_nKey;
	string			m_strArgument;
	bool			m_bShift;
	bool			m_bCtrl;
	int				m_nHitTest;
	int				m_nItemIndex;
	int				m_nItemSubItem;
	MRECT			m_ItemRect;
	DWORD			m_dwUserData;
	
public:
	MEventArgs()
	{
		Reset();
	}

	void Reset()
	{
		m_pOwner = NULL;
		m_nPosX = 0;
		m_nPosY = 0;
		m_nWheel = 0;
		m_nKey = 0;
		m_strArgument = "";
		m_bShift = false;
		m_bCtrl = false;
		m_nItemIndex = -1;
		m_nItemSubItem = 0;
		m_dwUserData = 0;
	}

	void SetFromEvent( MWidget* pOwner, MEvent* pEvent =NULL)
	{
		m_pOwner = pOwner;

		if ( pEvent == NULL)
		{
			m_nPosX = 0;
			m_nPosY = 0;
			m_nWheel = 0;
			m_nKey = 0;
			m_bShift = false;
			m_bCtrl = false;
			m_nHitTest = 0;
		}
		else
		{
			m_nPosX = pEvent->Pos.x;
			m_nPosY = pEvent->Pos.y;
			m_nWheel = pEvent->nDelta;
			m_nKey = pEvent->nKey;
			m_bShift = pEvent->bShift;
			m_bCtrl = pEvent->bCtrl;
			m_nHitTest = pEvent->nHitTest;
		}
	}

	MWidget* GetOwner()								{ return m_pOwner;								}
	MWLua::ret_int2 MEventArgs::GetCursorPos()		{ return MWLua::ret_int2( m_nPosX, m_nPosY);	}
	int GetItemIndex() const						{ return m_nItemIndex;							}
	int GetItemSubItem() const						{ return m_nItemSubItem;						}
	void SetItemRect( int x, int y, int w, int h)	{ m_ItemRect = MRECT( x, y, w, h);				}
	MWLua::ret_int4 GetItemRect() const				{ return MWLua::ret_int4( m_ItemRect.x, m_ItemRect.y, m_ItemRect.w, m_ItemRect.h);	}
	void SetUserData( int nData)					{ m_dwUserData = nData;							}
	int GetUserData() const							{ return m_dwUserData;							}
	void SetUserArgument( const char* strArg)		{ m_strArgument = strArg;						}
	const char* GetUserArgument() const				{ return m_strArgument.c_str();					}
};

class MWidget;

class MDragEventArgs
{
protected:
	MWidget*		m_pSender;
	MDragData*		m_pDragData;
	MPOINT			m_ptPoint;
	bool			m_bDropped;


public:
	MDragEventArgs()
	{
		m_pSender = NULL;
		m_pDragData = NULL;
		m_ptPoint = MPOINT( 0, 0);
		m_bDropped = false;
	}

	void Set( MWidget* pSender, MDragData* pDragData, MPOINT& ptPoint, bool bDropped)
	{
		m_pSender = pSender;
		m_pDragData = pDragData;
		m_ptPoint = ptPoint;
		m_bDropped = bDropped;
	}

	MWidget* GetSender();
	void AddDragData( const char* szText, const char* szImage, int nItemData);
	void SetFont( const char* szFont);
	void SetImageSize( int width, int height);
	void SetColor( int r, int g, int b);
	void SetHotSpot( int px, int py);
	void SetOpacity( float opacity);
	int GetItemCount() const;
	bool IsEmpty() const;
	void SetItemText( int nIndex, const char* szText);
	const char* GetItemText( int nIndex) const;
	void SetItemImage( int nIndex, const char* szImage);
	const char* GetItemImage( int nIndex) const;
	int GetItemData( int nIndex) const;
	bool IsDropped() const						{ return m_bDropped;					}
	void SetEffect( const char* szEffect);
	const char* GetEffect();
};



class MPopupEventArgs
{
protected:
	MWidget*		m_pOwner;
	MPOINT			m_ptPoint;

public:
	MPopupEventArgs() : m_pOwner( NULL), m_ptPoint( MPOINT( 0, 0))		{}

	MWidget* GetOwner();
	void Set( MPOINT& ptPoint, MWidget* pOwner);
	MPOINT& GetPoint()								{ return m_ptPoint;		}
};


class MEventHandler
{
protected:
	MListener*		m_pListener;
	set<MEventMsg>	m_ScriptEvents;
	bool			DoScriptEvent( string& strName, MEventMsg nMsg, const char* szArgs);
public:
	MEventHandler() : m_pListener(NULL) {}
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
	bool OnScriptEvent(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
	bool OnScriptEvent( string& strName, MEventMsg nMsg, const char* szArgs=NULL);
	void SetListener(MListener* pListener) { m_pListener = pListener; }
	MListener* GetListener() { return m_pListener; }
	bool ExistScriptEvent(MEventMsg nMsg);
	void SetFlagScriptEvents(MEventMsg nMsg) { m_ScriptEvents.insert(nMsg); }
	bool Empty() { return m_ScriptEvents.empty(); }
};

const char* FindEventNameFromMsg(MEventMsg nEventName);
MEventMsg FindEventMsgFromName(const char* szEventName);

} // namespace mint3