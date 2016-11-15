#include "stdafx.h"

#include "MTypelist.h"

#include "VR_CoreToolRS3.h"
#include "VR_TerrainHeight.h"
#include "VR_TerrainAverage.h"
#include "VR_TerrainFlat.h"
#include "VR_TerrainSplat.h"
#include "VR_TerrainRelax.h"
#include "VR_TerrainGrass.h"
#include "VR_ZoneObjectNew.h"
#include "VR_EnvWeather.h"
#include "VR_EtcCamera.h"
#include "VR_EtcMapView.h"
#include "VR_EtcDecal.h"
#include "VR_EtcDev.h"
#include "VR_EtcCollision.h"
#include "VL_ToolBar.h"
#include "VL_StatusBar.h"
#include "VL_ResourceInfo.h"
#include "VL_RelaxAll.h"
#include "VL_SceneNodeList.h"
#include "VL_Objects.h"
#include "VG_LogicObjectRenderer.h"
#include "VG_Gizmo.h"
#include "VG_DefaultLight.h"

//#include "WS_PhysX.h"
// #include "WS_CutScene.h"
#include "WS_Script.h"
#include "WS_Scene.h"

const char* SIDE_TABS[] = {
	"All",
	"CoreToolRS3",
	"지형",
	"물체",
	"환경",
	"Entity",
	"기타",
	"물리",
	"컷씬",
	"ServerLogic",
	"Animation",
	"NPC Types",
	"Objects",
	"ObjectDef",
};

#include "CorePlugin.h"

bool CorePlugin::Initialize( CWorkSpaceManager* pWorkSpaceManager )
{
	// rollup "new"

	HINSTANCE hInstance = AfxGetInstanceHandle();
	pWorkSpaceManager->RegisterView<CRollupCoreToolRS3>(hInstance);
	pWorkSpaceManager->RegisterView<CRollupTerrainHeight>(hInstance);
	pWorkSpaceManager->RegisterView<CRollupTerrainAverage>(hInstance);
	pWorkSpaceManager->RegisterView<CRollupTerrainFlat>(hInstance);
	pWorkSpaceManager->RegisterView<CRollupTerrainSplat>(hInstance);
	pWorkSpaceManager->RegisterView<CRollupTerrainRelax>(hInstance);
	pWorkSpaceManager->RegisterView<CRollupTerrainGrass>(hInstance);
	pWorkSpaceManager->RegisterView<CRollupZoneObjectNew>(hInstance);
	pWorkSpaceManager->RegisterView<CRollupEnvWeather>(hInstance);
	pWorkSpaceManager->RegisterView<CRollupEtcCamera>(hInstance);
	pWorkSpaceManager->RegisterView<CRollupEtcMapView>(hInstance);
	pWorkSpaceManager->RegisterView<CRollupEtcDecal>(hInstance);
	pWorkSpaceManager->RegisterView<CRollupEtcDev>(hInstance);
	pWorkSpaceManager->RegisterView<CRollupEtcCollision>(hInstance);

	/// 다이얼로그 new 등록 , 공통부분
	pWorkSpaceManager->RegisterView<CCoreToolToolBar>(hInstance);
	pWorkSpaceManager->RegisterView<CCoreToolStatusBar>(hInstance);
	pWorkSpaceManager->RegisterView<CDlgResourceInfo>(hInstance);
	pWorkSpaceManager->RegisterView<CDlgRelaxAll>(hInstance);
	pWorkSpaceManager->RegisterView<CDlgSceneNodeList>(hInstance);
	pWorkSpaceManager->RegisterView<CDlgObjects>(hInstance);

	// general view
	pWorkSpaceManager->RegisterView<VG_Gizmo>(hInstance);
	pWorkSpaceManager->RegisterView<VG_LogicObjectRenderer>(hInstance);
	pWorkSpaceManager->RegisterView<VG_DefaultLight>(hInstance);


	// side tab 등록
	int nTabCount = _countof(SIDE_TABS);
	for (int i = 0; i < nTabCount; ++i)
		pWorkSpaceManager->RegisterSideTab( SIDE_TABS[i], i );

	//////////////////////////////////////////////////////////////////////////
	// 웍스페이스 등록

	// 기본 workspace 생성
	pWorkSpaceManager->RegisterWorkspace<CWorkSpaceDefault>();
	pWorkSpaceManager->RegisterWorkspace<CWorkSpaceScript>();
	pWorkSpaceManager->RegisterWorkspace<CWorkSpaceScene>();

	// 기본 icon 등록
	pWorkSpaceManager->RegisterIconImage( "unknown.png" );
	pWorkSpaceManager->RegisterIconImage( "folder.png" );
	pWorkSpaceManager->RegisterIconImage( "folder_star.png" );

	// ws_scene 용 icon image list 등록
	int nBaseImageListIndex = pWorkSpaceManager->RegisterIconImage( "actor.png" );
	pWorkSpaceManager->RegisterIconImage( "effect.png" );
	pWorkSpaceManager->RegisterIconImage( "dirlight.png" );
	pWorkSpaceManager->RegisterIconImage( "Light.png" );
	pWorkSpaceManager->RegisterIconImage( "Spotlight.png" );
	pWorkSpaceManager->RegisterIconImage( "trail.png" );
	pWorkSpaceManager->RegisterIconImage( "tree.png" );
	pWorkSpaceManager->RegisterIconImage( "sceneinstance.png" );

	return true;
}

