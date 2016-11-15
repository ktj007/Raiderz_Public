#include "stdafx.h"
#include "FBaseGame.h"
#include "GConst.h"
#include "GFieldAttrGrid.h"
#include "GTestForward.h"

SUITE(GQPNPCPosSender)
{
	struct Fixture : FBaseGame
	{
		Fixture()
		{
			PVP_AREA_INFO infoPVP;
			infoPVP.rcArea = MRect(-500, -500, 500, 500);
			GFieldInfo* pInfo = const_cast<GFieldInfo*>(m_pField->GetInfo());
			pInfo->m_PvPAreas.push_back(infoPVP);
			pInfo->GetAttrGrid()->Init();
			pInfo->Cooking();

			test.player.SetMockPlayerPVPArea_LocateAndTeamOne(m_pPlayer);

			m_pNPC = NewNPC(m_pField);
			m_pNPC->EnableQPMarkPos();			

			m_pLink->ResetCommands();
		}

		GEntityNPC* m_pNPC;
	};

	TEST_FIXTURE(Fixture, Disable)
	{
		m_pNPC->DisableQPMarkPos();

		m_pNPC->Update(GConst::QUESTPVP_MARKPOS_REFRESH_TIME);
		CHECK_EQUAL(1, m_pLink->GetCommandCount());
		CHECK_EQUAL(MC_QUESTPVP_UNMARK_NPCPOS, m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pNPC->GetID(), m_pLink->GetParam<int>(0,0));		
	}

	TEST_FIXTURE(Fixture, Duration)
	{		
		m_pNPC->Update(GConst::QUESTPVP_MARKPOS_REFRESH_TIME-0.1f);
		CHECK_EQUAL(0, m_pLink->GetCommandCount());

		m_pNPC->Update(0.1f);
		CHECK_EQUAL(1, m_pLink->GetCommandCount());
		CHECK_EQUAL(MC_QUESTPVP_MARK_NPCPOS, m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(m_pNPC->GetID(), m_pLink->GetParam<int>(0,0));
		CHECK_EQUAL(m_pNPC->GetPos(), m_pLink->GetParam<vec3>(0,1));
	}
}