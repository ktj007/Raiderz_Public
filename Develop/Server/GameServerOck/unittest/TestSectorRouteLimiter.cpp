#include "stdafx.h"
#include "GFieldgrid.h"
#include "GTestForward.h"
#include "GConst.h"
#include "GCommandCenter.h"
#include "FBaseMockLink.h"
#include "GPacketRouteLimiter.h"
#include "GPacketRouteLimiterMgr.h"

SUITE(PacketRouteLimiter)
{
	struct FGrid : public FBaseMockLink
	{
		FGrid()
		{
			old_SECTOR_CROWDED_LEVEL_CAPACITY = GConst::SECTOR_CROWDED_LEVEL_CAPACITY;
			old_SECTOR_PACKET_IGNORE_EACH_COUNT = GConst::SECTOR_PACKET_IGNORE_EACH_COUNT;
			GConst::SECTOR_CROWDED_LEVEL_CAPACITY = 2;
			GConst::SECTOR_PACKET_IGNORE_EACH_COUNT = 2;

			field_info.SetFieldMinXY_ForTest(-10000.0f, -10000.0f);
			field_info.SetFieldMaxXY_ForTest(10000.0f, 10000.0f);

			grid = new GFieldGrid();
			grid->Init(MUID(1), &field_info);
		}
		~FGrid()
		{
			SAFE_DELETE(grid);

			GConst::SECTOR_CROWDED_LEVEL_CAPACITY = old_SECTOR_CROWDED_LEVEL_CAPACITY;
			GConst::SECTOR_PACKET_IGNORE_EACH_COUNT = old_SECTOR_PACKET_IGNORE_EACH_COUNT;
		}

		GFieldInfo field_info;
		GFieldGrid* grid;
		int old_SECTOR_CROWDED_LEVEL_CAPACITY;
		int old_SECTOR_PACKET_IGNORE_EACH_COUNT;
	};

	TEST_FIXTURE(FGrid, LifeCycle)
	{
		GFieldGrid::Sector* const sector55 = grid->Lookup(5, 5);
		CHECK(NULL!=sector55);
		CHECK(NULL == grid->GetPacketRouteLimiterMgr()->Get(sector55->Position()));
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(222));
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(333));
		CHECK(NULL != grid->GetPacketRouteLimiterMgr()->Get(sector55->Position()));
		grid->RemoveEntity(sector55->Position(), ETID_PLAYER, MUID(222));
		grid->RemoveEntity(sector55->Position(), ETID_PLAYER, MUID(333));
		CHECK(NULL == grid->GetPacketRouteLimiterMgr()->Get(sector55->Position()));
	}

	TEST_FIXTURE(FGrid, LifeCycle_ZeroCase)
	{
		GFieldGrid::Sector* const sector55 = grid->Lookup(5, 5);
		CHECK(NULL!=sector55);

		GConst::SECTOR_CROWDED_LEVEL_CAPACITY = 0;
		CHECK(NULL == grid->GetPacketRouteLimiterMgr()->Get(sector55->Position()));
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(222));
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(333));
		CHECK(NULL == grid->GetPacketRouteLimiterMgr()->Get(sector55->Position()));
	}

	TEST_FIXTURE(FGrid, LifeCycle_ZeroCase2)
	{
		GFieldGrid::Sector* const sector55 = grid->Lookup(5, 5);
		CHECK(NULL!=sector55);

		GConst::SECTOR_PACKET_IGNORE_EACH_COUNT = 0;
		CHECK(NULL == grid->GetPacketRouteLimiterMgr()->Get(sector55->Position()));
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(222));
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(333));
		CHECK(NULL == grid->GetPacketRouteLimiterMgr()->Get(sector55->Position()));
	}

	TEST_FIXTURE(FGrid, CalcLevel)
	{
		GFieldGrid::Sector* const sector55 = grid->Lookup(5, 5);
		CHECK(NULL!=sector55);
		const MPoint& sector55Pt(sector55->Position());

		CHECK_EQUAL(0, grid->GetPacketRouteLimiterMgr()->CalcCrowdedLevel(sector55Pt, 0));
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(1));
		CHECK_EQUAL(0, grid->GetPacketRouteLimiterMgr()->CalcCrowdedLevel(sector55Pt, 1));
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(2));
		CHECK_EQUAL(1, grid->GetPacketRouteLimiterMgr()->CalcCrowdedLevel(sector55Pt, 2));
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(3));
		CHECK_EQUAL(1, grid->GetPacketRouteLimiterMgr()->CalcCrowdedLevel(sector55Pt, 3));
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(4));
		CHECK_EQUAL(2, grid->GetPacketRouteLimiterMgr()->CalcCrowdedLevel(sector55Pt, 4));
		grid->RemoveEntity(sector55Pt, ETID_PLAYER, MUID(4));
		CHECK_EQUAL(1, grid->GetPacketRouteLimiterMgr()->CalcCrowdedLevel(sector55Pt, 3));
		grid->RemoveEntity(sector55Pt, ETID_PLAYER, MUID(3));
		CHECK_EQUAL(1, grid->GetPacketRouteLimiterMgr()->CalcCrowdedLevel(sector55Pt, 2));
		grid->RemoveEntity(sector55Pt, ETID_PLAYER, MUID(2));
		CHECK_EQUAL(0, grid->GetPacketRouteLimiterMgr()->CalcCrowdedLevel(sector55Pt, 1));
		grid->RemoveEntity(sector55Pt, ETID_PLAYER, MUID(1));
		CHECK_EQUAL(0, grid->GetPacketRouteLimiterMgr()->CalcCrowdedLevel(sector55Pt, 0));
	}

	TEST_FIXTURE(FGrid, CircularPlayerQueue)
	{
		int old_SECTOR_PACKET_LIMIT_QUEUE_SIZE = GConst::SECTOR_PACKET_LIMIT_QUEUE_SIZE;
		GConst::SECTOR_PACKET_LIMIT_QUEUE_SIZE = 2;

		GPacketRouteLimiter limiter;
		const int CROWED_LEVEL_1_QTY = 2;
		const int CMD_ID = 1234;
		CHECK_EQUAL(0, limiter.GetRegisteredPlayerQty());
		limiter.TryRoute(CROWED_LEVEL_1_QTY, CMD_ID, MUID(1));
		CHECK_EQUAL(1, limiter.GetRegisteredPlayerQty());
		limiter.TryRoute(CROWED_LEVEL_1_QTY, CMD_ID, MUID(1));
		CHECK_EQUAL(1, limiter.GetRegisteredPlayerQty());
		limiter.TryRoute(CROWED_LEVEL_1_QTY, CMD_ID, MUID(2));
		CHECK_EQUAL(2, limiter.GetRegisteredPlayerQty());
		limiter.TryRoute(CROWED_LEVEL_1_QTY, CMD_ID, MUID(3));
		CHECK_EQUAL(2, limiter.GetRegisteredPlayerQty());
		limiter.TryRoute(CROWED_LEVEL_1_QTY, CMD_ID, MUID(4));
		CHECK_EQUAL(2, limiter.GetRegisteredPlayerQty());

		GConst::SECTOR_PACKET_LIMIT_QUEUE_SIZE = old_SECTOR_PACKET_LIMIT_QUEUE_SIZE;
	}

	TEST_FIXTURE(FGrid, LifeCycle_NeighborSectors)
	{
		GFieldGrid::Sector* const sector55 = grid->Lookup(5, 5);
		CHECK(NULL!=sector55);
		MPoint ptSector(5,5);
		CHECK(NULL == grid->GetPacketRouteLimiterMgr()->Get(ptSector));
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(222));
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(333));

		for (int y=5-1; y<=5+1; ++y)
		{
			for (int x=5-1; x<=5+1; ++x)
			{
				CHECK(NULL != grid->GetPacketRouteLimiterMgr()->Get(MPoint(x,y)));
			}
		}

		CHECK(NULL == grid->GetPacketRouteLimiterMgr()->Get(MPoint(9,9)));
		CHECK(NULL == grid->GetPacketRouteLimiterMgr()->Get(MPoint(1,1)));

		grid->RemoveEntity(ptSector, ETID_PLAYER, MUID(222));
		grid->RemoveEntity(ptSector, ETID_PLAYER, MUID(333));

		for (int y=5-1; y<=5+1; ++y)
		{
			for (int x=5-1; x<=5+1; ++x)
			{
				CHECK(NULL == grid->GetPacketRouteLimiterMgr()->Get(MPoint(x,y)));
			}
		}
	}

	TEST_FIXTURE(FGrid, LifeCycle_NeighborSectors2)
	{
		GFieldGrid::Sector* const sector55 = grid->Lookup(5, 5);
		CHECK(NULL!=sector55);

		MPoint ptSector(5,5);
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(222));
		grid->AddEntity(*sector55, ETID_PLAYER, MUID(333));

		GFieldGrid::Sector* const sector66 = grid->Lookup(6, 6);
		CHECK(NULL!=sector66);

		MPoint ptSector2(6,6);
		grid->AddEntity(*sector66, ETID_PLAYER, MUID(222));
		grid->AddEntity(*sector66, ETID_PLAYER, MUID(333));
		grid->RemoveEntity(ptSector2, ETID_PLAYER, MUID(222));
		grid->RemoveEntity(ptSector2, ETID_PLAYER, MUID(333));

		CHECK(NULL != grid->GetPacketRouteLimiterMgr()->Get(MPoint(6,6)));
		CHECK(NULL == grid->GetPacketRouteLimiterMgr()->Get(MPoint(7,7)));

		grid->RemoveEntity(ptSector, ETID_PLAYER, MUID(222));
		grid->RemoveEntity(ptSector, ETID_PLAYER, MUID(333));

		for (int y=5-1; y<=5+1; ++y)
		{
			for (int x=5-1; x<=5+1; ++x)
			{
				CHECK(NULL == grid->GetPacketRouteLimiterMgr()->Get(MPoint(x,y)));
			}
		}
	}

	TEST_FIXTURE(FGrid, TryRoute)
	{
		GFieldGrid::Sector* sector = grid->Lookup(5, 5);
		CHECK(NULL!=sector);
		GFieldGrid::Sector& sectorRef(*sector);
		MPoint ptSector(5,5);

		grid->AddEntity(sectorRef, ETID_PLAYER, MUID(222));
		grid->AddEntity(sectorRef, ETID_PLAYER, MUID(333));

		CHECK_EQUAL(1, grid->GetPacketRouteLimiterMgr()->CalcCrowdedLevel(ptSector, 2));

		CHECK_EQUAL(false, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 1, MUID(1)));
		CHECK_EQUAL(false, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 1, MUID(1)));
		CHECK_EQUAL(true, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 1, MUID(1)));
		CHECK_EQUAL(false, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 1, MUID(1)));
		CHECK_EQUAL(false, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 1, MUID(1)));
		CHECK_EQUAL(true, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 1, MUID(1)));

		CHECK_EQUAL(false, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 1, MUID(2)));
		CHECK_EQUAL(false, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 1, MUID(2)));
		CHECK_EQUAL(true, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 1, MUID(2)));
		CHECK_EQUAL(false, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 1, MUID(2)));
		CHECK_EQUAL(false, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 1, MUID(2)));
		CHECK_EQUAL(true, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 1, MUID(2)));

		CHECK_EQUAL(false, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 2, MUID(1)));
		CHECK_EQUAL(false, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 2, MUID(2)));
		CHECK_EQUAL(false, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 2, MUID(1)));
		CHECK_EQUAL(false, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 2, MUID(2)));
		CHECK_EQUAL(true, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 2, MUID(1)));
		CHECK_EQUAL(true, grid->GetPacketRouteLimiterMgr()->TryRoute(sectorRef, 2, MUID(2)));
	}

	TEST_FIXTURE(FGrid, RouteToCell)
	{
		int nCmdID = 9999;
		MGetCommandDescMap()->AddCommandDesc(
			new MCommandDesc(nCmdID, MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "TestName", "TestDesc"));
		MCommandDesc* pDesc = MGetCommandDescMap()->GetCommandDescByID(nCmdID);
		TVALID(pDesc);
		CHECK_EQUAL(true, pDesc->IsFlag(MCDT_FILTER_CROWDED));

		MockField* pField = GUTHelper_Field::DefaultMockField();
		pField->SetInfo(&field_info);

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(pField, vec3(0,0,0));
		CHECK(NULL!=pPlayer);
		GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(pField, vec3(0,0,0));
		CHECK(NULL!=pPlayer2);
		MockLink* m_pLinkTester = test.network.NewLink(pPlayer);
		m_pLinkTester->IgnoreAll();
		m_pLinkTester->AddAllowedID(nCmdID);

		MCommand* pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer2, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(0, m_pLinkTester->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer2, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(0, m_pLinkTester->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer2, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		// �ݺ�
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer2, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer2, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer2, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(2, m_pLinkTester->GetCommandCount());

		pField->Destroy();
	}

	TEST_FIXTURE(FGrid, RouteToCell_CollectEssentialReceivers)
	{
		int nCmdID = 9999;
		MGetCommandDescMap()->AddCommandDesc(
			new MCommandDesc(nCmdID, MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "TestName", "TestDesc"));
		MCommandDesc* pDesc = MGetCommandDescMap()->GetCommandDescByID(nCmdID);
		TVALID(pDesc);
		CHECK_EQUAL(true, pDesc->IsFlag(MCDT_FILTER_CROWDED));

		MockField* pField = GUTHelper_Field::DefaultMockField();
		CHECK(NULL!=pField);
		pField->SetInfo(&field_info);

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(pField, vec3(0,0,0));
		CHECK(NULL!=pPlayer);
		GEntityPlayer* pMember = GUTHelper::NewEntityPlayer(pField, vec3(0,0,0));
		CHECK(NULL!=pMember);
		test.party.CreateParty(pPlayer, pMember);
		MockLink* pPlayerLink = test.network.NewLink(pPlayer);
		MockLink* pMemberLink = test.network.NewLink(pMember);
		pPlayerLink->IgnoreAll();
		pPlayerLink->AddAllowedID(nCmdID);
		pMemberLink->IgnoreAll();
		pMemberLink->AddAllowedID(nCmdID);

		MCommand* pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(0, pPlayerLink->GetCommandCount());
		CHECK_EQUAL(0, pMemberLink->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(0, pPlayerLink->GetCommandCount());
		CHECK_EQUAL(0, pMemberLink->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(1, pPlayerLink->GetCommandCount());
		CHECK_EQUAL(1, pMemberLink->GetCommandCount());

		pField->Destroy();
	}

	TEST_FIXTURE(FGrid, RouteToCell_NonCreatedLimiter)
	{
		int nCmdID = 9998;
		MGetCommandDescMap()->AddCommandDesc(
			new MCommandDesc(nCmdID, MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "TestName", "TestDesc"));
		MCommandDesc* pDesc = MGetCommandDescMap()->GetCommandDescByID(nCmdID);
		TVALID(pDesc);
		CHECK_EQUAL(true, pDesc->IsFlag(MCDT_FILTER_CROWDED));

		MockField* pField = GUTHelper_Field::DefaultMockField();
		CHECK(NULL!=pField);
		pField->SetInfo(&field_info);

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(pField, vec3(0,0,0));
		CHECK(NULL!=pPlayer);
		MockLink* m_pLinkTester = test.network.NewLink(pPlayer);
		m_pLinkTester->IgnoreAll();
		m_pLinkTester->AddAllowedID(nCmdID);

		MCommand* pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(2, m_pLinkTester->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(3, m_pLinkTester->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(4, m_pLinkTester->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer, *pField, pCmd, MPoint(0, 0), 1, 0.0f);
		CHECK_EQUAL(5, m_pLinkTester->GetCommandCount());

		pField->Destroy();
	}

	TEST_FIXTURE(FGrid, RouteToCell2)
	{
		int nCmdID = 9997;
		MGetCommandDescMap()->AddCommandDesc(
			new MCommandDesc(nCmdID, MCDT_MACHINE2MACHINE|MCDT_FILTER_CROWDED, MCF_S2C, "TestName", "TestDesc"));
		MCommandDesc* pDesc = MGetCommandDescMap()->GetCommandDescByID(nCmdID);
		TVALID(pDesc);
		CHECK_EQUAL(true, pDesc->IsFlag(MCDT_FILTER_CROWDED));

		MockField* pField = GUTHelper_Field::DefaultMockField();
		CHECK(NULL!=pField);
		pField->SetInfo(&field_info);

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(pField, vec3(0,0,0));
		CHECK(NULL!=pPlayer);
		GEntityPlayer* pPlayer2 = GUTHelper::NewEntityPlayer(pField, vec3(0,0,0));
		CHECK(NULL!=pPlayer2);
		MockLink* m_pLinkTester = test.network.NewLink(pPlayer);
		m_pLinkTester->IgnoreAll();
		m_pLinkTester->AddAllowedID(nCmdID);

		MCommand* pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer2, *pField, pCmd, MPoint(1, 0), 1, 0.0f);
		CHECK_EQUAL(0, m_pLinkTester->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer2, *pField, pCmd, MPoint(1, 0), 1, 0.0f);
		CHECK_EQUAL(0, m_pLinkTester->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer2, *pField, pCmd, MPoint(1, 0), 1, 0.0f);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		// �ݺ�
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer2, *pField, pCmd, MPoint(1, 0), 1, 0.0f);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer2, *pField, pCmd, MPoint(1, 0), 1, 0.0f);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		pCmd = gsys.pCommandCenter->NewCommand(nCmdID);
		GFieldGridRouter::RouteToCell(*pPlayer2, *pField, pCmd, MPoint(1, 0), 1, 0.0f);
		CHECK_EQUAL(2, m_pLinkTester->GetCommandCount());

		pField->Destroy();
	}
}