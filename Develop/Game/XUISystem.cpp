#include "stdafx.h"
#include "XUISystem.h"
#include "XBaseApplication.h"
#include "XConsoleCmds.h"
#include "XChatCmds.h"
#include "XPost_Local.h"
#include "XController.h"
#include "XGlueGameEvent.h"
#include "XMsgBoxListener.h"
#include "XConst.h"
#include "XMintScriptDeclarer.h"
#include "MMainFrame.h"
#include "XProfiler.h"
#include "XMsgBox.h"
#include "XCharacterCaption.h"
#include "Mint4Game.h"
#include "Mint4Null.h"
#include "XWidgetNameDef.h"
#include "XUIChattingBox.h"
#include "XStrings.h"
#include "XGameFrameUILoader.h"
#include "XGameInterface.h"

#include "XMyPlayer.h"
#include "XModuleUI.h"

#include "XMLogFrm.h"
#include "XDebugNPCMonitor.h"
#include "XPlayFrame.h"
#include "XCaptionMgr.h"
#include "XEffectInvoker.h"
#include "XGuideMsg.h"
#include "XHelp.h"
#include "XNonPlayer.h"

#define MINT_GAMEUI		"GameUI"

XMintBase::XMintBase( bool bNullMint/*=false*/ ) : m_bNullMint(bNullMint), m_pMint(NULL), m_pDefFont(NULL), m_pDC(NULL)
{
	if (false == m_bNullMint)
	{
		m_pMint = new Mint4Game();
	}
	else
	{
		m_pMint = new Mint4Null();
	}
}

XMintBase::~XMintBase()
{
	SAFE_DELETE(m_pMint);
}

void XMintBase::CreateDefaultFont( const wchar_t* szAliasName, const wchar_t* szFontName, int nHeight, rs3::RDeviceD3D* pDevice )
{
	if( m_pDefFont != NULL)
	{
		m_pDefFont->Destroy();
		SAFE_DELETE( m_pDefFont );
	}

	int nWorkspaceHeight = m_pMint->GetWorkspaceHeight();

	if(nHeight == 0)
		nHeight = (12 * nWorkspaceHeight) / STANDARD_SCREEN_HEIGHT;

	if (false == m_bNullMint)
	{
		MFontR3* pNewFontR3 = new MFontR3();
		if( !pNewFontR3->Create((rs3::RDeviceD3D*)(pDevice), MLocale::ConvUTF16ToAnsi(szAliasName).c_str(), MLocale::ConvUTF16ToAnsi(szFontName).c_str(), nHeight) )
		{
			pNewFontR3->Destroy();
			SAFE_DELETE( pNewFontR3 );
		}
		m_pDefFont = pNewFontR3;
	}
	else
	{
		m_pDefFont = new MFontNull();
	}
}

void XMintBase::CreateDC( rs3::RDeviceD3D* pDevice )
{
	if (false == m_bNullMint)
	{
		m_pDC = new MDrawContextR3((rs3::RDeviceD3D*)(pDevice));
	}
	else
	{
		m_pDC = new MDrawContextNull();
	}
}

void XMintBase::Init( rs3::RDeviceD3D* pDevice, int nWorkspaceWidth, int nWorkspaceHeight, HWND hWnd, MWLua* pLua )
{
	// 폰트 해상도에 따른 크기 변환

	int nDefaultFontHeight = XCONST::DEFAULT_UI_FONT_HEIGHT;
	if(nDefaultFontHeight == 0)
		nDefaultFontHeight = (12 * nWorkspaceHeight) / STANDARD_SCREEN_HEIGHT;

	ChangeColorTableValue();

	CreateDefaultFont(L"Default", XCONST::DEFAULT_UI_FONT, nDefaultFontHeight, pDevice);
	CreateDC(pDevice);

	m_pMint->Initialize(nWorkspaceWidth, 
						nWorkspaceHeight, 
						STANDARD_SCREEN_WIDTH, 
						STANDARD_SCREEN_HEIGHT, 
						m_pDC, 
						m_pDefFont, 
						pLua);


	m_pMint->SetHWND(hWnd);
}

void XMintBase::Fini()
{
	m_pMint->Finalize();

	SAFE_DELETE(m_pDefFont);
	SAFE_DELETE(m_pDC);

	MFontManager::Destroy();
	MBitmapManager::Destroy();
}

void XMintBase::ChangeColorTableValue()
{
//	mint3::MColorTable::MEDIT_OUTLINE = MCOLOR(0xFF72624F);
//	mint3::MColorTable::TEXTAREA_OUTLINE = MCOLOR(0xFF534737);
//	mint3::MColorTable::MLIST_OUTLINE = MCOLOR(0xFF534737);
}

