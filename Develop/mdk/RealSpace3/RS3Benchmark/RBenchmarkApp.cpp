#include "stdafx.h"
#include "MFileSystem.h"
#include "REngine.h"
#include "RSceneManager.h"
#include "RSceneNodeID.h"
#include "RWeatherEffectSet.h"
#include "REnvironmentSet.h"
//#include "RFont.h"
//#include "RTextSceneNode.h"
#include "RShadowStrategy.h"
#include "RProfiler.h"
#include "RBenchmarkApp.h"
#include "RBenchmarkConfig.h"

namespace rs3
{

RBenchmarkApp::RBenchmarkApp() : RD3DApplication(),
// m_pFont(NULL)
//,m_pTextSceneNodeNode(NULL)
 m_pCameraSceneNode(NULL)
,m_pWorldSceneNode(NULL)
,m_pWeatherEffectSet(NULL)
,m_pEnvSet(NULL)
,m_pBenchmarkConfig(NULL)
,m_pNVPerf(NULL)
,m_bBeginNVPerf(false)
,m_fCameraX(0.f)
,m_fCameraZ(0.f)
{
}

RBenchmarkApp::~RBenchmarkApp()
{
	_ASSERT( NULL == m_pCameraSceneNode );
	_ASSERT( NULL == m_pWeatherEffectSet );
	_ASSERT( NULL == m_pEnvSet );
	_ASSERT( NULL == m_pWorldSceneNode );
	_ASSERT( NULL == m_pNVPerf );

	SAFE_DELETE( m_pBenchmarkConfig );
}
 
char* RBenchmarkApp::GetName()
{
	return "RS3 Benchmark Application - Press F1 for help";
}

bool RBenchmarkApp::OnInit()
{
	Config();
	return true;
}

bool RBenchmarkApp::OnCreate()
{
	_ASSERT( m_pBenchmarkConfig );

	if( false == CreatePerformanceMonitor() )
		return false;

	CreateScene( m_pBenchmarkConfig->GetSceneFileName() );
	CreateCameraSceneNode( m_pWorldSceneNode );
	CreateEnvironment( m_pBenchmarkConfig->GetSceneEnvFileName() );
	CreateFontAndTextSceneNode();

	SetShadowConfig();
	SetPostEffectConfig();
	return true;
}

void RBenchmarkApp::OnDestroy()
{
	DestroyFontAndTextSceneNode();
	DestroyEnvironment();
	DestroyCameraSceneNode();
	DestroyScene();
	DestroyPerformanceMonitor();
}

void RBenchmarkApp::OnUpdate()
{
	_ASSERT( m_pBenchmarkConfig );
	if( m_pBenchmarkConfig->GetNavigationMode() )
	{
		CameraMove();
	}

//	char szBuffer[MAX_PATH];
//	sprintf_s( szBuffer, "FPS %3.1f camera ( %3.3f, %3.3f, %3.3f )",GetDevice()->GetFrameRate(), cameraPos.x,cameraPos.y,cameraPos.z);
//	sprintf_s( szBuffer, "FPS %3.1f", REngine::GetDevice().GetFrameRate() );
//	SetText( szBuffer );
}

void RBenchmarkApp::OnPreRender()
{

}

void RBenchmarkApp::OnRender()
{

}

void RBenchmarkApp::OnPostRender()
{

}

int RBenchmarkApp::MainLoop()
{
	RPFC_RESET;

	MSG  msg;

	do
	{
		RPFC_B("rs3::RBenchmarkApp::MainLoop");

		//RPFC_B("rs3::RBenchmarkApp::MainLoop::Sleep");
		//if(!m_bActive)
		//	Sleep(10);
		//RPFC_E;

		bool bExistMessage = true;
		if( !ProcessMessage(msg) )
		{
			Update();

			if( m_bBeginNVPerf && m_pNVPerf )
			{
				m_pNVPerf->RunExperiment( this );
				m_bBeginNVPerf = false;
			}
			else
			{
				Render();
			}
			bExistMessage = false;
		}
		RPFC_E;

		if( false == bExistMessage )
		{
#ifdef _USE_ENGINE_PROFILER
			m_DebuggingInfo.SendOut(m_hWnd);
#endif
		}

		GetProfileManager().ResetCurrentInfo();

	}while( WM_QUIT != msg.message );

	return (INT)msg.wParam;
}

bool RBenchmarkApp::OnWndMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pReturn )
{
	switch (message)
	{
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_F1:
			ShowHelpMessage();
			break;
		case VK_F2:
			ToggleNavigationMode();
			break;
		case VK_F3:
			SaveCameraTransform();
			break;
		case VK_F4:
			LoadCameraTransform();
			break;
		case VK_F5:
			ReloadConfig();
			break;
		case VK_F8:
			SaveConfig();
			break;
		}
		break;

	case WM_CHAR:
		switch( wParam )
		{
		case VK_ESCAPE:
			PostMessage( hWnd, WM_CLOSE, 0, 0 );
			break;

		//case '0':
		//	m_bBeginNVPerf = true;
		//	break;
		}

		break;
	};

	return false;
}

