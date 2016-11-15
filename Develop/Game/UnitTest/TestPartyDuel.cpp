#include "stdafx.h"
#include "FTestGame.h"
#include "XMyPlayer.h"

SUITE(PartyDuel)
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

		XNetPlayer* pNetPlayer1 = NewNetPlayer(vec3::ZERO);
		XNetPlayer* pNetPlayer2 = NewNetPlayer(vec3::ZERO);
		XNetPlayer* pNetPlayer3 = NewNetPlayer(vec3::ZERO);


		CHECK(gvar.Game.Duel.nTeam == -1);

		vector<MUID> vecTeam1UIDs;
		vecTeam1UIDs.push_back(pMyPlayer->GetUID());
		vecTeam1UIDs.push_back(pNetPlayer1->GetUID());

		vector<MUID> vecTeam2UIDs;
		vecTeam2UIDs.push_back(pNetPlayer2->GetUID());
		vecTeam2UIDs.push_back(pNetPlayer3->GetUID());

		m_pNetAgent->OnRecv(MC_DUEL_PARTY_READY, 2, NEW_BLOB(vecTeam1UIDs), NEW_BLOB(vecTeam2UIDs));

		CHECK(gvar.Game.Duel.nTeam == 0);
	}

}