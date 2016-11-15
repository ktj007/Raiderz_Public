#include "stdafx.h"
#include "GFieldAIEventInvoker.h"
#include "GField.h"

GFieldAIEventInvoker::GFieldAIEventInvoker(GField* pField)
: m_pOwnerField(pField)
, m_isPlayerEmpty(true)
{
	_ASSERT(pField != NULL);
}

GFieldAIEventInvoker::~GFieldAIEventInvoker()
{
	// do nothing
}

void GFieldAIEventInvoker::EnterPlayer(void)
{
	if (!m_isPlayerEmpty)
	{
		return;
	}

	m_isPlayerEmpty = false;


	// AI Event
	MUID uidField = m_pOwnerField->GetUID();	
	InvokeEnterPlayer(uidField);
}

void GFieldAIEventInvoker::LeavePlayer(void)
{
	if (0 < m_pOwnerField->GetPlayerQty())
	{
		return;
	}

	m_isPlayerEmpty = true;
	

	// AI Event
	MUID uidField = m_pOwnerField->GetUID();
	InvokeLeavePlayer(uidField);
}

void GFieldAIEventInvoker::InvokeEnterPlayer(MUID uidField)
{
	GAIEvent event;
	event.eEventID = GAIEvent::EVENT_ID_FIELD_ENTER_PLAYER;
	event.uidField = uidField;

	InvokeEvent(event);
}

void GFieldAIEventInvoker::InvokeLeavePlayer(MUID uidField)
{
	GAIEvent event;
	event.eEventID = GAIEvent::EVENT_ID_FIELD_LEAVE_PLAYER;
	event.uidField = uidField;

	InvokeEvent(event);
}
