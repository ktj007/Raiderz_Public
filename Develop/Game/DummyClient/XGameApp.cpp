#include "stdafx.h"
#include "XGameApp.h"
#include "resource.h"
#include "version.h"
#include "SVNRevision.h"
#include "XDummyFrame.h"
#include "XGlobalInstance.h"
#include "XGameInterface.h"
#include "XGameInterfaceOption.h"
#include "XMotionLogic.h"
#include "XResourceManager.h"

XGameApp::XGameApp() : XBaseApplication()
{
	m_nIconResID = IDI_ICON1;
}


XGameApp::~XGameApp()
{
}

const wchar_t* XGameApp::GetProductVersion()
{
	USES_CONVERSION_EX;

	static wstring strVersion;
	strVersion = A2W_EX(PRODUCT_VERSION, 0);

	return strVersion.c_str();
}

const wchar_t* XGameApp::GetSVNRevision()
{
	USES_CONVERSION_EX;

	static wstring strRevision;
	strRevision = A2W_EX(SVN_REVISION, 0);

	return strRevision.c_str();
}

const wchar_t* XGameApp::GetSVNDate()
{
	USES_CONVERSION_EX;

	static wstring strSvnDate;
	strSvnDate = A2W_EX(SVN_DATE, 0);

	return strSvnDate.c_str(); 
}

void XGameApp::CreateGameFrames()
{
	XBaseApplication::CreateGameFrames();
	m_GameFrameMgr.Add(new XDummyFrame());
}

bool XGameApp::ParseArguments()
{
	if (XBaseApplication::ParseArguments()) return true;

	// 파라미터가 리플레이 파일명인지 확인한다
	wstring strFileName = ParseFileNameFromArguments();
	size_t nLength = strFileName.length();

	if(wcsicmp(strFileName.c_str() + nLength - wcslen(FILEEXT_DUMMYCLIENT), FILEEXT_DUMMYCLIENT)==0)
	{
		m_nMode = EM_DUMMYCLIENT;
		return m_GameFrameMgr.Start(DUMMYCLIENT_FRAME_ID, strFileName.c_str());
	}

	return true;
}


bool XGameApp::OnCreate()
{
	InitWindowCaption();
	DeclGlobalVals();

	if ( InitGlobalInstance() == false)
		return false;

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	CreateGameFrames();

	// 게임 옵션 설정
	XGetGameInterface().OPTIONS.UpdateSystemOption();
	XMotionLogic::BindMotionEvents();

	if (StartGameFrame() == false)
	{
		return false;
	}

	return true;
}

bool XGameApp::InitGlobalInstance()
{
	return m_pGlobalInstance->InitForDummyClient(GetWindowHandle());
}