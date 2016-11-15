#include "stdafx.h"
#include "FTestGame.h"
#include "XMyPlayer.h"

SUITE(Duel)
{
	class FDuelGame : public FTestGame
	{
	public:
		FDuelGame()
		{

		}

		virtual ~FDuelGame()
		{

		}
	};

	TEST_FIXTURE(FDuelGame, TestDuel)
	{
		XMyPlayer* pMyPlayer = GetMyPlayer();
		pMyPlayer->SetPosition(vec3::ZERO);

		XNetPlayer* pNetPlayer = NewNetPlayer(vec3::ZERO);

		CHECK(pMyPlayer->IsAlly(pNetPlayer) == true);

		m_pNetAgent->OnRecv(MC_DUEL_READY, 2, NEW_UID(pMyPlayer->GetUID()), NEW_UID(pNetPlayer->GetUID()));

		CHECK(pMyPlayer->IsAlly(pNetPlayer) == true);

		m_pNetAgent->OnRecv(MC_DUEL_FIGHT, 2, NEW_UID(pMyPlayer->GetUID()), NEW_UID(pNetPlayer->GetUID()));

		CHECK(pMyPlayer->IsAlly(pNetPlayer) == false);

		m_pNetAgent->OnRecv(MC_DUEL_FINISHED, 2, NEW_UID(pMyPlayer->GetUID()), NEW_UID(pNetPlayer->GetUID()));

		CHECK(pMyPlayer->IsAlly(pNetPlayer) == true);

	}

	TEST_FIXTURE(FDuelGame, TestDuelCancel)
	{
		XMyPlayer* pMyPlayer = GetMyPlayer();
		pMyPlayer->SetPosition(vec3::ZERO);

		XNetPlayer* pNetPlayer = NewNetPlayer(vec3::ZERO);

		CHECK(pMyPlayer->IsAlly(pNetPlayer) == true);

		m_pNetAgent->OnRecv(MC_DUEL_READY, 2, NEW_UID(pMyPlayer->GetUID()), NEW_UID(pNetPlayer->GetUID()));

		CHECK(pMyPlayer->IsAlly(pNetPlayer) == true);

		m_pNetAgent->OnRecv(MC_DUEL_FIGHT, 2, NEW_UID(pMyPlayer->GetUID()), NEW_UID(pNetPlayer->GetUID()));

		CHECK(pMyPlayer->IsAlly(pNetPlayer) == false);

		m_pNetAgent->OnRecv(MC_DUEL_CANCEL, 2, NEW_UID(pMyPlayer->GetUID()), NEW_UID(pNetPlayer->GetUID()));

		CHECK(pMyPlayer->IsAlly(pNetPlayer) == true);
	}

}