#include "stdafx.h"
#include "XBPartActorManager.h"
#include "XGameState.h"
#include "XModuleEntity.h"
#include "XNonPlayer.h"
#include "XBPartNonPlayer.h"

#include "MPhysX4R3/MPxDynamicActorNodeTC.h"

#include "RSceneManager.h"
#include "RDebugPassRenderer.h"

XBPartActorManager::XBPartActorManager()
: m_pOwner(NULL)
{
}

XBPartActorManager::~XBPartActorManager()
{

}

void XBPartActorManager::Init( XObject* pObject )
{
	m_pOwner = pObject;
}

void XBPartActorManager::Update( float fDelta )
{
	XModuleEntity * pModuleEntity = m_pOwner->GetModuleEntity();
	if(pModuleEntity == NULL)
		return;

	XCharacter* pActor = pModuleEntity->GetActor();
	if(pActor == NULL)
		return;

	UpdateNewBPartNpc(fDelta);

	if(pActor->GetUsable())
	{
		MPxDynamicActorNodeTC* pDynamicActorNodeTC = pModuleEntity->GetTransformController<MPxDynamicActorNodeTC>();
		if ( pDynamicActorNodeTC != NULL)
		{
			set<int>::iterator it = m_setIndex.begin();
			while(it != m_setIndex.end())
			{
				MPxTcDynamicActorNodesGroup* pDynamicActorNodesGroup = static_cast<MPxTcDynamicActorNodesGroup*>(pDynamicActorNodeTC->GetDynamicActorNodesGroup(*it));
				if ( pDynamicActorNodesGroup == NULL)
				{
					it = m_setIndex.erase(it);
					continue;
				}

				vector<string> vecEndList;
				pDynamicActorNodesGroup->CheckBPartPhysicSimulation(vecEndList);

				// BPart NPC 생성
				for(vector<string>::iterator itNew = vecEndList.begin(); itNew !=vecEndList.end(); ++itNew)
				{
					MUID uidDel = CreateBPartNpc(*itNew, pActor, *it);
					if(uidDel != MUID::ZERO)
					{
						stDelBPart delSave;
						delSave.m_uidNPC = uidDel;
						delSave.m_strDelActorNodeName = *itNew;
						m_vecDel.push_back(delSave);
					}
				}

				++it;
			}
		}
	}
}

void XBPartActorManager::GetBPartActorNodeList( rs3::RActorNode* _pParentActorNode, TD_USABLE_BPART* pUsableBpart, int nUsableBpartCount, std::vector<std::string>& _rBPartActorNodeInfo, std::vector<std::string>& _rFragActorNodes )
{
	if(pUsableBpart == NULL || _pParentActorNode == NULL)
		return;

	XModuleEntity * pModuleEntity = m_pOwner->GetModuleEntity();
	if(pModuleEntity == NULL)
		return;

	XCharacter* pActor = pModuleEntity->GetActor();
	if(pActor == NULL)
		return;

	const char* _pSzActorNodeName = _pParentActorNode->GetNodeName().c_str();

	for(int i = 0; i < nUsableBpartCount; ++i)
	{
		TD_USABLE_BPART* pBPartInfo = &pUsableBpart[i];

		std::string strActorNodeName;

		if(pBPartInfo->nFlag == -1)
		{
			// 혹시 flag node가 하나뿐입니까?
			if(_rFragActorNodes.size() == 1 &&
				_rFragActorNodes[0] == physx::MPxDynamicActorNodeTC::GetFragActorNodeName(_pSzActorNodeName, 0))
			{
				// 첫번째면 0번이다
				pBPartInfo->nFlag = 0;
				strActorNodeName = _rFragActorNodes[0];
			}
			else
				strActorNodeName = _pParentActorNode->GetNodeName();
		}
		else
		{
			strActorNodeName = physx::MPxDynamicActorNodeTC::GetFragActorNodeName(_pSzActorNodeName, pBPartInfo->nFlag);
			
			RActorNode* pActorNode = pActor->GetActorNode(strActorNodeName.c_str());
			if(pActorNode == NULL)
			{
				strActorNodeName = _pParentActorNode->GetNodeName();
				pBPartInfo->nFlag = -1;
			}
		}

		//BPART_ACTOR_NODE_INFO actorNodeInfo;
		//actorNodeInfo.strActorNodeName = strActorNodeName;
		//actorNodeInfo.vTargetPos = pBPartInfo->vPos;
		//actorNodeInfo.vTargetPos.z = gg.currentgamestate->GetWorld()->GetGroundZ(pBPartInfo->vPos);
		_rBPartActorNodeInfo.push_back(strActorNodeName);

		if(CheckActorNodeNameDuplication(strActorNodeName) == false)
		{
			stBPartActorNodeInfo stActorNodeInfo;
			stActorNodeInfo.setData(pBPartInfo, strActorNodeName);
			//stActorNodeInfo.setEffectTrace(pActor);

			m_vecBPartActorNodeInfo.push_back(stActorNodeInfo);
		}
	}
}

void XBPartActorManager::RemoveSimulationActorNodes( std::vector<std::string>& _rFragActorNodes, std::vector<BPART_ACTOR_NODE_INFO>& _rBPartActorNodeInfo )
{
	std::vector<std::string>::iterator it = _rFragActorNodes.begin();
	while(it != _rFragActorNodes.end())
	{
		bool bDuplication = false;
		for(std::vector<BPART_ACTOR_NODE_INFO>::iterator itBPart = _rBPartActorNodeInfo.begin(); itBPart != _rBPartActorNodeInfo.end(); ++itBPart)
		{
			if(*it == itBPart->strActorNodeName)
			{
				bDuplication = true;
			}
		}

		if(bDuplication)
		{
			it = _rFragActorNodes.erase(it);
			continue;;
		}

		++it;
	}
}

