#include "stdafx.h"
#include "XDummyMaster.h"
#include <string>
#include <fstream>
#include "MMaietXmlTestReporter.h"
#include "XGlobalInstance.h"
#include "XBaseApplication.h"
#include "XDummyFileLoader.h"
#include "MTcpServer.h"
#include "XDummyConnectListener.h"
#include "XDummyDisconnectListener.h"
#include "XDummySendListener.h"
#include "XDummyRecvListener.h"
#include "XNet.h"
#include "XDummyHandler.h"
#include "XDummyHandler_ConnFlood.h"
#include "XDummyHandler_LoginFlood.h"
#include "XDummyHandler_FieldLogin.h"
#include "XDummyHandler_DeleteAllCharacter.h"
#include "XDummyHandler_FieldJumper.h"
#include "XDummyHandler_TrialJumper.h"
#include "XDummyHandler_FieldInsertChar.h"
#include "XDummyReporter.h"
#include "XDummyBot.h"
#include "XDummyBot_Roam.h"
#include "XDummyBot_Combat.h"
#include "XDummyBot_Party.h"
#include "XDummyBot_WarpField.h"
#include "XDummyBot_MoveChannel.h"
#include "XDummyBot_PositionSpreader.h"
#include "XDummyBot_Loot.h"
#include "XDummyBot_Equip.h"
#include "XDummyBot_Chat.h"
#include "XDummyBot_Shop.h"
#include "XDummyBot_Quest.h"
#include "XDummyBot_Trade.h"
#include "XDummyBot_Train.h"
#include "XDummyBot_Echo.h"
#include "XDummyBot_Mail.h"
#include "XDummyBot_AnnounceRefreshServer.h"
#include "XDummyBot_Dye.h"
#include "XDummyBot_AutoParty.h"
#include "XDummyBot_Enchant.h"
#include "XDummyBot_QuestPVP.h"
#include "XDummyBot_Inven.h"
#include "XDummyBot_Gather.h"
#include "XDummyBot_Spawn.h"
#include "XDummyBot_FirstCutscene.h"
#include "XDummyBot_Whisper.h"
#include "XDummyBot_FullEquip.h"
#include "XDummyBot_FillInventory.h"
#include "XDummyBot_GetMaxQuest.h"
#include "XDummyBot_LevelUp.h"
#include "XDummyBot_SetBerserker.h"
#include "XDummyLazyCommandPoster.h"
#include "XDummyBot_MultiLogin.h"

XDummyAgentInfo* XDummyData::GetAgentInfo(const wchar_t* szName)
{
	for (size_t i = 0; i < m_AgentInfos.size(); i++)
	{
		if (!wcsicmp(szName, m_AgentInfos[i].strName.c_str())) return &m_AgentInfos[i];
	}
	return NULL;
}

void XDummyData::InitAgents()
{
	m_AgentMgr.SetStartingID(m_Config.nStartingID);

	for (size_t i = 0; i < m_AgentInfos.size(); ++i)
	{
		int nAgentCount = m_AgentInfos[i].nCount;
		for (int j = 0; j < nAgentCount; j++)
		{
			XBirdDummyAgent* pAgent = m_AgentMgr.AddAgent(
				m_AgentInfos[i], 
				m_Config.strID.c_str(),
				m_Config.nStartingID + (int)m_AgentMgr.GetSize(),
				m_Config.strGameServer.c_str(),
				m_Config.strLoginServer.c_str());

			pAgent->SetDirectLoginToGameServer(m_Config.bDirectLoginToGameServer);
		}
	}
}

void XDummyData::InitGlueHandlers()
{
	m_GlueHandlerMgr.Add(new XDummyHandler_ConnFlood());
	m_GlueHandlerMgr.Add(new XDummyHandler_LoginFlood());
	m_GlueHandlerMgr.Add(new XDummyHandler_LoginDisconnect());
	m_GlueHandlerMgr.Add(new XDummyHandler_FieldLogin());
	m_GlueHandlerMgr.Add(new XDummyHandler_FieldLoginDisconnect());
	m_GlueHandlerMgr.Add(new XDummyHandler_DeleteAllCharacter());
	m_GlueHandlerMgr.Add(new XDummyHandler_FieldJumper());
	m_GlueHandlerMgr.Add(new XDummyHandler_TrialJumper());
	m_GlueHandlerMgr.Add(new XDummyHandler_InsertChar());
}

