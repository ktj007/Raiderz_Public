#ifndef _XUISYSTEM_H
#define _XUISYSTEM_H

#include "XGlobal.h"
#include "MWLua.h"
#include "MConsole.h"
#include "XInputDevice.h"
#include "XCommandLogFrm.h"
#include "XScreenEffectManager.h"
#include "XMintLoader.h"
#include "XTips.h"
#include "XLuaLogFrm.h"
#include "XCursor.h"
#include "XMLogFrm.h"

#include "XMiniHpBar.h"
#include "XUIPresentationBox.h"
#include "XGuideMsgEnum.h"
#include "XCrossHair.h"

#include "CSFormatSTringParameter.h"

using namespace rs3;
using namespace mint3;

class XMsgBox;
class XCaptionMgr;
class XModuleEntity;
class Mint4Game;
class XGuideMsg;
class XHelpMgr;

#define STANDARD_SCREEN_WIDTH	1024
#define STANDARD_SCREEN_HEIGHT	768



class XMintBase
{
private:
	bool				m_bNullMint;
	Mint*				m_pMint;
	MFont*				m_pDefFont;
	MDrawContext*		m_pDC;
	void CreateDefaultFont(const wchar_t* szAliasName, const wchar_t* szFontName, int nHeight, rs3::RDeviceD3D* pDevice);
	void CreateDC(rs3::RDeviceD3D* pDevice);
	void ChangeColorTableValue();
public:
	XMintBase(bool bNullMint=false);
	~XMintBase();

	void Init(rs3::RDeviceD3D* pDevice, int nWorkspaceWidth, int nWorkspaceHeight, HWND hWnd, MWLua* pLua);
	void Fini();
	Mint* GetMint() { return m_pMint; }
	MFont* GetDefFont() { return m_pDefFont; }
	MDrawContext* GetDC() { return m_pDC; }
	MWidget*	GetUIPanel();
};

/// UI 시스템
class XUISystem
{
private:

	// UI에 필요한 Device
	rs3::RDeviceD3D*		m_pDevice;
	XInputDevice*			m_pInputDevice;

	// Window Info
	int						m_nWorkspaceWidth;
	int						m_nWorkspaceHeight;
	HWND					m_hWnd;

	// mint, tip, mouse, screen effect, etc...
	XMintBase*				m_pMintBase;
	XMintLoader				m_MintLoader;

	wchar_t					m_szFileName[256];
	wchar_t					m_szLastSheet[256];
	MWLua*					m_pLua;
	MConsole*				m_pConsole;
	XCommandLogFrm*			m_pCommandLog;
//	XMsgBox*				m_pMsgBox;
//	XMsgBox*				m_pConfirmMsgBox;
	XCursor					m_Mouse;
	XTips					m_Tips;
	XTipInGameUpdater		m_TipInGameUpdater;

	XHelpMgr*				m_pHelpMgr;
	XScreenEffectManager*	m_pScreenEffectMgr;
	XCaptionMgr*			m_pCaptionMgr;
	XLuaLogFrm*				m_pLuaLog;
	XMLogFrm*				m_pMLog;
	//XMiniHpBar*				m_pMiniBar;
	XGuideMsg*				m_pGuideMsg;
	XCrossHair				m_crossHair;

	// resource creation / destruction
	void					CreateDefaultWidgets();

	void					CreateCharacterCaptionResource();
	void					DestroyCharacterCaptionResource();

	void					CreateScreenEffectMgr();
	void					DestroyScreenEffectMgr();

	bool					GlobalMintEventCallback(MEvent* pEvent);

	// static callback
	static void				LoggerCustomOutputCallback(const char* szText, MLogLevel nLevel);
	static void				MaietWLuaLog(const char* text);

	// Target
	vec2					GetTargetUIPosForScreenEffectType();

public:
	XUISystem(bool bNullMint=false);
	virtual ~XUISystem();

	//////////////////////////////////////////////////////////////////////////
	// Create / Destroy
	//
	bool					Create(rs3::RDeviceD3D* pDevice, int nWorkspaceWidth, int nWorkspaceHeight, HWND hWnd, MWLua* pLua);
	void					Destroy();


	//////////////////////////////////////////////////////////////////////////
	// Load / Reload
	//
	bool					Load(const wchar_t* szFileName);
	void					Reload(const wchar_t* szNewFileName=NULL);		// 새로운 UI로 리로드하려면 szNewFileName에 값을 넣으면 된다.
	void					Clear();


	//////////////////////////////////////////////////////////////////////////
	// Update / Render
	//
	void					Update(float fDelta);
	void					Render();


	//////////////////////////////////////////////////////////////////////////
	// Mint
	//
	bool					OnEvent(MEvent& e);
	void					ShowSheet(const wchar_t* szSheetName);
	void					HideSheet(const wchar_t* szSheetName);
	bool					IsShowSheet(const wchar_t* szSheetName);
	void					DeleteSheet(const wchar_t* szSheetName);
	bool					IsExistSheet(const wchar_t* szSheetName);
	const wchar_t*				GetLastSheetName()	{ return m_szLastSheet; }

	Mint*					GetMint()		{ return m_pMintBase->GetMint();		}
	MFont*					GetDefFont()	{ return m_pMintBase->GetDefFont();		}
	MDrawContext*			GetDC()			{ return m_pMintBase->GetDC();			}

	bool					IsChattingBoxFocused();

