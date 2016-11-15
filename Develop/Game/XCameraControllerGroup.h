#pragma once

static const char* XCameraTypeStr[] =
{
	"0_CAMERA_MAIN_BACKVIEW",
	"1_CAMERA_FREE_LOOK",
	"2_CAMERA_GRAPPLED",
	"3_CAMERA_PVP_FREEVIEW",
	"4_CAMERA_REPLAY_BACKVIEW",
	"5_CAMERA_ENGINE_DEBUG",
	"6_CAMERA_CUT_SCENE",
	"CAMERA_MAX"
};

enum XCameraType
{
	// game
	CAMERA_MAIN_BACKVIEW = 0,
	CAMERA_FREE_LOOK,
	CAMERA_GRAPPLED,
	CAMERA_PVP_FREEVIEW,

	// replay
	CAMERA_REPLAY_BACKVIEW,

	// engine debug
	CAMERA_ENGINE_DEBUG,

	CAMERA_CUT_SCENE,

	CAMERA_MAX
};

class XCameraSceneNodeController;
class XCameraBackViewController;
class XCameraFreeLookController;
class XCameraGrappledController;
class XCameraReplayBackViewController;
class XCameraEngineDebugController;
class XCameraCutSceneController;
class XCameraGroundTargetController;
class XCameraPVPFreeViewController;

namespace cameracontroller_binder
{
	template< typename T > struct Type2Enum { enum { TypeID = CAMERA_DEFAULT }; };

	template<> struct Type2Enum< XCameraBackViewController> { enum { TypeID = CAMERA_MAIN_BACKVIEW }; };
	template<> struct Type2Enum< XCameraFreeLookController> { enum { TypeID = CAMERA_FREE_LOOK }; };
	template<> struct Type2Enum< XCameraGrappledController> { enum { TypeID = CAMERA_GRAPPLED}; };
	template<> struct Type2Enum< XCameraReplayBackViewController> { enum { TypeID = CAMERA_REPLAY_BACKVIEW}; };
	template<> struct Type2Enum< XCameraEngineDebugController> { enum { TypeID = CAMERA_ENGINE_DEBUG }; };
	template<> struct Type2Enum< XCameraCutSceneController> { enum { TypeID = CAMERA_CUT_SCENE }; };
	template<> struct Type2Enum< XCameraPVPFreeViewController> { enum { TypeID = CAMERA_PVP_FREEVIEW }; };
}

class XCameraControllerGroup
{
private:
	vector<XCameraSceneNodeController*>	m_vecCameraControllers;

	XCameraType m_nCameraType;

	bool CreateCameraController(XCameraType nCameraType);
	void DeleteCameraController( XCameraType nType );

public:
	XCameraControllerGroup(void);
	~XCameraControllerGroup(void);

	void CreateCamerasOnFrameOpen(int nFrameID);
	void DestroyCamerasOnFrameClose();

	template <typename T_CameraController>
	T_CameraController*	GetCameraController()
	{
		int nID = cameracontroller_binder::Type2Enum<T_CameraController>::TypeID;
		return static_cast<T_CameraController*>(GetCameraController(XCameraType(nID)));
	}

	XCameraSceneNodeController* GetCameraController(XCameraType nCameraType)
	{
		return m_vecCameraControllers[nCameraType];
	}
};
