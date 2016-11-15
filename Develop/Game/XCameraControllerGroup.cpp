#include "StdAfx.h"
#include "XCameraControllerGroup.h"
#include "XCameraManager.h"

#include "XCameraGrappledController.h"
#include "XCameraReplayBackViewController.h"
#include "XCameraEngineDebugController.h"
#include "XCameraCutSceneController.h"
#include "XCameraPVPFreeViewController.h"

XCameraControllerGroup::XCameraControllerGroup(void)
{
	m_vecCameraControllers.resize(CAMERA_MAX);
	for (size_t i = 0; i < m_vecCameraControllers.size(); i++) m_vecCameraControllers[i] = NULL;
}

XCameraControllerGroup::~XCameraControllerGroup(void)
{
	DestroyCamerasOnFrameClose();
}

void XCameraControllerGroup::DeleteCameraController( XCameraType nType )
{
	if (m_vecCameraControllers[nType])
	{
		m_vecCameraControllers[nType]->RemoveFromCamera();
		SAFE_DELETE(m_vecCameraControllers[nType]);
	}
}



bool XCameraControllerGroup::CreateCameraController( XCameraType nCameraType )
{
	m_nCameraType = nCameraType;

	switch (nCameraType)
	{
	case CAMERA_MAIN_BACKVIEW:			m_vecCameraControllers[CAMERA_MAIN_BACKVIEW]		=new XCameraBackViewController();		return true;
	case CAMERA_FREE_LOOK:				m_vecCameraControllers[CAMERA_FREE_LOOK]			=new XCameraFreeLookController();		return true;
	case CAMERA_GRAPPLED:				m_vecCameraControllers[CAMERA_GRAPPLED]				=new XCameraGrappledController();		return true;
	case CAMERA_REPLAY_BACKVIEW:		m_vecCameraControllers[CAMERA_REPLAY_BACKVIEW]		=new XCameraReplayBackViewController();	return true;
	case CAMERA_ENGINE_DEBUG:			m_vecCameraControllers[CAMERA_ENGINE_DEBUG]			=new XCameraEngineDebugController();	return true;
	case CAMERA_CUT_SCENE:				m_vecCameraControllers[CAMERA_CUT_SCENE]			=new XCameraCutSceneController();		return true;
	case CAMERA_PVP_FREEVIEW:			m_vecCameraControllers[CAMERA_PVP_FREEVIEW]			=new XCameraPVPFreeViewController();	return true;
	}

	return false;
}

void XCameraControllerGroup::CreateCamerasOnFrameOpen( int nFrameID )
{
	switch (nFrameID)
	{
	case LOGIN_FRAME_ID:
		{

		}
		break;
	case CHAR_FRAME_ID:
		{

		}
		break;
	case PLAY_FRAME_ID:
		{
			CreateCameraController(CAMERA_MAIN_BACKVIEW);
			CreateCameraController(CAMERA_FREE_LOOK);
			CreateCameraController(CAMERA_GRAPPLED);
			CreateCameraController(CAMERA_CUT_SCENE);
			CreateCameraController(CAMERA_PVP_FREEVIEW);
		}
		break;
	case REPLAY_FRAME_ID:
		{
			CreateCameraController(CAMERA_MAIN_BACKVIEW);
			CreateCameraController(CAMERA_FREE_LOOK);
			CreateCameraController(CAMERA_REPLAY_BACKVIEW);
		}
		break;
	case ENGINE_DEBUG_FRAME_ID:
		{
			CreateCameraController(CAMERA_MAIN_BACKVIEW);
			CreateCameraController(CAMERA_FREE_LOOK);
			CreateCameraController(CAMERA_GRAPPLED);
			CreateCameraController(CAMERA_ENGINE_DEBUG);
		}
		break;
	default:
		{
			_ASSERTE(0 && "프레임 추가가 안되어 있습니다.");
		}
	}
}

void XCameraControllerGroup::DestroyCamerasOnFrameClose()
{
	for (size_t i = 0; i < m_vecCameraControllers.size(); i++)
	{
		DeleteCameraController(XCameraType(i));
	}
}