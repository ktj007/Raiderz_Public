#include "stdafx.h"
#include "XBaseApplication.h"
#include "XEngine.h"
#include "XConst.h"
#include "XPlayFrame.h"
#include "XLoginFrame.h"
#include "XCharFrame.h"
#include "XReplayFrame.h"
#include "XMintDesignFrame.h"
#include "MiNet.h"
#include "XGlobalInstance.h"
#include "XStrings.h"
#include <shlwapi.h>
#include "RConfiguration.h"
#include "MCrashDump.h"
#include "XSceneManager.h"
#include "XTalentEvent.h"
#include "XSystem.h"
#include "XProfiler.h"
#include "SVNRevision.h"
#include "CAppHelper.h"
#include "XGameInterface.h"
#include "MMainFrame.h"
#include "XCameraManager.h"
#include "XFeedbackSystem.h"
#include "XPath.h"
#include "XResourceManager.h"
#include "XResourceRefMgr.h"
#include "XAlwaysResourceGrabber.h"
#include "XCrashReporter.h"
#include "XArgumentsParser.h"
#include "XApplicationConfig.h"
#include "XGameInitializer.h"
#include "XAntiHack.h"
#include "XEngineDebugFrame.h"
#include "XEngineDebugger.h"
#include "XAppErrorCollector.h"
#include "RShadowStrategy.h"
#include "XFreeCamTracer.h"
#include "CLangInfo.h"


// 기본 그림자 옵션
RSHADOWMETHOD	DEFAULT_SHADOW_METHOD = SHADOW_PSSM;
int				DEFAULT_SHADOW_BUFFER_SIZE = 512;
int				DEFAULT_SHADOW_BUFFER_COUNT = 3;
//RSHADOWFILTERMETHOD	DEFAULT_SHADOW_FILTER_METHOD = SF_PCF;
RSHADOWFILTERMETHOD	DEFAULT_SHADOW_FILTER_METHOD = SF_POISSON;

XBaseApplication::XBaseApplication() : RD3DApplication(), m_nMode(EM_GAME), m_pGlobalInstance(NULL), m_pGlobalInfoInstance(NULL)
, m_Mutex(NULL)
{
	m_szCmdLine[0] = 0;
	m_fLossElapsedTime = 0.0f;
	m_fillMode	= RFILL_SOLID;
	SetClearColor( 0xFF000000 );

	m_pInitializer = new XGameInitializer();
}

void XBaseApplication::CreateGlobal()
{
	m_pGlobalInstance = new XGlobalInstance();
	m_pGlobalInfoInstance = new XGlobalInfoInstance();
}

XBaseApplication::~XBaseApplication()
{
	SAFE_DELETE(m_pGlobalInfoInstance);
	SAFE_DELETE(m_pGlobalInstance);

	global.SetNULL();
	info.SetNull();

	SAFE_DELETE(m_pInitializer);

	if (m_Mutex) {
		ReleaseMutex(m_Mutex);	
		m_Mutex = NULL;
	}
}

void XBaseApplication::InitPath()
{
	// Current Directory를 맞춘다.
	wchar_t szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);
}

void XBaseApplication::Init( LPWSTR lpCmdLine )
{
	CreateGlobal();

	m_pInitializer->Init_Pre_CreatingWindow(m_pGlobalInfoInstance, m_pGlobalInstance);

//	InitPath();					// WinMain 처음으로 이동. 가장 먼저 처리되어야 할 구문
//	InitDebug();

	//if (true == XCONST::CONST_MAKE_COMMAND_RESULT)
	//	XStrings::SaveCommandResultTable();

	m_CommandLine.ParseFromCommandLine();
	wcscpy_s(m_szCmdLine, lpCmdLine);

}


void XBaseApplication::InitDebug()
{
	return;

	// 이런식으로 시스템 정보를 로그로 남기면 됩니다. - bird
	vector<MLogSysInfo>		vecSysInfos;
	MLogSysInfo info[3];
	info[0].strName = "Operating environment";
	info[0].strValue = "Windows 5.1";
	info[1].strName = "CPU";
	info[1].strValue = "Intel(R) Pentium(R) D CPU 2.80GHz";
	info[2].strName = "Display Device";
	info[2].strValue = "NVIDIA GeForce 6600        Driver 8.1.9.8";
	vecSysInfos.push_back(info[0]);
	vecSysInfos.push_back(info[1]);
	vecSysInfos.push_back(info[2]);

	MGetDefaultLogger()->OutputSysInfo(vecSysInfos);

}

