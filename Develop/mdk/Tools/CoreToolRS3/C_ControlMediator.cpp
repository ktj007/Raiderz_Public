#include "stdafx.h"
#include "RMeshMgr.h"
#include "C_ControlMediator.h"
#include "C_ControlDoAction.h"
#include "C_XMLConfig.h"
#include "C_XMLXConst.h"
#include "C_XMLEditInfoFile.h"
#include "C_StatusBarHandler.h"
#include "C_XMLResourceList.h"

//#include "CoreToolRS3.h"
#include "RApplication.h"
#include "RTerrain.h"
#include "RConfiguration.h"
#include "M_ToolActor.h"
#include "M_ToolTerrain.h"

#include "RUtil.h"
#include "RCollisionTreeBuilder.h"

#include "RSpeedGrass.h"
//#include "RSceneCloud.h"
#include "RRenderHelper.h"
#include "REnvironmentSet.h"

//manager or helper
#include "C_ControlMediatorHelperResource.h"
#include "C_ControlMediatorHelperRender.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_MintManager.h"
#include "M_ToolCmdMgr.h"
#include "M_ToolLogicObjectManager.h"

#include "RTreeResourceMgr.h"

#include "RTreePassRenderer.h"
#include "RWaterPassRenderer.h"
#include "MProfiler.h"
#include "RProfiler.h"

#include <string>

#include "WS_WorkSpaceManager.h"
#include "VR_EtcCollision.h"

//#include "WS_CutScene.h"

using namespace rs3;
CControlMediator::CControlMediator()
{
	//컨테이너 초기화
	ZeroMemory(WB_ActionTable_,  sizeof(WB_ActionTable_));

	//ActionTable 초기화
	CControlDoAction::InitActionTable(WB_ActionTable_);
	m_bNavigation_ = false;
	m_bDeviceInited_ = false;

	//자원 초기화
	m_pCurrentDevice_ = NULL;
	m_pCurrentCamera_ = NULL;
	m_pCurrentSceneManager_ = NULL;
	m_pEnvironmentSet_ = NULL;

	m_wbCurrentState_ = WORKBENCH_STATE::ETC_DEV;
	m_wbPreviousState_ = WORKBENCH_STATE::ETC_DEV;
	m_nEditMode = EDITMODE_NONE;

	//mouse 초기화
	m_mouseInfo.mouseState.bDrag = false;
	m_mouseInfo.mouseState.bMDrag = false;
	m_mouseInfo.mouseState.bLButton = false;
	m_mouseInfo.mouseState.bMButton = false;
	m_mouseInfo.mouseState.bRButton = false;
	m_mouseInfo.mouseState.bEvented = false;
	m_mouseInfo.setIgnoreMouseMovedPickingRActor( false );
	m_mouseInfo.m_nWheelDelta = 0;
	m_mouseInfo.m_bPopAble  = true;
	m_mouseInfo.m_bCaptured = false;

	//helper / manager class
	m_pCmdMgr_ = new CToolCmdMgr;
	m_pLogicObjectMgr_ = new CToolLogicObjectManager(this);
	m_pWorkSpaceMgr_ = new CWorkSpaceManager(this);
	m_pWorkSpaceMgr_->LoadPlugins();

	m_pHelperResource_ = new CControlMediatorHelperResource;
	m_pHelperAction_ = new CControlMediatorHelperAction;
	m_pHelperRender_ = new CControlMediatorHelperRender;
	
	// 파일 변경 감시
	m_pFileWatcher = new FileWatcher("../../Data");
	m_pFileWatcher->InstallChangedFileSetCallback( fastdelegate::MakeDelegate( this, &CControlMediator::Report ) );
}

CControlMediator::~CControlMediator()
{
	SAFE_DELETE(m_pCmdMgr_);
	SAFE_DELETE(m_pLogicObjectMgr_);
	SAFE_DELETE(m_pWorkSpaceMgr_);

	SAFE_DELETE(m_pHelperResource_);
	SAFE_DELETE(m_pHelperAction_);
	SAFE_DELETE(m_pHelperRender_);
	SAFE_DELETE(m_pFileWatcher);
}

