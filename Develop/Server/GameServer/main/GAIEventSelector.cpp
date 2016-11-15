#include "stdafx.h"
#include "GAIEventSelector.h"
#include "GAIProfiler.h"

void GAIEventSelector::OnNotify(GAIEvent aiEvent)
{
	if (m_pHandler != NULL && IsSelectEvent(aiEvent))
	{
		m_pHandler->OnNotify(aiEvent);
	}
}

void GAIEventSelector::RegistHandler(GAIEventHandler* pHandler)
{
	_ASSERT(pHandler != NULL);
	m_pHandler = pHandler;
}

void GAIEventSelector::UnregistHandler(void)
{
	m_pHandler = NULL;
}
