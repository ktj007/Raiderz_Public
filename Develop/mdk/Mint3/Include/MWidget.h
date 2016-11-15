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

namespace mint3 {


class MWidget;
class MResourceMap;

/// 위젯의 Z 순서
enum MZOrder{
	MZ_TOP = 0,				///< 맨 앞으로
	MZ_BOTTOM = 100,		///< 맨 뒤로
};

// 위젯 타입
enum MWidgetLookType
{
	MWIDGET_MBUTTONLOOK,			// 버튼 Look
	MWIDGET_MFRAMELOOK,				// 프레임 Look
	MWIDGET_MBBUTTONLOOK			// 이미지 버튼 Look
};

struct MAnchorStyle
{
	bool bLeft;
	bool bRight;
	bool bTop;
	bool bBottom;
	MAnchorStyle(void)	{ bLeft = true; bRight = false; bTop = true; bBottom = false; }
	MAnchorStyle(bool left, bool right, bool top, bool bottom)
	{ bLeft = left; bRight = right; bTop = top; bBottom = bottom; }
};



#define MWIDGET_TEXT_LENGTH			256			///< 위젯 텍스트 최대 길이
#define MWIDGET_WINDOW_MIN_SIZE_W	50			// 위젯 창 최소 사이즈(Width)
#define MWIDGET_WINDOW_MIN_SIZE_H	50			// 위젯 창 최소 사이즈(Height)

#define WIDGET_UI_LOOK_TYPE(TY)			virtual MWidgetLookType GetUILookType()	{ return TY; }

/// 기본 위젯 클래스
class MWidget  : public MListener 
{
private:
	friend class MEventHandler;

	bool				m_bEnable;			///< Enable
	bool				m_bFocusEnable;		///< Focus Enable

//	MListener*			m_pListener;		///< Listener
	string				m_strTooltip;		///< 툴팁 메시지
	string				m_strHelpMsg;		///< 도움말 메시지(Mint의 설정에서 보여줄지 말지를 결정할 수 있는 메시지)

	int					m_nAndPos;
	int					m_nAccelerator;		///< Accelerator Key
protected:
	bool				m_bVisible;			///< Visible Flag

	list<MWidget*>		m_Children;			///< Children Widgets

	MWidget*			m_pParent;			///< Parent Widget
	list<MWidget*>		m_Exclusive;		///< Exclusive Child Widget Stack ( for Modal Support )
	MEventHandler		m_EventHandler;
	bool				m_bDeclToScript;


	MCursor*			m_pCursor;			///< Default Cursor of this Widget
	MFont*				m_pFont;			///< Default Font of this Widget

	static MWidget*		m_pCapturedWidget;	///< SetCapture(), ReleaseCapture()
	static MWidget*		m_pFocusedWidget;	///< SetFocus()

	bool				m_bZOrderChangable;	///< Z Order Change by L-Button Click
	bool				m_bResizable;		///< Resizable?

	int					m_nResizeSide;		///< 0: N/A   1: Up  2: Right  4: Down  8: Left(BitFlag)

	float				m_fOpacity;			///< 불투명도(0 : 완전투명, 1 : 완전불투명)

	bool				m_bClipByParent;	///< Parent Widget에 의해 Clip되는가?

	MAlignmentMode		m_BoundsAlignment;	///< m_Rect에 대한 Alignment
	MAnchorStyle		m_Anchor;			///< Parent Widget에 비례해서 위치하는가?

	bool				m_bLButtonDown;
	bool				m_bRButtonDown;

// 	bool				m_bUseRatioToStandardScreenSize;	///< 화면 비율 사용 유(true)/무(flase, Default)
// 	float				m_fRatioToStandardScreenWidth;	///< 화면 가로 비율
// 	float				m_fRatioToStandardScreenHeight; ///< 화면 세로 비율

	bool				m_bIterating;		///< 업데이트 이터레이팅 중인가
	
	struct ZORDERRESERVED {
		MWidget* pWidget;
		MZOrder zOrder;
	};
	list< ZORDERRESERVED > m_reservedZOrder;

public:

	string				m_strName;			///< 인스턴스 Name
//	char				m_szText[MWIDGET_TEXT_LENGTH];	///< Text
	string				m_strText;			///< Text

	MRECT				m_Rect;				///< Rect in Parent Widget
	
	int					m_nMinWidth, m_nMinHeight;	///< 위젯의 최소 크기