void CorePlugin::Finalize( CWorkSpaceManager* pWorkSpaceManager )
{
	// 롤업
	pWorkSpaceManager->UnregisterView<CRollupCoreToolRS3>();
	pWorkSpaceManager->UnregisterView<CRollupTerrainHeight>();
	pWorkSpaceManager->UnregisterView<CRollupTerrainAverage>();
	pWorkSpaceManager->UnregisterView<CRollupTerrainFlat>();
	pWorkSpaceManager->UnregisterView<CRollupTerrainSplat>();
	pWorkSpaceManager->UnregisterView<CRollupTerrainRelax>();
	pWorkSpaceManager->UnregisterView<CRollupTerrainGrass>();
	pWorkSpaceManager->UnregisterView<CRollupZoneObjectNew>();
	pWorkSpaceManager->UnregisterView<CRollupEnvWeather>();
	pWorkSpaceManager->UnregisterView<CRollupEtcCamera>();
	pWorkSpaceManager->UnregisterView<CRollupEtcMapView>();
	pWorkSpaceManager->UnregisterView<CRollupEtcDecal>();
	pWorkSpaceManager->UnregisterView<CRollupEtcDev>();
	pWorkSpaceManager->UnregisterView<CRollupEtcCollision>();

	/// 다이얼로그
	pWorkSpaceManager->UnregisterView<CCoreToolToolBar>();
	pWorkSpaceManager->UnregisterView<CCoreToolStatusBar>();
	pWorkSpaceManager->UnregisterView<CDlgResourceInfo>();
	pWorkSpaceManager->UnregisterView<CDlgRelaxAll>();
	pWorkSpaceManager->UnregisterView<CDlgSceneNodeList>();
	pWorkSpaceManager->UnregisterView<CDlgObjects>();

	// general view
	pWorkSpaceManager->UnregisterView<VG_Gizmo>();
	pWorkSpaceManager->UnregisterView<VG_LogicObjectRenderer>();
	pWorkSpaceManager->UnregisterView<VG_DefaultLight>();

	// 기본 workspace 등록해제
	pWorkSpaceManager->UnregisterWorkspace<CWorkSpaceScene>();
	pWorkSpaceManager->UnregisterWorkspace<CWorkSpaceScript>();
	pWorkSpaceManager->UnregisterWorkspace<CWorkSpaceDefault>();

}

//////////////////////////////////////////////////////////////////////////
// Default Workspace
const char*	CorePlugin::CWorkSpaceDefault::DEFAULT_WORKSPACE_NAME = "default";

void CorePlugin::CWorkSpaceDefault::InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction)
{
	// minimal ui
	int nDefaultTabIdx = _rUiSetter.AddTab("CoreToolRS3", WORKBENCH_STATE::ETC_MAP_VIEW);
	_rUiSetter.AddRollup(nDefaultTabIdx, CRollupCoreToolRS3::GetID(), true, WORKBENCH_STATE::ETC_MAP_VIEW, false);

	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_MAP_VIEW, nDefaultTabIdx, CRollupCoreToolRS3::GetID());
}

HINSTANCE CorePlugin::GetInstanceHandle()
{
	return AfxGetInstanceHandle();
}