void RBenchmarkApp::SaveCameraTransform()
{
	_ASSERT( m_pBenchmarkConfig );
	_ASSERT( m_pCameraSceneNode );
	m_pBenchmarkConfig->SaveCameraTransform( m_pCameraSceneNode->GetPosition(), m_pCameraSceneNode->GetDirection() );
}

void RBenchmarkApp::LoadCameraTransform()
{
	_ASSERT( m_pBenchmarkConfig );
	_ASSERT( m_pCameraSceneNode );
	m_pCameraSceneNode->SetTransform( m_pBenchmarkConfig->GetCameraPosition(), m_pBenchmarkConfig->GetCameraDirection() );
}

void RBenchmarkApp::Config()
{
	_ASSERT( NULL == m_pBenchmarkConfig );
	m_pBenchmarkConfig = new RBenchmarkConfig;
	_ASSERT( m_pBenchmarkConfig );

	m_pBenchmarkConfig->LoadConfig();

	MCreateFileSystem( MFILEACCESS_GENERIC, m_pBenchmarkConfig->GetFilePathString().c_str() );

	SetScreenConfig( false );
}

void RBenchmarkApp::SaveConfig()
{
	_ASSERT( m_pBenchmarkConfig );
	m_pBenchmarkConfig->SaveConfig();
}

void RBenchmarkApp::ReloadConfig()
{
	_ASSERT( m_pBenchmarkConfig );
	if( m_pBenchmarkConfig->ReloadConfig() )
	{
		SetScreenConfig( true );
		SetCameraConfig();
		SetShadowConfig();
		SetPostEffectConfig();
	}
}

void RBenchmarkApp::SetScreenConfig( bool bResetDevice )
{
	_ASSERT( m_pBenchmarkConfig );

	bool bNeedResetDevice = false;
	bool bNeedToggleFullScreen = false;
	if( bResetDevice )
	{
		if( REngine::GetConfig().m_nWidth != m_pBenchmarkConfig->GetScreenWidth() )
			bNeedResetDevice = true;

		if( REngine::GetConfig().m_nHeight != m_pBenchmarkConfig->GetScreenHeight() )
			bNeedResetDevice = true;

		if( REngine::GetConfig().m_bFullScreen != m_pBenchmarkConfig->GetFullScreen() )
			bNeedToggleFullScreen = true;
	}

	REngine::GetConfig().m_nWidth = m_pBenchmarkConfig->GetScreenWidth();
	REngine::GetConfig().m_nHeight = m_pBenchmarkConfig->GetScreenHeight();
	REngine::GetConfig().m_bBackgroundCreateShaderComposite = false;

	if( bNeedToggleFullScreen )
	{
		ToggleFullScreen();
	}
	else
	if( bNeedResetDevice )
	{
		ResetDevice();
	}
}

