#include "stdafx.h"
#include "XDummyMaster.h"
#include "XDummyFileLoader.h"

SUITE(DummyClient)
{
	TEST(TestDummyFileLoading_Config)
	{
		XDummyData dummyData;
		XDummyFileLoader loader(&dummyData);

		char szStream[] = "<maiet>																\
						   <Config>																\
						   <LoginServer>localhost</LoginServer>											\
						   <GameServer>localhost</GameServer>											\
						   <ID>BirdTest</ID>													\
						   <RunningTime>60</RunningTime>										\
						   </Config>															\
						   </maiet>";																

		bool ret = loader.LoadFromStream(szStream);
		CHECK_EQUAL(ret, true);

		CHECK_EQUAL(dummyData.m_Config.strGameServer, "localhost");
		CHECK_EQUAL(dummyData.m_Config.strLoginServer, "localhost");
		CHECK_EQUAL(dummyData.m_Config.strID, "BirdTest");
		CHECK_EQUAL(dummyData.m_Config.nRunningTime, 60);
	}


	TEST(TestDummyFileLoading_AgentInfo)
	{
		XDummyData dummyData;
		XDummyFileLoader loader(&dummyData);

		char szStream[] = "<maiet>																\
						  <Agents>																\
						  <Agent name='Login' count='10'>										\
						  <Pattern type='xml'>LoginTest</Pattern>								\
						  <Pattern type='xml'>LoginField</Pattern>								\
						  <Pattern type='glue'>LoginFieldGlueHandler</Pattern>					\
						  </Agent>																\
						  <Agent name='SelectChar' count='50'>									\
						  <Pattern type='glue'>LoginField</Pattern>								\
						  </Agent>																\
						  </Agents>																\
						  </maiet>";																

		bool ret = loader.LoadFromStream(szStream);
		CHECK_EQUAL(ret, true);

		XDummyAgentInfo* pAgentInfo = dummyData.GetAgentInfo("Login");
		CHECK(pAgentInfo != NULL);
		if (pAgentInfo == NULL) return;

		CHECK_EQUAL(pAgentInfo->strName, "Login");
		CHECK_EQUAL(pAgentInfo->nCount, 10);
		CHECK_EQUAL(pAgentInfo->strXmlPatterns.size(), 2);
		CHECK_EQUAL(pAgentInfo->strGluePatterns.size(), 1);

		if (pAgentInfo->strXmlPatterns.size() == 2)
		{
			CHECK_EQUAL(pAgentInfo->strXmlPatterns[0], "LoginTest");
			CHECK_EQUAL(pAgentInfo->strXmlPatterns[1], "LoginField");
		}

		if (pAgentInfo->strXmlPatterns.size() == 1)
		{
			CHECK_EQUAL(pAgentInfo->strXmlPatterns[0], "LoginFieldGlueHandler");
		}

		pAgentInfo = dummyData.GetAgentInfo("SelectChar");
		CHECK(pAgentInfo != NULL);
		if (pAgentInfo == NULL) return;

		CHECK_EQUAL(pAgentInfo->strName, "SelectChar");
		CHECK_EQUAL(pAgentInfo->nCount, 50);
		CHECK_EQUAL(pAgentInfo->strXmlPatterns.size(), 0);
		CHECK_EQUAL(pAgentInfo->strGluePatterns.size(), 1);

		if (pAgentInfo->strXmlPatterns.size() == 1)
		{
			CHECK_EQUAL(pAgentInfo->strXmlPatterns[0], "LoginField");
		}
	}


	TEST(TestDummyFileLoading_GlueHandler)
	{
		XDummyData dummyData;
		XDummyFileLoader loader(&dummyData);

		char szStream[] = "<maiet>																\
						  <Config>																\
						  <Server>localhost</Server>											\
						  <ID>BirdTest</ID>														\
						  </Config>																\
						  <Agents>																\
						  <Agent name='Login' count='10'>										\
						  <Pattern type='glue'>LoginFlood</Pattern>								\
						  </Agent>																\
						  </Agents>																\
						  </maiet>";																

		bool ret = loader.LoadFromStream(szStream);
		CHECK_EQUAL(ret, true);

		dummyData.InitFromInfo();

		CHECK_EQUAL(dummyData.m_AgentMgr.GetSize(), 10);

		for (size_t i = 0; i < dummyData.m_AgentMgr.GetSize(); i++)
		{
			XBirdDummyAgent* pAgent = dummyData.m_AgentMgr.GetAgent(i);
			if (pAgent == NULL) continue;

			char login_id[64];
			sprintf_s(login_id, "%s%d", "BirdTest", i+1);
			CHECK_EQUAL(pAgent->GetLoginID(), string(login_id));

			CHECK_EQUAL(pAgent->GetName(), "Login");
			CHECK_EQUAL(pAgent->GetHandlerCount(), 1);
		}
	}


	TEST(TestDummyFileLoading_XmlHandler)
	{
		XDummyData dummyData;
		XDummyFileLoader loader(&dummyData);

		char szStream[] = "<maiet>																\
						  <Config>																\
						  <Server>localhost</Server>											\
						  <ID>BirdTest</ID>														\
						  </Config>																\
						  <Agents>																\
						  <Agent name='Login' count='10'>										\
						  <Pattern type='xml'>Login</Pattern>									\
						  </Agent>																\
						  </Agents>																\
						  <Patterns>															\
						  <Pattern name = 'Login'>												\
						  <OnCommand ID = 'MC_LOCAL_NET_ONDISCONNECT'>							\
						  <Post ID = 'MC_LOCAL_NET_CONNECT'>									\
						  <Param Type = 'string'>localhost</Param>								\
						  <Param Type = 'int'>7400</Param>										\
						  </Post>																\
						  </OnCommand>															\
						  </Pattern>															\
						  </Patterns>															\
						  </maiet>";																

		bool ret = loader.LoadFromStream(szStream);
		CHECK_EQUAL(ret, true);

		dummyData.InitFromInfo();

		CHECK_EQUAL(dummyData.m_AgentMgr.GetSize(), 10);

		for (size_t i = 0; i < dummyData.m_AgentMgr.GetSize(); i++)
		{
			XBirdDummyAgent* pAgent = dummyData.m_AgentMgr.GetAgent(i);
			if (pAgent == NULL) continue;

			char login_id[64];
			sprintf_s(login_id, "%s%d", "BirdTest", i+1);
			CHECK_EQUAL(pAgent->GetLoginID(), string(login_id));

			CHECK_EQUAL(pAgent->GetName(), "Login");
			CHECK_EQUAL(pAgent->GetHandlerCount(), 1);
		}
	}

}