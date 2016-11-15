#include "StdAfx.h"
#include "M_EntityCommandBuffer.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"

#include "M_ToolCmdMgr.h"
#include "M_EntityObject.h"
#include "M_ToolLogicObjectManager.h"

#include "M_ToolTerrain.h"
#include "M_ToolSceneManager.h"

#include "VL_SceneNodeList.h"
#include "VR_EntityObjectList.h"
#include "M_PathFinder.h"

#include "MDebug.h"

//////////////////////////////////////////////////////////////////////////////
///
/// Entity New
CEntityNewCmdBuffer::CEntityNewCmdBuffer(CEntityObject* _pEntityObject)
:	m_strUndoMessage_("Entity물체 생성편집")
{
	m_pEntityObject_ = _pEntityObject;
	m_tCommandBufferSize = 20;
	m_bUnDoExecuted = false;
}

CEntityNewCmdBuffer::~CEntityNewCmdBuffer()
{
	if (m_bUnDoExecuted)
	{
		delete m_pEntityObject_;
	}
}

VIEW_MESSAGE::ID CEntityNewCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pEntityObject_);
	_pController->GetToolLogicObjectMgr()->RemoveFromManager(m_pEntityObject_);
	m_pEntityObject_->DestroyLogicObject();
	m_bUnDoExecuted = true;

	return VIEW_MESSAGE::UNDO_OBJ_NEW_DEL;
}

void CEntityNewCmdBuffer::LogUndoMessage()
{
	if (m_pEntityObject_)
	{
		rs3::RVector vPos;
		m_pEntityObject_->GetPosition(vPos);
		std::string strEntityName;
		const CEntityObject::TYPE eEntityType = m_pEntityObject_->GetType();
		if (eEntityType == CEntityObject::ENTITY_MARKER)
			strEntityName = "Entity Marker";
		else if (eEntityType == CEntityObject::ENTITY_SENSOR)
			strEntityName = "Entity Sensor";
		else if (eEntityType == CEntityObject::ENTITY_SPAWN)
			strEntityName = "Entity Spawn";

		mlog("%s(%s) New Operation did at %.3f, %.3f, %.3f\n", strEntityName.c_str(), m_pEntityObject_->GetObjectName(), vPos.x, vPos.y, vPos.z);
	}
}

//////////////////////////////////////////////////////////////////////////////
///
/// Entity Delete
CEntityDeleteCmdBuffer::CEntityDeleteCmdBuffer(CEntityObject* _pEntityObject)
:	m_strUndoMessage_("Entity물체 삭제편집")
{
	m_pEntityObject_ = _pEntityObject;
	m_tCommandBufferSize = 20;
}
CEntityDeleteCmdBuffer::~CEntityDeleteCmdBuffer()
{
	if(m_pEntityObject_ != NULL)
	{
		m_pEntityObject_->DestroyLogicObject();
		delete m_pEntityObject_;
	}
}

VIEW_MESSAGE::ID CEntityDeleteCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pEntityObject_);

	_pController->GetToolLogicObjectMgr()->AddToManager(m_pEntityObject_);
	m_pEntityObject_ = NULL;
	return VIEW_MESSAGE::UNDO_OBJ_NEW_DEL;
}

void CEntityDeleteCmdBuffer::LogUndoMessage()
{
	if (m_pEntityObject_)
	{
		rs3::RVector vPos;
		m_pEntityObject_->GetPosition(vPos);
		std::string strEntityName;
		const CEntityObject::TYPE eEntityType = m_pEntityObject_->GetType();
		if (eEntityType == CEntityObject::ENTITY_MARKER)
			strEntityName = "Entity Marker";
		else if (eEntityType == CEntityObject::ENTITY_SENSOR)
			strEntityName = "Entity Sensor";
		else if (eEntityType == CEntityObject::ENTITY_SPAWN)
			strEntityName = "Entity Spawn";

		mlog("%s(%s) Deletion Operation did at %.3f, %.3f, %.3f\n", strEntityName.c_str(), m_pEntityObject_->GetObjectName(), vPos.x, vPos.y, vPos.z);
	}
}

//////////////////////////////////////////////////////////////////////////////
///
/// Entity Move
CEntityMoveCmdBuffer::CEntityMoveCmdBuffer(CEntityObject* _pEntityObject, const rs3::RVector& _refVecOrgPos)
:	m_strUndoMessage_("Entity물체 이동편집"),
	m_vPos_(_refVecOrgPos)
{
	m_pEntityObject_ = _pEntityObject;
	m_tCommandBufferSize = 5;
}
CEntityMoveCmdBuffer::~CEntityMoveCmdBuffer(){}

VIEW_MESSAGE::ID CEntityMoveCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pEntityObject_);
	m_pEntityObject_->SetPosition(m_vPos_);

	// 네비게이션 메시에 유효한지 아닌지를 재 검사
	m_pEntityObject_->SetValid(g_PathfinderTest.IsVailid3DPostiion(m_vPos_));

	return VIEW_MESSAGE::UNDO_ENTITY_EDIT;
}

void CEntityMoveCmdBuffer::LogUndoMessage()
{
	if (m_pEntityObject_)
	{
		rs3::RVector vPos;
		m_pEntityObject_->GetPosition(vPos);
		std::string strEntityName;
		const CEntityObject::TYPE eEntityType = m_pEntityObject_->GetType();
		if (eEntityType == CEntityObject::ENTITY_MARKER)
			strEntityName = "Entity Marker";
		else if (eEntityType == CEntityObject::ENTITY_SENSOR)
			strEntityName = "Entity Sensor";
		else if (eEntityType == CEntityObject::ENTITY_SPAWN)
			strEntityName = "Entity Spawn";

		mlog("%s(%s) Move Operation did at %.3f, %.3f, %.3f\n", strEntityName.c_str(), m_pEntityObject_->GetObjectName(), vPos.x, vPos.y, vPos.z);
	}
}

