#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include <map>
using namespace std;


#include "MintTypes.h"
#include "MEvent.h"
#include "MWidgetManager.h"
#include "MDragManager.h"
#include "MToolTipManager.h"
#include "MTimerManager.h"

class MWLua;

namespace mint3 {



class MDrawContext;
class MWidget;
class MFont;
class MBitmap;
class MCursor;
class MIDLResource;
class MListener;
class MEvent; 
class MintLoader;
class MScriptManager;
class MButtonGroupManager;
class MAnimationTemplateManager;
class MLookBase;
class MLookManager;

#define MINT_VERSION	3
#define	MVersion()	MINT_VERSION


/// Global Event Callback
typedef bool(MGLOBALEVENTCALLBACK)(MEvent* pEvent);


class MMainFrame;

/// Mint Global Object Class
class Mint{

protected:
	friend class MWidget;

	// madduck [4/4/2006]
	friend class MintLoader;

	HWND					m_hWnd;

	static Mint*			m_pInstance;				///< Instance
	MMainFrame*				m_pMainFrame;				///< Main Frame Widget

	MDrawContext*			m_pDC;						///< DrawContext
	DWORD					m_dwUpdateTime;				///< Update time
	MGLOBALEVENTCALLBACK*	m_fnGlobalEventCallBack;	///< Global Event Callback function

	MLookManager*			m_pLookManager;				///< Look manager
	MWidgetManager*			m_pWidgetManager;			///< Widget manager
	MScriptManager*			m_pScriptManager;			///< Lua 스크립트 관리
	MDragManager			m_DragManager;				///< Drag and Drop manager
	MToolTipManager			m_ToolTipManager;			///< ToolTip Manager
	MTimerManager			m_TimerManager;				///< Timer manager
	MButtonGroupManager*	m_pButtonGroupManager;		///< 버튼의 그룹 관리자
	MWidget*				m_pPopupMenu;				///< Popup menu

	bool					m_bShowCursor;				///< Show cursor
	MPOINT					m_ptLastCursorPos;			///< 마지막 커서 위치

	//////////////////////////////////////////////////////////////////////////
	// 리소스 관리자들, 
	// TODO: 폰트및 빗맵도 이쪽으로 끌어오자
	MAnimationTemplateManager*	m_pAnimationTemplateManager;	///< 애니메이션 템플릿 관리자

	// Workspace Size
	int						m_nWorkspaceWidth;
	int						m_nWorkspaceHeight;
	// Standard Size
	int						m_nStandardWidth;
	int						m_nStandardHeight;
	float					m_fRatioToStandardScreenWidth;
	float					m_fRatioToStandardScreenHeight;

	// Mint profiler
	bool					m_bStartMintProfiler;

	// IME 관련
	void*	m_pCandidateList;		// Candidate List, LPCANDIDATELIST로 캐스팅해서 사용한다.
	int		m_nCandidateListSize;	// Candidate List Size
	MPOINT	m_CandidateListPos;		// Candidate List가 보여질 위치
	bool	m_bEnableIME;
	bool	m_bEnableCharInput;		// 채팅 활성
public:
	DWORD	m_nCompositionAttributeSize;				// Composition 문자열의 속성 개수
	BYTE	m_nCompositionAttributes[MIMECOMPOSITIONSTRING_LENGTH];		// Composition 문자열의 속성
	//DWORD	m_dwCompositionClauses[MIMECOMPOSITIONSTRING_LENGTH];		// Composition 문자열의 속성에 따른 문자열 구분
	int		m_nCompositionCaretPosition;				// Composition 문자열에서의 캐럿 위치


protected:
	/// Candidate List 그리기, 위젯으로 생성하지 않고 직접 그린다
	virtual void DrawCandidateList(MDrawContext* pDC, MPOINT& p);


public:
	Mint(void);
	virtual ~Mint(void);

