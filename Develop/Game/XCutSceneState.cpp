#include "stdafx.h"
#include "XCutSceneState.h"
#include "XWorld.h"
#include "MockWorld.h"
#include "XEffectManager.h"
#include "XRealSound.h"
#include "XCameraManager.h"
#include "XWidgetNameDef.h"
#include "XMyPlayer.h"
#include "XModuleEntity.h"
#include "XCharacter.h"
#include "XCutSceneController.h"
#include "XGameStateMgr.h"
#include "XScreenFader.h"

//////////////////////////////////////////////////////////////////////////
#include "XPost_Cutscene.h"

#include "XCutSceneContent.h"
#include "XCutSceneGameEntity.h"
#include "XCameraCutSceneController.h"
#include "XCutSceneObjectController.h"
#include "XCutSceneObjectControllerMove.h"
#include "XCutSceneFieldIntroMgr.h"

//////////////////////////////////////////////////////////////////////////

class XCutSceneContentBuilderForGame : public XCutSceneContentBuilder
{
public:
	XCutSceneContentBuilderForGame(XCutSceneState* pCutSceneState, XObjectManager*	pObjectManager, XCutSceneContent* pCutSceneContent)
	{
		m_pCutSceneState = pCutSceneState;
		m_pObjectManager = pObjectManager;
		m_pCutSceneContent = pCutSceneContent;
	}

	virtual ~XCutSceneContentBuilderForGame()
	{
	}

protected:
	// clapper
	virtual bool OnClapperCreated(XCutSceneClapper* pClapper)
	{
		return true;
	}

	// entity
	virtual XCutSceneObjectEntity* OnCreateCameraEntity()
	{
		return new XCutSceneCameraEntity();
	}

	virtual XCutSceneObjectEntity* OnCreateActorEntity(const std::wstring& _rStrType)
	{
		return new XCutSceneActorEntity(_rStrType, m_pObjectManager);
	}

	virtual XCutSceneObjectEntity* OnCreateEventEntity(MXmlElement* _pEventElement)
	{
		return new XCutSceneEventEntity(m_pCutSceneContent, _pEventElement);
	}

private:
	XCutSceneState*		m_pCutSceneState;
	XObjectManager*		m_pObjectManager;
	XCutSceneContent*	m_pCutSceneContent;
};

//////////////////////////////////////////////////////////////////////////

XCutSceneState::XCutSceneState( XGameStateMgr * pStateMgr, bool bRealGameNotTest /*= true*/ )
: XGameState(pStateMgr), m_pCutSceneContent(NULL)
{
	m_bRealGameNotTest				= bRealGameNotTest;
	m_bUseGameWorld					= false;

	m_nCutSceneID					= 0;

	m_pController = new XCutSceneController(this);
	m_pScreenFader = new XScreenFader;

	Create();
}

XCutSceneState::~XCutSceneState()
{
	SAFE_DELETE(m_pCutSceneContent);
	SAFE_DELETE(m_pScreenFader);
}

bool XCutSceneState::Create()
{
	return XGameState::Create();
}

bool XCutSceneState::Ready( XGameState* pPreState, int nParam /*= 0*/ )
{
	_ASSERT( nParam != 0);

	m_bUseGameWorld = false;
	m_nCutSceneID = nParam;

	// 컷신 데이터 로딩

	// 카메라 위치
	m_vStartPos = gvar.Game.pMyPlayer->GetPosition();
	m_vStartDir = gvar.Game.pMyPlayer->GetDirection();
	m_vStartUp	= gvar.Game.pMyPlayer->GetUpvector();

	// 우선 테스트로 같은 맵을 쓰자.
	m_pWorld = pPreState->GetWorld();
	m_bUseGameWorld = true;
	wchar_t buff[256] = {0, };
	swprintf_s(buff, L"%s%d%s", PATH_CUTSCENE, nParam, FILEEXT_CUTSCENE);

	// CutScene Content 로딩하기
	_ASSERT(m_pCutSceneContent == NULL);
	m_pCutSceneContent = new XCutSceneContent;
	XCutSceneContentBuilderForGame builder(this, m_pObjectManager, m_pCutSceneContent);
	bool bRet = builder.Build(buff, m_pCutSceneContent);

	// 카메라를 첫번째 위치로
	SetCameraStartPos();

	// 오브젝트 리소스 레퍼런스 카운트 추가
	m_pOwnerStateMgr->m_ResourceMgr.AddObjectResourceReferenCount();

	if (!bRet && m_bRealGameNotTest)
	{
		EndCutScene();
		
		return false;
	}

	return XGameState::Ready(pPreState);
}