	void					ShowMouse(bool bVisible){ return m_Mouse.Show(bVisible); }	///< 마우스 관련
	bool					IsMouseVisible()		{ return m_Mouse.IsVisible(); }
	XCursor&				GetCursor()				{ return m_Mouse; }
	void					UpdateDInputMouse(int* _pUpdatedRelativeX = NULL, int* _pUpdatedRelativeY = NULL);

	bool					IsMouseCursorToggleKeyDown();
	bool					IsFreeCameraKeyDown();
	bool					IsDebugSpeedUpKeyDown();

	void					ShowGameUI(bool bShow);										///< 게임 UI를 화면에 보일지 여부
	void					ToggleShowGameUI();

	//////////////////////////////////////////////////////////////////////////
	// 메세지 박스, 창
	//
	void					ShowMessageBox(const wchar_t* szText, MListener* pCustomListenter=NULL, int nMessageID=0);
	void					ShowMessageBox(int nCommandResult, MListener* pCustomListenter=NULL);
	void					SetLogCallbackToScript();


	//////////////////////////////////////////////////////////////////////////
	// Screen Effect, Character Caption, Tips
	//
	XScreenEffectManager*	GetScreenEffectMgr()	{ return m_pScreenEffectMgr; }
	XCaptionMgr*			GetCaptionMgr(){ return m_pCaptionMgr; }
	const wstring&			GetFieldDesc();
	const wstring&			GetRandomTip();
	XHelpMgr*				GetHelpMgr()			{ return m_pHelpMgr; }

	//////////////////////////////////////////////////////////////////////////
	// 메세지 출력
	//
	void					OnNarration( wstring strKey, float fRemainTime=0.f );
	void					OnSystemMsg(wstring format, FSParam param = FSParam());
	void					OnBattleMsg( wstring format, FSParam param = FSParam() );		// _by tripleJ 110504
	void					OnChatMsg(CSMsgType nMsgType, wstring format, FSParam param = FSParam());
	void					OnPresentationUpper(wstring strMsg, XPRESENTATIONICON_TYPE nIconType = PRESENTATION_ICON_NONE, bool bOutputSystemMsg=false);
	void					OnPresentationLower(wstring strMsg, XPRESENTATIONICON_TYPE nIconType = PRESENTATION_ICON_NONE,bool bOutputSystemMsg=false);
	void					OnGuideMsg( XGuideMsgEnum nMsg, MUID uidTarget=MUID::ZERO);
	void					ReleaseGuideMsg( XGuideMsgEnum nMsg, MUID uidTarget=MUID::ZERO);

	//////////////////////////////////////////////////////////////////////////
	// Console
	//
	void					InputConsoleCmd(const wchar_t* szCmd);
	void					OutputToConsole(const wchar_t* szText, bool bOutputToChatToo=true);
	void					ToggleConsole();
	bool					IsConsoleVisible();
	XCommandLogFrm*			GetCommandLog()		{ return m_pCommandLog;	}
	XLuaLogFrm*				GetLuaLog()			{ return m_pLuaLog;	}
	XMLogFrm*				GetMLog()			{ return m_pMLog;	}

	//////////////////////////////////////////////////////////////////////////
	// 세부적인 UI
	//
	//void					PartyUIRefresh();		///< Party UI
	void					InventoryUIRefresh();	///< Inventory UI
	void					InventoryMoneyRefresh();	///< Inventory, Silver

	void					TargetUIStart();		///< 타겟 UI
	void					TargetUIEnd();
	void					TargetUILock();
	void					TargetUIAim();
	vec2					GetTargetUIPos();

	void					RefreshFactionUI();
	void					RefreshTradeUI();

	void					PaletteUIRefresh( bool bBPartsChanged = false, bool bIsBPartsPalette = false);	///< 팔레트 
	void					CraftUIRefresh();		///< 생산
	void					TalentRefresh();		///< 탈렌트

	void					WeaponChange();

	void					MiniMapInfoRefresh();
	void					MiniMapInit();

	MWidget*				GetUIPanel()	{ return m_pMintBase->GetUIPanel();	}

	RECT					GetScreenRect();

	void					OnWorldLoadingComplete();
	void					ShowNPCMonitorFrm();

	void					NPCLog(vector<TD_DEBUG_NPCLOG>& pVecTDDebugNPCLog);

	MBitmap*				GetIcon(wstring szIcon);


	void					OnLearnTalentEffect();
	void					OnUnTalentEffect();
	void					OnSoulBindingEffect();
	void					OnSwimStartEffect(vec3 vPos);

	void					UI_Victory();
	void					UI_VictoryEnd();

	void					SetWorkspaceSize(int w, int h);

	void					SetPaletteTalent(int nTalentID);
	void					SetPaletteItem(int nItemID);
	void					RemovePaletteItem(int nItemID);

	void					SetSilver( int nSilver );

	// cross hair
	int						GetCrossHairListCount();
	wstring					GetCrossHairName(int nIndex);
	wstring					GetCureentCrossHairName();
	wstring					GetCureentCrossHairPickName();
	wstring					GetCurrentCrossHairAwaysName();

protected:
	void					SetPaletteItem(PALETTE_ITEM_TYPE _type, int nItemIDorTalentID);

	bool					ShowMouseForGame(bool bVisible);							///< 게임 조건에 의한 마우스 관련
};


#endif // _XUISYSTEM_H