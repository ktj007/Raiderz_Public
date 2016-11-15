#include "stdafx.h"
#include "WS_CutScene.h"
#include "WS_Field.h"

#include "VR_CutSceneObjectList.h"

#include "VR_EnvWeather.h"
#include "VR_EntityObjectList.h"
#include "VR_EtcMapView.h"

#include "C_ControlMediatorHelperResource.h"
#include "C_ControlMediatorHelperRender.h"
#include "C_StatusBarHandler.h"
#include "M_ToolLogicObjectManager.h"

#include "RRenderHelper.h"

#include "WS_CutSceneXCutSceneWrapper.h"
#include "WS_CutSceneContentBuilder.h"
#include "../../Game/XCutSceneContent.h"

#include "C_ControlDoActionImpl.h"
#include "C_ControlMediatorHelperActionAddon.h"
#include "C_ControlMediatorHelperRenderAddon.h"
#include "C_ControlMediatorHelperResourceAddon.h"

void CWorkSpaceCutScene::InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction)
{
	CXCutSceneWrapper::InitCXCutSceneWrapper( m_pControlMediator_ );

	// 컷씬
	int nCutSceneTabIdx = _rUiSetter.AddTab("컷씬", WORKBENCH_STATE::OBJECT_SELECT);
	_rUiSetter.AddRollup(nCutSceneTabIdx, CRollupCutSceneObjectList::GetID(), true, WORKBENCH_STATE::OBJECT_SELECT, false);

	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_SELECT, nCutSceneTabIdx, CRollupCutSceneObjectList::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_MOVE, nCutSceneTabIdx, CRollupCutSceneObjectList::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_ROTATE, nCutSceneTabIdx, CRollupCutSceneObjectList::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_SCALE, nCutSceneTabIdx, CRollupCutSceneObjectList::GetID());

	// 환경
	int nEvnTabIdx = _rUiSetter.AddTab("환경");
	_rUiSetter.AddRollup(nEvnTabIdx, CRollupEnvWeather::GetID(), true, WORKBENCH_STATE::STATE_NUM, false);

	// 엔티티
	int nEntityTabIdx = _rUiSetter.AddTab("Entity");
	_rUiSetter.AddRollup(nEntityTabIdx, CRollupEntityObjectList::GetID(), true, WORKBENCH_STATE::STATE_NUM, true);

	// 기타
	int nEtcTabIdx = _rUiSetter.AddTab("기타", WORKBENCH_STATE::ETC_MAP_VIEW);
	_rUiSetter.AddRollup(nEtcTabIdx, CRollupEtcMapView::GetID(), true, WORKBENCH_STATE::ETC_MAP_VIEW, false);
	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_MAP_VIEW, nEtcTabIdx, CRollupEtcMapView::GetID());

	// 툴바
	_rUiSetter.m_sToolBarButton.m_bLightMap = false;
	_rUiSetter.m_sToolBarButton.m_bOnTerrain = false;
	_rUiSetter.m_sToolBarButton.m_bClip = true;

	_rUiSetter.SetDefaultWB(WORKBENCH_STATE::OBJECT_SELECT);

	// helpers and actions
	_rHelperAndAction.m_pDoActionObjectMove = new CDoActionObjectMoveImpl;
	_rHelperAndAction.m_pDoActionObjectNew = new CDoActionObjectNewImpl;
	_rHelperAndAction.m_pDoActionObjectRotate = new CDoActionObjectRotateImpl;
	_rHelperAndAction.m_pDoActionObjectScale = new CDoActionObjectScaleImpl;
	_rHelperAndAction.m_pDoActionObjectSelect = new CDoActionObjectSelectImpl;
	_rHelperAndAction.m_pEntityActionHelper = new CControlMediatorHelperEntityActionImpl(m_pControlMediator_);
	_rHelperAndAction.m_pNewMapResourceHelper = new CControlMediatorHelperNewMapResourceImpl(m_pControlMediator_, CControlMediatorHelperNewMapResourceImpl::E_CUTSCENE);
	_rHelperAndAction.m_pObjectRenderHelper = new CControlMediatorHelperObjectRendererImpl;
}