	/// Mint Initialize
	bool Initialize(int nWorkspaceWidth, int nWorkspaceHeight, int nStandardWidth, int nStandardHeight,
					MDrawContext* pDC, MFont* pDefaultFont, MWLua* pLua=NULL);
	/// Mint Finalize
	void Finalize(void);

#ifdef WIN32
	/// Event Processing
	bool ProcessEvent(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
#endif
	bool ProcessEvent(MEvent& e);

	/// Mint Run
	virtual void Run(void);
	/// Mint Draw
	virtual void Draw(void);

	/// Update(Redraw)
	virtual void Update(void){}

	/// Get MainFrame Widget
	MMainFrame* GetMainFrame(void);

	/// Get DrawContext
	MDrawContext* GetDrawContext(void);

	/// Singleton Instance
	static Mint* GetInstance(void);



	// 윈도우 핸들 지정
	void SetHWND(HWND hWnd);
	HWND GetHWND(void);

	HIMC m_hImc;


	void EnableIME(bool bEnable);
	bool IsEnableIME(void);
	void EnableCharInput(bool bEnable);
	bool IsEnableCharInput();
	void ClearImeCompose();

	MToolTipManager* GetToolTipManager()			{ return &m_ToolTipManager;			}
	void ClearToolTip()								{ m_ToolTipManager.Reset();			}
	void ChangedToolTipText( MWidget* pWidget);

	MTimerManager* GetTimerManager()				{ return &m_TimerManager;			}

	// Popup Menu
	bool SetPopupMenu( MWidget* pPopupMenu);
	bool ClearPopupMenu();
	MWidget* GetPopupMenu()							{ return m_pPopupMenu;				}

	// Drag and drop manager
	MDragManager* GetDragManager()					{ return &m_DragManager;			}

	bool ShowCursor( bool bShow);
	bool IsShowCursor() const						{ return m_bShowCursor;				}
	// Get last cursor position
	MPOINT GetLastCursorPos() const					{ return m_ptLastCursorPos;			}

	// Reset mouse state
	void ResetMouseState();

	// Start mint profiler
	void StartMintProfiler();

	// Drag and drop manager
	virtual MWidget* NewWidget(const char* szClass, const char* szName, MWidget* pParent, MListener* pListener);

	MWidget* FindWidget(MPOINT& p);
	MWidget* FindWidget(int x, int y);
	MWidget* FindWidget(const char* szName);

	/// Mint가 동작될 화면의 가로 크기 얻기
	int GetWorkspaceWidth(void);
	/// Mint가 동작될 화면의 세로 크기 얻기
	int GetWorkspaceHeight(void);
	/// Mint가 기준이 되는 화면의 가로 크기 얻기
	int GetStandardWidth(void);
	/// Mint가 기준이 되는 화면의 세로 크기 얻기
	int GetStandardHeight(void);
	/// Mint가 동작될 화면의 크기 설정
	void SetWorkspaceSize(int w, int h);
	/// Mint가 기준이 되는 화면 크기
	void SetStandardSize(int w, int h);

	/// 시스템에 맞는 Bitmap Open Function을 제공해야 한다.
	virtual MBitmap*	NewBitmap(const char* szName, const char* szFileName) = 0;
	/// 시스템에 맞는 Font Open Function을 제공해야 한다.
	virtual MFont*		NewFont( const char* szName, const char* szFontName, const char* szFileName, int nHeight, bool bBold =false, bool bItalic =false, bool bAntiAlias =false, float fOutline =0.0f, float fScale =1.0f) = 0;
	virtual MFont*		NewFontMemory( const char* szName, const char* szFontName, const char* szFileName, int nHeight, bool bBold =false, bool bItalic =false, bool bAntiAlias =false, float fOutline =0.0f, float fScale =1.0f) = 0;
	/// 시스템에 맞는 cursor
	virtual MCursor*	NewCursor(const char* pszAliasName);

	void SetGlobalEvent(MGLOBALEVENTCALLBACK pGlobalEventCallback);

	// IME 관련 함수들
	/// 현재 언어에 따른 기본 폰트 얻기
	const char* GetDefaultFontName(void) const;

	/// 주 언어 ID 얻기
	/// LANG_KOREAN, LANG_JAPANESE, LANG_CHINESE, etc... in "winnt.h"
	int GetPrimaryLanguageIdentifier(void) const;

	/// 부 언어 ID 얻기
	/// 중국어 간체, 번체를 얻기 위해서는 이 정보가 필요
	/// SUBLANG_KOREAN, SUBLANG_CHINESE_TRADITIONAL, SUBLANG_CHINESE_SIMPLIFIED, ... in "winnt.h"
	int GetSubLanguageIdentifier(void) const;

	/// 인디케이터(En, 한, 日, 中 등) 얻기
	const char* GetLanguageIndicatorString(void) const;

	/// Native 언어로 설정되어 있는가?
	bool IsNativeIME(void) const;

	/// WM_IME_NOTIFY의 IMN_OPENCANDIDATE 대응, Candidate List 팝업될 때 호출
	void OpenCandidateList(void);
	/// WM_IME_NOTIFY의 IMN_CLOSECANDIDATE 대응, Candidate List 팝업이 닫힐 때 호출
	void CloseCandidateList(void);

	// Candidate List 관련 함수들
	/// 인덱스값으로 Candidate 문자열 얻기
	const char* GetCandidate(int nIndex) const;
	/// Candidate 문자열 총 개수 얻기
	int GetCandidateCount(void) const;
	/// Candidate 문자열들 중 현재 선택된 문자열 인덱스 얻기
	int GetCandidateSelection(void) const;
	/// 현재 페이지에 보여줄 Candidate 문자열의 첫번째 인덱스 얻기
	int GetCandidatePageStart(void) const;
	/// 한 페이지에 보여줄 Candidate 문자열의 개수 얻기
	int GetCandidatePageSize(void) const;

	/// Candidate List가 보여질 위치 지정하기
	/// p는 조합되는 글자의 시작 위치
	/// 아래쪽 여백이 부족하면 위로 출력해줘야 하므로 위젯의 높이를 알아야 한다.
	void SetCandidateListPosition(MPOINT& p, int nWidgetHeight);
	/// Candidate List 너비
	int GetCandidateListWidth(void);
	/// Candidate List 높이
	int GetCandidateListHeight(void);

	/// Composition 속성 개수 얻기
	DWORD GetCompositionAttributeSize(void) const { return m_nCompositionAttributeSize; }
	/// Composition 속성 얻기
	const BYTE* GetCompositionAttributes(void) const { return m_nCompositionAttributes; }
	//const DWORD* GetCompositionClauses(void) const { return m_dwCompositionClauses; }

	/// 컴포지션 속성 그리기
	/// p는 szComposition[i]의 위치
	int DrawCompositionAttribute(MDrawContext* pDC, MPOINT& p, const char* szComposition, int i);
	/// p는 Composition이 시작하는 위치
	void DrawCompositionAttributes(MDrawContext* pDC, MPOINT& p, const char* szComposition);
	/// 왼쪽에 인디케이터 그리기
	void DrawIndicator(MDrawContext* pDC, MRECT& r);

	/// DC 반환
	MDrawContext* GetDC()		{ return m_pDC; }

	/// 업데이트 시간 반환
	DWORD GetUpdateTime() const	{ return m_dwUpdateTime;	}


	// XML 파싱 관련 ---------
	void Clear();

	// Lua 관련 --------------
	MScriptManager*		GetScriptManager()	{ return m_pScriptManager; }

	MWidgetManager*		GetWidgetManager()	{ return  m_pWidgetManager;}

	MWidget* FindWidget(string& strName)	{ return m_pWidgetManager->FindWidget(strName); }
	void FindWidgets(MWidgetList& widgetList, string& strName) { m_pWidgetManager->FindWidgets(widgetList, strName); }

	// 해상도에 따라 비율 계산, 해상도가 변하면 같이 스케일이 이루어질 경우
	void TransformScreenSizeRatioToStandardSize(int &nx, int &ny, int &nWidth, int &nHeight);
	// 해상도에 따른 비율 값
	float GetRatioToStandardScreenWidth() { return m_fRatioToStandardScreenWidth; }
	float GetRatioToStandardScreenHeight() { return m_fRatioToStandardScreenHeight; }

	MLookManager *		GetLookManager() { return m_pLookManager; };
	MAnimationTemplateManager* GetAnimationTemplateManager() { return m_pAnimationTemplateManager; }
	//////////////////////////////////////////////////////////////////////////

	// Enabled lua script
	void EnableCallScript( bool bEnable);
	bool IsEnableCallScript();
};

/// Mint 인스턴스 반환
inline Mint* MGetMint(void) {
	return Mint::GetInstance();
}

/// Mint가 동작될 화면의 가로 크기 얻기
inline int MGetWorkspaceWidth(void){
	return Mint::GetInstance()->GetWorkspaceWidth();
}
/// Mint가 동작될 화면의 세로 크기 얻기
inline int MGetWorkspaceHeight(void){
	return Mint::GetInstance()->GetWorkspaceHeight();
}


class MResourceMap;

// Sample
void MCreateSample(void);
void MDestroySample(void);


#ifdef WIN32
#define MMODIFIER_ALT	MOD_ALT
#define MMODIFIER_CTRL	MOD_CONTROL
#define MMODIFIER_SHIFT	MOD_SHIFT
#else
#define MMODIFIER_ALT	1
#define MMODIFIER_CTRL	2
#define MMODIFIER_SHIFT	4
#endif

#define IsHangul(x) ((unsigned char)(x)>127) 


 
// TODO: MLocale로 대체합니다. 차후 제거하겠습니다.  [2010/9/28 praptor]
//std::wstring mbs_to_wcs(std::string const& str, std::locale const& loc = std::locale());
//std::string wcs_to_mbs(std::wstring const& str, std::locale const& loc = std::locale());

bool SetClipboard( const char* text);
bool GetClipboard( char* text, size_t size);

} // namespace mint3