MWidget* XMintBase::GetUIPanel()
{
	const char* UI_WINDOW_WIDGET_NAME = MINT_GAMEUI;
	MWidget* pUIWindow = m_pMint->FindWidget(UI_WINDOW_WIDGET_NAME);
	return pUIWindow;
}
//////////////////////////////////////////////////////////////////////////////////////////////
XUISystem::XUISystem(bool bNullMint) : m_pMintBase(new XMintBase(bNullMint)), m_pConsole(NULL), m_pDevice(NULL),
						 m_nWorkspaceWidth(800), m_nWorkspaceHeight(600), m_hWnd(0), m_pLua(NULL),
						 m_pScreenEffectMgr(NULL), /*m_pMsgBox(NULL), m_pConfirmMsgBox(NULL),*/
						 m_pCommandLog(NULL), m_pGuideMsg(new XGuideMsg()), m_pHelpMgr(new XHelpMgr())
{
	m_szFileName[0] = 0;
	m_szLastSheet[0] = 0;
	m_pInputDevice = new XInputDevice();
	m_pCaptionMgr = new XCaptionMgr();

	m_pMLog = NULL;

	//m_pMiniBar = NULL;
}

XUISystem::~XUISystem()
{
	SAFE_DELETE(m_pHelpMgr);
	SAFE_DELETE(m_pGuideMsg);
	SAFE_DELETE(m_pInputDevice);
	SAFE_DELETE(m_pCaptionMgr);
	SAFE_DELETE(m_pMintBase);
}

bool XUISystem::Create(rs3::RDeviceD3D* pDevice, int nWorkspaceWidth, int nWorkspaceHeight, HWND hWnd, MWLua* pLua)
{
	m_pDevice = pDevice;
	m_nWorkspaceWidth = nWorkspaceWidth;
	m_nWorkspaceHeight = nWorkspaceHeight;
	m_hWnd = hWnd;
	m_pLua = pLua;

	m_pInputDevice->Create(hWnd);

	m_pMintBase->Init((rs3::RDeviceD3D*)(pDevice), nWorkspaceWidth, nWorkspaceHeight, hWnd, pLua);
	MEvent::SetIMESupport(true);	// ime 설정을 따로 해줘야 한글이 써진다.

	// 팁
	m_Tips.Load();

	// 도움말, 튜토리얼	by tripleJ 110414
	m_pHelpMgr->LoadHelpMgr();

	// 스크린 이팩트
	CreateScreenEffectMgr();

	// 캐릭터 캡션
	CreateCharacterCaptionResource();

	// 디폴트 위젯
	CreateDefaultWidgets();

	XMintScriptDeclarer mintScriptDeclarer;
	mintScriptDeclarer.InitDeclWidgets(pLua);

	// 채팅 커맨드 연결
	XChatCmds::Init();

	m_pLua->SetLogFunc(MaietWLuaLog);

	m_pCaptionMgr->CreatDamageCaption();

	// 크로스 헤어
	m_crossHair.Init();

	return true;
}


void XUISystem::CreateDefaultWidgets()
{
	Mint* pMint = m_pMintBase->GetMint();

//	m_pConsole = (MConsole*)pMint->NewWidget( MINT_CONSOLE, NULL, pMint->GetMainFrame(), NULL);
	m_pConsole = new MConsole( "Console", pMint->GetMainFrame(), NULL);
	m_pConsole->SetRect(1, 1, 500, 400);
	m_pConsole->Show(false);
	pMint->GetMainFrame()->AddChild( m_pConsole);

	// 콘솔 명령어 초기화
#ifndef _PUBLISH
#define GMCLIENT
#endif

#ifdef GMCLIENT
	XConsoleCmds::Init(m_pConsole);
#endif

	// madduck [5/4/2006]
	m_pCommandLog = (XCommandLogFrm*)pMint->NewWidget(MINT_COMMANDLOGFRM, NULL, pMint->GetMainFrame(), NULL);
	m_pCommandLog->SetRect(pMint->GetWorkspaceWidth()-805, pMint->GetWorkspaceHeight()-205, 800, 200);
	m_pCommandLog->Show(false);

	// 기본 MsgBox
// 	m_pMsgBox = new XMsgBox(NULL, pMint->GetMainFrame(), NULL, MT_OK);
// 	m_pMsgBox->SetTitle(GAME_NAME_STR);
// 	m_pMsgBox->SetOKButtonText("OK");
// 	m_pMsgBox->SetListener(XGetMsgBoxListener());

// 	m_pConfirmMsgBox = new XMsgBox(NULL, pMint->GetMainFrame(), NULL, MT_YESNO);
// 	m_pConfirmMsgBox->SetListener(XGetConfirmMsgBoxListener());

	m_pLuaLog = (XLuaLogFrm*)pMint->NewWidget(MINT_LUALOGFRM, NULL, pMint->GetMainFrame(), NULL);
	m_pLuaLog->SetRect(1, pMint->GetWorkspaceHeight()-205, 700, 200);
	m_pLuaLog->Show(false);

	m_pMLog = (XMLogFrm*)pMint->NewWidget(MINT_MLOGFRM, NULL, pMint->GetMainFrame(), NULL);
	m_pMLog->SetRect(1, pMint->GetWorkspaceHeight()-305, 350, 300);
	m_pMLog->Show(false);
}

