#include "stdafx.h"

#include "MPxObjDynamic.h"

#include "MPhysX.h"
#include "MPhysXShape.h"

#include "NxCooking.h"

namespace physx {

MPxObjDynamic::MPxObjDynamic()
{
	m_pDynamicObjectEntity = NULL;
	m_bActive = false;

	m_bObservingResultOnSimulated = true;
	m_bUsingAreaBox = true;
}

MPxObjDynamic::~MPxObjDynamic()
{
	_ASSERT(m_pDynamicObjectEntity == NULL);
}

bool MPxObjDynamic::AddSubActorFromEntity()
{
	if (NULL == m_pDynamicObjectEntity)
		return false;

	int nShapeCount = m_pDynamicObjectEntity->GetSubActorCount();
	if (nShapeCount < 1)
		return false;

	std::vector<NxVec3> vertices;
	std::vector<NxU32> faces;
	MMatrix matLocal;
	bool bKinematic;

	bool bResult = true;
	std::string strName;
	for (int i = 0; i < nShapeCount; ++i)
	{
		if (m_pDynamicObjectEntity->GetSubActorInfo(i, strName, vertices, faces, matLocal, bKinematic))
		{
			//mlog("---%s\n", strName.c_str());
			MPhysXShape* pShape = m_pPhysX->CreatePxShape(MPhysXShape::PSHT_CONVEX, strName);
			pShape->CreateConvexShape(vertices, faces);

			MSUBACTORHANDLE hHandle = m_NxInfos.Add( NX_INFO(PS_DYNAMIC_CONVEX, matLocal, bKinematic? -1.f : 10.f, pShape) );
			m_pDynamicObjectEntity->OnSubActorAdded(i, hHandle);

			vertices.clear();
			faces.clear();
		}
		else
		{
			bResult = false;
		}
	}

	return bResult;
}

void MPxObjDynamic::ActivateSubActorsAll()
{
	ActivateAllSubActors();
	if (m_pDynamicObjectEntity)
	{
		_ASSERT(m_pPhysX);

		MMatrix matWorld;
		m_pDynamicObjectEntity->OnActivatedSubActorsAll(matWorld);
		DoSetWorldTransform(matWorld);
	}

	DoUpdateTransform();
	m_bActive = true;
}

void MPxObjDynamic::DeActivateSubActorsAll()
{
	DeActiveAllSubActors();
	if (m_pDynamicObjectEntity)
		m_pDynamicObjectEntity->OnDeActivatedSubActorsAll();
	m_bActive = false;
}

void MPxObjDynamic::ApplyForce( const MVector3& _rForce, const MVector3& _rForceWorldPos )
{
	const list<int>& listUsedHandle = m_NxInfos.GetUsedHandleList();
	for (list<int>::const_iterator itr = listUsedHandle.begin(); itr != listUsedHandle.end(); ++itr)
	{
		MSUBACTORHANDLE hnd = *itr;
		NX_INFO& info = m_NxInfos.Get( hnd );
		if (info.m_pActor)
		{
			info.m_pActor->addForceAtPos( NxVec3(_rForce.x, _rForce.y, _rForce.z), NxVec3(_rForceWorldPos.x, _rForceWorldPos.y, _rForceWorldPos.z), NX_VELOCITY_CHANGE );
			//info.m_pActor->addForceAtPos( NxVec3(_rForce.x, _rForce.y, _rForce.z), NxVec3(_rForceWorldPos.x, _rForceWorldPos.y, _rForceWorldPos.z), NX_SMOOTH_VELOCITY_CHANGE );
		}
	}
}

const MBox* MPxObjDynamic::GetAreaBox()
{
	MBox* pBox = NULL;

	// aabb update
	if (m_bActive && m_pDynamicObjectEntity)
	{
		m_aabb = m_pDynamicObjectEntity->GetAreaBox();
		pBox = &m_aabb;
	}

	// 외부에서 area box를 사용하는지 여부
	// 내부 작업을 위해 aabb는 업데이트하지만, 외부에서 사용안할수 있다.
	if (!m_bUsingAreaBox)
		return NULL;

	return pBox;
}

void MPxObjDynamic::RegisterDynamicObjectEntity( DynamicObjectEntity* pEntity )
{
	_ASSERT(pEntity);
	m_pDynamicObjectEntity = pEntity;
	m_pDynamicObjectEntity->m_pOwnerDynamicObject = this;
}

void MPxObjDynamic::UnRegisterDynamicObjectEntity()
{
	if (m_pDynamicObjectEntity)
	{
		m_pDynamicObjectEntity->m_pOwnerDynamicObject = NULL;
		m_pDynamicObjectEntity = NULL;
	}
}

void MPxObjDynamic::NotifyResultOnSimulated()
{
	// Entity 와 관련된 Notify 시작
	if (NULL == m_pDynamicObjectEntity)
		return;

	if(!m_bObservingResultOnSimulated)
		return;

	// SubActor 단위 Notify
	const list<int>& listUsedHandle = m_NxInfos.GetUsedHandleList();
	for (list<int>::const_iterator itr = listUsedHandle.begin(); itr != listUsedHandle.end(); ++itr)
	{
		NX_INFO& info = m_NxInfos.Get( *itr );
		if (NULL == info.m_pActor)
			continue;

		m_pDynamicObjectEntity->OnSubActorSimulated(info.m_pActor);
	}

	// PxObject (Dynamic) 단위 Notify
	m_pDynamicObjectEntity->OnSimulated();
}

} // namespace physx
