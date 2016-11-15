#include "StdAfx.h"
#include "M_LogicObjectCommandBuffer.h"
#include "M_ToolLogicObjectManager.h"
#include "M_LogicObject_SceneNodes.h"


//////////////////////////////////////////////////////////////////////////////
///
/// Logic Object New
CLogicNewCmdBuffer::CLogicNewCmdBuffer(CLogicObject* _pObject)
:	m_strUndoMessage_("Object 생성편집")
{
	m_pObject_ = _pObject;
	m_tCommandBufferSize = 20;
}
CLogicNewCmdBuffer::~CLogicNewCmdBuffer(){}

VIEW_MESSAGE::ID CLogicNewCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pObject_);
	_pController->GetToolLogicObjectMgr()->RemoveFromManager(m_pObject_);
	m_pObject_->DestroyLogicObject();

	delete m_pObject_;

	return VIEW_MESSAGE::UNDO_OBJ_NEW_DEL;
}

void CLogicNewCmdBuffer::LogUndoMessage()
{
	if (m_pObject_)
	{
		rs3::RVector vPos;
		m_pObject_->GetPosition(vPos);
		std::string strName = m_pObject_->GetObjectName();

		mlog("%s New Operation did at %.3f, %.3f, %.3f\n", m_pObject_->GetObjectName(), vPos.x, vPos.y, vPos.z);
	}
}


//////////////////////////////////////////////////////////////////////////////
///
/// Logic Object Delete
CLogicDeleteCmdBuffer::CLogicDeleteCmdBuffer(CLogicObject* _pObject)
:	m_strUndoMessage_("Object 삭제편집")
{
	m_pObject_ = _pObject;
	m_tCommandBufferSize = 20;
}
CLogicDeleteCmdBuffer::~CLogicDeleteCmdBuffer()
{
	if(m_pObject_ != NULL)
	{
		m_pObject_->DestroyLogicObject();
		delete m_pObject_;
	}
}

VIEW_MESSAGE::ID CLogicDeleteCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pObject_);

	_pController->GetToolLogicObjectMgr()->AddToManager(m_pObject_);
	m_pObject_ = NULL;
	return VIEW_MESSAGE::UNDO_OBJ_NEW_DEL;
}

void CLogicDeleteCmdBuffer::LogUndoMessage()
{
	if (m_pObject_)
	{
		rs3::RVector vPos;
		m_pObject_->GetPosition(vPos);

		mlog("%s Deletion Operation did at %.3f, %.3f, %.3f\n", m_pObject_->GetObjectName(), vPos.x, vPos.y, vPos.z);
	}
}

//////////////////////////////////////////////////////////////////////////////
///
/// Logic Move
CLogicMoveCmdBuffer::CLogicMoveCmdBuffer(CLogicObject* _pLogicObject, const rs3::RVector& _refVecOrgPos)
: m_strUndoMessage_("로직물체 이동편집")
, m_vPos_(_refVecOrgPos)
{
	m_pLogicObject_ = _pLogicObject;
	m_tCommandBufferSize = 5;
}
CLogicMoveCmdBuffer::~CLogicMoveCmdBuffer(){}

VIEW_MESSAGE::ID CLogicMoveCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pLogicObject_);
	m_pLogicObject_->SetPosition(m_vPos_);
	_pController->BroadCastMessage( CoreMessage(VIEW_MESSAGE::LOGIC_MOVE, m_pLogicObject_) );

	return VIEW_MESSAGE::UNDO_MOVE;
}

void CLogicMoveCmdBuffer::LogUndoMessage()
{
	//if (m_pEntityObject_)
	//{
	//	rs3::RVector vPos;
	//	m_pEntityObject_->GetPosition(vPos);
	//	std::string strEntityName;
	//	const CEntityObject::TYPE eEntityType = m_pEntityObject_->GetType();
	//	if (eEntityType == CEntityObject::ENTITY_MARKER)
	//		strEntityName = "Entity Marker";
	//	else if (eEntityType == CEntityObject::ENTITY_SENSOR)
	//		strEntityName = "Entity Sensor";
	//	else if (eEntityType == CEntityObject::ENTITY_SPAWN)
	//		strEntityName = "Entity Spawn";

	//	mlog("%s(%s) Move Operation did at %.3f, %.3f, %.3f\n", strEntityName.c_str(), m_pEntityObject_->GetObjectName(), vPos.x, vPos.y, vPos.z);
	//}
}