bool CControlMediator::CreateWBDeviceAndNotify(UINT nWidth, UINT nHeight, HWND _hWnd, UINT _workTime)
{
	ASSERT(m_pCurrentDevice_==NULL);
	if (nWidth == 0) nWidth = 640;
	if (nHeight == 0) nHeight = 480;

	//workTime 설정
	int worktime = 100/_workTime;
	ELAPSE_IN_TIMER = static_cast<DWORD>(worktime);

	if ( REngine::GetConfig().LoadFromXMLFile(FILENAME_CONFIG) == false )
	{
		MessageBox(NULL,"REngine::GetConfig().xml 파일을 읽을 수 없습니다.", "Error", MB_OK);
		return false;
	}

	//set device configuration to create in config
	REngine::GetConfig().m_nWidth = nWidth;
	REngine::GetConfig().m_nHeight = nHeight;
	REngine::GetConfig().m_bFullScreen = false;
	REngine::GetConfig().pixelFormat = RFMT_ARGB32;
	REngine::GetConfig().depthFormat = RFMT_D24S8;

	//set config from config file
	if ( CXMLConfig::GetInstance()->LoadXML(FILENAME_CONFIG) == false )
	{
		MessageBox(NULL,"REngine::GetConfig().xml 파일을 읽을 수 없습니다.", "Error", MB_OK);
		return false;
	}

	// game const 파일 읽기
	if (XCONST::Load("../../Data/Zone/Zone_Utils/const.xml") == false)
	{
		mlog("Failed To Load const.xml\n");
	}

	// 패쓰 설정후에 도움 관리자 설정
	m_pCmdMgr_->InitCommandManager(this);
	m_pHelperResource_->InitMediatorHelper_(this);
	m_pHelperAction_->InitHelperAction(this);
	m_pLogicObjectMgr_->LoadTablesSafely();

	// 3D 장치 만들기 --
	m_pCurrentDevice_ = new RDeviceD3D;
	if ( !m_pCurrentDevice_->Create( _hWnd ))
	{
		AfxMessageBox("Device 개체 생성을 실패했습니다.");
		SAFE_DELETE(m_pCurrentDevice_);
		return false;
	}

	REngine::RegisterDevice(m_pCurrentDevice_);

	// 툴에서는 디바이스가 기본적으로 쉐이더 지원
	REngine::GetConfig().m_bUsingShader = true;
	m_pCurrentDevice_->SetLighting(false);

	// 컴파일된 쉐이더 사용
	if( REngine::IsDevelopVersion() == false )
	{
		REngine::GetConfig().m_bUsingCompiledShader = true;
		REngine::GetConfig().m_bUsingCompiledShaderCaching = true;
		REngine::GetConfig().m_strCompiledShaderCachePath = "Caching/";
	}

	// 씬 매니저 만들기 --
	_ASSERT( m_pCurrentSceneManager_ == NULL );
	m_pCurrentSceneManager_ = new RToolSceneManager(this);
	REngine::RegisterSceneManager(m_pCurrentSceneManager_);
	if ( m_pCurrentSceneManager_->Create(m_pCurrentDevice_) == false )
	{
		mlog("씬매니저 생성 실패\n");
		DestoryResource();
		DestroyDevice();
		return false;
	}
	if(!REngine::InitSharedResource())
	{
		AfxMessageBox("공유 리소스 ( 쉐이더 등 ) 생성을 실패했습니다.");
		DestoryResource();
		DestroyDevice();
		return false;
	}

	// 환경 설정
	ASSERT( m_pEnvironmentSet_ == NULL );
	m_pEnvironmentSet_ = new rs3::REnvironmentSet;
	m_pCurrentSceneManager_->AddSceneNode(m_pEnvironmentSet_);
	m_pEnvironmentSet_->AddAttribute( RSNA_DO_NOT_SAVE );

	//카메라 설정 / 만들기
	m_cameraInfo.ROTATE_STEP = CXMLConfig::GetInstance()->fRotateStep;
	m_cameraInfo.MOVE_STEP   = CXMLConfig::GetInstance()->fPanStep;
	m_cameraInfo.KEY_STEP    = CXMLConfig::GetInstance()->fVelocity;
	m_cameraInfo.WHEEL_ZOOM_STEP   = CXMLConfig::GetInstance()->fWheelZoomFactor;

	m_pCurrentCamera_ = new rs3::RToolCamera;
	m_pCurrentCamera_->SetClearColor(D3DCOLOR_XRGB( 35,135,195)); //기본설정
	m_pCurrentCamera_->SetNearFarZ( 5.0f, CXMLConfig::GetInstance()->fMaxFarZ);
	m_pCurrentCamera_->AddAttribute( RSNA_DO_NOT_SAVE );

	//Distortion 활성화
	m_pCurrentSceneManager_->SetDistortionEnable(true);


	// PassRenderer 설정 --
	REngine::GetSceneManager().GetPassRenderer< rs3::RTreePassRenderer >()->SetTreeLOD(XCONST::TREE_LOD_NEAR, XCONST::TREE_LOD_FAR);
	
	m_pCurrentSceneManager_->AddSceneNode(m_pCurrentCamera_);
	m_pCurrentSceneManager_->SetPrimaryCamera(m_pCurrentCamera_);
	m_pCurrentCamera_->SetPosAndTarget(rs3::RVector(120, -120, 120), rs3::RVector(0, 0, 0), true);

	//워크벤치 초기화
	ResizeWBDevice(nWidth, nHeight);
	m_wbhWnd_ = _hWnd;

	// 디바이스 관련 클래스들 초기화
	m_pHelperRender_->InitHelperRender(this, nWidth, nHeight);
	CMintManager::GetReference().InitMintManager( this );

	// 초기 업데이트
	RunRS3();

	//장치가 만들어진것을 뷰에 알림
	BroadCastMessage(VIEW_MESSAGE::DEVICE_CREATED);

	m_bDeviceInited_ = true;

	GetGPUProfiler().Create();

	return true;
}
void CControlMediator::RunRS3()
{
	
	if( m_pCurrentSceneManager_ == NULL )
		return;

	PFC_B("CControlMediator::RunRS3()");
	// 디바이스 상태 검사
	ASSERT(m_pCurrentDevice_);
	static RDEVICESTATUS lastDeviceStatus = RDEVICE_OK;
	RDEVICESTATUS deviceStatus = m_pCurrentDevice_->QueryStatus();
	if(deviceStatus==RDEVICE_LOST)
	{
		lastDeviceStatus = deviceStatus;
		return;
	}
	else if(deviceStatus==RDEVICE_RESTORED)
	{
		m_pCurrentDevice_->ResetDevice();
	}

	//pause 확인
	if(m_pHelperAction_->IsPause())
	{
		m_pHelperRender_->RenderPause();
		return;
	}

	// update
	m_pCurrentSceneManager_->UpdateTimer();
	if(m_pWorkSpaceMgr_->GetCurrentWorkSpace() != NULL)
	{
		m_pWorkSpaceMgr_->GetCurrentWorkSpace()->OnBetweenCoreToolUpdateAndRS3Render();
	}

	GetHelperAction()->UpdateSelectedObjects();

	m_pCurrentSceneManager_->Update();

	// file 변경 감시
	m_pFileWatcher->Update();

	// camera update
	RSceneNodeUpdateVisitor updateVisitor;
	m_pCurrentCamera_->Traverse(&updateVisitor);
	m_pCurrentCamera_->UpdateUserClipPlane(true, REngine::GetConfig().m_bUsingShader);

	// ref mesh
	m_pHelperResource_->UpdateRefMesh();

	// render
	m_pCurrentDevice_->Clear(true, true, true, D3DCOLOR_XRGB(35,135,195), 1.0f, 0L);
	m_pCurrentDevice_->BeginScene();

	m_pCurrentSceneManager_->RenderPrimaryCamera();

	m_pCurrentCamera_->UpdateUserClipPlane(false);
	m_pHelperRender_->OnDrawHelperRender();

	m_pCurrentDevice_->EndScene();
	m_pCurrentDevice_->Flip();

	m_pHelperRender_->OnUpdateHelperRender();

	m_DebuggingInfo.SendOut(m_wbhWnd_);
	GetProfileManager().ResetCurrentInfo();

	PFC_E;
}

