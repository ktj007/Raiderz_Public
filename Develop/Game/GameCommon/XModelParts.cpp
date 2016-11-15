#include "stdafx.h"
#include "XModelParts.h"
#include "XCharacterParts.h"
#include "XCharacter.h"
#include "XDef.h"
#include "MProfiler.h"

XPartInfo::XPartInfo() : m_nState(PART_STATE_INVALID)
{
	nPartsID = 0;
	pMesh = NULL;
}

void XPartInfo::AddNode( tstring& nodeName, tstring& parentName, tstring& actorNodeName, unsigned int nColor, RActorNode* actorNode/*=NULL*/ )
{
	XPartsNodeInfo partsNode;
	partsNode.SetActorNode(actorNode);
	partsNode.strNodeName = nodeName;
	partsNode.strParentName = parentName;
	partsNode.strActorNodeName = actorNodeName;
	partsNode.dwColor = nColor;

	if (partsNode.strActorNodeName.empty())
	{
		partsNode.strActorNodeName = partsNode.strNodeName;
	}

	m_vecNodes.push_back(partsNode);
}

void XPartInfo::Assign( XPartsParam* pPartsParam )
{
	this->nPartsID = pPartsParam->nPartsID;
	this->strMeshName = pPartsParam->strMeshName;

	for (size_t i = 0; i < pPartsParam->vecParts.size(); i++)
	{
		this->AddNode(pPartsParam->vecParts[i].strNodeName, 
					  pPartsParam->vecParts[i].strParentName, 
					  pPartsParam->vecParts[i].strActorNodeName,
					  pPartsParam->vecParts[i].dwPartsColor);
	}
}

bool XPartInfo::IsEqualTo( const XPartsParam* pPartsParam )
{
	if (pPartsParam->strMeshName != this->strMeshName)
	{
		return false;
	}

	for (size_t i = 0; i < this->m_vecNodes.size(); ++i)
	{
		if (pPartsParam->vecParts[i].strNodeName != this->m_vecNodes[i].strNodeName)
		{
			return false;
		}

		if (pPartsParam->vecParts[i].strParentName != this->m_vecNodes[i].strParentName)
		{
			return false;
		}

		if (pPartsParam->vecParts[i].dwPartsColor != this->m_vecNodes[i].dwColor)
		{
			return false;
		}

	}

	return true;
}

void XModelPartsInfo::Clear()
{
	for (ModelPartsMap::iterator itor = m_PartsMap.begin(); itor != m_PartsMap.end(); ++itor)
	{
		XPartInfo* pParts = (*itor).second;
		delete pParts;
	}
	m_PartsMap.clear();

	for (ModelPartsList::iterator itor = m_LoadingQueue.begin(); itor != m_LoadingQueue.end(); ++itor)
	{
		XPartInfo* pParts = (*itor);
		delete pParts;
	}
	m_LoadingQueue.clear();
}

void XModelPartsInfo::DeletePartInfo( int nPartsID )
{
	ModelPartsMap::iterator itor = m_PartsMap.find(nPartsID);
	if (itor == m_PartsMap.end()) return;

	delete (*itor).second;
	m_PartsMap.erase(itor);
}

void XModelPartsInfo::AddPartInfo( XPartInfo* pParts )
{
	m_PartsMap.insert(ModelPartsMap::value_type(pParts->nPartsID, pParts));
}

XPartInfo* XModelPartsInfo::Find( int nPartsID )
{
	ModelPartsMap::iterator itor = m_PartsMap.find(nPartsID);
	if (itor == m_PartsMap.end()) return NULL;
	return (*itor).second;
}

tstring XModelPartsInfo::GetReservedPartsMeshName( int nPartsID )
{
	for (ModelPartsList::iterator itor = m_LoadingQueue.begin(); itor != m_LoadingQueue.end(); ++itor)
	{
		XPartInfo* pPart = (*itor);
		if (nPartsID == pPart->nPartsID)
		{
			return pPart->strMeshName;
		}
	}

	return _T("");
}

void XModelPartsInfo::Reserve( XPartsParam* pPartsParam )
{
	const int nPartsID = pPartsParam->nPartsID;

	for (ModelPartsList::iterator itor = m_LoadingQueue.begin(); itor != m_LoadingQueue.end(); )
	{
		XPartInfo* pPartInQueue = (*itor);

		if (pPartInQueue->nPartsID == nPartsID)
		{
			delete pPartInQueue;
			itor = m_LoadingQueue.erase(itor);
		}
		else
		{
			++itor;
		}
	}

	XPartInfo* pPart = Find(nPartsID);
	if (pPart)
	{
		if (pPart->IsEqualTo(pPartsParam)) return;
	}

	XPartInfo* pNewPart = new XPartInfo();
	pNewPart->Assign(pPartsParam);
	pNewPart->SetState(PART_STATE_READY_TO_CREATE_MESH);

	m_LoadingQueue.push_back(pNewPart);
}

void XModelPartsInfo::UpdateQueue()
{
	PFC_THISFUNC;
	for (ModelPartsList::iterator itor = m_LoadingQueue.begin(); itor != m_LoadingQueue.end(); )
	{
		XPartInfo* pLoadingPart = (*itor);

		const int nPartsID = pLoadingPart->nPartsID;
		XPartInfo* pPart = Find(nPartsID);

		if (pLoadingPart->GetState() == PART_STATE_LOADING_COMPLETED)
		{
			DeletePartInfo(nPartsID);
			AddPartInfo(pLoadingPart);

			itor = m_LoadingQueue.erase(itor);
		}
		else if (pLoadingPart->GetState() == PART_STATE_LOADING_CANCELED)
		{
			delete pLoadingPart;
			itor = m_LoadingQueue.erase(itor);
		}
		else
		{
			++itor;
		}
	}
}

bool XModelPartsInfo::IsExistActorNode( RActorNode* pActorNode )
{
	for (ModelPartsMap::iterator itor = m_PartsMap.begin(); itor != m_PartsMap.end(); ++itor)
	{
		XPartInfo* pPart = (*itor).second;

		for (size_t i = 0; i < pPart->m_vecNodes.size(); i++)
		{
			if (pPart->m_vecNodes[i].pActorNode == pActorNode) 
			{
				return true;
				break;
			}
		}
	}
	return false;
}

bool XModelPartsInfo::IsLoadingQueueEmpty()
{
	return m_LoadingQueue.empty();
}

