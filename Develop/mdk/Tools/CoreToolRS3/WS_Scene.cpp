#include "stdafx.h"
#include "WS_Scene.h"

#include "VL_SceneNodeList.h"
#include "VR_TerrainHeight.h"
#include "VR_TerrainAverage.h"
#include "VR_TerrainFlat.h"
#include "VR_TerrainSplat.h"
#include "VR_TerrainRelax.h"
#include "VR_TerrainGrass.h"
#include "VR_ZoneObjectNew.h"
#include "VR_EtcCamera.h"
#include "VR_EtcMapView.h"
#include "VR_EtcDecal.h"
#include "VR_EtcDev.h"

#include "C_XMLResourceList.h"
#include "C_XMLConfig.h"

#include "C_ControlMediatorHelperResource.h"
#include "C_StatusBarHandler.h"
#include "M_ToolLogicObjectManager.h"
#include "M_Visitors.h"
#include "C_ControlMediatorHelperEntityActionImpl_Scene.h"

#include "MFileSystem.h"
#include "RUtil.h"
#include "REffectInstance.h"
#include "REffectManager.h"
// #include "RMeshMgr.h"

#include <algorithm>

// #include "C_ControlDoActionImpl.h"
// #include "C_ControlMediatorHelperActionAddon.h"
// #include "C_ControlMediatorHelperRenderAddon.h"
// #include "C_ControlMediatorHelperResourceAddon.h"
// #include "C_ControlPopupActionImpl.h"
#include "C_KeyEventHandler.h"


const char* CWorkSpaceScene::GetWorkSpaceKeyword()
{
	return "scene.xml";
}

void CWorkSpaceScene::InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction)
{
	// 지형
	int nTerrainTabIdx = _rUiSetter.AddTab("Terrain Height", WORKBENCH_STATE::TERRAIN_HEIGHT);
	_rUiSetter.AddRollup(nTerrainTabIdx, CRollupTerrainHeight::GetID(), true, WORKBENCH_STATE::TERRAIN_HEIGHT, true);
	_rUiSetter.AddRollup(nTerrainTabIdx, CRollupTerrainAverage::GetID(), false, WORKBENCH_STATE::TERRAIN_AVER, true);
	_rUiSetter.AddRollup(nTerrainTabIdx, CRollupTerrainFlat::GetID(), false, WORKBENCH_STATE::TERRAIN_FLAT, true);
	_rUiSetter.AddRollup(nTerrainTabIdx, CRollupTerrainSplat::GetID(), false, WORKBENCH_STATE::TERRAIN_SPLAT, true);
	_rUiSetter.AddRollup(nTerrainTabIdx, CRollupTerrainRelax::GetID(), false, WORKBENCH_STATE::TERRAIN_RELAX, true);
	_rUiSetter.AddRollup(nTerrainTabIdx, CRollupTerrainGrass::GetID(), false, WORKBENCH_STATE::TERRAIN_GRASS, true);

	_rUiSetter.BindWBState(WORKBENCH_STATE::TERRAIN_HEIGHT, nTerrainTabIdx, CRollupTerrainHeight::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::TERRAIN_AVER, nTerrainTabIdx, CRollupTerrainAverage::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::TERRAIN_FLAT, nTerrainTabIdx, CRollupTerrainFlat::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::TERRAIN_SPLAT, nTerrainTabIdx, CRollupTerrainSplat::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::TERRAIN_RELAX, nTerrainTabIdx, CRollupTerrainRelax::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::TERRAIN_GRASS, nTerrainTabIdx, CRollupTerrainGrass::GetID());

	// 물체
	int nObjectTabIdx = _rUiSetter.AddTab("Select Object", WORKBENCH_STATE::OBJECT_SELECT);
	_rUiSetter.AddRollup(nObjectTabIdx, CRollupZoneObjectNew::GetID(), true, WORKBENCH_STATE::OBJECT_NEW, false);

	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_NEW, nObjectTabIdx, CRollupZoneObjectNew::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_SELECT, nObjectTabIdx, CDlgSceneNodeList::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_MOVE, nObjectTabIdx, CDlgSceneNodeList::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_ROTATE, nObjectTabIdx, CDlgSceneNodeList::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_SCALE, nObjectTabIdx, CDlgSceneNodeList::GetID());

	// 기타
	int nEtcTabIdx = _rUiSetter.AddTab("Select Path", WORKBENCH_STATE::ETC_PATH_SELECT);
	_rUiSetter.AddRollup(nEtcTabIdx, CRollupEtcCamera::GetID(), false, WORKBENCH_STATE::ETC_CAMERA, true);
	_rUiSetter.AddRollup(nEtcTabIdx, CRollupEtcMapView::GetID(), false, WORKBENCH_STATE::ETC_MAP_VIEW, true);
	_rUiSetter.AddRollup(nEtcTabIdx, CRollupEtcDev::GetID(), false, WORKBENCH_STATE::ETC_DEV, true);
	_rUiSetter.AddRollup(nEtcTabIdx, CRollupEtcDecal::GetID(), false, WORKBENCH_STATE::ETC_DECAL, true);

	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_CAMERA, nEtcTabIdx, CRollupEtcCamera::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_MAP_VIEW, nEtcTabIdx, CRollupEtcMapView::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_DEV, nEtcTabIdx, CRollupEtcDev::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_DECAL, nEtcTabIdx, CRollupEtcDecal::GetID());

	_rHelperAndAction.m_pPopupActionRepository = new CControlPopupActionRepository;
	_rHelperAndAction.m_pEntityActionHelper = new CControlMediatorHelperEntityActionImpl_Scene( m_pControlMediator_ );

	_rUiSetter.SetDefaultWB(WORKBENCH_STATE::OBJECT_SELECT);
}

