#include "stdafx.h"

#include "MPhysXObject.h"
#include "MPhysX.h"

#include "MPhysXStream.h"
#include "NxCooking.h"

#include "mmsystem.h"

namespace physx {

MPhysXObject::MPhysXObject()
: m_matWorldTransform(MMatrix::IDENTITY)
, m_bNeedToUpdateTransform(false)
{
}

MPhysXObject::~MPhysXObject()
{
	// nx_info
	const list<int>& usedHandle = m_NxInfos.GetUsedHandleList();
	while (usedHandle.empty() == false)
	{
		int nHandle = *(usedHandle.begin());
		ClearNxInfoAndNxActor( m_NxInfos.Get(nHandle), true );
		m_NxInfos.Remove(nHandle);
	}
}

NxScene* MPhysXObject::GetNxScene()
{
	_ASSERT(m_pPhysX != NULL);
	return m_pPhysX->m_pScene;
}

NxActor* MPhysXObject::GetNxActorBelongToSubActor( MSUBACTORHANDLE _hSubActor )
{
	return m_NxInfos.Get(_hSubActor).m_pActor;
}

void MPhysXObject::ReserveToReleaseNxActor( NxActor* _pActor )
{
	_ASSERT(m_pPhysX);
	_ASSERT(_pActor);

	// 연결된 유저 데이타 초기화
	_pActor->userData = NULL;

	// 지울 NxActor에 추가
	m_pPhysX->m_vecNxActorToRelease.push_back(_pActor);
}

void MPhysXObject::ActivateAllSubActors()
{
	const list<int>& listUsedHandle = m_NxInfos.GetUsedHandleList();
	for (list<int>::const_iterator itr = listUsedHandle.begin(); itr != listUsedHandle.end(); ++itr)
		ActivateSubActor( (MSUBACTORHANDLE)*itr );
}

void MPhysXObject::ActivateSubActor( MSUBACTORHANDLE _hShape )
{
	// Activate하는 시점에 NxActor를 생성한다
	CreateNxActorByNxInfo(m_NxInfos.Get(_hShape), true);
	m_bNeedToUpdateTransform = true;
}

void MPhysXObject::DeActiveSubActor( MSUBACTORHANDLE _hShape )
{
	ClearNxInfoAndNxActor(m_NxInfos.Get(_hShape), false);
}

void MPhysXObject::DeActiveAllSubActors()
{
	const list<int>& listUsedHandle = m_NxInfos.GetUsedHandleList();
	for (list<int>::const_iterator itr = listUsedHandle.begin(); itr != listUsedHandle.end(); ++itr)
		DeActiveSubActor( (MSUBACTORHANDLE)*itr );
}

MSUBACTORHANDLE MPhysXObject::DoAddSubActorPlane( const MPlane& _rLocalPlane, float _fDensity )
{
	// Create a plane with default descriptor
	NxPlaneShapeDesc* pPlaneDesc = new NxPlaneShapeDesc;
	pPlaneDesc->normal.x = _rLocalPlane.a;
	pPlaneDesc->normal.y = _rLocalPlane.b;
	pPlaneDesc->normal.z = _rLocalPlane.c;
	pPlaneDesc->d = _rLocalPlane.d;

	return m_NxInfos.Add( NX_INFO(PS_PLANE, MMatrix::IDENTITY, _fDensity, m_pPhysX->CreatePxShape(pPlaneDesc)) );
}

MSUBACTORHANDLE MPhysXObject::DoAddSubActorBox( const MBox& _rLocalBox, float _fDensity )
{
	NxBoxShapeDesc* pBodyDesc = new NxBoxShapeDesc;

	// The actor has one shape, a box
	pBodyDesc->dimensions.set(  (_rLocalBox.vmax.x - _rLocalBox.vmin.x) / 2,
								(_rLocalBox.vmax.y - _rLocalBox.vmin.y) / 2,
								(_rLocalBox.vmax.z - _rLocalBox.vmin.z) / 2 );
	pBodyDesc->localPose.t = NxVec3(_rLocalBox.GetCenter().x, _rLocalBox.GetCenter().y, _rLocalBox.GetCenter().z);

	return m_NxInfos.Add( NX_INFO(PS_DYNAMIC_CONVEX, MMatrix::IDENTITY, _fDensity, m_pPhysX->CreatePxShape(pBodyDesc)) );
}

MSUBACTORHANDLE MPhysXObject::DoAddSubActorCapsule( const MCapsule& _rLocalCapsule, float _fDensity )
{
	MVector3 vCenterDir( (_rLocalCapsule.top - _rLocalCapsule.bottom ) * 0.5f );
	MVector3 vDir(vCenterDir);
	if (vDir.x < FLT_EPSILON && vDir.y < FLT_EPSILON)
		vDir = MVector3::AXISY;

	MMatrix matLocal = MMatrix::IDENTITY;
	matLocal.SetLocalMatrix(_rLocalCapsule.bottom + vCenterDir, vDir, MVector3::AXISZ);

	NxCapsuleShapeDesc* pCapsuleDesc = new NxCapsuleShapeDesc;
	pCapsuleDesc->height = _rLocalCapsule.height;
	pCapsuleDesc->radius = _rLocalCapsule.radius;

	// 눕혀있는 캡슐 세워주기
	NxMat33 matRot;
	matRot.rotX(NxPi*0.5f);
	pCapsuleDesc->localPose.M = matRot;

	return m_NxInfos.Add( NX_INFO(PS_CAPSULE, matLocal, _fDensity, m_pPhysX->CreatePxShape(pCapsuleDesc)) );
}

MSUBACTORHANDLE MPhysXObject::DoAddSubActorSphere( const MSphere& _rLocalSphere, float _fDensity )
{
	// The actor has one shape, a sphere
	NxSphereShapeDesc* pSphereDesc = new NxSphereShapeDesc;
	pSphereDesc->radius = _rLocalSphere.radius;
	pSphereDesc->localPose.t = NxVec3(_rLocalSphere.center.x, _rLocalSphere.center.y, _rLocalSphere.center.z);

	return m_NxInfos.Add( NX_INFO(PS_SPHERE, MMatrix::IDENTITY, _fDensity, m_pPhysX->CreatePxShape(pSphereDesc)) );
}

MSUBACTORHANDLE MPhysXObject::DoAddSubActorSphereCCD( const MSphere& _rLocalSphere, float _fDensity )
{
	// The actor has one shape, a sphere
	NxSphereShapeDesc* pSphereDesc = new NxSphereShapeDesc;
	pSphereDesc->radius = _rLocalSphere.radius;
	pSphereDesc->localPose.t = NxVec3(_rLocalSphere.center.x, _rLocalSphere.center.y, _rLocalSphere.center.z);
	
	MPhysXShape* pShape = m_pPhysX->CreatePxShape(pSphereDesc);
	pShape->CreateSphereCCD(_rLocalSphere);

	return m_NxInfos.Add( NX_INFO(PS_SPHERE, MMatrix::IDENTITY, _fDensity, pShape) );
}

void MPhysXObject::DoUpdateTransform()
{
	if (m_bNeedToUpdateTransform == false)
		return;

	// Shape Transform Update 하기 전에 해주어야 할것들
	OnDoPreUpdateSubActorTransform();

	// Shape Transform Update
	const list<int>& usedHandle = m_NxInfos.GetUsedHandleList();
	if (usedHandle.empty() == false)
	{
		NxMat33 orient;
		NxVec3 pos;

		for(list<int>::const_iterator i=usedHandle.begin(); i!=usedHandle.end();i++)
		{
			NX_INFO& info = m_NxInfos.Get(*i);
			NxActor* pActor = info.m_pActor;
			if (pActor == NULL)
				continue;

			const MMatrix& matShapeLocal = info.m_matLocalTransform;
			MPhysXHelper::DecomposeMMatrix( matShapeLocal * m_matWorldTransform, orient, pos );
			pActor->setGlobalOrientation(orient);
			pActor->setGlobalPosition(pos);
		}
	}

	// Shape Transform Update 후에 불려야 하는것들
	OnDoPostUpdateSubActorTransform();

	// UpdateTransform 완료
	m_bNeedToUpdateTransform = false;
}

void MPhysXObject::CreateNxActorByNxInfo( NX_INFO& _rNxInfo, bool bWaitForAllCreation )
{
	_ASSERT(_rNxInfo.m_pPhysXShape != NULL);
	_ASSERT(GetNxScene());

	// body를 갖고있는 모든 actorDesc는 actor를 default dynamic으로 만든다. actorDesc가 NULL body를 갖고 있다면 static 이다.
	NxActorDesc actorDesc;

	// density와 body 설정
	// density 가 0 이면 static 이라고 가정해서 설정하지 않는다.
	if (_rNxInfo.m_fDensity >= FLT_EPSILON)
	{
		// 양수면 밀도설정
		actorDesc.density = _rNxInfo.m_fDensity;
		actorDesc.body = &_rNxInfo.m_BodyDesc;
	}
	else if ( _rNxInfo.m_fDensity < 0.f )
	{
		// 음수면 키네마틱
		_rNxInfo.m_BodyDesc.flags |= NX_BF_KINEMATIC;
		actorDesc.density = 1.f;
		actorDesc.body = &_rNxInfo.m_BodyDesc;
	}

	actorDesc.shapes.push_back(_rNxInfo.m_pPhysXShape->GetNxShapeDesc());
	MPhysXHelper::DecomposeMMatrix(_rNxInfo.m_matLocalTransform, actorDesc.globalPose.M, actorDesc.globalPose.t);
	if(actorDesc.isValid() == false )
		return;

	_rNxInfo.m_pActor = GetNxScene()->createActor(actorDesc);
	if (bWaitForAllCreation && _rNxInfo.m_pActor == NULL)
	{
		while (_rNxInfo.m_pActor != NULL)
		{
			_rNxInfo.m_pActor = GetNxScene()->createActor(actorDesc);
		}
	}
}

void MPhysXObject::ClearNxInfoAndNxActor( NX_INFO& _rNxInfo, bool bDeleteDesc )
{
	// NxActor;
	if (_rNxInfo.m_pActor)
	{
		ReserveToReleaseNxActor(_rNxInfo.m_pActor);
		_rNxInfo.m_pActor = NULL;
	}

	// NxShapeDesc
	if (bDeleteDesc && _rNxInfo.m_pPhysXShape)
	{
		m_pPhysX->RemovePxShape( _rNxInfo.m_pPhysXShape );
		_rNxInfo.m_pPhysXShape = NULL;
	}
}

bool MPhysXObject::AddToCollisionGroup( const char* _pSzCollsionGroup )
{
	std::set<std::string>::iterator itr = m_setCollsionGroups.find(_pSzCollsionGroup);
	if (itr != m_setCollsionGroups.end())
		return false;

	m_setCollsionGroups.insert( _pSzCollsionGroup );
	return ( OnRefreshCollisionGroup() != 0 );
}

bool MPhysXObject::RemoveFromCollisionGroup( const char* _pSzCollsionGroup )
{
	std::set<std::string>::iterator itr = m_setCollsionGroups.find(_pSzCollsionGroup);
	if (itr == m_setCollsionGroups.end())
		return false;

	m_setCollsionGroups.erase(itr);
	return ( OnRefreshCollisionGroup() != 0 );
}

int MPhysXObject::OnRefreshCollisionGroup()
{
	int nCollisionGroup = 0;
	for (std::set<std::string>::iterator itr = m_setCollsionGroups.begin(); itr != m_setCollsionGroups.end(); ++itr)
	{
		int nCollMask = m_pPhysX->GetCollsionGroupId(*itr);
		if (nCollMask > 0)
		{
			nCollisionGroup += nCollMask;
		}
		else
		{
			mlog("%s is not defined collision group\n", itr->c_str() );
		}
	}

	return nCollisionGroup;
}

} // namespace physx