void CControlMediator::ResizeWBDevice(UINT _w, UINT _h)
{
	m_wbWidth_ = _w;
	m_wbHeight_ = _h;

	float ratio = (float)_w/(float)_h;
	if ( m_pCurrentDevice_ != NULL )
	{
		REngine::GetConfig().m_nWidth = (int)_w;
		REngine::GetConfig().m_nHeight = (int)_h;

		CWorkSpace* pWS = GetWorkSpaceMgr()->GetCurrentWorkSpace();
		pWS->OnLostDevice();
		m_pCurrentDevice_->ResetDevice();
		pWS->OnRestoreDevice();
		
		if(m_pCurrentCamera_)
			m_pCurrentCamera_->SetFov( DegreeToRadian((float)XCONST::CAMERA_FOV), ratio);
		
		CMintManager::GetReference().SetWorkspaceSize(m_wbWidth_, m_wbHeight_);
	}
}

MAP_EDIT_TYPE CControlMediator::LoadZone(const char* _pStrName, MAP_EDIT_TYPE _wantedMapType, bool _bWithRelatedConfig)
{
	ASSERT(m_pCurrentSceneManager_);

	//set zone info
	char sz_path[128];
	char sz_file[128];
	sz_path[0] = '\0';
	sz_file[0] = '\0';
	SplitPath(_pStrName, sz_path, sz_file);
	m_mapInfo.m_strScenePath = sz_path;
	m_mapInfo.m_strSceneFile = sz_file;
	m_mapInfo.m_strImportSceneFile.clear();

	DWORD dwStartTime = GetTickCount();

	// 로딩되기전에, 예전에 메디에디터는 씬이었던걸로 설정
	m_pHelperResource_->SetMediatorAfterSceneLoaded(MAP_EDIT_TYPE_SCENE);
	if(LoadSceneAndEnvFromXML_(_pStrName, _wantedMapType) == false)
	{
		mlog("Environment .xml 읽기 실패\n");
		
		m_mapInfo.m_strScenePath = "";
		m_mapInfo.m_strSceneFile = "";

		m_mapInfo.m_eMapEditType = MAP_EDIT_TYPE_ERROR;

		mlog("zone loading time : %6.2f sec\n", (float)(GetTickCount() - dwStartTime)/1000.f);
		return m_mapInfo.m_eMapEditType;
	}

	mlog("zone loading time : %6.2f sec\n", (float)(GetTickCount() - dwStartTime)/1000.f);

	//config 파일 로딩전에, 메디에디터 설정을 씬으로 설정
	m_pHelperResource_->SetMediatorAfterSceneLoaded(MAP_EDIT_TYPE_SCENE);
	if(_bWithRelatedConfig)
		m_pHelperResource_->ManageConfigFileRelatedMap(_pStrName, CControlMediatorHelperResource::MAP_CONFIG_LOAD, true);

	// map info 설정
	if (_wantedMapType != MAP_EDIT_TYPE_SCENE)
		m_pHelperResource_->SetMediatorAfterSceneLoaded(_wantedMapType);

	// 씬이 로딩되었다고 view에게 알리기
	// m_pEffect는 V_EnvWeather에서 Load된다.
	BroadCastMessage(VIEW_MESSAGE::SCENE_LOADED);
	return m_mapInfo.m_eMapEditType;
}