void XCutSceneState::LoadResource()
{
	// 카메라 설정
	global.camera->SetPrimaryCamera(CAMERA_CUT_SCENE);

	// UI 숨기기
	if (global.ui) 
	{
		global.ui->HideSheet(UI_GAME_SHEET_NAME);
		global.ui->ShowSheet(UI_CUTSCENE_SHEET_NAME);
	}

	//////////////////////////////////////////////////////////////////////////
	// 생성
	// 월드 생성
	//_CreateWorld();

	XCameraCutSceneController* pCameraController = global.camera->GetCameraController<XCameraCutSceneController>();
	if (pCameraController)
	{
		pCameraController->Init(m_vStartPos, m_vStartDir);
	}
}

void XCutSceneState::Enter()
{
	if (m_pCutSceneContent)
	{
		VEC_CUTSCNEOBJ& objs = m_pCutSceneContent->GetCutSceneObjects();
		for (VEC_CUTSCNEOBJ::iterator itr = objs.begin(); itr != objs.end(); ++itr)
		{
			XCutSceneGameEntity* pEntity = static_cast<XCutSceneGameEntity*>((*itr)->GetEntity());
			if (pEntity)
				pEntity->OnEnter();
		}
	}
}

void XCutSceneState::Exit()
{
	// UI 보이기
	if (global.ui) 
	{
		global.ui->ShowSheet(UI_GAME_SHEET_NAME);
		global.ui->HideSheet(UI_CUTSCENE_SHEET_NAME);
	}

	//m_pWorld->Destroy();
	//SAFE_DELETE(m_pWorld);
	m_pWorld = NULL;

	// CutScene Content
	SAFE_DELETE(m_pCutSceneContent);

	// sound
	if ( global.sound)
		global.sound->StopAll();

	XGameState::Exit();
}

void XCutSceneState::Destroy()
{
	if(m_bUseGameWorld)
	{
		m_pWorld = NULL;
	}

	XGameState::Destroy();
}

void XCutSceneState::EndCutScene()
{
	SAFE_DELETE(m_pCutSceneContent);
	XPostCutscene_EndReq(m_nCutSceneID);
	m_nCutSceneID = 0;

	XCutSceneFieldIntroMgr::GetCutSceneFieldIntroMgr().MoveMyPlayerPos();
	XCutSceneFieldIntroMgr::GetCutSceneFieldIntroMgr().InitBlockProc(0, 8);
	XCutSceneFieldIntroMgr::GetCutSceneFieldIntroMgr().ReleaseResource();
}

void XCutSceneState::Update( float fDelta )
{
	XGameState::Update(fDelta);

	// 로딩이 완료 되기 전까지 컷씬을 대기 시킵니다.
	if(CheckCutSceneObjectLoadingComplete() == false)
	{
		XCutSceneFieldIntroMgr::GetCutSceneFieldIntroMgr().SetObjectLoading(true);

		return;
	}

	if ( global.sound)
		global.sound->SetSilence( false);

	XCutSceneFieldIntroMgr::GetCutSceneFieldIntroMgr().SetObjectLoading(false);

	//////////////////////////////////////////////////////////////////////////
	// 컷씬 업데이트
	if (m_pCutSceneContent)
	{
		m_pCutSceneContent->Run(fDelta);
		XCutSceneClapper* pClapper = m_pCutSceneContent->GetClapper();
		_ASSERT(pClapper);

		if (pClapper->GetDuration() < m_pCutSceneContent->GetCurrnetRunningTime() ||
			m_pCutSceneContent->IsCutSceneSkip() == true)		
		{//컷씬 종료 처리
			EndCutScene();
		}
	}
}

void XCutSceneState::OnWorldLoadingComplete()
{

}

XCutScenePlayer* XCutSceneState::InCutScenePlayer( MUID& uid )
{
	return NULL;
}

