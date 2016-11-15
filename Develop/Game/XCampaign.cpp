#include "stdafx.h"
#include "XCampaign.h"
#include "XWorld.h"
#include "XGameState.h"
#include "XCameraManager.h"
#include "XCameraHelper.h"
#include "XNonPlayer.h"
#include "XModuleMotion.h"
#include "XGameInterface.h"


#define SKIPRENDER_COUNT			10
#define TIME_CAMMOVE				5000



// XCampaign
XCampaign::XCampaign()
{
	m_pWorld = NULL;
	m_bCreated = false;
	m_bLoadingCompleted = false;
	m_SkipRenderCount = 0;
	m_bCallOnCreate = false;
	m_bCallOnLoadingComplete = false;
	m_nSceneIndex = -1;
}


// ~XCampaign
XCampaign::~XCampaign()
{
	Destroy();
}


// Create
bool XCampaign::Create()
{
	if ( m_bCreated == true)	return false;


	// Create world
	m_pWorld = new XWorld();

	// Create camera
	if ( XGETCFG_VIDEO_HDR == true)
		global.smgr->SetHDREnable( true);

	if ( XGETCFG_VIDEO_DOF == true)
	{
		global.smgr->SetDOFEnable( true);
		global.smgr->SetDOFFocalDist( XCONST::CHAR_DOF_FOCUS_DIST);
		global.smgr->SetDOFFocalRange( XCONST::CHAR_DOF_FOCUS_RANGE);
		global.smgr->SetDOFCOCSize( XCONST::CHAR_DOF_COC_SIZE);
	}

	XCameraHelper::ResetFov( global.smgr->GetPrimaryCamera());


	// Do not rendering world
	if ( global.camera  &&  global.camera->GetPrimaryCamera())
		global.camera->GetPrimaryCamera()->SetPassIgnore( RRP_WORLDSCENE, RRP_POSTEFFECT);


	m_bCreated = true;
	m_bLoadingCompleted = false;
	m_SkipRenderCount = 0;
	m_bCallOnCreate = false;
	m_bCallOnLoadingComplete = false;

	return true;
}


// Destroy
bool XCampaign::Destroy()
{
	if ( m_bCreated == false)		return false;


	// Delete world
	if ( m_pWorld != NULL)
	{
		m_pWorld->Final();
		SAFE_DELETE( m_pWorld);
	}

	
	// Destroy camera
	if ( global.camera)			global.camera->DestroyCamerasOnFrameClose();


	// Clear scene manager
	if ( global.smgr)			global.smgr->Clear();


	// Call event handler
	OnDestroy();


	m_bCreated = false;
	m_bLoadingCompleted = false;
	return true;
}


// IsLoadingCompleted
bool XCampaign::IsLoadingCompleted()
{
	return m_bLoadingCompleted;
}


// CheckLoadingCompleted
bool XCampaign::CheckLoadingCompleted()
{
	if ( m_bCreated == false)				return false;
	if ( m_bLoadingCompleted == true)		return true;


	// Check background worker
	RBackgroundWorker& bg_woker = REngine::GetBackgroundWorker();
	int nWorkerCount = bg_woker.GetRemainedWorkableTicketSize();
	if (nWorkerCount > 0)					return false;


	// Check world loading
	if ( gg.currentgamestate  &&  gg.currentgamestate->GetWorld()  &&  gg.currentgamestate->GetWorld()->GetTerrain())
	{
		if ( gg.currentgamestate->GetWorld()->GetTerrain()->IsLoadingCompleted() == false)
			return false;
	}

	return true;
}


// Update
void XCampaign::Update( float fDelta)
{
	if ( m_bCreated == false)			return;


	// Skip rendering for cleared screen
	if ( m_SkipRenderCount < SKIPRENDER_COUNT)
	{
		m_SkipRenderCount++;
		return;
	}


	// Call create event handler
	if ( m_bCallOnCreate == false)
	{
		m_bCallOnCreate = true;
		OnCreate();
	}


	if ( m_pWorld == NULL)				return;


	// Check loading completed
	m_bLoadingCompleted = CheckLoadingCompleted();
	if ( m_bLoadingCompleted == false)	return;
	if ( m_bCallOnLoadingComplete == false)
	{
		m_bCallOnLoadingComplete = true;

		OnLoadingComplete();
	}


	// Update world
	m_pWorld->Update( fDelta);


	OnUpdate( fDelta);
}


// Render
void XCampaign::Render()
{
	if ( m_bCreated == true  &&  m_pWorld != NULL)
	{
		OnRender();
	}
}


// ChangeScene
void XCampaign::ChangeScene( int nScene)
{
	if ( nScene == m_nSceneIndex)		return;

	int nPrev = m_nSceneIndex;
	m_nSceneIndex = nScene;

	OnChangedScene( nPrev, nScene);
}


// OnCreate
void XCampaign::OnCreate()
{
}


// OnDestroy
void XCampaign::OnDestroy()
{
}


// OnLoadingComplete
void XCampaign::OnLoadingComplete()
{
	// Start rendering world
	if ( global.camera  &&  global.camera->GetPrimaryCamera())
		global.camera->GetPrimaryCamera()->RemoveAllIgnorePass();
}


// OnUpdate
void XCampaign::OnUpdate( float fDelta)
{
}


// OnRender
void XCampaign::OnRender()
{
}


// OnChangedScene
void XCampaign::OnChangedScene( int nPrev, int nScene)
{
}