void CControlMediator::SaveZone(const char* _filename, bool _bSort, bool _bSaveAs)
{
	ASSERT(m_pCurrentSceneManager_);
	if(!m_mapInfo.m_bLoaded)
		return;

	std::string strFileName;
	if(_filename == NULL)
		strFileName = m_mapInfo.m_strScenePath + m_mapInfo.m_strSceneFile;
	else
	{
		strFileName = _filename;
		if( _bSaveAs )
		{
			char sz_path[128];
			char sz_file[128];
			sz_path[0] = '\0';
			sz_file[0] = '\0';
			SplitPath(_filename, sz_path, sz_file);
			m_mapInfo.m_strScenePath = sz_path;
			m_mapInfo.m_strSceneFile = sz_file;
		}
	}
	const char* filename = strFileName.c_str();

	// 일단은 아웃도어만 XML 소팅
	if(_bSort && m_mapInfo.m_eMapEditType == MAP_EDIT_TYPE_SCENE && m_pCurrentSceneManager_->GetCurrentTerrain())
	{
		MXml xml;
		m_pCurrentSceneManager_->SaveRootSceneToXML(filename, xml);
		m_pHelperResource_->SortZoneXmlAndSave(filename, xml);
	}
	else
		m_pCurrentSceneManager_->SaveRootSceneToXML(filename);

	// Env파일 저장
	ASSERT(m_pEnvironmentSet_);
	char szBuffer[MAX_PATH];
	szBuffer[0] = '\0';
	strcpy(szBuffer, filename);
	char *p = strrchr(szBuffer,'.');
	strcpy(p,".env.xml");
	m_pEnvironmentSet_->SaveToFile(szBuffer);

	// WorkSpace에 통보, 현재는 zone workspace만
	CWorkSpace* pZoneWorkSpace = m_pWorkSpaceMgr_->GetWorkSpace("zone.xml");
	if (pZoneWorkSpace)
	{
		pZoneWorkSpace->OnSceneSaved(filename);
	}

	//config file save
	if( _bSaveAs )
		m_pHelperResource_->ManageConfigFileRelatedMap(filename, CControlMediatorHelperResource::MAP_CONFIG_SAVE_AS, true);
	else
		m_pHelperResource_->ManageConfigFileRelatedMap(filename, CControlMediatorHelperResource::MAP_CONFIG_SAVE, true);
}