//////////////////////////////////////////////////////////////////////////////
///
/// Entity Scale
CEntityScaleCmdBuffer::CEntityScaleCmdBuffer(CEntityObject* _pEntityObject, float _fOrgScale)
:	m_strUndoMessage_("Entity물체 크기변경")
{
	m_pEntityObject_ = _pEntityObject;
	m_tCommandBufferSize = 5;
	m_fOrgScale_ = _fOrgScale;
}
CEntityScaleCmdBuffer::~CEntityScaleCmdBuffer(){}

VIEW_MESSAGE::ID CEntityScaleCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pEntityObject_);
	m_pEntityObject_->SetScale(m_fOrgScale_);

	return VIEW_MESSAGE::UNDO_ENTITY_EDIT;
}

void CEntityScaleCmdBuffer::LogUndoMessage()
{
	if (m_pEntityObject_)
	{
		rs3::RVector vPos;
		m_pEntityObject_->GetPosition(vPos);
		std::string strEntityName;
		const CEntityObject::TYPE eEntityType = m_pEntityObject_->GetType();
		if (eEntityType == CEntityObject::ENTITY_MARKER)
			strEntityName = "Entity Marker";
		else if (eEntityType == CEntityObject::ENTITY_SENSOR)
			strEntityName = "Entity Sensor";
		else if (eEntityType == CEntityObject::ENTITY_SPAWN)
			strEntityName = "Entity Spawn";
		else if (eEntityType == CEntityObject::ENTITY_SOUND)
			strEntityName = "Entity Sound";

		mlog("%s(%s) Scale Operation did at %.3f, %.3f, %.3f\n", strEntityName.c_str(), m_pEntityObject_->GetObjectName(), vPos.x, vPos.y, vPos.z);
	}
}

//////////////////////////////////////////////////////////////////////////////
///
/// Entity Rotate
CEntityRotateCmdBuffer::CEntityRotateCmdBuffer(CEntityObject* _pEntityObject, const rs3::RVector& _refVecOrgDir)
:	m_strUndoMessage_("Entity물체 방향변경"),
	m_vDir_(_refVecOrgDir)
{
	m_pEntityObject_ = _pEntityObject;
	m_tCommandBufferSize = 5;
}
CEntityRotateCmdBuffer::~CEntityRotateCmdBuffer(){}

VIEW_MESSAGE::ID CEntityRotateCmdBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pEntityObject_);
	m_pEntityObject_->SetDirectionUp(m_vDir_, RVector(0,0,1) );

	return VIEW_MESSAGE::UNDO_ENTITY_EDIT;
}

void CEntityRotateCmdBuffer::LogUndoMessage()
{
	if (m_pEntityObject_)
	{
		rs3::RVector vPos;
		m_pEntityObject_->GetPosition(vPos);
		std::string strEntityName;
		const CEntityObject::TYPE eEntityType = m_pEntityObject_->GetType();
		if (eEntityType == CEntityObject::ENTITY_MARKER)
			strEntityName = "Entity Marker";
		else if (eEntityType == CEntityObject::ENTITY_SENSOR)
			strEntityName = "Entity Sensor";
		else if (eEntityType == CEntityObject::ENTITY_SPAWN)
			strEntityName = "Entity Spawn";

		mlog("%s(%s) Rotation Operation did at %.3f, %.3f, %.3f\n", strEntityName.c_str(), m_pEntityObject_->GetObjectName(), vPos.x, vPos.y, vPos.z);
	}
}

//////////////////////////////////////////////////////////////////////////////
///
/// Attribute Generic Num 1 changed
CEntityAttrGenVec1Buffer::CEntityAttrGenVec1Buffer(CEntitySpawn* _pEntityObject, SAttributeAtXML* _pEntityAttr, double _dGenVec1, const std::string& _strMessage)
:	m_strUndoMessage_(_strMessage)
{
	m_dGenVec1_ = _dGenVec1;
	m_pEntityAttribute_ = _pEntityAttr;
	m_pEntitySpawn_ = _pEntityObject;
	m_tCommandBufferSize = 5;
}
CEntityAttrGenVec1Buffer::~CEntityAttrGenVec1Buffer(){}

VIEW_MESSAGE::ID CEntityAttrGenVec1Buffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pEntityAttribute_);
	m_pEntityAttribute_->m_vGenericNum[0] = m_dGenVec1_;
	if(m_pEntitySpawn_ != NULL)
		m_pEntitySpawn_->UpdateColor();

	return VIEW_MESSAGE::UNDO_ENTITY_EDIT;
}

//////////////////////////////////////////////////////////////////////////////
///
/// Attribute Generic String changed
CEntityAttrGenStringBuffer::CEntityAttrGenStringBuffer(SAttributeAtXML* _pEntityAttr, const std::string& _strGenString, const std::string& _strMessage)
:	m_strUndoMessage_(_strMessage),
	m_strGenString_(_strGenString)
{
	m_pEntityAttribute_ = _pEntityAttr;
	m_tCommandBufferSize = 5;
}
CEntityAttrGenStringBuffer::~CEntityAttrGenStringBuffer(){}

VIEW_MESSAGE::ID CEntityAttrGenStringBuffer::Undo(CControlMediator* _pController)
{
	ASSERT(m_pEntityAttribute_);
	m_pEntityAttribute_->m_strGenericString = m_strGenString_;

	return VIEW_MESSAGE::UNDO_ENTITY_EDIT;
}