void XBPartActorManager::AddIndex( int nIndex )
{
	m_setIndex.insert(nIndex);
}

MUID XBPartActorManager::CreateBPartNpc( string& strActorNodeName, XCharacter* pActor, int nBPartID )
{
	XNonPlayer* pNpc = m_pOwner->AsNPC();

	for(vector<stBPartActorNodeInfo>::iterator it = m_vecBPartActorNodeInfo.begin(); it != m_vecBPartActorNodeInfo.end(); ++it)
	{
		if(it->strActorNodeName == strActorNodeName)
		{
			RActorNode* pActorNode = pActor->GetActorNode(strActorNodeName.c_str());
			if(pActorNode == NULL)
				return true;

			MMatrix matLocal = MMatrix::IDENTITY;
			pActorNode->GetBaseLocalTransform().GetInverse(&matLocal);
			MMatrix matWorld = matLocal * pActorNode->GetWorldTransform();
			MMatrix matActorNode = pActorNode->GetWorldTransform();

			// 바운드 박스 센터가 중간에 빠져있다면...
			float fGapZ = CheckBoundBoxFromGroundZ(matActorNode, pActorNode->m_pMeshNode->m_boundingBox);
			if(fGapZ > 0)
			{
				matWorld._43 += fGapZ;
			}

			//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis( matWorld, 6, 60.f);
			//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis( pActorNode->GetWorldTransform(), 5, 60.f, 0xffffff00);


			vec3 vBpartPos = matWorld.GetTranslation();
			float fGroundZ = gg.currentgamestate->GetWorld()->GetGroundZ(vBpartPos);
			vBpartPos.z = fGroundZ;

			TD_UPDATE_CACHE_PBART tempData;
			tempData.nNPCID = pNpc->GetInfo()->nID;
			tempData.uid = it->tdBPartInfo.uid;
			tempData.nUIID = it->tdBPartInfo.uiid;
			tempData.nBPartID = nBPartID;
			tempData.nBPartFlag = it->tdBPartInfo.nFlag;
			tempData.vPos = vBpartPos;
			if(it->bLootable)
				SetBitSet(tempData.nStatusFlag, UNS_LOOTABLE);
			SetBitSet(tempData.nStatusFlag, UNS_DEAD);

			gg.currentgamestate->InBPartNPC(&tempData, matWorld, it->tdBPartInfo.vPos);

			//it->effectTrace.Destroy();
			m_vecBPartActorNodeInfo.erase(it);
			return tempData.uid;
		}
	}

	return MUID::ZERO;
}

void XBPartActorManager::UpdateNewBPartNpc(float fDelta)
{
	// NPC 로딩이 완료가 되면 그때... 
	// BPart 삭제
	// 그래야 어색하지 않게 모델이 이어간다.
	std::vector<stDelBPart>::iterator it = m_vecDel.begin();
	while(it != m_vecDel.end())
	{
		XBPartNonPlayer* pBPartNpc = gg.omgr->FindBPartNPC(it->m_uidNPC);
		if(pBPartNpc && pBPartNpc->GetModuleEntity()->IsLoadingCompleted())
		{
			rs3::RActorNode* pActorNode = m_pOwner->GetActor()->GetActorNode(it->m_strDelActorNodeName.c_str());
			if(pActorNode)
			{
				// 그냥 사라지게만 한다. 나중에 알아서 삭제 할 것이다.
				pActorNode->SetVisible(false);
			}

			it = m_vecDel.erase(it);
			continue;
		}

		++it;
	}
}

bool XBPartActorManager::CheckActorNodeNameDuplication( string strActorNodeName )
{
	for(vector<stBPartActorNodeInfo>::iterator it = m_vecBPartActorNodeInfo.begin(); it != m_vecBPartActorNodeInfo.end(); ++it)
	{
		if(it->strActorNodeName == strActorNodeName)
			return true;
	}

	return false;
}

float XBPartActorManager::CheckBoundBoxFromGroundZ( MMatrix& matWorld, RBoundingBox& boundingBox )
{
	vec3 vCenter = boundingBox.GetCenter();
	matWorld.TransformVect(boundingBox.GetCenter(), vCenter);

	//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis( matWorld, 6, 60.f, 0xffff0000);
	//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( matWorld, boundingBox, 60.f, 0xffffff00);

	//MMatrix matTEst(MMatrix::IDENTITY);
	//matTEst.SetLocalMatrix(vCenter, vec3::AXISY, vec3::AXISZ);
	//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis( matTEst, 6, 60.f);


	float fGroundZ = gg.currentgamestate->GetWorld()->GetGroundZ(vCenter);

	if(vCenter.z < fGroundZ)
		return fGroundZ - vCenter.z;

	return 0.0f;
}

void XBPartActorManager::SetLootableBPartNPC( MUID uid, bool bLootable )
{
	for(vector<stBPartActorNodeInfo>::iterator it = m_vecBPartActorNodeInfo.begin(); it != m_vecBPartActorNodeInfo.end(); ++it)
	{
		if(it->tdBPartInfo.uid == uid)
		{
			it->bLootable = bLootable;
		}
	}
}