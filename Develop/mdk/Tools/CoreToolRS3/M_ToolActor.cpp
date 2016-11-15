#include "stdafx.h"
#include <assert.h>
#include "M_ToolActor.h"
#include "C_XMLEditInfoFile.h"
#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperRender.h"
#include "C_ControlMediatorHelperAction.h"

#include "RCollisionTreeBuilder.h"
#include "RMaterialResource.h"
#include "RMaterialResourceMgr.h"
#include "RMeshNode.h"
#include "RMesh.h"


namespace rs3 {

RToolActor::RToolActor(CControlMediator* pControlMediator) : m_bEluInstanceType(true)
, m_pControlMediator( pControlMediator )
{
}

RToolActor::~RToolActor()
{
}

int RToolActor::GetPartsNum()
{
	return GetActorNodeCount();
}


void RToolActor::SetCullState( bool _bSet )
{
	DeleteLODContext();
}


void RToolActor::GetPartsMatrix(int nIndex, RMatrix *pMat)
{
	assert(nIndex<GetActorNodeCount());
	assert(nIndex>=0);

	//RActorNode *pActorNode = &m_pScene[nIndex];
	RActorNode *pActorNode = m_actorNodes[nIndex];
	if ( pMat )
		*pMat = pActorNode->GetWorldTransform();
}

void RToolActor::DrawParts(int nIndex)
{
	assert(nIndex<GetActorNodeCount());
	assert(nIndex>=0);

	RActorNode *pActorNode = m_actorNodes[nIndex];
	//RActorNode *pActorNode = &m_pScene[nIndex];
	if ( pActorNode == NULL )
		return;
	if ( pActorNode->m_pMeshNode->isPhysique())
		return;
	if( !pActorNode->GetVisible() )
		return;

	RMeshVertexBuffer *pVB = pActorNode->m_pMeshNode->m_pVb;
	RMeshIndexBuffer *pIB = pActorNode->m_pMeshNode->m_pIb;

	if ( pVB == NULL )
		return;
	if ( pIB == NULL )
		return;

	pVB->SetVertexBuffer();
	pIB->SetIndices();

	for ( int index = 0; index < pActorNode->m_pMeshNode->m_nMtrlCnt; ++index )
	{
		RMaterial* pMtrl = pActorNode->m_pMeshNode->GetMtrl(index);
		if ( pMtrl == NULL )	continue;
		if ( pActorNode->m_pMeshNode->m_pMtrlTableInfo == NULL )	continue;

		pVB->RenderIndexBuffer(
			pActorNode->m_pMeshNode->m_pMtrlTableInfo[index].offset,
			pActorNode->m_pMeshNode->m_pMtrlTableInfo[index].count,
			pActorNode->m_pMeshNode->m_pIb->m_PrimitiveType);
	}
}

RSceneNode* RToolActor::Clone() 	// 복사 생성
{
	RToolActor* pToNode = new RToolActor(m_pControlMediator);

	bool bRet = m_pControlMediator->GetToolSceneManager()->LoadSceneNodeFromFile(pToNode, GetMeshName().c_str() );
	if ( !bRet )
	{
		delete pToNode;
		return NULL;
	}

	for ( RSceneNodeList::iterator itr = m_children.begin(); itr != m_children.end(); ++itr )
	{
		RSceneNode* pChild = *itr;
		if ( _stricmp( pChild->GetNodeIDString(), RSID_LIGHTSCENENODE ) == 0 )
		{
			RSceneNode* pLightSceneNode = pChild->Clone();
			if (pChild->QueryAttribute(RSNA_DO_NOT_SAVE))
				pLightSceneNode->AddAttribute(RSNA_DO_NOT_SAVE);
			else
				pLightSceneNode->RemoveAttribute(RSNA_DO_NOT_SAVE);

			pToNode->AddChild( pLightSceneNode );
			pToNode->AddToSceneNodeGroup(pLightSceneNode);

			// UpdateTransform시에 포인트라이트가 관리자에 더해진다.
//			pLightSceneNode->UpdateTransform();
//			pLightSceneNode->Update();
			RSceneNodeUpdateVisitor visitor;
			pLightSceneNode->Traverse(&visitor);
		}
	}

//	pToNode->SetLightMapPath( REngine::GetSceneManager().GetXMLFilePath() );

	pToNode->SetTransform( GetLocalTransform() );
	return pToNode;
}

void RToolActor::CreateDeviceDependentPart(RMesh* pMesh)
{
	RActor::CreateDeviceDependentPart(pMesh);
}

void RToolActor::SetPointLightName(const char* _pSzDelim)
{
	std::vector< RSceneNode* > vPointLight;
	SerializeToVector(SPointLightSelector::Selector, vPointLight );

	char szBuffer[256];
	szBuffer[0] = '\0';
	for ( std::vector<RSceneNode*>::iterator itr = vPointLight.begin(); itr != vPointLight.end(); ++itr )
	{
		sprintf(szBuffer, "%s", (*itr)->GetNodeName().c_str());
		std::string pureStr( strtok(szBuffer,"//") );
		sprintf(szBuffer, "%s%s%s", pureStr.c_str(), _pSzDelim, m_strNodeName.c_str());
		(*itr)->SetNodeName(szBuffer);
	}
}

void RToolActor::SetCreationType(bool bEluInstance)
{
	m_bEluInstanceType = bEluInstance;

	if (m_bEluInstanceType)
	{
		for (RSceneNodeList::iterator itr = m_children.begin(); itr != m_children.end(); ++itr )
		{
			(*itr)->AddAttribute(RSNA_DO_NOT_SAVE);
		}
		SetPointLightName("//");
	} 
	else
	{
		for (RSceneNodeList::iterator itr = m_children.begin(); itr != m_children.end(); ++itr )
		{
			(*itr)->RemoveAttribute(RSNA_DO_NOT_SAVE);
		}
		SetPointLightName("_");
	}
}

void RToolActor::OnAdd()
{
	RActor::OnAdd();

	// 붙을때는 임시적으로 가지고 있는 이팩트를 초기화한다.
	m_vecEffectSceneNode.clear();
}
void RToolActor::OnRemove()
{
	RActor::OnRemove();

	// delete로 지워질때 이팩트도 같이 지워져야 한다.
	// 그래서 부모로부터 떨어질때 이팩트를 선택적으로 가지고 있게 한다.
	SerializeToVector( EffectSelector::Selector, m_vecEffectSceneNode );
}

void RToolActor::SetTexColor(DWORD _dwColor)
{
	size_t nSize = m_actorNodes.size();
	for (size_t i = 0; i < nSize; i++)
	{
		m_actorNodes[i]->SetPartsColor(_dwColor);
	}
}

void RToolActor::CollectStaticCollisionPolygons(RSceneNodePolygonCollector* pCollector) const
{
	if(!GetCollisionCreationEnable())
		return;

	if(m_pMesh->GetFileVersion() < EXPORTER_MESH_VER12)
	{
	//	string strMessage = string(m_pMesh->GetName()) + "이전파일입니다. 익스포터버그로 충돌정보를 만들수 없습니다. 다시 익스포트 부탁해요.";
		pCollector->HandleError(m_pMesh->GetName());
	//	MessageBox(NULL,strMessage.c_str(),"I'm sorry.",MB_OK);
		return;
	}

	DWORD dwAttribute = RCollisionTriangle::MESH;

	// 실제 메시 정보를 pTree
	for(int m=0;m<GetActorNodeCount();m++)
	{
		RActorNode* pActorNode = const_cast< RToolActor* >(this)->GetActorNodes()[m];
		//if(!pActorNode->GetVisible())
		//	bVisible = false;

		RMeshNode* pMeshNode = pActorNode->m_pMeshNode;
		// 보이는건 상관없이 충돌 속성이 있으면 출력해준다.
		if( ( pMeshNode->FindUserProperty("nocollision")) || ( false == pMeshNode->isCollisionMesh() ) )
			continue;
	
		RMatrix matWorld = pMeshNode->m_matBase * GetWorldTransform();
		int nBaseVertex = 0;
		for(int i=0;i<pMeshNode->m_nFaceCnt;i++)
		{
			RMeshPolygonData* pPolygonData = &pMeshNode->m_pPolygonTable[i];

			// 마테리얼이 없는 메시는 충돌 메시로 사용하지 않는다.
			const RMaterialResource* pMaterialResource = m_pMesh->GetMaterialResource(pPolygonData->mtrl_id);
			if( pMaterialResource == NULL )
				continue;

			RMaterial* pMaterial = pMaterialResource->m_pMaterial;
			if(pMaterial->CheckFlag(RM_FLAG_ADDITIVE)) 
				continue;

			DWORD nZoneMaterialID = pCollector->GetZoneMaterialID(pMaterial->GetName());

			for(int j=0;j<(pPolygonData->nVertices-2);j++)
			{
				RVector pos[3];
				pos[0] = pMeshNode->m_pPointTable[pPolygonData->index[0].p] * matWorld;
				pos[1] = pMeshNode->m_pPointTable[pPolygonData->index[j+1].p] * matWorld;
				pos[2] = pMeshNode->m_pPointTable[pPolygonData->index[j+2].p] * matWorld;

				pCollector->OnAddPolygon(pos[0],pos[1],pos[2], dwAttribute, 0, nZoneMaterialID);
			}
			nBaseVertex += pPolygonData->nVertices;
		}
	}
}

}