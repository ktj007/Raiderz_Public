#include "stdafx.h"
#include "XObject.h"
#include "XModuleEntity.h"
#include "XModuleMotion.h"
#include "XModuleAction.h"
#include "XModuleMovable.h"
#include "XModuleCollision.h"
#include "XModuleMyControl.h"
#include "XModuleNetControl.h"
#include "XModuleTalent.h"
#include "XModuleMessageQueue.h"
#include "XModuleEffect.h"
#include "XModuleUI.h"
#include "XCharacter.h"
#include "XMyPlayer.h"
#include "XNetPlayer.h"
#include "XNonPlayer.h"
#include "XBPartNonPlayer.h"

MImplementRootRTTI(XObject);

bool XObject::Create()
{
	m_bCreated = true;
	m_vFacingDir = vec3(0,1,0);

	RegisterModules();	// 모듈 추가

	return OnCreate();
}

void XObject::Destroy()
{
	OnDestroy();

	UnregisterModules();	// 모듈 제거

	m_bCreated = false;
}

void XObject::Update(float fDelta)
{
	OnUpdate(fDelta);
}

void XObject::PostUpdate(float fDelta)
{
	OnPostUpdate(fDelta);
}

void XObject::Render()
{
	OnRender();
}

void XObject::OnEvent(XEvent& msg)
{
	m_Modules.Event(msg);
}

void XObject::Event(XEvent& msg)
{
	OnEvent(msg);
}

XObject::~XObject()
{
	if (m_bCreated)
	{
		Destroy();
	}
}

bool XObject::IsActiveModule(int nID)
{
	return m_Modules.IsActiveModule(nID);
}

void XObject::ActiveModule( int nID, bool bActive )
{
	m_Modules.ActiveModule(nID, bActive);
}

XModule* XObject::GetModule(int nID)
{
	return m_Modules.GetModule(nID);
}

void XObject::OnUpdate(float fDelta)
{
	PFC_THISFUNC;
	m_Modules.UpdateModules(fDelta);
}

void XObject::OnPostUpdate(float fDelta)
{
	m_Modules.PostUpdateModules(fDelta);
}

vec3 XObject::GetPosition()
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if (pModuleEntity) return pModuleEntity->GetPosition();
	return vec3::ZERO;
}

vec3 XObject::GetDirection()
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if (pModuleEntity) return pModuleEntity->GetDirection();
	return vec3(0.0f, 1.0f, 0.0f);
}

vec3 XObject::GetUpvector()
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if(pModuleEntity) return pModuleEntity->GetUpvector();
	return vec3(0.0f, 0.0f, 1.0f);
}

mat4 XObject::GetWorldTransform()
{
	mat4 matTemp;
	matTemp.MakeIdentity();
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if (pModuleEntity) 
	{
		if(pModuleEntity->GetActor())
		{
			return pModuleEntity->GetActor()->GetWorldTransform();
		}
	}
	return matTemp;
}


void XObject::SetPosition(const vec3& pos)
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if (pModuleEntity) 
	{
		pModuleEntity->SetPosition(pos, this);
	}
}

void XObject::SetDirection(const vec3& dir)
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if (pModuleEntity) 
	{
		pModuleEntity->SetDirection(dir, this);
	}
}

void XObject::SetDirectionSmoothly( const vec3& dir, float fRotateSpeed )
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if (pModuleEntity) 
	{
		pModuleEntity->SetDirectionSmoothly(dir, fRotateSpeed);
	}
}

void XObject::SetUpVector( vec3& up )
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if (pModuleEntity) 
	{
		pModuleEntity->SetUpVector(up, this);
	}
}

void XObject::PostDelayEvent(XEvent& msg, unsigned int nDelayTime)
{
	// 만약 delay가 0이하이면 바로 메세지 처리
	if (nDelayTime <= 0)
	{
		OnEvent(msg);
		return;
	}

	if (GetModuleMessageQueue() != NULL)
	{
		GetModuleMessageQueue()->Push(msg, nDelayTime);
	}
}

void XObject::RemoveDelayEvent( XEventID nEventID )
{
	if (GetModuleMessageQueue() != NULL)
	{
		GetModuleMessageQueue()->Remove(nEventID);
	}
}

XModuleEntity* XObject::GetModuleEntity()
{ 
	return static_cast<XModuleEntity*>(GetModule(XMID_ENTITY)); 
}

XModuleMotion* XObject::GetModuleMotion()
{
	return static_cast<XModuleMotion*>(GetModule(XMID_MOTION)); 
}

XModuleAction* XObject::GetModuleAction()
{
	return static_cast<XModuleAction*>(GetModule(XMID_ACTION)); 
}

XModuleMovable* XObject::GetModuleMovable()
{
	return static_cast<XModuleMovable*>(GetModule(XMID_MOVABLE)); 
}