	bool				m_bEventAcceleratorCall;///< 키입력이벤트로 불렸는지 감지
protected:
	// Only for Designer Mode
	bool				m_bEnableDesignerMode;	///< Designer Mode 활성화 플래그
	int					m_nDMDragWidget;		///< 위젯 Move 또는 Resize 플래그
	MPOINT				m_DMDragPoint;			///< 위젯 Move 또는 Resize 포지션
	bool				m_bModifiedByDesigner;	///< Designer에 의해 위치,크기가 수정된 경우
	bool				m_bAddedByDesigner;		///< Designer에 의해 추가된 경우
	int					m_nID;					///< 위젯의 ID

private:
	/// Local Coordinate를 이용한 Local Event로 변환
	void MakeLocalEvent(MEvent* pLoalEvent, const MEvent* pEvent);
	/// Event에서 Resize부분 처리
	bool EventResize(MEvent* pEvent);
	/// 스크립트에서 해제
	void ReleaseFromScript();

	/// 업데이트 중에는 zorder를 직접 바꾸면 안되고 예약을 해야합니다.
	void ReserveZOrder(MZOrder z);
	/// 예약된 zorder 를 실행
	void ExecuteReservedZOrder();

protected:
	/// 이벤트 핸들러 -----------------------------------------------
	virtual void OnEnabled(bool bEnabled);
	virtual void OnShow();
	virtual void OnHide();
	virtual void OnClick(MEvent* pEvent);
	virtual void OnRClick(MEvent* pEvent);
	virtual void OnDoubleClick(MEvent* pEvent);
	virtual void OnMouseDown(MEvent* pEvent);
	virtual void OnMouseUp(MEvent* pEvent);
	virtual void OnKeyDown(MEvent* pEvent);
	virtual void OnKeyUp(MEvent* pEvent);
	virtual void OnMouseWheel(MEvent* pEvent);
	virtual void OnItemSelected(MEvent* pEvent);
protected:
	/// Child Widget 맨 앞에 추가
	void InsertChild(MWidget* pWidget);
	/// Child Widget 추가
	void AddChild(MWidget* pWidget);
	/// Child Widget 삭제
	void RemoveChild(MWidget* pWidget);
	/// Parent로 부터 삭제
	void RemoveFromParent();

	// Accelerator를 szText의 &뒤에 나오는 문자로 한다.
	//void UseAcceleractorAndCharacter(char* szText);

	/// Exclusive Stack에 추가한다.
	void AddExclusive(MWidget* pWidget);
	/// Exclusive Stack에서 제거한다.
	bool RemoveExclusive(MWidget* pWidget);

	/// Runtime Loop
	virtual void OnRun(void);
	/// Draw Handler
	virtual void OnDraw(MDrawContext* pDC);
	/*
	/// Draw Handler
	/// @deprecated	3D Rendering Pass를 따로 처리하기 위한 코드로 더이상 사용하지 않는다.
	virtual void OnDraw3D(MDrawContext3D* pDC);
	*/
	/// Event Handler. Event가 처리되면 return true, 아니면 return false.
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);

	/// 핫키를 눌렀을때 불려지는 이벤트 핸들러
	virtual bool OnHotKey(int nID){ return false; }


	/// 포커스 핸들러
	virtual void OnSetFocus(void);
	virtual void OnReleaseFocus(void);

	/// SetSize(), SetBounds()호출에 의한 Event Handler
	virtual void OnSize(int w, int h);

	/// Tab Key
	virtual bool OnTab(bool bForward=true);

	/// Drop&Drop에 의해 놓여지는 핸들러
	virtual bool OnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString);

	/// 드랍이 실패 한경우
	virtual bool OnUnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString);

	/// Listener Event Handler ( 지정한 메세지를 수행했으면 return true를 한다. )
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs=NULL){ return false; }

	/// Anchor 설정에 따른 Children Resize
	/// @param w	new width
	/// @param h	new height
	void ResizeChildrenByAnchor(int w, int h);

	/// Bounds Alignment에 따른 위치값
	void GetBoundsAlignmentPosition(MPOINT* p, MAlignmentMode am, int w=-1, int h=-1);
public:
	/// Workspace가 변경될때 호출되는 함수
	//virtual void OnReSize();
	static MEventArgs		m_EventArgs;		///< 루아에서 사용하는 이벤트 인자값
	static MDragEventArgs	m_DragEventArgs;
	static MLinkedTextArgs	m_LinkedTextArgs;
