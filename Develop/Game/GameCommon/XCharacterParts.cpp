#include "stdafx.h"
#include "XCharacterParts.h"
#include "XCharacter.h"
#include "XCoverMesh.h"
#include "XDef.h"
#include "RMeshNode.h"
#include "RSceneManager.h"
#include "RMeshMgr.h"
#include "MStatisticsProfiler.h"
#include "MProfiler.h"
#include "MLocale.h"

XCharacterParts::XCharacterParts( XCharacter* pCharacter ) : m_pCharacter(pCharacter)
{
	m_pPartsLoadingController = new XPartsLoadingController(this);
	m_pCoverMeshList = new XCoverMeshList(pCharacter);

	m_bUsingBackgroundLoad = true;
}

XCharacterParts::~XCharacterParts()
{
	ClearParts();

	SAFE_DELETE(m_pCoverMeshList);
	SAFE_DELETE(m_pPartsLoadingController);
}

void XCharacterParts::Destroy()
{
	ClearParts();
}

void XCharacterParts::ClearParts()
{
	PFI_BLOCK_THISFUNC(1200);

	for (XModelPartsInfo::ModelPartsMap::iterator itor = m_PartsInfo.GetParts().begin(); itor != m_PartsInfo.GetParts().end(); ++itor)
	{
		XPartInfo* pPart = (*itor).second;

		DeletePartsNodes(pPart);
	}

	m_PartsInfo.Clear();
}

void XCharacterParts::CreatePartsNodeAsync( XPartsParam* pPartsParam )
{
	PFI_BLOCK_THISFUNC(1201);

	m_PartsInfo.Reserve(pPartsParam);

	_ASSERT(!pPartsParam->vecParts.empty());
}


void XCharacterParts::CompletePartsLoading( XPartInfo* pPart )
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(1202);

	size_t nPartNodesCnt = pPart->m_vecNodes.size();
	for (size_t i = 0; i < nPartNodesCnt; i++)
	{
		RActorNode* pActorNode = 
			CreatePartsNode(pPart->pMesh, 
							NULL, 
							pPart->m_vecNodes[i].strNodeName, 
							pPart->m_vecNodes[i].strParentName, 
							pPart->m_vecNodes[i].strActorNodeName,
							(i+1 == nPartNodesCnt));	// 마지막에만 컨트롤러에게 notify해준다.

		//mlog("XCharacterParts::CompletePartsLoading - CreatePartsNode : %s\n", pPart->m_vecNodes[i].strActorNodeName.c_str());

		// 파츠노드 생성 실패
		if(pActorNode == NULL)
			continue;

		// 두 슬롯을 사용하지만 액터 노드는 하나인 경우도 있을 수 있다.
		bool bExistActorNode = m_PartsInfo.IsExistActorNode(pActorNode);
		if (!bExistActorNode) 
		{
			pPart->m_vecNodes[i].SetActorNode(pActorNode);
		}

		if (pPart->m_vecNodes[i].dwColor != 0xFFFFFFFF)
		{
			pActorNode->SetPartsColor(pPart->m_vecNodes[i].dwColor);
		}
	}


	pPart->SetState(PART_STATE_LOADING_COMPLETED);
}

void XCharacterParts::DeletePartsNodes( XPartInfo* pPart )
{
	PFI_BLOCK_THISFUNC(1203);

	//////////////////////////////////////////////////////////////////////////
	// 지우는 부분은 이런식으로 Builder를 사용
	RActor::RActorNodeBuilder Builder(m_pCharacter);

	///*
	for (size_t i = 0; i < pPart->m_vecNodes.size(); i++)
	{
		RActorNode* pActorNode = m_pCharacter->GetActorNode( MLocale::ConvTCHARToAnsi(pPart->m_vecNodes[i].strActorNodeName.c_str()).c_str() );

		if (pActorNode == NULL || pActorNode->m_pMeshNode == NULL) 
			continue;

		m_pCoverMeshList->SetCoverMesh(pActorNode->m_pMeshNode, false);

		Builder.DeleteActorNode(pActorNode);
		pPart->m_vecNodes[i].SetActorNode(NULL);
		
	}
	//*/
	
	Builder.Build();

	//mlog("XCharacterParts::DeletePartsNodes - %s\n", pPart->strMeshName.c_str());
	//////////////////////////////////////////////////////////////////////////

	if(pPart->pMesh)
	{
		ReleaseMesh( pPart->pMesh );
		pPart->pMesh = NULL;
	}
}