void XUISystem::CreateCharacterCaptionResource()
{
	// character caption setting
	_ASSERT( m_pCaptionMgr != NULL );
	_ASSERT( m_pCaptionMgr->m_pCaptionFont == NULL );

	m_pCaptionMgr->m_pCaptionFont = new RFont;
	if ( m_pCaptionMgr->m_pCaptionFont->Initialize(m_pDevice) )
	{
		if ( global.filesys->GetAccessMode() == MFILEACCESS_ENCRYPTED)
			m_pCaptionMgr->m_pCaptionFont->AddFontResourceFromMemory( XCONST::CHARACTER_CAPTION_FONT_FILE);
		else
			m_pCaptionMgr->m_pCaptionFont->AddFontResourceFromFile( XCONST::CHARACTER_CAPTION_FONT_FILE);

		if( !m_pCaptionMgr->m_pCaptionFont->Create(m_pDevice, XCONST::CHARACTER_CAPTION_FONT_NAME, XCONST::CHARACTER_CAPTION_FONT_HEIGHT) )
			SAFE_DELETE( m_pCaptionMgr->m_pCaptionFont );
	}
	else
		SAFE_DELETE(m_pCaptionMgr->m_pCaptionFont);

	//_ASSERT( m_pCharacterCaptionMgr->IsEmptyCaptionContainer() == true );
}

void XUISystem::DestroyCharacterCaptionResource()
{
	_ASSERT( m_pCaptionMgr );
	//_ASSERT( m_pCharacterCaptionMgr->IsEmptyCaptionContainer() == true );
	SAFE_DELETE(m_pCaptionMgr->m_pCaptionFont);
}

void XUISystem::CreateScreenEffectMgr()
{
	_ASSERT( m_pScreenEffectMgr == NULL );
	m_pScreenEffectMgr = new XScreenEffectManager();
	m_pScreenEffectMgr->Create(REngine::GetSceneManagerPtr());
	m_pScreenEffectMgr->LoadScreenEffect();	// 스크린 이펙트 로딩
//	m_pScreenEffectMgr->ResisterTarget();	// 타겟 스크린 이펙트 등록
}

void XUISystem::DestroyScreenEffectMgr()
{
	if(m_pScreenEffectMgr)
	{
		m_pScreenEffectMgr->Destroy();
		delete m_pScreenEffectMgr;
		m_pScreenEffectMgr = NULL;
	}
}

void XUISystem::Destroy()
{
// 	SAFE_DELETE(m_pMsgBox);
// 	SAFE_DELETE(m_pConfirmMsgBox);

	m_pLuaLog = NULL;
	m_pCommandLog=NULL;
	m_MintLoader.ClearIncludedFileNames();
	m_pMintBase->Fini();

	m_pCaptionMgr->DestroyDamageCaption();

	m_pInputDevice->Destroy();

	// by tripleJ 110414
	m_pHelpMgr->FinishHelpMgr();

	DestroyCharacterCaptionResource();
	DestroyScreenEffectMgr();

	if(gg.omgr)	gg.omgr->OffScreenDialog();
}

bool XUISystem::OnEvent(MEvent& e)
{
	// HelpMgr Event 처리
	if( true == m_pHelpMgr->OnEvent( e ) )
		return true;

	if(GlobalMintEventCallback(&e))
		return true;

	// 마우스가 활성화가 안되어 있으면 Mint 이벤트 처리를 안한다.
	if(m_Mouse.IsVisible() == false)
		return false;

	if ( m_pMintBase->GetMint()->ProcessEvent( e) == true)		return true;
	return false;
}


bool XUISystem::Load(const wchar_t* szFileName)
{
	wcscpy_s(m_szFileName, szFileName);

	m_MintLoader.ClearIncludedFileNames();
	bool ret = m_MintLoader.LoadFromFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str());

	return ret;
}


void XUISystem::Reload(const wchar_t* szNewFileName)
{
	bool bConsoleVisible = false;
	MRECT rtConsole;


#ifndef _PUBLISH
	if ( global.gfmgr != NULL  &&  global.gfmgr->GetCurrFrameID() == PLAY_FRAME_ID  &&  global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "UI", "LEAVE");
	}
#endif


	if ((m_pConsole) && (m_pConsole->GetShow())) 
	{
		bConsoleVisible = true;
		rtConsole = m_pConsole->GetRect();
	}

	if ( gvar.Game.pMyPlayer != NULL)
	{
		XModuleUI* pModuleUI = gvar.Game.pMyPlayer->GetModuleUI();
		if(pModuleUI) pModuleUI->Destroy();
	}

	Destroy();
	Create(m_pDevice , m_nWorkspaceWidth, m_nWorkspaceHeight, m_hWnd, m_pLua);


#ifndef _PUBLISH
	if ( m_szLastSheet[0] != 0  &&  wcscmp( m_szLastSheet, L"game") == 0)
	{
		XPlayFrame* pPlayFrame = static_cast<XPlayFrame*>(global.gfmgr->GetCurrFrame());
		if ( pPlayFrame)	pPlayFrame->ReloadDashBoard();
	}
