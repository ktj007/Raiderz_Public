#include "stdafx.h"
#include "GAIEventMgr.h"
#include "GAIEventHandler.h"
#include "GAIProfiler.h"
#include "GAIEventNotifier.h"

GAIEventMgr::GAIEventMgr()
{
	m_pEventNotifier = new GAIEventNotifier;
}

GAIEventMgr::~GAIEventMgr()
{
	SAFE_DELETE(m_pEventNotifier);
}

void GAIEventMgr::Notify(GAIEvent aiEvent)
{
	m_pEventNotifier->Notify(aiEvent);
}

void GAIEventMgr::AddEventHandler(MUID uidNPC, GAIEventHandler* pHandler)
{
	m_pEventNotifier->AddEventHandler(uidNPC, pHandler);
}

void GAIEventMgr::RemoveEventHandler(MUID uidNPC, GAIEventHandler* pHandler)
{
	m_pEventNotifier->RemoveEventHandler(uidNPC, pHandler);
}