bool XBaseApplication::CheckSecondWindow()
{
	m_Mutex = CreateMutex(NULL, TRUE, APPLICATION_NAME);
	if (GetLastError() == ERROR_ALREADY_EXISTS)	{
#ifndef _MADDUCK
		//CheckMoveSecondTestWindow();
#endif
		return false;
	}

	return true;
}

void CustomPureCallHandler()
{
	RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, NULL);
}

void CustomInvalidParameterHandler(const wchar_t *, const wchar_t *, const wchar_t *, unsigned int, uintptr_t)
{
	RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, NULL);
}

// Exception Handler 테스트 코드
//#pragma warning(disable: 4355)
//class CDerived;
//class CBase
//{
//public:
//	CBase(CDerived *derived): m_pDerived(derived) {};
//	~CBase();
//	virtual void function(void) = 0;
//
//	CDerived * m_pDerived;
//};
//class CDerived : public CBase
//{
//public:
//	CDerived() : CBase(this) {};   // C4355
//	virtual void function(void) {};
//};
//CBase::~CBase()
//{
//	m_pDerived -> function();
//}


bool XBaseApplication::OnCreate()
{
	// set handler
	_set_purecall_handler(CustomPureCallHandler);
	_set_invalid_parameter_handler(CustomInvalidParameterHandler);

	// Exception Handler 테스트 코드
	//{
	//	CDerived myDerived;
	//}
	//std::vector<int> error_test;
	//int error_int = error_test[10];


	// Set window handle
	XGetAppErrorCollector()->SetWindowHandle( m_hWnd);
	XGetAntiHack()->CreatedWindow( m_hWnd);


	SetCursor(LoadCursor(NULL, IDC_ARROW));

	InitWindowCaption();
	CheckSecondWindow();
	DeclGlobalVals();

	CreateGameFrames();

	bool bInitialiezerRet = m_pInitializer->Init_Post_CreatingWindow(this, m_pGlobalInfoInstance, m_pGlobalInstance);

	if ( bInitialiezerRet == false)
	{
		mlog("CreatingWindow failed.\n");
		return false;
	}

	if (StartGameFrame() == false)
	{
		mlog("StartGameFrame failed.\n");
		return false;
	}

	return true;
}

void XBaseApplication::OnDestroy()
{
	// Set window handle
	XGetAntiHack()->DestroyedWindow();


	//로거에게 멈춰라! 라고 알려줌
	MFinalizeLog();

	// frame destruction
	m_GameFrameMgr.DeleteAll();

	//m_EffectManager.Destroy();

	m_pGlobalInstance->Fini();
}

void XBaseApplication::UpdateTimer()
{
	float fDelta = global.system->GetTimer().Update();

	// 엔진 업데이트 시간에서 나오는 손실을 구해서 다음에 반영한다.
	float fElapsed = (fDelta + m_fLossElapsedTime) * 1000.0f;
	DWORD dwElapsed = (fDelta + m_fLossElapsedTime) * 1000;

	m_pSceneManager->UpdateTimerCustom(dwElapsed);

	m_fLossElapsedTime = (fElapsed - dwElapsed) / 1000.0f;
}

static float fSecPerFrame = 1.0f / 60.0f; 
static float fTimer = 0;
void XBaseApplication::OnUpdate()
{
	PFC_THISFUNC;

	// Update application error collector
	XGetAppErrorCollector()->Update();


	float fDelta = global.system->GetTimer().GetDelta();

	// update debugger routine before game update
	#ifndef _PUBLISH
	XEngineDebugger::UpdateBeforeGame(fDelta);
	#endif


	// Update free camera tracer
	XGetFreeCamTracer()->Update();


	m_GameFrameMgr.Update(fDelta);


	// Update anti-hack module
	XGetAntiHack()->Update();


#ifdef _KOREA
//	m_YouthSelfRegulation.OnUpadate();
#endif // _KOREA

	//	RD3DApplication::OnUpdate();
}

void XBaseApplication::OnActive(bool bActive)
{
	RD3DApplication::OnActive(bActive);
	m_GameFrameMgr.OnActive(bActive);
}

void XBaseApplication::OnPreRender()
{
	PFC_THISFUNC;

	m_GameFrameMgr.OnPreRender();

	if (global.system && global.phx)
	{
		global.phx->UpdateXPhysX(global.system->GetTimer().GetDelta());
	}
}