XModuleCollision* XObject::GetModuleCollision()
{
	return static_cast<XModuleCollision*>(GetModule(XMID_COLLISION)); 
}

XModuleUI* XObject::GetModuleUI()
{
	return static_cast<XModuleUI*>(GetModule(XMID_UI)); 
}

void XObject::SetVisible(bool bVisible)
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if (pModuleEntity) 
		pModuleEntity->SetVisible(bVisible);
}

bool XObject::IsVisible()
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if(pModuleEntity)
		return pModuleEntity->IsVisible();

	return false;
}

bool XObject::IsModuleEntity()
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if(pModuleEntity) return true;
	return false;
}

bool XObject::IsModuleAction()
{
	XModuleAction* pModuleAction = GetModuleAction();
	if(pModuleAction) return true;
	return false;
}

bool XObject::IsModuleMotion()
{
	XModuleMotion* pModuleMotion = GetModuleMotion();
	if(pModuleMotion) return true;
	return false;
}

bool XObject::IsModuleTalent()
{
	XModuleTalent* pModuleTalent = GetModuleTalent();
	if(pModuleTalent) return true;
	return false;
}

bool XObject::IsModuleEffect()
{
	XModuleEffect* pModuleEffect = GetModuleEffect();
	if( pModuleEffect) return true;
	return false;
}

XCharacter* XObject::GetActor()
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if(pModuleEntity) 
	{
		return pModuleEntity->GetActor();
	}

	return NULL;
}

XCharacter* XObject::GetCollisionActor()
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if(pModuleEntity) 
	{
		return pModuleEntity->GetCollisionActor();
	}

	return NULL;
}

float XObject::GetScale()
{
	XModuleEntity* pModuleEntity = GetModuleEntity();

	if (pModuleEntity != NULL)
	{
		return GetModuleEntity()->GetScale();
	}
	return 1.0f;
}

void XObject::SetTarget( const MUID& target)
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if (NULL == pModuleEntity) return;

	pModuleEntity->SetTarget( target);
}

const MUID& XObject::GetTarget()
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if (NULL == pModuleEntity) return MUID::ZERO;

	return pModuleEntity->GetTarget();
}

void XObject::SetExtraTarget( const MUID& target)
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if (NULL == pModuleEntity) return;

	pModuleEntity->SetExtraTarget( target);
}

const MUID& XObject::GetExtraTarget()
{
	XModuleEntity* pModuleEntity = GetModuleEntity();
	if (NULL == pModuleEntity) return MUID::ZERO;

	return pModuleEntity->GetExtraTarget();
}

float XObject::GetRadius()
{
	XModuleCollision* pModuleCollision = GetModuleCollision();
	if (pModuleCollision) return pModuleCollision->GetRadius();
	return 0.0f;
}

vec3 XObject::GetNamePos()
{
	XModuleUI* pModuleUI = GetModuleUI();
	if(pModuleUI)
		return pModuleUI->GetNamePos();

	return vec3::ZERO;
}

void XObject::AddModule( XModule *pModule, bool bInitialActive )
{
	m_Modules.AddModule(pModule, bInitialActive);
}

XMyPlayer* XObject::AsMyPlayer()
{
	_ASSERT(GetType() == XOBJ_MY_PLAYER);

	return static_cast<XMyPlayer*>(this);
}

XNetPlayer* XObject::AsNetPlayer()
{
	_ASSERT(GetType() == XOBJ_NET_PLAYER);

	return static_cast<XNetPlayer*>(this);
}

XPlayer* XObject::AsPlayer()
{
	_ASSERT(GetType() == XOBJ_NET_PLAYER || GetType() == XOBJ_MY_PLAYER || GetType() == XOBJ_MOCK_PLAYER);

	return static_cast<XPlayer*>(this);
}

XNonPlayer* XObject::AsNPC()
{
	_ASSERT(GetType() == XOBJ_NPC || GetType() == XOBJ_BPART_NPC);

	return static_cast<XNonPlayer*>(this);
}

XCutScenePlayer* XObject::AsCutScenePlayer()
{
	_ASSERT(GetType() == XOBJ_CUTSCENE_PLAYER);

	return reinterpret_cast<XCutScenePlayer*>(this);
}

XActor* XObject::AsActor()
{
	_ASSERT( (GetType() == XOBJ_NPC) || (GetType() == XOBJ_NET_PLAYER) || (GetType() == XOBJ_MY_PLAYER) || GetType() == XOBJ_BPART_NPC);

	return static_cast<XActor*>(this);
}

bool XObject::Reload()
{
	if(GetModuleMotion())
		GetModuleMotion()->Reload();
	if (GetModuleTalent())
		GetModuleTalent()->Reload();

	return true;
}

XBPartNonPlayer* XObject::AsBPartNPC()
{
	_ASSERT(GetType() == XOBJ_BPART_NPC);

	return static_cast<XBPartNonPlayer*>(this);
}