void RBenchmarkApp::SetCameraConfig()
{
	if( NULL == m_pCameraSceneNode )
		return;

	m_pCameraSceneNode->Set( m_pBenchmarkConfig->GetCameraPosition(), m_pBenchmarkConfig->GetCameraDirection() );
	m_pCameraSceneNode->SetFov( m_pBenchmarkConfig->GetCameraFov(), m_pBenchmarkConfig->GetCameraAspectRatio() );
	m_pCameraSceneNode->SetNearFarZ( m_pBenchmarkConfig->GetCameraNear(), m_pBenchmarkConfig->GetCameraFar() );

	UpdateCameraParam();
}

void RBenchmarkApp::SetShadowConfig()
{
	RShadowDesc desc;

	if( false == m_pBenchmarkConfig->GetShadowEnable() )
	{
		desc.eMethod = SHADOW_NONE;
	}
	else
	{
		desc.eMethod = m_pBenchmarkConfig->GetShadowType();
		desc.nTextureSize = m_pBenchmarkConfig->GetShadowTextureSize();
		desc.nTextureNum = min( m_pBenchmarkConfig->GetShadowSplitCount(), 4 );

		for( int i = 0; i < 4; ++i )
			desc.fSplitDistance[i] = m_pBenchmarkConfig->GetShadowSplitDistance( i );
	}

	REngine::GetSceneManager().SetRealtimeShadowMethod( desc );
}

void RBenchmarkApp::SetPostEffectConfig()
{
	REngine::GetSceneManager().SetHDREnable( m_pBenchmarkConfig->GetHDREnable() );

	REngine::GetSceneManager().SetDOFEnable( m_pBenchmarkConfig->GetDOFEnable() );

	if( m_pBenchmarkConfig->GetDOFEnable() )
	{
		REngine::GetSceneManager().SetDOFFocalDist( m_pBenchmarkConfig->GetDOFFocusDist() );
		REngine::GetSceneManager().SetDOFFocalRange( m_pBenchmarkConfig->GetDOFFocusRange() );
		REngine::GetSceneManager().SetDOFCOCSize( m_pBenchmarkConfig->GetDOFCoCSize() );
	}

	REngine::GetSceneManager().SetDistortionEnable( m_pBenchmarkConfig->GetDistortionEnable() );

	REngine::GetSceneManager().SetSoftParticleEnable( m_pBenchmarkConfig->GetSoftParticleEnable() );
	if( m_pBenchmarkConfig->GetSSAOEnable() )
	{
		REngine::GetSceneManager().UseSSAO( m_pBenchmarkConfig->GetSSAOQuality() );
	}
	else
	{
		REngine::GetSceneManager().UseSSAO( 0 );
	}
}

void RBenchmarkApp::ShowHelpMessage()
{
	MessageBox( GetWindowHandle(),
		"F2: Toggle Navigation Mode\n"
		" Navigation Mode Control\n"
		"  Move: W S A D Ctrl Space\n"
		"  Rotate: Mouse, ArrowKeys\n"
		"  Accelerate: Shift\n"
		"F3: Save Camera Transform\n"
		"F4: Load Camera Transform\n"
		"F5: Reload Config File\n"
		"F8: Save Config File\n"
		, "Help", MB_OK );
}

void RBenchmarkApp::CreateCameraSceneNode( RWorldSceneNode* pWorldSceneNode )
{
	_ASSERT( pWorldSceneNode );

	_ASSERT( NULL == m_pCameraSceneNode );
	m_pCameraSceneNode = static_cast<RCameraSceneNode*>( REngine::GetSceneManager().CreateSceneNode( RSID_CAMERASCENENODE ) );
	_ASSERT( m_pCameraSceneNode );

	REngine::GetSceneManager().AddSceneNode( pWorldSceneNode, m_pCameraSceneNode, false );
	REngine::GetSceneManager().SetPrimaryCamera( m_pCameraSceneNode );

	SetCameraConfig();
}