void XCharacterParts::DeleteParts( int nPartsID )
{
	PFI_BLOCK_THISFUNC(1204);

	XPartInfo* pPart = m_PartsInfo.Find(nPartsID);

	if (pPart)
	{
		DeletePartsNodes(pPart);
	}

	CancelFromLoadingQueue(nPartsID);
	m_PartsInfo.DeletePartInfo(nPartsID);
}

bool XCharacterParts::DeleteActorNode( const TCHAR* szActorNodeName, bool bNotifyToController )
{
	PFI_BLOCK_THISFUNC(1205);

	RActorNode* pActorNode = m_pCharacter->GetActorNode(MLocale::ConvTCHARToAnsi(szActorNodeName).c_str());

	if (pActorNode == NULL) 
	{
		return false;
	}
	if (pActorNode->m_pMeshNode == NULL) return false;

	RMesh* pMesh = pActorNode->m_pMeshNode->m_pBaseMesh;

	// 먼저 쓰던 메시노드의 가리기 규칙 취소
	m_pCoverMeshList->SetCoverMesh(pActorNode->m_pMeshNode, false);

	if (m_pCharacter->DeleteActorNode(MLocale::ConvTCHARToAnsi(szActorNodeName).c_str(), bNotifyToController))
	{
		//mlog("Parts:DeleteActorNode = %s\n", pActorNode->GetName().c_str());
	}

	return true;
}

tstring XCharacterParts::GetReservedSlotMeshName( int nPartsID )
{
	return m_PartsInfo.GetReservedPartsMeshName(nPartsID);
}

bool XCharacterParts::IsLoadingCompleted()
{
	PFI_BLOCK_THISFUNC(1206);

	if (m_PartsInfo.IsLoadingQueueEmpty() == false) return false;

	for (XModelPartsInfo::ModelPartsMap::iterator itor = m_PartsInfo.GetParts().begin(); itor != m_PartsInfo.GetParts().end(); ++itor)
	{
		XPartInfo* pPart = (*itor).second;

		if(pPart->pMesh && !pPart->pMesh->IsLoadingCompleted() )
			return false;
	}

	return true;
}

RActorNode* XCharacterParts::CreatePartsNode( RMesh* pMesh, const CSItemData* pItemData, tstring& strNodeName, tstring& strParentName, tstring& strActorNodeName, bool bNotifyToController)
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(1207);

	// 이미 있는 ActorNode 삭제
	DeleteActorNode(strActorNodeName.c_str(), false);

	RActorNode* retActorNode = NULL;

	// item mesh 설정
	RMeshNode* pMeshNode = pMesh->GetNode(MLocale::ConvTCHARToAnsi(strNodeName.c_str()).c_str());

	RActorNode* pParentNode = NULL;
	if (!strParentName.empty())
	{
		pParentNode = m_pCharacter->GetActorNode(MLocale::ConvTCHARToAnsi(strParentName.c_str()).c_str());
	}

	// 메쉬 추가
	if(pMeshNode)
	{
		retActorNode = m_pCharacter->AddMeshNode(pMeshNode, pParentNode, MLocale::ConvTCHARToAnsi(strActorNodeName.c_str()).c_str(), m_bUsingBackgroundLoad, false);
	}

	if (retActorNode)
	{
		// 새 메시노드의 가리기 규칙 적용
		m_pCoverMeshList->SetCoverMesh(retActorNode->m_pMeshNode,true);

		if (pItemData)
		{
			// Item Sub Material 
			for ( size_t i = 0; i < pItemData->m_vecMtrls.size(); ++i )
			{
				for ( int j = 0; j < pItemData->m_vecMtrls[i]->nSubCount; ++j )
				{
					m_pCharacter->ChangeMatetial(MLocale::ConvTCHARToAnsi(strActorNodeName.c_str()), j, pItemData->m_vecMtrls[i]->vecSubMtrls[j]);
				}
			}
		}
	}

	if(bNotifyToController)
		m_pCharacter->ReNewContollers();

	return retActorNode;
}

