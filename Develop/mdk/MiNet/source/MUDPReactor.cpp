#include "stdafx.h"
#include "MUDPReactor.h"
#include "MCommandBuilder.h"
#include "MUDPCommandHandler.h"

namespace minet {

MUDPReactor::MUDPReactor()
: m_pCommandHandler(NULL)
{	
	m_UDPSocket.SetCallbackContext(this);
	m_UDPSocket.SetRecvCallback(SocketRecvEvent);
}

MUDPReactor::~MUDPReactor()
{
	m_pCommandHandler = NULL;
	m_UDPSocket.Destroy();
}

bool MUDPReactor::Create(int nPort)
{
	return m_UDPSocket.Create(false, nPort);
}

void MUDPReactor::Destory(void)
{
	m_UDPSocket.Destroy();
}

bool MUDPReactor::Send(int nIP, int nPort, char* pPacket, int nPacketSize)
{
	return m_UDPSocket.Send(nIP, nPort, pPacket, nPacketSize);
}

void MUDPReactor::SetCommandHandler(IUDPCommandHandler* pCommandHandler)
{
	m_pCommandHandler = pCommandHandler;
}

bool MUDPReactor::OnRecv(DWORD dwIP, WORD wPort, char* pPacket, DWORD dwSize)
{
	if (m_pCommandHandler == NULL)	return false;


	// 커맨드 추출
	MCommandBuilder builder;
	MCommandList commandList;	

	builder.BuildCommand((char*)pPacket, dwSize, commandList, NULL);


	// 커맨드 처리
	while(!commandList.empty())
	{		
		MCommand* pCmd = commandList.front();
		
		m_pCommandHandler->OnDo(dwIP, wPort, pCmd);

		delete pCmd;
		commandList.pop_front();
	}


	return true;
}

bool MUDPReactor::SocketRecvEvent(void* pCallbackContext, DWORD dwIP, WORD wRawPort, char* pPacket, DWORD dwSize)
{
	if (pCallbackContext == NULL)	return false;

	MUDPReactor* pThis = (MUDPReactor*)pCallbackContext;
	WORD wPort = ntohs(wRawPort);

	return pThis->OnRecv(dwIP, wPort, pPacket, dwSize);
}

}
