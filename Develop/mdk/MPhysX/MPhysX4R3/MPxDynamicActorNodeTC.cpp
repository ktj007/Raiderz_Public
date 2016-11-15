#include "stdafx.h"
#include "MPxDynamicActorNodeTC.h"
#include "MPhysX.h"

#include "RActor.h"
#include "RAnimationControllerNode.h"
#include "RPhysXActorController.h"

#include "MTime.h"

namespace physx
{

using namespace rs3;

MPxTcDynamicActorNodesGroup::MPxTcDynamicActorNodesGroup(MPxDynamicActorNodeTC* pOwner, MPxTcEntity* pTcEntity)
{
	_ASSERT(pTcEntity);
	_ASSERT(pOwner);

	m_pOwnerActorNodeTC = pOwner;
	m_pActor = pTcEntity->GetActor();

	m_pPxObjectDynamicBody = NULL;
	m_pDynamicBodyEntity = new MPxDynamicObjectEntity(pTcEntity);
}

MPxTcDynamicActorNodesGroup::~MPxTcDynamicActorNodesGroup()
{
	_ASSERT(m_pPxObjectDynamicBody == NULL);
	delete m_pDynamicBodyEntity;
}

void MPxTcDynamicActorNodesGroup::StartPhysicSimulation( const std::vector<std::string>& _rActorNodeName, const vec3& _rForce, const vec3& _rForceWorldPosition, const char* _pSzDeletionChunck, bool bSetForce /*= true*/ )
{
	//MTimeChecker tc;
	//tc.Start();
	StopPhysicSimulation();

	m_pPxObjectDynamicBody = MPhysX::GetInstance()->CreatePxObject<physx::MPxObjDynamic>();
	m_pDynamicBodyEntity->ResetDynamicBodyEntity(m_pActor, _rActorNodeName, _pSzDeletionChunck);

	m_pPxObjectDynamicBody->RegisterDynamicObjectEntity(m_pDynamicBodyEntity);
	//mlog("3. %u ms : MPhysX 물체 만들기 및 Entity등록\n", tc.Check());
	//tc.Start();
	if (m_pPxObjectDynamicBody->AddSubActorFromEntity())
	{
		// set collision group
		bool bSelfGroupCollision = false;
		if (m_pOwnerActorNodeTC->IsLinkedAtDynamicActorNode())
			bSelfGroupCollision = true;

		_ASSERT(MPhysX::GetInstance());
		//int nCollisionGroup = MPhysX::GetInstance()->GetDefaultDynamicObjectCollisionGroupId(bSelfGroupCollision);
		int nCollisionGroup = MPhysX::GetInstance()->GetDefaultDynamicObjectCollisionGroupId(true);
		m_pDynamicBodyEntity->SetCollisionGroup(nCollisionGroup);

		// activate subactor
		m_pPxObjectDynamicBody->ActivateSubActorsAll();
		if(bSetForce)
			m_pPxObjectDynamicBody->ApplyForce(_rForce, _rForceWorldPosition);

		//mlog("5. %u ms : PhysX 액터만들기\n", tc.Check());
	}
	else
	{
		mlog("failed adding shape in physx\n");
		StopPhysicSimulation();
	}
}

void MPxTcDynamicActorNodesGroup::StopPhysicSimulation()
{
	if (m_pPxObjectDynamicBody)
	{
		m_pDynamicBodyEntity->DeleteActorNodes();

		m_pPxObjectDynamicBody->DeActivateSubActorsAll();
		m_pPxObjectDynamicBody->UnRegisterDynamicObjectEntity();

		MPhysX::GetInstance()->RemovePxObject(m_pPxObjectDynamicBody);
		m_pPxObjectDynamicBody = NULL;
	}
	m_pDynamicBodyEntity->ClearHandles();
}

void MPxTcDynamicActorNodesGroup::SetMaterial( int nIdx )
{
	for each (NxActor* each in m_pDynamicBodyEntity->GetActors())
	{
		NxU32 nShapes = each->getNbShapes();
		if(nShapes > 0)
		{
			NxShape*const* shapes = each->getShapes();
			while( nShapes-- )
			{
				shapes[nShapes]->setMaterial(nIdx);
			}
		}
	}
}

void MPxTcDynamicActorNodesGroup::CheckBPartPhysicSimulation( std::vector<string>& vecEndList )
{
	m_pDynamicBodyEntity->CheckEndSubActorSimulate(vecEndList);
}

void MPxTcDynamicActorNodesGroup::SetGroupId( std::vector<std::string>& vecList, int nId )
{
	for each (NxActor* each in m_pDynamicBodyEntity->GetActors())
	{
		rs3::RPhysXActorController* pPhysxActorController = m_pDynamicBodyEntity->GetPhysXActorController();
		if(pPhysxActorController)
		{
			RActorNode* pActorNode = pPhysxActorController->GetActorNodeFromUpdateMap(each);
			if(pActorNode)
			{
				std::string strActorNodeName = pActorNode->GetNodeName();

				for(std::vector<std::string>::iterator it = vecList.begin(); it != vecList.end(); ++it)
				{
					if(*it == strActorNodeName)
					{
						each->setGroup(nId);
						NxU32 nShapes = each->getNbShapes();
						if(nShapes > 0)
						{
							NxShape*const* shapes = each->getShapes();
							while( nShapes-- )
							{
								shapes[nShapes]->setGroup(nId);
							}
						}
					}
				}
			}
		}
	}
}

void MPxTcDynamicActorNodesGroup::SetForce( std::string& strActorNodeName, vec3& vForce, const vec3& _rForceWorldPosition )
{
	for each (NxActor* each in m_pDynamicBodyEntity->GetActors())
	{
		rs3::RPhysXActorController* pPhysxActorController = m_pDynamicBodyEntity->GetPhysXActorController();
		if(pPhysxActorController)
		{
			RActorNode* pActorNode = pPhysxActorController->GetActorNodeFromUpdateMap(each);
			if(pActorNode)
			{
				if(pActorNode->GetNodeName() == strActorNodeName)
				{
					each->addForceAtPos( NxVec3(vForce.x, vForce.y, vForce.z), NxVec3(_rForceWorldPosition.x, _rForceWorldPosition.y, _rForceWorldPosition.z), NX_VELOCITY_CHANGE );
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

MPxDynamicActorNodeTC::MPxDynamicActorNodeTC( MPxTcEntity* pTcEntity )
: MPxTcImpl(pTcEntity)
{
	m_bLinkedAtDynamicActorNodes = false;
	m_pDynamicBodyEntityForShape = new MPxDynamicObjectEntity(pTcEntity);
}

MPxDynamicActorNodeTC::~MPxDynamicActorNodeTC()
{
	Stop();
	delete m_pDynamicBodyEntityForShape;
}

void MPxDynamicActorNodeTC::Stop()
{
	for (DYNAMIC_ACTORNODES_MAP::iterator itr = m_mapDynamicActorNodesGroup.begin(); itr != m_mapDynamicActorNodesGroup.end(); ++itr)
	{
		itr->second->StopPhysicSimulation();
		delete itr->second;
	}

	ReleaseDynamicActorShape();
}

void MPxDynamicActorNodeTC::MakeUpwardForce(vec3& vOut, const vec3* _pInitDirection, float fXYForce, float fZForce)
{
	// 초기값 설정
	if (_pInitDirection)
	{
		vOut.Set(*_pInitDirection);
	}
	else
	{
		vOut.Set(vec3::AXISX);
		MMatrix matRotZ(MMatrix::IDENTITY);
		matRotZ.SetRotationZ( MMath::RandomNumber(0.f, MMath::TWOPI) );
		matRotZ.TransformVect(vOut);
	}

	vOut *= fXYForce;
	vOut.z = fZForce;
}

void MPxDynamicActorNodeTC::MakeUpwardRandomForce(vec3& vOut, const vec3* _pInitDirection)
{
	float fXYForce = MMath::RandomNumber(250.f, 300.f);
	float fZForce = MMath::RandomNumber(650.f, 800.f);

	return MakeUpwardForce(vOut, _pInitDirection, fXYForce, fZForce);
}

bool MPxDynamicActorNodeTC::MakeAndPrepareSimulationActorNodes( RActorNode* _pParentActorNode, std::vector<std::string>& _rFragActorNodes, bool _bPrepareActorNode )
{
	_ASSERT(_rFragActorNodes.empty());
	if (_pParentActorNode == NULL)
		return false;

	RActor* pActor = _pParentActorNode->m_pActor;
	const char* _pSzActorNodeName = _pParentActorNode->GetNodeName().c_str();

	int nCount = 0;
	char chNodeName[257] = {0,};

	while (true)
	{
		sprintf_s(chNodeName, "%s%s%02d", _pSzActorNodeName, "_frag_", nCount);
		RActorNode* pActorNodeFrag = pActor->GetActorNode(chNodeName);
		if (pActorNodeFrag == NULL)
		{
			std::string strImActorNodeName(chNodeName);
			strImActorNodeName += "_im";
			pActorNodeFrag = pActor->GetActorNode(strImActorNodeName.c_str());
		}

		if (pActorNodeFrag)
		{
			if (_bPrepareActorNode)
				PrepareForDynamicActorNode(pActorNodeFrag);

			_rFragActorNodes.push_back(pActorNodeFrag->GetNodeName());

			++nCount;
		}
		else
		{
			break;
		}
	}

	if (_rFragActorNodes.empty())
	{
		// 준비를 안해야 하는 경우
		if (_pParentActorNode->HasRefBone())
			return false;

		if (_bPrepareActorNode)
			PrepareForDynamicActorNode(_pParentActorNode);

		_rFragActorNodes.push_back(_pParentActorNode->GetNodeName());
	}
	else
	{
		if (_bPrepareActorNode)
			_pParentActorNode->SetVisible(false);
	}

	return true;
}

void MPxDynamicActorNodeTC::PrepareForDynamicActorNode( RActorNode* pActorNode)
{
	//////////////////////////////////////////////////////////////////////////
	//
	// disconnect hierarchy and set for non-hierarchy
	//
	//////////////////////////////////////////////////////////////////////////

	// Visible Visitor. by pok. (2010. 11. 6)
	// bparts 밑에 하위 자식이 붙을수 있게 되었다.
	// hierarchy와 Scale은 하위 자식의 경우 상위에 영향을 받아 동작하고
	// 변경해야할 필요는 없으므로 Visitor로 변경하지 않는다.
	//pActorNode->SetVisible(true);
	//pActorNode->SetVisibility(1.f);
	class VisibleVisitor : public RSceneNodeVisitor
	{
		virtual bool OnPreOrderVisit( RSceneNode* pSceneNode )
		{
			pSceneNode->SetVisible(true);
			pSceneNode->SetVisibility(1.f);
			return true;
		}
		virtual void OnPostOrderVisit( RSceneNode* pSceneNode ){}

	} visableVisitor;
	pActorNode->Traverse(&visableVisitor);

	pActorNode->AddAttribute(RSNA_NO_HIERARCHY_UPDATE);
	//RActor* pOwnerActor = pActorNode->m_pActor;
	//pActorNode->SetScale(pActorNode->GetScale() * pOwnerActor->GetScale());

	if (false == pActorNode->GetChildren().empty())
		m_bLinkedAtDynamicActorNodes = true;

	//RAnimationControllerNode* pNode = pOwnerActor->GetAnimationController().GetNodeController( pActorNode->GetNodeName().c_str() ) ;
	//if (pNode)
	//{
	//	pNode->SetEnableAnimationUpdate(false);
	//}
}

void MPxDynamicActorNodeTC::Update( float fDelta )
{
	for(DYNAMIC_ACTORNODES_MAP::iterator itr = m_mapDynamicActorNodesGroup.begin(); itr != m_mapDynamicActorNodesGroup.end();)
	{
		MPxTcDynamicActorNodesGroup* pDynamicActorNodesGroup = itr->second;
		if( pDynamicActorNodesGroup->m_pDynamicBodyEntity->IsAllShapesDeActive())
		{
			pDynamicActorNodesGroup->StopPhysicSimulation();
			delete pDynamicActorNodesGroup;
			m_mapDynamicActorNodesGroup.erase( itr++ );
		}
		else
		{
			++itr;
		}
	}

	if (m_mapDynamicActorNodesGroup.empty())
		SetEnable(false);
}

MPxTcDynamicActorNodesGroup* MPxDynamicActorNodeTC::CreateDynamicActorNodesGroup( int nGroupId )
{
	if (m_mapDynamicActorNodesGroup.find(nGroupId) != m_mapDynamicActorNodesGroup.end())
		return NULL;

	MPxTcDynamicActorNodesGroup* pDynamicActorNodesGroup = new MPxTcDynamicActorNodesGroup(this, m_pTcEntity);
	m_mapDynamicActorNodesGroup.insert(DYNAMIC_ACTORNODES_MAP::value_type(nGroupId, pDynamicActorNodesGroup));
	SetEnable(true);

	return pDynamicActorNodesGroup;
}

MPxTcDynamicActorNodesGroup* MPxDynamicActorNodeTC::GetDynamicActorNodesGroup( int nGroupId )
{
	DYNAMIC_ACTORNODES_MAP::iterator itr = m_mapDynamicActorNodesGroup.find(nGroupId);
	if ( itr != m_mapDynamicActorNodesGroup.end())
	{
		return itr->second;
	}

	return NULL;
}

void MPxDynamicActorNodeTC::GrabDynamicActorShape()
{
	RActorNode* pFirstBPartsActorNode = GetActor()->GetActorNode("b_parts_01");
	if (NULL == pFirstBPartsActorNode)
		return;

	std::vector< std::string > vAllParts;
	MakeAndPrepareSimulationActorNodes( pFirstBPartsActorNode, vAllParts, false );

	std::vector< std::string > vSubParts;
	MakeAndPrepareSimulationActorNodes( GetActor()->GetActorNode("b_parts_02"), vSubParts, false );
	vAllParts.insert(vAllParts.end(), vSubParts.begin(), vSubParts.end());

	vSubParts.clear();
	MakeAndPrepareSimulationActorNodes( GetActor()->GetActorNode("b_parts_03"), vSubParts, false );
	vAllParts.insert(vAllParts.end(), vSubParts.begin(), vSubParts.end());

	vSubParts.clear();
	MakeAndPrepareSimulationActorNodes( GetActor()->GetActorNode("b_parts_04"), vSubParts, false );
	vAllParts.insert(vAllParts.end(), vSubParts.begin(), vSubParts.end());

	m_pDynamicBodyEntityForShape->ResetDynamicBodyEntity(GetActor(), vAllParts, NULL);
	m_pDynamicBodyEntityForShape->GrabSubActorShape();

	//mlog("Success Grab PhysXShape\n");
}

void MPxDynamicActorNodeTC::ReleaseDynamicActorShape()
{
	m_pDynamicBodyEntityForShape->ReleaseSubActorShape();
}

void MPxDynamicActorNodeTC::ClearForReload()
{
	ReleaseDynamicActorShape();

	m_pDynamicBodyEntityForShape->ClearHandles();

}	

std::string MPxDynamicActorNodeTC::GetFragActorNodeName( const char* chActorNodeName, int nIndex )
{
	char chNodeName[257] = {0,};
	sprintf_s(chNodeName, "%s%s%02d", chActorNodeName, "_frag_", nIndex);

	return chNodeName;
}

}
