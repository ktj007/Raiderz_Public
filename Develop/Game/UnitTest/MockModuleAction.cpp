#include "stdafx.h"
#include "MockModuleAction.h"

MockModuleAction::MockModuleAction( XObject* pOwner/*=NULL*/ ) : XModuleAction(pOwner)
{
	
}

void MockModuleAction::OnInitialized()
{

}

void MockModuleAction::Init( XModuleMotion* pModuleMotion /*= NULL*/, XModuleMovable* pModuleMovable /*= NULL*/ )
{
	m_pModuleMotion = pModuleMotion;
	m_pModuleMovable = pModuleMovable;
}