#endif


	if (szNewFileName != 0)
	{
		wcscpy_s(m_szFileName, szNewFileName);
	}
	if (m_szFileName[0] != 0)
	{
		Load(m_szFileName);
	}

	if (m_szLastSheet[0] != 0)
	{
		ShowSheet(m_szLastSheet);
	}

	if (bConsoleVisible)
	{
//		m_pConsole->GetShow(rtConsole);
		m_pConsole->Show(true);
	}

	// 입력창 활성화 취소
	m_pMintBase->GetMint()->EnableCharInput(false);

	for(XObjectMap::iterator itObject = gg.omgr->BeginItor(); itObject != gg.omgr->EndItor(); itObject++)
	{
		XObject* pObj = itObject->second;
		if(pObj->GetEntityType() == ETID_NPC)
		{
			XNonPlayer* pNPC = pObj->AsNPC();
			if (pNPC)
			{
				XModuleUI* pModuleUI = pNPC->GetModuleUI();
				if(pModuleUI) pModuleUI->SetNPCNamePane();
			}
		}
	}

#ifndef _PUBLISH
	if ( global.gfmgr != NULL  &&  global.gfmgr->GetCurrFrameID() == PLAY_FRAME_ID  &&  global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "UI", "ENTER");
		global.script->GetGlueGameEvent().OnGameEvent( "UI", "LOADED");
	}
#endif
}


void XUISystem::Clear()
{
	m_pMintBase->GetMint()->Clear();

	CreateDefaultWidgets();


#ifndef _PUBLISH
	if ( m_szLastSheet[0] != 0  &&  wcscmp( m_szLastSheet, L"game") == 0)
	{
		XPlayFrame* pPlayFrame = static_cast<XPlayFrame*>(global.gfmgr->GetCurrFrame());
		if ( pPlayFrame)	pPlayFrame->ReloadDashBoard();
	}
#endif

	// 입력창 활성화 취소
	m_pMintBase->GetMint()->EnableCharInput(false);
}


void XUISystem::Update(float fDelta)
{
	m_pInputDevice->Update(fDelta);

	// update screen effect manager
	m_pScreenEffectMgr->Update();

	// by tripleJ 110414
	m_pHelpMgr->UpdateHelpMgr( fDelta );

	m_TipInGameUpdater.Update(fDelta, &m_Tips, this);

	m_crossHair.StartCrossHair_Aways(GetTargetUIPos());
}

void XUISystem::Render()
{
	// 3d font 는 카메라를 device의 view 트랜스폼을 사용한다.
	m_pCaptionMgr->Render();

	// screen effect render, 카메라 재설정을 포함한다
	// character caption
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(13);
	m_pScreenEffectMgr->Render();
}

void XUISystem::ShowSheet(const wchar_t* szSheetName)
{
	wcscpy_s(m_szLastSheet, szSheetName);

	MWidget* pSheet = m_pMintBase->GetMint()->FindWidget(MLocale::ConvUTF16ToAnsi(szSheetName));
	if (pSheet)
	{
		pSheet->Show(true);
//		pSheet->SetFocus();
	}
}

void XUISystem::HideSheet(const wchar_t* szSheetName)
{
	MWidget* pSheet = global.mint->FindWidget(MLocale::ConvUTF16ToAnsi(szSheetName));
	if (pSheet)
	{
		pSheet->Hide();
	}
}

bool XUISystem::IsShowSheet(const wchar_t* szSheetName)
{
	MWidget* pSheet = global.mint->FindWidget(MLocale::ConvUTF16ToAnsi(szSheetName));
	if ( pSheet != NULL)		return pSheet->GetShow();
	return false;
}

void XUISystem::DeleteSheet(const wchar_t* szSheetName)
{
	MWidget* pSheet = global.mint->FindWidget(MLocale::ConvUTF16ToAnsi(szSheetName));
	if (pSheet)
	{
		pSheet->Hide();
		pSheet->RemoveFromParent();
		pSheet->DeleteChildren();
		delete pSheet;
	}
}

bool XUISystem::IsExistSheet(const wchar_t* szSheetName)
{
	wcscpy_s(m_szLastSheet, szSheetName);

	MWidget* pSheet = m_pMintBase->GetMint()->FindWidget(MLocale::ConvUTF16ToAnsi(szSheetName));
	if (pSheet)			return true;
	return false;
}

void XUISystem::InputConsoleCmd(const wchar_t* szCmd)
{
	if (m_pConsole) 
	{
		m_pConsole->Input(MLocale::ConvUTF16ToAnsi(szCmd).c_str());
	}
}

void XUISystem::OutputToConsole(const wchar_t* szText, bool bOutputToChatToo)
{
	if (m_pConsole) m_pConsole->OutputMessage(MLocale::ConvUTF16ToAnsi(szText).c_str());

	OnSystemMsg(szText);
}

void XUISystem::ToggleConsole()
{
	if ( m_pConsole)
	{
		bool bShow = !m_pConsole->GetShow();
		m_pConsole->Show( bShow);

		if(bShow)
			ShowMouse(bShow);				// 열때에는 무조건 보인다.
		else
			ShowMouseForGame(bShow);		// 닫을때에는 창이 열려 있는지 체크
	}
}