//////////////////////////////////////////////////////////////////////////////
///
/// Logic Rotate
CLogicRotateCmdBuffer::CLogicRotateCmdBuffer(CLogicObject* _pLogicObject, const rs3::RVector& _refVecOrgDir, const rs3::RVector& _refVecOrgUp)
: m_strUndoMessage_("로직물체 방향변경")
, m_vDir_(_refVecOrgDir)
, m_vUp_(_refVecOrgUp)
, m_bPosMoved(false)
{
	m_pLogicObject_ = _pLogicObject;
	m_tCommandBufferSize = 5;
}
CLogicRotateCmdBuffer::CLogicRotateCmdBuffer(CLogicObject* _pLogicObject, const rs3::RVector& _refVecOrgDir, const rs3::RVector& _refVecOrgUp,
											 const rs3::RVector& _refVecOrgPos, const rs3::RVector& _refVecPivot)
: m_strUndoMessage_("로직물체 방향변경")
, m_vDir_(_refVecOrgDir)
, m_vUp_(_refVecOrgUp)
, m_vPos_(_refVecOrgPos)
, m_bPosMoved(true)
, m_vPivot(_refVecPivot)
{
	m_pLogicObject_ = _pLogicObject;
	m_tCommandBufferSize = 5;
}
CLogicRotateCmdBuffer::~CLogicRotateCmdBuffer(){}

VIEW_MESSAGE::ID CLogicRotateCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pLogicObject_);
	m_pLogicObject_->SetDirectionUp(m_vDir_, m_vUp_);
	if(m_bPosMoved)
		m_pLogicObject_->SetPosition(m_vPos_);
	_pController->BroadCastMessage( CoreMessage(VIEW_MESSAGE::LOGIC_MOVE, m_pLogicObject_) );
	return VIEW_MESSAGE::UNDO_GENERAL;
}

void CLogicRotateCmdBuffer::LogUndoMessage()
{
	//if (m_pEntityObject_)
	//{
	//	rs3::RVector vPos;
	//	m_pEntityObject_->GetPosition(vPos);
	//	std::string strEntityName;
	//	const CEntityObject::TYPE eEntityType = m_pEntityObject_->GetType();
	//	if (eEntityType == CEntityObject::ENTITY_MARKER)
	//		strEntityName = "Entity Marker";
	//	else if (eEntityType == CEntityObject::ENTITY_SENSOR)
	//		strEntityName = "Entity Sensor";
	//	else if (eEntityType == CEntityObject::ENTITY_SPAWN)
	//		strEntityName = "Entity Spawn";

	//	mlog("%s(%s) Rotation Operation did at %.3f, %.3f, %.3f\n", strEntityName.c_str(), m_pEntityObject_->GetObjectName(), vPos.x, vPos.y, vPos.z);
	//}
}

//////////////////////////////////////////////////////////////////////////////
///
/// Logic Scale
CLogicScaleCmdBuffer::CLogicScaleCmdBuffer(CLogicObject* _pLogicObject, float _fOrgScale)
:	m_strUndoMessage_("Logic Object 물체 크기변경")
{
	m_pLogicObject_ = _pLogicObject;
	m_tCommandBufferSize = 5;
	m_fOrgScale_ = _fOrgScale;
}
CLogicScaleCmdBuffer::~CLogicScaleCmdBuffer(){}

VIEW_MESSAGE::ID CLogicScaleCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pLogicObject_);
	m_pLogicObject_->SetScale(m_fOrgScale_);

	return VIEW_MESSAGE::UNDO_ENTITY_EDIT;
}

