#pragma once

#include "MPxTransformController.h"
#include "MPxDynamicObjectEntity.h"

namespace physx
{

//////////////////////////////////////////////////////////////////////////
//
// 시뮬레이션을 할 액터노드의 그룹.
// 그룹단위 액터노드들이 시뮬레이션 한다.
//
//////////////////////////////////////////////////////////////////////////

class MPxTcDynamicActorNodesGroup
{
	friend class	MPxDynamicActorNodeTC;

public:

	void					StartPhysicSimulation(const std::vector<std::string>& _rActorNodeName, const vec3& _rForce, const vec3& _rForceWorldPosition, const char* _pSzDeletionChunck, bool bSetForce = true);
	void					StopPhysicSimulation();

	void					SetStoppedFadeOut(float fFadeOut){ m_pDynamicBodyEntity->SetFadeOutFactor(fFadeOut); } //-1 이면 fade out 하지 않는다.
	void					SetCustomReferenceNode(const std::string& _rRefNodePostfix){ m_pDynamicBodyEntity->SetCustomReferenceNode(_rRefNodePostfix); }

	void					SetMaterial(int nIdx);
	void					SetGroupId(std::vector<std::string>& vecList, int nId);
	void					SetForce(std::string& strActorNodeName, vec3& vForce, const vec3& _rForceWorldPosition);

	void					CheckBPartPhysicSimulation(std::vector<string>& vecEndList);

private:
							MPxTcDynamicActorNodesGroup(MPxDynamicActorNodeTC* pOwner, MPxTcEntity* pTcEntity);
	virtual					~MPxTcDynamicActorNodesGroup();

	rs3::RActor*			m_pActor;
	MPxDynamicActorNodeTC*	m_pOwnerActorNodeTC;

	MPxObjDynamic*			m_pPxObjectDynamicBody;
	MPxDynamicObjectEntity*	m_pDynamicBodyEntity;
};

//////////////////////////////////////////////////////////////////////////
//
// 액터노드의 시뮬레이션을 명령하는 TC
//
//////////////////////////////////////////////////////////////////////////

class MPxDynamicActorNodeTC : public MPxTcImpl<MPxDynamicActorNodeTC>
{
public:
	MPxDynamicActorNodeTC(MPxTcEntity* pTcEntity);
	virtual ~MPxDynamicActorNodeTC();


	//////////////////////////////////////////////////////////////////////////
	// MPxTransformController Interface
	//
	virtual void						Update(float fDelta);
	virtual void						Stop();
	void								ClearForReload();


	//////////////////////////////////////////////////////////////////////////
	// For Shape
	//
	void								GrabDynamicActorShape();
	void								ReleaseDynamicActorShape();


	//////////////////////////////////////////////////////////////////////////
	// Physics Simulation Object
	//
	MPxTcDynamicActorNodesGroup*		CreateDynamicActorNodesGroup(int nGroupId);
	MPxTcDynamicActorNodesGroup*		GetDynamicActorNodesGroup(int nGroupId);


	//////////////////////////////////////////////////////////////////////////
	// Physics Simulation Factor Maker
	//
	void								MakeUpwardForce(vec3& vOut, const vec3* _pInitDirection, float fXYForce, float fZForce);
	void								MakeUpwardRandomForce(vec3& vOut, const vec3* _pInitDirection);
	bool								MakeAndPrepareSimulationActorNodes(rs3::RActorNode* _pParentActorNode, std::vector<std::string>& _rFragActorNodes, bool _bPrepareActorNode = true);


	//////////////////////////////////////////////////////////////////////////
	// DynamicActorNodesGroup 에서 필요로 하는 정보들
	//
	bool								IsLinkedAtDynamicActorNode(){ return m_bLinkedAtDynamicActorNodes; }

	static std::string					GetFragActorNodeName(const char* chActorNodeName, int nIndex);

private:
	typedef								map<int, MPxTcDynamicActorNodesGroup*> DYNAMIC_ACTORNODES_MAP;
	DYNAMIC_ACTORNODES_MAP				m_mapDynamicActorNodesGroup;
	void								PrepareForDynamicActorNode(rs3::RActorNode* pActorNode);

	bool								m_bLinkedAtDynamicActorNodes;
	MPxDynamicObjectEntity*				m_pDynamicBodyEntityForShape;
};

}
