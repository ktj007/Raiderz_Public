#include "stdafx.h"
#include "GUTHelper_Field.h"
#include "FBaseMockLink.h"
#include "GDBManager.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "CCommandTable.h"
#include "GCmdHandler_Action.h"
#include "GCommand.h"

SUITE(EntityPlayer_Move)
{
	struct FEntityPlayer_Move : public FBaseMockLink
	{
		FEntityPlayer_Move()
		{
			field = GUTHelper_Field::DefaultMockField();
			player.entity = GUTHelper::NewEntityPlayer(field);
			player.link = NewLink(player.entity);

			anotherPlayer.entity = GUTHelper::NewEntityPlayer(field);
			anotherPlayer.link = NewLink(anotherPlayer.entity);
			anotherPlayer.link->AddIgnoreID(MC_ACTION_STOP);

			SECTOR_SIZE_X = field->GetGrid().GetSizeX();
			SECTOR_SIZE_Y = field->GetGrid().GetSizeY();
		}

		virtual ~FEntityPlayer_Move()
		{
			field->Destroy();
		}

		struct Player
		{
			GEntityPlayer*	entity;
			MockLink*		link;
		};

		MockField*	field;
		Player		player;
		Player		anotherPlayer;
		int			SECTOR_SIZE_X;
		int			SECTOR_SIZE_Y;
	};

	// 현재 이동패킷이 초당 8개씩 온다.
	// 테스트는 클라이언트가 위의 규격에 맞춰 이동 패킷을 보낸다고 가정한다
	
	TEST_FIXTURE(FEntityPlayer_Move, testRouteMovingCmdInSameSector)
	{
		// 초당 8개인데, 5개로 보정한다
		// 24개 -> 15개

		anotherPlayer.entity->SetPos(vec3(0.0f, 0.0f, 0.0f));	// sector 0, 0

		TD_PC_MOVE moveDesc;
		moveDesc.vTarPos = vec3(0.0f, 0.0f, 0.0f);				// sector 0, 0
		moveDesc.svDir = vec3(0.0f, 1.0f, 0.0f);
		moveDesc.nFlags = MOVEMENTFLAG_FORWARD;

		for (int i = 0; i < 12; ++i)
		{
			// 
			MCommand* movingCmd = MakeNewCommand(MC_ACTION_MOVE_REQ, 1, NEW_SINGLE_BLOB(&moveDesc, sizeof(TD_PC_MOVE)));
			movingCmd->SetSenderUID(player.entity->GetUID());

			GCmdHandler_Action::OnRequestMove(movingCmd, NULL);
		}

		{
			MCommand* movingCmd = MakeNewCommand(MC_ACTION_STOP_REQ, 2, NEW_VEC(0.0f, 0.0f, 0.0f), NEW_SVEC2(1.0f, 0.0f));
			movingCmd->SetSenderUID(player.entity->GetUID());

			GCmdHandler_Action::OnRequestMove(movingCmd, NULL);
		}

		for (int i = 0; i < 20; ++i)
		{
			MCommand* movingCmd = MakeNewCommand(MC_ACTION_MOVE_REQ, 1, NEW_SINGLE_BLOB(&moveDesc, sizeof(TD_PC_MOVE)));
			movingCmd->SetSenderUID(player.entity->GetUID());

			GCmdHandler_Action::OnRequestMove(movingCmd, NULL);
		}

		{
			MCommand* movingCmd = MakeNewCommand(MC_ACTION_STOP_REQ, 2, NEW_VEC(0.0f, 0.0f, 0.0f), NEW_SVEC2(1.0f, 0.0f));
			movingCmd->SetSenderUID(player.entity->GetUID());

			GCmdHandler_Action::OnRequestMove(movingCmd, NULL);
		}

		CHECK_EQUAL(30, anotherPlayer.link->GetCommandCount());
		CHECK_EQUAL(MC_ACTION_MOVE, anotherPlayer.link->GetCommand(0).GetID());
	}

	TEST_FIXTURE(FEntityPlayer_Move, testRouteMovingCmdInNeighborSector)
	{
	}

	TEST_FIXTURE(FEntityPlayer_Move, testRouteMovingCmdToPVPTarget)
	{
	}

	TEST_FIXTURE(FEntityPlayer_Move, testRouteMovingCmdToParty)
	{
	}
}
