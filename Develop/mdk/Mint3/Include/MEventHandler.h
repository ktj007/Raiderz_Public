#pragma once

#include <set>
#include <string>
using namespace std;

namespace mint3 {

class MWidget;

/// Listener에게 전달되는 메세지
enum MEventMsg
{
	MEVENT_UNKNOWN = 0,

	// Widget
	MEVENT_SHOW,				///< 적용됨. 위젯이 보여질때 발생하는 메세지
	MEVENT_HIDE,				///< 적용됨. 위젯이 숨겨질때 발생하는 메세지
	MEVENT_CLICK,				///< 적용됨. 클릭될 때 발생하는 메세지
	MEVENT_RCLICK,
	MEVENT_ENTER,				///< 적용됨. 포커스될 때 발생하는 메세지
	MEVENT_LEAVE,				///< 적용됨. 포커스가 사라질 때 발생하는 메세지
	MEVENT_DRAW,				///< 적용됨. 그릴때 발생하는 매세지
	MEVENT_CHAR,
	MEVENT_DOUBLE_CLICK,
	MEVENT_KEY_DOWN,			///< 키를 입력했을 때 발생하는 메세지
	MEVENT_KEY_UP,				///< 키를 뗄때 발생하는 메세지
	MEVENT_MOUSE_DOWN,			///< 마우스를 눌렀을 때 발생하는 메세지
	MEVENT_MOUSE_UP,			///< 마우스를 뗄 때 발생하는 메세지
	

//	MEVENT_SIZE,

	// Timer
	MEVENT_TIMER_TICK,

	// TabCtrl
	MEVENT_TABCTRL_TAB_SELECTED,	///< 탭 페이지가 선택됨. 인자 - 선택된 페이지의 인덱스
	MEVENT_BUTTON_CHECK_CHANGED,	///< 버튼의 체크가 변경된 후 발생하는 메세지

	MEVENT_DROP,
	MEVENT_MOUSE_WHEEL,			///<

	// Button
	MEVENT_BUTTON_CLICK,		///< 버튼이 클릭될때 Listener에게 발생되는 메세지
	MEVENT_BUTTON_IN,			///< 버튼 위에 커서가 올려 질때
	MEVENT_BUTTON_OUT,			///< 버튼 위에 커서가 빠질때 올려 질때

	// Edit
	MEVENT_EDIT_KEYDOWN,		///< 키를 입력했을때
	MEVENT_EDIT_CHAR,			///< 키를 입력했을때
	MEVENT_EDIT_ENTER,			///< Enter키를 입력했을때
	MEVENT_EDIT_ESC,			///< ESC키를 입력했을때

	// List
	MEVENT_LIST_VALUE_CHANGED,	///< value가 바뀔때 떨어지는 메세지



	// ComboBox
	MEVENT_CB_SELECTED_INDEX_CHANGED,		///< 선택한 항목이 바뀌었을 때

	// Frame
	MEVENT_FRAME_CLOSE,
	MEVENT_FRAME_MINIMIZE,

	// ListBox
	MEVENT_LB_ITEM_SEL,
	MEVENT_LB_ITEM_SEL2,
	MEVENT_LB_ITEM_DBLCLK,
	MEVENT_LB_ITEM_SELLOST,
	MEVENT_LB_ITEM_DEL,
	MEVENT_LB_ITEM_START,		///< 시작아이템이 바뀔때
	MEVENT_LB_ITEM_CLICKOUT,

	// TextArea
	MEVENT_TEXTAREA_ENTER_VALUE,
	MEVENT_TEXTAREA_ESC_VALUE,
	MEVENT_TEXTAREA_LINKEDTEXT,

	// MsgBox
	MEVENT_MSGBOX_OK,
	MEVENT_MSGBOX_CANCEL,
	MEVENT_MSGBOX_YES,
	MEVENT_MSGBOX_NO,

	// FileDialog
	MEVENT_FILEDIALOG_OK,
	MEVENT_FILEDIALOG_CANCEL,

	

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
	MEventArgs()
	{
		m_nKeyValue = 0;
		m_nMouseX = m_nMouseY = m_nMouseDelta = 0;
		m_bCtrl = false;
		m_bShift = false;
	}
	~MEventArgs() {}
	void SetFromEvent(MEvent* pEvent)
	{
		m_nKeyValue		= pEvent->nKey;
		m_nMouseX		= pEvent->Pos.x;
		m_nMouseY		= pEvent->Pos.y;
		m_nMouseDelta	= pEvent->nDelta;
		m_bCtrl			= pEvent->bCtrl;
		m_bShift		= pEvent->bShift;
	}

	unsigned int	m_nKeyValue;		///< KeyDown 또는 KeyUp 이벤트에 대한 키보드 값
	int				m_nMouseX;
	int				m_nMouseY;
	int				m_nMouseDelta;			// Wheel Mouse Delta
	bool			m_bCtrl;
	bool			m_bShift;
};


class MDragEventArgs
{
private:
	char m_szString[256];
	char m_szItemString[256];
public:
	MDragEventArgs()
	{
		m_nX = m_nY = 0;
		m_szString[0] = 0;
		m_szItemString[0] = 0;
	}
	~MDragEventArgs() {}
	void Set(MWidget* pSender, int x, int y, bool bCtrl, const char* szString, const char* szItemString)
	{
		this->m_pSender = pSender;
		this->m_nX = x;
		this->m_nY = y;
		this->m_bCtrl = bCtrl;

		strcpy_s(m_szString, szString);
		strcpy_s(m_szItemString, szItemString);
	}

	MWidget*	m_pSender;
	int			m_nX;
	int			m_nY;
	bool		m_bCtrl;

	const char* GetString()		{ return m_szString; }
	const char* GetItemString()	{ return m_szItemString; }
};


class MLinkedTextArgs
{
private:
	char m_szString[ 256];
	char m_szArgument[ 256];
public:
	MLinkedTextArgs()
	{
		m_szString[ 0] = 0;
		m_szArgument[ 0] = 0;
	}

	void Set( const char* szString, const char* szArgument)
	{
		strcpy_s( m_szString, szString);
		strcpy_s( m_szArgument, szArgument);
	}

	const char* GetString()			{ return m_szString;		}
	const char* GetArgument()		{ return m_szArgument;		}
};


class MEventHandler
{
protected:
	MListener*		m_pListener;
	set<MEventMsg>	m_ScriptEvents;
	void			DoScriptEvent(MWidget* pWidget, MEventMsg nMsg, const char* szArgs);
public:
	MEventHandler() : m_pListener(NULL) {}
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL);
	void SetListener(MListener* pListener) { m_pListener = pListener; }
	MListener* GetListener() { return m_pListener; }
	bool ExistScriptEvent(MEventMsg nMsg);
	void SetFlagScriptEvents(MEventMsg nMsg) { m_ScriptEvents.insert(nMsg); }
};

MEventMsg FindEventMsgFromName(const char* szEventName);

} // namespace mint3