void CWorkSpaceScene::HaltWorkSpace( CWorkSpaceHelpersAndActions& _rHelperAndAction )
{
	SAFE_DELETE(_rHelperAndAction.m_pPopupActionRepository);
	SAFE_DELETE(_rHelperAndAction.m_pEntityActionHelper);
}

void CWorkSpaceScene::OnWBObjectEditMouseMessage(VIEW_MESSAGE::ID message)
{
	m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>()->Listen( CoreMessage(message) );
}

MAP_EDIT_TYPE CWorkSpaceScene::OnOpenFile( const char* _pSzFileName )
{
	DWORD dwStartTime = GetTickCount();

	//set zone info
	char sz_path[128];
	char sz_file[128];
	sz_path[0] = '\0';
	sz_file[0] = '\0';
	rs3::SplitPath(_pSzFileName, sz_path, sz_file);
	m_pControlMediator_->m_mapInfo.m_strScenePath = sz_path;
	m_pControlMediator_->m_mapInfo.m_strSceneFile = sz_file;
	m_pControlMediator_->m_mapInfo.m_strImportSceneFile.clear();

	rs3::RToolSceneManager* pSceneManager = m_pControlMediator_->GetToolSceneManager();
	// 맵툴에서는 AsyncLoading을 하지 않는다.
	if ( !pSceneManager->LoadEditableSceneXML(_pSzFileName) )
	{
		pSceneManager->AddErrorMessage(".scene.xml failed to load.");
		//AfxMessageBox(".zone.xml 파일 읽기 실패");
		m_pControlMediator_->m_mapInfo.m_eMapEditType = MAP_EDIT_TYPE_ERROR;

		mlog("zone loading time : %6.2f sec\n", (float)(GetTickCount() - dwStartTime)/1000.f);
		return m_pControlMediator_->m_mapInfo.m_eMapEditType;
	}

	m_pControlMediator_->GetHelperResource()->SetMediatorAfterSceneLoaded();

	//config 파일 로딩
	bool _bWithRelatedConfig = true;
	CControlMediatorHelperResource* pHelperResource = m_pControlMediator_->GetHelperResource();
	if(_bWithRelatedConfig)
		pHelperResource->ManageConfigFileRelatedMap(_pSzFileName, CControlMediatorHelperResource::MAP_CONFIG_LOAD, false);

	m_pControlMediator_->BroadCastMessage( VIEW_MESSAGE::SCENE_LOADED );

	return MAP_EDIT_TYPE_SCENE;
}

std::string CWorkSpaceScene::OnSaveFile(const char* _pSzFileName, bool bSaveAs)
{
	CStatusBarHandler::GetInstance()->ShowStatusBarMessage("맵 저장중");

	m_pControlMediator_->GetToolSceneManager()->SaveRootSceneToXML( _pSzFileName );

	CControlMediatorHelperResource* pHelperResource = m_pControlMediator_->GetHelperResource();
	//config file save
	if(!bSaveAs)
		pHelperResource->ManageConfigFileRelatedMap(_pSzFileName, CControlMediatorHelperResource::MAP_CONFIG_SAVE, true);
	else 
		pHelperResource->ManageConfigFileRelatedMap(_pSzFileName, CControlMediatorHelperResource::MAP_CONFIG_SAVE_AS, true);

	return _pSzFileName;
}

void CWorkSpaceScene::OnFileModified( const string& strFileName )
{
	if( strFileName.find( ".effect.scene.xml", 0) != string::npos )
		ReloadEffect( strFileName );
	else if( strFileName.find( ".scene.xml", 0) != string::npos )
		ReloadSceneInstance( strFileName );
}

void CWorkSpaceScene::ReloadEffect( const string& strFileName )
{
	mlog( "%s 이펙트를 다시 읽습니다\n", strFileName.c_str() );

	string strEffectFileName = MGetPureFileNameExt( strFileName );
	string strEffectName = m_pControlMediator_->GetToolSceneManager()->GetEffectNameFromFileName( strEffectFileName );

	// effect manager 에서 리로드
	m_pControlMediator_->GetToolSceneManager()->GetEffectManager()->ClearEffectSource();

	// effectinstancescenenode 리로드
	CEffectInstanceReloader effectInstanceReloader( strEffectName );

	rs3::RWorldSceneNode& rWorldSceneNode = m_pControlMediator_->GetToolSceneManager()->GetRootSceneNode();
	rWorldSceneNode.Traverse(&effectInstanceReloader);
}

void CWorkSpaceScene::ReloadSceneInstance( const string& strFileName )
{
	mlog( "%s 신을 다시 읽습니다\n", strFileName.c_str() );

	// sceneinstancescenenode 리로드
	string strSceneFileName = MGetPureFileNameExt( strFileName );

	rs3::RWorldSceneNode& rWorldSceneNode = m_pControlMediator_->GetToolSceneManager()->GetRootSceneNode();

	// 여러군데 사용되어있을수 있으니까 순회해서 모든곳에서 clear 한 이후 다시 로딩
	CSceneInstanceClearVisitor sceneInstanceClearVisitor( strSceneFileName );
	rWorldSceneNode.Traverse(&sceneInstanceClearVisitor);

	// 캐쉬가 사용되지 않으므로 의미없네요
// 	rs3::RMeshMgr* pMeshMgr = m_pControlMediator_->GetToolSceneManager()->GetResourceMgr< rs3::RMeshMgr >();
// 	pMeshMgr->ClearCache();

	// 재로딩
	CSceneInstanceReloader sceneInstanceReloader( strSceneFileName );
	rWorldSceneNode.Traverse(&sceneInstanceReloader);
}
