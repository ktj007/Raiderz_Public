#include "stdafx.h"
#include "GFieldgrid.h"
#include "GTestForward.h"
#include "FBaseMockLink.h"

SUITE(FieldGrid)
{
	struct FGrid : public FBaseMockLink
	{
		FGrid()
		{
			field_info.SetFieldMinXY_ForTest(-10000.0f, -10000.0f);
			field_info.SetFieldMaxXY_ForTest(10000.0f, 10000.0f);

			grid = new GFieldGrid();
			grid->Init(MUID(1), &field_info);

			selector = new GFieldGrid::SectorSelector(*grid);
			entitySelector = new GFieldGrid::EntitySelector(*selector);
		}
		~FGrid()
		{
			SAFE_DELETE(grid);
			SAFE_DELETE(selector);
			SAFE_DELETE(entitySelector);
		}

		GFieldInfo field_info;
		GFieldGrid* grid;
		GFieldGrid::SectorSelector* selector;
		GFieldGrid::EntitySelector* entitySelector;
	};

	TEST_FIXTURE(FGrid, Init)
	{
		CHECK_EQUAL(7, grid->m_nSizeX);
		CHECK_EQUAL(7, grid->m_nSizeY);
		CHECK(grid->m_Sectors);
		// 셀들 순회해서 크래쉬나는지 검사
		for (int y = 0; y < grid->m_nSizeY; y++)
			for (int x = 0; x < grid->m_nSizeX; x++)
				grid->m_Sectors[grid->CalcFlatPoint(x, y)] = GFieldGrid::Sector();
		grid->Clear();
		CHECK_EQUAL(-1, grid->m_nSizeX);
		CHECK_EQUAL(-1, grid->m_nSizeY);
		CHECK(!grid->m_Sectors);
	}

	TEST_FIXTURE(FGrid, CalcFlatPoint)
	{
		CHECK_EQUAL(0, grid->CalcFlatPoint(0, 0));
		CHECK_EQUAL(10, grid->CalcFlatPoint(3, 1));
		CHECK_EQUAL(48, grid->CalcFlatPoint(6, 6));
	}

	TEST_FIXTURE(FGrid, ConvertToIdx)
	{
		MPoint ptIdx;
		ptIdx = grid->ConvertToIdx(vec3(0,0,0));
		CHECK_EQUAL(3, ptIdx.x);
		CHECK_EQUAL(3, ptIdx.y);
		ptIdx = grid->ConvertToIdx(vec3(-10000,10000,0));
		CHECK_EQUAL(0, ptIdx.x);
		CHECK_EQUAL(6, ptIdx.y);
	}

	TEST(FieldCell_EntityMgr)
	{
		GFieldGrid::Sector cell;
		CHECK_EQUAL(0, cell.GetEntityQty());
		for (int i = 0; i < ETID_MAX; ++i)
			CHECK_EQUAL(0, cell.GetEntityQty((ENTITY_TYPEID)i));

		MUID uidNPC = MUID(111);
		CHECK_EQUAL(false, cell.IsExist(uidNPC));
		cell.AddEntity(ETID_NPC, uidNPC);
		CHECK_EQUAL(true, cell.IsExist(uidNPC));
		cell.RemoveEntity(ETID_NPC, uidNPC);
		CHECK_EQUAL(false, cell.IsExist(uidNPC));

		cell.AddEntity(ETID_NPC, MUID(222));
		cell.AddEntity(ETID_PLAYER, MUID(333));
		CHECK_EQUAL(2, cell.GetEntityQty());
		CHECK_EQUAL(1, cell.GetEntityQty(ETID_NPC));
		CHECK_EQUAL(1, cell.GetEntityQty(ETID_PLAYER));
		CHECK_EQUAL(2, cell.GetEntityQty(ETID_ACTOR));
		CHECK_EQUAL(0, cell.GetEntityQty(ETID_BUFF_ENTITY));

		{
			class CheckOneNPCEntity : public GFieldGrid::Sector::IVisitor
			{
			public:
				CheckOneNPCEntity() : totalCnt(0) {}
				~CheckOneNPCEntity()
				{
					CHECK_EQUAL(1, totalCnt);
				}

			private:
				void OnVisit(const MUID& uid)
				{
					CHECK_EQUAL(MUID(222), uid);
					++totalCnt;
				}

			private:
				int totalCnt;
			};
			cell.Visit(CheckOneNPCEntity(), ETID_NPC);
		}

		{
			class CheckOnePlayerEntity : public GFieldGrid::Sector::IVisitor
			{
			public:
				CheckOnePlayerEntity() : totalCnt(0) {}
				~CheckOnePlayerEntity()
				{
					CHECK_EQUAL(1, totalCnt);
				}

			private:
				void OnVisit(const MUID& uid)
				{
					CHECK_EQUAL(MUID(333), uid);
					++totalCnt;
				}

			private:
				int totalCnt;
			};
			cell.Visit(CheckOnePlayerEntity(), ETID_PLAYER);
		}

		{
			class CheckAllEntities : public GFieldGrid::Sector::IVisitor
			{
			public:
				CheckAllEntities() : totalCnt(0) {}
				~CheckAllEntities()
				{
					CHECK_EQUAL(2, totalCnt);
				}

			private:
				void OnVisit(const MUID& uid)
				{
					CHECK(MUID(222) == uid || MUID(333) == uid);
					++totalCnt;
				}

			private:
				int totalCnt;
			};
			cell.Visit(CheckAllEntities(), ETID_MAX);
		}
	
		cell.RemoveEntity(ETID_NPC, MUID(222));
		CHECK_EQUAL(1, cell.GetEntityQty());
		CHECK_EQUAL(0, cell.GetEntityQty(ETID_NPC));
		CHECK_EQUAL(1, cell.GetEntityQty(ETID_PLAYER));
		CHECK_EQUAL(1, cell.GetEntityQty(ETID_ACTOR));
		CHECK_EQUAL(0, cell.GetEntityQty(ETID_BUFF_ENTITY));
		cell.ClearEntity();
		CHECK_EQUAL(0, cell.GetEntityQty());
		CHECK_EQUAL(0, cell.GetEntityQty(ETID_NPC));
		CHECK_EQUAL(0, cell.GetEntityQty(ETID_PLAYER));
		CHECK_EQUAL(0, cell.GetEntityQty(ETID_ACTOR));
		CHECK_EQUAL(0, cell.GetEntityQty(ETID_BUFF_ENTITY));
	}

	TEST_FIXTURE(FGrid, LookupForCell)
	{
		CHECK(!grid->Lookup(-1, -1));
		CHECK(!grid->Lookup(999, 999));
		GFieldGrid::Sector* cell = grid->Lookup(5, 5);
		CHECK(cell);
		cell->AddEntity(ETID_NPC, MUID(222));
		GFieldGrid::Sector* cell2 = grid->Lookup(5, 5);
		CHECK(cell2);
		CHECK_EQUAL(cell, cell2);

		class CheckOneNPCEntity : public GFieldGrid::Sector::IVisitor
		{
		public:
			CheckOneNPCEntity() : totalCnt(0) {}
			~CheckOneNPCEntity()
			{
				CHECK_EQUAL(1, totalCnt);
			}

		private:
			void OnVisit(const MUID& uid)
			{
				CHECK_EQUAL(MUID(222), uid);
				++totalCnt;
			}

		private:
			int totalCnt;
		};
		cell2->Visit(CheckOneNPCEntity(), ETID_MAX);
	}

	TEST_FIXTURE(FGrid, SelectSingle)
	{
		GFieldGrid::Sector* cell = grid->Lookup(5, 5);
		CHECK(cell);
		cell->AddEntity(ETID_NPC, MUID(222));
		GFieldGrid::Sector* cell2 = selector->Lookup(5, 5);
		CHECK(cell2);

		class CheckOneNPCEntity : public GFieldGrid::Sector::IVisitor
		{
		public:
			CheckOneNPCEntity() : totalCnt(0) {}
			~CheckOneNPCEntity()
			{
				CHECK_EQUAL(1, totalCnt);
			}

		private:
			void OnVisit(const MUID& uid)
			{
				CHECK_EQUAL(MUID(222), uid);
				++totalCnt;
			}

		private:
			int totalCnt;
		};
		cell2->Visit(CheckOneNPCEntity(), ETID_MAX);
	}

	TEST_FIXTURE(FGrid, SelectAllNeighborCells)
	{
		for (int y = 0; y < grid->GetSizeY(); y++)
			for (int x = 0; x < grid->GetSizeX(); x++)
				grid->Lookup(x, y)->AddEntity(ETID_NPC, MUID(grid->CalcFlatPoint(x, y)));

		vector<GFieldGrid::Sector*> vecCells;

		class SectorCollector : public GFieldGrid::SectorSelector::IVisitor
		{
		public:
			SectorCollector(vector<GFieldGrid::Sector*>& sectors) : m_sectors(sectors) {}

		private:
			void OnVisit(GFieldGrid::Sector& sector)
			{
				m_sectors.push_back(&sector);
			}

		private:
			vector<GFieldGrid::Sector*>& m_sectors;
		};

		selector->VisitNeighbors(SectorCollector(vecCells), MPoint(0, 0), 1);
		TVALID_EQ(4, vecCells.size());
		vecCells.clear();
		selector->VisitNeighbors(SectorCollector(vecCells), MPoint(0, 5), 1);
		TVALID_EQ(6, vecCells.size());
		CHECK_EQUAL(true, vecCells[0]->IsExist(MUID(grid->CalcFlatPoint(0, 4))));
		CHECK_EQUAL(true, vecCells[1]->IsExist(MUID(grid->CalcFlatPoint(1, 4))));
		CHECK_EQUAL(true, vecCells[2]->IsExist(MUID(grid->CalcFlatPoint(0, 5))));
		CHECK_EQUAL(true, vecCells[3]->IsExist(MUID(grid->CalcFlatPoint(1, 5))));
		CHECK_EQUAL(true, vecCells[4]->IsExist(MUID(grid->CalcFlatPoint(0, 6))));
		CHECK_EQUAL(true, vecCells[5]->IsExist(MUID(grid->CalcFlatPoint(1, 6))));
		vecCells.clear();
		selector->VisitNeighbors(SectorCollector(vecCells), MPoint(5, 5), 1);
		TVALID_EQ(9, vecCells.size());
		vecCells.clear();
		selector->VisitNeighbors(SectorCollector(vecCells), MPoint(7, 7), 1);
		TVALID_EQ(1, vecCells.size());
		CHECK_EQUAL(true, vecCells[0]->IsExist(MUID(grid->CalcFlatPoint(6, 6))));
	}

	TEST_FIXTURE(FGrid, SelectNeighborCells)
	{
		for (int y = 0; y < grid->GetSizeY(); y++)
			for (int x = 0; x < grid->GetSizeX(); x++)
				grid->Lookup(x, y)->AddEntity(ETID_NPC, MUID(grid->CalcFlatPoint(x, y)));

		vector<GFieldGrid::Sector*> vecCells;

		class CollectSectors : public GFieldGrid::SectorSelector::IVisitor
		{
		public:
			CollectSectors(vector<GFieldGrid::Sector*>& sectors) : m_sectors(sectors)
			{
				m_sectors.clear();
			}

		private:
			void OnVisit(GFieldGrid::Sector& sector)
			{
				m_sectors.push_back(&sector);
			}

		private:
			vector<GFieldGrid::Sector*>& m_sectors;
		};
		selector->VisitNeighborsByRadius(CollectSectors(vecCells), vec3(0,0,0), 1);
		TVALID_EQ(1, vecCells.size());
		CHECK_EQUAL(true, vecCells[0]->IsExist(MUID(grid->CalcFlatPoint(3, 3))));
		selector->VisitNeighborsByRadius(CollectSectors(vecCells), vec3(SECTOR_SIZE*2,SECTOR_SIZE*2,0), 1);
		TVALID_EQ(1, vecCells.size());
		CHECK_EQUAL(true, vecCells[0]->IsExist(MUID(grid->CalcFlatPoint(5, 5))));
		
		selector->VisitNeighborsByRadius(CollectSectors(vecCells), vec3(SECTOR_SIZE*2,SECTOR_SIZE*2,0), SECTOR_SIZE);
		TVALID_EQ(9, vecCells.size());
		CHECK_EQUAL(true, vecCells[0]->IsExist(MUID(grid->CalcFlatPoint(4, 4))));
		CHECK_EQUAL(true, vecCells[1]->IsExist(MUID(grid->CalcFlatPoint(5, 4))));
		CHECK_EQUAL(true, vecCells[2]->IsExist(MUID(grid->CalcFlatPoint(6, 4))));
		CHECK_EQUAL(true, vecCells[3]->IsExist(MUID(grid->CalcFlatPoint(4, 5))));
		CHECK_EQUAL(true, vecCells[4]->IsExist(MUID(grid->CalcFlatPoint(5, 5))));
		CHECK_EQUAL(true, vecCells[5]->IsExist(MUID(grid->CalcFlatPoint(6, 5))));
		CHECK_EQUAL(true, vecCells[6]->IsExist(MUID(grid->CalcFlatPoint(4, 6))));
		CHECK_EQUAL(true, vecCells[7]->IsExist(MUID(grid->CalcFlatPoint(5, 6))));
		CHECK_EQUAL(true, vecCells[8]->IsExist(MUID(grid->CalcFlatPoint(6, 6))));

		selector->VisitNeighborsByRadius(CollectSectors(vecCells), vec3(SECTOR_SIZE*2,SECTOR_SIZE*2,0), SECTOR_SIZE / 2);
		TVALID_EQ(4, vecCells.size());
		CHECK_EQUAL(true, vecCells[0]->IsExist(MUID(grid->CalcFlatPoint(4, 4))));
		CHECK_EQUAL(true, vecCells[1]->IsExist(MUID(grid->CalcFlatPoint(5, 4))));
		CHECK_EQUAL(true, vecCells[2]->IsExist(MUID(grid->CalcFlatPoint(4, 5))));
		CHECK_EQUAL(true, vecCells[3]->IsExist(MUID(grid->CalcFlatPoint(5, 5))));
	}

	TEST_FIXTURE(FGrid, SelectNeighborCells_ForAABB)
	{
		for (int y = 0; y < grid->GetSizeY(); y++)
			for (int x = 0; x < grid->GetSizeX(); x++)
				grid->Lookup(x, y)->AddEntity(ETID_NPC, MUID(grid->CalcFlatPoint(x, y)));

		vector<GFieldGrid::Sector*> vecCells;
		class NeighborSectorsPusher : public GFieldGrid::SectorSelector::IVisitor
		{
		public:
			NeighborSectorsPusher(vector<GFieldGrid::Sector*>& sectors) : m_sectors(sectors)
			{
				m_sectors.clear();
			}

		private:
			void OnVisit(GFieldGrid::Sector& sector)
			{
				m_sectors.push_back(&sector);
			}

		private:
			vector<GFieldGrid::Sector*>& m_sectors;
		};
		selector->VisitNeighborsIntersectWithAABB(NeighborSectorsPusher(vecCells), vec3(0,0,0), MBox(-1, -1, 0, 1, 1, 0));
		TVALID_EQ(1, vecCells.size());
		CHECK_EQUAL(true, vecCells[0]->IsExist(MUID(grid->CalcFlatPoint(3, 3))));

		float fSize = SECTOR_SIZE;
		selector->VisitNeighborsIntersectWithAABB(NeighborSectorsPusher(vecCells), vec3(SECTOR_SIZE*2,SECTOR_SIZE*2,0), MBox(-fSize, -fSize, 0, fSize, fSize, 0));
		TVALID_EQ(9, vecCells.size());
		CHECK_EQUAL(true, vecCells[0]->IsExist(MUID(grid->CalcFlatPoint(4, 4))));
		CHECK_EQUAL(true, vecCells[1]->IsExist(MUID(grid->CalcFlatPoint(5, 4))));
		CHECK_EQUAL(true, vecCells[2]->IsExist(MUID(grid->CalcFlatPoint(6, 4))));
		CHECK_EQUAL(true, vecCells[3]->IsExist(MUID(grid->CalcFlatPoint(4, 5))));
		CHECK_EQUAL(true, vecCells[4]->IsExist(MUID(grid->CalcFlatPoint(5, 5))));
		CHECK_EQUAL(true, vecCells[5]->IsExist(MUID(grid->CalcFlatPoint(6, 5))));
		CHECK_EQUAL(true, vecCells[6]->IsExist(MUID(grid->CalcFlatPoint(4, 6))));
		CHECK_EQUAL(true, vecCells[7]->IsExist(MUID(grid->CalcFlatPoint(5, 6))));
		CHECK_EQUAL(true, vecCells[8]->IsExist(MUID(grid->CalcFlatPoint(6, 6))));

		fSize = SECTOR_SIZE/2;
		selector->VisitNeighborsIntersectWithAABB(NeighborSectorsPusher(vecCells), vec3(SECTOR_SIZE*2,SECTOR_SIZE*2,0), MBox(-fSize, -fSize, 0, fSize, fSize, 0));
		TVALID_EQ(4, vecCells.size());
		CHECK_EQUAL(true, vecCells[0]->IsExist(MUID(grid->CalcFlatPoint(4, 4))));
		CHECK_EQUAL(true, vecCells[1]->IsExist(MUID(grid->CalcFlatPoint(5, 4))));
		CHECK_EQUAL(true, vecCells[2]->IsExist(MUID(grid->CalcFlatPoint(4, 5))));
		CHECK_EQUAL(true, vecCells[3]->IsExist(MUID(grid->CalcFlatPoint(5, 5))));
	}

	TEST_FIXTURE(FGrid, DeltaSelector_CalcMovingDirection)
	{
		GFieldGrid::DeltaSelector delta_selector(*selector);
		CHECK_EQUAL(GFieldGrid::MD_SAME, delta_selector.CalcMovingDirection(MPoint(1, 1), MPoint(1, 1)));
		CHECK_EQUAL(GFieldGrid::MD_FULL, delta_selector.CalcMovingDirection(MPoint(1, 1), MPoint(4, 5)));
		CHECK_EQUAL(GFieldGrid::MD_TOP, delta_selector.CalcMovingDirection(MPoint(1, 1), MPoint(1, 0)));
		CHECK_EQUAL(GFieldGrid::MD_LEFT, delta_selector.CalcMovingDirection(MPoint(1, 1), MPoint(0, 1)));
		CHECK_EQUAL(GFieldGrid::MD_RIGHT, delta_selector.CalcMovingDirection(MPoint(1, 1), MPoint(2, 1)));
		CHECK_EQUAL(GFieldGrid::MD_BOTTOM, delta_selector.CalcMovingDirection(MPoint(1, 1), MPoint(1, 2)));
		CHECK_EQUAL(GFieldGrid::MD_TOPLEFT, delta_selector.CalcMovingDirection(MPoint(1, 1), MPoint(0, 0)));
		CHECK_EQUAL(GFieldGrid::MD_TOPRIGHT, delta_selector.CalcMovingDirection(MPoint(1, 1), MPoint(2, 0)));
		CHECK_EQUAL(GFieldGrid::MD_BOTTOMLEFT, delta_selector.CalcMovingDirection(MPoint(1, 1), MPoint(0, 2)));
		CHECK_EQUAL(GFieldGrid::MD_BOTTOMRIGHT, delta_selector.CalcMovingDirection(MPoint(1, 1), MPoint(2, 2)));
	}

// 	TEST_FIXTURE(FGrid, DeltaSelector_GetDeltaCells)
// 	{
// 		for (int y = 0; y < grid->GetSizeY(); y++)
// 			for (int x = 0; x < grid->GetSizeX(); x++)
// 				grid->Lookup(x, y)->AddEntity(ETID_NPC, MUID(grid->CalcFlatPoint(x, y)));
// 
// 		vector<GFieldGrid::Sector*> vecOldCells;
// 		vector<GFieldGrid::Sector*> vecNewCells;
// 		GFieldGrid::DeltaSelector delta_selector(*selector);
// 
// 		// SAME
// 		delta_selector.GetDeltaSectors(vecOldCells, vecNewCells, MPoint(3, 3), MPoint(3, 3));
// 		TVALID_EQ(0, vecOldCells.size());
// 		TVALID_EQ(0, vecNewCells.size());
// 
// 		// FULL
// 		delta_selector.GetDeltaSectors(vecOldCells, vecNewCells, MPoint(3, 3), MPoint(5, 5));
// 		TVALID_EQ(9, vecOldCells.size());
// 		CHECK_EQUAL(true, vecOldCells[0]->IsExist(MUID(grid->CalcFlatPoint(2, 2))));
// 		CHECK_EQUAL(true, vecOldCells[1]->IsExist(MUID(grid->CalcFlatPoint(3, 2))));
// 		CHECK_EQUAL(true, vecOldCells[2]->IsExist(MUID(grid->CalcFlatPoint(4, 2))));
// 		CHECK_EQUAL(true, vecOldCells[3]->IsExist(MUID(grid->CalcFlatPoint(2, 3))));
// 		CHECK_EQUAL(true, vecOldCells[4]->IsExist(MUID(grid->CalcFlatPoint(3, 3))));
// 		CHECK_EQUAL(true, vecOldCells[5]->IsExist(MUID(grid->CalcFlatPoint(4, 3))));
// 		CHECK_EQUAL(true, vecOldCells[6]->IsExist(MUID(grid->CalcFlatPoint(2, 4))));
// 		CHECK_EQUAL(true, vecOldCells[7]->IsExist(MUID(grid->CalcFlatPoint(3, 4))));
// 		CHECK_EQUAL(true, vecOldCells[8]->IsExist(MUID(grid->CalcFlatPoint(4, 4))));
// 		TVALID_EQ(9, vecNewCells.size());
// 		CHECK_EQUAL(true, vecNewCells[0]->IsExist(MUID(grid->CalcFlatPoint(4, 4))));
// 		CHECK_EQUAL(true, vecNewCells[1]->IsExist(MUID(grid->CalcFlatPoint(5, 4))));
// 		CHECK_EQUAL(true, vecNewCells[2]->IsExist(MUID(grid->CalcFlatPoint(6, 4))));
// 		CHECK_EQUAL(true, vecNewCells[3]->IsExist(MUID(grid->CalcFlatPoint(4, 5))));
// 		CHECK_EQUAL(true, vecNewCells[4]->IsExist(MUID(grid->CalcFlatPoint(5, 5))));
// 		CHECK_EQUAL(true, vecNewCells[5]->IsExist(MUID(grid->CalcFlatPoint(6, 5))));
// 		CHECK_EQUAL(true, vecNewCells[6]->IsExist(MUID(grid->CalcFlatPoint(4, 6))));
// 		CHECK_EQUAL(true, vecNewCells[7]->IsExist(MUID(grid->CalcFlatPoint(5, 6))));
// 		CHECK_EQUAL(true, vecNewCells[8]->IsExist(MUID(grid->CalcFlatPoint(6, 6))));
// 
// 		// TOP
// 		delta_selector.GetDeltaSectors(vecOldCells, vecNewCells, MPoint(3, 3), MPoint(3, 2));
// 		TVALID_EQ(3, vecOldCells.size());
// 		CHECK_EQUAL(true, vecOldCells[0]->IsExist(MUID(grid->CalcFlatPoint(2, 4))));
// 		CHECK_EQUAL(true, vecOldCells[1]->IsExist(MUID(grid->CalcFlatPoint(3, 4))));
// 		CHECK_EQUAL(true, vecOldCells[2]->IsExist(MUID(grid->CalcFlatPoint(4, 4))));
// 		TVALID_EQ(3, vecNewCells.size());
// 		CHECK_EQUAL(true, vecNewCells[0]->IsExist(MUID(grid->CalcFlatPoint(2, 1))));
// 		CHECK_EQUAL(true, vecNewCells[1]->IsExist(MUID(grid->CalcFlatPoint(3, 1))));
// 		CHECK_EQUAL(true, vecNewCells[2]->IsExist(MUID(grid->CalcFlatPoint(4, 1))));
// 
// 		// BOTTOM
// 		delta_selector.GetDeltaSectors(vecOldCells, vecNewCells, MPoint(3, 3), MPoint(3, 4));
// 		TVALID_EQ(3, vecOldCells.size());
// 		CHECK_EQUAL(true, vecOldCells[0]->IsExist(MUID(grid->CalcFlatPoint(2, 2))));
// 		CHECK_EQUAL(true, vecOldCells[1]->IsExist(MUID(grid->CalcFlatPoint(3, 2))));
// 		CHECK_EQUAL(true, vecOldCells[2]->IsExist(MUID(grid->CalcFlatPoint(4, 2))));
// 		TVALID_EQ(3, vecNewCells.size());
// 		CHECK_EQUAL(true, vecNewCells[0]->IsExist(MUID(grid->CalcFlatPoint(2, 5))));
// 		CHECK_EQUAL(true, vecNewCells[1]->IsExist(MUID(grid->CalcFlatPoint(3, 5))));
// 		CHECK_EQUAL(true, vecNewCells[2]->IsExist(MUID(grid->CalcFlatPoint(4, 5))));
// 
// 		// LEFT
// 		delta_selector.GetDeltaSectors(vecOldCells, vecNewCells, MPoint(3, 3), MPoint(2, 3));
// 		TVALID_EQ(3, vecOldCells.size());
// 		CHECK_EQUAL(true, vecOldCells[0]->IsExist(MUID(grid->CalcFlatPoint(4, 2))));
// 		CHECK_EQUAL(true, vecOldCells[1]->IsExist(MUID(grid->CalcFlatPoint(4, 3))));
// 		CHECK_EQUAL(true, vecOldCells[2]->IsExist(MUID(grid->CalcFlatPoint(4, 4))));
// 		TVALID_EQ(3, vecNewCells.size());
// 		CHECK_EQUAL(true, vecNewCells[0]->IsExist(MUID(grid->CalcFlatPoint(1, 2))));
// 		CHECK_EQUAL(true, vecNewCells[1]->IsExist(MUID(grid->CalcFlatPoint(1, 3))));
// 		CHECK_EQUAL(true, vecNewCells[2]->IsExist(MUID(grid->CalcFlatPoint(1, 4))));
// 
// 		// RIGHT
// 		delta_selector.GetDeltaSectors(vecOldCells, vecNewCells, MPoint(3, 3), MPoint(4, 3));
// 		TVALID_EQ(3, vecOldCells.size());
// 		CHECK_EQUAL(true, vecOldCells[0]->IsExist(MUID(grid->CalcFlatPoint(2, 2))));
// 		CHECK_EQUAL(true, vecOldCells[1]->IsExist(MUID(grid->CalcFlatPoint(2, 3))));
// 		CHECK_EQUAL(true, vecOldCells[2]->IsExist(MUID(grid->CalcFlatPoint(2, 4))));
// 		TVALID_EQ(3, vecNewCells.size());
// 		CHECK_EQUAL(true, vecNewCells[0]->IsExist(MUID(grid->CalcFlatPoint(5, 2))));
// 		CHECK_EQUAL(true, vecNewCells[1]->IsExist(MUID(grid->CalcFlatPoint(5, 3))));
// 		CHECK_EQUAL(true, vecNewCells[2]->IsExist(MUID(grid->CalcFlatPoint(5, 4))));
// 
// 		// TOPLEFT
// 		delta_selector.GetDeltaSectors(vecOldCells, vecNewCells, MPoint(3, 3), MPoint(2, 2));
// 		TVALID_EQ(5, vecOldCells.size());
// 		CHECK_EQUAL(true, vecOldCells[0]->IsExist(MUID(grid->CalcFlatPoint(2, 4))));
// 		CHECK_EQUAL(true, vecOldCells[1]->IsExist(MUID(grid->CalcFlatPoint(3, 4))));
// 		CHECK_EQUAL(true, vecOldCells[2]->IsExist(MUID(grid->CalcFlatPoint(4, 4))));
// 		CHECK_EQUAL(true, vecOldCells[3]->IsExist(MUID(grid->CalcFlatPoint(4, 2))));
// 		CHECK_EQUAL(true, vecOldCells[4]->IsExist(MUID(grid->CalcFlatPoint(4, 3))));
// 		TVALID_EQ(5, vecNewCells.size());
// 		CHECK_EQUAL(true, vecNewCells[0]->IsExist(MUID(grid->CalcFlatPoint(1, 1))));
// 		CHECK_EQUAL(true, vecNewCells[1]->IsExist(MUID(grid->CalcFlatPoint(2, 1))));
// 		CHECK_EQUAL(true, vecNewCells[2]->IsExist(MUID(grid->CalcFlatPoint(3, 1))));
// 		CHECK_EQUAL(true, vecNewCells[3]->IsExist(MUID(grid->CalcFlatPoint(1, 2))));
// 		CHECK_EQUAL(true, vecNewCells[4]->IsExist(MUID(grid->CalcFlatPoint(1, 3))));
// 
// 		// TOPRIGHT
// 		delta_selector.GetDeltaSectors(vecOldCells, vecNewCells, MPoint(3, 3), MPoint(4, 2));
// 		TVALID_EQ(5, vecOldCells.size());
// 		CHECK_EQUAL(true, vecOldCells[0]->IsExist(MUID(grid->CalcFlatPoint(2, 4))));
// 		CHECK_EQUAL(true, vecOldCells[1]->IsExist(MUID(grid->CalcFlatPoint(3, 4))));
// 		CHECK_EQUAL(true, vecOldCells[2]->IsExist(MUID(grid->CalcFlatPoint(4, 4))));
// 		CHECK_EQUAL(true, vecOldCells[3]->IsExist(MUID(grid->CalcFlatPoint(2, 2))));
// 		CHECK_EQUAL(true, vecOldCells[4]->IsExist(MUID(grid->CalcFlatPoint(2, 3))));
// 		TVALID_EQ(5, vecNewCells.size());
// 		CHECK_EQUAL(true, vecNewCells[0]->IsExist(MUID(grid->CalcFlatPoint(3, 1))));
// 		CHECK_EQUAL(true, vecNewCells[1]->IsExist(MUID(grid->CalcFlatPoint(4, 1))));
// 		CHECK_EQUAL(true, vecNewCells[2]->IsExist(MUID(grid->CalcFlatPoint(5, 1))));
// 		CHECK_EQUAL(true, vecNewCells[3]->IsExist(MUID(grid->CalcFlatPoint(5, 2))));
// 		CHECK_EQUAL(true, vecNewCells[4]->IsExist(MUID(grid->CalcFlatPoint(5, 3))));
// 
// 		// BOTTOMLEFT
// 		delta_selector.GetDeltaSectors(vecOldCells, vecNewCells, MPoint(3, 3), MPoint(2, 4));
// 		TVALID_EQ(5, vecOldCells.size());
// 		CHECK_EQUAL(true, vecOldCells[0]->IsExist(MUID(grid->CalcFlatPoint(2, 2))));
// 		CHECK_EQUAL(true, vecOldCells[1]->IsExist(MUID(grid->CalcFlatPoint(3, 2))));
// 		CHECK_EQUAL(true, vecOldCells[2]->IsExist(MUID(grid->CalcFlatPoint(4, 2))));
// 		CHECK_EQUAL(true, vecOldCells[3]->IsExist(MUID(grid->CalcFlatPoint(4, 3))));
// 		CHECK_EQUAL(true, vecOldCells[4]->IsExist(MUID(grid->CalcFlatPoint(4, 4))));
// 		TVALID_EQ(5, vecNewCells.size());
// 		CHECK_EQUAL(true, vecNewCells[0]->IsExist(MUID(grid->CalcFlatPoint(1, 5))));
// 		CHECK_EQUAL(true, vecNewCells[1]->IsExist(MUID(grid->CalcFlatPoint(2, 5))));
// 		CHECK_EQUAL(true, vecNewCells[2]->IsExist(MUID(grid->CalcFlatPoint(3, 5))));
// 		CHECK_EQUAL(true, vecNewCells[3]->IsExist(MUID(grid->CalcFlatPoint(1, 3))));
// 		CHECK_EQUAL(true, vecNewCells[4]->IsExist(MUID(grid->CalcFlatPoint(1, 4))));
// 
// 		// BOTTOMRIGHT
// 		delta_selector.GetDeltaSectors(vecOldCells, vecNewCells, MPoint(3, 3), MPoint(4, 4));
// 		TVALID_EQ(5, vecOldCells.size());
// 		CHECK_EQUAL(true, vecOldCells[0]->IsExist(MUID(grid->CalcFlatPoint(2, 2))));
// 		CHECK_EQUAL(true, vecOldCells[1]->IsExist(MUID(grid->CalcFlatPoint(3, 2))));
// 		CHECK_EQUAL(true, vecOldCells[2]->IsExist(MUID(grid->CalcFlatPoint(4, 2))));
// 		CHECK_EQUAL(true, vecOldCells[3]->IsExist(MUID(grid->CalcFlatPoint(2, 3))));
// 		CHECK_EQUAL(true, vecOldCells[4]->IsExist(MUID(grid->CalcFlatPoint(2, 4))));
// 		TVALID_EQ(5, vecNewCells.size());
// 		CHECK_EQUAL(true, vecNewCells[0]->IsExist(MUID(grid->CalcFlatPoint(3, 5))));
// 		CHECK_EQUAL(true, vecNewCells[1]->IsExist(MUID(grid->CalcFlatPoint(4, 5))));
// 		CHECK_EQUAL(true, vecNewCells[2]->IsExist(MUID(grid->CalcFlatPoint(5, 5))));
// 		CHECK_EQUAL(true, vecNewCells[3]->IsExist(MUID(grid->CalcFlatPoint(5, 3))));
// 		CHECK_EQUAL(true, vecNewCells[4]->IsExist(MUID(grid->CalcFlatPoint(5, 4))));
// 	}

	TEST_FIXTURE(FGrid, DeltaSelector_GetEntitiesInDeltaCells)
	{
		for (int y = 0; y < grid->GetSizeY(); y++)
			for (int x = 0; x < grid->GetSizeX(); x++)
				grid->Lookup(x, y)->AddEntity(ETID_NPC, MUID(grid->CalcFlatPoint(x, y)));

		GFieldGrid::DeltaSelector delta_selector(*selector);

		class EntityCollector : public GFieldGrid::Sector::IVisitor
		{
		public:
			vector<MUID> m_entities;

		private:
			void OnVisit(const MUID& uid)
			{
				m_entities.push_back(uid);
			}
		};
		EntityCollector entityCollector;
		vector<MUID>&	entities(entityCollector.m_entities);

		// SAME
		delta_selector.VisitNewSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(3, 3), 1);
		CHECK_EQUAL(0, entities.size());

		delta_selector.VisitOldSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(3, 3), 1);
		CHECK_EQUAL(0, entities.size());

		// FULL
		delta_selector.VisitOldSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(5, 5), 1);
		CHECK_EQUAL(9, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 3))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 3))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 3))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 4))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 4))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 4))));

		entities.clear();
		delta_selector.VisitNewSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(5, 5), 1);
		CHECK_EQUAL(9, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 4))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(5, 4))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(6, 4))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 5))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(5, 5))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(6, 5))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 6))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(5, 6))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(6, 6))));

		// TOP
		entities.clear();
		delta_selector.VisitOldSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(3, 2), 1);
		CHECK_EQUAL(3, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 4))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 4))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 4))));

		entities.clear();
		delta_selector.VisitNewSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(3, 2), 1);
		CHECK_EQUAL(3, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 1))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 1))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 1))));
		
		// BOTTOM
		entities.clear();
		delta_selector.VisitOldSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(3, 4), 1);
		CHECK_EQUAL(3, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 2))));

		entities.clear();
		delta_selector.VisitNewSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(3, 4), 1);
		CHECK_EQUAL(3, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 5))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 5))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 5))));

		// LEFT
		entities.clear();
		delta_selector.VisitOldSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(2, 3), 1);
		CHECK_EQUAL(3, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 3))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 4))));

		entities.clear();
		delta_selector.VisitNewSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(2, 3), 1);
		CHECK_EQUAL(3, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(1, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(1, 3))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(1, 4))));

		// RIGHT
		entities.clear();
		delta_selector.VisitOldSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(4, 3), 1);
		CHECK_EQUAL(3, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 3))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 4))));

		entities.clear();
		delta_selector.VisitNewSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(4, 3), 1);
		CHECK_EQUAL(3, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(5, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(5, 3))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(5, 4))));

		// TOPLEFT
		entities.clear();
		delta_selector.VisitOldSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(2, 2), 1);
		CHECK_EQUAL(5, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 4))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 4))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 4))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 3))));

		entities.clear();
		delta_selector.VisitNewSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(2, 2), 1);
		CHECK_EQUAL(5, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(1, 1))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 1))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 1))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(1, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(1, 3))));

		// TOPRIGHT
		entities.clear();
		delta_selector.VisitOldSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(4, 2), 1);
		CHECK_EQUAL(5, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 4))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 4))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 4))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 3))));

		entities.clear();
		delta_selector.VisitNewSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(4, 2), 1);
		CHECK_EQUAL(5, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 1))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 1))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(5, 1))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(5, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(5, 3))));

		// BOTTOMLEFT
		entities.clear();
		delta_selector.VisitOldSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(2, 4), 1);
		CHECK_EQUAL(5, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 3))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 4))));

		entities.clear();
		delta_selector.VisitNewSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(2, 4), 1);
		CHECK_EQUAL(5, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(1, 5))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 5))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 5))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(1, 3))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(1, 4))));

		// BOTTOMRIGHT
		entities.clear();
		delta_selector.VisitOldSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(4, 4), 1);
		CHECK_EQUAL(5, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 2))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 3))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(2, 4))));

		entities.clear();
		delta_selector.VisitNewSectors(entityCollector, ETID_NPC, MPoint(3, 3), MPoint(4, 4), 1);
		CHECK_EQUAL(5, entities.size());

		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(3, 5))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(4, 5))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(5, 5))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(5, 3))));
		CHECK_EQUAL(true, entities.end() != find(entities.begin(), entities.end(), MUID(grid->CalcFlatPoint(5, 4))));
	}

	TEST_FIXTURE(FGrid, EntityMgr)
	{
		GFieldGrid::Sector* sector55 = grid->Lookup(5, 5);
		CHECK(NULL!=sector55);

		// 추가
		grid->AddEntity(*sector55, ETID_NPC, MUID(222));
		{
			class CheckOneNPCEntity : public GFieldGrid::Sector::IVisitor
			{
			public:
				CheckOneNPCEntity() : totalCnt(0) {}
				~CheckOneNPCEntity()
				{
					CHECK_EQUAL(1, totalCnt);
				}

			private:
				void OnVisit(const MUID& uid)
				{
					CHECK_EQUAL(MUID(222), uid);
					++totalCnt;
				}

			private:
				int totalCnt;
			};
			sector55->Visit(CheckOneNPCEntity(), ETID_MAX);
		}

		// 삭제
		grid->RemoveEntity(sector55->Position(), ETID_NPC, MUID(222));

		{
			class CheckNoEntity : public GFieldGrid::Sector::IVisitor
			{
			public:
				CheckNoEntity() : totalCnt(0) {}
				~CheckNoEntity()
				{
					CHECK_EQUAL(0, totalCnt);
				}

			private:
				void OnVisit(const MUID& uid)
				{
					CHECK(0);
					++totalCnt;
				}

			private:
				int totalCnt;
			};
			sector55->Visit(CheckNoEntity(), ETID_MAX);
		}

		grid->AddEntity(*sector55, ETID_NPC, MUID(222));
		// 이동 실패
		grid->MoveEntity(MPoint(5,4), MPoint(5,6), ETID_NPC, MUID(222));
		GFieldGrid::Sector* sector56 = selector->Lookup(5, 6);
		CHECK(NULL!=sector56);

		{
			class CheckNoEntity : public GFieldGrid::Sector::IVisitor
			{
			public:
				CheckNoEntity() : totalCnt(0) {}
				~CheckNoEntity()
				{
					CHECK_EQUAL(0, totalCnt);
				}

			private:
				void OnVisit(const MUID& uid)
				{
					CHECK(0);
					++totalCnt;
				}

			private:
				int totalCnt;
			};
			sector56->Visit(CheckNoEntity(), ETID_MAX);
		}

		// 이동 성공
		grid->MoveEntity(MPoint(5,5), MPoint(5,6), ETID_NPC, MUID(222));
		sector55 = selector->Lookup(5, 5);

		{
			class CheckNoEntity : public GFieldGrid::Sector::IVisitor
			{
			public:
				CheckNoEntity() : totalCnt(0) {}
				~CheckNoEntity()
				{
					CHECK_EQUAL(0, totalCnt);
				}

			private:
				void OnVisit(const MUID& uid)
				{
					CHECK(0);
					++totalCnt;
				}

			private:
				int totalCnt;
			};
			sector55->Visit(CheckNoEntity(), ETID_MAX);
		}

		sector56 = selector->Lookup(5, 6);

		{
			class CheckOneNPCEntity : public GFieldGrid::Sector::IVisitor
			{
			public:
				CheckOneNPCEntity() : totalCnt(0) {}
				~CheckOneNPCEntity()
				{
					CHECK_EQUAL(1, totalCnt);
				}

			private:
				void OnVisit(const MUID& uid)
				{
					CHECK_EQUAL(MUID(222), uid);
					++totalCnt;
				}

			private:
				int totalCnt;
			};
			sector56->Visit(CheckOneNPCEntity(), ETID_MAX);
		}
	}

	TEST_FIXTURE(FGrid, HugeCell)
	{
		GFieldGrid::Sector* sector55 = grid->Lookup(5, 5);
		CHECK(NULL!=sector55);

		CHECK(!grid->LookupHugeSector(MPoint(5,5)));
		grid->AddEntity(*sector55, ETID_NPC, MUID(222), false);
		CHECK(!grid->LookupHugeSector(MPoint(5,5)));
		grid->AddEntity(*sector55, ETID_NPC, MUID(333), true);
		CHECK(grid->LookupHugeSector(MPoint(5,5)));
		grid->AddEntity(*sector55, ETID_NPC, MUID(444), true);
		CHECK(grid->LookupHugeSector(MPoint(5,5)));
		CHECK(!grid->LookupHugeSector(MPoint(5,6)));
		grid->RemoveEntity(MPoint(5,5), ETID_NPC, MUID(444), true);
		CHECK(grid->LookupHugeSector(MPoint(5,5)));
		grid->RemoveEntity(MPoint(5,5), ETID_NPC, MUID(333), true);
		CHECK(!grid->LookupHugeSector(MPoint(5,5)));
		grid->RemoveEntity(MPoint(5,5), ETID_NPC, MUID(222), false);
		CHECK(!grid->LookupHugeSector(MPoint(5,5)));
	}

	TEST_FIXTURE(FGrid, RouteToHugeSector)
	{
		MockField* pField = GUTHelper_Field::DefaultMockField();
		CHECK(NULL!=pField);
		pField->SetInfo(&field_info);

		GEntityNPC* pNPC = test.npc.SpawnNPC(pField);
		CHECK(NULL!=pNPC);

		GFieldGrid::Sector* const sector55 = pField->GetGrid().Lookup(5, 5);
		CHECK(NULL!=sector55);

		GFieldGrid::Sector* const sector66 = pField->GetGrid().Lookup(6, 6);
		CHECK(NULL!=sector66);

		// 6,6 에 엔티티 추가
		pField->GetGrid().AddEntity(*sector66, ETID_PLAYER, MUID(222), false);

		// 인접 일반 셀에서 라우팅
		tr1::shared_ptr<MCommand> cmd(new MCommand);
		vector<MUID> vecEntities;

		CHECK_EQUAL(true, vecEntities.empty());
		GFieldGridRouter::CollectCellReceivers(vecEntities, *pNPC, *pField, cmd.get(), MPoint(4, 4), 1, 0.0f);
		CHECK_EQUAL(true, vecEntities.empty());
		GFieldGridRouter::CollectCellReceivers(vecEntities, *pNPC, *pField, cmd.get(), MPoint(5, 5), 1, 0.0f);
		CHECK_EQUAL(false, vecEntities.empty());

		// 인접 거대 셀에서 라우팅
		pField->GetGrid().RemoveEntity(MPoint(5,5), ETID_PLAYER, MUID(222));
		pField->GetGrid().AddEntity(*sector55, ETID_PLAYER, MUID(222), true);
		vecEntities.clear();
		CHECK_EQUAL(true, vecEntities.empty());
		GFieldGridRouter::CollectCellReceivers(vecEntities, *pNPC, *pField, cmd.get(), MPoint(13, 0), HUGE_SECTOR_SCALE * 3 / 2, 0.0f);
		CHECK_EQUAL(true, vecEntities.empty());
		GFieldGridRouter::CollectCellReceivers(vecEntities, *pNPC, *pField, cmd.get(), MPoint(1, 1), HUGE_SECTOR_SCALE * 3 / 2, 0.0f);
		CHECK_EQUAL(false, vecEntities.empty());

		pField->Destroy();
	}

	class MockBoundChecker : public GFieldGrid::BoundChecker::Lisener
	{
	public:
		MockBoundChecker()
		{
			Clear();
		}

		void Clear()
		{
			m_nPlayerQtyInBoundary = 0;
		}

		virtual void OnChangeOutters(int nPlayerQtyInBoundary) override
		{
			m_nPlayerQtyInBoundary = nPlayerQtyInBoundary;
		}
	public:
		int m_nPlayerQtyInBoundary;
	};

	void EnterPlayerToSector(GFieldGrid* grid, MockBoundChecker& lisener, GFieldGrid::Sector& sector, MUID uidPlayer)
	{
		lisener.Clear();
		grid->AddEntity(sector, ETID_PLAYER, uidPlayer);
	}

	void LeavePlayerToSector(GFieldGrid* grid, MockBoundChecker& lisener, const MPoint& ptSector, MUID uidPlayer)
	{
		lisener.Clear();
		grid->RemoveEntity(ptSector, ETID_PLAYER, uidPlayer);
	}

	TEST_FIXTURE(FGrid, BoundChecker)
	{
		MockBoundChecker lisener;
		grid->CreateBoundChecker(&lisener);

		GFieldGrid::Sector* sector00 = grid->Lookup(0, 0);
		CHECK(NULL!=sector00);
		GFieldGrid::Sector* sector05 = grid->Lookup(0, 5);
		CHECK(NULL!=sector05);
		GFieldGrid::Sector* sector50 = grid->Lookup(5, 0);
		CHECK(NULL!=sector50);
		GFieldGrid::Sector* sector66 = grid->Lookup(6, 6);
		CHECK(NULL!=sector66);
		GFieldGrid::Sector* sector06 = grid->Lookup(0, 6);
		CHECK(NULL!=sector06);
		GFieldGrid::Sector* sector60 = grid->Lookup(6, 0);
		CHECK(NULL!=sector60);

		CHECK_EQUAL(0, lisener.m_nPlayerQtyInBoundary);
		EnterPlayerToSector(grid, lisener, *sector00, MUID(111));
		CHECK_EQUAL(1, lisener.m_nPlayerQtyInBoundary);
		EnterPlayerToSector(grid, lisener, *sector05, MUID(222));
		CHECK_EQUAL(2, lisener.m_nPlayerQtyInBoundary);
		EnterPlayerToSector(grid, lisener, *sector50, MUID(333));
		CHECK_EQUAL(3, lisener.m_nPlayerQtyInBoundary);
		EnterPlayerToSector(grid, lisener, *sector66, MUID(444));
		CHECK_EQUAL(4, lisener.m_nPlayerQtyInBoundary);
		EnterPlayerToSector(grid, lisener, *sector06, MUID(555));
		CHECK_EQUAL(5, lisener.m_nPlayerQtyInBoundary);
		EnterPlayerToSector(grid, lisener, *sector60, MUID(666));
		CHECK_EQUAL(6, lisener.m_nPlayerQtyInBoundary);

		LeavePlayerToSector(grid, lisener, MPoint(0,0), MUID(111));
		CHECK_EQUAL(5, lisener.m_nPlayerQtyInBoundary);
		LeavePlayerToSector(grid, lisener, MPoint(0,5), MUID(222));
		CHECK_EQUAL(4, lisener.m_nPlayerQtyInBoundary);
		LeavePlayerToSector(grid, lisener, MPoint(5,0), MUID(333));
		CHECK_EQUAL(3, lisener.m_nPlayerQtyInBoundary);
		LeavePlayerToSector(grid, lisener, MPoint(6,6), MUID(444));
		CHECK_EQUAL(2, lisener.m_nPlayerQtyInBoundary);
		LeavePlayerToSector(grid, lisener, MPoint(0,6), MUID(555));
		CHECK_EQUAL(1, lisener.m_nPlayerQtyInBoundary);
		LeavePlayerToSector(grid, lisener, MPoint(6,0), MUID(666));
		CHECK_EQUAL(0, lisener.m_nPlayerQtyInBoundary);
	}

	TEST_FIXTURE(FGrid, CheckHeight)
	{
		GFieldInfo* pFieldInfo = GUTHelper_Field::NewFieldInfo();
		pFieldInfo->m_fSectorHeight = 50.0f;
		
		GField* pField = GUTHelper_Field::NewMockField(pFieldInfo);
		
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(pField, vec3(0.0f, 0.0f, 100.0f));
		MUID uidPlayer = pPlayer->GetUID();

		CHECK(!GFieldGridRouter::CheckHeight(0.0f, uidPlayer, *pField));
		CHECK(GFieldGridRouter::CheckHeight(50.0f, uidPlayer, *pField));
		CHECK(GFieldGridRouter::CheckHeight(100.0f, uidPlayer, *pField));
		CHECK(GFieldGridRouter::CheckHeight(150.0f, uidPlayer, *pField));
		CHECK(!GFieldGridRouter::CheckHeight(200.0f, uidPlayer, *pField));

		SAFE_DELETE(pField);
	}

	TEST_FIXTURE(FGrid, CheckEmptyPlayer)
	{
		GFieldGrid::Sector* const sector = grid->Lookup(5, 5);
		CHECK(NULL!=sector);
		CHECK_EQUAL(0, sector->GetEntityQty());
		for (int i = 0; i < ETID_MAX; ++i)
			CHECK_EQUAL(0, sector->GetEntityQty((ENTITY_TYPEID)i));

		CHECK_EQUAL(true, entitySelector->IsEmptyPlayers(MPoint(5, 5), 1));
		MUID playerUID = MUID(111);
		CHECK_EQUAL(false, sector->IsExist(playerUID));
		sector->AddEntity(ETID_PLAYER, playerUID);
		CHECK_EQUAL(true, sector->IsExist(playerUID));
		CHECK_EQUAL(false, entitySelector->IsEmptyPlayers(MPoint(5, 5), 1));
		grid->RemoveEntity(MPoint(5, 5), ETID_PLAYER, playerUID);
		CHECK_EQUAL(false, sector->IsExist(playerUID));
		CHECK_EQUAL(true, entitySelector->IsEmptyPlayers(MPoint(5, 5), 1));
	}
}
