//
// MINT ( MAIET In-house wiNdows sysTem )
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "MDrawContext.h"
#include "MintTypes.h"
#include "MEvent.h"
#include "MEventHandler.h"
#include "MWLua.h"
#include <string>
#include <list>
using namespace std;




namespace mint3
{

// Message process
#define MNOTPROCESSED		0
#define MTRANSLATED			1
#define MRESULT				unsigned int

/// Draw state
#define MDS_NOTDRAW			0
#define MDS_DRAW			1

// Look
#define DECLARE_DEFAULT_LOOK(_CLASS)		{ static _CLASS look;  m_pLookAndFill = &look; }

#define MDEFAULT_TOOLTIPLOOK				"__default_tooltip_look"
#define MDEFAULT_POPUPMENULOOK				"__default_popmenu_look"


class MWidget;
class MLookBase;
class MDragData;
class MScrollBar;
class MPopupMenu;
class MResourceMap;




// 위젯 파트 정의(스트링형)
#define MWP_BORDER			"border"				///< Border
#define MWP_TITLEBAR		"titlebar"				///< Title bar
#define MWP_ITEMBKGRND		"itembkgrnd"			///< Item background
#define MWP_CARET			"caret"					///< Caret
#define MWP_IMECARET		"imecaret"				///< IME caret
#define MWP_SELECTBAR		"selectbar"				///< Select bar
#define MWP_COLUMNHEADER	"columnheader"			///< Column header
#define MWP_UPARROW			"uparrow"				///< 위쪽 화살표
#define MWP_DOWNARROW		"downarrow"				///< 아래쪽 화살표
#define MWP_LEFTARROW		"leftarrow"				///< 왼쪽 화살표
#define MWP_RIGHTARROW		"rightarrow"			///< 오른쪽 화살표
#define MWP_THUMB			"thumb"					///< Thumb 버튼
#define MWP_VERTTRACK		"verttrack"				///< 수직 트랙
#define MWP_HORZTRACK		"horztrack"				///< 수평 트랙
#define MWP_PROGRESSBAR		"progressbar"			///< 진행막대


// 위젯 상태 정의(스트링형)
#define MWS_NORMAL			"normal"
#define MWS_FOCUS			"focus"
#define MWS_DOWN			"down"
#define MWS_DISABLE			"disable"


// 위젯 상태 번호 정의(정수형)
#define MWSN_NORMAL			0
#define MWSN_FOCUS			1
#define MWSN_DOWN			1
#define MWSN_DISABLE		2


// Hit test
#define MHTNOWHERE			0
#define MHTCLIENT			1
#define MHTCAPTION			2
#define MHTSIZEFIRST		10
#define MHTLEFT				11
#define MHTRIGHT			12
#define MHTTOP				13
#define MHTTOPLEFT			14
#define MHTTOPRIGHT			15
#define MHTBOTTOM			16
#define MHTBOTTOMLEFT		17
#define MHTBOTTOMRIGHT		18
#define MHTSIZELAST			19


// Cursor
#define MCURSOR_ARROW		"arrow"
#define MCURSOR_WAIT		"wait"
#define MCURSOR_IBEAM		"ibeam"
#define MCURSOR_SIZENWSE	"sizenwse"
#define MCURSOR_SIZENESW	"sizenesw"
#define MCURSOR_SIZEWE		"sizewe"
#define MCURSOR_SIZENS		"sizens"
#define MCURSOR_SIZEALL		"sizeall"
#define MCURSOR_HAND		"hand"




/// class : MWidget
class MWidget  : public MListener 
{
	friend class Mint;
	friend class MEventHandler;
	friend class MScriptManager;
	friend class MLookBase;



/// Member variables
protected:
	/// Parent and child
	MWidget*			m_pParent;			///< Parent Widget
	list< MWidget*>		m_Children;			///< Children Widgets
	list< MWidget*>		m_MessageFilter;	///< Message hooking
	MWidget*			m_pWndDisable;		///< Windows disabled because of BeginModalState


