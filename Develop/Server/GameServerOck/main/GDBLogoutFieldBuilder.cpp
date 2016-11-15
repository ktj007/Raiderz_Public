#include "stdafx.h"
#include "GDBLogoutFieldBuilder.h"
#include "GEntityPlayer.h"
#include "GField.h"
#include "GGlobal.h"
#include "GFieldMgr.h"
#include "GPlayerFieldDynamic.h"


GDBLogoutFieldBuilder::GDBLogoutFieldBuilder( GEntityPlayer* pPlayer) : m_pPlayer(pPlayer), m_pField(NULL)
{
	_ASSERT(NULL != pPlayer);
}


void GDBLogoutFieldBuilder::Build()
{
	if (NULL == m_pPlayer)
		return;

	Init();

	if (!SetField())
		 return;
	
	SetPlayerPos();
	SetSharedFieldID();
	SetDyanmicField();	
}

int GDBLogoutFieldBuilder::GetSharedFieldID()
{
	return m_nSharedFieldID;
}

MUID& GDBLogoutFieldBuilder::GetDyanmicFieldGroupUID()
{
	return m_Field.uidDynamicFieldGroup;
}

int GDBLogoutFieldBuilder::GetDynamicFieldID()
{
	return m_Field.nDynamicFieldID;
}

vec3& GDBLogoutFieldBuilder::GetEnterPos()
{
	return m_Field.vEnterPos;
}


bool GDBLogoutFieldBuilder::IsEnteringToOtherField()
{
	// 다른 필드로 이동중일때는 팰드 정보가 NULL일 수 있다.
	return NULL == m_pPlayer->GetField();
}


bool GDBLogoutFieldBuilder::SetField()
{
	if (IsEnteringToOtherField())
		m_pField = gmgr.pFieldMgr->GetField(m_pPlayer->GetPlayerField().GetFieldEntry().GetReservedWarpField());
	else
		m_pField = m_pPlayer->GetField();;

	if (NULL == m_pField)
		return false;

	return true;
}


void GDBLogoutFieldBuilder::SetPlayerPos()
{
	if (IsEnteringToOtherField())
	{
		m_vPlayerPos = m_pPlayer->GetPlayerField().GetFieldEntry().GetReservedWarpPos();
		return;
	}

	m_vPlayerPos = m_pPlayer->GetPos();
}

void GDBLogoutFieldBuilder::SetSharedFieldID()
{
	m_nSharedFieldID = m_pField->GetID();
}

void GDBLogoutFieldBuilder::Init()
{
	m_nSharedFieldID				= 0;
	m_Field.nDynamicFieldID			= 0;
	m_Field.uidDynamicFieldGroup	= 0;
	m_Field.vEnterPos				= vec3::ZERO;
}

void GDBLogoutFieldBuilder::SetDyanmicField()
{
	if (m_pField->IsDynamicField())
	{
		GDynamicField* pDynamicField = static_cast<GDynamicField*>(m_pField);

		m_Field.uidDynamicFieldGroup	= pDynamicField->GetGroupUID();
		m_Field.nDynamicFieldID			= pDynamicField->GetID();
		m_nSharedFieldID				= m_pPlayer->GetPlayerField().GetFieldDynamic().GetEnterPos().nFieldID;
		m_Field.vEnterPos				= m_pPlayer->GetPlayerField().GetFieldDynamic().GetEnterPos().vPoint;
	}
}

vec3& GDBLogoutFieldBuilder::GetPlayerPos()
{
	return m_vPlayerPos;
}