MAP_EDIT_TYPE CControlMediator::LoadWorkSpaceFile(const char* _pStrName, const std::string& _rWsKeyword)
{
	ASSERT(m_pCurrentSceneManager_);
	CWorkSpace* pWorkSpace = m_pWorkSpaceMgr_->GetWorkSpace(_rWsKeyword);
	_ASSERT(pWorkSpace);

	if (_rWsKeyword == "field.xml")		m_nEditMode = EDITMODE_FIELD;
	else if (_rWsKeyword == "zone.xml")	m_nEditMode = EDITMODE_ZONE;
	else	
	{
		//	ASSERT(false && "unknown ws keyword");
	}

	// set helpers
	CControlMediatorHelperEntityAction* pEntityActionHelper = m_pWorkSpaceMgr_->GetHelperEntityAction();
	CControlMediatorHelperObjectRenderer* pObjectRenderHelper = m_pWorkSpaceMgr_->GetHelperObjectRenderer();
	m_pWorkSpaceMgr_->SetHelperEntityAction( pWorkSpace->m_WorkspaceHelpersAndActions_.m_pEntityActionHelper );
	m_pWorkSpaceMgr_->SetHelperObjectRenderer( pWorkSpace->m_WorkspaceHelpersAndActions_.m_pObjectRenderHelper );

	MAP_EDIT_TYPE mapOpenResult = pWorkSpace->OnOpenFile(_pStrName);

	// reset helpers
	m_pWorkSpaceMgr_->SetHelperEntityAction( pEntityActionHelper );
	m_pWorkSpaceMgr_->SetHelperObjectRenderer( pObjectRenderHelper );

	return mapOpenResult;
}

std::string CControlMediator::SaveWorkSpaceFile(const char* _pSzFileName, bool bSaveAs)
{
	BroadCastMessage( VIEW_MESSAGE::PRE_SAVE_DOCUMENT );

	std::string strFileName = m_pWorkSpaceMgr_->GetCurrentWorkSpace()->OnSaveFile(_pSzFileName, bSaveAs);

	BroadCastMessage( VIEW_MESSAGE::POST_SAVE_DOCUMENT );

	return strFileName;
}

