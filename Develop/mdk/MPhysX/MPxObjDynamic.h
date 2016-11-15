#pragma once

#include "MPhysXObject.h"

namespace physx {

//////////////////////////////////////////////////////////////////////////
//
// MPxObjDynamic
//  - Dynamic Object (동적인 SubActors를 가지는 오브젝트)
//  - Dynamic NxActor와 유사하며 모든 충돌 및 물리 시뮬레이션을
//    MPhysX의 m_pScene이 알아서 처리한다.
//
//////////////////////////////////////////////////////////////////////////

class MPhysXCollisionReport;
class MPxObjDynamic : public MPhysXObject
{
public:
	MPxObjDynamic();
	virtual ~MPxObjDynamic();

	//////////////////////////////////////////////////////////////////////////
	// Dynamic Object Entity
	//
	class DynamicObjectEntity
	{
	public:

		// Collision Group
		int						GetCollisionGroup(){ return m_nCollisionGroup; }
		void					SetCollisionGroup(int _nCollisionGroup){ m_nCollisionGroup = _nCollisionGroup; }

	protected:
								DynamicObjectEntity(){ m_pOwnerDynamicObject = NULL; m_nCollisionGroup = 0; }
		virtual					~DynamicObjectEntity(){ _ASSERT(m_pOwnerDynamicObject == NULL); }

		// 생성에 필요한 정보
		virtual int				GetSubActorCount()	= 0;
		virtual bool			GetSubActorInfo(int _nIndex, std::string& _rShapeName, std::vector<NxVec3>& _refVertices, std::vector<NxU32>& _refFaces, MMatrix& _rMatLocal, bool& _rIsKinematic) = 0;

		// 생성시에 (SubActor 단위로 생성)
		virtual void			OnSubActorAdded( int _nSubActorIndex, MSUBACTORHANDLE _hAddedSubActorHandle ) = 0;
		virtual void			OnActivatedSubActorsAll(MMatrix& _rOutEntityTM) = 0;
		virtual void			OnDeActivatedSubActorsAll() = 0;

		// SubActor 시뮬레이션 관련
		virtual void			OnSubActorSimulated( NxActor* pNxActor ) = 0;
		virtual void			OnSubActorCollisionStarted(NxActor* pNxActor) = 0;


		// MPhysX 영역 활성화를 위한 AreaBox
		virtual const MBox&		GetAreaBox() = 0;

		// 시뮬레이트 된 후에
		virtual void			OnSimulated() = 0;

		// 핸들로 부터 NxActor 얻어오기
		NxActor* GetNxActorFromHandle( MSUBACTORHANDLE _hHandle )
		{
			if (NULL == m_pOwnerDynamicObject)
				return NULL;

			return m_pOwnerDynamicObject->GetNxActor(_hHandle);
		}

	private:
		friend					MPxObjDynamic;
		friend					MPhysXCollisionReport;
		MPxObjDynamic*			m_pOwnerDynamicObject;
		int						m_nCollisionGroup;
	};


	//////////////////////////////////////////////////////////////////////////
	// Dynamic Object Public Method
	//
	void						RegisterDynamicObjectEntity(DynamicObjectEntity* pEntity);
	void						UnRegisterDynamicObjectEntity();

	bool						AddSubActorFromEntity();
	void						ActivateSubActorsAll();
	void						DeActivateSubActorsAll();

	void						ApplyForce(const MVector3& _rForce, const MVector3& _rForceWorldPos);

	virtual void				NotifyResultOnSimulated();
	void						SetEnableObservingResultOnSimulated(bool bEnable){ m_bObservingResultOnSimulated = bEnable; }

	virtual const MBox*			GetAreaBox();
	void						SetEnableUsingAreaBox(bool bEnalbe){ m_bUsingAreaBox = bEnalbe; }

protected:

	virtual void				OnDoPreUpdateSubActorTransform() {}
	virtual void				OnDoPostUpdateSubActorTransform(){}

	bool						m_bActive;
	DynamicObjectEntity*		m_pDynamicObjectEntity;
	MBox						m_aabb;

	bool						m_bObservingResultOnSimulated;
	bool						m_bUsingAreaBox;
};

} // namespace physx
