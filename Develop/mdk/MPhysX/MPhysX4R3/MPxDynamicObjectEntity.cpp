#include "stdafx.h"
#include "MPxDynamicObjectEntity.h"
#include "MPhysX.h"

#include "RSceneNode.h"
#include "RPhysXActorController.h"
#include "RPhysXDynamicActorNodeBuilder.h"

#include "RActor.h"
#include "RAnimationControllerNode.h"
#include "RMesh.h"
#include "RMeshNode.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RDebugPassRenderer.h"

//////////////////////////////////////////////////////////////////////////

namespace physx
{

using namespace rs3;

MPxDynamicObjectEntity::MPxDynamicObjectEntity(MPxTcEntity* _pPxTcEntity)
: m_strCustomReferenceNodePostfix("_effect")
{
	m_pPhysXActorController = NULL;
	m_pActor = NULL;
	m_nActorNodesDeActive = 0;
	m_ActorNodeInfos.m_fFadeOutFactor = 10.f;

	m_bKinematicMode = false;
	m_pPxTcEntity = _pPxTcEntity;
}

void MPxDynamicObjectEntity::ResetDynamicBodyEntity( RActor* pActor, const std::vector<std::string>& _rActorNodeName, const char* _pSzDeletionChunck )
{
	_ASSERT( m_pActor == NULL );
	m_pActor = pActor;

	_ASSERT( m_ActorNodeInfos.m_vecActorNodesList.empty() );
	m_ActorNodeInfos.m_vecActorNodesList = _rActorNodeName;

	_ASSERT( m_vecNxInfoHandles.empty() );
	m_vecNxInfoHandles.resize(m_ActorNodeInfos.GetActorNodesCount<size_t>());

	if (_pSzDeletionChunck)
		m_ActorNodeInfos.m_strDeletionChunck = _pSzDeletionChunck;
}

int MPxDynamicObjectEntity::GetSubActorCount()
{
	return m_ActorNodeInfos.GetActorNodesCount<int>();
}

bool MPxDynamicObjectEntity::GetSubActorInfo( int _nIndex, std::string& _rShapeName, std::vector<NxVec3>& _refVertices, std::vector<NxU32>& _refFaces, MMatrix& _rMatLocal, bool& _rIsKinematic )
{
	_ASSERT(m_pActor);
	const char* pSzActorNodeName = m_ActorNodeInfos.GetActorNodeName(_nIndex);
	RActorNode* pActorNode = m_pActor->GetActorNode(pSzActorNodeName);
	if (NULL == pActorNode || NULL == pActorNode->m_pMeshNode)
		return false;

	// check reference-countable shape
	const RVector3& vActorNodeScale = m_pActor->GetScale() * pActorNode->GetScale();
	char szShapeName[1028];
	sprintf_s(szShapeName, "%s%s%.2f", m_pActor->GetNodeName().c_str(), pSzActorNodeName, vActorNodeScale.x );
	_rShapeName = szShapeName;
	if (NULL == MPhysX::GetInstance()->FindPxShape(_rShapeName))
	{
		RMeshNode* pMeshNode = pActorNode->m_pMeshNode;
		//mlog("이름 %s(%s) v %d f %d\n", pSzActorNodeName, _rShapeName.c_str(), pMeshNode->m_nVertexIndexTableCnt, pMeshNode->m_nFaceIndexTableCnt);
		for(int i = 0; i < pMeshNode->m_nVertexIndexTableCnt; ++i )
		{
			int nPosIndex = pMeshNode->m_pVertexIndexTable[i].p;
			RVector3 vRet = pMeshNode->m_pPointTable[nPosIndex] * vActorNodeScale;
			_refVertices.push_back( NxVec3( vRet.x, vRet.y, vRet.z ) );
		}

		if (_refVertices.empty())
		{
			mlog("bpart(%s) _refVertices are empty\n", pSzActorNodeName);
			return false;
		}

		for (int i = 0; i < pMeshNode->m_nFaceIndexTableCnt; ++i)
		{
			NxU32 f = (NxU32)pMeshNode->m_pFaceIndexTable[i];
			_refFaces.push_back(f);
		}

		if (_refFaces.empty())
		{
			mlog("bpart(%s) _refFaces are empty\n", pSzActorNodeName);
			return false;
		}
	}

	// for local tm
	m_pActor->UpdateTransform();
	RActorNode* pStartRefNode = pActorNode;
	if (!m_strCustomReferenceNodePostfix.empty())
	{
		RActorNode* pParent = pActorNode->GetParentActorNode();
		if (pParent)
		{
			pStartRefNode = m_pActor->GetActorNode( (pParent->GetNodeName() + m_strCustomReferenceNodePostfix).c_str() );
			if (NULL == pStartRefNode)
				pStartRefNode = pActorNode;
		}
	}
	DWORD nFrame = m_pActor->GetAnimationController().GetFrame();
	pStartRefNode->GetResultTransformAt(nFrame, _rMatLocal);
	//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox(_rMatLocal * m_pActor->GetWorldTransform(), 10, 50, D3DCOLOR_XRGB(0, 255, 255));

	// for kinematic
	_rIsKinematic = m_bKinematicMode;

	return true;
}

void MPxDynamicObjectEntity::OnActivatedSubActorsAll( MMatrix& _rOutEntityTM )
{
	_ASSERT(m_pActor);

	// Controller
	_ASSERT( m_pPhysXActorController == NULL );
	m_pPhysXActorController = new RPhysXActorController(MPhysX::GetInstance()->GetNxScene(), m_bKinematicMode? RSNCTRL_POSTUPDATE : RSNCTRL_CUSTOMUPDATE);
	m_pActor->AddController(m_pPhysXActorController);

	// mass info
	std::set< std::pair<float, std::string> > massSortedActorNodes;

	// builder
	int nSize = (int)m_vecNxInfoHandles.size();
	_ASSERT( m_vecNxInfoHandles.size() == m_ActorNodeInfos.GetActorNodesCount<size_t>() );
	RPhysXDynamicActorNodeBuilder builder;
	for (int i = 0; i < nSize; ++i)
	{
		//////////////////////////////////////////////////////////////////////////
		//
		// 컨트롤러가 리소스를 가지지 않으므로 직접 여기서 NxActor관련 설정을 해준다.
		//
		//////////////////////////////////////////////////////////////////////////

		NxActor* pNxActor = GetNxActorFromHandle(m_vecNxInfoHandles[i]);
		_ASSERT(pNxActor);
		//pNxActor->setName(m_ActorNodeInfos.m_vecActorNodesList[i].c_str());

		// 키네마틱 모드가 아닐때
		if (!m_bKinematicMode)
		{
			// Random Mass
			pNxActor->updateMassFromShapes( MMath::RandomNumber(0.8f, 1.3f), 0 );

			float fActorMass = pNxActor->getMass();
			massSortedActorNodes.insert( std::pair<float, std::string>(fActorMass, m_ActorNodeInfos.m_vecActorNodesList[i]) );

			// 충돌
			int nCollisionGroup = GetCollisionGroup();
			if ( nCollisionGroup > 0)
			{
				// 충돌통지 리스너
				pNxActor->userData = (void*)this;

				// Actor 그룹설정
				pNxActor->setGroup(nCollisionGroup);

				// shape 그룹설정
				NxU32 nShapes = pNxActor->getNbShapes();
				if(nShapes > 0)
				{
					NxShape*const* shapes = pNxActor->getShapes();
					while( nShapes-- )
					{
						shapes[nShapes]->setGroup(nCollisionGroup);
					}
				}
			}

			// 충돌안함
			//if (m_ActorNodeInfos.m_vecActorNodesList[i].find("_im") != std::string::npos)
			//	pNxActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
		}

		// 빌더에 더하기
		builder.AddActorNodeInfo(new RPhysXDynamicActorNodeInfo( m_ActorNodeInfos.m_vecActorNodesList[i], pNxActor ) );
	}

	// set mass rank from mass info
	int nRank = 1;
	for(std::set< std::pair<float, std::string> >::reverse_iterator ritr = massSortedActorNodes.rbegin();
		ritr != massSortedActorNodes.rend(); ++ritr)
	{
		m_ActorNodeInfos.m_mapActorNodeMassRank.insert( std::map< std::string, int >::value_type(ritr->second, nRank) );
		++nRank;
	}

	// building
	m_pPhysXActorController->BuildNxObject(&builder);

	// world transform
	_rOutEntityTM = m_pActor->GetWorldTransform();
}

void MPxDynamicObjectEntity::OnDeActivatedSubActorsAll()
{
	_ASSERT(m_pActor);
	if (m_pPhysXActorController)
	{
		m_pActor->RemoveController(m_pPhysXActorController);
		delete m_pPhysXActorController;
		m_pPhysXActorController = NULL;
	}
}

const MBox& MPxDynamicObjectEntity::GetAreaBox()
{
	_ASSERT(m_pActor);

	//제대로된 AABB 얻어오기위해서
	m_pActor->UpdateBoundingVolume();
	return m_pActor->GetAABB();
}

void MPxDynamicObjectEntity::OnSubActorAdded( int _nSubActorIndex, physx::MSUBACTORHANDLE _hAddedSubActorHandle )
{
	m_vecNxInfoHandles[_nSubActorIndex] = _hAddedSubActorHandle;
}

void MPxDynamicObjectEntity::OnSubActorSimulated( NxActor* pNxActor )
{
	_ASSERT(m_pActor);
	_ASSERT(m_pPhysXActorController);

	// 현재는 SubActor simulation 때 fade out 만 확인한다.
	if (false == m_ActorNodeInfos.IsFadeOutEnabled())
		return;

	// NxActor와 연결되어있는 액터노드 구하기
	RActorNode* pActorNode = m_pPhysXActorController->GetActorNodeFromUpdateMap(pNxActor);
	if (NULL == pActorNode)
		return;

	// 물리 상태로 멈춤상태인지 확인하기
	bool bIsStopState = IsStopSubActorSimulated(pNxActor);

	// fade out 이 시작되었다면, 멈춤상태로 진행하기
	if (false == bIsStopState)
	{
		bIsStopState = m_ActorNodeInfos.IsFadeOutStartedNode(pActorNode->GetNodeName());
		if(false == bIsStopState)
			return;
	}

	// fade out 상태에 돌입되면 충돌 시작 이벤트 수신을 하지 않는다.
	pNxActor->userData = NULL;

	// fade out 을 위한 delta 구하기
	float fDelta = MPhysX::GetInstance()->GetCurrentElapsed();

	// 씬노드의 Visible과 Visibility를 설정했으니, 그것에 대한값을 얻는다.
	if ( pActorNode->RSceneNode::GetVisible() == true && m_ActorNodeInfos.CheckFadeOutStart( pActorNode->GetNodeName(), fDelta ) )
	{
		float fVisibility = pActorNode->GetVisibility();
		float fNextVisibility = fVisibility - fDelta * 0.26f;
		if (fNextVisibility > 0.f)
		{
			pActorNode->SetVisibility(fNextVisibility);
		}
		else
		{
			pActorNode->SetVisible(false);
			++m_nActorNodesDeActive;

			// 다이나믹 속성 꺼주기
			pNxActor->raiseBodyFlag(NX_BF_KINEMATIC);
		}
	}
}

void MPxDynamicObjectEntity::OnSubActorCollisionStarted( NxActor* pNxActor )
{
	_ASSERT(m_pActor);
	_ASSERT(m_pPhysXActorController);

	if (m_pPxTcEntity)
		m_pPxTcEntity->OnDynamicObjectCollided(pNxActor, this);
}

void MPxDynamicObjectEntity::DeleteActorNodes()
{
	_ASSERT(m_pActor != NULL);

	std::set<RActorNode*> setActorNodes;
	int nSize = m_ActorNodeInfos.GetActorNodesCount<int>();
	for (int i = 0; i < nSize; ++i)
	{
		RActorNode* pActorNode = m_pActor->GetActorNode( m_ActorNodeInfos.GetActorNodeName(i) );
		if (pActorNode == NULL)
			continue;

		RActorNode* pParent = pActorNode->GetParentActorNode();
		if (pParent && pParent->GetNodeName() == m_ActorNodeInfos.m_strDeletionChunck)
		{
			setActorNodes.insert(pParent);
		}
		else
		{
			setActorNodes.insert(pActorNode);
		}
	}

	for (std::set<RActorNode*>::iterator itr = setActorNodes.begin(); itr != setActorNodes.end(); ++itr)
	{
		m_pActor->DeleteActorNode((*itr)->GetNodeName().c_str());
	}

	m_nActorNodesDeActive = 0;
}

void MPxDynamicObjectEntity::SetKinematicMode( bool bKinematic, MPxObjDynamic* pDynamic )
{
	m_bKinematicMode = bKinematic;

	pDynamic->SetEnableObservingResultOnSimulated(!m_bKinematicMode);
	pDynamic->SetEnableUsingAreaBox(!m_bKinematicMode);
}

int MPxDynamicObjectEntity::GetMassRank( const std::string& _rActorNodeName )
{
	std::map< std::string, int >::iterator itr = m_ActorNodeInfos.m_mapActorNodeMassRank.find(_rActorNodeName);
	if (itr == m_ActorNodeInfos.m_mapActorNodeMassRank.end())
		return -1;

	return itr->second;
}

void MPxDynamicObjectEntity::OnSimulated()
{
	if (m_pPhysXActorController)
	{
		m_pPhysXActorController->Update(NULL);
	}
}

void MPxDynamicObjectEntity::GrabSubActorShape()
{
	int nShapeCount = GetSubActorCount();
	if (nShapeCount < 1)
		return;

	std::vector<NxVec3> vertices;
	std::vector<NxU32> faces;
	MMatrix matLocal;
	bool bKinematic;

	std::string strName;
	for (int i = 0; i < nShapeCount; ++i)
	{
		if (GetSubActorInfo(i, strName, vertices, faces, matLocal, bKinematic))
		{
			//mlog("|||| %s\n", strName.c_str());
			MPhysXShape* pShape = MPhysX::GetInstance()->CreatePxShape(MPhysXShape::PSHT_CONVEX, strName);
			pShape->CreateConvexShape(vertices, faces);

			m_vecShapeNames.push_back(strName);

			vertices.clear();
			faces.clear();
		}
	}
}

void MPxDynamicObjectEntity::ReleaseSubActorShape()
{
	for (std::vector<std::string>::iterator itr = m_vecShapeNames.begin(); itr != m_vecShapeNames.end(); ++itr)
	{
		MPhysX::GetInstance()->RemovePxShape(*itr);
	}
}

vector<NxActor*> MPxDynamicObjectEntity::GetActors()
{
	vector<NxActor*> vecRet;

	for (size_t i = 0; i < m_vecNxInfoHandles.size(); ++i)
	{
		NxActor* pNxActor = GetNxActorFromHandle(m_vecNxInfoHandles[i]);
		if (!pNxActor)
			continue;

		vecRet.push_back(pNxActor);
	}

	return vecRet;
}

void MPxDynamicObjectEntity::CheckEndSubActorSimulate( std::vector<std::string>& vecEndList )
{
	if(m_pPhysXActorController->GetUpdateCount() <= 0)
		return;

	int nSize = (int)m_vecNxInfoHandles.size();
	_ASSERT( m_vecNxInfoHandles.size() == m_ActorNodeInfos.GetActorNodesCount<size_t>() );
	for (int i = 0; i < nSize; ++i)
	{
		NxActor* pNxActor = GetNxActorFromHandle(m_vecNxInfoHandles[i]);
		if(pNxActor == NULL)
			continue;

		if(IsStopSubActorSimulated(pNxActor))
		{
			RActorNode* pActorNode = m_pActor->GetActorNode( m_ActorNodeInfos.GetActorNodeName(i) );
			if (pActorNode == NULL ||
				pActorNode->GetVisible() == false ||
				pActorNode->GetVisibilityResult() == 0.0f)
				continue;

			vecEndList.push_back(m_ActorNodeInfos.m_vecActorNodesList[i]);
		}
	}
}

bool MPxDynamicObjectEntity::IsStopSubActorSimulated( NxActor* pNxActor )
{
	return
		(
		(pNxActor->getLinearVelocity().magnitudeSquared() < 10 && pNxActor->getAngularVelocity().magnitudeSquared() < 10 ) ||
		(pNxActor->getGlobalPosition().z < -100000.0f)
		);
}

}
