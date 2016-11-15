#pragma once

#include "IK/CNode.h"

using namespace rs3;

class IIKSolver;
class CNode;


namespace rs3 {
	class RActor;
	class RActorNode;
}


struct RIKNode
{
	RIKNode() : pSceneNode(NULL), pActorNode(NULL) { }

	RActorNode* pActorNode;
	CNode		node;				// IK node
	RSceneNode *pSceneNode;			// 디버그를 위한 SceneNode
};



class RIKSkeletonController
{
	RActor*			m_pActor;
	IIKSolver*		m_pIKSolver;
	list<RIKNode*>	m_nodes;
	RMatrix			m_OffsetMatrix;  //  root의 부모노드가 있을때

	void			AddNode(RActorNode* pActorNode, bool IsRoot = false);

public:
	RIKSkeletonController(RActor* pActor);
	virtual ~RIKSkeletonController(void);

	bool			Create(RActorNode* pStartNode,RActorNode* pEndNode);
	size_t			GetNodeCount();
	void			SetTargetPosition(const RVector& targetPosition);
	void			Solve();
	void			Initialize();

	RIKNode*		GetRoot();
	RIKNode*		GetEffector();
};


//////////////
inline size_t RIKSkeletonController::GetNodeCount() { return m_nodes.size(); }
inline RIKNode* RIKSkeletonController::GetRoot() { return m_nodes.empty() ? NULL : m_nodes.front(); }
inline RIKNode* RIKSkeletonController::GetEffector() { return m_nodes.empty() ? NULL : m_nodes.back(); }