public:
	/// @param szName	위젯 이름
	/// @param pParent	부모 위젯
	/// @param szName	메세지를 전달받을 리스너, 보통은 부모 위젯이 해당된다.
	MWidget(const char* szName=NULL, MWidget* pParent=NULL, MListener* pListener=NULL);
	virtual ~MWidget(void);

	/// Loop가 돌때마다 Widget System에서 처리할 일이 있을때 실행한다.
	void Run(void);
	/// 이 Widget을 비롯한 Child Widget을 그린다.
	void Draw(MDrawContext* pDC);
	/// 현재 다시 그린다.
	void Redraw(void);
	/// 이벤트를 처리할 핸들러 호출
	bool Event(MEvent* pEvent);
	/// 키보드 Accelerator 이벤트만 처리
	bool EventAccelerator(MEvent* pEvent);
	/// 디폴트 키(Enter, ESC) 핸들러 호출
	bool EventDefaultKey(MEvent* pEvent);

	/// 위젯을 보여준다.
	/// @param bVisible	Visible Flag
	/// @param bModal	Modal Dialog Flag
	virtual void Show(bool bVisible=true, bool bModal=false);
	/// 위젯을 감춘다.
	void Hide(void){ Show(false); }
	/// 위젯을 활성화 시킨다.
	/// @param bEnable	Enable Flag
	void Enable(bool bEnable=true);
	/// 위젯이 보이는가?
	bool IsVisible(void);
	/// 위젯이 활성화 되었는가?
	bool IsEnable(void);

	void SetResizable(bool bEnable);
	bool IsResizable(void);

	/// 리스너 지정
	/// @param pListener	지정할 리스너
	virtual void SetListener(MListener* pListener);
	/// 현재 리스너 얻기
	/// @return 현재 리스너
	virtual MListener* GetListener(void);


	/// 위젯 ID 얻기
	int GetID(void);
	/// 위제 ID 지정하기
	void SetID(int nID);

	/// 위젯 인스턴스 이름 얻기
	string& GetName();

	/// 위젯 텍스트(Name) 지정하기
	virtual void SetText(const char* szText);
	/// 위젯 텍스트(Name) 얻기
	virtual const char* GetText(void);

	/// 현재 위젯에 모든 마우스 메세지가 가도록 캡쳐
	void SetCapture(void);
	/// 현재 캡쳐된 위젯을 해제
	void ReleaseCapture(void);

	void SetFocusEnable(bool bEnable);
	bool IsFocusEnable(void);

	/// 위젯에 포커스를 위치시킨다. 포커스된 위젯에 키보드 메세지가 우선적으로 들어간다.
	void SetFocus(void);
	/// 위젯에 포커스를 해제시킨다.
	void ReleaseFocus(void);
	/// 위젯에 포커스를 가지고 있는가?
	virtual bool IsFocus(void);

	/// 부모 위젯을 설정한다.
	void SetParent(MWidget* pParent);
	/// 부모 위젯을 얻어낸다.
	MWidget* GetParent(void);
	/// 자식 위젯 갯수를 얻어낸다.
	int GetChildCount(void);
	/// 자식 위젯을 얻어낸다.
	/// @param i	자식 위젯 인덱스
	MWidget* GetChild(int i);
	/// 자식 위젯의 인덱스를 얻어낸다.
	/// @param	pWidget	자식 위젯
	/// @return	자식 위젯의 인덱스
	int GetChildIndex(MWidget* pWidget);
	/// Child Widget 삭제 및 메모리에서도 해제
	void DeleteChildren();
	bool IsEmptyChildWidgets() { return ((m_Children.empty()) ? true : false); }

	/// Exculsive 모드(Modal)로 전환한다. Exclusive 모드로 설정되어 있는 위젯이 모든 메세지를 독점한다.
	void SetExclusive(void);
	/// Exclusive 모드를 해제하나.
	void ReleaseExclusive(void);
	/// Exclusive Stack의 가장 마지막 위젯을 얻어낸다.
	MWidget* GetLatestExclusive(void);
	/// Child Widget이 Exclusive인지 테스트
	bool IsExclusive(MWidget* pWidget);

	/// 위젯에 커서를 설정한다.
	/// @param pCursor	위젯에 설정할 커서
	/// @return			이전에 설정되어 있던 커서
	MCursor* SetCursor(MCursor* pCursor);
	/// 현재 커서를 얻어낸다.
	MCursor* GetCursor(void);

	/// 위젯에 폰트를 설정한다.
	MFont* SetFont(MFont* pFont);
	/// 현재 폰트를 얻어낸다.
	MFont* GetFont(void);

	/// Size 지정하기
	/// @param w, h		가로 세로 크기
	void SetSize(int w, int h);
	/// Size 지정하기
	/// @param s		사이즈
	void SetSize(MSIZE& s);

	/// Parent Widget의 Local 좌표계에서 Poition 지정하기
	/// @param x, y		위치
	void SetPosition(int x, int y);
	/// Parent Widget의 Local 좌표계에서 Poition 지정하기
	/// @param p		위치
	void SetPosition(MPOINT& p);
	/// Parent Widget의 Local 좌표계에서 Initial Rect 지정하기
	void SetInitialBounds(MRECT& r);
	/// Parent Widget의 Local 좌표계에서 Rect 지정하기
	/// @param r		Rectangle
	void SetBounds(MRECT& r);
	/// Parent Widget의 Local 좌표계에서 Rect 지정하기
	/// @param x,y,w,h		Rectangle
	void SetBounds(int x, int y, int w, int h);
	/// Parent Widget의 Local 좌표계에서 Position 얻어내기
	MPOINT GetPosition(void);
	/// Parent Widget의 Local 좌표계에서 Rect 얻어내기
	MRECT GetRect(void);
	/// Anchor 정보 얻기
	MAnchorStyle& GetAnchor()			{ return m_Anchor;		}
	/// Anchor 정보 지정하기
	void SetAnchor(MAnchorStyle& anchor);
	/// 위치에대한 얼라인먼트 지정
	void SetBoundsAlignment(MAlignmentMode am, int w, int h);
	/// 위치에대한 얼라인먼트 얻기
	MAlignmentMode GetBoundsAlignment(void);

	void SetOpacity(float fOpacity);
	float GetOpacity();

	/// Screen 좌표계에서 Rect 얻어내기
	/// @deprecated		전역 좌표계는 혼란을 가져오므로 로컬 좌표계 함수들로 대체, 전역 좌표를 알고 싶으면 MClientToScreen(),MScreenToClient()를 이용
	MRECT GetScreenRect(void);

	/// 툴팁 메시지 설정 및 구함
	void SetTooltipMsg( const char* szToolTipMsg =NULL);
	const char* GetTooltipMsg();

	/// 도움말 메시지 설정 및 구함
	void SetHelpMsg( const char* szHelpMsg =NULL);
	const char* GetHelpMsg();

	/// Accelerator 지정
	void SetAccelerator(int a);
	/// Label에서 &다음에 나오는 문자로 Accelerator 지정
	void SetLabelAccelerator(void);
	/// 현재 Accelerator 얻기
	char GetLabelAccelerator(void);

	/// Widget의 커스터마이즈된 Client Rect
	virtual MRECT GetClientRect(void);
	/// Widget의 원래 Client Rect(0, 0, width, height)
	MRECT GetInitialClientRect(void);

	/// Child Widget 순서를 바꿔 그리는 순서를 조정한다.
	void SetZOrder(MZOrder z);

	/// 자손 Widget중 맨처음 발견된 Exclusive Widget 리턴
	MWidget* FindExclusiveDescendant(void);

	/// 위치로 위젯 검색
	MWidget* Find(int x, int y){ return Find(MPOINT(x, y)); }
	/// 위치로 위젯 검색
	MWidget* Find(MPOINT& p);
	MWidget* FindDropAble(MPOINT& p);

	/// 해당 ID의 Drag Object가 Drop가능한가?
	virtual bool IsDropable(MWidget* pSender){ return false; }
	bool Drop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString);
	bool UnDrop(MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString);

	bool GetVisible() { return m_bVisible; }

	/// 계층 구조를 포함한 이름 얻어내기 ( 예: Parent/Child )
	void GetHierarchicalName(char* szName);
	/// 계층 구조를 포함한 이름으로 위젯 찾기
	MWidget* FindWidgetByHierarchicalName(const char* szName);

	/// 현재 위젯에서 Accelerator등에 의해 수행될 수 있는 Default Command를 지정 ( 오버라이드한 경우 return true를 한다. )
	virtual bool DefaultCommand(void){ return false; }

	/// 메세지가 일치하는가?
	static bool IsMsg(const char* szMsg1, const char* szMsg2);

	/// Query Anything
	virtual void* Query(const char* szQuery);

	void SetClipByParent(bool b) {
		m_bClipByParent = b;
	}

	/// 최소 사이즈 보다 작으면 최소 사이즈로 셋팅
	void ResetMinSize(MRECT& rt, const MRECT& rtOrg);

	virtual void SetTextColor(MCOLOR color) {}

	void MakeLocalPos(MEvent* pLoalEvent, const MEvent* pEvent)
	{
		MakeLocalEvent(pLoalEvent, pEvent);
	}

	// 마우스 상태 초기화
	void InitMouseState();

