#include "stdafx.h"
#include "XCharacterBParts.h"
#include "RMesh.h"
#include "XCharacter.h"
#include "MStatisticsProfiler.h"
#include "MProfiler.h"

XCharacterBParts::XCharacterBParts( XCharacter* pCharacter )
: XCharacterParts(pCharacter)
{

}

RActorNode* XCharacterBParts::CreatePartsNode( RMesh* pMesh, const CSItemData* pItemData, tstring& strNodeName, tstring& strParentName, tstring& strActorNodeName, bool bNotifyToController /*= true */ )
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(1207);

	// 이미 있는 ActorNode 삭제
	//DeleteActorNode(strActorNodeName.c_str(), false);

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

	if(bNotifyToController)
		m_pCharacter->ReNewContollers();

	return retActorNode;
}
