#include "stdafx.h"
#include "GFieldSystem.h"
#include "GTestWrapper.h"
#include "MockField.h"
#include "MockLink.h"
#include "GUTHelper_Field.h"
#include "GTestForward.h"

SUITE(FieldSystem)
{
	struct Fixture
	{
		GTestSysWrapper<GFieldSystem> m_FieldSystemWrapper;
		MockField* m_pField;
		GEntityPlayer* m_pPlayer;
		GEntityPlayer* m_pPlayer2;
		MockLink* m_pLink;
		MockLink* m_pLink2;

		Fixture()
		{			
			m_pField = GUTHelper_Field::NewMockField();
		
			m_pPlayer = test.player.NewPlayer(m_pField);
			m_pPlayer2 = test.player.NewPlayer(m_pField);

			m_pLink = test.network.NewLink(m_pPlayer, true);
			m_pLink2 = test.network.NewLink(m_pPlayer2, true);
		}

		~Fixture()
		{
			m_pField->Destroy();
			SAFE_DELETE(m_pField);
		}

		vec3 GetSectorCenterPos(int x, int y)
		{
			vec3 vPos;
			vPos.x = (SECTOR_SIZE * x) + (SECTOR_SIZE / 2.0f);
			vPos.y = (SECTOR_SIZE * y) + (SECTOR_SIZE / 2.0f);
			vPos.z = 1.0;

			return vPos;
		}
	};

	TEST_FIXTURE(Fixture, TestWarp_SameSector)
	{
		// arrange
		vec3 vFromPos = GetSectorCenterPos(3, 3);		
		m_pPlayer->SetPos(vFromPos);		
		m_pPlayer2->SetPos(vFromPos);
		
		vec3 vToPos = GetSectorCenterPos(3, 3);
		m_pLink->AddAllowedID(MC_ENTITY_WARP);
		m_pLink2->AddAllowedID(MC_ENTITY_WARP);		


		// act
		gsys.pFieldSystem->Warp(m_pPlayer, vToPos, vec3(0, 1, 0), true);


		// assert
		CHECK_EQUAL(m_pLink->GetCommandID(0), MC_ENTITY_WARP);
		CHECK_EQUAL(m_pLink2->GetCommandID(0), MC_ENTITY_WARP);
	}

	TEST_FIXTURE(Fixture, TestWarp_ToFarSector)
	{
		// arrange
		vec3 vFromPos = GetSectorCenterPos(0, 0);		
		m_pPlayer->SetPos(vFromPos);		
		m_pPlayer2->SetPos(vFromPos);
		
		vec3 vToPos = GetSectorCenterPos(5, 5);
		m_pLink->AddAllowedID(MC_ENTITY_WARP);
		m_pLink2->AddAllowedID(MC_ENTITY_WARP);		


		// act
		gsys.pFieldSystem->Warp(m_pPlayer, vToPos, vec3(0, 1, 0), true);


		// assert
		CHECK_EQUAL(m_pLink->GetCommandID(0), MC_ENTITY_WARP);
		CHECK_EQUAL(m_pLink2->GetCommandID(0), 0);
	}

	TEST_FIXTURE(Fixture, TestWarp_FromFarSector)	
	{
		// arrange
		vec3 vFromPos = GetSectorCenterPos(0, 0);		
		m_pPlayer->SetPos(vFromPos);		
		
		vec3 vToPos = GetSectorCenterPos(5, 5);
		m_pPlayer2->SetPos(vToPos);

		m_pLink->AddAllowedID(MC_ENTITY_WARP);
		m_pLink2->AddAllowedID(MC_ENTITY_WARP);		


		// act
		gsys.pFieldSystem->Warp(m_pPlayer, vToPos, vec3(0, 1, 0), true);


		// assert
		CHECK_EQUAL(m_pLink->GetCommandID(0), MC_ENTITY_WARP);
		CHECK_EQUAL(m_pLink2->GetCommandID(0), 0);
	}

	TEST_FIXTURE(Fixture, TestWarp_NeightborSector)	
	{
		// arrange
		vec3 vFromPos = GetSectorCenterPos(3, 3);		
		m_pPlayer->SetPos(vFromPos);		
		
		vec3 vToPos = GetSectorCenterPos(4, 4);
		m_pPlayer2->SetPos(vToPos);

		m_pLink->AddAllowedID(MC_ENTITY_WARP);
		m_pLink2->AddAllowedID(MC_ENTITY_WARP);		


		// act
		gsys.pFieldSystem->Warp(m_pPlayer, vToPos, vec3(0, 1, 0), true);


		// assert
		CHECK_EQUAL(m_pLink->GetCommandID(0), MC_ENTITY_WARP);
		CHECK_EQUAL(m_pLink2->GetCommandID(0), MC_ENTITY_WARP);
	}

	TEST_FIXTURE(Fixture, TestWarp_FromShortSector)
	{
		// arrange
		vec3 vFromPos = GetSectorCenterPos(3, 3);		
		m_pPlayer->SetPos(vFromPos);		
		
		vec3 vToPos = GetSectorCenterPos(5, 5);
		m_pPlayer2->SetPos(vToPos);

		m_pLink->AddAllowedID(MC_ENTITY_WARP);
		m_pLink2->AddAllowedID(MC_ENTITY_WARP);		


		// act
		gsys.pFieldSystem->Warp(m_pPlayer, vToPos, vec3(0, 1, 0), true);


		// assert
		CHECK_EQUAL(m_pLink->GetCommandID(0), MC_ENTITY_WARP);
		CHECK_EQUAL(m_pLink2->GetCommandID(0), MC_ENTITY_WARP);
	}
}