#define MINT_WIDGET	"Widget"
	/// 클래스 이름 얻기
	virtual const char* GetClassName(void){ return MINT_WIDGET; }

	/// 정렬 지정
	virtual MAlignmentMode SetAlignment(MAlignmentMode am) { return MAM_NOTALIGN; }

	/// Designer Mode가 허용되는 Widget?
	bool IsEnableDesignerMode(void);
	/// Designer Mode를 허용
	void EnableDesignerMode(bool bEnable);
	/// Designer Mode에 의해 수정되었는지?
	bool IsModifiedByDesigner(void);
	/// AddedByDesignerFlag가 셋팅되어 있는가?
	bool IsAddedByDesignerFlag(void);

public:
	void LinkScriptEvent(string& strEventName, string& strScript);
	void DeclToSctipt();

	// Lua에서 사용하는 함수(glue가 붙은것은 lua에서 사용할 수 있다.)
	void					glueShow() { Show(true); }
	void					glueHide() { Show(false); }
	void					glueShowModal() { Show(true, true); }
	void					glueSetOpacity(float fOpacity);
	void					glueSetSize(int nWidth, int nHeight);
	void					glueSetText(const char* szText);
	bool					glueIsVisible();

	void					glueSetWidth(int nWidth);

	// madduck [5/11/2006]
	bool					glueIsFocus();
	void					glueSetFocus();

	int						glueGetWidth();
	int						glueGetHeight();
	const char*				glueGetName();
	const char*				glueGetText();
	float					glueGetOpacity();
	MWidget*				glueGetParent();
	bool					glueIsEnabled()	{ return IsEnable(); }
	void					glueSetEnabled(bool enabled)	{ Enable(enabled); }
	MWLua::ret_int2			glueGetPosition();
	void					glueSetPosition(int x, int y);
	void					glueClick()	{ OnClick(NULL); }
	int						glueGetPositionX();
	int						glueGetPositionY();
	void					glueBringToTop();
};