void RBenchmarkApp::DestroyCameraSceneNode()
{
	if( m_pCameraSceneNode )
	{
		m_pCameraSceneNode->RemoveFromParent();
		SAFE_DELETE( m_pCameraSceneNode );
	}
}

void RBenchmarkApp::CreateFontAndTextSceneNode()
{
	//_ASSERT( NULL == m_pFont );
	//_ASSERT( NULL == m_pTextSceneNodeNode );

	//m_pFont = new RFont;
	//_ASSERT( m_pFont );
	//m_pFont->AddFontResourceFromFile( L"../data/interface/SFdhk.ttf" );

	//m_pTextSceneNodeNode = REngine::GetSceneManager().CreateSceneNode( RSID_TEXTSCENENODE );
	//_ASSERT( m_pTextSceneNodeNode );
	//m_pTextSceneNodeNode->SetFont( m_pFont );
	//m_pTextSceneNodeNode->SetText( "Font Test" );
	//m_pTextSceneNodeNode->SetPosition( 0, 0 );
	//REngine::GetSceneManager().AddSceneNode( m_pTextSceneNodeNode );
}

void RBenchmarkApp::DestroyFontAndTextSceneNode()
{
	//if( m_pTextSceneNodeNode )
	//{
	//	m_pTextSceneNodeNode->RemoveFromParent();
	//	SAFE_DELETE( m_pTextSceneNodeNode );
	//}
	//SAFE_DELETE( m_pFont );
}

void RBenchmarkApp::SetText( const char* pText )
{
	//m_pTextSceneNodeNode->SetText( szText );
}

void RBenchmarkApp::CreateScene( const string& sceneFileName )
{
	if( sceneFileName.empty() )
		return;

	_ASSERT( NULL == m_pWorldSceneNode );
	m_pWorldSceneNode = REngine::GetSceneManager().CreateRootSceneNode();
	_ASSERT( m_pWorldSceneNode );
	REngine::GetSceneManager().SetCurrentRootSceneNode( m_pWorldSceneNode );

	REngine::GetSceneManager().LoadWorldSceneNodeFromRootXMLFile( m_pWorldSceneNode, sceneFileName.c_str(), false );
}

void RBenchmarkApp::DestroyScene()
{
	if( m_pWorldSceneNode )
	{
		REngine::GetSceneManager().DeleteRootSceneNode( m_pWorldSceneNode );
		m_pWorldSceneNode = NULL;
	}
}

void RBenchmarkApp::CreateEnvironment( const string& envFileName )
{
	if( envFileName.empty() )
		return;

	_ASSERT( NULL == m_pWeatherEffectSet );
	m_pWeatherEffectSet = new RWeatherEffectSet();
	_ASSERT( m_pWeatherEffectSet );
	if( false == m_pWeatherEffectSet->LoadXML() )
		return;

	m_pEnvSet = new REnvironmentSet( m_pWeatherEffectSet );
	_ASSERT( m_pWorldSceneNode );
	if( false == REngine::GetSceneManager().AddSceneNode( m_pWorldSceneNode, m_pEnvSet ) )
		return;

	if( false == m_pEnvSet->CreateFromFile( envFileName.c_str(), false ) )
		return;

	m_pEnvSet->SetCurrentEnvSet( REnvironment::DAYTIME, REnvironment::SUNNY, false, 0.0f, false );
}

void RBenchmarkApp::DestroyEnvironment()
{
	if( m_pEnvSet )
	{
		m_pEnvSet->RemoveFromParent();
		SAFE_DELETE( m_pEnvSet );
	}
	SAFE_DELETE( m_pWeatherEffectSet );
}

bool RBenchmarkApp::CreatePerformanceMonitor()
{
	_ASSERT( NULL == m_pNVPerf );
	m_pNVPerf = new NVPerf;
	return true;
}

