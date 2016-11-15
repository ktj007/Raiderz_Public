#include "stdafx.h"
#include "GUTHelper.h"
#include "GSharedField.h"
#include "GDynamicFieldMaster.h"
#include "GPlayerGameStart_FieldGater.h"
#include "GPlayerGameStart_State.h"
#include "MockField.h"
#include "GModuleCombat.h"
#include "GEntityPlayer.h"
#include "GSoulBindingInfo.h"
#include "GPlayerFieldDynamic.h"
#include "GUTHelper_Field.h"
#include "GFieldMgr.h"
#include "GFieldInfoMgr.h"
#include "GTestWrapper.h"
#include "GConst.h"

SUITE(PlayerGameStart_FieldGater)
{
	struct Fixture
	{
		Fixture()
		{
			m_pPlayer = GUTHelper::NewEntityPlayer();

			GFieldInfo* pFieldInfo = GUTHelper_Field::NewFieldInfo(GConst::GetFirstEnterFieldID());
			GSharedField* pFirstEnterField = GUTHelper_Field::NewSharedField(pFieldInfo);
			m_pFirstEnterMarkerInfo = GUTHelper_Field::InsertNewMarkerInfo(pFieldInfo, FIRST_ENTER_MARKER_ID);

			m_pSavedSoulBindingField = GUTHelper_Field::NewSharedField();
			m_pSavedSoulBindingMarkerInfo = GUTHelper_Field::InsertNewMarkerInfo(m_pSavedSoulBindingField->GetInfo());
			GSoulBindingInfo* pSoulBindingInfo = m_Helper.NewSoulBindingInfo(m_pSavedSoulBindingMarkerInfo);
			m_pPlayer->GetPlayerInfo()->nSoulBindingID = pSoulBindingInfo->m_nID;

			GGateInfo gateInfo = m_pPlayer->GetPlayerField().GetGateInfo();
			gateInfo.vPosition = vec3(1234, 1111, 8123);
			m_pPlayer->GetPlayerField().SetGateInfo(gateInfo);

			m_pFieldGater = new GPlayerGameStart_FieldGater(m_pPlayer);
		}

		virtual ~Fixture()
		{
			SAFE_DELETE(m_pFieldGater);
		}

		GUTHelper				m_Helper;
		GEntityPlayer*			m_pPlayer;
		GPlayerGameStart_FieldGater*	m_pFieldGater;

		MARKER_INFO*			m_pFirstEnterMarkerInfo;
		MARKER_INFO*			m_pSavedSoulBindingMarkerInfo;
		GSharedField*			m_pSavedSoulBindingField;

		GTestMgrWrapper<GFieldMgr>		m_wrapperFieldMgr;
		GTestMgrWrapper<GFieldInfoMgr>	m_wrapperFieldInfoMgr;
	};
	

	// 첫번째 로그인 하는 경우
	TEST_FIXTURE(Fixture, FirstLogin)
	{
		GGateInfo gateInfo = m_pPlayer->GetPlayerField().GetGateInfo();
		gateInfo.nFieldID = 0;
		m_pPlayer->GetPlayerField().SetGateInfo(gateInfo);

		m_pFieldGater->Gate(GPlayerGameStart_State(m_pPlayer));
		m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

		CHECK_EQUAL(m_pFirstEnterMarkerInfo->nFieldID, m_pPlayer->GetFieldID());
		CHECK_EQUAL(m_pFirstEnterMarkerInfo->vPoint, m_pPlayer->GetPos());
		CHECK_EQUAL(m_pFirstEnterMarkerInfo->vDir, m_pPlayer->GetDir());
	}

	namespace BattleArena
	{
		struct FBattleArena : public Fixture
		{
			FBattleArena()
			{
				m_pBattleArenaEnterPosField = GUTHelper_Field::NewSharedField();
				m_pBattleArenaEnterPosMarkerInfo = GUTHelper_Field::InsertNewMarkerInfo(m_pBattleArenaEnterPosField->GetInfo());
				m_pPlayer->GetPlayerField().GetFieldDynamic().SetEnterPos(*m_pBattleArenaEnterPosMarkerInfo);

				m_pBattleArenaFieldMaster = GUTHelper_Field::NewTrialFieldMaster();

				GGateInfo gateInfo = m_pPlayer->GetPlayerField().GetGateInfo();
				gateInfo.nFieldID = m_pBattleArenaFieldMaster->GetInfo()->pStartMarkerInfo->nFieldID;
				m_pPlayer->GetPlayerField().SetGateInfo(gateInfo);
			}

			virtual ~FBattleArena()
			{
			}


			GSharedField*			m_pBattleArenaEnterPosField;
			MARKER_INFO*			m_pBattleArenaEnterPosMarkerInfo;
			GDynamicFieldMaster*	m_pBattleArenaFieldMaster;
		};

		// 필드가 존재하는 경우
		TEST_FIXTURE(FBattleArena, ExistField)
		{
			m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(m_pBattleArenaFieldMaster->GetUID());

			m_pFieldGater->Gate(GPlayerGameStart_State(m_pPlayer));
			m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();


			GGateInfo rGateInfo = m_pPlayer->GetPlayerField().GetGateInfo();
			CHECK_EQUAL(rGateInfo.nFieldID, m_pPlayer->GetFieldID());
			CHECK_EQUAL(rGateInfo.vPosition, m_pPlayer->GetPos());
		}

		namespace NotExistField
		{
			// 플레이어가 죽어있는 경우
			TEST_FIXTURE(FBattleArena, DeadPlayer)
			{
				m_pPlayer->SetDie();
				m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(MUID::ZERO);

				m_pFieldGater->Gate(GPlayerGameStart_State(m_pPlayer));
				m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

				CHECK_EQUAL(m_pSavedSoulBindingMarkerInfo->nFieldID, m_pPlayer->GetFieldID());
				CHECK_EQUAL(m_pSavedSoulBindingMarkerInfo->vPoint, m_pPlayer->GetPos());
				CHECK_EQUAL(m_pSavedSoulBindingMarkerInfo->vDir, m_pPlayer->GetDir());
			}

			// 플레이어가 살아있는 경우
			TEST_FIXTURE(FBattleArena, LivePlayer)
			{
				m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(MUID::ZERO);

				m_pFieldGater->Gate(GPlayerGameStart_State(m_pPlayer));
				m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

				CHECK_EQUAL(m_pBattleArenaEnterPosMarkerInfo->nFieldID, m_pPlayer->GetFieldID());
				CHECK_EQUAL(m_pBattleArenaEnterPosMarkerInfo->vPoint, m_pPlayer->GetPos());
				CHECK_EQUAL(m_pBattleArenaEnterPosMarkerInfo->vDir, m_pPlayer->GetDir());
			}			
		}
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
			}


			GSharedField*			m_pDynamicEnterPosField;
			MARKER_INFO*			m_pDynamicEnterPosMarkerInfo;
			MARKER_INFO*			m_pDynamicCheckpointMarkerInfo;		

			GDynamicFieldMaster*	m_pDynamicFieldMaster;

		};

		namespace ExistField
		{
			// 플레이어가 죽어있고, 자동 부활 시간을 넘은 경우
			TEST_FIXTURE(FDynamic, DeadPlayerAndOverAutoRebirthTime)
			{
				m_pPlayer->SetDie();
				m_pPlayer->GetPlayerInfo()->nElapsedDeadTimeSec = static_cast<int>(GAME_AUTO_REBIRTH_CHECK_TICK + 1.0f);
				
				m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(m_pDynamicFieldMaster->GetUID());

				m_pFieldGater->Gate(GPlayerGameStart_State(m_pPlayer));
				m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

				CHECK_EQUAL(m_pDynamicCheckpointMarkerInfo->nFieldID, m_pPlayer->GetFieldID());
				CHECK_EQUAL(m_pDynamicCheckpointMarkerInfo->vPoint, m_pPlayer->GetPos());
				CHECK_EQUAL(m_pDynamicCheckpointMarkerInfo->vDir, m_pPlayer->GetDir());
			}

			// 플레이어가 죽어있고, 자동 부활 시간을 넘지 않은 경우
			TEST_FIXTURE(FDynamic, DeadPlayerAndNotOverAutoRebirthTime)
			{
				m_pPlayer->SetDie();

				m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(m_pDynamicFieldMaster->GetUID());

				m_pFieldGater->Gate(GPlayerGameStart_State(m_pPlayer));
				m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

				GGateInfo rGateInfo = m_pPlayer->GetPlayerField().GetGateInfo();
				CHECK_EQUAL(rGateInfo.nFieldID, m_pPlayer->GetFieldID());
				CHECK_EQUAL(rGateInfo.vPosition, m_pPlayer->GetPos());
			}

			// 플레이어가 살아있는 경우
			TEST_FIXTURE(FDynamic, LivePlayer)
			{
				m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(m_pDynamicFieldMaster->GetUID());

				m_pFieldGater->Gate(GPlayerGameStart_State(m_pPlayer));
				m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

				GGateInfo rGateInfo = m_pPlayer->GetPlayerField().GetGateInfo();
				CHECK_EQUAL(rGateInfo.nFieldID, m_pPlayer->GetFieldID());
				CHECK_EQUAL(rGateInfo.vPosition, m_pPlayer->GetPos());
			}
		}


		namespace NotExistField
		{
			// 플레이어가 죽어있는 경우
			TEST_FIXTURE(FDynamic, DeadPlayer)
			{
				m_pPlayer->SetDie();

				m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(MUID::ZERO);

				m_pFieldGater->Gate(GPlayerGameStart_State(m_pPlayer));
				m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

				CHECK_EQUAL(m_pSavedSoulBindingMarkerInfo->nFieldID, m_pPlayer->GetFieldID());
				CHECK_EQUAL(m_pSavedSoulBindingMarkerInfo->vPoint, m_pPlayer->GetPos());
				CHECK_EQUAL(m_pSavedSoulBindingMarkerInfo->vDir, m_pPlayer->GetDir());
			}

			// 플레이어가 살아있는 경우
			TEST_FIXTURE(FDynamic, LivePlayer)
			{
				m_pPlayer->GetPlayerField().SetDynamicFieldGroupUID(MUID::ZERO);

				m_pFieldGater->Gate(GPlayerGameStart_State(m_pPlayer));
				m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

				CHECK_EQUAL(m_pDynamicEnterPosMarkerInfo->nFieldID, m_pPlayer->GetFieldID());
				CHECK_EQUAL(m_pDynamicEnterPosMarkerInfo->vPoint, m_pPlayer->GetPos());
				CHECK_EQUAL(m_pDynamicEnterPosMarkerInfo->vDir, m_pPlayer->GetDir());
			}
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

		// 플레이어가 죽어있고, 자동 부활 시간을 넘은 경우
		TEST_FIXTURE(FShare, DeadPlayerAndOverAutoRebirthTime)
		{
			m_pPlayer->SetDie();
			m_pPlayer->GetPlayerInfo()->nElapsedDeadTimeSec = static_cast<int>(GAME_AUTO_REBIRTH_CHECK_TICK + 1.0f);			

			m_pFieldGater->Gate(GPlayerGameStart_State(m_pPlayer));
			m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

			CHECK_EQUAL(m_pSavedSoulBindingMarkerInfo->nFieldID , m_pPlayer->GetFieldID());
			CHECK_EQUAL(m_pSavedSoulBindingMarkerInfo->vPoint, m_pPlayer->GetPos());
			CHECK_EQUAL(m_pSavedSoulBindingMarkerInfo->vDir, m_pPlayer->GetDir());
		}

		// 플레이어가 죽어있고, 자동 부활 시간을 넘지 않은 경우
		TEST_FIXTURE(FShare, DeadPlayerAndNotOverAutoRebirthTime)
		{
			m_pPlayer->SetDie();

			m_pFieldGater->Gate(GPlayerGameStart_State(m_pPlayer));
			m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

			GGateInfo rGateInfo = m_pPlayer->GetPlayerField().GetGateInfo();
			CHECK_EQUAL(rGateInfo.nFieldID, m_pPlayer->GetFieldID());
			CHECK_EQUAL(rGateInfo.vPosition, m_pPlayer->GetPos());
		}

		// 플레이어가 살아있는 경우
		TEST_FIXTURE(FShare, LivePlayer)
		{
			m_pFieldGater->Gate(GPlayerGameStart_State(m_pPlayer));
			m_pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();

			GGateInfo rGateInfo = m_pPlayer->GetPlayerField().GetGateInfo();
			CHECK_EQUAL(rGateInfo.nFieldID, m_pPlayer->GetFieldID());
			CHECK_EQUAL(rGateInfo.vPosition, m_pPlayer->GetPos());
		}		
	}
}
