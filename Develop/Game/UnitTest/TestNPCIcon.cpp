#include "stdafx.h"
#include "TestModule.h"
#include "RSceneManager.h"

SUITE(NPCICON)
{
	class FNPCIconFixture : public SimpleTestFixture
	{
		RCameraSceneNode* pCameraSceneNode;

	public:
		FNPCIconFixture()
		{
			TestModuleContext	testContext(this);
			m_pNPC = testContext.CreateNPCAndEnterToGame(1, vec3(100.0f, 100.0f, 100.0f));
			pCameraSceneNode = REngine::GetSceneManager().GetPrimaryCamera();
		}
		virtual ~FNPCIconFixture()
		{
			// 테스트에서 쓰이는 카메라 다시 셋팅
			REngine::GetSceneManager().SetPrimaryCamera(pCameraSceneNode);
		}

		void RecvQuestBegin()
		{
			TD_NPC_ICON nodes[1];
			nodes[0].m_nUIID = m_pNPC->GetUIID();
			nodes[0].m_nIcon = NIT_QUEST_BEGIN;

			TD_ESSENTIAL_NPC_ICON essentialNPCIcon;

			m_pNetAgent->OnRecv(MC_NPCINTERACTION_ICON, 2, 
				NEW_BLOB(nodes, sizeof(TD_NPC_ICON), 1), 
				NEW_BLOB(&essentialNPCIcon, sizeof(TD_ESSENTIAL_NPC_ICON), 1));
		}

		XNonPlayer* m_pNPC;
	};

	TEST_FIXTURE(FNPCIconFixture, TestNPCIcon)
	{
		CHECK_EQUAL(m_pNPC->GetIConState(), NIS_NONE);
	}

	TEST_FIXTURE(FNPCIconFixture, TestNPCIconQuest)
	{
		RecvQuestBegin();

		m_pGame->Update(1.0f);

		CHECK_EQUAL(m_pNPC->GetIConState(), NIS_QUEST_BEGIN);
	}

	TEST_FIXTURE(FNPCIconFixture, TestNPCIconQuestAccept)
	{
		RecvQuestBegin();

		TD_INTERACTION_ELEMENT ie;
		m_pNetAgent->OnRecv(MC_NPCINTERACTION_INTERACTION, 3,
			NEW_UID(m_pNPC->GetUID()), NEW_WSTR(L"하이"), NEW_BLOB(&ie, sizeof(TD_INTERACTION_ELEMENT), 1));

		m_pNetAgent->OnRecv(MC_QUEST_SHOW_ACCPET_UI, 1, NEW_INT(1));

		m_pGame->Update(1.0f);

		CHECK_EQUAL(m_pNPC->GetIConState(), NIS_QUEST_BEGIN);

		m_pGame->Update(1.0f);

		CHECK_EQUAL(m_pNPC->GetIConState(), NIS_QUEST_INTERACTION);
	}

	TEST_FIXTURE(FNPCIconFixture, TestNPCIconQuestAcceptReject)
	{
		RecvQuestBegin();

		m_pNetAgent->OnRecv(MC_QUEST_SHOW_ACCPET_UI, 1, NEW_INT(1));

		m_pNPC->SetNPCIConTypeToIconState( NIT_QUEST_BEGIN);

		m_pGame->Update(1.0f);

		CHECK_EQUAL(m_pNPC->GetIConState(), NIS_QUEST_BEGIN);
	}
}