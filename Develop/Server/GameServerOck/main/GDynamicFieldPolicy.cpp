#include "StdAfx.h"
#include "GDynamicFieldPolicy.h"
#include "GDynamicFieldMaster.h"

GDynamicFieldPolicy::GDynamicFieldPolicy(GDynamicFieldMaster* pOwner)
: m_pOwner(pOwner)
, m_isVirginField(true)
{
}

GDynamicFieldPolicy::~GDynamicFieldPolicy()
{
}

void GDynamicFieldPolicy::Create()
{
	OnCreate();
}

void GDynamicFieldPolicy::Destroy()
{
	OnDestroy();
}

void GDynamicFieldPolicy::Update(float fDelta)
{
	OnUpdate(fDelta);

	if (CheckExistPlayer()) return;
}

bool GDynamicFieldPolicy::EnterPlayer(GEntityPlayer* pPlayer)
{
	m_isVirginField = false;
	return OnEnterPlayer(pPlayer);
}

void GDynamicFieldPolicy::LeavePlayer(GEntityPlayer* pPlayer)
{
	OnLeavePlayer(pPlayer);
}

bool GDynamicFieldPolicy::CheckExistPlayer()
{
	if (m_isVirginField)				return false;
	if (m_pOwner->GetPlayerQty() != 0)	return false;

	m_pOwner->RunSelfDestructor();

	return true;
}

FIELD_ENTER_FAIL_TYPE GDynamicFieldPolicy::IsEnterable()
{
	return ENTERFAIL_OK;
}