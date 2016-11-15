#pragma once

struct SPhysXContext
{
};

//// simulation context
//class CSimulationContext;
//
//// CRollupPhysXRagDoll 대화 상자입니다.
//
//	bool					IsSimulationState(){ return (m_pCurrentSimulationContext!=NULL); }
//	void					RunSimulation();
//
//	void					AddSimulationContext(const std::string& _rContexName, CSimulationContext* _pContext);
//	void					ClearSimulationContext();
//
//	// 대화 상자 데이터입니다.
//	enum { IDD = IDD_VR_PHYSX_RAGDOLL };
//
//private:
//	CSimulationContext*		m_pCurrentSimulationContext;
//	typedef					std::map< std::string, CSimulationContext*> MAP_SCONTEXT;
//	MAP_SCONTEXT			m_mapSimulationContext;
//
//	void					SetEnableSimulationUI(bool bEnable);
//
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
//
//	virtual void OnCancel(){}
//	virtual void OnOK(){}
//
//	DECLARE_MESSAGE_MAP()
//public:
//	afx_msg void OnBnClickedButtonPhxRun();
//	afx_msg void OnBnClickedButtonPhxReload();
//	afx_msg void OnLbnDblclkListPhxSimulation();
//	afx_msg void OnBnClickedButtonPhxStop();
//
//private:
//	CListBox		m_ctlListSimulation_;
//	physx::MPhysX*	m_pPhx;
//};
//
//
////////////////////////////////////////////////////////////////////////////
////
//// Simulation Action And Context
////
////////////////////////////////////////////////////////////////////////////
//
//class CSimulationAction
//{
//public:
//	CSimulationAction() : m_fBlockTime(0) {}
//	virtual					~CSimulationAction(){}
//
//	virtual void			RunAction() = 0;
//	virtual void			StopAction() = 0;
//
//	float					GetBlockTime()			{ return m_fBlockTime; }
//	void					SetBlockTime(float _f)	{ m_fBlockTime = _f; }
//
//protected:
//	float					m_fBlockTime;
//};
//
//class CSimulationContext
//{
//public:
//	CSimulationContext() : m_fContexElapsed(-1.f), m_nCurrentIndex(0) {}
//	~CSimulationContext();
//
//	bool Run();
//	void AddAct(CSimulationAction* pAct){ m_deqSA.push_back(pAct); }
//	void ResetToInit();
//
//private:
//	float					m_fContexElapsed;
//	CSimulationAction*		m_pCurrentSA;
//	typedef					std::deque<CSimulationAction*> DEQ_SA;
//	DEQ_SA					m_deqSA;
//	size_t					m_nCurrentIndex;
//};

