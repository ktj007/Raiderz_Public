#include "stdafx.h"
#include "WS_PhysX.h"

#include "VR_PhysX.h"
#include "VR_EtcMapView.h"
#include "VR_EtcCollision.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperResource.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_ControlMediatorHelperRender.h"

#include "RRenderHelper.h"
#include "M_ToolSceneManager.h"
#include "M_ToolTerrain.h"
#include "M_Visitors.h"

#include "M_PhysXObject.h"
#include "M_CollisionTree.h"

#include "MPhysX.h"
#include "MPxObjStatic.h"
#include "MPhysX4R3.h"

void CWorkSpacePhysX::InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction)
{
	// 물리
	int nRagDollTabIdx = _rUiSetter.AddTab("물리", WORKBENCH_STATE::ETC_DEV);
	_rUiSetter.AddRollup(nRagDollTabIdx, CRollupPhysX::GetID(), true, WORKBENCH_STATE::ETC_DEV, false);
	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_DEV, nRagDollTabIdx, CRollupPhysX::GetID());

	// 기타
	int nEtcIdx = _rUiSetter.AddTab("기타", WORKBENCH_STATE::ETC_MAP_VIEW);
	_rUiSetter.AddRollup(nEtcIdx, CRollupEtcMapView::GetID(), true, WORKBENCH_STATE::ETC_MAP_VIEW, false);
	_rUiSetter.AddRollup(nEtcIdx, CRollupEtcCollision::GetID(), true, WORKBENCH_STATE::ETC_MAP_VIEW, false);
	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_MAP_VIEW, nEtcIdx, CRollupEtcMapView::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_MAP_VIEW, nEtcIdx, CRollupEtcCollision::GetID());

	// 툴바
	_rUiSetter.m_sToolBarButton.m_bAxis = false;
	_rUiSetter.m_sToolBarButton.m_bWeather = false;
	_rUiSetter.m_sToolBarButton.m_bLightMap = false;
	_rUiSetter.m_sToolBarButton.m_bOnTerrain = false;
	_rUiSetter.m_sToolBarButton.m_bLock = false;
	_rUiSetter.m_sToolBarButton.m_bClip = false;

	_rUiSetter.SetDefaultWB(WORKBENCH_STATE::ETC_DEV);

	InitMPhysX();
}

void CWorkSpacePhysX::HaltWorkSpace( CWorkSpaceHelpersAndActions& _rHelperAndAction )
{
	HaltMPhysX();
}

MAP_EDIT_TYPE CWorkSpacePhysX::OnOpenFile(const char* _pSzFileName)
{
	// load scene and set
	rs3::RToolSceneManager* pSceneMgr = m_pControlMediator_->GetToolSceneManager();
	if (false == pSceneMgr->LoadEditableSceneXML("physx_plane.scene.xml"))
		return MAP_EDIT_TYPE_ERROR;

	// set scene object
	m_pControlMediator_->BroadCastMessage( VIEW_MESSAGE::SCENE_LOADED );
	m_pControlMediator_->GetHelperResource()->SetMediatorAfterSceneLoaded();

	// set logic object, 디폴트 라이트에 대한 보기를 끈다.
	std::string strDefaultLight("DefaultLight");
	LOGIC_OBJECTLIST& objList = m_pControlMediator_->GetToolLogicObjectMgr()->GetNoneEntityObjectList();
	for (LOGIC_OBJECTLIST::iterator itr = objList.begin(); itr != objList.end(); ++itr)
	{
		CLogicObject* pLogicObject = *itr;
		pLogicObject->SetEditable(false);
		if (pLogicObject->GetObjectName() == strDefaultLight)
		{
			m_pControlMediator_->GetHelperAction()->SetVisible(pLogicObject, false);
			break;
		}
	}
	m_pControlMediator_->BroadCastMessage( VIEW_MESSAGE::LOGIC_LOADED );

	//////////////////////////////////////////////////////////////////////////
	//
	// 커스텀한 설정들이 있으므로 VIEW_MESSAGE::LOGIC_LOADED 후에 호출해주는 것들
	//
	//////////////////////////////////////////////////////////////////////////

	// physx_plane 안보이게 설정
	class CPhysXSceneSetter : public rs3::RSceneNodeVisitor
	{
	public:
		CPhysXSceneSetter() : m_bMustDoIt(false){}
		virtual ~CPhysXSceneSetter(){}

		virtual bool OnPreOrderVisit( rs3::RSceneNode* pSceneNode )
		{
			if (m_bMustDoIt || pSceneNode->GetNodeName().find("physx_plane") != std::string::npos)
			{
				m_bMustDoIt = true;
				pSceneNode->SetVisibility(0.f);
			}
			return true;
		}
		virtual void OnPostOrderVisit( rs3::RSceneNode* pSceneNode )
		{
			m_bMustDoIt = false;
		}
		bool m_bMustDoIt;

	} physxSceneSetter;
	pSceneMgr->GetRootSceneNode().Traverse(&physxSceneSetter);

	// 카메라 피봇을 위한 더미 로직 오브젝트 설정
	class CCenterDummyLogicObject : public CLogicObject
	{
	public:
		CCenterDummyLogicObject(){ m_strObjectName = "Camera Pivot (dummy object)"; m_pStrLogicObjectName_ = &m_strObjectName; }
		virtual ~CCenterDummyLogicObject(){}

	protected:
		virtual void OnRender(){}
		virtual CLogicObject* OnPickForSelection_(float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to){ return NULL; }
		virtual void OnPickForSelection_(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum){}
		virtual void OnSelected_(bool bSelected){}
		virtual bool OnGetPos_(rs3::RVector& _refVecPos) const { _refVecPos = rs3::RVector::ZERO; return true; }
		std::string m_strObjectName;
	};
	CCenterDummyLogicObject* pCenterDummyLogicObject = new CCenterDummyLogicObject;
	m_pControlMediator_->GetToolLogicObjectMgr()->AddToManager(pCenterDummyLogicObject);
	pCenterDummyLogicObject->SetEditable(true);
	m_pControlMediator_->GetHelperAction()->AddSelectection(pCenterDummyLogicObject, true);

	// init object list and load static object
	m_pControlMediator_->GetViewPtr<CRollupPhysX>()->LoadPxTabList(CRollupPhysX::E_MONSTER);
	InitStaticObject();

	// init workbench and camera
	rs3::RToolCamera* pToolCamera = m_pControlMediator_->GetToolCamera();
	if (pToolCamera)
	{
		m_dwOldCameraColor = pToolCamera->GetClearColor();
		pToolCamera->SetClearColor(D3DCOLOR_ARGB(255, 128, 128, 128));
		pToolCamera->SetPosAndTarget(rs3::RVector(0, -700.f, 100.f), rs3::RVector(0.f, 0.f, 0.f), true);
	}
	m_pControlMediator_->GetHelperRender()->SetVisibleWorldCenterAxis(false);
	return MAP_EDIT_TYPE_LOGIC;
}

