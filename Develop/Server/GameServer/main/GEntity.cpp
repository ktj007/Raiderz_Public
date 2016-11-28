#include "stdafx.h"
#include "GEntity.h"
#include "GField.h"
#include "GEntityActor.h"


GEntity::GEntity() 
: m_UID(0)
, m_vPos(0,0,0)
, m_vDir(0,1,0)
, m_pField(NULL)
, m_bCreated(false)
, m_bDeleteMe(false)
, m_bDead(false)
, m_vFacingDir(0,1,0)
, m_nTypeID(ETID_ENTITY)
, m_isActivateUpdate(true)
{
	m_nType = ET_ENTITY;
}

GEntity::~GEntity()
{
	_ASSERT(!m_bCreated && L"반드시 Destory()를 호출한 후 삭제해야합니다.");
}

bool GEntity::Create(MUID& uid)
{
	m_bDead = false;
	m_bCreated = true;
	m_UID = uid;
	
	RegisterModules();	// 모듈 추가

	return true;
}

void GEntity::Destroy()
{
	if (!m_bCreated) 
		return;

	OnDestroy();

	if (IsInField())
	{
		LeaveFromField();
	}

	UnregisterModlues();	// 모듈 제거
	m_bCreated = false;
}

void GEntity::EnterToField(GField* pField, const vec3& pos, const vec3& dir)
{
	VALID(pField);

	m_pField = pField;
	m_pField->OnEntityEntered(this);
	
	SetPos(pos);
	SetDir(dir);
	SetFacingDir(dir);

	OnEnter();

	if (m_pField->IsDynamicField())
	{
		OnEnterToDynamicField();
	}
	else
	{
		OnEnterToSharedField();
	}
}

void GEntity::LeaveFromField()
{
	if (!m_pField) 
		return;
	
	OnLeave();
	m_pField->OnEntityLeaved(this);
	
	m_pField = NULL;
}

void GEntity::Update(float fDelta)
{
	OnUpdate(fDelta);
}

void GEntity::Message(GMessage& msg)
{
	OnMessage(msg);
}

void GEntity::OnUpdate(float fDelta)
{

}

void GEntity::OnMessage(GMessage& msg)
{
	m_Modules.Message(msg);
}

bool GEntity::SetPos(const vec3& pos)
{
	m_vPos = pos;

	return true;
}

bool GEntity::IsActiveModule(int nID)
{
	return m_Modules.IsActiveModule(nID);
}

GModule* GEntity::GetModule(int nID)
{
	return m_Modules.GetModule(nID);
}


void GEntity::doDie()
{
	m_bDead = true;
}

void GEntity::OnRebirth()
{
	m_bDead = false;
}

MUID GEntity::GetFieldUID() const
{ 
	return (m_pField) ? m_pField->GetUID() : MUID::ZERO;
}

int GEntity::GetFieldID() const
{ 
	if (!m_pField)				return -1;
	if (!m_pField->GetInfo())	return -1;

	return m_pField->GetInfo()->m_nFieldID;
}

const GFieldInfo* GEntity::GetFieldInfo() const
{
	if (!m_pField)				return NULL;
	return m_pField->GetInfo();
}

bool GEntity::IsAILodEnableFiled() const
{
	if (!m_pField) return false;
	if (!m_pField->GetInfo()) return false;

	return m_pField->GetInfo()->m_bAILODEnable;
}

bool GEntity::IsType(uint32 mask) const
{
	if (mask & m_nType) return true;
	return false;
}

bool GEntity::IsNPC() const
{
	return IsType(ET_NPC);
}

bool GEntity::IsPlayer() const
{
	return IsType(ET_PC);
}

bool GEntity::IsActor() const
{
	return IsType(ET_ACTOR);
}

bool GEntity::IsBuffEntity() const
{
	return IsType(ET_BUFF_ENTITY);
}

bool GEntity::IsBPart() const
{
	return IsType(ET_ENTITY_BPART);
}

float GEntity::DistanceTo( GEntity* pEntity )
{
	if (NULL == pEntity) return 0.0f;

	return m_vPos.DistanceTo(pEntity->GetPos());
}

void GEntity::SetDir( const vec3& dir )
{
	if (dir.LengthSq() < 0.001f)
	{
		return;
	}

#ifdef _DEBUG
	{
		_ASSERT(dir.Length() > 0.001f);
	}
#endif

	m_vDir = dir;
}

bool GEntity::IsBack(const vec3& vecTargetPos)
{
	vec3 vecTargetDir = vecTargetPos - m_vPos;
	vecTargetDir.Normalize();

	return (m_vDir.DotProduct(vecTargetDir) < 0.0f);
}

void GEntity::SetActivateUpdate(bool isActivate)
{
	m_isActivateUpdate = isActivate;
}

bool GEntity::IsActivateUpdate() const
{
	return m_isActivateUpdate;
}

void GEntity::SetFacingDir( const vec3& dir )
{
	if (dir.LengthSq() < 0.001f)
	{
		return;
	}

	m_vFacingDir = dir;
}