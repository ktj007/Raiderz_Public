#include "stdafx.h"
#include "WS_RzDev.h"

#include "VR_EtcMapView.h"
#include "VR_EtcDev.h"

#include "C_ControlDoActionImpl.h"
#include "C_ControlMediatorHelperActionAddon.h"
#include "C_ControlMediatorHelperRenderAddon.h"
#include "C_ControlMediatorHelperResourceAddon.h"

#include "REffectSceneNode.h"
#include "REffectInstance.h"

void CWorkSpaceRzDev::InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction)
{
	// 기타
	int nEtcTabIdx = _rUiSetter.AddTab("기타", WORKBENCH_STATE::ETC_MAP_VIEW);
	_rUiSetter.AddRollup(nEtcTabIdx, CRollupEtcMapView::GetID(), true, WORKBENCH_STATE::ETC_MAP_VIEW, true);
	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_MAP_VIEW, nEtcTabIdx, CRollupEtcMapView::GetID());

	_rUiSetter.AddRollup(nEtcTabIdx, CRollupEtcDev::GetID(), true, WORKBENCH_STATE::ETC_DEV, true);
	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_DEV, nEtcTabIdx, CRollupEtcDev::GetID());

	// 툴바
	_rUiSetter.m_sToolBarButton.m_bAxis = false;
	_rUiSetter.m_sToolBarButton.m_bWeather = false;
	_rUiSetter.m_sToolBarButton.m_bLightMap = false;
	_rUiSetter.m_sToolBarButton.m_bOnTerrain = false;
	_rUiSetter.m_sToolBarButton.m_bLock = false;
	_rUiSetter.m_sToolBarButton.m_bClip = false;

	_rUiSetter.SetDefaultWB(WORKBENCH_STATE::ETC_MAP_VIEW);

	// helpers and actions
	_rHelperAndAction.m_pDoActionObjectMove = new CDoActionObjectMoveImpl;
	_rHelperAndAction.m_pDoActionObjectNew = new CDoActionObjectNewImpl;
	_rHelperAndAction.m_pDoActionObjectRotate = new CDoActionObjectRotateImpl;
	_rHelperAndAction.m_pDoActionObjectScale = new CDoActionObjectScaleImpl;
	_rHelperAndAction.m_pDoActionObjectSelect = new CDoActionObjectSelectImpl;
}

void CWorkSpaceRzDev::HaltWorkSpace( CWorkSpaceHelpersAndActions& _rHelperAndAction )
{
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectMove);
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectNew);
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectRotate);
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectScale);
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectSelect);
}

using namespace rs3;

class DevTestInterface
{
public:
	DevTestInterface(){}
	virtual ~DevTestInterface(){}

	virtual void init() = 0;
	virtual void halt() = 0;
	virtual void run() = 0;

	CControlMediator* m_pControlMediator_;
};

class TestEffect : public DevTestInterface
{
	virtual void init()
	{
		pEffectInstanceSceneNode = new REffectInstanceSceneNode;
		bool bRet = pEffectInstanceSceneNode->Create( "ef_bparts" );
		_ASSERT(bRet);
	}

	virtual void halt()
	{
		for (std::list<REffectInstanceSceneNode*>::iterator itr = effects.begin(); itr != effects.end(); ++itr)
		{
			delete *itr;
		}
		delete pEffectInstanceSceneNode;
	}

	virtual void run()
	{
		elapsed += REngine::GetTimer().GetElapsedTime();
		if (elapsed > 8000 || !bInit)
		{
			//REngine::GetConfig().m_bLogResource = true;
			REngine::m_uintDebugInfo.clear();
			REngine::m_strDebugInfo.clear();

			REngine::m_strDebugFlags = "/check_actor_createtime/check_resource_loadtime";

			std::vector<unsigned int> checked_time;
			std::vector<size_t> message_size;
			size_t prev_message = 0;

			RTimeChecker100us tc;
			for (int i = 0; i < 100; ++i)
			{
				int nSize = 10000000;
				char* test_byte = new char[nSize];
				memset(test_byte, 0, nSize * sizeof(char));
				sprintf(test_byte, "%d", i);
				tc.Start();
				REffectInstanceSceneNode* pEffectInstance = static_cast<REffectInstanceSceneNode*>(REngine::GetSceneManagerPtr()->CreateSceneNode(RSID_EFFECTINSTANCE));
				if( pEffectInstance->Create( "ef_bparts" ) )
				{
					unsigned int elasped_effect = tc.Check();

					REngine::GetSceneManager().AttachEffect(pEffectInstance, NULL, NULL);
					effects.push_back(pEffectInstance);
					checked_time.push_back(elasped_effect);
					message_size.push_back(REngine::m_strDebugInfo.size() - prev_message);
					prev_message = REngine::m_strDebugInfo.size();
				}
				delete test_byte;
			}
			elapsed = 0;
			bInit = true;

			int nCnt = (int)checked_time.size();
			int nMessageCnt = 0;

			unsigned int nLargesTime = 0;
			int nLagestIdx = 0;
			int nLoagestMessage = 0;
			size_t nLongestMsgEnd = 0;
			for (int i = 0; i < nCnt; ++i)
			{
				if (nLargesTime < checked_time[i])
				{
					nLargesTime = checked_time[i];
					nLagestIdx = i;
					nLoagestMessage = nMessageCnt;
					nLongestMsgEnd = message_size[i];
				}
				nMessageCnt += message_size[i];
			}

			mlog("Longest Time : %u\n", checked_time[nLagestIdx]);
			for (size_t i = 0; i < nLongestMsgEnd; ++i)
			{
				mlog("%s : %u\n", REngine::m_strDebugInfo[nLoagestMessage+i].c_str(), REngine::m_uintDebugInfo[nLoagestMessage+i]);
			}
		}

		for (std::list<REffectInstanceSceneNode*>::iterator itr = effects.begin(); itr != effects.end();)
		{
			REffectInstanceSceneNode* pEffectInstance = *itr;
			REffectInstanceSceneNode::ControlEffectSceneNodeLife((REffectSceneNode**)&pEffectInstance);
			if (NULL == pEffectInstance)
			{
				itr = effects.erase(itr);
				//mlog("delete effect instant\n");
			}
			else
			{
				++itr;
			}
		}
	}

private:
	REffectInstanceSceneNode* pEffectInstanceSceneNode;
	DWORD elapsed;
	bool bInit;
	std::list< REffectInstanceSceneNode* > effects;
};