void CControlMediator::CreateZone()
{
	if(m_pCurrentSceneManager_ == NULL)
	{
		AfxMessageBox("존 새로 생성하기 실패 - 씬 매니저가 없습니다.");
		return;
	}

	// 환경 생성
	ASSERT(m_pEnvironmentSet_);
	m_pEnvironmentSet_->CreateEnvironment("default");
	
	// 아웃도어 존일 경우
	if( m_mapInfo.m_strImportSceneFile.empty() )
	{
		// 지형생성
		rs3::RToolTerrain* pTerrain = static_cast<RToolTerrain*>(m_pCurrentSceneManager_->CreateSceneNode(RSID_TERRAINSCENENODE));
		m_pCurrentSceneManager_->AddSceneNode(pTerrain, true);
		m_pCurrentSceneManager_->AttachLogicObject( pTerrain );

		// 기본 라이트맵
		rs3::RDirectionalLightSceneNode *pLightNode = new RDirectionalLightSceneNode();
		pLightNode->SetNodeName("TerrainLightMap0");
		_ASSERT( pLightNode->GetUserData() == NULL );
		m_pCurrentSceneManager_->AttachLogicObject( pLightNode );

		m_pCurrentSceneManager_->AddSceneNode(pLightNode, true);

		char szName[MAX_PATH];
		szName[0] = '\0';

		SplitPath(m_mapInfo.m_strSceneFile.c_str(), NULL, szName);
		if ( char *p = strchr(szName,'.'))
			*p = '\0';

		// create new terrain
		if(!pTerrain->CreateNewTerrain(m_mapInfo.m_strScenePath.c_str(), szName, m_mapInfo.m_strTerrainBaseTile.c_str(), m_mapInfo.m_colFace + 1, m_mapInfo.m_rowFace + 1, m_mapInfo.m_width, m_mapInfo.m_height))
		{
			AfxMessageBox("지형 생성에 실패했습니다.");
			return;
		}
	}
	else // INDOOR_ZONE
	{
		// 이게 붙어있어야 editable 하게 scene 을 로딩한다
		m_mapInfo.m_bEditSceneObject = true;	///< TODO: 이거 참 안좋네요

		m_pCurrentSceneManager_->LoadEditableSceneXML(m_mapInfo.m_strImportSceneFile.c_str());

		m_pHelperResource_->SetAutoAnimationProperty();
	}

	// xml 파일패쓰 설정
	m_pCurrentSceneManager_->GetRootSceneNode().SetXMLFileName( m_mapInfo.m_strSceneFile );

	// 메디에이터 세팅
	m_pHelperResource_->SetMediatorAfterSceneLoaded();

	// 존 로드 이벤트 방송 및 인포메이션 파일생성
	// ZONE_LOADED 메시지가 방송될때 될때 풀 편집등에 필요한 정보가 로드된다.
	// 이때, Save된 Zone Information 파일이 필요하다.
	string strTemp(m_mapInfo.m_strScenePath + m_mapInfo.m_strSceneFile);
	CXMLEditInfoFile::GetReference().SaveZoneInformationFile(strTemp.c_str(), true);
	BroadCastMessage(VIEW_MESSAGE::SCENE_LOADED);

	// 편집 더티 플래그 설정을위해 null 액션을 추가
	m_pCmdMgr_->RecordCommand(NULL);

	if ( m_pEnvironmentSet_ )
		m_pEnvironmentSet_->LoadWeatherFromXML();

	// refresh zone
	m_pHelperResource_->RefreshZone();
}

void CControlMediator::CreateWorkSpaceFile(const char* _pSzWorkSpaceName, MAP_EDIT_TYPE _mapType)
{
	CWorkSpace* pWorkspace = m_pWorkSpaceMgr_->GetWorkSpace(_pSzWorkSpaceName);
	if (pWorkspace)
	{
		m_pWorkSpaceMgr_->SetHelperEntityAction( pWorkspace->m_WorkspaceHelpersAndActions_.m_pEntityActionHelper );
		m_pWorkSpaceMgr_->SetHelperObjectRenderer( pWorkspace->m_WorkspaceHelpersAndActions_.m_pObjectRenderHelper );

		pWorkspace->OnCreateWorkspaceFile();

		ASSERT(m_pCurrentSceneManager_);
		m_mapInfo.m_eMapEditType = _mapType;

		//필드가 로딩되었다고 view에게 알리기, 기존 필드로 부터 생성시 등에 필요
		if (_mapType != MAP_EDIT_TYPE_SCENE)
		{
			m_mapInfo.m_bEditSceneObject = false;
			m_mapInfo.m_bSceneEditableMap = false;
			BroadCastMessage(VIEW_MESSAGE::LOGIC_LOADED);
		}
		else
		{
			m_mapInfo.m_bEditSceneObject = true;
			m_mapInfo.m_bSceneEditableMap = true;
			BroadCastMessage(VIEW_MESSAGE::SCENE_LOADED);
		}
	}
}