void XBaseApplication::OnRender()
{
	PFC_THISFUNC;

	m_GameFrameMgr.OnRender();
}

bool XBaseApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	switch (message)
	{
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		{
			// alt 키 무시
			int key;
			key = wParam;

			if (key == VK_MENU)
			{
				*pReturn = 1;
				return true;
			}
		}
		break;
	}


	MEvent e;
	int nResult = e.TranslateEvent(hWnd, message, wParam, lParam);

	if (nResult&EVENT_MINT_TRANSLATED)
	{
		PFC_THISFUNC;
		if (m_GameFrameMgr.OnEvent(e) == true) return true;
	}

	return false;
}

void XBaseApplication::ChangeFrame(int nFrameID)
{
	m_GameFrameMgr.Change(nFrameID);
}

int XBaseApplication::GetCurrGameFrame()
{
	return m_GameFrameMgr.GetCurrFrameID();
}

void XBaseApplication::MessageToFrame( const wchar_t* szMsg )
{
	m_GameFrameMgr.Message(szMsg);
}

bool XBaseApplication::ResetDevice()
{
	if( RD3DApplication::ResetDevice() == false )
		return false;

	// Alt+Enter 단축키로 직접 풀 스크린 전환이 되는 경우가 있어서 설정을 한번 더 해줍니다.
	// 차라리 단축키를 빼는게 낫지 않을까요?
	bool _fullscr = XGETCFG_VIDEO_FULLSCREEN;
	XSETCFG_VIDEO_FULLSCREEN( rs3::REngine::GetConfig().m_bFullScreen);
	if ( _fullscr != XGETCFG_VIDEO_FULLSCREEN)
		XGetGameInterface().OPTIONS.SaveOption();


	// Mint 초기화
	if ( m_pGlobalInstance->ui->GetMint())
	{
		int width = rs3::REngine::GetConfig().m_nWidth;
		int height = rs3::REngine::GetConfig().m_nHeight;
		m_pGlobalInstance->ui->SetWorkspaceSize(width, height);
		m_pGlobalInstance->ui->GetMint()->SetWorkspaceSize( width, height);
		m_pGlobalInstance->ui->GetMint()->GetMainFrame()->SetSize( width, height);
	}

	return true;
}

void XBaseApplication::DeclGlobalVals()
{
	global.app = this;
	global.device = (rs3::RDeviceD3D*)(REngine::GetDevicePtr());
	global.smgr = REngine::GetSceneManagerPtr();
	global.gfmgr = &m_GameFrameMgr;

	global.system = m_pGlobalInstance->system;
	global.sound = m_pGlobalInstance->sound;
	global.script = m_pGlobalInstance->script;
	global.ui = m_pGlobalInstance->ui;
	global.mint = m_pGlobalInstance->ui->GetMint();
	global.net = m_pGlobalInstance->net;
	global.engine = m_pGlobalInstance->engine;
	global.history = m_pGlobalInstance->history;
	global.camera = m_pGlobalInstance->camera;
	global.emgr = m_pGlobalInstance->emgr;
	global.res = m_pGlobalInstance->res;
	global.phx = m_pGlobalInstance->phx;
	global.bandi_capturer = m_pGlobalInstance->bandi_capturer;
	global.feedback = m_pGlobalInstance->feedback;
	global.msg = m_pGlobalInstance->msg;
	

	// info

	info.item = m_pGlobalInfoInstance->item;
	info.npc = m_pGlobalInfoInstance->npc;
	info.field = m_pGlobalInfoInstance->field;
	info.quest = m_pGlobalInfoInstance->quest;
	info.talent = m_pGlobalInfoInstance->talent;
	info.talent_effect = m_pGlobalInfoInstance->talent_effect;
	info.recipe = m_pGlobalInfoInstance->recipe;
	info.mesh_info = m_pGlobalInfoInstance->mesh_info;
	info.motion = m_pGlobalInfoInstance->motion;
	info.buff = m_pGlobalInfoInstance->buff;
	info.textureMt = m_pGlobalInfoInstance->textureMt;
	info.effect_Info = m_pGlobalInfoInstance->effect_Info;
	info.colorTable = m_pGlobalInfoInstance->colorTable;
	info.swordtrail_sampling = m_pGlobalInfoInstance->swordtrail_sampling;
	info.faction	= m_pGlobalInfoInstance->faction;
	info.cond	= m_pGlobalInfoInstance->cond;
	info.cond_item	= m_pGlobalInfoInstance->cond_item;
	info.item_droper = m_pGlobalInfoInstance->item_droper;
	info.item_questrewarder = m_pGlobalInfoInstance->item_questrewarder;
	info.item_recipeproduct = m_pGlobalInfoInstance->item_recipeproduct;
	info.chat_channel = m_pGlobalInfoInstance->chat_channel;
	info.model_info = m_pGlobalInfoInstance->model_info;
	info.dialog_text = m_pGlobalInfoInstance->dialog_text;
	info.recipenpc = m_pGlobalInfoInstance->recipenpc;
	info.lang = m_pGlobalInfoInstance->lang;
}