class TestAddSceneNode : public DevTestInterface
{
	RSceneNode* pSceneNode;
	virtual void init()
	{
		pSceneNode = new RActor();
		bool bRet = ((RActor*)pSceneNode)->Create("Monster/goblin_mage/goblin_Mage.elu");
		_ASSERT(bRet);

		((RActor*)pSceneNode)->Stop();

		//RSceneNodeUpdateVisitor visitor;
		//pSceneNode->Traverse(&visitor);

		REngine::GetSceneManager().AddSceneNode(pSceneNode);
	}

	virtual void halt()
	{
		delete pSceneNode;
	}

	virtual void run()
	{
		//m_pControlMediator_->GetHelperRender()->RegisterToInstantRenderSceneNode(pSceneNode);
	}
};

#define CURRENT_TEST TestAddSceneNode

//class _t : public DevTestInterface
//{
//	virtual void init()
//	{
//
//	}
//
//	virtual void halt()
//	{
//
//	}
//
//	virtual void run()
//	{
//
//	}
//};

DevTestInterface* g_current_test = NULL;
#define INIT_TEST {g_current_test = new CURRENT_TEST(); g_current_test->m_pControlMediator_ = m_pControlMediator_; g_current_test->init();}
#define HALT_TEST if(g_current_test) {g_current_test->halt(); delete g_current_test; g_current_test = NULL; }
#define RUN_TEST if(g_current_test) g_current_test->run();

MAP_EDIT_TYPE CWorkSpaceRzDev::OnOpenFile(const char* _pSzFileName)
{
	INIT_TEST;

	//m_pControlMediator_->GetHelperResource()->SetMediatorAfterSceneLoaded();
	//rs3::RToolSceneManager* pSceneManager = m_pControlMediator_->GetToolSceneManager();
	//if ( !pSceneManager->LoadRootSceneFromXML("..\\..\\Data\\Zone\\Rengot_Village\\Rengot_Village.zone.xml", false) )
	//{
	//	pSceneManager->AddErrorMessage(".scene.xml 파일 읽기 실패");
	//	//AfxMessageBox(".zone.xml 파일 읽기 실패");
	//	m_pControlMediator_->m_mapInfo.m_eMapEditType = MAP_EDIT_TYPE_ERROR;
	//	return m_pControlMediator_->m_mapInfo.m_eMapEditType;
	//}
	//m_pControlMediator_->BroadCastMessage( VIEW_MESSAGE::SCENE_LOADED );

	// init mediator
	m_pControlMediator_->GetHelperResource()->SetMediatorAfterSceneLoaded();
	return MAP_EDIT_TYPE_SCENE;
}

std::string CWorkSpaceRzDev::OnSaveFile(const char* _pSzFileName, bool bSaveAs)
{
	return "";
}

void CWorkSpaceRzDev::OnResetResourceAfterSceneCleared(bool bOnDestroy)
{
	HALT_TEST
}

void CWorkSpaceRzDev::OnBetweenCoreToolUpdateAndRS3Render()
{
	RUN_TEST
}