XDummyBot* XDummyData::NewDummyBot( const wchar_t* szName, XBirdDummyAgent* pAgent, XDummyAgentInfo* pAgentInfo )
{
	assert(NULL!=szName);
	assert(NULL!=pAgent);
	assert(NULL!=pAgentInfo);

	if (!wcsicmp(szName, XDummyBot_Roam::GetName())) return new XDummyBot_Roam(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Combat::GetName())) return new XDummyBot_Combat(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Party::GetName())) return new XDummyBot_Party(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_AutoParty::GetName())) return new XDummyBot_AutoParty(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_WarpField::GetName())) return new XDummyBot_WarpField(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_MoveChannel::GetName())) return new XDummyBot_MoveChannel(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_PositionSpreader::GetName())) return new XDummyBot_PositionSpreader(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Loot::GetName())) return new XDummyBot_Loot(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Equip::GetName())) return new XDummyBot_Equip(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Chat::GetName())) return new XDummyBot_Chat(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Shop::GetName())) return new XDummyBot_Shop(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Quest::GetName())) return new XDummyBot_Quest(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Trade::GetName())) return new XDummyBot_Trade(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Train::GetName())) return new XDummyBot_Train(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Echo::GetName())) return new XDummyBot_Echo(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Mail::GetName())) return new XDummyBot_Mail(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Dye::GetName())) return new XDummyBot_Dye(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Enchant::GetName())) return new XDummyBot_Enchant(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_QuestPVP::GetName())) return new XDummyBot_QuestPVP(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Inven::GetName())) return new XDummyBot_Inven(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Gather::GetName())) return new XDummyBot_Gather(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Spawn::GetName())) return new XDummyBot_Spawn(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_FirstCutscene::GetName())) return new XDummyBot_FirstCutscene(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_Whisper::GetName())) return new XDummyBot_Whisper(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_FullEquip::GetName())) return new XDummyBot_FullEquip(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_FillInventory::GetName())) return new XDummyBot_FillInventory(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_GetMaxQuest::GetName())) return new XDummyBot_GetMaxQuest(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_LevelUp::GetName())) return new XDummyBot_LevelUp(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_SetBerserker::GetName())) return new XDummyBot_SetBerserker(pAgent, pAgentInfo);
	if (!wcsicmp(szName, XDummyBot_MultiLogin::GetName())) return new XDummyBot_MultiLogin(pAgent, pAgentInfo);

	// 서버 재시작 알림용 봇. - StressTest 와 관계 없습니다.
	if (!wcsicmp(szName, XDummyBot_AnnounceRefreshServer::GetName())) return new XDummyBot_AnnounceRefreshServer(pAgent, pAgentInfo);
		
	return NULL;
}

void XDummyData::InitXmlHandlers()
{
	for (XDummyPatternMgr::iterator itor = m_DummyPatternMgr.begin(); itor != m_DummyPatternMgr.end(); itor++)
	{
		XDummyPattern* pPattern = (*itor).second;
		m_XmlHandlerMgr.Add(new XXMLDummyHandler(pPattern));
	}
}

void XDummyData::AttachHandlerToAgent()
{
	for (size_t i = 0; i < m_AgentMgr.GetSize(); i++)
	{
		XBirdDummyAgent* pAgent = m_AgentMgr.GetAgent(i);
		
		XDummyAgentInfo* pAgentInfo = GetAgentInfo(pAgent->GetName());
		if (pAgentInfo)
		{
			// glue patterns
			for (size_t j = 0; j < pAgentInfo->strGluePatterns.size(); j++)
			{
				wstring strGluePatternName = pAgentInfo->strGluePatterns[j];
				XDummyHandler* pDummyHandler = m_GlueHandlerMgr.GetHandler(strGluePatternName.c_str());
				if (pDummyHandler)
				{
					pAgent->AddCommandHandler(pDummyHandler);
				}
				else
				{
					_ASSERT(0);
				}
			}

			// xml patterns
			for (size_t j = 0; j < pAgentInfo->strXmlPatterns.size(); j++)
			{
				wstring strPatternName = pAgentInfo->strXmlPatterns[j];
				XDummyHandler* pDummyHandler = m_XmlHandlerMgr.GetHandler(strPatternName.c_str());
				if (pDummyHandler)
				{
					pAgent->AddCommandHandler(pDummyHandler);
				}
				else
				{
					_ASSERT(0);
				}
			}

			// bots
			for (size_t j = 0; j < pAgentInfo->strBotPatterns.size(); j++)
			{
				wstring strPatternName = pAgentInfo->strBotPatterns[j];

				XDummyBot* pBot = NewDummyBot(strPatternName.c_str(), pAgent, pAgentInfo);
				if (pBot)
				{
					pAgent->AddBot(pBot);
				}
				else
				{
					_ASSERT(0);
				}
			}
		}
	}
}

XDummyData::~XDummyData()
{
	m_AgentMgr.Clear();
	m_GlueHandlerMgr.Clear();

	SAFE_DELETE(m_pReporter);
}

XDummyData::XDummyData()
{
	m_pReporter = new XDummyReporter(&m_AgentMgr);
}

void XDummyData::InitGlobalInfo()
{
	global.app->GetGlobalInfoInstance()->Init();
}

