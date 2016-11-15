#pragma once

#include "XGlobal.h"
#include "XBaseCameraSceneNode.h"
#include "XCameraControllerGroup.h"
#include "XCameraEffect.h"
#include "XCameraBackViewController.h"
#include "XCameraFreeLookController.h"
#include "XCameraReplayBackViewController.h"
#include "XCameraGrappledController.h"
#include "XCameraPVPFreeViewController.h"

/// 카메라 관리자
class XCameraManager
{
private:
	XCameraControllerGroup	m_CameraControllerGroup;
	XCameraEffect			m_CameraEffect;

	XBaseCameraSceneNode*	m_pCameraSceneNode;

	unsigned int			m_nLastPlayCameraEfect;

	void InitPrimaryCamera();
public:
	XCameraManager();
	~XCameraManager();

	bool Create();
	void Destroy();

	void CreateCamerasOnFrameOpen(int nFrameID);
	void DestroyCamerasOnFrameClose();

	void UpdateCamera(float fDelta, int nDeltaX, int nDeltaY, bool bFreeLook);

	void ResetFOV();

	RCameraSceneNode* GetPrimaryCamera();
	void SetPrimaryCamera( XCameraType nCameraType );
	bool IsPrimaryCameraType(XCameraType nCameraType);

	XBaseCameraSceneNode* GetCamera()
	{
		return m_pCameraSceneNode;
	}

	bool PlayCameraEffect(const wchar_t* szAniName, float _fDuration);
	bool WasPlayingCameraEffectRecently();

	//현재 scenenode에 붙어 있는 카메라 콘트롤러의 종류
	template <typename T_CameraController>
	T_CameraController*	GetCameraController()
	{
		int nID = cameracontroller_binder::Type2Enum<T_CameraController>::TypeID;

		return static_cast<T_CameraController*>(GetCameraController(XCameraType(nID)));
	}

	XCameraSceneNodeController* GetCameraController(XCameraType nCameraType)
	{
		if(GetPrimaryCamera() == NULL) return NULL;
		return static_cast<XCameraSceneNodeController*>(GetPrimaryCamera()->GetCameraController(RCCT_PREUPDATE, XCameraTypeStr[nCameraType]));
	}

	XCameraSceneNodeController* GetCurrentCameraController()
	{
		for (int i = CAMERA_MAIN_BACKVIEW; i < CAMERA_MAX; i++)
		{
			XCameraSceneNodeController* pController = GetCameraController(XCameraType(i));
			if (pController)
			{
				return pController;
			}
		}

		return NULL;
	}
	
};	