// Help Functions
/// &가 있는 위치를 알려준다. szText가 NULL이면 m_szText에서 &의 위치를 찾는다.
int GetAndPos(const char* szText);
/// & 뒤에 붙어있는 문자를 얻어낸다.
char GetAndChar(const char* szText);
/// &문자 제거
int RemoveAnd(char* szText);
/// &문자 제거
/// @param szRemovedText	[out]	제거된 문자열
/// @param szText			[in]	소스 문자열
int RemoveAnd(char* szRemovedText, const char* szText);
/// &문자 제거
/// @param szRemovedFrontText	[out]	&앞쪽 문자열
/// @param cUnderLineChar		[out]	&로 인해 밑줄이 붙을 문자열
/// @param szRemovedBackText	[out]	&뒤쪽 문자열
/// @param szText				[in]	소스 문자열
int RemoveAnd(char* szRemovedFrontText, char* cUnderLineChar, char* szRemovedBackText, const char* szText);

/// 로컬 위치를 전역 위치로 변환
MPOINT MClientToScreen(MWidget* pWidget, MPOINT& p);
/// 전역 위치를 로컬 위치로 변환
MPOINT MScreenToClient(MWidget* pWidget, MPOINT& p);
/// 로컬 영역을 전역 영역으로 변환
MRECT MClientToScreen(MWidget* pWidget, MRECT& p);
/// 전역 영역을 로컬 영역으로 변환
MRECT MScreenToClient(MWidget* pWidget, MRECT& p);

/// 현재 커서 위치 얻기
MPOINT GetCursorPosition(void);

/// 같은 위젯 클래스인지 검사
bool MIsSameWidgetClass(MWidget* pWidget, const char* szClassName);
/// 같은 위젯 클래스인지 검사
bool MIsSameWidgetClass(MWidget* pWidgetA, MWidget* pWidgetB);


} // namespace mint3