//// CRollupPhysXRagDoll 메시지 처리기입니다.
//
//void CRollupPhysXRagDoll::OnBnClickedButtonPhxRun()
//{
//	int i = m_ctlListSimulation_.GetCurSel();
//	if (i < 0)
//	{
//		AfxMessageBox("리스트에서 올바른 시뮬레이션을 선택하셔야합니다.");
//		return;
//	}
//
//	CString strSimulation;
//	m_ctlListSimulation_.GetText(i, strSimulation);
//	MAP_SCONTEXT::iterator itr = m_mapSimulationContext.find((LPCTSTR)strSimulation);
//	if (itr != m_mapSimulationContext.end())
//	{
//		m_pCurrentSimulationContext = itr->second;
//		SetEnableSimulationUI(false);
//	}
//}
//
//void CRollupPhysXRagDoll::OnBnClickedButtonPhxReload()
//{
//	CWorkSpacePhysX* pWSPhx = m_pControlMediator_->GetCurrentWorkSpaceAs<CWorkSpacePhysX>();
//	if (pWSPhx)
//	{
//		pWSPhx->ReloadMPhxContext();
//	}
//}
//
//void CRollupPhysXRagDoll::RunSimulation()
//{
//	_ASSERT(m_pCurrentSimulationContext != NULL);
//	if (!m_pCurrentSimulationContext->Run())
//	{
//		m_pCurrentSimulationContext = NULL;
//		SetEnableSimulationUI(true);
//	}
//}
//
//void CRollupPhysXRagDoll::SetEnableSimulationUI( bool bEnable )
//{
//	if (bEnable)
//	{
//		CWnd::GetDlgItem(IDC_BUTTON_PHX_RUN)->EnableWindow(TRUE);
//		CWnd::GetDlgItem(IDC_BUTTON_PHX_RELOAD)->EnableWindow(TRUE);
//		CWnd::GetDlgItem(IDC_LIST_PHX_SIMULATION)->EnableWindow(TRUE);
//		CWnd::GetDlgItem(IDC_BUTTON_PHX_STOP)->EnableWindow(FALSE);
//	}
//	else
//	{
//		CWnd::GetDlgItem(IDC_BUTTON_PHX_RUN)->EnableWindow(FALSE);
//		CWnd::GetDlgItem(IDC_BUTTON_PHX_RELOAD)->EnableWindow(FALSE);
//		CWnd::GetDlgItem(IDC_LIST_PHX_SIMULATION)->EnableWindow(FALSE);
//		CWnd::GetDlgItem(IDC_BUTTON_PHX_STOP)->EnableWindow(TRUE);
//	}
//}
//
//void CRollupPhysXRagDoll::AddSimulationContext( const std::string& _rContexName, CSimulationContext* _pContext )
//{
//	_ASSERT(m_mapSimulationContext.find(_rContexName) == m_mapSimulationContext.end());
//	m_mapSimulationContext.insert( MAP_SCONTEXT::value_type(_rContexName, _pContext) );
//
//	m_ctlListSimulation_.AddString(_rContexName.c_str());
//}
//
//void CRollupPhysXRagDoll::ClearSimulationContext()
//{
//	for (MAP_SCONTEXT::iterator itr = m_mapSimulationContext.begin(); itr != m_mapSimulationContext.end(); ++itr)
//	{
//		delete itr->second;
//	}
//	m_mapSimulationContext.clear();
//}
//
//CSimulationContext::~CSimulationContext()
//{
//	for (DEQ_SA::iterator itr = m_deqSA.begin(); itr != m_deqSA.end(); ++itr)
//	{
//		delete *itr;
//	}
//}
//
//bool CSimulationContext::Run()
//{
//	if (m_nCurrentIndex == m_deqSA.size())
//		return false;
//
//	if (m_fContexElapsed < 0.f)
//	{
//		m_fContexElapsed = 0;
//		m_pCurrentSA = m_deqSA[m_nCurrentIndex];
//		m_pCurrentSA->RunAction();
//	}
//	else
//	{
//		m_pCurrentSA->RunAction();
//		m_fContexElapsed = m_fContexElapsed + float(rs3::REngine::GetTimer().GetElapsedTime()) * 0.001f;
//		float fDuration = m_pCurrentSA->GetBlockTime();
//		if (fDuration >=0.f && m_fContexElapsed >= fDuration)
//		{
//			m_nCurrentIndex++;
//			m_fContexElapsed = -1.f;
//		}
//	}
//
//	return true;
//}
//
//void CSimulationContext::ResetToInit()
//{
//	m_fContexElapsed = -1.f;
//	m_nCurrentIndex = 0;
//
//	for (DEQ_SA::iterator itr = m_deqSA.begin(); itr != m_deqSA.end(); itr++)
//	{
//		(*itr)->StopAction();
//	}
//}
//
//void CRollupPhysXRagDoll::OnLbnDblclkListPhxSimulation()
//{
//	OnBnClickedButtonPhxRun();
//}
//
//void CRollupPhysXRagDoll::OnBnClickedButtonPhxStop()
//{
//	// reset logic object
//	CWorkSpacePhysX* pWSPhx = m_pControlMediator_->GetCurrentWorkSpaceAs<CWorkSpacePhysX>();
//	if (pWSPhx)
//	{
//		pWSPhx->ResetLogicObjectToInit();
//	}
//
//	// reset simulation context
//	for (MAP_SCONTEXT::iterator itr = m_mapSimulationContext.begin(); itr != m_mapSimulationContext.end(); ++itr)
//	{
//		itr->second->ResetToInit();
//	}
//	m_pCurrentSimulationContext = NULL;
//
//	// reset ui
//	SetEnableSimulationUI(true);
//}
//