std::string CWorkSpacePhysX::OnSaveFile(const char* _pSzFileName, bool bSaveAs)
{
	return "";
}

void CWorkSpacePhysX::OnResetResourceAfterSceneCleared(bool bOnDestroy)
{
	// halt physx object
	UnLoadPxObject();

	// halt static object
	HaltStaticObject();

	// restore workspace setting
	if (m_pControlMediator_->GetToolCamera())
		m_pControlMediator_->GetToolCamera()->SetClearColor(m_dwOldCameraColor);
	m_pControlMediator_->GetHelperRender()->SetVisibleWorldCenterAxis(true);
}

void CWorkSpacePhysX::InitMPhysX()
{
	physx::MPhysX* pPhysXSingletone = physx::MPhysX::GetInstance();
	if ( NULL == pPhysXSingletone )
	{
		pPhysXSingletone = new physx::MPhysX;
		pPhysXSingletone->Init( 10.f,"localhost");
		pPhysXSingletone->SetSceneGravity( (-1400.f) * 1.5f );
	}
}

void CWorkSpacePhysX::HaltMPhysX()
{
	physx::MPhysX* pPhysXSingletone = physx::MPhysX::GetInstance();
	if ( NULL != pPhysXSingletone )
	{
		pPhysXSingletone->Destroy();
		delete pPhysXSingletone;
	}
}

void CWorkSpacePhysX::OnBetweenCoreToolUpdateAndRS3Render()
{
	// 게임과 비슷하도록 elapsed 조정
	float fTime = m_pControlMediator_->GetWBTimerTic() * 0.001f * 0.6f;

	// Update MPhysX
	physx::MPhysX* pPhysX =  physx::MPhysX::GetInstance();
	if (pPhysX)
		pPhysX->Update(fTime);

	// Update PhysX Object
	if (m_pCurrentPxObject)
		m_pCurrentPxObject->Update(m_pControlMediator_, fTime);
}

void CWorkSpacePhysX::LoadPxObject( const char* _pSzMeshName )
{
	if (NULL != m_pCurrentPxObject)
		delete m_pCurrentPxObject;

	m_pCurrentPxObject = new CPhysXObject(_pSzMeshName);
	if (false == m_pCurrentPxObject->IsActorLoaded())
	{
		delete m_pCurrentPxObject;
		m_pCurrentPxObject = NULL;

		::MessageBox(NULL,"모델 로딩에 실패했습니다.","error",MB_OK);
	}
}

void CWorkSpacePhysX::OnWBDevAction()
{
}

void CWorkSpacePhysX::OnWBHelperRender()
{
	using namespace rs3;

	RRenderHelper::SetRenderState();

	RMatrix matCenter;

	matCenter.MakeIdentity();
	matCenter.SetTranslation(RVector( -500.f, -500.f, 0.f ));
	RRenderHelper::RenderGrid(matCenter, 10.f, 100, 100, 0xff898989);

	matCenter.MakeIdentity();
	matCenter.SetTranslation(RVector( -2000.f, -2000.f, 0.f ));
	RRenderHelper::RenderGrid(matCenter, 100.f, 40, 40, 0xff8f8f8f);

	RRenderHelper::RenderCenter(RMatrix::IDENTITY, 4000.f, 4000.f, 0xff88ff88);

	RRenderHelper::EndRenderState();
}