	/// Default variables
	MLookBase*			m_pLookAndFill;		///< Look and fill
	string				m_strName;			///< 인스턴스 Name
	string				m_strText;			///< Text
	string				m_strToolTip;		///< 툴팁 메시지
	MPOINT				m_ptToolTipOffset;	///< 툴팁 위치 Offset
	MRECT				m_Rect;				///< Rect in Parent Widget
	MAnchorStyle		m_Anchor;			///< Parent Widget에 비례해서 위치하는가?

	bool				m_bModalState;		///< 모달 여부
	int					m_nModalResult;		///< 모달 결과 값
	bool				m_bShow;			///< Visible Flag
	bool				m_bEnable;			///< Enable
	bool				m_bEnableFocus;		///< Focus Enable
	float				m_fOpacity;			///< 투명도(0.0=투명, 1.0=불투명)
	bool				m_bShowBorder;		///< Border 표시 여부
	bool				m_bZOrderChangable;	///< Z Order Change by Button Click
	bool				m_bStatic;			///< 정적 위젯인지 여부

	bool				m_bLButtonDown;
	bool				m_bRButtonDown;


	/// Event and script
	MEventHandler		m_EventHandler;
	bool				m_bDeclToScript;


	/// Special state widget
	static MWidget*		m_pCapturedWidget;
	static MWidget*		m_pFocusedWidget;
	static MWidget*		m_pMouseOverWidget;