void RBenchmarkApp::DestroyPerformanceMonitor()
{
	if( m_pNVPerf )
	{
		SAFE_DELETE( m_pNVPerf );
	}
}

void RBenchmarkApp::ToggleNavigationMode()
{
	_ASSERT( m_pBenchmarkConfig );
	bool bNavigationMode = m_pBenchmarkConfig->ToggleNavigationMode();
	ShowCursor( !bNavigationMode );

	if( bNavigationMode )
	{
		UpdateCameraParam();
	}
}

void RBenchmarkApp::UpdateCameraParam()
{
	_ASSERT( m_pCameraSceneNode );

	RVector dirNormalized = Normalize( m_pCameraSceneNode->GetDirection() );
	m_fCameraX = acosf( dirNormalized.z );
	m_fCameraZ = acosf( dirNormalized.x / sinf( m_fCameraX ) );
}

void RBenchmarkApp::CameraMove()
{
#define IsKeyDown(key) ((GetAsyncKeyState(key) & 0x8000)!=0)

	_ASSERT( m_pCameraSceneNode );

	if( IsActive() )
	{
		DWORD elapsed = REngine::GetDevice().GetLastElapsedTime() * (IsKeyDown(VK_SHIFT) ? 15 : 1);

		float fRotateStep = elapsed * 0.001f;
		float fMoveStep = elapsed * 0.5f;
		RVector posDiff( RVector::ZERO );

		//////////////////////////////////////////////////////////////////////////
		// 회전
		if(IsKeyDown(VK_LEFT))	m_fCameraZ += fRotateStep;
		if(IsKeyDown(VK_RIGHT)) m_fCameraZ -= fRotateStep;
		if(IsKeyDown(VK_UP))	m_fCameraX -= fRotateStep;
		if(IsKeyDown(VK_DOWN))	m_fCameraX += fRotateStep;

		int screenCenterX = REngine::GetDevice().GetScreenWidth() / 2;
		int screenCenterY = REngine::GetDevice().GetScreenHeight() / 2;

		POINT pt;
		GetCursorPos( &pt );
		ScreenToClient( GetWindowHandle(), &pt );
		int iDeltaX, iDeltaY;
		iDeltaX = pt.x - screenCenterX;
		iDeltaY = pt.y - screenCenterY;

		m_fCameraZ += -iDeltaX * 0.01f;
		m_fCameraX +=  iDeltaY * 0.01f;
		m_fCameraX = max( 0.01f, min( MMath::PI-0.01f, m_fCameraX ) );

		POINT ptCenter = { screenCenterX, screenCenterY };
		ClientToScreen( GetWindowHandle(), &ptCenter );
		SetCursorPos( ptCenter.x, ptCenter.y );

		RVector newDirection( cosf(m_fCameraZ) * sinf(m_fCameraX), sinf(m_fCameraZ) * sinf(m_fCameraX), cosf(m_fCameraX) );

		//////////////////////////////////////////////////////////////////////////
		// W A S D 이동
		RVector dir = newDirection;
		RVector right = RVector::AXISZ.CrossProduct( dir );
		right.Normalize();

		if(IsKeyDown('W'))			posDiff += fMoveStep * dir;
		if(IsKeyDown('S'))			posDiff -= fMoveStep * dir;
		if(IsKeyDown('A'))			posDiff += fMoveStep * right;
		if(IsKeyDown('D'))			posDiff -= fMoveStep * right;
		if(IsKeyDown(VK_SPACE))		posDiff += fMoveStep * RVector::AXISZ;
		if(IsKeyDown(VK_CONTROL))	posDiff -= fMoveStep * RVector::AXISZ;

		RVector newPosition = m_pCameraSceneNode->GetPosition() + posDiff;

		/////////////////////////////////////////////////////////////////////////
		
		m_pCameraSceneNode->SetTransform( newPosition, newDirection );
	}
}

}