bool XUISystem::IsConsoleVisible()
{
	return ( m_pConsole ? m_pConsole->GetShow() : false);
}

void XUISystem::ShowGameUI(bool bShow)
{
	if (bShow)
	{
		Reload();
	}
	else
	{
		MWidget* pSheet = global.mint->FindWidget(MLocale::ConvUTF16ToAnsi(UI_GAME_SHEET_NAME));
		if (pSheet)
		{
			pSheet->DeleteChildren();
			pSheet->SetFocus();
		}
	}
}

void XUISystem::ShowMessageBox(const wchar_t* szText, MListener* pCustomListenter, int nMessageID)
{
//	if (pCustomListenter)
//		m_pMsgBox->SetCustomListener(pCustomListenter);

	wchar_t text[1024] =L"";

	// nMessageID가 0이 아니면 메세지 뒤에 메세지 번호도 함께 출력해준다.(다른 나라 말일때 확인하기 위함)
	if (nMessageID != 0)
	{
		swprintf(text, L"%s (M%d)", szText, nMessageID);
	}
	else
	{
		wcscpy(text, szText);
	}

// 	if (m_pMsgBox)
// 	{
// 		m_pMsgBox->SetText(text);
// 		m_pMsgBox->Show(true);
// 	}
}

void XUISystem::ShowMessageBox(int nCommandResult, MListener* pCustomListenter)
{
	ShowMessageBox(XCmdResultStr(nCommandResult));
}

//void XUISystem::PartyUIRefresh()
//{
//	if(global.script)
//	{
//		wstring sz;
//		gvar.MyInfo.Party.GetMemberUIDList(sz);
//		global.script->GetGlueGameEvent().OnPartyWindowRefresh(sz.c_str());
//	}
//}

void XUISystem::InventoryUIRefresh()
{
	// 게임중일 때에만 ui 업데이트
	if ((global.gfmgr != NULL) && (global.gfmgr->GetCurrFrameID() != PLAY_FRAME_ID)) return;

	if (global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "INVENTORY");
	}
}

void XUISystem::InventoryMoneyRefresh()
{
	if(global.script)
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "MONEY");
}

void XUISystem::LoggerCustomOutputCallback( const char* szText, MLogLevel nLevel )
{
	return;

#ifndef _PUBLISH
	if(global.ui && global.ui->GetMLog())
		global.ui->GetMLog()->AddString(szText);
#endif
}

void XUISystem::SetLogCallbackToScript()
{
	// 쓰레드 문제때문에 당분간 로그를 스크립트로 보내는 것은 사용하지 않음. - birdkr
	//return;

	if (MGetDefaultLogger())
	{
		MGetDefaultLogger()->SetCustomOutputCallback(LoggerCustomOutputCallback);
	}
}

const wstring& XUISystem::GetFieldDesc()
{
	return m_Tips.GetFieldDesc();
}

const wstring& XUISystem::GetRandomTip()
{
	return m_Tips.GetRandomTip();
}

bool XUISystem::GlobalMintEventCallback( MEvent* pEvent )
{
	switch ( pEvent->nMessage)
	{
	case MWM_KEYDOWN :
		{
			if ( pEvent->nKey == 0xC0 /*`*/  &&  pEvent->bCtrl == true)
			{
// CBT2 종료 후 Neowiz QA 팀의 테스트 편의를 위해 빼야 합니다. - aibeast
#if defined( _PUBLISH)  &&  ( defined( _KOREA))

				if (gvar.MyInfo.ChrInfo.nPlayerGrade < PLAYER_GRADE_GM)
					return false;
#endif

				ToggleConsole();
				return true;
			}
			break;
		}

	default :
		break;
	}

	return false;
}

void XUISystem::TargetUIStart()
{
	vec2 vPos = GetTargetUIPosForScreenEffectType();	
	
	if(m_crossHair.StartCrossHair(GetTargetUIPos()) == false && m_pScreenEffectMgr)
	{
		m_pScreenEffectMgr->SetTarget(TUS_START, vPos.x, vPos.y);
	}
}

void XUISystem::TargetUIEnd()
{
	m_crossHair.EndCrossHair();
	if(m_pScreenEffectMgr)
	{
		vec2 vPos = GetTargetUIPosForScreenEffectType();
		m_pScreenEffectMgr->SetTarget(TUS_END, vPos.x, vPos.y);
	}
}

void XUISystem::TargetUILock()
{
	vec2 vPos = GetTargetUIPosForScreenEffectType();

	if(m_crossHair.StartCrossHair_Pick(GetTargetUIPos()) == false && m_pScreenEffectMgr)
	{
		m_pScreenEffectMgr->SetTarget(TUS_LOCK, vPos.x, vPos.y);
	}
}

void XUISystem::TargetUIAim()
{
	vec2 vPos = GetTargetUIPosForScreenEffectType();

	if(m_crossHair.StartCrossHair(GetTargetUIPos()) == false && m_pScreenEffectMgr)
	{
		m_pScreenEffectMgr->SetTarget(TUS_AIM, vPos.x, vPos.y);
	}
}

