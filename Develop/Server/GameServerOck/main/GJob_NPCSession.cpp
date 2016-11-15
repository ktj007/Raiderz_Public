#include "StdAfx.h"
#include "GJob_NPCSession.h"
#include "GEntityNPC.h"
#include "GField.h"
#include "GFieldNPCSessionMgr.h"

GJob_NPCSession::GJob_NPCSession(GModuleAI* pmAI, MUID uidSession)
	:GJob(pmAI)
	, m_uidSession(uidSession)
{
}

GJob_NPCSession::~GJob_NPCSession(void)
{
}

string GJob_NPCSession::GetParamString() const 
{
	return "SessionJob";
}

GJobResult GJob_NPCSession::OnRun( float fDelta )
{
	GField* pField = GetOwnerNPC()->GetField();
	VALID_RET(pField, JR_CANCELED);
	if (pField->GetSession()->Find(m_uidSession))
		return JR_RUNNING;

	return JR_COMPLETED;
}
