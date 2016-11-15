#pragma once

#include "RPhysXActorControllerBuilder.h"

#include <list>

namespace rs3 {

//////////////////////////////////////////////////////////////////////////
//
// Dynamic ActorNode Info
//
//////////////////////////////////////////////////////////////////////////

class RPhysXDynamicActorNodeInfo
{
public:
	RPhysXDynamicActorNodeInfo(const std::string& _rActorNodeName, NxActor* pNxActor)
		: m_strActorNodeName(_rActorNodeName)
		, m_pNxActor(pNxActor)
	{
	}

	std::string		m_strActorNodeName;
	NxActor*		m_pNxActor;
};

//////////////////////////////////////////////////////////////////////////
//
// Controller Builder for Dynamic ActorNode
//
//////////////////////////////////////////////////////////////////////////

class RPhysXDynamicActorNodeBuilder : public RPhysXActorControllerBuilder
{
public:

	RPhysXDynamicActorNodeBuilder(){}
	virtual ~RPhysXDynamicActorNodeBuilder();


	//////////////////////////////////////////////////////////////////////////
	// Builder Interface
	//
	virtual bool	Build( RPhysXActorController* pPhysXController );

	void			AddActorNodeInfo(RPhysXDynamicActorNodeInfo* pActorNodeInfo)	{ m_listInfos.push_back(pActorNodeInfo); }
	void			RemoveActorNodeInfo(RPhysXDynamicActorNodeInfo* pActorNodeInfo)	{ m_listInfos.remove(pActorNodeInfo); }

private:
	std::list<RPhysXDynamicActorNodeInfo*> m_listInfos;
};

}
