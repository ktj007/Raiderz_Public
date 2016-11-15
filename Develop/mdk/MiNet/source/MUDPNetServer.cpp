#include "stdafx.h"
#include "MUDPNetServer.h"
#include "MPacketBuilder.h"
#include "MUDPReactor.h"
#include "MUDPCommandHandler.h"

namespace minet {

MUDPNetServer::MUDPNetServer()
: m_pReactor(NULL)
, m_pCommandHandler(NULL)
{
	m_pCommandHandler = new MUDPCommandHandler<MUDPNetServer>(this);
	
	m_pReactor = new MUDPReactor();
	m_pReactor->SetCommandHandler(m_pCommandHandler);
}

MUDPNetServer::~MUDPNetServer()
{	
	SAFE_DELETE(m_pReactor);	
	SAFE_DELETE(m_pCommandHandler);
}

bool MUDPNetServer::Create(int nPort)
{
	return m_pReactor->Create(nPort);
}

void MUDPNetServer::Destroy(void)
{
	m_pReactor->Destory();
}

bool MUDPNetServer::PostCommand(int nIP, int nPort, MCommand* pCommand)
{
	if (pCommand == NULL)	return false;
	auto_ptr<MCommand> selfDestruct(pCommand);

	_ASSERT(pCommand->m_pCommandDesc->GetFlow() != MCF_C2S);	


	int nCmdSize = pCommand->GetSize();
	if ((nCmdSize <= 0) || (nCmdSize >= MAX_PACKET_SIZE)) 
	{
		_ASSERT(0);
		return false;
	}


	int nPacketSize = nCmdSize + sizeof(MPacketHeader);
	MCommandMsg* pMsg = (MCommandMsg*)malloc(nPacketSize);	// malloc로 메모리 할당

	if (pMsg == NULL)
	{
		return false;
	}


	MServerPacketBuilder builder;
	nPacketSize = builder.BuildPacket((BYTE*)pMsg, nPacketSize, pCommand, NULL);

	if (nPacketSize <= 0)
	{
		_ASSERT(0);
		free(pMsg);

		return false;
	}


	if (!m_pReactor->Send(nIP, nPort, (char*)pMsg, nPacketSize))
	{
		free(pMsg);
		return false;
	}

	return true;
}

}
