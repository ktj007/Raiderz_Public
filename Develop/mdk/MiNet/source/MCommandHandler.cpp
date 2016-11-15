#include "stdafx.h"
#include "MCommandHandler.h"
#include "MCommandCommunicator.h"

namespace minet {


void MCommandHandler::SetCmdHandler(int nID, MCommandHanderFunc* fnFunc)
{
	m_pCC->SetCommandHandler(nID, this, fnFunc);
	m_vecCmdID.push_back(nID);
}

MCommandHandler::~MCommandHandler() 
{ 
	for (int i = 0; i < (int)m_vecCmdID.size(); i++)
	{
		m_pCC->SetCommandHandler(m_vecCmdID[i], NULL, NULL);
	}
	m_pCC=NULL; 
	m_vecCmdID.clear();
}







} // namespace minet