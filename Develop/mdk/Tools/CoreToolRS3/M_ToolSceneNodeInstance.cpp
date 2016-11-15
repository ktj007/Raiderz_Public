#include "stdafx.h"
#include <assert.h>
#include "M_ToolSceneNodeInstance.h"
#include "MainFrm.h"
#include "MFileSystem.h"
#include "REmitter.h"
#include "REffectSceneNode.h"

namespace rs3 {

MImplementRTTI( RToolSceneNodeInstance, RSceneNodeInstance );

RToolSceneNodeInstance::RToolSceneNodeInstance() 
{
}

RToolSceneNodeInstance::~RToolSceneNodeInstance()
{
	_ASSERT( GetUserData() == NULL );
}

RSceneNode* RToolSceneNodeInstance::Clone() 	// 복사 생성
{
	string strNodeName = GetNodeName().c_str();

	RToolSceneNodeInstance* pToNode = new RToolSceneNodeInstance;
	pToNode->SetNodeName( strNodeName.c_str() );
	pToNode->SetTransform( GetLocalTransform() );
	pToNode->SetScale( GetScale() );

	bool bRet = pToNode->Create(m_strFileName.c_str(), false);
	if ( !bRet )
	{
		delete pToNode;
		return NULL;
	}

	return pToNode;	
}

bool RToolSceneNodeInstance::OnPick(RPICKINFO& pickInfo)
{
	if(!GetAABB().ContainsLineSegment(pickInfo.inPickOrigin,pickInfo.inPickTo)) 
		return false;

	bool bPicked = false;

	const RSceneNodeList *pChildNodeList = &GetChildren();
	for ( RSceneNodeList::const_iterator it = pChildNodeList->begin(); it != pChildNodeList->end(); ++it )
	{
		RSceneNode *pNode = *it;
		bPicked |= pNode->Pick(pickInfo);
	}

	return bPicked;
}

void RToolSceneNodeInstance::OnPostUpdate()
{
	UpdateAABB();
}

void RToolSceneNodeInstance::UpdateAABB()
{
	class RAABBUpdateVisitor : public RSceneNodeVisitor
	{
	public:
		RAABBUpdateVisitor() : m_aabb(RBox::INVALID) {}
		virtual ~RAABBUpdateVisitor() {}

		virtual bool OnPreOrderVisit( RSceneNode* pSceneNode ) override
		{
			return true;
		}

		virtual void OnPostOrderVisit( RSceneNode* pSceneNode ) override
		{
			pSceneNode->UpdateBoundingVolume();
			m_aabb.Add( pSceneNode->GetAABB() );
		}

		const RBox& GetAABB() const { return m_aabb; }

	private:
		RBox	m_aabb;
	};

	SetAABB( RBox::INVALID );
	RAABBUpdateVisitor v;
	Traverse( &v );
	SetAABB( v.GetAABB() );
}

void RToolSceneNodeInstance::ConvertToAsCopy(RSceneNodeInstance* pSceneNode, RSceneNodeList* pList )
{
	RSceneNode* pParent = pSceneNode->GetParent();
	_ASSERT( pParent );

	while( !pSceneNode->GetChildren().empty() )
	{
		RSceneNode* pChildNode = pSceneNode->GetChildren().front();

		// 관리하고있던 자식노드인경우 편집가능하도록 RSNA_DO_NOT_SAVE 플래그 해제
		if( pChildNode->GetGroupOwner() == pSceneNode )
			pChildNode->RemoveAttribute( RSNA_DO_NOT_SAVE );

		if(pList)
			pList->push_back( pChildNode );

		RMatrix matChildWorldTM = pChildNode->GetWorldTransform();
		pChildNode->RemoveFromParent();

		RMatrix matInvParent;
		pParent->GetWorldTransform().GetInverse( &matInvParent );
		
		pChildNode->SetTransform( matInvParent * matChildWorldTM );

		pParent->AddChild( pChildNode );
		pParent->AddToSceneNodeGroup( pChildNode );
	}

	pSceneNode->RemoveFromParent();
	delete pSceneNode;
}

void RToolSceneNodeInstance::ClearForReload()
{
	DeleteManagingChildren();
}

}