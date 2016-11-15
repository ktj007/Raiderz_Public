#include "stdafx.h"
#include "MCommandManager.h"
#include "MCommandStream.h"
#include "MLink.h"
#include "MDelayTester.h"
#include "MNetClient.h"
#include "MCommand.h"
#include "FCommandDesc.h"

using namespace minet;

SUITE(CommandStream)
{
	class FCommandStream : public FCommandDesc
	{
	public:
		FCommandStream() : FCommandDesc() {}
		virtual ~FCommandStream() {}
	};

	TEST(TestCommandStream)
	{
		MUID uidSender = 2, uidReceiver = 1;
		minet::MCommandManager cm;
		minet::MCommandStream cs(&cm);

		char packet_buf[1024] = {0, };
		DWORD dwPacketLen = 0;

		cs.Read(packet_buf, dwPacketLen);

		while (minet::MCommand* pCmd = cs.GetCommand())
		{

		}
	}

	TEST(TestHeartBeat)
	{
		MPingData hb;

		uint32 nNowTime = 100;

		hb.Sent(nNowTime);

		nNowTime = 300;
		CHECK(hb.CheckTimeOut(nNowTime, 1000) == false);

		nNowTime = 400;
		CHECK(hb.CheckHeartBeat(nNowTime, 500) == false);

		CHECK(hb.CheckHeartBeat(nNowTime, 200) == true);
	}

	TEST(TestDelayTester)
	{
		MCommandManager cm;

		MDelayTester dt;
		dt.SetDelay(100);

		uint32 nNowTime = 0;

		MCommand* pNewCmd = new MCommand();
		dt.Post(pNewCmd, nNowTime);

		// 10ms지나도 100딜레이니까 아직 CommandManager에 넣지 않았다.
		nNowTime += 10;
		dt.OnRun(cm, nNowTime);
		CHECK_EQUAL(cm.GetCommandQueueCount(), 0);

		// 110ms가 지났으니 이제 COmmandManager에 넣었다.
		nNowTime += 100;
		dt.OnRun(cm, nNowTime);
		CHECK_EQUAL(cm.GetCommandQueueCount(), 1);

	}

	class CryptTestNetClient : public MNetClient
	{
	public:
		CryptTestNetClient(const MNetClientDesc& desc) : MNetClient(desc) {}
		using MNetClient::MakeCmdPacket;
		using MNetClient::InitCrypt;
	};

	TEST_FIXTURE(FCommandStream, TestCommandCrypt)
	{
		MCommandManager cm;

		AddLoginCommandDesc();


		MCommand* pCommand = new MCommand(MC_COMM_REQUEST_LOGIN, MUID::ZERO, MUID::ZERO);
		pCommand->AddParameter(new MCommandParameterString("BirdTest15"));
		pCommand->AddParameter(new MCommandParameterString("1111"));
		pCommand->AddParameter(new MCommandParameterInt(27));

		MUID uidServer = MUID(0, 3412);
		MUID uidClient = MUID(0, 23423);

		MNetClientDesc clientDesc;
		CryptTestNetClient* pNetClient = new CryptTestNetClient(clientDesc);
		pNetClient->InitCrypt(uidServer, uidClient);

		int nPacketSize = CalcPacketSize(pCommand);
		CHECK(nPacketSize > 0);

		char* pSendBuf = new char[nPacketSize];

		int size = pNetClient->MakeCmdPacket(pSendBuf, nPacketSize, pCommand);
		CHECK(size > 0);


		MServerTcpSocket tcpSocket(NULL, NULL, NULL, NULL);
		MLink* pServerLink = new MLink(uidServer, uidClient, tcpSocket, &cm);

		MCommandStream* pCmdStream = pServerLink->GetCommandStream();
		bool ret = pCmdStream->Read((char*)pSendBuf, size);
		CHECK_EQUAL(ret, true);

		MCommand* pServerCmd = pCmdStream->GetCommand();
		CHECK(pServerCmd != NULL);
		if (pServerCmd != NULL)
		{
			CHECK_EQUAL(pServerCmd->GetID(), MC_COMM_REQUEST_LOGIN);

			string strUserName;
			ret = pServerCmd->GetParameter(strUserName,	0, MPT_STR);
			CHECK_EQUAL(ret, true);
			CHECK_EQUAL(strUserName, "BirdTest15");

			string strPassword;
			ret = pServerCmd->GetParameter(strPassword,	1, MPT_STR);
			CHECK_EQUAL(ret, true);
			CHECK_EQUAL(strPassword, "1111");

			int nCommandVersion;
			ret = pServerCmd->GetParameter(&nCommandVersion,	2, MPT_INT);
			CHECK_EQUAL(ret, true);
			CHECK_EQUAL(nCommandVersion, 27);

			delete pServerCmd;
			pServerCmd = NULL;
		}


		delete pCommand;
		delete [] pSendBuf;

		delete pNetClient;
		delete pServerLink;
	}
}