RActorNode* XCharacterParts::CreatePartsNode( const TCHAR* szMeshName, const CSItemData* pItemData, tstring& strNodeName, tstring& strParentName, tstring& strActorNodeName)
{
	PFI_BLOCK_THISFUNC(1208);

	RActorNode* pReturn = NULL;
	RMesh* pMesh = CreateMesh(strNodeName.c_str());

	if( pMesh->BuildAll() )
	{
		pReturn = CreatePartsNode( pMesh, pItemData, strNodeName, strParentName, strActorNodeName);
	}

	ReleaseMesh( m_pCharacter->m_pMesh );

	return pReturn;
}

void XCharacterParts::Update()
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(1209);
	if( !m_pCharacter->GetUsable() ) return;

	UpdatePartsListForCreatingMesh();

	UpdatePartsLoading();

	m_PartsInfo.UpdateQueue();
}

void XCharacterParts::UpdateForNoneBackgroundIfAllMeshLoaded()
{
	m_bUsingBackgroundLoad = false;
	Update();
	m_bUsingBackgroundLoad = true;
}

void XCharacterParts::UpdatePartsListForCreatingMesh()
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(1210);

	if (!m_PartsInfo.IsLoadingQueueEmpty())
	{
		for (XModelPartsInfo::ModelPartsList::iterator itor = m_PartsInfo.GetLoadingQueue().begin(); itor != m_PartsInfo.GetLoadingQueue().end(); ++itor)
		{
			XPartInfo* pPart = (*itor);
			if (pPart->GetState() == PART_STATE_READY_TO_CREATE_MESH)
			{
				pPart->pMesh = CreateMesh( pPart->strMeshName.c_str() );
				if (pPart->pMesh && !pPart->pMesh->IsLoadingCompleted() )
				{
					m_bUsingBackgroundLoad = true;
					const bool bBackground = true;
					pPart->pMesh->BuildAll( bBackground, m_pPartsLoadingController );
				}

				pPart->SetState(PART_STATE_READY_TO_CREATE_PARTS_NODE);
			}
		}
	}
}

void XCharacterParts::UpdatePartsLoading()
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(1211);

	for (XModelPartsInfo::ModelPartsList::iterator itor = m_PartsInfo.GetLoadingQueue().begin(); itor != m_PartsInfo.GetLoadingQueue().end(); ++itor)
	{
		XPartInfo* pPart = (*itor);

		if ( pPart->GetState() == PART_STATE_READY_TO_CREATE_PARTS_NODE )
		{
			if (pPart->pMesh && pPart->pMesh->IsLoadingCompleted() )
			{
				DeleteParts(pPart->nPartsID);
				CompletePartsLoading( pPart );
			}
		}
	}
}

void XCharacterParts::CancelFromLoadingQueue( int nPartsID )
{
	for (XModelPartsInfo::ModelPartsList::iterator itor = m_PartsInfo.GetLoadingQueue().begin(); itor != m_PartsInfo.GetLoadingQueue().end(); ++itor)
	{
		XPartInfo* pPart = (*itor);

		if ( pPart->nPartsID == nPartsID)
		{
			pPart->SetState(PART_STATE_LOADING_CANCELED);
		}
	}
}

void XCharacterParts::ReleaseMesh( RMesh* pMesh )
{
	PFI_BLOCK_THISFUNC(1213);
	REngine::GetSceneManager().ReleaseResource( pMesh );
}

RMesh* XCharacterParts::CreateMesh( const TCHAR* szMeshName )
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(1212);

	return REngine::GetSceneManager().CreateResource< RMesh >(MLocale::ConvTCHARToAnsi(szMeshName).c_str());
}

vector<XPartsNodeInfo>* XCharacterParts::GetPartsActorNode( int nSlotID )
{
	XPartInfo* pPartInfo = m_PartsInfo.Find(nSlotID);
	if(pPartInfo)
	{
		return &(pPartInfo->m_vecNodes);
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
int XPartsLoadingController::CalcPriority()
{
	return RBGWK_HIGHEST;
}

void XPartsLoadingController::OnLoadingComplete(RResource* pResource)
{
	//mlog("Loading Complete (%s)\n", pResource->GetName().c_str());
}