void XUISystem::PaletteUIRefresh( bool bBPartsChanged, bool bIsBPartsPalette)
{
	if(global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "PALETTE", "REFRESH", bBPartsChanged ? 1 : 0, bIsBPartsPalette ? 1 : 0);
	}
}

void XUISystem::CraftUIRefresh()
{
	if (global.script)
	{
		global.script->GetGlueGameEvent().OnGameEvent( "CRAFTING", "REFRESH");
	}
}

void XUISystem::TalentRefresh()
{
	if (global.script)
	{
		global.script->GetGlueGameEvent().OnRefreshTalent();
	}
}

void XUISystem::WeaponChange()
{
//	if (global.script)	global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "WEAPON_CHANGE");
}

void XUISystem::MiniMapInfoRefresh()
{
//	if (global.script)	global.script->GetGlueGameEvent().OnMiniMapInfoRefresh();
}

void XUISystem::MiniMapInit()
{
//	if (global.script)	global.script->GetGlueGameEvent().OnMiniMapInit();
}

void XUISystem::MaietWLuaLog( const char* text )
{
	if (global.ui && global.ui->m_pLuaLog)
	{
		global.ui->m_pLuaLog->AddLog(text);
	}
	mlog(text);
}

void XUISystem::OnNarration( wstring strKey, float fRemainTime/*=0.f */ )
{
//	m_pNarrationLabel->SetText(strKey.c_str(), fRemainTime);
}

vec2 XUISystem::GetTargetUIPos()
{
	const float TARGET_POS_HEIGHT_RATE	= 0.45f;
	const float TARGET_POS_WIDTH_RATE = 0.5f;

	float fScreenHeight = m_nWorkspaceHeight;
	float fScreenWidth = m_nWorkspaceWidth;
	int nHeightScreenPos = (float)(fScreenHeight * TARGET_POS_HEIGHT_RATE);
	int nWidthScreenPos = (float)(fScreenWidth * TARGET_POS_WIDTH_RATE);

	return vec2(nWidthScreenPos, nHeightScreenPos);
}

vec2 XUISystem::GetTargetUIPosForScreenEffectType()
{
	vec2 vTargetPos = GetTargetUIPos();

	float fScreenHeight = m_nWorkspaceHeight;
	
	return vec2(0, (fScreenHeight / 2.0f) - vTargetPos.y);
}

RECT XUISystem::GetScreenRect()
{
	RECT rt;
	rt.left = rt.top = 0;
	rt.right = m_nWorkspaceWidth;
	rt.bottom = m_nWorkspaceHeight;

	return rt;
}

void XUISystem::OnWorldLoadingComplete()
{
	MiniMapInit();

	XGetGameInterface().OPTIONS.UpdateSystemOption();
}

bool XUISystem::IsChattingBoxFocused()
{
	if (global.mint == NULL) return false;

	MWidget* pChatWindow = global.mint->FindWidget(WIDGET_NAME_CHATTINGBOX);	
	if (pChatWindow)
	{
		if ( strcmp( pChatWindow->GetClassName(), MINT_CHATTINGBOX) != 0) return false;

		XUIChattingBox* pChatFrame = dynamic_cast<XUIChattingBox*>(pChatWindow);
		return pChatFrame->IsChatInputActivated();
	}

	return false;
}

void XUISystem::RefreshFactionUI()
{
	if (global.script)
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "FACTION");
}

void XUISystem::ShowNPCMonitorFrm()
{
	if (global.mint == NULL) return;

#ifndef _PUBLISH
	XPlayFrame* pPlayFrame = static_cast<XPlayFrame*>(global.gfmgr->GetCurrFrame());
	if (pPlayFrame)
	{
		pPlayFrame->GetDashBoard()->ShowNPCMonitor(true);
	}
#endif
}

void XUISystem::NPCLog(vector<TD_DEBUG_NPCLOG>& pVecTDDebugNPCLog)
{
	if (global.mint == NULL) return;

#ifndef _PUBLISH
	XPlayFrame* pPlayFrame = static_cast<XPlayFrame*>(global.gfmgr->GetCurrFrame());
	if (pPlayFrame)
	{
		pPlayFrame->GetDashBoard()->SetNPCLog(pVecTDDebugNPCLog);
	}
#endif
}

bool XUISystem::ShowMouseForGame( bool bVisible )
{
	// 마우스를 닫을때...
	// 창이 열려 있으면 마우스 포인터가 보이고
	// 창이 없으면 마우스 포인터가 사라집니다.
	if(bVisible == false)
	{
		int nVisibleWindowCount = 0;

		if(global.script)
			nVisibleWindowCount = global.script->GetGlueGameEvent().GetVisibledWindow();

		if (nVisibleWindowCount > 0)
		{
			ShowMouse(true);

			return true;
		}
		else
		{
			ShowMouse(false);

			return false;
		}
	}
	else
	{
		ShowMouse(bVisible);
	}

	return false;
}

