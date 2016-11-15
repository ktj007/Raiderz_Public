#include "stdafx.h"
#include "XRequestCommandFilter.h"

void XRequestCommandFilter::AddFilter( int nRequestCommandID, int nResponseCommandID )
{
	m_FilterMap.insert(map<int, int>::value_type(nResponseCommandID, nRequestCommandID));
	m_RequestCommandFilter.insert(nRequestCommandID);
}

bool XRequestCommandFilter::CheckRequestEnable( int nRequestCommandID )
{
	if (m_CurrentSendRequestCommands.find(nRequestCommandID) != m_CurrentSendRequestCommands.end())
	{
		return false;
	}

	if (m_RequestCommandFilter.find(nRequestCommandID) != m_RequestCommandFilter.end())
	{
		m_CurrentSendRequestCommands.insert(nRequestCommandID);
	}

	return true;
}

void XRequestCommandFilter::OnResponse( int nResponseCommandID )
{
	map<int, int>::iterator itor = m_FilterMap.find(nResponseCommandID);
	if (itor != m_FilterMap.end())
	{
		int nRequestCommandID = (*itor).second;

		set<int>::iterator itor2 = m_CurrentSendRequestCommands.find(nRequestCommandID);
		if (itor2 != m_CurrentSendRequestCommands.end())
		{
			m_CurrentSendRequestCommands.erase(itor2);
		}
	}
}

void XRequestCommandFilter::ClearSendRequestCommands()
{
	m_CurrentSendRequestCommands.clear();
}