///// @file WS_PhysX.h
//#pragma once
//
//#include "WS_WorkSpaceManager.h"
//
//#include "MXml.h"
//#include <deque>
//
//namespace rs3
//{
//	class RResource;
//	class RMeshNode;
//	class RActor;
//}
//
//	typedef						std::deque< rs3::RResource* > DEQ_RES;
//	DEQ_RES						m_deqResources;
//
//	typedef						std::map< std::string, CPhysXLogicObject* > MAP_LOGIC_OBJ;
//	MAP_LOGIC_OBJ				m_mapLogicObject;
//
//	typedef						std::map< std::string, MXmlElement* > MAP_SCENE;
//	MAP_SCENE					m_mapScene;
//	bool						LoadScene(const std::string& _rSceneName);
//
//	std::string					m_strLastLoadedFile;
//};
/////////////////////////////////////////////////////////////////////
//#include "stdafx.h"
//#include "WS_PhysX.h"
//
//#include "C_XMLConfig.h"
//#include "VR_PhysXRagDoll.h"
//#include "VR_EtcMapView.h"
//#include "VL_SceneNodeList.h"
//
//#include "MPhysX.h"
//#include "MPxObjRagDoll.h"
//
//#include "RActor.h"
//#include "RMesh.h"
//#include "RMeshMgr.h"
//#include "RMeshNode.h"
//
//#include "RFont.h"
//#include "RDebugPassRenderer.h"
//
//#include "RPhysXActorController.h"
//#include "RPhysXRagDollBuilder.h"
//
//#include "MPxTransformController.h"
//#include "MPxAttacherTC.h"
//#include "MPxRagDollTC.h"
//#include "MPxAniLookUpTC.h"
//#include "MPxDynamicCollisionTC.h"
//
//#include "C_ControlDoActionImpl.h"
//#include "C_ControlMediatorHelperActionAddon.h"
//#include "C_ControlMediatorHelperRenderAddon.h"
//#include "C_ControlMediatorHelperResourceAddon.h"
//
//
//class CPhysXLogicObject
//{
//public:
//	CPhysXLogicObject(rs3::RActor* pActor, const std::string& _rPhxName,
//		const rs3::RVector3& _rInitPos, const rs3::RVector3& _rInitDir, const std::string& _rInitAni)
//		: m_vInitPos(_rInitPos), m_vInitDir(_rInitDir), m_strInitAni(_rInitAni), m_strPhxName(_rPhxName)
//	{
//		m_PxEntity.m_pOwner = this;
//		m_pActor = pActor;
//
//		using namespace physx;
//		int i=0;
//
//		MPxAttacherTC::SetID(i++);
//		m_vecTC.push_back( new MPxAttacherTC(&m_PxEntity) );
//
//		MPxRagDollTC::SetID(i++);
//		MPxRagDollTC* pRagDollTC = new MPxRagDollTC(&m_PxEntity);
//		pRagDollTC->InitRagDollInfoFromModelName(_rPhxName.substr(0, _rPhxName.length() - 8), NULL);
//		m_vecTC.push_back(pRagDollTC);
//
//		MPxAniLookUpTC::SetID(i++);
//		MPxAniLookUpTC* pAniLookupTC = new MPxAniLookUpTC(&m_PxEntity);
//		pAniLookupTC->SetEnableCollisionTcHooking(true);
//		m_vecTC.push_back(pAniLookupTC);
//
//		MPxDynamicCollisionTC::SetID(i++);
//		m_vecTC.push_back(new MPxDynamicCollisionTC(&m_PxEntity));
//
//		m_vAniDummyPos.Set(0,0, -100000.f);
//	}
//
//	virtual ~CPhysXLogicObject()
//	{
//		for (std::vector<physx::MPxTransformController*>::iterator itr = m_vecTC.begin(); itr != m_vecTC.end(); ++itr)
//		{
//			delete *itr;
//		}
//
//		rs3::RSceneNodeController* pController = m_pActor->GetFirstControllerByName(rs3::RSNCTRL_PRERENDER, "TOOL_RAGDOLL");
//		if (pController)
//		{
//			m_pActor->RemoveController(pController);
//			delete pController;
//		}
//		delete m_pActor;
//	}
//
//	void Update()
//	{
//		m_ActorUpdator.UpdateForRender(m_pActor, m_vAniDummyPos);
//		for (std::vector<physx::MPxTransformController*>::iterator itr = m_vecTC.begin(); itr != m_vecTC.end(); ++itr)
//		{
//			physx::MPxTransformController* pTc = *itr;
//			if (pTc->IsEnabled())
//			{
//				pTc->Update(rs3::REngine::GetTimer().GetElapsedTime() * 0.001f);
//			}
//		}
//	}
//
//	void SetAnimation(const std::string& _rAniName)
//	{
//		std::string strAni(_rAniName);
//		if (!GetTC<physx::MPxAniLookUpTC>()->HookOnAnimationSetting(strAni))
//		{
//			m_pActor->SetAnimation(strAni.c_str());
//		}
//		m_vAniDummyPos = m_pActor->GetWorldPosition();
//	}
//
//	bool IsPlayDone()
//	{
//		return m_pActor->IsPlayDone();
//	}
//
//	void SetTransform(const rs3::RVector3& vPos, const rs3::RVector3& vDir)
//	{
//		m_pActor->SetTransform(vPos, vDir);
//		m_pActor->UpdateTransform();
//		m_vAniDummyPos = vPos;
//	}
//
//	void ResetToInit()
//	{
//		for (std::vector<physx::MPxTransformController*>::iterator itr = m_vecTC.begin(); itr != m_vecTC.end(); ++itr)
//		{
//			(*itr)->Stop();
//		}
//		SetAnimation(m_strInitAni);
//		SetTransform(m_vInitPos, -m_vInitDir);
//
//		m_vAniDummyPos.Set(0,0, -100000.f);
//	}
//
//	void ReloadRagDollContext()
//	{
//		using namespace physx;
//		MPhysX::PX_INFO_XML* pPxInfo = MPhysX::GetInstance()->GetPxInfoXMLAllContext<MPxObjRagDoll>(m_strPhxName);
//		if (pPxInfo)
//		{
//			for (MPhysX::PX_INFO_XML::iterator itr = pPxInfo->begin(); itr != pPxInfo->end(); ++itr)
//			{
//				GetTC<MPxRagDollTC>()->ChangeRagDollFromContext(itr->first.c_str());
//			}
//		}
//	}
//
//	rs3::RActor* GetActor()
//	{
//		return m_pActor;
//	}
//
//	template<typename T>
//	T* GetTC()
//	{
//		return static_cast<T*>(m_vecTC[ T::GetID() ]);
//	}
//
//private:
//	rs3::RActor*		m_pActor;
//
//	rs3::RVector		m_vInitPos;
//	rs3::RVector		m_vInitDir;
//	std::string			m_strInitAni;
//	std::string			m_strPhxName;
//	rs3::RVector		m_vAniDummyPos;
//
//	friend class CPhysXLogicObject;
//	class CPhysXLogicObjectEntity : public physx::MPxTcEntity
//	{
//	public:
//		CPhysXLogicObjectEntity(){}
//		virtual ~CPhysXLogicObjectEntity(){}
//
//		virtual rs3::RActor* GetActor()
//		{
//			return m_pOwner->m_pActor;
//		}
//
//		virtual physx::MPxTransformController* GetTransformController(int nId)
//		{
//			if (nId < 0 || nId >= (int)(m_pOwner->m_vecTC.size()) )
//			{
//				return NULL;
//			}
//
//			return m_pOwner->m_vecTC[nId];
//		}
//
//		virtual bool IsAvailableActor(rs3::RActor* pActor){ return true; }
//
//		virtual void OnDynamicObjectStopped(NxActor* _pNxActor, physx::MPxDynamicObjectEntity* _pDynamicObjEntity){}
//		virtual void OnDynamicObjectCollided(NxActor* _pNxActor, physx::MPxDynamicObjectEntity* _pDynamicObjEntity){}
//
//		CPhysXLogicObject* m_pOwner;
//
//	} m_PxEntity;
//
//	std::vector<physx::MPxTransformController*> m_vecTC;
//
//	class RActorUpdateControl : rs3::RActorController
//	{
//	public:
//		RActorUpdateControl() : RActorController(rs3::RSNCTRL_POSTUPDATE){}
//		void UpdateForRender(rs3::RActor* pActor, const rs3::RVector& _rPos)
//		{
//			using namespace rs3;
//
//			m_pActor = pActor;
//			RActorNode* pActorNode = m_pActor->GetActorNode("dummy_loc");
//			if (NULL != pActorNode)
//			{
//				DWORD nFrame = m_pActor->GetAnimationController().GetFrame();
//
//				RMatrix mat;
//				bool bPlayDone = false;
//				m_pActor->GetAnimationController().GetNextFrameAndState(REngine::GetTimer().GetElapsedTime(), nFrame, bPlayDone);
//				pActorNode->GetLocalTransformAt(nFrame, mat);
//				RVector3 vPos(_rPos);
//				vPos.x -= mat._41;
//				vPos.y -= mat._42;
//				vPos.z -= mat._43;
//
//				RVector3 vDir = m_pActor->GetWorldDirection();
//				m_pActor->SetTransform(vPos, vDir);
//				m_pActor->UpdateTransform();
//			}
//			if (m_pActor->GetUpdateForRenderRefCount() > 0)
//				UpdateActorForRender();
//		}
//
//	} m_ActorUpdator;
//};
//
//MAP_EDIT_TYPE CWorkSpacePhysX::OnOpenFile(const char* _pSzFileName)
//{
//	MXml aXml;
//	if (!aXml.LoadFile(_pSzFileName))
//		return MAP_EDIT_TYPE_ERROR;
//
//	MXmlElement* pRealSpaceElement = aXml.DocHandle().FirstChildElement("ROOT").FirstChildElement("REALSPACE").Element();
//	if (!pRealSpaceElement)
//		return MAP_EDIT_TYPE_ERROR;
//
//	std::string strInitScene;
//	_Attribute(strInitScene, pRealSpaceElement, "init");
//
//	// 씬
//	std::string strSceneName;
//	MXmlElement* pSceneElement = pRealSpaceElement->FirstChildElement("SCENE");
//	while(pSceneElement)
//	{
//		strSceneName.clear();
//		_Attribute(strSceneName, pSceneElement, "name");
//		_ASSERT(m_mapScene.find(strSceneName) == m_mapScene.end());
//
//		m_mapScene.insert( MAP_SCENE::value_type( strSceneName, (MXmlElement*)(pSceneElement->Clone()) ) );
//		pSceneElement = pSceneElement->NextSiblingElement("SCENE");
//	}
//
//	m_strLastLoadedFile = _pSzFileName;
//	if (!LoadScene(strInitScene))
//		AfxMessageBox("초기 씬 로딩 실패");
//
//	// 시뮬레이션
//	std::string strSimulationContext;
//	std::string strSimulationActionType;
//	MXmlElement* pSimulationContextElement = aXml.DocHandle().FirstChildElement("ROOT").FirstChildElement("MPHYSX").FirstChildElement("SIMULATION").FirstChild("CONTEXT").Element();
//	while(pSimulationContextElement)
//	{
//		using namespace rs3;
//
//		strSimulationContext.clear();
//		_Attribute(strSimulationContext, pSimulationContextElement, "name", &aXml);
//		CSimulationContext* pContext = new CSimulationContext;
//		m_pControlMediator_->GetViewPtr<CRollupPhysXRagDoll>()->AddSimulationContext(strSimulationContext, pContext);
//
//		MXmlElement* pActElement = pSimulationContextElement->FirstChildElement("ACT");
//		while (pActElement)
//		{
//			strSimulationActionType.clear();
//			_Attribute(strSimulationActionType, pActElement, "type");
//			if (strSimulationActionType == "move")
//			{
//				class CActionMove : public CSimulationAction
//				{
//				public:
//					CActionMove(){ bInit = false; }
//					virtual ~CActionMove(){}
//
//					void RunAction()
//					{
//						if (bInit)
//							return;
//
//						bInit = true;
//						int nSize = (int)m_vActor.size();
//						for (int i = 0; i < nSize; ++i)
//						{
//							m_vActor[i]->SetTransform( m_vPos[i], -m_vDir[i] );
//							if (!m_vAni[i].empty())
//							{
//								m_vActor[i]->SetAnimation(m_vAni[i].c_str());
//							}
//						}
//					}
//
//					void StopAction()
//					{
//						bInit = false;
//					}
//
//					std::vector<CPhysXLogicObject*> m_vActor;
//					std::vector<RVector3> m_vPos;
//					std::vector<RVector3> m_vDir;
//					std::vector<std::string> m_vAni;
//
//					bool bInit;
//				};
//
//				CActionMove* pActionMove = new CActionMove;
//				pContext->AddAct(pActionMove);
//
//				float fBlockTime = 0.f;
//				_Attribute(&fBlockTime, pActElement, "btime");
//				pActionMove->SetBlockTime(fBlockTime);
//
//				std::string actor;
//				RVector3 v;
//				MXmlElement* pMovable = pActElement->FirstChildElement("MOVABLE");
//				while(pMovable)
//				{
//					actor.clear();
//					_Attribute(actor, pMovable, "name");
//					pActionMove->m_vActor.push_back( m_mapLogicObject[actor] );
//
//					_Attribute(&v.x, pMovable, "x");
//					_Attribute(&v.y, pMovable, "y");
//					_Attribute(&v.z, pMovable, "z");
//					pActionMove->m_vPos.push_back(v);
//
//					_Attribute(&v.x, pMovable, "dir_x");
//					_Attribute(&v.y, pMovable, "dir_y");
//					_Attribute(&v.z, pMovable, "dir_z");
//					pActionMove->m_vDir.push_back(v);
//
//					actor.clear();
//					_Attribute(actor, pMovable, "ani");
//					pActionMove->m_vAni.push_back( actor );
//
//					pMovable = pMovable->NextSiblingElement("MOVABLE");
//				}
//			}
//			else if (strSimulationActionType == "ani")
//			{
//				class CActionAni : public CSimulationAction
//				{
//				public:
//					CActionAni(){ bInit = false; fBlockTime = 0.f; }
//					virtual ~CActionAni(){}
//
//					void RunAction()
//					{
//						if (bInit)
//						{
//							if (m_fBlockTime < 0.f)
//							{
//								fBlockTime = -1.f;
//								if ( (ani.find("ragdoll_") != std::string::npos) ||  pActor->IsPlayDone())
//									m_fBlockTime = 0.f;
//							}
//							return;
//						}
//
//						bInit = true;
//						pActor->SetAnimation(ani.c_str());
//					}
//
//					void StopAction()
//					{
//						bInit = false;
//						if(fBlockTime < 0.f)
//							m_fBlockTime = fBlockTime;
//					}
//
//					CPhysXLogicObject* pActor;
//					std::string ani;
//					bool bInit;
//					float fBlockTime;
//				};
//
//				CActionAni* pActionAni = new CActionAni;
//				pContext->AddAct(pActionAni);
//
//				float fBlockTime = -1.f;
//				_Attribute(&fBlockTime, pActElement, "btime");
//				pActionAni->SetBlockTime(fBlockTime);
//
//				std::string actor;
//				_Attribute(actor, pActElement, "actor");
//				pActionAni->pActor = m_mapLogicObject[actor];
//
//				_Attribute(actor, pActElement, "ani");
//				pActionAni->ani = actor;
//			}
//			else if (strSimulationActionType=="attach")
//			{
//				class CActionAttach : public CSimulationAction
//				{
//				public:
//					CActionAttach(){ bInit = false;}
//					virtual ~CActionAttach(){}
//
//					void RunAction()
//					{
//						if (bInit)
//							return;
//
//						bInit = true;
//						physx::MPxAttacherTC* pTc = pAttachee->GetTC<physx::MPxAttacherTC>();
//						pTc->StartAttacher(pAttacher->GetActor(), strDestBone.c_str(), strSrcBone.c_str(), fInterpolation, RMatrix::IDENTITY);
//					}
//					void StopAction()
//					{
//						bInit = false;
//					}
//					CPhysXLogicObject* pAttacher;
//					CPhysXLogicObject* pAttachee;
//
//					std::string strSrcBone;
//					std::string strDestBone;
//
//					float fInterpolation;
//					bool bInit;
//				};
//
//				CActionAttach* pActionAttacher = new CActionAttach;
//				pContext->AddAct(pActionAttacher);
//
//				float fBlockTime = 0.f;
//				_Attribute(&fBlockTime, pActElement, "btime");
//				pActionAttacher->SetBlockTime(fBlockTime);
//				_Attribute(&pActionAttacher->fInterpolation, pActElement, "interpolation");
//
//				std::string actor;
//				_Attribute(actor, pActElement, "attacher");
//				pActionAttacher->pAttacher = m_mapLogicObject[actor];
//
//				actor.clear();
//				_Attribute(actor, pActElement, "attachee");
//				pActionAttacher->pAttachee = m_mapLogicObject[actor];
//
//				_Attribute(pActionAttacher->strSrcBone, pActElement, "src");
//				_Attribute(pActionAttacher->strDestBone, pActElement, "dest");
//			}
//
//			pActElement = pActElement->NextSiblingElement("ACT");
//		}
//		pSimulationContextElement = pSimulationContextElement->NextSiblingElement("CONTEXT");
//	}
//
//	// MPhysX 정보
//	physx::MPhysX::GetInstance()->ClearPxInfoXML();
//	physx::MPhysX::GetInstance()->CreatePxInfoXML(m_strLastLoadedFile, false);
//
//	// SceneNodeList 창 업데이트
//	m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>()->UpdateTreeControl();
//	return MAP_EDIT_TYPE_LOGIC;
//}
//
//void CWorkSpacePhysX::OnResetResourceAfterSceneCleared(bool bOnDestroy)
//{
//	// 로직 오브젝트 지우기
//	// 이때 액터가 지워진다. 씬매니저에서는 씬매니저 Clear시에 떨어져나간다.
//	for (MAP_LOGIC_OBJ::iterator itr = m_mapLogicObject.begin(); itr != m_mapLogicObject.end(); ++itr)
//	{
//		delete itr->second;
//	}
//	m_mapLogicObject.clear();
//
//	// 리소스 삭제. 인스턴스(액터) 삭제후에 지워준다.
//	while (!m_deqResources.empty())
//	{
//		rs3::RResource* pResource = *(m_deqResources.end() - 1);
//		rs3::REngine::GetSceneManager().ReleaseResource(pResource);
//		m_deqResources.pop_back();
//	}
//
//	// 씬 정보
//	for (MAP_SCENE::iterator itr = m_mapScene.begin(); itr != m_mapScene.end(); ++itr)
//	{
//		delete itr->second;
//	}
//	m_mapScene.clear();
//
//	// 시뮬레이션 정보
//	m_pControlMediator_->GetViewPtr<CRollupPhysXRagDoll>()->ClearSimulationContext();
//}
//
//void CWorkSpacePhysX::OnBetweenCoreToolUpdateAndRS3Render()
//{
//	CRollupPhysXRagDoll* pRagDollRollup = m_pControlMediator_->GetViewPtr<CRollupPhysXRagDoll>();
//	pRagDollRollup->UpdatePhysX();
//
//	if (pRagDollRollup->IsSimulationState())
//	{
//		pRagDollRollup->RunSimulation();
//		for (MAP_LOGIC_OBJ::iterator itr = m_mapLogicObject.begin(); itr != m_mapLogicObject.end(); ++itr)
//		{
//			itr->second->Update();
//		}
//	}
//}
//
//void CWorkSpacePhysX::OnWBDevAction()
//{
//	using namespace rs3;
//	using namespace physx;
//
//	CRollupPhysXRagDoll* pRagDollRollup = m_pControlMediator_->GetViewPtr<CRollupPhysXRagDoll>();
//	if (pRagDollRollup->IsSimulationState())
//		return;
//
//	bool bUp = GetAsyncKeyState(VK_UP) ? true : false;
//	bool bDown = GetAsyncKeyState(VK_DOWN) ? true : false;
//	bool bLeft = GetAsyncKeyState(VK_LEFT) ? true : false;
//	bool bRight = GetAsyncKeyState(VK_RIGHT) ? true : false;
//
//	if(bUp||bDown||bLeft||bRight)
//	{
//		MAP_LOGIC_OBJ::iterator itr = m_mapLogicObject.find("girl");
//		if (itr != m_mapLogicObject.end())
//		{
//			RActor* pActor = itr->second->GetActor();
//			RPhysXActorController* pController = (RPhysXActorController*)(pActor->GetFirstControllerByName(RSNCTRL_PRERENDER, "TOOL_RAGDOLL"));
//			if (pController)
//			{
//				RMatrix mat = pActor->GetLocalTransform();
//				if (bUp)
//				{
//					mat._43 += 10.f;
//				}
//				else if (bDown)
//				{
//					mat._43 -= 10.f;
//				}
//				else if (bLeft)
//				{
//					mat._41 -= 10.f;
//				}
//				else if (bRight)
//				{
//					mat._41 += 10.f;
//				}
//
//				pController->SetKinematicModeTransform(mat, true);
//			}
//			else
//			{
//				RActor* pActor = itr->second->GetActor();
//				RVector3 vPos = pActor->GetWorldPosition();
//				if (bUp)
//				{
//					vPos.z += 10.f;
//				}
//				else if (bDown)
//				{
//					vPos.z -= 10.f;
//				}
//				else if (bLeft)
//				{
//					vPos.x -= 10.f;
//				}
//				else if (bRight)
//				{
//					vPos.x += 10.f;
//				}
//				pActor->SetPosition(vPos);
//				pActor->UpdateTransform();
//			}
//		}
//	}
//
//	static bool bClick = false;
//	if (GetAsyncKeyState('T'))
//	{
//		MAP_LOGIC_OBJ::iterator itr = m_mapLogicObject.find("girl");
//		if (itr != m_mapLogicObject.end() && bClick == false)
//		{
//			RActor* pActor = itr->second->GetActor();
//			RSceneNodeController* pController = pActor->GetFirstControllerByName(RSNCTRL_PRERENDER, "TOOL_RAGDOLL");
//			if (pController)
//			{
//				pActor->RemoveController(pController);
//				delete pController;
//				pActor->Stop();
//				/*pActor->SetAnimation("1hb_idle");*/
//			}
//			else
//			{
//				// add contoller
//				RPhysXActorController* pPhxController = new RPhysXActorController(MPhysX::GetInstance()->GetNxScene(),RSNCTRL_PRERENDER, "TOOL_RAGDOLL");
//				pActor->AddController(pPhxController);
//
//				// build controller resource
//				RPhysXRagDollBuilder ragdollBuilder;
//				pPhxController->BuildNxObject(&ragdollBuilder);
//				pPhxController->SetEnableKinematicMode( RSkeleton::GetBipName(RBIPID_SPINE1) );
//				MXmlElement* pRagDollContextElement = MPhysX::GetInstance()->GetPxInfoXML<MPxObjRagDoll>(m_strLastLoadedFile, "default");
//				if (pRagDollContextElement)
//				{
//					RPhysXRagDollFactor factor;
//					factor.LoadRagDollFactorFromContext(pRagDollContextElement);
//					factor.ChangeRagDollControllerFromFactor(pPhxController);
//				}
//
//				// 초기 Kinematic Transform
//				RMatrix mat = pActor->GetLocalTransform();
//				pPhxController->SetKinematicModeTransform(mat, true);
//			}
//		}
//		bClick = true;
//	}
//	else
//	{
//		bClick = false;
//	}
//}
//
//void CWorkSpacePhysX::OnWBHelperRender()
//{
//	using namespace rs3;
//
//	MAP_LOGIC_OBJ::iterator itr = m_mapLogicObject.find("me");
//	if (itr == m_mapLogicObject.end())
//		return;
//
//	//RActor* pActor = itr->second->GetActor();
//	//RFont* pFont = CActionHelper::GetReference().GetRFont();
//
//	//// 디버깅 렌더링
//	//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->SetCurrentRenderingTextFont(pFont);
//	//vector<RActorNode*>& rActorNodes = pActor->GetActorNodes();
//	//size_t nMax = rActorNodes.size();
//	//for (size_t i = 0; i < nMax; ++i)
//	//{
//	//	if (rActorNodes[i]->QueryAttribute(RSNA_NO_DEBUG_RENDERING) == false)
//	//	{
//	//		REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugAxis(rActorNodes[i]->GetWorldTransform(), 4.f, 0.01f);
//	//		REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugText(rActorNodes[i]->GetWorldPosition(), rActorNodes[i]->GetNodeName().c_str(), 0.01f);
//	//	}
//	//}
//}
//
//bool CWorkSpacePhysX::LoadScene( const std::string& _rSceneName )
//{
//	MAP_SCENE::iterator itr = m_mapScene.find(_rSceneName);
//	if (itr == m_mapScene.end())
//		return false;
//
//	using namespace rs3;
//	using namespace physx;
//
//	// 디버깅 정보
//	class DebugRenderingBoneCullingVisitor : public RSceneNodeVisitor
//	{
//	public:
//		DebugRenderingBoneCullingVisitor(){}
//		~DebugRenderingBoneCullingVisitor(){}
//
//		virtual bool OnPreOrderVisit( RSceneNode* pSceneNode )
//		{
//			const std::string& rNodeName = pSceneNode->GetNodeName();
//			//if (rNodeName.find("Finger") != std::string::npos
//			//	//|| rNodeName.find("Bip") == std::string::npos
//			//	|| rNodeName.find("Twist") != std::string::npos
//			//	)
//			//{
//			//	pSceneNode->AddAttribute(RSNA_NO_DEBUG_RENDERING);
//			//}
//			if (rNodeName.find("Dummy04") != std::string::npos
//				|| rNodeName.find("Dummy03") != std::string::npos
//				|| rNodeName.find("Dummy05") != std::string::npos
//				|| rNodeName.find("Dummy02") != std::string::npos
//				|| rNodeName.find("Dummy01") != std::string::npos
//
//				|| rNodeName.find("Bip01 L UpperArm") != std::string::npos
//				|| rNodeName.find("Bip01 L Forearm") != std::string::npos
//				|| rNodeName.find("Bip01 R UpperArm") != std::string::npos
//				|| rNodeName.find("Bip01 R Forearm") != std::string::npos
//
//				|| rNodeName.find("Bip01 L Calf") != std::string::npos
//				|| rNodeName.find("Bip01 R Calf") != std::string::npos
//				)
//			{
//			}
//			else
//			{
//				pSceneNode->AddAttribute(RSNA_NO_DEBUG_RENDERING);
//			}
//
//			return true;
//		}
//		virtual void OnPostOrderVisit( RSceneNode* pSceneNode ){}
//
//	} debugRenderingVisitor;
//
//	std::string strDataPath(CXMLConfig::GetInstance()->pcDataPath);
//
//	std::string strSceneFileName;
//	MXmlElement* pSceneElement = itr->second;
//	_Attribute(strSceneFileName, pSceneElement, "file");
//	if (MAP_EDIT_TYPE_ERROR == m_pControlMediator_->LoadZone( (strDataPath + strSceneFileName).c_str(), MAP_EDIT_TYPE_LOGIC, false ))
//		return false;
//
//	// 카메라
//	MXmlElement* pCameraElement = pSceneElement->FirstChildElement("CAMERA");
//	if(pCameraElement)
//	{
//		RVector3 vCamPos;
//		RVector3 vCamDir;
//		_Attribute(&vCamPos.x, pCameraElement, "x");
//		_Attribute(&vCamPos.y, pCameraElement, "y");
//		_Attribute(&vCamPos.z, pCameraElement, "z");
//		_Attribute(&vCamDir.x, pCameraElement, "dir_x");
//		_Attribute(&vCamDir.y, pCameraElement, "dir_y");
//		_Attribute(&vCamDir.z, pCameraElement, "dir_z");
//
//		m_pControlMediator_->GetToolCamera()->Set(vCamPos, vCamDir);
//	}
//
//	// 액터 로딩
//	CControlMediatorHelperResource::SExtraInformedActorLoaderParam informedActorParam;
//	MXmlElement* pActorElement = pSceneElement->FirstChildElement("ACTOR");
//	while(pActorElement)
//	{
//		informedActorParam.Clear();
//
//		// 인스턴스 생성
//		RActor* pActor = static_cast< RActor* >( REngine::GetSceneManager().CreateSceneNode( rs3::RSID_ACTOR ) );
//		informedActorParam.m_pActorToLoad = pActor;
//		informedActorParam.m_pInformedXmlElement = pActorElement;
//		informedActorParam.m_bAddToSceneNode = true;
//
//		m_pControlMediator_->GetHelperResource()->LoadExtraInformedActor(informedActorParam, m_deqResources);
//		_ASSERT( m_mapLogicObject.find(informedActorParam.m_strOutActorName) == m_mapLogicObject.end());
//		m_mapLogicObject.insert (
//			MAP_LOGIC_OBJ::value_type (
//			informedActorParam.m_strOutActorName,
//			new CPhysXLogicObject (
//			pActor, m_strLastLoadedFile, informedActorParam.m_vOutPos, informedActorParam.m_vOutDir, informedActorParam.m_strOutAnimationName
//			)
//			)
//			);
//
//		// 디버깅 정보
//		pActor->Traverse(&debugRenderingVisitor);
//
//		// 다음 elu
//		pActorElement = pActorElement->NextSiblingElement("ACTOR");
//	}
//
//	return true;
//}
//
//void CWorkSpacePhysX::ResetLogicObjectToInit()
//{
//	for (MAP_LOGIC_OBJ::iterator itr = m_mapLogicObject.begin(); itr != m_mapLogicObject.end(); ++itr)
//	{
//		itr->second->ResetToInit();
//	}
//}
//
//void CWorkSpacePhysX::ReloadMPhxContext()
//{
//	physx::MPhysX::GetInstance()->CreatePxInfoXML(m_strLastLoadedFile, true);
//	for (MAP_LOGIC_OBJ::iterator itr = m_mapLogicObject.begin(); itr != m_mapLogicObject.end(); ++itr)
//	{
//		itr->second->ReloadRagDollContext();
//	}
//}