void CControlMediator::ResetResource()
{
	m_pCurrentDevice_->SetLighting(false);

	//씬 매니저 리셋 && 씬 매니저 종속적인것 삭제
	if(m_pCurrentSceneManager_)
	{
		// CommandManager Reset
		// 씬매니저 지우기 전
		m_pCmdMgr_->ResetCommandManager();

		// 씬매니저에 붙어있는 RefMesh 지우기
		m_pHelperResource_->ResetRefMesh(NULL);

		//environment set 지우기
		if ( m_pEnvironmentSet_ )
		{
			m_pEnvironmentSet_->RemoveFromParent();
			delete m_pEnvironmentSet_;

		}

		// SceneManager Clear
		BroadCastMessage(VIEW_MESSAGE::BEFORE_SCENE_CLEAR);

		// Logic Object Manager
		// BEFORE_SCENE_CLEAR 메시지 후, 씬매니저 Clear 직전
		m_pLogicObjectMgr_->ResetLogicObjectManager();

		// 씬 매니저 Clear
		m_pCurrentSceneManager_->Clear();

		// WorkSpace Resource Reset
		CWorkSpace* pCurrentWorkSpace = m_pWorkSpaceMgr_->GetCurrentWorkSpace();
		if (pCurrentWorkSpace)
			pCurrentWorkSpace->OnResetResourceAfterSceneCleared(false);

		m_pEnvironmentSet_ = new rs3::REnvironmentSet; // 싱글턴이라서..
		m_pCurrentSceneManager_->AddSceneNode(m_pEnvironmentSet_);
		m_pEnvironmentSet_->AddAttribute( RSNA_DO_NOT_SAVE );
	}

	//Helper
	m_pHelperAction_->ResetHelperAction();
	m_pHelperRender_->ResetHelperRender(m_pCurrentSceneManager_);

	//tmp file 지우기
	m_pHelperResource_->CreateMiniMapAndSetCamera("_minimap.tmp", CControlMediatorHelperResource::MINI_DELETE);
}

void CControlMediator::DestoryResource()
{
	// Environment Set
	if ( m_pEnvironmentSet_ )
	{
		m_pEnvironmentSet_->RemoveFromParent();
		delete m_pEnvironmentSet_;
		m_pEnvironmentSet_ = NULL;
	}

	// 커맨드 버퍼
	// 씬매니저 초기화 이전에 해준다.
	m_pCmdMgr_->ResetCommandManager();

	//직접 생성한 카메라들을 직접 지워준다.
	if(m_pCurrentCamera_ != NULL)
	{
		m_pCurrentCamera_->RemoveFromParent();
		delete m_pCurrentCamera_;
		m_pCurrentCamera_ = NULL;
	}

	if( m_pCurrentSceneManager_ != NULL)
	{
		// 씬 매니저와 관련된 Helper 객체 지우기
		m_pHelperRender_->DestroyHelperRender();
		CMintManager::GetReference().DestroyMintManager();

		m_pHelperResource_->ResetRefMesh(NULL);

		// SceneManager Clear
		BroadCastMessage(VIEW_MESSAGE::BEFORE_SCENE_CLEAR);

		// Logic Object Manager
		// BEFORE_SCENE_CLEAR 메시지 후, 씬매니저 Clear 직전
		m_pLogicObjectMgr_->ResetLogicObjectManager();

		// 씬 매니저 초기화
		m_pCurrentSceneManager_->Clear();

		// WorkSpace Resource Reset
		CWorkSpace* pCurrentWorkSpace = m_pWorkSpaceMgr_->GetCurrentWorkSpace();
		if (pCurrentWorkSpace)
			pCurrentWorkSpace->OnResetResourceAfterSceneCleared(true);

		// Broadcast Resource destroy message to view
		BroadCastMessage(VIEW_MESSAGE::BEFORE_RES_DESTROY);

		// Destroy Resource
		REngine::DestroySharedResource();
		m_pCurrentSceneManager_->Destroy();

		delete m_pCurrentSceneManager_;
		m_pCurrentSceneManager_ = NULL;
		REngine::RegisterSceneManager(NULL);
	}

	//tmp file 지우기
	m_pHelperResource_->CreateMiniMapAndSetCamera("_minimap.tmp", CControlMediatorHelperResource::MINI_DELETE);
}

