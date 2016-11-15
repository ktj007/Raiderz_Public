#pragma once

#include "RTypes.h"
#include "RActor.h"
#include "RActorController.h"

#include <map>

class NxScene;
class NxActor;
class NxJoint;

namespace rs3 {

class RSceneNode;

//////////////////////////////////////////////////////////////////////////
//
// PhysXSceneNode ActorNode Custom Controller
//
//////////////////////////////////////////////////////////////////////////

class RPhysXActorNodeCustomController
{
public:
	RPhysXActorNodeCustomController(){}
	virtual ~RPhysXActorNodeCustomController(){}

	virtual void OnCustomControl(RActor* pActor, NxActor* pNxActor, NxJoint* pNxJoint, NxMat34& rResultMat) = 0;
};

//////////////////////////////////////////////////////////////////////////
//
// PhysXSceneNode Controller
//
//////////////////////////////////////////////////////////////////////////

class RPhysXActorController : public RActorController
{
	friend class RPhysXActorControllerBuilder;

public:

	RPhysXActorController(NxScene* pNxScene, RSNCTRL_TYPE eType = RSNCTRL_CUSTOMUPDATE, const char* _pSzControllerName = NULL);
	virtual ~RPhysXActorController(void);


	//////////////////////////////////////////////////////////////////////////
	// 업데이트를 위한 정보
	//
	enum E_UPDATE_ATTRIBUTE : DWORD
	{
		APPLY_ACTOR_POS			= 1 << 1,
		APPLY_ACTOR_AXIS		= 1 << 2,
		APPLY_JOINT_POS			= 1 << 3,

		ADD_HIERARCHY_ATTR		= 1 << 4,
		ADD_BIP_ROOT_ATTR		= 1 << 5,
		ADD_PELVIS_ATTR			= 1 << 6,

		ADD_COLLISION_DATA		= 1 << 7,
		ADD_LINKEDNODE_DATA		= 1 << 8,
		ADD_CUSTOMCONTROL_DATA	= 1 << 9
	};


	//////////////////////////////////////////////////////////////////////////
	// RActorController로 부터 상속받아 구현
	//
	virtual void			OnAdd();
	virtual void			OnRemove();
	virtual void			ReNew(){ RefreshUpdateInfoMap(); }
	virtual bool			IsNeedAutoUpdate(){ return true; }

	virtual void			Update(RCameraSceneNode* pCamera) override;


	//////////////////////////////////////////////////////////////////////////
	// Actor 관련 정보 얻기
	//
	inline const RBox&		GetControlledActorAABB();


	//////////////////////////////////////////////////////////////////////////
	// NxActor관련
	//
	bool					IsNxObjectUpdatable(){ return !m_vecNxUpdateInfo.empty(); }

	bool					BuildNxObject(RPhysXActorControllerBuilder* pNxBuilder);
	void					ReleaseNxObject();

	inline NxActor*			GetNxActorFromResourceMap(int nActorID);
	inline NxJoint*			GetNxJointFromResourceMap(int nJointID);
	inline RActorNode*		GetActorNodeFromUpdateMap( NxActor* pRelatedNxActor );

	void					SetEnableMoveFuncInKinematic(bool bUseMoveFunc)	{ m_bUseMoveFuncInKinematic = bUseMoveFunc; }
	void					SetEnableMoveFuncInUpdateToPx(bool bUseMoveFunc){ m_bUseMoveFuncInUpdateToPx = bUseMoveFunc; }

	void					SetResourceNxActorDensity(float fDensity);
	void					SetResouceNxActorCollisionGroup(int nGroup);


	//////////////////////////////////////////////////////////////////////////
	// Frozen Mode
	//
	void					SetEnableKinematicMode(const char* pSzFixedActorNodeName);
	void					SetDisableKinematicMode(){ SetEnableKinematicMode(NULL); }
	void					SetKinematicModeTransform(const RMatrix& _rMat, bool bAlsoUpdateActor);
	const RMatrix&			GetKinematicLocalTM(){ return m_matKinematicLocalTransform; }

	unsigned int			GetUpdateCount() { return m_nUpdateCount; }

protected:

	// 컨트롤러 관련
	bool					m_bAddedOnActor;
	unsigned int			m_nUpdateCount;
	virtual void			OnBuilderAdded(){}
	void					SetEnableActorNodeAnimationUpdate(const std::string& _rStrActorNodeName, bool bEnable);
	void					UpdateRsToPx();
	void					UpdatePxToRs();

	//////////////////////////////////////////////////////////////////////////
	// Inner Types
	//
	struct SNxUpdateLinkedData
	{
		SNxUpdateLinkedData()
		{
			m_pLocalMat = NULL;
		}

		~SNxUpdateLinkedData()
		{
			delete m_pLocalMat;
			m_pLocalMat = NULL;
		}

		std::string	m_strLinkedNodeName;
		RMatrix*	m_pLocalMat;
	};

	struct SNxUpdateInfo
	{
	private:
		RActorNode*		m_pManagedActorNode;

	private:
		// 복사생성 금지
		SNxUpdateInfo(const SNxUpdateInfo& _rOlder){}

