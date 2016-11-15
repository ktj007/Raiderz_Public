#include "stdafx.h"
#include "MPxDynamicCollisionTC.h"
#include "MPhysX.h"

#include "RActor.h"

namespace physx
{
using namespace rs3;


MPxDynamicCollisionTC::MPxDynamicCollisionTC( MPxTcEntity* pTcEntity )
: MPxTcImpl(pTcEntity), m_pPxObjectDynamicBody(NULL)
, m_strCollisionPrefix("col_")
{
	m_pDynamicBodyEntity = new MPxDynamicObjectEntity(pTcEntity);
}

MPxDynamicCollisionTC::~MPxDynamicCollisionTC()
{
	DeActivateCollision();
	delete m_pDynamicBodyEntity;
}

void MPxDynamicCollisionTC::Update( float fDelta )
{
	DeActivateCollision();
}

void MPxDynamicCollisionTC::Stop()
{
	DeActivateCollision();
}

void MPxDynamicCollisionTC::ActivateCollision()
{
	DeActivateCollision();
	RActor* pActor = GetActor();
	_ASSERT(pActor);
	std::vector<std::string> vActorNodes;
	std::vector< RActorNode* >& rActorNodes = pActor->GetActorNodes();
	for(std::vector< RActorNode* >::iterator itr = rActorNodes.begin(); itr != rActorNodes.end(); ++itr)
	{
		const std::string& rStrNodeName = (*itr)->GetNodeName();
		if ( rStrNodeName.find(m_strCollisionPrefix) == 0)
		{
			vActorNodes.push_back(rStrNodeName);
		}
	}

	if (vActorNodes.empty())
	{
		mlog("no col_ actor node\n");
		return;
	}

	m_pPxObjectDynamicBody = MPhysX::GetInstance()->CreatePxObject<physx::MPxObjDynamic>();
	m_pDynamicBodyEntity->ResetDynamicBodyEntity(pActor, vActorNodes, NULL);
	m_pPxObjectDynamicBody->RegisterDynamicObjectEntity(m_pDynamicBodyEntity);
	m_pDynamicBodyEntity->SetKinematicMode(true, m_pPxObjectDynamicBody);

	if (m_pPxObjectDynamicBody->AddSubActorFromEntity())
	{
		m_pPxObjectDynamicBody->ActivateSubActorsAll();
	}
	else
	{
		mlog("failed adding shape in physx\n");
		DeActivateCollision();
	}
}

void MPxDynamicCollisionTC::DeActivateCollision()
{
	if (m_pPxObjectDynamicBody)
	{
		m_pPxObjectDynamicBody->DeActivateSubActorsAll();
		m_pPxObjectDynamicBody->UnRegisterDynamicObjectEntity();

		MPhysX::GetInstance()->RemovePxObject(m_pPxObjectDynamicBody);
		m_pPxObjectDynamicBody = NULL;
	}
	m_pDynamicBodyEntity->ClearHandles();
}

}