bool XCutSceneState::_CreateWorld()
{
	// 월드 생성
	// 컷신 데이터로 필요한 월드를 생성한다.

	// 현재 테스트로 같은 월드 생성
	if(m_bRealGameNotTest)
	{
		m_pWorld = new XWorld();
	}
	else
	{
		m_pWorld = new MockWorld();
	}

	// 필드 생성
	XWorldInitParam worldInitParam;
	worldInitParam.SetField(gvar.World.GetFieldID(), true);

	bool bInitWorldRet = m_pWorld->Init(worldInitParam);

	if (bInitWorldRet == false)
		return false;

	// 환경 설정
	XWorldEnv* pEnv = gg.currentgamestate->GetWorld()->GetWorldEnv();
	m_pWorld->OnCmdSetEnv(gvar.World.nGameTime, gvar.World.nWeather);

	return true;
}

void XCutSceneState::_DestroyWorld()
{
	m_pWorld->Final();
	SAFE_DELETE(m_pWorld);
}

void XCutSceneState::GetDataForSoundListener( vec3& vPos, vec3& vDir )
{
	if ( global.camera  &&  global.camera->GetPrimaryCamera())
	{
		vPos = global.camera->GetPrimaryCamera()->GetWorldPosition();
		vDir = global.camera->GetPrimaryCamera()->GetWorldDirection();
	}
}

bool XCutSceneState::CheckCutSceneObjectLoadingComplete()
{
	bool bAllLoadingComplete = true;;

	for(XObjectMap::iterator itObject = m_pObjectManager->BeginItor(); itObject != m_pObjectManager->EndItor(); itObject++)
	{
		XModuleEntity* pModuleEntity = itObject->second->GetModuleEntity();
		if(pModuleEntity)
		{
			if(pModuleEntity->IsLoadingCompleted() == false)
			{
				bAllLoadingComplete = false;
			}
		}
	}

	return bAllLoadingComplete;
}

void XCutSceneState::SetCameraStartPos()
{
	XCutSceneObjectCamera* pCutSceneObjectCamera = dynamic_cast<XCutSceneObjectCamera*>(m_pCutSceneContent->FindCutSceneObject(L"camera"));
	// 게임일때만 체크한다.
	if(m_bRealGameNotTest)
		_ASSERT(pCutSceneObjectCamera);
	if(pCutSceneObjectCamera)
	{
		for(VEC_CUTSCENE_CONTROLLERS::iterator itControllers = pCutSceneObjectCamera->GetControllers().begin(); itControllers != pCutSceneObjectCamera->GetControllers().end(); ++itControllers)
		{
			if((*itControllers)->GetType() == XCutSceneObjectController::E_MOVE)
			{
				XCutSceneObjectControllerMove* pObjectControllersMove = dynamic_cast<XCutSceneObjectControllerMove*>(*itControllers);
				float fTempTime;
				bool bWarp;
				pObjectControllersMove->GetKeyInfo(0, fTempTime, m_vStartPos, m_vStartDir, m_vStartUp, bWarp);
			}
		}
	}
}

void XCutSceneState::SetControlNetCmd()
{
	m_ControlNetCmdState.SetUsableCmdID(MC_CUTSCENE_BEGIN_ME);
	m_ControlNetCmdState.SetUsableCmdID(MC_CUTSCENE_BEGIN_OTHER);
	m_ControlNetCmdState.SetUsableCmdID(MC_CUTSCENE_END_ME);
	m_ControlNetCmdState.SetUsableCmdID(MC_CUTSCENE_END_OTHER);
}

void XCutSceneState::Render()
{
	if (m_pScreenFader)	m_pScreenFader->Draw();
}

void XCutSceneState::FadeUpdate( float fFadingAlpha )
{
	if (m_pScreenFader)	m_pScreenFader->FadeUpdate(fFadingAlpha);
}

void XCutSceneState::FadeEnd()
{
	if (m_pScreenFader)	m_pScreenFader->FadeEnd();
}

bool XCutSceneState::OnEvent(MEvent& e)
{
	if(m_pCutSceneContent)
	{
		m_pCutSceneContent->OnEvent(e);
		return true;
	}

	return false;
}
