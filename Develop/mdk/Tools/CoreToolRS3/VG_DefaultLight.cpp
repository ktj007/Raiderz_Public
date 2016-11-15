#include "StdAfx.h"
#include "VG_DefaultLight.h"
#include "M_ToolSceneManager.h"
#include "M_LogicObject.h"
#include "M_LogicObject_SceneNodes.h"
#include "M_ToolLogicObjectManager.h"

VG_DefaultLight::VG_DefaultLight(void)
: m_pDefaultLight( NULL )
, m_bLightToCamera( false )
{
}

VG_DefaultLight::~VG_DefaultLight(void)
{
}

void VG_DefaultLight::Listen(CoreMessage& _message)
{
	// 전역 처리
	switch( _message.nID)
	{
	case VIEW_MESSAGE::SCENE_LOADED:
	case VIEW_MESSAGE::LOGIC_LOADED:
		AddDefaultLight();
		break;
	case VIEW_MESSAGE::PRE_CLOSE_DOCUMENT:
		RemoveDefaultLight();
		break;
	case VIEW_MESSAGE::LOGIC_RENDER : 
		OnLogicRender(); 
		break;
	case VIEW_MESSAGE::LOGIC_DELETE:
		OnLogicDelete( _message.pLogicObject );
		break;
	}
}

void VG_DefaultLight::AddDefaultLight()
{
	rs3::RToolSceneManager* pToolSceneManager = m_pControlMediator_->GetToolSceneManager();
	
	// 이미 기본광원이 있는 경우에는 만들지 않는다
	if( pToolSceneManager->GetDefaultLight() != NULL )
		return;

	m_pDefaultLight = new rs3::RDirectionalLightSceneNode;
	m_pDefaultLight->SetLightAmbient(rs3::RVector4(0.1f, 0.1f, 0.1f, 1.0f));
	m_pDefaultLight->SetLightDirection(rs3::RVector3(0.2, 1, -1));
	m_pDefaultLight->SetPosition(rs3::RVector3(0, 0, 200));
	m_pDefaultLight->SetNodeName("DefaultLight");
	m_pDefaultLight->AddAttribute( rs3::RSNA_DO_NOT_SAVE );

	rs3::RSceneNodeUpdateVisitor	sceneNodeUpdator;
	m_pDefaultLight->Traverse(&sceneNodeUpdator);

	pToolSceneManager->AddSceneNode(m_pDefaultLight, true);

	// undo 가 불가능한 logicobject
	class CLogicObject_DefaultLight : public CLogicObject_DirectionalLight
	{
		ICommandBuffer*		CreateDeleteCmdBuffer() override { return NULL; }
	};

	CLogicObject_DefaultLight* pLogicObject = new CLogicObject_DefaultLight;
	pLogicObject->LinkModelSceneNode( m_pDefaultLight );
	pLogicObject->SetController( m_pControlMediator_ );
	m_pControlMediator_->GetToolLogicObjectMgr()->AddToManager(pLogicObject);
}

void VG_DefaultLight::RemoveDefaultLight()
{
	if( m_pDefaultLight == NULL ) return;

 	rs3::RToolSceneManager* pToolSceneManager = m_pControlMediator_->GetToolSceneManager();
	CLogicObject* pDefaultLightLogicObject = pToolSceneManager->GetLogicObjectLinkedTo( m_pDefaultLight );
	if(pDefaultLightLogicObject)
		m_pControlMediator_->GetToolLogicObjectMgr()->RemoveFromManager( pDefaultLightLogicObject );
	pDefaultLightLogicObject->DestroyLogicObject();
	delete pDefaultLightLogicObject;

}

// 기본광원을 삭제한경우 처리
void VG_DefaultLight::OnLogicDelete( CLogicObject* pLogicObject )
{
	if( m_pDefaultLight )
	{
		rs3::RToolSceneManager* pToolSceneManager = m_pControlMediator_->GetToolSceneManager();
		CLogicObject* pDefaultLightLogicObject = pToolSceneManager->GetLogicObjectLinkedTo( m_pDefaultLight );

		if( pLogicObject == pDefaultLightLogicObject )
		{
			m_pDefaultLight->RemoveFromParent();
			SAFE_DELETE(m_pDefaultLight);
		}
	}
}

void VG_DefaultLight::ToggleLight()
{
	if( IsEnable() )
		RemoveDefaultLight();
	else
		AddDefaultLight();
}

void VG_DefaultLight::ToggleLightToCamera()
{
	m_bLightToCamera = !m_bLightToCamera;
}

void VG_DefaultLight::OnLogicRender()
{
	if( m_pDefaultLight == NULL ) return;
	if( m_bLightToCamera )
	{
		rs3::RVector3 vecCamDir = m_pControlMediator_->GetToolCamera()->GetDirection();

		CLogicObject* pLogicObject = m_pControlMediator_->GetToolSceneManager()->GetLogicObjectLinkedTo( m_pDefaultLight );
		pLogicObject->SetDirectionUp( vecCamDir, rs3::RVector(0,0,1) );
	}
}