bool XBaseApplication::StartGameFrame()
{
	//////////////////////////////////////////////////////////////////////////

	// 엔진 디버그모드
	// argument가 sln의 솔루션 구성별로 따로 저장되지 않아 전처리기를 씁니다. by pok
#ifdef _DEBUG_ENGINE
	XBaseFrame* pFrame = m_GameFrameMgr.GetFrameFromName(L"engine_debug");
	if (pFrame)
	{
		int nFrameID = pFrame->GetID();
		m_nMode = EM_GAME;
		return m_GameFrameMgr.Start(nFrameID, NULL);
	}
#endif

	//////////////////////////////////////////////////////////////////////////

	if (ParseArguments() == true)
	{
		return true;
	}

	mlog("ParseArguments failed.\n");


#ifdef _PUBLISH
	XGetAppErrorCollector()->SetLastError( ERRORTYPE_APPLICATION, L"ParseArguments failed.");
	return false;
#endif

	// 아무 인자가 없으면 로그인 창으로 이동
	m_nMode = EM_GAME;
	return m_GameFrameMgr.Start(START_FRAME_ID);
}

bool XBaseApplication::ParseArguments()
{
	const wchar_t* szArgs = GetArguments();

#ifndef _PUBLISH
	mlog("argument: %s\n", MLocale::ConvUTF16ToAnsi(szArgs).c_str());
#endif

	XArgumentsParser parser;
	XArgumentsParserResult result = parser.Parse(szArgs, GetCommandLine(), &m_GameFrameMgr);

	m_nMode = result.nExecuteMode;

	if (result.strGameFrameArgs.empty())
	{
		return m_GameFrameMgr.Start(result.nStartGameFrameID);
	}
	else
	{
		return m_GameFrameMgr.Start(result.nStartGameFrameID, result.strGameFrameArgs.c_str());
	}

	return false;
}

