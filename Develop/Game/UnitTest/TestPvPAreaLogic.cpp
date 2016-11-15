#include "stdafx.h"
#include "XPvPAreaLogic.h"
#include "FTestGame.h"
#include "XGame.h"
#include "XWorld.h"
#include "XMyPlayer.h"
#include "XNetPlayer.h"
#include "XPlayerPVP.h"
#include "XFactionInfo.h"

SUITE(PvPAreaLogic)
{
	TEST(TestPvPAreaInfo)
	{
		XPvPAreaInfo pvpAreaInfo;

		CHECK_EQUAL(false, pvpAreaInfo.IsInPvPArea());

		pvpAreaInfo.SetInPvPArea(100, wstring(L"faction1"), 101, wstring(L"faction2"));

		CHECK_EQUAL(true, pvpAreaInfo.IsInPvPArea());

		CHECK(wstring(L"faction1") == pvpAreaInfo.GetTeam1FactionName());
		CHECK(wstring(L"faction2") == pvpAreaInfo.GetTeam2FactionName());

		pvpAreaInfo.SetOutPvPArea();
		CHECK_EQUAL(false, pvpAreaInfo.IsInPvPArea());
	}

	class TestPvPAreaLogic : public XPvPAreaLogic
	{
	public:
		using XPvPAreaLogic::m_PvPAreaInfo;
		using XPvPAreaLogic::SetMyPlayerInPvPArea;
	};

	class FPvPAreaLogic : public FTestGame
	{
	public:
		FPvPAreaLogic()
		{
			m_nFaction1 = 100;
			m_nFaction2 = 101;

			XFactionInfo* pFactionInfo1 = new XFactionInfo(m_nFaction1, 30000, L"", L"");
			info.faction->Insert(pFactionInfo1);
			XFactionInfo* pFactionInfo2 = new XFactionInfo(m_nFaction2, 30000, L"", L"");
			info.faction->Insert(pFactionInfo2);

			XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();

			PVP_AREA_INFO area;
			area.rcArea = MRect(100, 100, 1000, 1000);
			area.nFaction1 = m_nFaction1;
			area.nFaction2 = m_nFaction2;

			pFieldInfo->m_PvPAreas.push_back(area);

			m_MyPlayerFactions.SetQuantity(m_nFaction1, 60000);

			m_vPvPSpot = vec3(500.0f, 500.0f, 0.0f);	// pvp 지역
		}
		virtual ~FPvPAreaLogic()
		{

		}
		void SetPosInPvp(XObject* pObj)
		{
			pObj->SetPosition(m_vPvPSpot);	// pvp 지역
		}

		TestPvPAreaLogic	m_PvPAreaLogic;
		XPlayerFactions		m_MyPlayerFactions;

		int					m_nFaction1;
		int					m_nFaction2;

		vec3				m_vPvPSpot;
	};

	TEST_FIXTURE(FPvPAreaLogic, TestPvPAreaLogic_IsInPvPArea)
	{
		XMyPlayer* pMyPlayer = GetMyPlayer();
		pMyPlayer->SetPosition(vec3::ZERO);	// pvp 지역

		m_PvPAreaLogic.Update(5.0f, m_MyPlayerFactions);
		CHECK_EQUAL(false, m_PvPAreaLogic.IsInPvPArea());

		// pvp 지역에 들어옴
		pMyPlayer->SetPosition(m_vPvPSpot);	// pvp 지역

		m_PvPAreaLogic.Update(5.0f, m_MyPlayerFactions);
		CHECK_EQUAL(true, m_PvPAreaLogic.IsInPvPArea());
	}

	TEST_FIXTURE(FPvPAreaLogic, TestPvPAreaLogic_IsMyEnemy)
	{
		XMyPlayer* pMyPlayer = GetMyPlayer();

		// pvp 지역에 들어옴
		SetPosInPvp(pMyPlayer);

		m_PvPAreaLogic.Update(5.0f, m_MyPlayerFactions);

		XNetPlayer* pNetPlayer = NewNetPlayer(m_vPvPSpot);
		pNetPlayer->GetPlayerPVP()->SetPvPAreaFaction(false, true);

		m_PvPAreaLogic.SetupEnemy(pNetPlayer->GetUID());

		m_PvPAreaLogic.Update(5.0f, m_MyPlayerFactions);

		CHECK_EQUAL(true, m_PvPAreaLogic.IsEnemy(pNetPlayer));
	}


	class FPvPAreaLogic_SetmyPlayerInPvPArea : public FPvPAreaLogic
	{
	public:
		FPvPAreaLogic_SetmyPlayerInPvPArea() 
		{
			XMyPlayer* pMyPlayer = GetMyPlayer();

			// pvp 지역에 들어옴
			SetPosInPvp(pMyPlayer);

			m_pNetPlayer = NewNetPlayer(m_vPvPSpot);
			m_pNetPlayer->GetPlayerPVP()->SetPvPAreaFaction(false, true);
			SetPosInPvp(m_pNetPlayer);
		}
		virtual ~FPvPAreaLogic_SetmyPlayerInPvPArea()
		{

		}
		XNetPlayer*	m_pNetPlayer;
	};

	TEST_FIXTURE(FPvPAreaLogic_SetmyPlayerInPvPArea, TestPvPAreaLogic_SetMyPlayerInPvPArea)
	{
		XMyPlayer* pMyPlayer = GetMyPlayer();

		XPlayerFactions myPlayerFactions;
		myPlayerFactions.SetQuantity(m_nFaction1, 60000);

		m_PvPAreaLogic.Update(5.0f, myPlayerFactions);

		CHECK_EQUAL(false, pMyPlayer->GetPlayerPVP()->IsAlly(m_pNetPlayer->GetPlayerPVP()));
	}

	TEST_FIXTURE(FPvPAreaLogic_SetmyPlayerInPvPArea, TestPvPAreaLogic_SetMyPlayerInPvPArea_Negative)
	{
		XMyPlayer* pMyPlayer = GetMyPlayer();

		XPlayerFactions myPlayerFactions;
		myPlayerFactions.SetQuantity(m_nFaction1, 1200);	// 팩션이 있지만 아주 적다.

		m_PvPAreaLogic.Update(5.0f, myPlayerFactions);

		CHECK_EQUAL(true, pMyPlayer->GetPlayerPVP()->IsAlly(m_pNetPlayer->GetPlayerPVP()));
	}

}