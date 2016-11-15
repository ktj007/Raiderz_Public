#include "stdafx.h"
#include "RApplication.h"
#include "RSceneManager.h"
#include "RMeshMgr.h"
#include "RConfiguration.h"
//#include "RPortalSceneManager.h"
#include "RCameraSceneNode.h"
#include "RProfileInfo.h"
#include "RSystemInfo.h"
#include "mmsystem.h"

namespace rs3 {

bool		RApplication::IsFullScreen()				{ return REngine::GetConfig().m_bFullScreen; }

RApplication::RApplication()
: m_pSceneManager(NULL)
, m_pDevice(NULL)
, m_bActive(true)
, m_dwClearColor(D3DCOLOR_ARGB(255, 0, 0, 255))
{
#ifdef WIN32
	m_hWnd = 0;
#endif

	
}

RApplication ::~RApplication()
{

}

bool RApplication::Create()
{
	CreateApplicationInfo();

	mlog("Application create\n");
	if(!CreateDevice()) 
	{
		return false;
	}
	mlog("Device created\n");
	m_lastDeviceStatus = m_pDevice->QueryStatus();

	// 백그라운드 워커 초기화
	REngine::GetBackgroundWorker().Init();

	// 인스턴스 생성
	_ASSERT( m_pSceneManager == NULL );
	m_pSceneManager = CreateSceneManager();
	_ASSERT( m_pSceneManager );
	REngine::RegisterSceneManager(m_pSceneManager);

	if(!OnCreateSceneManager())
		return false;

	mlog("SceneManager created\n");
	return true;
}

RSceneManager* RApplication::CreateSceneManager()
{
	RSceneManager* pSceneManager = new RSceneManager;
	REngine::RegisterSceneManager(pSceneManager);
	return pSceneManager;
}

bool RApplication::OnCreateSceneManager()
{
	// SceneManager Create
	_ASSERT( m_pDevice );
	if(!m_pSceneManager->Create(m_pDevice))
		return false;

	if(!REngine::InitSharedResource())
		return false;

	return true;
}

void RApplication ::Destroy()
{
	// 생성 호출순서의 역순
	// Scene Manager 보다는 먼저 죽게
	OnDestroy();

	// Scene Manager 죽이기
	OnDestroySceneManager();
	SAFE_DELETE(m_pSceneManager);
	REngine::RegisterSceneManager(NULL);

	// 백그라운드 워커 소멸
	REngine::GetBackgroundWorker().Destroy();

	// Device 지우기
	DestroyDevice();
}

void RApplication::OnDestroySceneManager()
{
	if(m_pSceneManager)
		m_pSceneManager->Clear();

	REngine::DestroySharedResource();

	if(m_pSceneManager)
		m_pSceneManager->Destroy();
}

int RApplication::Run()
{
	mlog("Application started\n");

	RPFC_B("rs3::RApplication::Run() - init");

	if(!OnInit())
	{
		mlog("Application failed to initialize\n");
		return 0;
	}

	if(!Create())
	{
		mlog("Application failed to create\n");
		Destroy();
		return 0;
	}
	if(!OnCreate())
	{
		mlog("Application::OnCreate failed.\n");
		Destroy();
		return 0;
	}

	RPFC_E;

	m_pSceneManager->ResetTimer();
	int nReturn = MainLoop();

	Destroy();

	return nReturn;
}

void RApplication::UpdateTimer()
{
	m_pSceneManager->UpdateTimer();
}

void RApplication::Update()
{
	RPFC_THISFUNC;
	OnPreUpdate();
	UpdateTimer();
	{
		TIMEPROFILE("App Update");
		RPFC_B("RApplication::OnUpdate");
		OnUpdate();
		RPFC_E;
	}
	
	UpdateBackgroundWorker();

	_ASSERT( m_pDevice );
	m_pSceneManager->Update( m_pDevice->GetFrameCount() );

	OnPostUpdate();
}

bool RApplication::ResetDevice()
{
	if(m_lastDeviceStatus == RDEVICE_OK)
		OnLostDevice();

	if(m_pDevice->ResetDevice() == false)
	{
		return false;
	}

	OnResetDevice();
	mlog("device reset.\n");

	return true;
}

void RApplication::Render()
{
	//TIMEPROFILE_THIS_FUNCTION;
	RPFC_THISFUNC;

	OnPreRender();

	RDEVICESTATUS deviceStatus = m_pDevice->QueryStatus();

	if(deviceStatus==RDEVICE_LOST)
	{
		if(m_lastDeviceStatus==RDEVICE_OK)
			OnLostDevice();

		m_lastDeviceStatus = deviceStatus;

		OnPostRender();
		return;
	}
	else if(deviceStatus==RDEVICE_RESTORED)
	{
		if(ResetDevice() == true)
			deviceStatus = RDEVICE_OK;
		else
			return;
	}

	m_lastDeviceStatus = deviceStatus;

	m_pDevice->Clear(true, true, false, m_dwClearColor);


	m_pDevice->BeginScene();

	m_pSceneManager->RenderPrimaryCamera( this);
	OnRender();

	m_pDevice->EndScene();

	PreFlip();
	m_pDevice->Flip();
	PostFlip();

	OnPostRender();

}

void RApplication::UpdateBackgroundWorker()
{
	RPFC_THISFUNC;
	REngine::GetBackgroundWorker().Update();
}

void RApplication::CreateApplicationInfo()
{
	m_dwMainThreadId = GetCurrentThreadId();
}

void RApplication::PreFlip()
{
	OnPreFlip();
}

void RApplication::PostFlip()
{
	OnFlip();
	
}

}
