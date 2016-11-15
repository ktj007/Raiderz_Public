#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include "MiNet.h"
#include "MNetServer.h"

using namespace minet;

#define MC_ECHO_REQ			1001
#define MC_ECHO				1002

void AddCommandTable()
{
	BEGIN_CMD_DESC();

	C(MC_ECHO_REQ,	MCDT_MACHINE2MACHINE,	MCF_C2S, "Echo Request");
	P(MPT_STR, "Message");

	C(MC_ECHO,		MCDT_MACHINE2MACHINE,	MCF_S2C, "Echo Resonse")
		P(MPT_STR,	"Message");
		P(MPT_BLOB, "MessageData");


	END_CMD_DESC();
}

class MCmdHandler_EchoServer : public MCommandHandler
{
public:
	MCmdHandler_EchoServer(MCommandCommunicator* pCC) : MCommandHandler(pCC)
	{
		SetCmdHandler(MC_ECHO_REQ,			OnEchoReq);
	}
	virtual ~MCmdHandler_EchoServer()
	{

	}
protected:
	static MCommandResult OnEchoReq(MCommand* pCommand, MCommandHandler* pHandler)
	{
		char szMessage[256] = "";
		if( !pCommand->GetParameter(szMessage,	0, MPT_STR, sizeof(szMessage)) )	return CR_ERROR;

		MNetServer* pServer = (static_cast<MNetServer*>(pHandler->GetCommandCommunicator()));

		MCommand* pNewCommand = pServer->NewCommand(MC_ECHO, pCommand->GetSenderUID());
		pNewCommand->AddParameter(new MCommandParameterString(szMessage));

		struct _Data
		{
			char n[30000];
			_Data()
			{
				for (int i = 0; i < 30000;i++) n[i] = i;
			}
		};

		vector<_Data> vecData;
		vecData.resize(1);

		pNewCommand->AddParameter(new MCommandParameterBlob(vecData));

		pServer->Post(pNewCommand);


//		printf("메세지:%s (ping=%u)", szMessage, pServer->GetPing(pCommand->GetSenderUID()));

		return CR_TRUE;
	}
};

class EchoServer : public MNetServer
{
private:
	MCmdHandler_EchoServer* m_pHandler;
	void OnRun_FloodPing()
	{
		cml2::MCriticalSection::Guard guard(m_LinkMap.m_csLock);

		MLinkMap::LINK_MAP::iterator itor = m_LinkMap.GetLinkMap().begin();
		if (itor == m_LinkMap.GetLinkMap().end()) return;

		MLink* pLink = (*itor).second;

		static uint32 nLastSendPingCount = 0;

		//if (pLink->GetHeartBeat().GetRecvCount() != nLastSendPingCount)
		//{
		//	printf("Ping : Time=%ums, Send:%u, Recv:%u\n",
		//		pLink->GetHeartBeat().GetPing(),
		//		pLink->GetHeartBeat().GetSendCount(),
		//		pLink->GetHeartBeat().GetRecvCount()
		//		);
		//	nLastSendPingCount = pLink->GetHeartBeat().GetSendCount();
		//}
	}
public:
	EchoServer(MNetServerDesc& desc) : MNetServer(desc)
	{
		m_pHandler = new MCmdHandler_EchoServer(this);
	}
	virtual ~EchoServer()
	{
		delete m_pHandler;
	}
	virtual void OnRun()
	{
		MNetServer::OnRun();

		OnRun_FloodPing();
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _DEBUG
	cml2::SetThisThreadName("MainThread");
#endif

	MInitLog();

	minet::Init(1);
	AddCommandTable();

	MNetServerDesc desc;
	desc.bHeartBeat = true;
	desc.nHeartBeatTickTime = 60000;
	desc.nHeartBeatTimeoutTime = 300000;
	desc.bDelayTest = false;

	EchoServer* pServer = new EchoServer(desc);

	const int nPort = 7000;

	if (pServer->Create(nPort, true) == false)
	{
		return -1;
	}

	printf("서버 시작(port=%d)\n", nPort);

	while (TRUE) 
	{
		Sleep(10);
		pServer->Run();
	}

	pServer->Destroy();

	delete pServer;

	minet::Fini();
	MFinalizeLog();

	return 0;
}