void CWorkSpaceCutScene::HaltWorkSpace( CWorkSpaceHelpersAndActions& _rHelperAndAction )
{
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectMove);
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectNew);
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectRotate);
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectScale);
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectSelect);
	SAFE_DELETE(_rHelperAndAction.m_pEntityActionHelper);
	SAFE_DELETE(_rHelperAndAction.m_pNewMapResourceHelper);
	SAFE_DELETE(_rHelperAndAction.m_pObjectRenderHelper);
}

MAP_EDIT_TYPE CWorkSpaceCutScene::OnOpenFile(const char* _pSzFileName)
{
	XCutSceneContent* pContent = new XCutSceneContent;
	CCutSceneContentBuilder builder(pContent);

	if (!builder.Build(_pSzFileName, pContent))
	{
		CStatusBarHandler::GetInstance()->ShowStatusBarMessage("컷씬 xml 읽기 실패");
		delete pContent;
		return MAP_EDIT_TYPE_ERROR;
	}

	XCutSceneClapper* pClapper = pContent->GetClapper();

	// TODO: 좀더 아름답게
//	if (!m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->LoadField( pClapper->GetField()))

	const char* FIELD_WORKSPACE_KEYWORD = "field.xml";
	CWorkSpaceField* pWorkspaceField = static_cast<CWorkSpaceField*>(m_pControlMediator_->GetWorkSpaceMgr()->GetWorkSpace(FIELD_WORKSPACE_KEYWORD));
	if (!pWorkspaceField->LoadField( pClapper->GetField()))
	{
		delete pContent;
		return MAP_EDIT_TYPE_ERROR;
	}

	//config 파일 로딩
	m_pControlMediator_->GetHelperResource()->ManageConfigFileRelatedMap(_pSzFileName, CControlMediatorHelperResource::MAP_CONFIG_LOAD, false);

	//필드가 로딩되었다고 view에게 알리기
	m_pControlMediator_->BroadCastMessage(VIEW_MESSAGE::LOGIC_LOADED);

	//Field Entity들을 읽기 전용으로
	m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->SetEditingModeReadOnly(true, false);

	//CutScene UI
	m_pControlMediator_->GetViewPtr<CRollupCutSceneObjectList>()->ResetUI(pContent);

	return MAP_EDIT_TYPE_LOGIC;
}

std::string CWorkSpaceCutScene::OnSaveFile(const char* _pSzFileName, bool bSaveAs)
{
	CStatusBarHandler::GetInstance()->ShowStatusBarMessage("컷씬 저장중");

	if (m_pControlMediator_->GetViewPtr<CRollupCutSceneObjectList>()->SaveToXMLCurrentContent(_pSzFileName) )
	{
		m_pControlMediator_->GetHelperResource()->ManageConfigFileRelatedMap(_pSzFileName, CControlMediatorHelperResource::MAP_CONFIG_SAVE, false);
		CStatusBarHandler::GetInstance()->ShowStatusBarMessage("컷씬 저장완료");
	}
	else
	{
		AfxMessageBox("저장 실패");
	}
	return _pSzFileName;
}

void CWorkSpaceCutScene::OnResetResourceAfterSceneCleared(bool bOnDestroy)
{
	if (!bOnDestroy)
	{
		m_pControlMediator_->GetViewPtr<CRollupCutSceneObjectList>()->CloseCutSceneEditor();
	}
}

void CWorkSpaceCutScene::OnBetweenCoreToolUpdateAndRS3Render()
{
	//m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->SetVisibleFieldNPC();
	m_pControlMediator_->GetViewPtr<CRollupCutSceneObjectList>()->RunCutScene();
}

void CWorkSpaceCutScene::OnWBDevAction()
{
}

void CWorkSpaceCutScene::OnWBHelperRender()
{
	if (m_pControlMediator_->GetViewPtr<CRollupCutSceneObjectList>()->IsRunningCutScene())
		return;

	rs3::RRenderHelper::SetRenderState();

	LOGIC_OBJECTLIST& rList = m_pControlMediator_->GetToolLogicObjectMgr()->GetNoneEntityObjectList();
	for (LOGIC_OBJECTLIST::iterator itr = rList.begin(); itr != rList.end(); ++itr)
	{
		(*itr)->Render();
	}

	rs3::RRenderHelper::EndRenderState();
}

bool CWorkSpaceCutScene::OnCheckAlwaysPickingWhenMoused()
{
	return !m_pControlMediator_->GetViewPtr<CRollupCutSceneObjectList>()->IsRunningCutScene();
}
