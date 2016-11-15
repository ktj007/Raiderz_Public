#include "stdafx.h"
#include "GBehaviorState.h"
#include "GMessageID.h"
#include "GModuleAI.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GNPCDebugger.h"
#include "GScriptGroup_NPC.h"

GBehaviorState::GBehaviorState( GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM ) 
: GState<GModuleAI>(pOwner, pFSM)
, m_pOwnerNPC(pOwner->GetOwnerNPC())
{


}
