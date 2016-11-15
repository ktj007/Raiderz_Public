#include "stdafx.h"
#include "GSectorAIEventInvoker.h"

GSectorAIEventInvoker::GSectorAIEventInvoker(MUID uidField)
: m_uidField(uidField)
, m_isPlayerEmpty(true)
, m_isLazedNotify(false)
{
}

GSectorAIEventInvoker::~GSectorAIEventInvoker()
{
	// do nothing
}

void GSectorAIEventInvoker::EnterPlayer(MPoint ptCell)
{
	_ASSERT(0 <= ptCell.x);
	_ASSERT(0 <= ptCell.y);

	if (!m_isPlayerEmpty)
	{
		return;
	}

	m_isPlayerEmpty = false;


	// AI Event
	InvokeEnterPlayer(m_uidField, ptCell.x, ptCell.y);
}

void GSectorAIEventInvoker::LeavePlayer(MPoint ptCell, int nPlayerQty)
{
	_ASSERT(0 <= ptCell.x);
	_ASSERT(0 <= ptCell.y);

	if (0 < nPlayerQty)
	{
		return;
	}

	m_isPlayerEmpty = true;


	// AI Event	
	InvokeLeavePlayer(m_uidField, ptCell.x, ptCell.y);
}

void GSectorAIEventInvoker::MoveNPC(MUID uidNPC, bool bPlayerEmptyInNeighborCells)
{
	if (bPlayerEmptyInNeighborCells)
	{
		InvokeMoveNPCEmptyPlayer(uidNPC);		
	}
	else
	{
		InvokeMoveNPCBePlayer(uidNPC);
	}	
}

void GSectorAIEventInvoker::LazedNotifyBegin(void)
{
	_ASSERT(!m_isLazedNotify);

	m_isLazedNotify = true;
}

void GSectorAIEventInvoker::LazedNotifyEnd(void)
{
	_ASSERT(m_isLazedNotify);

	while(!m_queueEvent.empty())
	{
		InvokeEvent(m_queueEvent.front());
		m_queueEvent.pop();
	}

	m_isLazedNotify = false;
}

void GSectorAIEventInvoker::Invoke(GAIEvent aiEvent)
{
	if (m_isLazedNotify)
	{
		m_queueEvent.push(aiEvent);
	}
	else
	{
		InvokeEvent(aiEvent);
	}
}

void GSectorAIEventInvoker::InvokeEnterPlayer(MUID uidField, int nSectorX, int nSectorY)
{
	GAIEvent event;
	event.eEventID = GAIEvent::EVENT_ID_SECTOR_ENTER_PLAYER;
	event.uidField = uidField;
	event.nSectorX = nSectorX;
	event.nSectorY = nSectorY;

	Invoke(event);	
}

void GSectorAIEventInvoker::InvokeLeavePlayer(MUID uidField, int nSectorX, int nSectorY)
{
	GAIEvent event;
	event.eEventID = GAIEvent::EVENT_ID_SECTOR_LEAVE_PLAYER;
	event.uidField = uidField;
	event.nSectorX = nSectorX;
	event.nSectorY = nSectorY;

	Invoke(event);
}

void GSectorAIEventInvoker::InvokeMoveNPCBePlayer(MUID uidNPC)
{
	GAIEvent event;
	event.eEventID = GAIEvent::EVENT_ID_SECTOR_MOVE_BE_PLAYER;
	event.uidNPC = uidNPC;

	Invoke(event);
}

void GSectorAIEventInvoker::InvokeMoveNPCEmptyPlayer(MUID uidNPC)
{
	GAIEvent event;
	event.eEventID = GAIEvent::EVENT_ID_SECTOR_MOVE_EMPTY_PLAYER;
	event.uidNPC = uidNPC;

	Invoke(event);
}
