#pragma once

#include "RPhysXActorController.h"

//////////////////////////////////////////////////////////////////////////
//
// PhysXSceneNode 'Controller' Builder
//
//////////////////////////////////////////////////////////////////////////

namespace rs3 {

class RPhysXActorControllerBuilder
{
public:
	RPhysXActorControllerBuilder(){}
	virtual ~RPhysXActorControllerBuilder(){}


	//////////////////////////////////////////////////////////////////////////
	//
	// Builder Interface
	//
	//////////////////////////////////////////////////////////////////////////

	virtual bool		Build( RPhysXActorController* pPhysXController ) = 0;


protected:

	//////////////////////////////////////////////////////////////////////////
	//
	// RPhysXActorControllerBuilder 를 상속받은 객체들은 부모를 통해서만
	// RPhysXActorController에 접근할 수 있다.
	//
	//////////////////////////////////////////////////////////////////////////

	inline RActor*		GetActor(RPhysXActorController* pPhysXController);
	inline NxScene*		GetNxScene(RPhysXActorController* pPhysXController);


	// 리소스 관련
	inline void			AddNxActorResource( RPhysXActorController* pPhysXController, int nActorID, NxActor* pNxActor);
	inline void			AddNxJointResource( RPhysXActorController* pPhysXController, int nJointID, NxJoint* pNxJoint );

	// 업데이트 관련
	inline void			PushBackNxUpdateInfo( RPhysXActorController* pPhysXController, const std::string& _rActorNodeName, NxActor* pNxActor, NxJoint* pNxJoint, DWORD dwAttr, const char* pSzLinkedNodeName = NULL, RPhysXActorNodeCustomController* pCustomController = NULL);
	inline void			RefreshNxUpdateInfoMap(RPhysXActorController* pPhysXController){ pPhysXController->RefreshUpdateInfoMap(); }
	inline void			SetHierarchyRootInvTransform(RPhysXActorController* pPhysXController, const RMatrix& _rMat ){ pPhysXController->m_matHierarchyRootInv = _rMat; }
};
//----------------------------------------------------------------------------------------------------
inline RActor* RPhysXActorControllerBuilder::GetActor(RPhysXActorController* pPhysXController)
{
	return pPhysXController->m_pActor;
}
//----------------------------------------------------------------------------------------------------
inline NxScene* RPhysXActorControllerBuilder::GetNxScene(RPhysXActorController* pPhysXController)
{
	return pPhysXController->m_pNxScene;
}
//----------------------------------------------------------------------------------------------------
inline void RPhysXActorControllerBuilder::AddNxActorResource( RPhysXActorController* pPhysXController, int nActorID, NxActor* pNxActor)
{
	_ASSERT(pPhysXController->m_mapNxActors.find(nActorID) == pPhysXController->m_mapNxActors.end());
	pPhysXController->m_mapNxActors.insert(RPhysXActorController::NX_ACTOR_RES_MAP::value_type( nActorID, pNxActor ) );
}
//----------------------------------------------------------------------------------------------------
inline void RPhysXActorControllerBuilder::AddNxJointResource( RPhysXActorController* pPhysXController, int nJointID, NxJoint* pNxJoint )
{
	_ASSERT(pPhysXController->m_mapNxJoints.find(nJointID) == pPhysXController->m_mapNxJoints.end());
	pPhysXController->m_mapNxJoints.insert(RPhysXActorController::NX_JOINT_RES_MAP::value_type( nJointID, pNxJoint ) );
}
//----------------------------------------------------------------------------------------------------
inline void RPhysXActorControllerBuilder::PushBackNxUpdateInfo( RPhysXActorController* pPhysXController, const std::string& _rActorNodeName, NxActor* pNxActor, NxJoint* pNxJoint, DWORD dwAttr, const char* pSzLinkedNodeName, RPhysXActorNodeCustomController* pCustomController)
{
	void* pCustomData = NULL;

	if (pSzLinkedNodeName)
	{
		_ASSERT(pCustomController == NULL);
		RPhysXActorController::SNxUpdateLinkedData* pLinkedNodeData = new RPhysXActorController::SNxUpdateLinkedData;
		pLinkedNodeData->m_strLinkedNodeName = pSzLinkedNodeName;
		pCustomData = (void*)pLinkedNodeData;
	}
	else if (pCustomController)
	{
		pCustomData = (void*)pCustomController;
	}

	RPhysXActorController::SNxUpdateInfo * pNxUpdateInfo = new RPhysXActorController::SNxUpdateInfo(_rActorNodeName, pNxActor, pNxJoint, dwAttr, NULL, (void*)pCustomData);
	pPhysXController->m_vecNxUpdateInfo.push_back( pNxUpdateInfo );
}
//----------------------------------------------------------------------------------------------------
}
