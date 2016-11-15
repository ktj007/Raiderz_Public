#pragma once

#include "MPhysXObject.h"

namespace physx {

//////////////////////////////////////////////////////////////////////////
//
// MPxObjRagDoll
//  - 랙돌 오브젝트. MPxObjDynamic과는 다른 Entity를 가진다.
//
//////////////////////////////////////////////////////////////////////////

class MPxObjRagDoll : public MPhysXObject
{
public:
	MPxObjRagDoll();
	virtual ~MPxObjRagDoll();

	//////////////////////////////////////////////////////////////////////////
	// Ragdoll Entity : MPxObjRagDoll 이 관리하는 랙돌 엔티티
	//
	class RagDollEntity
	{
	public:
		RagDollEntity(){}
		virtual ~RagDollEntity(){}

		virtual void		Create(NxScene* pScene){}
		virtual void		Destroy(NxScene* pScene){}

		virtual bool		IsActive(){ return true; }

		virtual const MBox& GetAreaBox() = 0;
		virtual void		OnSimulated() = 0;
		virtual void		OnRefreshCollisionGroup(int nCollGroup){}
	};

	void					CreateSampleRagDoll(const MVector3& _rPos);

	void					RegisterRagDollEntity(RagDollEntity* pRagDollEntity){ m_pRagDoll = pRagDollEntity; }
	void					UnRegisterRagDollEntity(){ m_pRagDoll = NULL; }

	virtual void			NotifyResultOnSimulated(){ if (m_pRagDoll) { m_pRagDoll->OnSimulated(); } }
	virtual const MBox*		GetAreaBox();

protected:

	RagDollEntity*			m_pRagDoll;
	MBox					m_AABB;

	virtual void			OnDoPreUpdateSubActorTransform() { _ASSERT(0); } //미구현
	virtual void			OnDoPostUpdateSubActorTransform(){ _ASSERT(0); } //미구현

	virtual int				OnRefreshCollisionGroup();
};

} // namespace physx
