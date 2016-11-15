#include "stdafx.h"
#include "GUTHelper.h"
#include "GEntityPlayer.h"
#include "GPlayerGameStart_AutoRebirther.h"
#include "GPlayerGameStart_State.h"
#include "GPlayerBattleArena.h"
#include "GSharedField.h"
#include "GDynamicFieldMaster.h"
#include "GPlayerFieldDynamic.h"
#include "GUTHelper_Field.h"

SUITE(PlayerGameStart_AutoRebirther)
{
	struct Fixture
	{
		Fixture()
		{
			m_pPlayer = GUTHelper::NewEntityPlayer();		
			m_pPlayer->SetDie();			

			m_pAutoRebirther = new GPlayerGameStart_AutoRebirther(m_pPlayer);
		}

		virtual ~Fixture()
		{
			SAFE_DELETE(m_pAutoRebirther);
		}

		GUTHelper		m_Helper;
		GEntityPlayer*	m_pPlayer;
		GPlayerGameStart_AutoRebirther*		m_pAutoRebirther;
	};

	TEST_FIXTURE(Fixture, BattleArena)
	{
		GGateInfo gateInfo = m_pPlayer->GetPlayerField().GetGateInfo();
		gateInfo.nFieldID = 1;

		m_pPlayer->GetPlayerField().SetGateInfo(gateInfo);

		m_pPlayer->GetPlayerBattleArena().SetBattleArenaUID(SUnitTestUtil::NewUID());
		m_pAutoRebirther->Rebirth(GPlayerGameStart_State(m_pPlayer));

		CHECK_EQUAL(false, m_pPlayer->IsDead());
	}

	namespace Dynamic
	{
		struct FDynamic : public Fixture
		{
			FDynamic()
			{
				m_pDynamicEnterPosField = GUTHelper_Field::NewSharedField();
				m_pDynamicEnterPosMarkerInfo = GUTHelper_Field::InsertNewMarkerInfo(m_pDynamicEnterPosField->GetInfo());
				m_pPlayer->GetPlayerField().GetFieldDynamic().SetEnterPos(*m_pDynamicEnterPosMarkerInfo);

				m_pDynamicFieldMaster = GUTHelper_Field::NewTrialFieldMaster();
				m_pDynamicCheckpointMarkerInfo = GUTHelper_Field::InsertNewMarkerInfo(m_pDynamicFieldMaster->GetInfo()->pStartFieldInfo);
				m_pPlayer->GetPlayerField().GetFieldDynamic().SetCheckpoint(*m_pDynamicCheckpointMarkerInfo);

				GGateInfo gateInfo = m_pPlayer->GetPlayerField().GetGateInfo();
				gateInfo.nFieldID = m_pDynamicFieldMaster->GetInfo()->pStartMarkerInfo->nFieldID;
				m_pPlayer->GetPlayerField().SetGateInfo(gateInfo);
			}

			virtual ~FDynamic()
			{
				m_pDynamicEnterPosField->Destroy();
				m_pDynamicFieldMaster->Destroy();
			}


			GSharedField*			m_pDynamicEnterPosField;
			MARKER_INFO*			m_pDynamicEnterPosMarkerInfo;
			MARKER_INFO*			m_pDynamicCheckpointMarkerInfo;		

			GDynamicFieldMaster*	m_pDynamicFieldMaster;
		};

		namespace ExistField
		{
			TEST_FIXTURE(FDynamic, AutoRebirthTimeOver)
			{
				m_pPlayer->GetPlayerInfo()->nElapsedDeadTimeSec = static_cast<int>(GAME_AUTO_REBIRTH_CHECK_TICK + 1.0f);				
				m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(m_pDynamicFieldMaster->GetUID());

				m_pAutoRebirther->Rebirth(GPlayerGameStart_State(m_pPlayer));

				CHECK_EQUAL(false, m_pPlayer->IsDead());
			}

			TEST_FIXTURE(FDynamic, NotAutoRebirthTimeOver)
			{
				m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(m_pDynamicFieldMaster->GetUID());

				m_pAutoRebirther->Rebirth(GPlayerGameStart_State(m_pPlayer));

				CHECK_EQUAL(true, m_pPlayer->IsDead());
			}
		}

		TEST_FIXTURE(FDynamic, NotExistField)
		{
			m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(MUID::ZERO);

			m_pAutoRebirther->Rebirth(GPlayerGameStart_State(m_pPlayer));

			CHECK_EQUAL(false, m_pPlayer->IsDead());
		}
	}

	namespace Share
	{
		struct FShare : public Fixture
		{
			FShare()
			{
				m_pShareField = GUTHelper_Field::NewSharedField();

				GGateInfo gateInfo = m_pPlayer->GetPlayerField().GetGateInfo();
				gateInfo.nFieldID = m_pShareField->GetID();
				m_pPlayer->GetPlayerField().SetGateInfo(gateInfo);
			}

			virtual ~FShare()
			{
			}

			GSharedField*			m_pShareField;
		};

		TEST_FIXTURE(FShare, AutoRebirthTimeOver)
		{
			m_pPlayer->GetPlayerInfo()->nElapsedDeadTimeSec = static_cast<int>(GAME_AUTO_REBIRTH_CHECK_TICK + 1.0f);

			m_pAutoRebirther->Rebirth(GPlayerGameStart_State(m_pPlayer));

			CHECK_EQUAL(false, m_pPlayer->IsDead());
		}

		TEST_FIXTURE(FShare, NotAutoRebirthTimeOver)
		{
			m_pAutoRebirther->Rebirth(GPlayerGameStart_State(m_pPlayer));

			CHECK_EQUAL(true, m_pPlayer->IsDead());
		}

		TEST_FIXTURE(FShare, AutoRebirth_ButPlayerIsNotInField)
		{
			m_pPlayer->doDie();
			m_pPlayer->GetPlayerInfo()->nElapsedDeadTimeSec = static_cast<int>(GAME_AUTO_REBIRTH_CHECK_TICK + 1.0f);

			m_pPlayer->LeaveFromField();
			m_pAutoRebirther->Rebirth(GPlayerGameStart_State(m_pPlayer));

			CHECK_EQUAL(false, m_pPlayer->IsDead());
		}

	}
}