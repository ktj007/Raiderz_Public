#include "stdafx.h"
#include "MNetServer.h"
#include "MCommandManager.h"
#include "MLink.h"
#include "MCmdHandler_NetServer.h"
#include "MAcceptor.h"
#include "MDisconnector.h"
#include "MSender.h"
#include "MReceiver.h"
#include "MCommandTable.h"
#include "MPacket.h"
#include "MCommandCommunicator.h"

namespace minet {

class MCommandServerAcceptListener : public MAcceptListener
{
private:
	MNetServer& m_CommandServer;
public:
	MCommandServerAcceptListener(MNetServer& server) : m_CommandServer(server) {}
	virtual void OnAccept(MServerTcpSocket& tcp_socket)
	{
		MUID uidAlloc = m_CommandServer.NewUID();

		MLink* pNewLink = new MLink(m_CommandServer.GetUID(), 
									uidAlloc, 
									tcp_socket, 
									m_CommandServer.GetCommandManager());
		tcp_socket.SetLink(uidAlloc);

		m_CommandServer.m_LinkMap.Add(pNewLink);

		MCommand* pNewCommand = m_CommandServer.NewCommand(MC_LOCAL_NET_ONACCEPT, m_CommandServer.GetUID());
		pNewCommand->AddParameter(new MCommandParameterUID(uidAlloc));
		m_CommandServer.Post(pNewCommand);
	}
};

class MCommandServerDisconnectListner : public MDisconnectListener
{
private:
	MNetServer& m_CommandServer;
public:
	MCommandServerDisconnectListner(MNetServer& server) : m_CommandServer(server) {}
	virtual void OnDisconnect(MUID uidLink)
	{
		if (uidLink == MUID::ZERO)
		{
			// 사실 이것이 나오면 안됨. - birdkr
			return;
		}

		MUID outReAllocedUID = MUID::ZERO; ///< UID가 ReAlloc됐을 경우를 대비
		bool bRet = m_CommandServer.m_LinkMap.Delete(uidLink, &outReAllocedUID);
		if (bRet == false) return;

		uidLink = (MUID::ZERO == outReAllocedUID) ? uidLink : outReAllocedUID;

		MCommand* pNewCommand = m_CommandServer.NewCommand(MC_LOCAL_NET_CLEAR, m_CommandServer.GetUID());
		pNewCommand->AddParameter(new MCommandParameterUID(uidLink));
		m_CommandServer.Post(pNewCommand);
	}
};

class MCommandServerSendListener : public MSendListener
{
private:
	MNetServer& m_CommandServer;
public:
	MCommandServerSendListener(MNetServer& server) : m_CommandServer(server) {}
	virtual void OnSend()
	{
	}
};

class MCommandServerRecvListener : public MRecvListener
{
private:
	MNetServer& m_CommandServer;
public:
	MCommandServerRecvListener(MNetServer& server) : m_CommandServer(server) {}
	virtual void OnDisconnect(MUID uidLink) override
	{
		MLink* pLink = m_CommandServer.m_LinkMap.GetLink(uidLink);
		if (pLink == NULL)
		{
			pLink = m_CommandServer.m_LinkMap.GetLinkFromReAllocTable(uidLink);
			if (pLink == NULL)
				return;
		}

		MCommand* pNewCommand = m_CommandServer.NewCommand(MC_LOCAL_NET_ONDISCONNECT, m_CommandServer.GetUID());
		pNewCommand->AddParameter(new MCommandParameterUID(pLink->GetUID()));
		m_CommandServer.Post(pNewCommand);

		pLink->Drop();
	}
	virtual void OnHardDisconnect(MUID uidLink) override
	{
		MLink* pLink = m_CommandServer.m_LinkMap.GetLink(uidLink);
		if (pLink == NULL)
		{
			pLink = m_CommandServer.m_LinkMap.GetLinkFromReAllocTable(uidLink);
			if (pLink == NULL)
				return;
		}

		MCommand* pNewCommand = m_CommandServer.NewCommand(MC_LOCAL_NET_ONDISCONNECT_HARD, m_CommandServer.GetUID());
		pNewCommand->AddParameter(new MCommandParameterUID(pLink->GetUID()));
		m_CommandServer.Post(pNewCommand);

		pLink->Drop();
	}
	virtual void OnRecv(MServerTcpSocket& tcpsocket, MUID uidLink, BYTE* pData, DWORD dwPacketLen)
	{
		m_CommandServer.m_TrafficMonitor.RecordRecv(dwPacketLen);

		{
			MLink* pLink = m_CommandServer.m_LinkMap.GetLink(uidLink);
			if (pLink == NULL)
			{
				pLink = m_CommandServer.m_LinkMap.GetLinkFromReAllocTable(uidLink);
				if (pLink == NULL)
					return;
			}

			if (pLink->IsAllowed() == false) 
			{
				pLink->Drop();
				return;
			}


			// New Cmd Buffer ////////////////
			MPacketHeader* pPacket = (MPacketHeader*)(pData);
			MCommandStream* pCmdStream = pLink->GetCommandStream();

			if (!pCmdStream->Read((char*)pPacket, dwPacketLen))
			{
				// 패킷이 제대로 안오면 끊어버린다.
				pLink->SetAllowed(false);
				pLink->Disconnect();
				m_CommandServer.IncreaseDisconnCountByWrongPacket();

				pLink->Drop();

				return;
			}

			pLink->SetRecvTime(timeGetTime());

			MUID uidReceiver = m_CommandServer.GetUID();
			MUID uidSender = pLink->GetUID();

			while(MCommand* pCmd = pCmdStream->GetCommand()) 
			{
				if (pCmd->m_pCommandDesc == NULL)
				{
					_ASSERT(0);	// Client->Server 커맨드가 잘못 날라옴
					SAFE_DELETE(pCmd);
					continue;
				}

				if (pCmd->m_pCommandDesc->GetFlow() == MCF_S2C)
				{
					_ASSERT(0);	// Client->Server 커맨드가 잘못 날라옴
					SAFE_DELETE(pCmd);
					continue;
				}

				pCmd->m_Sender = uidSender;
				pCmd->m_Receiver = uidReceiver;

				if ((pCmd->m_pCommandDesc->IsFlag(MCDT_MACHINE2MACHINE)==true) && (pCmd->m_Receiver != m_CommandServer.GetUID()) )
				{
					_ASSERT(0);	// Client->Server 커맨드가 잘못 날라옴
					SAFE_DELETE(pCmd);
					continue;
				}

				m_CommandServer.Post(pCmd);
			}

			pLink->Drop();
		}
	}
};

void MNetServer::SendCommand( MCommand* pCommand )
{
	_ASSERT(pCommand->GetReceiverUID().Value);
	_ASSERT(pCommand->m_pCommandDesc->GetFlow() != MCF_C2S);

	int nCmdSize = pCommand->GetSize();
	if ((nCmdSize <= 0) || (nCmdSize >= MAX_PACKET_SIZE)) 
	{
		_ASSERT(0);
		return;
	}

	int nPacketSize = nCmdSize + sizeof(MPacketHeader);
	MCommandMsg* pMsg = (MCommandMsg*)malloc(nPacketSize);	// malloc로 메모리 할당

	if (pMsg == NULL)
	{
		return;
	}

	nPacketSize = m_PacketBuilder.BuildRawPacket((BYTE*)pMsg, nPacketSize, pCommand);

	if (nPacketSize <= 0)
	{
		_ASSERT(0);
		free(pMsg);

		return;
	}

	if (pCommand->HasAdditiveReceivers() == false)
	{
		if (!SendCommandToLink(pCommand->m_Receiver, pCommand, pMsg, nPacketSize))
		{
			free(pMsg);
		}
	}
	else
	{
		// 추가 receiver에게 먼저 보냄
		for (list<MUID>::iterator itor = pCommand->GetAdditiveReceiverListBegin(); 
			itor != pCommand->GetAdditiveReceiverListEnd(); ++itor)
		{
			_ASSERTE(pCommand->m_Receiver != *itor && "이거 뜨면 저(birdkr)한테 얘기해주세요");

			MCommandMsg* pCloneMsg = (MCommandMsg*)malloc(nPacketSize);	// malloc로 메모리 할당

			if (pCloneMsg == NULL)
			{
				break;
			}

			memcpy_s(pCloneMsg, nPacketSize, pMsg, nPacketSize);

			if (!SendCommandToLink((*itor), pCommand, pCloneMsg, nPacketSize))
			{
				free(pCloneMsg);
			}
		}

		// receiver에게 보냄
		if (!SendCommandToLink(pCommand->m_Receiver, pCommand, pMsg, nPacketSize))
		{
			free(pMsg);
		}
	}
}

bool MNetServer::SendCommandToLink( MUID uidReceiver, MCommand* pCommand, MCommandMsg* pMsg, int nPacketSize )
{
	MLink* pLink = m_LinkMap.GetLink(uidReceiver);
	if (pLink == NULL) return false;

	MPacketCrypter* pPacketCrypter = NULL;

	if(pCommand->m_pCommandDesc->IsFlag(MCDT_NON_ENCRYPTED) == false)
	{
		pPacketCrypter = pLink->GetPacketCrypter();
	}

	m_PacketBuilder.BuildPacketEncryptionAndChecksum(pMsg, pCommand, nPacketSize, pPacketCrypter);

	m_TrafficMonitor.RecordSend(nPacketSize);

	pLink->Send((BYTE*)(pMsg), nPacketSize);

	pLink->Drop();

	return true;
}


MNetServer::MNetServer(const MNetServerDesc& desc) : m_pTCPServer(new MTcpServer())
{
	m_Desc = desc;

	m_pCmdHandler = new MCmdHandler_NetServer(this);

	m_pAcceptListener = new MCommandServerAcceptListener(*this);
	m_pDisconnectListner = new MCommandServerDisconnectListner(*this);
	m_pSendListener = new MCommandServerSendListener(*this);
	m_pRecvListener = new MCommandServerRecvListener(*this);

	m_pTCPServer->SetAcceptorListener(m_pAcceptListener);
	m_pTCPServer->SetDisconnectorListener(m_pDisconnectListner);
	m_pTCPServer->SetSenderListener(m_pSendListener);
	m_pTCPServer->SetReceiverListener(m_pRecvListener);

	if (m_Desc.bDelayTest)
	{
		m_DelayTester.SetDelay(m_Desc.nTestDelayTime);
	}

	m_nHeartBeatLastTime = 0;

	m_nDisconnCountByHeartBeat = 0;
	m_nDisconnCountByWrongPacket = 0;
}

MNetServer::~MNetServer()
{
	Destroy();
}

bool MNetServer::Create(int nPort, const bool bReuse/*=false*/, MUID uidSeed/*=MUID::ZERO*/, int nSocketPoolSize/*=2000*/, int nSendPendingLimitCount /*= INT_MAX*/, const char* szMyNetworkCardIP/*=""*/)
{
	m_uidGenerator.SetSeed(uidSeed);

	MUID new_server_uid = NewUID();
	SetUID(new_server_uid);

	return m_pTCPServer->Start(AF_INET, nPort, nSocketPoolSize, nSendPendingLimitCount, szMyNetworkCardIP);
}

void MNetServer::Destroy()
{
	SAFE_DELETE(m_pTCPServer);
	SAFE_DELETE(m_pCmdHandler);

	SAFE_DELETE(m_pDisconnectListner);
	SAFE_DELETE(m_pAcceptListener);
	SAFE_DELETE(m_pSendListener);
	SAFE_DELETE(m_pRecvListener);
}

void MNetServer::Disconnect( MUID uidLink )
{
	MLink* pLink = m_LinkMap.GetLink(uidLink);
	if (pLink == NULL)
	{
		pLink = m_LinkMap.GetLinkFromReAllocTable(uidLink);
		if (pLink == NULL)
			return;
	}

	pLink->Disconnect();
	pLink->Drop();
}

void MNetServer::HardDisconnect(MUID uidLink)
{
	MLink* pLink = m_LinkMap.GetLink(uidLink);
	if (pLink == NULL)
	{
		pLink = m_LinkMap.GetLinkFromReAllocTable(uidLink);
		if (pLink == NULL)
			return;
	}

	pLink->HardDisconnect();
	pLink->Drop();
}


void MNetServer::GetTotalTraffic( int64& noutRecvSize, int64& noutSendSize )
{
	m_TrafficMonitor.GetTotalTraffic(noutRecvSize, noutSendSize);
}

void MNetServer::GetSecondTraffic( uint32& noutRecvSize, uint32& noutSendSize )
{
	m_TrafficMonitor.GetSecondTraffic(noutRecvSize, noutSendSize);
}

void MNetServer::OnPrepareRun( void )
{
	__super::OnPrepareRun();
	UpdateKeepingAlive();

	if (m_Desc.bDelayTest)
	{
		uint32 nNowTime = timeGetTime();
		m_DelayTester.OnRun(m_CommandManager, nNowTime);
	}

	return;
}


void MNetServer::UpdateKeepingAlive()
{
	if (!m_Desc.bHeartBeat) return;

	uint32 nNowTime = timeGetTime();

	if (nNowTime - m_nHeartBeatLastTime < m_Desc.nHeartBeatTickTime) return;
	m_nHeartBeatLastTime = nNowTime;

	{
		cml2::MCriticalSection::Guard guard(m_LinkMap.m_csLock);

		vector<MUID> uidFailedLinks;

		for (MLinkMap::LINK_MAP::iterator itor = m_LinkMap.GetLinkMap().begin();
			itor != m_LinkMap.GetLinkMap().end(); ++itor)
		{
			MLink* pLink = (*itor).second;

			if (pLink->CheckAlive(nNowTime, m_Desc.nHeartBeatTimeoutTime) == false)
			{
				if (pLink->Disconnect() == false)
				{
					uidFailedLinks.push_back((*itor).first);
				}
				IncreaseDisconnCountByHearBeat();
			}
		}

		if (!uidFailedLinks.empty())
		{
			for (size_t i = 0; i < uidFailedLinks.size(); ++i)
			{
				MLinkMap::LINK_MAP::iterator itor = m_LinkMap.GetLinkMap().find(uidFailedLinks[i]);
				if (itor != m_LinkMap.GetLinkMap().end())
				{
					MLink* pLink = (*itor).second;
					pLink->HardDisconnect();
				}
			}
		}
	}
}

bool MNetServer::Post( MCommand* pCommand )
{
	if (m_Desc.bDelayTest)
	{
		if (pCommand->m_pCommandDesc->IsFlag(MCDT_LOCAL) == false)
		{
			if(pCommand->CheckRule() == false) 
			{
				_ASSERT(0);
				return false;
			}

			uint32 nNowTime = timeGetTime();
			m_DelayTester.Post(pCommand, nNowTime);
			return true;
		}
	}

	return MCommandCommunicator::Post(pCommand);
}

bool MNetServer::ReAllocLinkUID( MUID uidOrigin, MUID uidReAlloc )
{
	if (!m_LinkMap.ReAllocUID(uidOrigin, uidReAlloc))
		return false;

	MCommand* pNewCommand = NewCommand(MC_NET_REALLOC_CLIENT_UID, uidReAlloc);
	pNewCommand->AddParameter(new MCommandParameterUID(uidReAlloc));
	Post(pNewCommand);
	return true;
}

int MNetServer::GetLinkCount() 
{
	return (int)m_LinkMap.GetLinkMap().size();
}

void MNetServer::IncreaseDisconnCountByHearBeat()
{
	InterlockedIncrement(&m_nDisconnCountByHeartBeat);
}

void MNetServer::IncreaseDisconnCountByWrongPacket()
{
	InterlockedIncrement(&m_nDisconnCountByWrongPacket);
}

void MNetServer::ResetDisconnCount()
{
	m_nDisconnCountByHeartBeat = 0;
	m_nDisconnCountByWrongPacket = 0;
}

} // namespace minet