void CLogicScaleCmdBuffer::LogUndoMessage()
{
	if (m_pLogicObject_)
	{
		rs3::RVector vPos;
		m_pLogicObject_->GetPosition(vPos);

		mlog("(%s) Scale Operation did at %.3f, %.3f, %.3f\n", m_pLogicObject_->GetObjectName(), vPos.x, vPos.y, vPos.z);
	}
}

//////////////////////////////////////////////////////////////////////////
//

CLogicObject_SceneNodeCloneNewCmdBuffer::CLogicObject_SceneNodeCloneNewCmdBuffer(CLogicObject_SceneNode* _pObject)
:	m_strUndoMessage_("신노드 클론 생성편집")
{
	m_pObject_ = _pObject;
	m_tCommandBufferSize = 20;

	// 여기서 클론을 하는게 맞지않나
}

CLogicObject_SceneNodeCloneNewCmdBuffer::~CLogicObject_SceneNodeCloneNewCmdBuffer()
{
}

VIEW_MESSAGE::ID CLogicObject_SceneNodeCloneNewCmdBuffer::Undo(CControlMediator* _pController)
{
	_pController->GetToolLogicObjectMgr()->RemoveFromManager( m_pObject_ );
	_ASSERT( m_pObject_ );
	
	rs3::RSceneNode* pSceneNode = m_pObject_->GetModelSceneNode();
	if ( pSceneNode )
	{
		_pController->GetToolSceneManager()->DetachLogicObject( pSceneNode );
		pSceneNode->RemoveFromParent();
		delete pSceneNode;
	}
	return VIEW_MESSAGE::UNDO_OBJ_NEW_DEL;
}

void CLogicObject_SceneNodeCloneNewCmdBuffer::LogUndoMessage()
{
	if (m_pObject_)
	{
		rs3::RVector vPos;
		m_pObject_->GetPosition(vPos);

		mlog("%s scene undo clone Operation did at %.3f, %.3f, %.3f\n", m_pObject_->GetObjectName(), vPos.x, vPos.y, vPos.z);
		}
}

//////////////////////////////////////////////////////////////////////////////
///
/// LogicObject_SceneNode Delete
CLogicObject_SceneNodeDeleteCmdBuffer::CLogicObject_SceneNodeDeleteCmdBuffer(CLogicObject* _pObject)
:	m_strUndoMessage_("신노드 삭제편집")
{
	m_pObject_ = _pObject;
	m_tCommandBufferSize = 20;

	CLogicObject_SceneNode* pLogicObjectSceneNode = dynamic_cast< CLogicObject_SceneNode* > (_pObject);
	_ASSERT( pLogicObjectSceneNode );
	
	// 실제 삭제 행위
	m_pModelNode = pLogicObjectSceneNode->GetModelSceneNode();
	m_pParentNode = m_pModelNode->GetParent();
	_ASSERT( m_pParentNode );

	m_pModelNode->RemoveFromParent();
}

CLogicObject_SceneNodeDeleteCmdBuffer::~CLogicObject_SceneNodeDeleteCmdBuffer()
{
	if(m_pObject_ != NULL)
	{
		m_pObject_->DestroyLogicObject();
		delete m_pObject_;
	}

	if( m_pModelNode )
		delete m_pModelNode;
}

VIEW_MESSAGE::ID CLogicObject_SceneNodeDeleteCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pObject_);

	_pController->GetToolLogicObjectMgr()->AddToManager(m_pObject_);
	m_pObject_ = NULL;

	m_pParentNode->AddChild( m_pModelNode );
	m_pParentNode->AddToSceneNodeGroup( m_pModelNode );

	m_pModelNode = NULL;
	m_pParentNode = NULL;

	return VIEW_MESSAGE::UNDO_OBJ_NEW_DEL;
}

void CLogicObject_SceneNodeDeleteCmdBuffer::LogUndoMessage()
{
	if (m_pObject_)
	{
		rs3::RVector vPos;
		m_pObject_->GetPosition(vPos);

		mlog("%s Deletion Operation did at %.3f, %.3f, %.3f\n", m_pObject_->GetObjectName(), vPos.x, vPos.y, vPos.z);
	}
}