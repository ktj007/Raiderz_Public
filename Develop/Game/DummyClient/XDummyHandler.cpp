#include "stdafx.h"
#include "XDummyHandler.h"
#include "XBirdDummyAgent.h"
#include "XDummyPattern.h"


minet::MCommandResult XXMLDummyHandler::OnCommand( XBirdDummyAgent* pAgent, MCommand* pCmd )
{
	XBirdClient* pClient = pAgent->GetClient();

	if (m_pPattern)
	{
		m_pPattern->OnCommand(pClient, pCmd);
	}

	return CR_FALSE;
}




XDummyHandlerMgr::~XDummyHandlerMgr()
{
	Clear();
}

void XDummyHandlerMgr::Clear()
{
	while (!empty())
	{
		delete (*begin()).second;
		erase(begin());
	}
}

void XDummyHandlerMgr::Add( XDummyHandler* pNewHandler )
{
	wstring strKey = pNewHandler->GetName();
	insert(value_type(strKey, pNewHandler));
}

XDummyHandler* XDummyHandlerMgr::GetHandler( const wchar_t* szName )
{
	iterator itor = find(szName);
	if (itor == end()) return NULL;
	return (*itor).second;
}


