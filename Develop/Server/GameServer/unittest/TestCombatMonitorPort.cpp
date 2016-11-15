#include "StdAfx.h"
#include "GTestForward.h"
#include "FBaseGame.h"
#include "GCombatMonitorPort.h"

SUITE(CombatMonitorPort)
{
	class TestSender : public GMonitorPortBase::Sender
	{
	public:
		TestSender()
		{

		}

		virtual ~TestSender()
		{

		}

		virtual bool Send(const wchar_t* szName, string strMessage) override
		{
			vecMessages.push_back(strMessage);
			return true;
		}

		vector<string> vecMessages;
	};

	struct CombatMonitorPort : public FBaseGame
	{
		CombatMonitorPort()
		{ 
			m_pField->AttachCombatMonitor(&cm);
		}

		virtual ~CombatMonitorPort() 
		{
			m_pField->DetachCombatMonitor();
		}

		GCombatMonitorPort cm;
	};


	TEST_FIXTURE(CombatMonitorPort, ObservingLifecycle)
	{
		CHECK_EQUAL(1, cm.GetSubjectQty()); // 플레이어 1

		GNPCInfo NPCInfo;
		NPCInfo.nID = 1;
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&NPCInfo);
		CHECK_EQUAL(2, cm.GetSubjectQty()); // 플레이어 1, NPC 1
		
		pNPC->DeleteMe();
		m_pField->Update(0.0f);
		CHECK_EQUAL(1, cm.GetSubjectQty()); // 플레이어 1
	}

	TEST_FIXTURE(CombatMonitorPort, ObserveAlreadyExistActors)
	{
		m_pField->DetachCombatMonitor();
		CHECK_EQUAL(0, cm.GetSubjectQty());

		GNPCInfo NPCInfo;
		NPCInfo.nID = 1;
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&NPCInfo);
		m_pField->AttachCombatMonitor(&cm);
		CHECK_EQUAL(2, cm.GetSubjectQty()); // 플레이어 1, NPC 1

		m_pField->DetachCombatMonitor();
		CHECK_EQUAL(0, cm.GetSubjectQty());
	}

	TEST_FIXTURE(CombatMonitorPort, XMLNPC)
	{
		TestSender* pSender = new TestSender();
		cm.SetSender(pSender);

		GNPCInfo NPCInfo;
		NPCInfo.nID = 1;
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(&NPCInfo);

		TVALID_EQ(1, pSender->vecMessages.size());
		CHECK_EQUAL("<NPC>\n"
			"    <Name></Name>\n"
			"    <UIID>10001</UIID>\n"
			"    <Level>1</Level>\n"
			"    <ID>1</ID>\n"
			"</NPC>\n", 
			pSender->vecMessages[0]);
	}
}