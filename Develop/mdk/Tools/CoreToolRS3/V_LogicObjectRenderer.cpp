#include "StdAfx.h"
#include "V_LogicObjectRenderer.h"
#include "M_LogicObject_AABBPicking.h"
#include "M_ToolSceneNodeInstance.h"
#include "REngine.h"
#include "M_Visitors.h"

//////////////////////////////////////////////////////////////////////////
// CLogicObjectRenderer_SceneNode

void CLogicObjectRenderer_SceneNode::OnAdd(CControlMediator*	pControlMediator_)
{
	rs3::RToolSceneManager* pSceneManager = pControlMediator_->GetToolSceneManager();
	rs3::RWorldSceneNode& worldSceneNode = pSceneManager->GetRootSceneNode();
	worldSceneNode.AddChild( m_pViewNode );

	m_pViewNode->AddAttribute( rs3::RSNA_DO_NOT_SAVE );

	if( GetCollisionEnabled() == false )
		m_pViewNode->AddAttribute( rs3::RSNA_DO_NOT_SAVE_COLLISION );
}

CLogicObjectRenderer_SceneNode::~CLogicObjectRenderer_SceneNode()
{
	delete m_pViewNode;
}

void CLogicObjectRenderer_SceneNode::OnRemove(CControlMediator*	pControlMediator_)
{
	m_pViewNode->RemoveFromParent();
	delete m_pViewNode;
	m_pViewNode = NULL;
}

void CLogicObjectRenderer_SceneNode::OnChanged()
{
	rs3::RVector vecPos;
	if( m_pLogicObject->GetPosition( vecPos ) )
		m_pViewNode->SetPosition( vecPos );

	rs3::RVector vecDir, vecUp;
	if( m_pLogicObject->GetDirection( vecDir ) && 
		m_pLogicObject->GetUp( vecUp ))
		m_pViewNode->SetDirection( vecDir, vecUp );

	rs3::RSceneNodeUpdateVisitor updater;
	m_pViewNode->Traverse( &updater );

	m_pViewNode->UpdateBoundingVolume();

	m_pLogicObject->SetAABB( m_pViewNode->GetAABB() );
}

void CLogicObjectRenderer_SceneNode::OnShow()
{
	CVisibleSetter visibleSetter( true );
	m_pViewNode->Traverse( &visibleSetter );
}

void CLogicObjectRenderer_SceneNode::OnHide()
{
	CVisibleSetter visibleSetter( false );
	m_pViewNode->Traverse( &visibleSetter );
}

//////////////////////////////////////////////////////////////////////////
//

void CLogicObjectRenderer_SceneNodeInstance::LoadSceneNodeInstance(CControlMediator* pControlMediator_, const char* szSceneFilename)
{
	// RToolSceneNodeInstance 을 써야 신노드의 aabb 가 자식을 포함된 걸로 갱신이 되어서 선택을 할수가 있습니다
	// RSceneNodeInstance 는 aabb 를 계산하지 않습니다.
	rs3::RSceneNodeInstance* pSceneInstance = new rs3::RToolSceneNodeInstance;
	bool bLoaded = pControlMediator_->GetToolSceneManager()->LoadSceneNodeInstanceFromRootXMLFile( pSceneInstance, szSceneFilename );

	if( bLoaded )
	{
		SetViewSceneNode( pSceneInstance );
	}
	else
	{
		// 실패한경우 ? 를 보여준다
		const char* QUESTIONMARK_VIEW_MESH_NAME = "question.elu";

		rs3::RActor* pViewActor = new rs3::RActor;
		pViewActor->Create( QUESTIONMARK_VIEW_MESH_NAME );
		SetViewSceneNode( pViewActor );
	}
}

void CLogicObjectRenderer_SceneNodeInstance::ReloadSceneNodeInstance(CControlMediator* pControlMediator_, const char* szSceneFilename)
{
	// 삭제하고 
	OnRemove(pControlMediator_);
	// 다시 생성해서 더한다
	LoadSceneNodeInstance( pControlMediator_, szSceneFilename );
	OnAdd(pControlMediator_);

}

//////////////////////////////////////////////////////////////////////////
//

void CLogicObjectRenderer_SceneNodeItself::OnChanged()
{
	rs3::RSceneNodeUpdateVisitor updater;
	m_pSceneNode->Traverse( &updater );

	m_pSceneNode->UpdateBoundingVolume();

	m_pLogicObject->SetAABB( m_pSceneNode->GetAABB() );
}

void CLogicObjectRenderer_SceneNodeItself::OnShow()
{
	CVisibleSetter visibleSetter( true );
	m_pSceneNode->Traverse( &visibleSetter );
}

void CLogicObjectRenderer_SceneNodeItself::OnHide()
{
	CVisibleSetter visibleSetter( false );
	m_pSceneNode->Traverse( &visibleSetter );
}