void XUISystem::RefreshTradeUI()
{
	// 게임중일 때에만 ui 업데이트
	if (global.gfmgr->GetCurrFrameID() != PLAY_FRAME_ID) return;

	if (global.script)
		global.script->GetGlueGameEvent().OnGameEvent( "TRADE", "REFRESH");
}

bool XUISystem::IsMouseCursorToggleKeyDown()
{
	// 마우스 커서키는 Ctrl키로 고정
	return (MEvent::GetCtrlState());
}

bool XUISystem::IsFreeCameraKeyDown()
{
	// 카메라키는 Alt키로 고정
	return (MEvent::GetShiftState());
}

bool XUISystem::IsDebugSpeedUpKeyDown()
{
#ifndef _DEBUG
	return false;
#endif

	return (MEvent::GetAltState());
}

///< 아이콘이 현재 로드되어있지 않으면 생성 시도.
MBitmap* XUISystem::GetIcon( wstring szIcon )
{
	MBitmap* pIcon = MBitmapManager::Get( MLocale::ConvUTF16ToAnsi(szIcon.c_str()).c_str());
	if (pIcon == NULL)
	{
		pIcon = GetMint()->NewBitmap(MLocale::ConvUTF16ToAnsi(szIcon.c_str()).c_str(), MLocale::ConvUTF16ToAnsi(szIcon.c_str()).c_str());
		if (pIcon != NULL) MBitmapManager::Add(pIcon);
	}

	return pIcon;
}

void XUISystem::OnSystemMsg(wstring format, FSParam param /*= FSParam()*/)
{
	if (!gg.controller)	return;

	wstring msg = FormatString(format, param);
	gg.controller->OnChat(MT_SYSTEM, msg);
}

void XUISystem::OnBattleMsg( wstring format, FSParam param /*= FSParam()*/)
{
	if (!gg.controller)	return;

	wstring msg = FormatString(format, param);
	gg.controller->OnChat(MT_BATTLE, msg);
}

void XUISystem::OnChatMsg(CSMsgType nMsgType, wstring format, FSParam param /*= FSParam()*/)
{
	if (!gg.controller)	return;

	wstring msg = FormatString(format, param);
	gg.controller->OnChat(nMsgType, msg);
}

void XUISystem::OnLearnTalentEffect()
{
	XEffectSinglyInvoker effectInvoker;
	effectInvoker.Invoke(wstring(L"LearnTalent"), gvar.Game.pMyPlayer->GetPosition(), vec3(vec3::AXISY), vec3(vec3::AXISZ));
}

void XUISystem::OnUnTalentEffect()
{
	XEffectSinglyInvoker effectInvoker;
	effectInvoker.Invoke(wstring(L"ResetTalent"), gvar.Game.pMyPlayer->GetPosition(), vec3(vec3::AXISY), vec3(vec3::AXISZ));
}

void XUISystem::OnSoulBindingEffect()
{
	XEffectSinglyInvoker effectInvoker;
	effectInvoker.Invoke(wstring(L"click_BindingStone"), gvar.Game.pMyPlayer->GetPosition(), vec3(vec3::AXISY), vec3(vec3::AXISZ));
}

void XUISystem::OnSwimStartEffect( vec3 vPos )
{
	XEffectSinglyInvoker effectInvoker;
	effectInvoker.Invoke(wstring(L"start_swim"), vPos, vec3(vec3::AXISY), vec3(vec3::AXISZ));
}

void XUISystem::OnPresentationLower( const wstring strMsg, XPRESENTATIONICON_TYPE nIconType, bool bOutputSystemMsg )
{
	if (strMsg.empty()) return;

	if (bOutputSystemMsg)
	{
		global.ui->OnSystemMsg(strMsg);
	}

	// call presentation(lua script function)
	const int PRESENTATION_TYPE_LOWER = 1;
	if (global.script->GetGlueGameEvent().OnGameEvent( "PRESENTATION", MLocale::ConvUTF16ToAnsi(strMsg.c_str()), PRESENTATION_TYPE_LOWER, (int)nIconType))
	{
		if(m_pGuideMsg)
			m_pGuideMsg->NotifyOnPresentationLower();
	}
}

void XUISystem::OnPresentationUpper( wstring strMsg, XPRESENTATIONICON_TYPE nIconType, bool bOutputSystemMsg/*=false*/ )
{
	if (strMsg.empty()) return;

	if (bOutputSystemMsg)
	{
		global.ui->OnSystemMsg(strMsg);
	}

	const int PRESENTATION_TYPE_UPPER = 0;

	global.script->GetGlueGameEvent().OnGameEvent( "PRESENTATION", MLocale::ConvUTF16ToAnsi(strMsg.c_str()), PRESENTATION_TYPE_UPPER, (int)nIconType);
}

void XUISystem::OnGuideMsg( XGuideMsgEnum nMsg, MUID uidTarget )
{
	m_pGuideMsg->OutputMsg(nMsg, uidTarget);
}

void XUISystem::ReleaseGuideMsg( XGuideMsgEnum nMsg, MUID uidTarget )
{
	m_pGuideMsg->ReleaseMsg(nMsg, uidTarget);
}

