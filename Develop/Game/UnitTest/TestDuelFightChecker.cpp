#include "stdafx.h"

#include "XUnitTestList.h"
#include "MockPlayer.h"
#include "XDuelFightChecker.h"
#include "XUnitTestUtil.h"
#include "XPlayerPVP.h"
#include "XUnitTestHelper.h"
#include "XNonPlayer.h"
#include "TestGame.h"

SUITE(DuelFightChecker)
{
	class XTestDuelFightChecker : public XDuelFightChecker
	{
	public:
		MockPlayer*			m_pMockPlayer;
		MockPlayer*			m_pMockAttackerPlayer;

	public:
		XTestDuelFightChecker() 
		{
			m_pMockPlayer = new MockPlayer(XUnitTestUtil::NewMUID(), SEX_FEMALE);
			m_pMockPlayer->Create();

			m_pMockAttackerPlayer = new MockPlayer(XUnitTestUtil::NewMUID(), SEX_FEMALE);
			m_pMockAttackerPlayer->Create();

			Init(m_pMockPlayer);
		}

		virtual ~XTestDuelFightChecker() 
		{
			m_pMockPlayer->Destroy();
			SAFE_DELETE(m_pMockPlayer);
		}

		XNonPlayer* CreateNPC()
		{
			TestGame		tg;
			XUnitTestHelper helper;
			XNonPlayer* pNPC = helper.InNPC(global.system->GenerateLocalUID(), 1, vec3::ZERO);

			return pNPC;
		}
	};

	TEST_FIXTURE(XTestDuelFightChecker, TestSetDuel)
	{
		SetDuel(m_pMockAttackerPlayer);

		CHECK_EQUAL(false, m_pMockPlayer->IsAlly(m_pMockAttackerPlayer));
	}

	TEST_FIXTURE(XTestDuelFightChecker, TestSetDuel_NonPlyaer)
	{
		XNonPlayer* pNpc = CreateNPC();

		CHECK_EQUAL(false, SetDuel(pNpc));
	}

	TEST_FIXTURE(XTestDuelFightChecker, TestPlayerDie)
	{
		SetDuel(m_pMockAttackerPlayer);

		m_pMockPlayer->SetDead(true);

		Update();

		CHECK_EQUAL(true, m_pMockAttackerPlayer->GetPlayerPVP()->IsAlly(m_pMockPlayer->GetPlayerPVP()));
	}
}

/////////////////////////////////////////////////////////////////////////

