#include "stdafx.h"
#include "MUDPNetClient.h"
#include "MPacketBuilder.h"
#include "MUDPReactor.h"
#include "MUDPCommandHandler.h"


namespace minet {

MUDPNetClient::MUDPNetClient()
: m_pReactor(NULL)
, m_pCommandHandler(NULL)
, m_nServerIP(0)
, m_nServerPort(0)
{
	m_pCommandHandler = new MUDPCommandHandler<MUDPNetClient>(this);
	
	m_pReactor = new MUDPReactor();
	m_pReactor->SetCommandHandler(m_pCommandHandler);
}

MUDPNetClient::~MUDPNetClient()
{	
	SAFE_DELETE(m_pReactor);
	SAFE_DELETE(m_pCommandHandler);
}

bool MUDPNetClient::Create(int nPort, int nServerIP, int nServerPort)
{
	SetServerAddress(nServerIP, nServerPort);

	return m_pReactor->Create(nPort);
}

void MUDPNetClient::Destroy(void)
{
	m_pReactor->Destory();
}

void MUDPNetClient::SetServerAddress(int nServerIP, int nServerPort)
{
	m_nServerIP = nServerIP;
	m_nServerPort = nServerPort;
}

bool MUDPNetClient::PostCommand(MCommand* pCommand)
{
	if (pCommand == NULL)	return false;
	auto_ptr<MCommand> selfDestruct(pCommand);

	_ASSERT(pCommand->m_pCommandDesc->GetFlow() != MCF_S2C);	


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


	MClientPacketBuilder builder;
	nPacketSize = builder.BuildPacket((char*)pMsg, nPacketSize, pCommand, NULL);

	if (nPacketSize <= 0)
	{
		_ASSERT(0);
		free(pMsg);

		return false;
	}


	if (!m_pReactor->Send(m_nServerIP, m_nServerPort, (char*)pMsg, nPacketSize))
	{
		free(pMsg);
		return false;
	}

	return true;
}

}