void XUISystem::UI_Victory()
{
	if(m_pScreenEffectMgr)
	{
		m_pScreenEffectMgr->SetVictory(TUS_START, 0, 170);
	}

	//XEffectInvoker effectInvoker;
	//effectInvoker.Invoke(gvar.Game.pMyPlayer, string(EFFECT_NAME_VICTORY), string(""), string(""));
}

void XUISystem::UI_VictoryEnd()
{
	if(m_pScreenEffectMgr)
	{
		m_pScreenEffectMgr->SetDraw(L"Victory", false);
	}
}

void XUISystem::ToggleShowGameUI()
{
	if ( IsShowSheet( UI_GAME_SHEET_NAME) == true)	HideSheet( UI_GAME_SHEET_NAME);
	else											ShowSheet( UI_GAME_SHEET_NAME);

}

void XUISystem::UpdateDInputMouse( int* _pUpdatedRelativeX, int* _pUpdatedRelativeY )
{
	if (m_pInputDevice)
	{
		RMouseState* pMouseStat = m_pInputDevice->GetMouseState();
		if (pMouseStat)
		{
/*			// fake capturing (for mouse up)
			UINT mouseMsg = 0;
			if ( MEvent::bLButton && !(pMouseStat->rgbButtons[0] & 0x80) )
				mouseMsg = WM_LBUTTONUP;
			if ( MEvent::bRButton && !(pMouseStat->rgbButtons[1] & 0x80) )
				mouseMsg = WM_RBUTTONUP;
			if (MEvent::bMButton && !(pMouseStat->rgbButtons[2] & 0x80) )
				mouseMsg = WM_MBUTTONUP;
			if (mouseMsg != 0)
			{
				LPARAM lparam = MAKELPARAM(MEvent::LatestPos.x, MEvent::LatestPos.y);
				MEvent e;
				int nResult = e.TranslateEvent( m_hWnd, mouseMsg, NULL, lparam);
				m_pMintBase->GetMint()->ProcessEvent(e);
			}
*/
			if (_pUpdatedRelativeX)
				*_pUpdatedRelativeX = (int)pMouseStat->lX;

			if (_pUpdatedRelativeY)
				*_pUpdatedRelativeY = (int)pMouseStat->lY;
		}
	}
}

void XUISystem::SetWorkspaceSize( int w, int h )
{
	m_nWorkspaceWidth = w;
	m_nWorkspaceHeight = h;
}

void XUISystem::SetPaletteTalent(int nTalentID)
{
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return;
	
	if (gvar.MyInfo.Talent.IsLearnedTalentLine(pTalentInfo->m_nTalentLine) == false ||
		gvar.MyInfo.PaletteList.IsRegistedTalentLine(pTalentInfo->m_nTalentLine) == false)
	{
		PALETTE_ITEM_TYPE _type = PIT_TALENT;
		SetPaletteItem(_type, nTalentID);
	}
}

void XUISystem::SetPaletteItem(int nItemID)
{
	PALETTE_ITEM_TYPE _type = PIT_ITEM;
	SetPaletteItem(_type, nItemID);
}

void XUISystem::RemovePaletteItem(int nItemID)
{
	PALETTE_ITEM_TYPE _type = PIT_ITEM;
	int nSlotNum = gvar.MyInfo.PaletteList.GetCurPaletteListIndex();
	int nSlotIndex = gvar.MyInfo.PaletteList.GetRegisteredPaletteIndex(_type, nItemID);
	if (nSlotIndex == -1) return;

	XPostPalette_PutDown( (PALETTE_NUM)nSlotNum, (PALETTE_SLOT)nSlotIndex);
}

void XUISystem::SetPaletteItem(PALETTE_ITEM_TYPE _type, int nItemIDorTalentID)
{
	int nSlotNum = gvar.MyInfo.PaletteList.GetCurPaletteListIndex();
	int nSlotIndex = gvar.MyInfo.PaletteList.GetEmptySlotIndex();
	if (nSlotIndex == -1) return;

	XPostPalette_PutUp( (PALETTE_NUM)nSlotNum, (PALETTE_SLOT)nSlotIndex, _type, nItemIDorTalentID);
}

void XUISystem::SetSilver( int nSilver )
{
	int _money = gvar.MyInfo.ChrInfo.GetMoney();
	gvar.MyInfo.ChrInfo.SetMoney(nSilver);

	if (global.ui  &&  _money != nSilver)
		global.ui->InventoryMoneyRefresh();
}

int XUISystem::GetCrossHairListCount()
{
	return m_crossHair.GetCrossHairCount();
}

std::wstring XUISystem::GetCrossHairName( int nIndex )
{
	return m_crossHair.GetCrossHairName(nIndex);
}

std::wstring XUISystem::GetCureentCrossHairName()
{
	return m_crossHair.GetCurrentCrossHairName();
}

std::wstring XUISystem::GetCureentCrossHairPickName()
{
	return m_crossHair.GetCurrentCrossHairPickName();
}

std::wstring XUISystem::GetCurrentCrossHairAwaysName()
{
	return m_crossHair.GetCurrentCrossHairAwaysName();
}
