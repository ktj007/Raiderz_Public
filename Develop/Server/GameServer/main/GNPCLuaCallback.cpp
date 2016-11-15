#include "StdAfx.h"
#include "GNPCLuaCallback.h"
#include "GEntityNPC.h"
#include "GScriptManager.h"
#include "GScriptGroup_NPC.h"

GNPCLuaCallback::GNPCLuaCallback( GEntityNPC* pOwner )
: m_pOwner(pOwner)
{
	m_pOwner->AttachObserver(this);
}

GNPCLuaCallback::~GNPCLuaCallback(void)
{
	m_pOwner->DetachObserver(this);
}

void GNPCLuaCallback::OnCombatBegin(GEntityActor* pOwner)
{
	GetScriptNPC()->OnCombatBegin(m_pOwner->GetDelegator());
}

void GNPCLuaCallback::OnCombatEnd(GEntityActor* pOwner)
{
	GetScriptNPC()->OnCombatEnd(m_pOwner->GetDelegator());
}
