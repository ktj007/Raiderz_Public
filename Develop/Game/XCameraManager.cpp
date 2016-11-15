#include "stdafx.h"
#include "XCameraManager.h"
#include "RWaterPassRenderer.h"
#include "XCameraHelper.h"

#include "XCameraBackViewController.h"
#include "XCameraGrappledController.h"
#include "XCameraCutSceneController.h"
#include "XCameraEngineDebugController.h"
#include "XCameraPVPFreeViewController.h"

XCameraManager::XCameraManager() : m_nLastPlayCameraEfect(0)
{
	m_pCameraSceneNode = NULL;
}

XCameraManager::~XCameraManager()
{
}

void XCameraManager::ResetFOV()
{
	XCameraHelper::ResetFov(GetPrimaryCamera());
}

void XCameraManager::InitPrimaryCamera()
{
	RCameraSceneNode* pPrimaryCamera = global.smgr->GetPrimaryCamera();
	if (pPrimaryCamera == NULL) return;

	ResetFOV();

	_ASSERT( XCONST::CAMERA_NEAR_Z >= 0.f && XCONST::CAMERA_FAR_Z > 0.f );
	pPrimaryCamera->SetNearFarZ(XCONST::CAMERA_NEAR_Z, XCONST::CAMERA_FAR_Z);
	pPrimaryCamera->SetClearColor(0xFF000000);
}


bool XCameraManager::Create()
{
	if (!global.smgr) return NULL;

	if (global.smgr->GetPrimaryCamera() == NULL)
	{
		if (m_pCameraSceneNode == NULL)
		{
			m_pCameraSceneNode = new XBaseCameraSceneNode();
		}

		global.smgr->SetPrimaryCamera(m_pCameraSceneNode);
	}

	if (global.smgr->GetPrimaryCamera() == NULL) return false;

	InitPrimaryCamera();

	// Camera Effect Handler
	m_CameraEffect.LoadCameraEffect(PATH_SFX_CAMERA);

	return true;
}

void XCameraManager::Destroy()
{
	if (global.smgr)
	{
		global.smgr->SetPrimaryCamera(NULL);
	}

	if (m_pCameraSceneNode)
	{
		if( m_pCameraSceneNode->IsAttached() )
			m_pCameraSceneNode->RemoveFromParent();

		SAFE_DELETE(m_pCameraSceneNode);
	}

	m_CameraEffect.DeleteCameraEffect();	// 애니메이션 리소스 때문에 미리 비워주어야 한다
}

RCameraSceneNode* XCameraManager::GetPrimaryCamera()
{
	if (!global.smgr) return NULL;
	return global.smgr->GetPrimaryCamera();
}

void XCameraManager::SetPrimaryCamera( XCameraType nCameraType )
{
	if (!global.smgr) return;

	// 현재 콘터롤러 타입이랑 비교해서 다르면
	if(IsPrimaryCameraType( nCameraType ) == false)
	{
		XCameraSceneNodeController* pCameraController = GetCurrentCameraController();
		if (pCameraController)
		{
			pCameraController->RemoveFromCamera();
		}

		XCameraSceneNodeController* pNewCameraController = m_CameraControllerGroup.GetCameraController(nCameraType);
		if(pNewCameraController)
		{
			m_pCameraSceneNode->AddCameraController(pNewCameraController);
			pNewCameraController->Init();
		}
		else
		{
			global.smgr->SetPrimaryCamera(NULL);//xptm
		}
	}
}

void XCameraManager::CreateCamerasOnFrameOpen( int nFrameID )
{
	m_CameraControllerGroup.CreateCamerasOnFrameOpen(nFrameID);
}

void XCameraManager::DestroyCamerasOnFrameClose()
{
	m_CameraControllerGroup.DestroyCamerasOnFrameClose();
}

bool XCameraManager::IsPrimaryCameraType( XCameraType nCameraType )
{
	if (GetCameraController(nCameraType) == NULL) return false;

	return true;
}

bool XCameraManager::PlayCameraEffect( const wchar_t* szAniName, float _fDuration )
{
	#ifdef _DEBUG_ENGINE
	mlog("%f\n", _fDuration * 1000);
	#endif

	if (global.system)
		m_nLastPlayCameraEfect = global.system->GetNowTime();

	return m_CameraEffect.PlayCameraEffect(szAniName, _fDuration);
}

void XCameraManager::UpdateCamera( float fDelta, int nDeltaX, int nDeltaY, bool bFreeLook )
{
	if ( IsPrimaryCameraType( CAMERA_MAIN_BACKVIEW))
	{
		// 카메라에게 자유모드인지 알려준다.
		XCameraBackViewController* pCameraController = GetCameraController<XCameraBackViewController>();
		if (pCameraController)
		{
			pCameraController->SetFreeLook(bFreeLook);
			pCameraController->UpdateCamera( fDelta, nDeltaX, nDeltaY);
		}
		return;
	}

	if ( IsPrimaryCameraType( CAMERA_FREE_LOOK))
	{
		XCameraFreeLookController* pCameraController = GetCameraController<XCameraFreeLookController>();
		if (pCameraController)
			pCameraController->UpdateCamera( fDelta, nDeltaX, nDeltaY);
		return;
	}

	if ( IsPrimaryCameraType( CAMERA_GRAPPLED))
	{
		XCameraGrappledController* pCameraController = GetCameraController<XCameraGrappledController>();
		if (pCameraController)
			pCameraController->UpdateCamera(fDelta, nDeltaX, nDeltaY);
		return;
	}

	if ( IsPrimaryCameraType( CAMERA_REPLAY_BACKVIEW))
	{
		XCameraReplayBackViewController* pCameraController = GetCameraController<XCameraReplayBackViewController>();
		if (pCameraController)
			pCameraController->UpdateCamera(fDelta, nDeltaX, nDeltaY);
		return;
	}

	if ( IsPrimaryCameraType( CAMERA_ENGINE_DEBUG))
	{
		XCameraEngineDebugController* pCameraController = GetCameraController<XCameraEngineDebugController>();
		if (pCameraController)
			pCameraController->UpdateCamera( fDelta, nDeltaX, nDeltaY);
		return;
	}

	if ( IsPrimaryCameraType( CAMERA_PVP_FREEVIEW))
	{
		XCameraPVPFreeViewController* pCameraController = GetCameraController<XCameraPVPFreeViewController>();
		if(pCameraController)
			pCameraController->UpdateCamera( fDelta, nDeltaX, nDeltaY);
		return;
	}

}

bool XCameraManager::WasPlayingCameraEffectRecently()
{
	unsigned int nNowTime = 0;
	if (global.system)
		nNowTime = global.system->GetNowTime();

	if (m_nLastPlayCameraEfect - nNowTime <= 3000)
	{
		return true;
	}
	
	return false;
}