CWorkSpacePhysX::CWorkSpacePhysX()
{
	m_pCurrentPxObject = NULL;
	m_pCollisionTree = NULL;
	m_pPhysXStatic = NULL;
}

CWorkSpacePhysX::~CWorkSpacePhysX()
{
	_ASSERT(NULL == m_pCurrentPxObject);
	_ASSERT(NULL == m_pCollisionTree);
	_ASSERT(NULL == m_pPhysXStatic);
}

void CWorkSpacePhysX::InitStaticObject()
{
	// check whether to init MPhysX 
	_ASSERT(NULL != physx::MPhysX::GetInstance());

	// cl2 load
	_ASSERT(NULL == m_pCollisionTree);
	m_pCollisionTree = new CCollisionTree;
	bool bRet = m_pCollisionTree->Load("physx_plane.cl2");
	m_pControlMediator_->GetViewPtr<CRollupEtcCollision>()->SetCollisionFileName("physx_plane.cl2");
	_ASSERT(bRet);

	// static actor builder
	RPxObjStaticSubActorBuilder rs3MeshBuilder;

	// static actor는 Terrain과 cl2로부터 생성
	rs3::RCollisionTree* pCl2 = m_pCollisionTree->GetCollisionTree();
	rs3::RTerrain* pTerrain = m_pControlMediator_->GetToolSceneManager()->GetCurrentTerrain();

	// 지형으로부터 만들어지는 배경 static actor를 위한 설정
	if (NULL != pTerrain)
	{
		_ASSERT( (rs3::RRS_FILLED == pTerrain->GetState()) || (rs3::RRS_LOADED == pTerrain->GetState()) );

		// 지형분활 수 및 사이즈 계산
		int nDivisionCount = 64;
		float fTerrainWidthSize = pTerrain->GetWidthSize();
		float fXCount = fTerrainWidthSize / pTerrain->GetXScale();
		float fYCount = fTerrainWidthSize / pTerrain->GetYScale();
		float fMinCount = fXCount;
		if(fXCount > fYCount)
			fMinCount = fYCount;

		if(nDivisionCount > fMinCount)
		{
			nDivisionCount = (int)fMinCount;
		}

		// 섹터 크기를 같이 정해준다.
		rs3MeshBuilder.SetTerrain( pTerrain, nDivisionCount );
	}

	// collision tree로부터 만들어지는 static actor를 위한 설정
	if (NULL != pCl2)
	{
		bool bFromCollisionTree = false;
		const rs3::RCollisionTreeNode* pCollisionRoot = pCl2->GetRootNode();
		if (pCollisionRoot)
		{
			if (pCollisionRoot->GetLeftNode())
				bFromCollisionTree = true;

			if (pCollisionRoot->GetRightNode())
				bFromCollisionTree = true;
		}

		if (bFromCollisionTree)
		{
			// 섹터 크기를 같이 정해준다.
			rs3MeshBuilder.SetCollisionTreeRootNode(pCollisionRoot, 10000.f);
		}
	}

	// static object
	_ASSERT(NULL == m_pPhysXStatic);
	m_pPhysXStatic = physx::MPhysX::GetInstance()->CreatePxObject<physx::MPxObjStatic>();
	m_pPhysXStatic->AddStaticSubActorsFromBuilder(rs3MeshBuilder);

	// 현재 위치에서 지연생성을 하면 랙에 쩔수 있으니까 한번 활성화 해준다.
	MBox vCurrentBox( - 5000.f, + 5000.f );
	std::vector< const MBox* > vList;
	vList.push_back(&vCurrentBox);
	m_pPhysXStatic->ActivateStaticSubActorsByAreaList(vList);
}

void CWorkSpacePhysX::HaltStaticObject()
{
	if (m_pCollisionTree)
	{
		m_pCollisionTree->Destroy();
		delete m_pCollisionTree;
		m_pCollisionTree = NULL;

		m_pControlMediator_->GetViewPtr<CRollupEtcCollision>()->SetCollisionFileName("");
	}

	if (m_pPhysXStatic)
	{
		physx::MPhysX::GetInstance()->RemovePxObject(m_pPhysXStatic);
		m_pPhysXStatic = NULL;
	}
}

void CWorkSpacePhysX::StartBPart( int _nIdx )
{
	if (m_pCurrentPxObject)
		m_pCurrentPxObject->StartBPart(_nIdx);
}

void CWorkSpacePhysX::UnLoadPxObject()
{
	if (NULL != m_pCurrentPxObject)
	{
		delete m_pCurrentPxObject;
		m_pCurrentPxObject = NULL;
	}
}
