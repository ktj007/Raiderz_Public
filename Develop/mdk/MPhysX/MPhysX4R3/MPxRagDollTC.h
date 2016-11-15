#pragma once

#include "MPxTransformController.h"
#include "../MPxObjRagDoll.h"

namespace rs3
{
	class RPhysXRagDollFactor;
	class RPhysXActorController;
	class RPhysXRagDollBuilder;
}

namespace physx
{

class MPxRagDollTC : public MPxTcImpl<MPxRagDollTC>
{
public:
								MPxRagDollTC(MPxTcEntity* pRActorWrapper);
	virtual						~MPxRagDollTC();


	//////////////////////////////////////////////////////////////////////////
	// MPxTransformController Interface
	//
	virtual void				Update(float fDelta);
	virtual void				Stop(){ EndRagDoll(); }


	//////////////////////////////////////////////////////////////////////////
	// RagDoll Information
	//
	void						InitRagDollInfoFromModelName( const std::string& strModelName, const char* _pSzCollisionGroup );
	bool						IsRagDollAvailable();

	rs3::RPhysXRagDollFactor*	GetRagDollFactor(){ return m_pRagDollFactor; }
	const MMatrix*				GetAttacherOffsetTM(){ return m_RagDollEntity.GetAttacherOffsetTM(); }
	void						SetAttacherOffsetTM(bool bCustom, const MMatrix& _rTM){ m_RagDollEntity.SetAttacherOffsetTM(bCustom, _rTM); }


	//////////////////////////////////////////////////////////////////////////
	// RagDoll Trigger
	//
	void						StartRagDoll(const char* pSzRagdollContextName);
	void						EndRagDoll();
	void						ChangeRagDollFromContext(const char* pSzRagdollContextName);


private:

	void						CreateRagDoll(const char* pSzRagdollContextName);
	void						DeleteRagDoll();

	MPxObjRagDoll*				m_pPxObjectRagDoll;
	std::string					m_strRagDollInfoName;
	std::string					m_strCollisionGroup;


	//////////////////////////////////////////////////////////////////////////
	// RagDoll Factor
	//
	rs3::RPhysXRagDollFactor*	m_pRagDollFactor;


	//////////////////////////////////////////////////////////////////////////
	//
	// 리얼스페이스 액터와 와 MPhysX를 연결하는 RagDoll Entity
	// 리얼스페이스 액터를 조정하는 컨트롤러와 컨트롤러를 만드는 빌더를 가진다.
	//
	class MPxTcRagDollEntity : public MPxObjRagDoll::RagDollEntity
	{
	public:
								MPxTcRagDollEntity();
		virtual					~MPxTcRagDollEntity(){}

		virtual const MBox&		GetAreaBox();
		virtual bool			IsActive();
		virtual void			OnRefreshCollisionGroup(int nCollGroup);
		virtual void			OnSimulated();

		void					CreateRagDollEntity(rs3::RActor* pActor);
		void					DestroyRagDollEntity(rs3::RActor* pActor);

		bool					IsNeedCreateRagDollBuilder();
		void					CreateRagDollBuilder();
		void					DestroyRagDollBuilder();

		void					RegisterRagDollFactor(rs3::RPhysXRagDollFactor* pRagDollFactor);
		void					ChangeRagDollFromFactor();
		void					BuildNxObject(rs3::RActor* pActor);

		bool					IsRagDollAvailable();
		const MMatrix*			GetAttacherOffsetTM();
		void					SetAttacherOffsetTM(bool bUsingCustomOffset, const MMatrix& _rTM);

		void					UpdateRagDollTransform(rs3::RActor* pActor);

	private:

		rs3::RPhysXActorController*				m_pPhysXActorController;
		rs3::RPhysXRagDollBuilder*				m_pPhysXRagDollBuilder;
		rs3::RPhysXRagDollFactor*				m_pRagDollFactor;
		bool									m_bUsingCustomTMOffset;
		MMatrix									m_matCustomAttacherOffsetTM;

	} m_RagDollEntity;
};

}
