#include "stdafx.h"
#include "ZFixtureHelper.h"
#include "ZGameServerObject.h"
#include "ZReceiveGameServerUIDSelector.h"
#include "ZPlayerLogic.h"
#include "TestDetails.h"
#include "ZPlayerStateChanger.h"


SUITE(ReceiveGameServerUIDSelector)
{
	struct Fixture : public FBaseServerField, public FBasePlayer, public FBaseMockLink
	{
		Fixture()
		{
		}

		~Fixture()
		{

		}

		MAP_RECEIEVE_GAMESERVER_UID mapReceiveGameServerUID;

		void Select(int nCID)
		{
			ZReceiveGameServerUIDSelector::Select(nCID, mapReceiveGameServerUID);
		}

		void CheckSelected(MUID nServerUID, int nPlayerCID)
		{
			MAP_RECEIEVE_GAMESERVER_UID::iterator itor = mapReceiveGameServerUID.find(nServerUID);;
			CHECK(mapReceiveGameServerUID.end() != itor);
			CHECK_EQUAL(nPlayerCID, itor->second.front());
		}

		void CheckUnselected()
		{
			CHECK_EQUAL(true, mapReceiveGameServerUID.empty());
		}
	};

	// 플레이어가 서버에 있으면, 채류중인 서버가 선택된다.
	TEST_FIXTURE(Fixture, InWorldPlayer)
	{
		ZGameServerObject* pServer = AddGameServerObject();
		ZPlayer* pPlayer = AddNewPlayerInWorld(pServer->GetID());
		
		Select(pPlayer->GetCID());
		CheckSelected(pServer->GetUID(), pPlayer->GetCID());
	}

	// 플레이어가 필드이동중이면, 예약된 서버가 선택된다.
	TEST_FIXTURE(Fixture, MoveServer)
	{
		ZGameServerObject* pServer = AddGameServerObject();
		ZPlayer* pPlayer = AddNewPlayerInWorld(pServer->GetID());
		ZGameServerObject* pReservedServer = AddGameServerObject();

		pPlayer->SetState(PS_MOVE_GAMESERVER);
		pPlayer->SetReservedGameServerID(pReservedServer->GetID());

		Select(pPlayer->GetCID());
		CheckSelected(pReservedServer->GetUID(), pPlayer->GetCID());
	}

	// 플레이어가 서버에 없고 필드이동중도 아니면, 서버가 선택되지 않는다.
	TEST_FIXTURE(Fixture, NotInworldAndNotMoveServer)
	{
		ZPlayer* pPlayer = AddNewPlayerInWorld(ZTestHelper::NewID());
		pPlayer->SetState(PS_END);

		Select(pPlayer->GetCID());
		CheckUnselected();
	}
}