void CControlMediator::DestroyDevice()
{
	if( m_pCurrentDevice_ != NULL)
	{
		m_pCurrentDevice_->Destroy();
		delete m_pCurrentDevice_;
		m_pCurrentDevice_ = NULL;
		REngine::RegisterDevice(NULL);
	}
}

bool CControlMediator::LoadSceneAndEnvFromXML_(const char* _pStrZoneName, MAP_EDIT_TYPE _eMapType)
{
	if (_eMapType != MAP_EDIT_TYPE_SCENE)
		m_pHelperResource_->SetMediatorAfterSceneLoaded(_eMapType);

	// 맵툴에서는 AsyncLoading을 하지 않는다.
	if ( !m_pCurrentSceneManager_->LoadEditableSceneXML(_pStrZoneName) )
	{
		m_pCurrentSceneManager_->AddErrorMessage(".zone.xml 파일 읽기 실패");
		//AfxMessageBox(".zone.xml 파일 읽기 실패");
		return false;
	}

	//Set for Environment
	ASSERT(m_pEnvironmentSet_);
	char szBuffer[MAX_PATH];
	szBuffer[0] = '\0';
	strcpy(szBuffer, _pStrZoneName);
	char *p = strrchr(szBuffer,'.');
	strcpy(p,".env.xml");

	CRollupEtcCollision* pCollision = GetViewPtr<CRollupEtcCollision>();
	if( pCollision->GetCollisionTree().GetCollisionTree() == NULL )
		pCollision->GetCollisionTree().Load( pCollision->GetCollisionFileName().c_str() );

	if ( m_pEnvironmentSet_->CreateFromFile(szBuffer, false) == false )
		m_pCurrentSceneManager_->AddErrorMessage("잘못된 환경정보 : env.xml 파일이 잘못되어있습니다.");

	return true;
}

void CControlMediator::SetCurrentWBState(WORKBENCH_STATE::STATE _state)
{
	m_wbPreviousState_ = m_wbCurrentState_;
	m_wbCurrentState_ = _state;
	m_mouseInfo.setIgnoreMouseMovedPickingRActor( false );

	BroadCastMessage( VIEW_MESSAGE::WB_STATE_CHANGED );
}

void CControlMediator::WB_CallBackAction()
{
	// 초기화가 안되어있거나 리소스가 지워진 상태에서 호출되는 경우가 있다.
	if (m_pCurrentCamera_ == NULL)
		return;

	//camera callback action
	m_cameraInfo.m_bCameraMode = m_pCurrentCamera_->DoCameraCallBackAction(this);

	//workbench callBackAction
	if( !m_cameraInfo.m_bCameraMode &&
		CControlDoAction::IsCallBackAction(m_wbCurrentState_) )
		WB_ActionTable_[m_wbCurrentState_](this);
}

void CControlMediator::WB_OnTimeAction()
{
	// 초기화가 안되어있거나 리소스가 지워진 상태에서 호출되는 경우가 있다.
	if (m_pCurrentCamera_ == NULL)
		return;

	if(m_pCurrentCamera_->DoCameraOnTimeActionAndBreak(this, m_bNavigation_))
		return;

	//workbench OnTime Action
	if( !m_cameraInfo.m_bCameraMode &&
		!CControlDoAction::IsCallBackAction(m_wbCurrentState_) &&
		!CMintManager::GetReference().IsActiveQuadPopup() )
		WB_ActionTable_[m_wbCurrentState_](this);
}

string CControlMediator::m_errorString;

string& CControlMediator::GetErrorString()
{
	return m_errorString;
}

void CControlMediator::ClearErrorString()
{
	m_errorString.clear();
}

void CControlMediator::AddErrorString( const char* szText )
{
	m_errorString += szText;
}

void CControlMediator::Report(const FileWatcher::StringVec& changedFiles)
{
	CWorkSpace* pWorkspace = m_pWorkSpaceMgr_->GetCurrentWorkSpace();

	for (FileWatcher::StringVec::const_iterator cur = changedFiles.begin(); 
		cur != changedFiles.end(); 
		++cur)
	{
		mlog("파일이 변경되었습니다. %s\n", (*cur).c_str() );
		if(pWorkspace)
			pWorkspace->OnFileModified( *cur );
	}
}