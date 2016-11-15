#include "stdafx.h"
#include "XLoginFrame.h"
#include "XCampaign.h"
#include "XWidgetNameDef.h"
#include "XGameFrameUILoader.h"
#include "XCommonFunction.h"
#include "XGameInitializer.h"
#include "XBaseApplication.h"
#include "XNetwork.h"


// XLoginFrame
XLoginFrame::XLoginFrame()
: XBaseFrame(), m_bUpdateOnce(false), m_bLoaded(false), m_bLogined(false)
{
	m_bLoadCampaign = false;
}


// ~XLoginFrame
XLoginFrame::~XLoginFrame()
{
	// Unload loading campaign
	XGetLoginCampaign()->Destroy();
}


// Open
bool XLoginFrame::Open( int nPrevFrameID, const wchar_t* szArgs)
{
#ifdef _PUBLISH

	if (nPrevFrameID == CHAR_FRAME_ID)
	{
		PostMessage( global.app->GetWindowHandle(), WM_CLOSE, 0, 0);
		return false;
	}

#endif

	dlog( "Enter frame : Login\n");

	m_bUpdateOnce = false;
	m_bLoaded = false;
	m_bLoadCampaign = false;
	m_bLogined = false;

	global.app->GetInitializer()->Init_Pre_LoadingLoginCampaign();


	// Load UI
	XGameFrameUILoader ui_loader;
	ui_loader.Load( UI_LOGIN_SHEET_NAME);
	global.ui->ShowSheet( UI_LOGIN_SHEET_NAME);

	// Call event
	if ( global.script)		global.script->GetGlueGameEvent().OnLoginEvent( "UI", "ENTER");
	if ( global.script)		global.script->GetGlueGameEvent().OnLoginEvent( "UI", "LOADING");

	if (!gvar.Game.strLastErrorMsg.empty())
	{
		global.script->GetGlueGameEvent().OnLoginEvent( "LOGIN_ERROR", MLocale::ConvUTF16ToAnsi(gvar.Game.strLastErrorMsg.c_str()));
		gvar.Game.strLastErrorMsg.clear();
	}


	if ( gvar.Network.bDirectLogin == false)
		global.net->PostConnectToLocatorServer( gvar.Network.vecLocatorIPList);

	return true;
}

void XLoginFrame::Load()
{
	global.app->GetInitializer()->Init_While_LoadingLoginCampaign();

	// Load loading campaign
	m_bLoadCampaign = XGetLoginCampaign()->Create();

	if ( global.script && !m_bLoadCampaign)		global.script->GetGlueGameEvent().OnLoginEvent( "UI", "LOADED");
}

// Close
void XLoginFrame::Close( int nNextFrameID)
{
	dlog( "Leave frame : Login\n");


	// Update에서 Init_While_LoadingLoginCampaign 함수가 안불릴 수 있으므로 여기서 확인차 다시 재호출
	global.app->GetInitializer()->Init_While_LoadingLoginCampaign();


	// Call event
	if ( global.script  &&  global.ui->IsExistSheet( UI_LOGIN_SHEET_NAME) == true)
		global.script->GetGlueGameEvent().OnLoginEvent( "UI", "LEAVE");


	// Check next frame ID
	if ( nNextFrameID != CHAR_FRAME_ID)
	{
		// Delete sheet
		global.ui->DeleteSheet( UI_LOGIN_SHEET_NAME);
		global.ui->DeleteSheet( UI_CHAR_SHEET_NAME);


		// Unload loading campaign
		XGetLoginCampaign()->Destroy();
	}
}


// OnEvent
bool XLoginFrame::OnEvent( MEvent& e)
{
	// Process UI event
	if ( global.ui->OnEvent( e) == true)				return true;

	return false;
}


// Update
void XLoginFrame::Update( float fDelta)
{
	// Show cursor
	if ( global.ui  &&  global.ui->IsMouseVisible() == false)		global.ui->ShowMouse( true);

	// 리소스 읽기 - 한번 로딩화면을 그려준 후에 실행하기 위하여 여기에서 Load
	if (UpdateOnce(fDelta) == true) return;

	// Check loading completed	
	if ( m_bLoadCampaign == true  &&  XGetLoginCampaign()->IsLoadingCompleted() == true)
	{
		if ( global.script)		global.script->GetGlueGameEvent().OnLoginEvent( "UI", "LOADED");
		m_bLoadCampaign = false;
	}


	// Check Direct login	
	if (m_bLogined == false)
	{
		if ( ( gvar.Network.bDirectLogin == true) && ( false ==  m_bLoadCampaign ) )
		{	
			if (gvar.Network.bWaitDirectLogin == false)
			{	
				XCommonFunction::Connect( gvar.Network.szLoginID, gvar.Network.szPassword, gvar.Network.szIP);			
				gvar.Network.bIsAlreadyDirectLogined = true;
				m_bLogined = true;
			}
		}
	}


	// Update base
	XBaseFrame::Update( fDelta);


	// Update login campaign
	XGetLoginCampaign()->Update( fDelta);
}


// OnRender
void XLoginFrame::OnRender()
{
	if ( !m_bLoaded || XGetLoginCampaign()->IsLoadingCompleted() == false)
	{
		global.mint->Draw();
		return;
	}


	// Render base
	XBaseFrame::OnRender();


	// Render campaign
	XGetLoginCampaign()->Render();
}

bool XLoginFrame::UpdateOnce( float fDelta )
{
	if (!m_bUpdateOnce)
	{
		m_bUpdateOnce = true;

		// Update base
		XBaseFrame::Update( fDelta);

		return true;
	}

	if (!m_bLoaded) 
	{
		Load();
		m_bLoaded = true;
	}

	return false;
}