// 테스트로 두번째 띄우는 윈도우면 오른쪽 하단에 위치하도록 한다.
void XBaseApplication::CheckMoveSecondTestWindow()
{
#ifdef _PUBLISH
	return;
#endif

	int x, y;

	RECT winrt;
	HWND hwnd=::FindWindow(TEXT("Progman"), TEXT("Program Manager"));
	hwnd=FindWindowEx(hwnd, NULL, TEXT("SHELLDLL_DefView"), NULL);
	::GetWindowRect(hwnd, &winrt);

	x = winrt.right-winrt.left - REngine::GetConfig().m_nWidth - 5;
	y = winrt.bottom-winrt.top - REngine::GetConfig().m_nHeight - 30;

	AdjustWindowSizeAndShowWindow(x, y, REngine::GetConfig().m_nWidth,REngine::GetConfig().m_nHeight,REngine::GetConfig().m_bFullScreen);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 적절한 씬 매니저 생성
//
RSceneManager* XBaseApplication::CreateSceneManager()
{
	RSceneManager* pSceneManager = new XSceneManager;
	REngine::RegisterSceneManager(pSceneManager);
	return pSceneManager;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO : 씬 매니저 new 후에 설정해주어야 할 것들은 이곳에 해줍니다.
//
bool XBaseApplication::OnCreateSceneManager()
{
	//////////////////////////////////////////////////
	// create scene manager creation
	//
	if(!m_pSceneManager->Create(m_pDevice))
		return false;

	RShadowDesc desc;
	desc.eMethod = SHADOW_NONE;
	m_pSceneManager->SetRealtimeShadowMethod(desc);
	m_pSceneManager->SetShadowFilterType(DEFAULT_SHADOW_FILTER_METHOD);
	
	bool bRet = REngine::InitSharedResource();

	if (bRet == false)
	{
		_ASSERT(bRet);
		mlog("Failed To Init Shared Resource\n");
	}

	return bRet;
}

void XBaseApplication::OnDestroySceneManager()
{
	if (m_pSceneManager)
	{
		// 이펙트 리셋
		if(global.emgr)
			global.emgr->Destroy();

		m_pSceneManager->Clear();

		if( global.engine )
			global.engine->DestroySharedResource();

		if( global.res )
			global.res->Fini(global.device);

		REngine::DestroySharedResource();
		m_pSceneManager->Destroy();
	}
}

void XBaseApplication::UpdateProfileInfoCallback(const wchar_t* szText,void* pUserPointer)
{
	PFC_THISFUNC;
	_ScrLine3(0,L"%s",szText);
}

int XBaseApplication::TestRun()
{
	mlog("Application started\n");
	OnInit();

	if(!Create()) {
		mlog("Application failed to create\n");
		Destroy();
		return 0;
	}
	OnCreate();

	m_pSceneManager->ResetTimer();


	return 0;
}

void XBaseApplication::TestDestroy()
{
	Destroy();
}

void XBaseApplication::Quit( int nRet )
{
	PostMessage(GetWindowHandle(),WM_CLOSE, nRet, 0);
}


void XBaseApplication::OnPreFlip()
{
	m_GameFrameMgr.OnPreFlip();
}


void XBaseApplication::InitWindowCaption()
{
	wstring strCaption = CAppHelper::MakeTitleCaption(APPLICATION_NAME, GetProductVersion(), GetSVNRevision(), GetSVNDate());
	SetWindowText(m_hWnd, strCaption.c_str()); 
}

void XBaseApplication::CreateGameFrames()
{
	// 게임 프레임 생성
	m_GameFrameMgr.Add(new XLoginFrame());
	m_GameFrameMgr.Add(new XCharFrame());
	m_GameFrameMgr.Add(new XPlayFrame());

	// 엔진 디버깅 전용 프레임
#ifdef _DEBUG_ENGINE
	m_GameFrameMgr.Add(new XEngineDebugFrame());
#endif

#ifndef _PUBLISH
	m_GameFrameMgr.Add(new XReplayFrame());
	m_GameFrameMgr.Add(new XTestFrame());
	m_GameFrameMgr.Add(new XMintDesignFrame());
	m_GameFrameMgr.Add(new XHelloWorldFrame());
#endif
}

DWORD XBaseApplication::GetWindowStyle( bool bFullScreen )
{
	DWORD dwWindowStyle;

	if (bFullScreen)
	{
		dwWindowStyle = WS_POPUP;
	}
	else
	{
		dwWindowStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	}

	return dwWindowStyle;
}

void XBaseApplication::SetClipboard( const wchar_t* szBuf )
{
	const int nLen = wcslen(szBuf);

	if(OpenClipboard(0)) 
	{
		EmptyClipboard();

		HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE |GMEM_DDESHARE, nLen+4);

		wchar_t* ptr = (wchar_t *)GlobalLock(hText);

		wcscpy_s(ptr, nLen + 4, szBuf);
		GlobalUnlock(hText);

		::SetClipboardData(CF_TEXT, hText);

		CloseClipboard();

		GlobalFree(hText);
	}

}

wstring XBaseApplication::ParseFileNameFromArguments()
{
	wchar_t szFileName[MAX_PATH] = L"";
	size_t nLength;

	const wchar_t* szArgs = GetArguments();

	// 따옴표 있으면 제거한다
	if(szArgs[0] == L'"') 
	{
		wcscpy_s(szFileName,szArgs+1);

		nLength = wcslen(szFileName);
		if(szFileName[nLength-1] == L'"')
		{
			szFileName[nLength-1]=0;
		}
	}
	else
	{
		wcscpy_s(szFileName,szArgs);

		wchar_t seps[] = L" ";
		wchar_t* token;
		wchar_t* nextToken;

		token = wcstok_s(szFileName, seps, &nextToken);
		if(token!=NULL)
		{
			wcscpy_s(szFileName, token);
		}
	}
	return wstring(szFileName);
}

void XBaseApplication::DeviceSetting()
{
	m_pDevice->SetFillMode(m_fillMode);
}

void XBaseApplication::ToggleSolidWire()
{
	m_fillMode = (m_fillMode == RFILL_SOLID ? RFILL_WIRE : RFILL_SOLID);
}

bool XBaseApplication::InitGlobalInstance()
{
	bool ret = m_pGlobalInstance->Init(GetWindowHandle());
	return ret;
}

char* XBaseApplication::GetName()
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(APPLICATION_NAME);

	return (char*)gvar.Temp.szTemp.c_str();
}