	// Lua event and helper
	static MEventArgs			m_EventArgs;
	static MDragEventArgs		m_DragEventArgs;
	static MPopupEventArgs		m_PopupEventArgs;

	
	// Running events
	static bool			m_RunEventOnPosition;
	static bool			m_RunEventOnSize;




/// Member functions
public:
	/// Constructor and Destructor
	MWidget( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~MWidget();


	/// 이벤트를 처리할 핸들러 호출
	MRESULT Event( MEvent* pEvent);
	bool DispatchModalWnd( MEvent* pEvent, MRESULT* nResult);
	bool DispatchMouse( MEvent* pEvent, MRESULT* nResult);
	bool DispatchKey( MEvent* pEvent, MRESULT* nResult);

	/// 단축키를 처리할 핸들러 호출
	MRESULT EventHotKey( unsigned int nHotKey);

	/// Loop가 돌때마다 Widget System에서 처리할 일이 있을때 실행한다.
	void Run( DWORD dwCurrTime =0);

	/// 이 Widget을 비롯한 Child Widget을 그린다.
	void Draw( MDrawContext* pDC);


	// 위젯의 상태를 구한다
	virtual const char* GetState();
	virtual int GetStateNum();


	/// 부모 위젯을 설정한다.
	void SetParent( MWidget* pParent);
	MWidget* GetParent()									{ return m_pParent;								}

	/// 자식 위젯 갯수를 얻어낸다.
	size_t GetChildCount()									{ return m_Children.size();						}

	/// 자식 리스트를 얻는다
	list< MWidget*>& GetChilList()							{ return m_Children;							}

	/// 자식 위젯을 얻어낸다.
	MWidget* GetChild( int i);

	/// 자식 위젯의 인덱스를 얻어낸다.
	int GetChildIndex( MWidget* pWidget);



	/// 해당 좌표의 위젯을 찾음
	MWidget* WindowFromPoint( MPOINT& pt, bool bEnFocus =false);
	MWidget* ToolTipWindowFromPoint( MPOINT& pt);



	// 모달 윈도우 실행/취소
	virtual void BeginModalState();
	virtual void EndModalState();

	// 모달 상태를 구함
	bool InModalState() const								{ return m_bModalState;							}

	// 모달 결과값을 구함
	int GetModalResult() const								{ return m_nModalResult;						}



	/// 리스너
	virtual void SetListener( MListener* pListener)			{ m_EventHandler.SetListener(pListener);		}
	virtual MListener* GetListener()						{ return m_EventHandler.GetListener();			}

	
	
	/// Child Widget 추가 및 삭제
	void AddChild( MWidget* pWidget);
	void RemoveChild( MWidget* pWidget);
	void DeleteChildren();
	void RemoveFromParent();


	/// 메시지 후킹
	void AddMessageFilter( MWidget* pHooker);
	void DeleteMessageFilter( MWidget* pHooker);


	/// Look and fill
	void SetLookAndFill( MLookBase* pLook);
	void SetLookAndFill( const char* szLookAndFill);
	MLookBase* GetLookAndFill()								{ return m_pLookAndFill;						}
	bool HasLookAndFill()									{ return ( m_pLookAndFill ? true : false);		}

	/// 위젯 인스턴스 이름 얻기
	string& GetName()										{ return m_strName;								}

	/// 위젯 텍스트 설정
	virtual void SetText( const char* szText);
	virtual const char* GetText()							{ return m_strText.c_str();						}

	/// 툴팁 메시지 설정 및 구함
	virtual void SetToolTipText( const char* szTooltip)		{ m_strToolTip = szTooltip;						}
	virtual const char* GetToolTipText() const				{ return m_strToolTip.c_str();					}
	MPOINT GetToolTipOffset() const							{ return m_ptToolTipOffset;						}
	void SetToolTipOffset( MPOINT ptOffset)					{ m_ptToolTipOffset = ptOffset;					}

	/// Parent Widget의 Local 좌표계에서 Rect 지정하기
	void SetRect( int x, int y, int w, int h);
	void SetRect( MRECT& r)									{ SetRect(r.x, r.y, r.w, r.h);					}
	MRECT GetRect() const									{ return m_Rect;								}

	/// 위젯의 클라이언트 영역을 구함
	MRECT GetClientRect();

	/// 위젯의 작업 영역을 구함
	MRECT GetWorkRect();
	MRECT GetWorkRectOffset();

	/// 위젯의 배경 영역을 구함
	MRECT GetBkgrndRect();
	MRECT GetBkgrndRectOffset();

	/// 위젯의 원래 클라이언트 영역을 구함(삭제 예정)
	MRECT GetInitialClientRect() const						{ return MRECT(0, 0, m_Rect.w, m_Rect.h);		}

	/// 위젯의 스크린 좌표를 구함
	MRECT GetWindowRect();

	/// Anchor 정보 지정하기
	void SetAnchor( MAnchorStyle& anchor)					{ m_Anchor = anchor;							}
	MAnchorStyle GetAnchor() const							{ return m_Anchor;								}

	/// Parent Widget의 Local 좌표계에서 Position 지정하기
	void SetPosition( int x, int y);
	void SetPosition( MPOINT& p)							{ SetPosition( p.x, p.y);						}
	MPOINT GetPosition() const								{ return MPOINT( m_Rect.x, m_Rect.y);			}

	/// Size 지정하기
	void SetSize( int w, int h);
	void SetSize( MSIZE& s)									{ SetSize( s.w, s.h);							}
	MSIZE GetSize() const									{ return MSIZE( m_Rect.w, m_Rect.h);			}

	/// 위젯을 보이거나 감춤
	bool Show( bool bShow =true);
	void Hide()												{ Show( false);									}
	bool GetShow() const									{ return m_bShow;								}
	virtual bool IsVisibled();

	/// 위젯을 활성화 시킨다.
	void Enable( bool bEnable =true);
	bool GetEnable() const									{ return m_bEnable;								}
	virtual bool IsEnabled();

	/// 포커싱
	void SetEnableFocus( bool bEnable)						{ m_bEnableFocus = bEnable;						}
	bool IsEnableFocus() const								{ return m_bEnableFocus;						}

	/// 위젯을 이동 가능한지 설정
	virtual void SetMovable( bool bMovable)					{}
	virtual bool IsMovable()								{ return false;									}

	/// 위젯의 사이즈를 변경 가능한지 설정
	virtual void SetResizable( bool bResize)				{}
	virtual bool IsResizable()								{ return false;									}

	/// 투명도 설정
	void SetOpacity( float fOpacity)						{ m_fOpacity = max( 0.0f, min( 1.0f, fOpacity));	}
	float GetOpacity()										{ return m_fOpacity;							}

	/// Border 표시 설정
	void ShowBorder( bool bShow)							{ m_bShowBorder = bShow;						}
	bool IsShowBorder() const								{ return m_bShowBorder;							}

	/// Border 표시 설정
	void SetStatic( bool bStatic)							{ m_bStatic = bStatic;							}
	bool IsStatic() const									{ return m_bStatic;								}

	/// 스크롤바
	virtual MScrollBar* GetScrollBar()						{ return NULL;									}

	/// 드래그 앤 드롭
	virtual void EnableDragAndDrop( bool bEnable)			{}
	virtual bool IsEnableDragAndDrop() const				{ return false;									}


	// 통지 메시지를 보냄
	bool SendNotify( MWidget* pSender, MEventMsg nMsg);

	// 유저 argument를 보냄
	bool SendUserArguments( const char* szArgs, bool bSendChild =false);


	/// 위젯에 포커스를 설정
	void SetFocus();
	void ReleaseFocus();
	virtual bool IsFocus()									{ return ((MWidget::m_pFocusedWidget == this) ? true : false);		}
	MWidget* GetFocus()										{ return MWidget::m_pFocusedWidget;									}

	/// 위젯이 메시지를 캡쳐함
	void SetCapture()										{ MWidget::m_pCapturedWidget = this;								}
	void ReleaseCapture()									{ MWidget::m_pCapturedWidget = NULL;								}
	bool IsCapture()										{ return ((MWidget::m_pCapturedWidget == this) ? true : false);		}
	MWidget* GetCapture()									{ return MWidget::m_pCapturedWidget;								}

	/// 위젯에 마우스가 위치함
	void SetMouseOver();
	void ReleaseMouseOver();
	bool IsMouseOver()										{ return ((MWidget::m_pMouseOverWidget == this) ? true : false);	}
	bool IsMouseIncluded();
	MWidget* GetMouseOver()									{ return MWidget::m_pMouseOverWidget;								}	


	// 타이머 설정
	void SetTimer( int nElapse, DWORD dwEventData =0);
	void KillTimer();


	// 이벤트 스크립트 등록
	void BindScriptEvent( string& strEventName, string& strScript);
	bool IsBindedScript()									{ return m_EventHandler.Empty();				}

	void DeclToSctipt()										{ m_bDeclToScript = true;						}
	bool IsDeclSctipt()										{ return m_bDeclToScript;						}



	// Z-Order
	virtual void SetAlwaysTop( bool bSet)					{}
	virtual bool GetAlwaysTop() const						{ return false;									}
	void BringToTop();


	// 위젯이 레이어 타입인지 여부를 구함
	virtual bool IsLayered() const							{ return false;									}


protected:
	/// Local Coordinate를 이용한 Local Event로 변환
	void MakeLocalEvent( MEvent* pLoalEvent, const MEvent* pEvent);

	/// Event에서 Resize부분 처리
	bool EventResize( MEvent* pEvent);

	/// 현재 위젯을 그리는게 가능한 상태인지 여부를 구함
	virtual int GetDrawState();

	// 마우스 상태 초기화
	virtual void InitMouseState();




/// Event caller
public:
	void EventCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs =NULL)	{ OnCommand( pWidget, nMsg, szArgs);			}
	void EventTimer( DWORD dwEventData)												{ OnTimer( dwEventData);						}
	void EventResizeLayer();
	void EventLoaded()																{ return OnLoaded();							}
	bool EventDragBegin( MDragData* pDragData, MPOINT& pt);
	bool EventDragEnd( MDragData* pDragData, MPOINT& pt, bool bDropped)				{ return OnDragEnd( pDragData, pt, bDropped);	}
	bool EventDrop( MWidget* pSender, MDragData* pDragData, MPOINT& pt)				{ return OnDrop( pSender, pDragData, pt);		}
	MWidget* EventToolTip()															{ return OnToolTip();							}
	void EventInitPopupMenu( MPopupMenu* pPopupMenu)								{ OnInitPopupMenu( pPopupMenu);					}
	void EventDrawItemBkgrnd( MDrawContext* pDC, int index, int subitem, MRECT& r);
	bool EventPreDrawItem( MDrawContext* pDC, int index, int subitem, MRECT& r);
	void EventDrawItem( MDrawContext* pDC, int index, int subitem, MRECT& r);




/// Event handler
protected:
	virtual bool OnPreFilterMessage( MEvent* pEvent);
	virtual MRESULT OnPreTranslateMsg( MEvent* pEvent);
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener);
	virtual bool OnQueryHotKey( unsigned int nHotKey);
	virtual void OnHotKey( unsigned int nHotKey);
	virtual bool OnCommand( MWidget* pWidget, MEventMsg nMsg, const char* szArgs =NULL);
	virtual void OnLoaded();
	virtual void OnChangedLookAndFill();
	virtual void OnRun( DWORD dwCurrTime);
	virtual void OnPreDraw( MDrawContext* pDC);
	virtual void OnDrawBorder( MDrawContext* pDC);
	virtual void OnDrawItemBkgrnd( MDrawContext* pDC, int index, int subitem, MRECT& r);
	virtual bool OnPreDrawItem( MDrawContext* pDC, int index, int subitem, MRECT& r);
	virtual void OnDrawItem( MDrawContext* pDC, int index, int subitem, MRECT& r);
	virtual void OnDraw( MDrawContext* pDC);
	virtual void OnShow( bool bShow);
	virtual void OnEnable( bool bEnabled);
	virtual	void OnActivate( bool bActivate);
	virtual void OnSetFocus();
	virtual void OnKillFocus();
	virtual void OnPosition( int x, int y);
	virtual void OnSize( int w, int h);
	virtual void OnLButtonDown( MEvent* pEvent);
	virtual void OnLButtonUp( MEvent* pEvent);
	virtual void OnRButtonDown( MEvent* pEvent);
	virtual void OnRButtonUp( MEvent* pEvent);
	virtual void OnMouseMove( MEvent* pEvent);
	virtual void OnClick( MEvent* pEvent);
	virtual void OnRClick( MEvent* pEvent);
	virtual void OnDblClick( MEvent* pEvent);
	virtual void OnMouseWheel( MEvent* pEvent);
	virtual void OnKeyDown( MEvent* pEvent);
	virtual void OnKeyUp( MEvent* pEvent);
	virtual void OnChar( MEvent* pEvent);
	virtual void OnScrollBarChanged( int nPos);
	virtual void OnSelChanged();
	virtual void OnValueChanged();
	virtual void OnItemClick( MEvent* pEvent, int nIndex);
	virtual void OnItemDblClick( MEvent* pEvent, int nIndex);
	virtual void OnItemRClick( MEvent* pEvent, int nIndex);
	virtual bool OnDragBegin( MDragData* pDragData, MPOINT& pt);
	virtual bool OnDragEnd( MDragData* pDragData, MPOINT& pt, bool bDropped);
	virtual bool OnDrop( MWidget* pSender, MDragData* pDragData, MPOINT& pt);
	virtual MWidget* OnToolTip();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void OnEnter();
	virtual void OnLeave();
	virtual bool OnSetCursor( MEvent* pEvent, string* pCursor);
	virtual void OnInitPopupMenu( MPopupMenu* pPopupMenu);
	virtual void OnTimer( DWORD dwEventData);
	virtual int OnNcHitTest( MEvent* pEvent);
	virtual bool OnUserArgument( const char* szArgument);



/// Lua glue 함수
public:
	MWidget*			glueGetParent()								{ return GetParent();								}
	const char*			glueGetName()								{ return GetName().c_str();							}
	void				glueSetText( const char* szText);
	const char*			glueGetText()								{ return GetText();									}
	void				glueSetToolTip( const char* szToolTip);
	void				glueModifyToolTip();
	const char*			glueGetToolTip()							{ return GetToolTipText();							}
	void				glueSetPosition( int x, int y)				{ SetPosition( x, y);								}
	MWLua::ret_int2		glueGetPosition() const						{ return MWLua::ret_int2( m_Rect.x, m_Rect.y);		}
	void				glueSetSize( int w, int h)					{ SetSize( w, h);									}
	MWLua::ret_int2		glueGetSize() const							{ return MWLua::ret_int2( m_Rect.w, m_Rect.h);		}
	int					glueGetWidth() const						{ return m_Rect.w;									}
	int					glueGetHeight() const						{ return m_Rect.h;									}
	void				glueSetRect( int x, int y, int w, int h)	{ SetRect( x, y, w, h);								}
	MWLua::ret_int4		glueGetRect();
	MWLua::ret_int4		glueGetClientRect();
	MWLua::ret_int4		glueGetWindowRect();
	bool				glueShow( bool bShow)						{ return Show( bShow);								}
	bool				glueGetShow() const							{ return GetShow();									}
	bool				glueIsVisibled()							{ return IsVisibled();								}
	void				glueEnable( bool bEnable)					{ Enable( bEnable);									}
	bool				glueGetEnable() const						{ return GetEnable();								}
	bool				glueIsEnabled()								{ return IsEnabled();								}
	void				glueSetOpacity( float fOpacity)				{ SetOpacity( fOpacity);							}
	float				glueGetOpacity()							{ return GetOpacity();								}
	void				glueShowBorder( bool bShow)					{ ShowBorder( bShow);								}
	bool				glueIsShowBorder() const					{ return IsShowBorder();							}
	int					glueSetScrollValue( int nValue);
	int					glueGetScrollValue();
	void				glueSetScrollThumbLen( int nLength);
	void				glueSetFocus()								{ SetFocus();										}
	void				glueSetStatic( bool bStatic)				{ SetStatic( bStatic);								}
	bool				glueIsStatic()								{ return IsStatic();								}
	bool				glueIsFocus()								{ return IsFocus();									}
	bool				glueIsCapture()								{ return IsCapture();								}
	bool				glueIsMouseOver()							{ return IsMouseOver();								}
	bool				glueIsMouseIncluded()						{ return IsMouseIncluded();							}
	void				glueBringToTop()							{ BringToTop();										}
	bool				glueTrackPopupMenu( const char* szPopupMenu);
	void				glueDoModal()								{ return BeginModalState();							}
	void				glueSetTimer( int nElapse, int nEventData)	{ SetTimer( nElapse, nEventData);					}
	void				glueKillTimer()								{ KillTimer();										}
	bool				glueSendUserArgument( const char* szArgument, bool bSendChild);
	void				glueAttachDragItem( const char* szText, const char* szImage, int nUserData);


#define MINT_WIDGET	"Widget"
	virtual const char* GetClassName()				{ return MINT_WIDGET;		}



#ifndef _PUBLISH
	bool				m_bShowRect;
	void ShowRect( bool bShow)						{ m_bShowRect = bShow;		}
#endif

};



/// 로컬 위치를 전역 위치로 변환
extern MPOINT MClientToScreen( MWidget* pWidget, MPOINT& p);
extern MRECT MClientToScreen( MWidget* pWidget, MRECT& p);

/// 전역 위치를 로컬 위치로 변환
extern MPOINT MScreenToClient( MWidget* pWidget, MPOINT& p);
extern MRECT MScreenToClient( MWidget* pWidget, MRECT& p);





/// &가 있는 위치를 알려준다. szText가 NULL이면 m_szText에서 &의 위치를 찾는다.
int GetAndPos(const char* szText);

/// & 뒤에 붙어있는 문자를 얻어낸다.
char GetAndChar(const char* szText);

/// &문자 제거
int RemoveAnd(char* szText);

/// &문자 제거
int RemoveAnd(char* szRemovedText, const char* szText);

/// &문자 제거
int RemoveAnd(char* szRemovedFrontText, char* cUnderLineChar, char* szRemovedBackText, const char* szText);

/// 현재 커서 위치 얻기
MPOINT GetCursorPosition(void);


} // namespace mint3