void CWorkSpaceRzDev::OnWBDevAction()
{
	//using namespace rs3;
	//using namespace physx;

	//CRollupPhysXRagDoll* pRagDollRollup = m_pControlMediator_->GetViewPtr<CRollupPhysXRagDoll>();
	//if (pRagDollRollup->IsSimulationState())
	//	return;

	//bool bUp = GetAsyncKeyState(VK_UP) ? true : false;
	//bool bDown = GetAsyncKeyState(VK_DOWN) ? true : false;
	//bool bLeft = GetAsyncKeyState(VK_LEFT) ? true : false;
	//bool bRight = GetAsyncKeyState(VK_RIGHT) ? true : false;

	//if(bUp||bDown||bLeft||bRight)
	//{
	//	MAP_LOGIC_OBJ::iterator itr = m_mapLogicObject.find("girl");
	//	if (itr != m_mapLogicObject.end())
	//	{
	//		RActor* pActor = itr->second->GetActor();
	//		RPhysXActorController* pController = (RPhysXActorController*)(pActor->GetFirstControllerByName(RSNCTRL_PRERENDER, "TOOL_RAGDOLL"));
	//		if (pController)
	//		{
	//			RMatrix mat = pActor->GetLocalTransform();
	//			if (bUp)
	//			{
	//				mat._43 += 10.f;
	//			}
	//			else if (bDown)
	//			{
	//				mat._43 -= 10.f;
	//			}
	//			else if (bLeft)
	//			{
	//				mat._41 -= 10.f;
	//			}
	//			else if (bRight)
	//			{
	//				mat._41 += 10.f;
	//			}

	//			pController->SetKinematicModeTransform(mat, true);
	//		}
	//		else
	//		{
	//			RActor* pActor = itr->second->GetActor();
	//			RVector3 vPos = pActor->GetWorldPosition();
	//			if (bUp)
	//			{
	//				vPos.z += 10.f;
	//			}
	//			else if (bDown)
	//			{
	//				vPos.z -= 10.f;
	//			}
	//			else if (bLeft)
	//			{
	//				vPos.x -= 10.f;
	//			}
	//			else if (bRight)
	//			{
	//				vPos.x += 10.f;
	//			}
	//			pActor->SetPosition(vPos);
	//			pActor->UpdateTransform();
	//		}
	//	}
	//}

	//static bool bClick = false;
	//if (GetAsyncKeyState('T'))
	//{
	//	MAP_LOGIC_OBJ::iterator itr = m_mapLogicObject.find("girl");
	//	if (itr != m_mapLogicObject.end() && bClick == false)
	//	{
	//		RActor* pActor = itr->second->GetActor();
	//		RSceneNodeController* pController = pActor->GetFirstControllerByName(RSNCTRL_PRERENDER, "TOOL_RAGDOLL");
	//		if (pController)
	//		{
	//			pActor->RemoveController(pController);
	//			delete pController;
	//			pActor->Stop();
	//			/*pActor->SetAnimation("1hb_idle");*/
	//		}
	//		else
	//		{
	//			// add contoller
	//			RPhysXActorController* pPhxController = new RPhysXActorController(MPhysX::GetInstance()->GetNxScene(),RSNCTRL_PRERENDER, "TOOL_RAGDOLL");
	//			pActor->AddController(pPhxController);

	//			// build controller resource
	//			RPhysXRagDollBuilder ragdollBuilder;
	//			pPhxController->BuildNxObject(&ragdollBuilder);
	//			pPhxController->SetEnableKinematicMode( RSkeleton::GetBipName(RBIPID_SPINE1) );
	//			MXmlElement* pRagDollContextElement = MPhysX::GetInstance()->GetPxInfoXML<MPxObjRagDoll>(m_strLastLoadedFile, "default");
	//			if (pRagDollContextElement)
	//			{
	//				RPhysXRagDollFactor factor;
	//				factor.LoadRagDollFactorFromContext(pRagDollContextElement);
	//				factor.ChangeRagDollControllerFromFactor(pPhxController);
	//			}

	//			// 초기 Kinematic Transform
	//			RMatrix mat = pActor->GetLocalTransform();
	//			pPhxController->SetKinematicModeTransform(mat, true);
	//		}
	//	}
	//	bClick = true;
	//}
	//else
	//{
	//	bClick = false;
	//}
}

void CWorkSpaceRzDev::OnWBHelperRender()
{
	//using namespace rs3;

	//MAP_LOGIC_OBJ::iterator itr = m_mapLogicObject.find("me");
	//if (itr == m_mapLogicObject.end())
	//	return;

	////RActor* pActor = itr->second->GetActor();
	////RFont* pFont = CActionHelper::GetReference().GetRFont();

	////// 디버깅 렌더링
	////REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->SetCurrentRenderingTextFont(pFont);
	////vector<RActorNode*>& rActorNodes = pActor->GetActorNodes();
	////size_t nMax = rActorNodes.size();
	////for (size_t i = 0; i < nMax; ++i)
	////{
	////	if (rActorNodes[i]->QueryAttribute(RSNA_NO_DEBUG_RENDERING) == false)
	////	{
	////		REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugAxis(rActorNodes[i]->GetWorldTransform(), 4.f, 0.01f);
	////		REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugText(rActorNodes[i]->GetWorldPosition(), rActorNodes[i]->GetNodeName().c_str(), 0.01f);
	////	}
	////}
}