// 독립적인 캠패인 모드이므로 하드코딩으로 함
enum
{
	SCENE_LOGIN			= 0,
	SCENE_CHARSELECT,
	SCENE_CHARCREATE
};

static const vec3 POS_CAMERA[] =
{
	vec3( 0.0f, -20295.203f, 12623.285f),
	vec3( 2812.435f, 602.378f, -30.592f),
	//vec3( 712.435f, -412.378f, -56.592f), //Char Select
	vec3( 22755.951f, -145.614f, 105.686f)
	//vec3( 20655.951f, -2145.614f, 105.686f) //Char Create
};

static const vec3 DIR_CAMERA[] =
{
	vec3( 0.0f, 1.0f, 0.0f),
	vec3( 0.0f, 1.0f, 0.14f),
	vec3( 0.0f, 1.0f, 0.0f)
};

static const float GAIN_CAMERA[] = { 100.0f, 100.0f, 100.0f };



// XLoginCampaign
XLoginCampaign::XLoginCampaign()
{
	m_nRestoreFOV = 60;
	m_bMoveCamera = false;
	m_dwCamMoveTimer = 0;
}


// GetInstance
XLoginCampaign* XLoginCampaign::GetInstance()
{
	static XLoginCampaign instance;
	return &instance;
}


// CheckLoadingCompleted
bool XLoginCampaign::CheckLoadingCompleted()
{
	if ( XCampaign::CheckLoadingCompleted() == true)		return true;

	return false;
}


// OnCreate
void XLoginCampaign::OnCreate()
{
	XCampaign::OnCreate();

	// Set restore FOV
	m_nRestoreFOV = XCONST::CAMERA_FOV;
	XCONST::CAMERA_FOV = 60;
	global.camera->ResetFOV();


	// Load zone
	XWorldInitParam worldInitParam;
	worldInitParam.SetZone( L"Login3", false, false);
	m_pWorld->Init( worldInitParam);
}


// OnDestroy
void XLoginCampaign::OnDestroy()
{
	// Stop sound
	if ( global.sound)
		global.sound->StopAll();


	// Restore FOV
	XCONST::CAMERA_FOV = m_nRestoreFOV;
	global.camera->ResetFOV();
}


// OnLoadingComplete
void XLoginCampaign::OnLoadingComplete()
{
	XCampaign::OnLoadingComplete();


	// Set camera move
	m_bMoveCamera = true;
	m_dwCamMoveTimer = timeGetTime();


	// Play sound
	if ( global.sound)
	{
		switch ( m_nSceneIndex)
		{
		case SCENE_LOGIN :
			if ( global.sound)
				global.sound->Play( "bgm_campaign_1", "field");
			break;

		case SCENE_CHARSELECT :
		case SCENE_CHARCREATE :
			if ( global.sound)
				global.sound->Play( "bgm_campaign_2", "field");
			break;
		}
	}

	// 이벤트
	XEvent evt;
	evt.m_nID = XEVTD_MESH_LOADING_COMPLETE;
}


// OnUpdate
void XLoginCampaign::OnUpdate( float fDelta)
{
	XCampaign::OnUpdate( fDelta);


	DWORD dwCurrent = timeGetTime();

	
	// Move camera
	if ( m_bMoveCamera == true)
	{
		double fRatio = (double)( dwCurrent - m_dwCamMoveTimer) / (double)TIME_CAMMOVE;
		if ( fRatio >= 1.0)
		{
			m_bMoveCamera = false;
			fRatio = 1.0;
		}

		vec3 pos = POS_CAMERA[ m_nSceneIndex];
		vec3 dir = DIR_CAMERA[ m_nSceneIndex];
		float gain = GAIN_CAMERA[ m_nSceneIndex];

		vec3 _dir = dir;
		_dir.z = 0;
		_dir.Normalize();

		fRatio = 1.0f - fRatio;
		pos = pos + _dir * ( gain * (float)( fRatio * fRatio));

		global.camera->GetPrimaryCamera()->SetPosition( pos);
		global.camera->GetPrimaryCamera()->SetDirection( dir);
	}
}


// OnChangedScene
void XLoginCampaign::OnChangedScene( int nPrev, int nScene)
{
	XCampaign::OnChangedScene( nPrev, nScene);


	// Set camera move
	m_bMoveCamera = true;
	m_dwCamMoveTimer = timeGetTime();


	// Play sound
	if ( (nPrev == SCENE_CHARSELECT  ||  nPrev == SCENE_CHARCREATE)  &&  nScene == SCENE_LOGIN)
	{
		if ( global.sound)
			global.sound->Play( "bgm_campaign_1", "field");
	}
	else if ( nPrev == SCENE_LOGIN  &&  (nScene == SCENE_CHARSELECT  ||  nScene == SCENE_CHARCREATE))
	{
		if ( global.sound)
			global.sound->Play( "bgm_campaign_2", "field");
	}


	if ( global.camera  &&  global.camera->GetPrimaryCamera())
	{
/*
		RCameraSceneNode* pCamera = global.camera->GetPrimaryCamera();
		
		if ( nScene == SCENE_CHARCREATE)
		{
			pCamera->SetPassIgnore( RRP_WORLDSCENE, RRP_POSTEFFECT);
			pCamera->SetPassIgnore( RRP_ACTOR, false);

			XGetGameInterface().OPTIONS.SetShadow( 0, false);
		}
		else
		{
			pCamera->RemoveAllIgnorePass();

			XGetGameInterface().OPTIONS.SetShadow( XGETCFG_VIDEO_SHADOW, false);
		}
*/
	}
}