	public:
		SNxUpdateInfo(const std::string& rActorNodeName, NxActor* pNxActor, NxJoint* pNxJoint, DWORD dwAttribute, RActorNode* pCustomActorNode = NULL, void* pCustomData = NULL)
			: m_strActorNodeName(rActorNodeName)
			, m_pNxActor(pNxActor)
			, m_pNxJoint(pNxJoint)
			, m_dwAttribute(dwAttribute)
			, m_pManagedActorNode(NULL)
			, m_pCustomData(pCustomData)
			, m_fCustomGravity(-1.f)
			, m_vActorNodeScale(1.0f)
		{
			SetManagedActorNode(pCustomActorNode);
		}

		~SNxUpdateInfo()
		{
			if (m_pCustomData)
			{
				if (QueryAttribute(ADD_LINKEDNODE_DATA))
				{
					SNxUpdateLinkedData* pLinkedNodeData = (SNxUpdateLinkedData*)m_pCustomData;
					delete pLinkedNodeData;
				}
				else if (QueryAttribute(ADD_CUSTOMCONTROL_DATA))
				{
					RPhysXActorNodeCustomController* pCustomControl = (RPhysXActorNodeCustomController*)m_pCustomData;
					delete pCustomControl;
				}
			}
		}

		std::string		m_strActorNodeName;

		NxActor*		m_pNxActor;
		NxJoint*		m_pNxJoint;
		DWORD			m_dwAttribute;

		void*			m_pCustomData;

		float			m_fCustomGravity;

		RVector			m_vActorNodeScale;

		bool QueryAttribute(E_UPDATE_ATTRIBUTE _eAttribute) const
		{
			if ( (m_dwAttribute & _eAttribute) == _eAttribute )
				return true;
			return false;
		}

		void			SetManagedActorNode(RActorNode* pCustomActorNode)
		{
			m_pManagedActorNode = pCustomActorNode;

			m_vActorNodeScale = RVector(1.0f, 1.0f, 1.0f);
			if(m_pManagedActorNode)
				m_vActorNodeScale = m_pManagedActorNode->GetScale();
		}

		RActorNode*		GetManagedActorNode() const		{ return m_pManagedActorNode; }
	};
	typedef std::map<int, NxActor*> NX_ACTOR_RES_MAP;
	typedef std::map<int, NxJoint*> NX_JOINT_RES_MAP;
	typedef std::vector< SNxUpdateInfo* > NX_UPDATE_ARRAY;


	//////////////////////////////////////////////////////////////////////////
	// 리소스 관련
	//
	NX_ACTOR_RES_MAP		m_mapNxActors;
	NX_JOINT_RES_MAP		m_mapNxJoints;
	NxScene*				m_pNxScene;


	//////////////////////////////////////////////////////////////////////////
	// Update Map
	//
	NX_UPDATE_ARRAY			m_vecNxUpdateInfo;
	RMatrix					m_matUpdateParent;
	RMatrix					m_matUpdateParentInv;
	RMatrix					m_matHierarchyRootInv;
	void					GetNxWorldTransform(const SNxUpdateInfo& _refNxInfo, RMatrix& _refWorldTM);
	void					RefreshUpdateInfoMap();


	//////////////////////////////////////////////////////////////////////////
	// Kinematic Mode
	//
	std::string				m_strKinematicActorNodeName;
	int						m_nKinematicUpdateInfoID;
	RVector					m_vKinematicCurrentScale;
	RVector					m_vKinematicCurrentPos;
	RMatrix					m_matKinematicCurrentOrient;
	RQuaternion				m_qKinematicCurrentOrient;
	RMatrix					m_matKinematicLocalTransform;

	bool					m_bUseMoveFuncInKinematic;
	bool					m_bUseMoveFuncInUpdateToPx;
};
//----------------------------------------------------------------------------------------------------
inline const RBox& RPhysXActorController::GetControlledActorAABB()
{
	_ASSERT(m_pActor != NULL);
	return m_pActor->GetAABB();
}
//----------------------------------------------------------------------------------------------------
inline NxActor* RPhysXActorController::GetNxActorFromResourceMap( int nActorID )
{
	NX_ACTOR_RES_MAP::iterator itr = m_mapNxActors.find(nActorID);
	if (itr == m_mapNxActors.end())
	{
		return NULL;
	}

	return itr->second;
}
//----------------------------------------------------------------------------------------------------
inline NxJoint* RPhysXActorController::GetNxJointFromResourceMap( int nJointID )
{
	NX_JOINT_RES_MAP::iterator itr = m_mapNxJoints.find(nJointID);
	if (itr == m_mapNxJoints.end())
	{
		return NULL;
	}

	return itr->second;
}
//----------------------------------------------------------------------------------------------------
RActorNode* RPhysXActorController::GetActorNodeFromUpdateMap( NxActor* pRelatedNxActor )
{
	for (NX_UPDATE_ARRAY::iterator itr = m_vecNxUpdateInfo.begin(); itr != m_vecNxUpdateInfo.end(); ++itr)
	{
		if ((*itr)->m_pNxActor == pRelatedNxActor)
			return (*itr)->GetManagedActorNode();
	}

	return NULL;
}
//----------------------------------------------------------------------------------------------------
}
