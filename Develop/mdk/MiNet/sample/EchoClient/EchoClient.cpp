#include "stdafx.h"
#include "UnitTest++.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "MiNet.h"
#include "MThread2.h"
#include "MNetClient.h"
#include "MCommandTable.h"

using namespace minet;

cml2::MAutoSignalEvent	g_SignalEvent;

#define MC_ECHO_REQ			1001
#define MC_ECHO				1002

uint32 g_nSendTime = 0;

void AddCommandTable()
{
	BEGIN_CMD_DESC();

	C(MC_ECHO_REQ,	MCDT_MACHINE2MACHINE,	MCF_C2S, "Echo Request");
	P(MPT_STR, "Message");

	C(MC_ECHO,		MCDT_MACHINE2MACHINE,	MCF_S2C, "Echo Resonse")
		P(MPT_STR, "Message");
	P(MPT_BLOB, "MessageData");

	END_CMD_DESC();
}

class MCmdHandler_EchoClient : public MCommandHandler
{
public:
	MCmdHandler_EchoClient(MCommandCommunicator* pCC) : MCommandHandler(pCC)
	{
		SetCmdHandler(MC_ECHO,			OnEcho);
		SetCmdHandler(MC_LOCAL_NET_ONCONNECT, OnConnect);
		SetCmdHandler(MC_LOCAL_NET_ONDISCONNECT, OnDisconnect);
	}
	virtual ~MCmdHandler_EchoClient()
	{

	}
protected:
	static MCommandResult OnConnect(MCommand* pCommand, MCommandHandler* pHandler)
	{
		printf("¿¬°áµÊ\n");
		g_SignalEvent.Signal();
		return CR_TRUE;
	}
	static MCommandResult OnDisconnect(MCommand* pCommand, MCommandHandler* pHandler)
	{
		printf("²÷¾îÁü\n");
		return CR_TRUE;
	}
	static MCommandResult OnEcho(MCommand* pCommand, MCommandHandler* pHandler)
	{
		char szMessage[256] = "";

		if( !pCommand->GetParameter(szMessage,	0, MPT_STR, sizeof(szMessage)) )	return CR_ERROR;


		struct _Data
		{
			char n[30000];
		};

		MCommandParameter* pParam1 = pCommand->GetParameter(1);
		if (pParam1->GetType() != MPT_BLOB) return CR_ERROR;

		int nBlobCount = pParam1->GetBlobCount();
		_Data* pData = (_Data*)pParam1->GetPointer();

		int nLastNumber = pData[nBlobCount-1].n[29999];

		//printf("¹ÞÀº ¸Þ¼¼Áö(time=%u)\n", timeGetTime()-g_nSendTime);
		printf("¹ÞÀº ¸Þ¼¼Áö(time=%u) : %s , Number = %d\n", timeGetTime()-g_nSendTime,szMessage, nLastNumber);

		g_SignalEvent.Signal();

		return CR_TRUE;
	}
};

class EchoClient : public MNetClient
{
private:
	MCmdHandler_EchoClient* m_pHandler;
public:
	EchoClient(const MNetClientDesc& desc) : MNetClient(desc)
	{
		m_pHandler = new MCmdHandler_EchoClient(this);
	}
	virtual ~EchoClient()
	{
		delete m_pHandler;
	}
};

class NetThread : public cml2::MThread
{
protected:
	MNetClient*	m_pClient;
	virtual void Run(void)
	{
		while (!Terminated())
		{
			m_pClient->Run();
			Sleep(10);

			OnRun_FloodPing();
		}
	}
	void OnRun_FloodPing()
	{
		static uint32 nLastSendPingCount = 0;
		if (m_pClient->GetPingData().GetRecvCount() != nLastSendPingCount)
		{
			printf("Ping : Time=%ums, Send:%u, Recv:%u\n",
				m_pClient->GetPingData().GetPing(),
				m_pClient->GetPingData().GetSendCount(),
				m_pClient->GetPingData().GetRecvCount()
				);
			nLastSendPingCount = m_pClient->GetPingData().GetSendCount();
		}
	}
public:
	NetThread(MNetClient* pClient) : m_pClient(pClient) {}
	virtual ~NetThread() {}
};

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _DEBUG
	cml2::SetThisThreadName("MainThread");
#endif

	MInitLog();

	minet::Init(1);

	AddCommandTable();

	const int nPort = 7000;
	const char* szHost = "localhost";

	MNetClientDesc desc;
	desc.bKeepAlive = false;
	desc.nKeepAliveTickTime = 0;

	EchoClient* pClient = new EchoClient(desc);
	pClient->Connect(szHost, nPort);

	const int BUFSIZE = 65535;
	char message[BUFSIZE];

	NetThread* net_thread = new NetThread(pClient);
	net_thread->Start();

	while (TRUE) 
	{
		g_SignalEvent.Wait(10000);

		fputs("Input Message (q to quit) : ", stdout);
		fgets(message, BUFSIZE, stdin);

		if (!strcmp(message, "q\n")) break;

		//g_SignalEvent.Wait(10);

		//for (size_t i = 0; i < 500;i++)
		//{
		//	message[i] = 'a';
		//}
		//message[500] = '\0';

		//strcpy_s(message, "askfhasdjkfhasdfjksdhjk fasdfasd fnsd,m fnsdaf sdlfk sadklf nasdfn sdaf sdf ,sdnf asdm,fn ,asdmf ns,dfn ,sdfn s,dafn as,d fnasd, fnsd");

		g_nSendTime = timeGetTime();

		MCommand* pNewCommand = pClient->NewCommand(MC_ECHO_REQ, pClient->GetServerUID());
		pNewCommand->AddParameter(new MCommandParameterString(message));
		pClient->Post(pNewCommand);

		message[0] = 0;
	}

	pClient->Disconnect();

	net_thread->Stop();
	delete net_thread;

	delete pClient;

	minet::Fini();
	MFinalizeLog();

	return 0;
}