void XDummyMaster::Run(float fDelta)
{
	m_Data.m_AgentMgr.Run(fDelta);
	assert(NULL!=m_pLazyCommandPoster);
	m_pLazyCommandPoster->Update(fDelta);
	m_Data.m_pReporter->Update();

	uint32 nNowTime = XGetNowTime();
	if (m_nStartTime > 0 && m_Data.m_Config.nRunningTime > 0 && (nNowTime - m_nStartTime) / 1000 > (uint32)m_Data.m_Config.nRunningTime)
	{
		global.app->Quit(MUnitTestRunner::CCNET_UNITTEST_SUCCESS);
	}
}

void XDummyMaster::Start()
{
	m_nStartTime = XGetNowTime();
	m_Data.m_AgentMgr.PostConnectToLoginServer();
}

void XDummyMaster::End()
{

}

XDummyMaster::XDummyMaster()
: m_nStartTime(0)
, m_pLazyCommmandPoster(NULL)
, m_pConnectListener(NULL)
, m_pDisconnectListener(NULL)
, m_pSendListener(NULL)
, m_pRecvListener(NULL)
{
}

XDummyMaster::~XDummyMaster()
{
	Report();
	delete m_pConnectListener;
	m_pConnectListener = NULL;
	delete m_pDisconnectListener;
	m_pDisconnectListener = NULL;
	delete m_pSendListener;
	m_pSendListener = NULL;
	delete m_pRecvListener;
	m_pRecvListener = NULL;
	delete m_pLazyCommandPoster;
	m_pLazyCommandPoster = NULL;
}

void XDummyMaster::Init( const wchar_t* szDummyFileName )
{
	{
		// net
		m_pConnectListener = new XDummyConnectListener(*this);
		m_pDisconnectListener = new XDummyDisconnectListener(*this);
		m_pSendListener = new XDummySendListener(*this);
		m_pRecvListener = new XDummyRecvListener(*this);
		assert(NULL!=m_pConnectListener);
		assert(NULL!=m_pDisconnectListener);
		assert(NULL!=m_pSendListener);
		assert(NULL!=m_pRecvListener);

		Net().SetConnectListener(m_pConnectListener);
		Net().SetDisconnectorListener(m_pDisconnectListener);
		Net().SetReceiverListener(m_pRecvListener);
		Net().SetSenderListener(m_pSendListener);

		m_pLazyCommandPoster = new XDummyLazyCommandPoster;
		assert(NULL!=m_pLazyCommandPoster);
	}

	XDummyFileLoader dummyFileLoader(&m_Data);
	dummyFileLoader.Load(szDummyFileName);

	m_Data.InitFromInfo();
}

void XDummyMaster::Report()
{
	const string strFileName = "Result_StressTest.xml";
	std::ofstream f(strFileName.c_str());
	MMaietXmlTestReporter reporter(f);


	MMaietTestResultList	resultList;

	float secondsElapsed = (XGetNowTime() - m_nStartTime) / 1000.0f;

	XDummyReporter::XDummyTotalInfo& totalInfo = m_Data.m_pReporter->GetTotalInfo();

	// summary
	char text[1024];
	sprintf_s(text, "Time : %.2f sec", secondsElapsed);
	reporter.AddMessage(string(text));

	sprintf_s(text, "Total Send Bytes = %.1f KB, Per Sec = %.1f KB", 
		totalInfo.m_nTotalSendBytes / 1024.0f, totalInfo.m_nSecSendBytes / 1024.0f);
	reporter.AddMessage(string(text));

	sprintf_s(text, "Total Recv Bytes = %.1f KB, Per Sec = %.1f KB", 
		totalInfo.m_nTotalRecvBytes / 1024.0f, totalInfo.m_nSecRecvBytes / 1024.0f);
	reporter.AddMessage(string(text));


	// agent
	vector<XDummyReporter::XDummyAgentInfo>& vecAgentInfo = m_Data.m_pReporter->GetAgentInfo();
	for (size_t i = 0; i < vecAgentInfo.size(); i++)
	{
		sprintf_s(text, "\tAgentName(%s) , count(%d) , send(%.2f kb) , recv(%.2f kb)\n",
			vecAgentInfo[i].strAgentName.c_str(), 
			vecAgentInfo[i].m_nAgentCount, 
			vecAgentInfo[i].m_nTotalSendBytes / 1024.0f,
			vecAgentInfo[i].m_nTotalRecvBytes / 1024.0f);

		reporter.AddMessage(string(text));
	}

	reporter.ReportSummary(1, 0, 0, secondsElapsed, resultList);
}

void XDummyMaster::OnConnect(MServerTcpSocket& tcp_socket)
{
	m_Data.m_AgentMgr.OnConnect(tcp_socket);
}

void XDummyMaster::OnDisconnect(const MUID& uid)
{
	m_Data.m_AgentMgr.OnDisconnect(uid);
}

void XDummyMaster::OnRecv(const MUID& uid,const BYTE* const data,const DWORD& dataLen)
{
	assert(NULL!=data);
	m_Data.m_AgentMgr.OnRecv(uid, data, dataLen);
}

void XDummyMaster::OnSend(const MUID& muid,DWORD bytes_transferred)
{
	m_Data.m_AgentMgr.OnSend(muid, bytes_transferred);
}
