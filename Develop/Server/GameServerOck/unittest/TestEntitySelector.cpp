#include "stdafx.h"
#include "FBaseMockLink.h"

SUITE(EntitySelectorTest)
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
		}
		~FGrid()
		{
			SAFE_DELETE(grid);
			SAFE_DELETE(selector);
		}

		GFieldInfo field_info;
		GFieldGrid* grid;
		GFieldGrid::SectorSelector* selector;
	};

	TEST_FIXTURE(FGrid, TestEntitySelector)
	{
		vector<GFieldGrid::Sector*> sectors;
		sectors.push_back(grid->Lookup(2, 2));
		sectors.push_back(grid->Lookup(3, 3));
		sectors.push_back(grid->Lookup(4, 4));
		CHECK(NULL!=sectors[0]);
		CHECK(NULL!=sectors[1]);
		CHECK(NULL!=sectors[2]);

		sectors[0]->AddEntity(ETID_NPC,		MUID(111));
		sectors[1]->AddEntity(ETID_NPC,		MUID(222));
		sectors[1]->AddEntity(ETID_PLAYER,	MUID(333));
		sectors[2]->AddEntity(ETID_NPC,		MUID(444));
		sectors[2]->AddEntity(ETID_PLAYER,	MUID(555));
		sectors[2]->AddEntity(ETID_NPC,		MUID(666));

		GFieldGrid::EntitySelector* grid_selector = grid->GetEntitySelector();
		CHECK(NULL!=grid_selector);

		class EntityCounter : public GFieldGrid::Sector::IVisitor
		{
		public:
			size_t Result() { return m_entities.size(); }

		private:
			void OnVisit(const MUID& uid)
			{
				CHECK(m_entities.end() == find(m_entities.begin(), m_entities.end(), uid));
				m_entities.push_back(uid);
			}
		private:
			vector<MUID> m_entities;
		};

		EntityCounter entityCounter;
		grid_selector->Visit(entityCounter, ETID_ACTOR, MPoint(3, 3), 1);
		CHECK_EQUAL(6, entityCounter.Result());
	}

	TEST_FIXTURE(FGrid, TestEntitySelectorHuge)
	{
		GFieldGrid::Sector* const sector11 = grid->Lookup(1, 1);
		CHECK(NULL!=sector11);
		GFieldGrid::Sector* const sector22 = grid->Lookup(2, 2);
		CHECK(NULL!=sector22);
		GFieldGrid::Sector* const sector33 = grid->Lookup(3, 3);
		CHECK(NULL!=sector33);
		GFieldGrid::Sector* const sector44 = grid->Lookup(4, 4);
		CHECK(NULL!=sector44);
		GFieldGrid::Sector* const sector55 = grid->Lookup(5, 5);
		CHECK(NULL!=sector55);
		GFieldGrid::Sector* const sector66 = grid->Lookup(6, 6);
		CHECK(NULL!=sector66);

		grid->AddEntity(*sector11, ETID_NPC,	MUID(111), true);
		grid->AddEntity(*sector22, ETID_PLAYER,	MUID(222), true);
		grid->AddEntity(*sector33, ETID_NPC,	MUID(333), true);
		grid->AddEntity(*sector44, ETID_NPC,	MUID(444), true);
		grid->AddEntity(*sector55, ETID_PLAYER,	MUID(555), true);
		grid->AddEntity(*sector66, ETID_NPC,	MUID(666), true);
		
		GFieldGrid::EntitySelector* grid_selector = grid->GetEntitySelector();
		CHECK(NULL!=grid_selector);

		int hugeEntityCount = 0;
		class CountHugeEntity : public GFieldGrid::Sector::IVisitor
		{
		public:
			CountHugeEntity(int& result) : m_result(result) {}
		private:
			void OnVisit(const MUID& uid) { ++m_result; }
		private:
			int& m_result;
		};
		grid_selector->VisitInHugeSectorsByPos(CountHugeEntity(hugeEntityCount), ETID_ACTOR, vec3(0, 0, 0));
		CHECK_EQUAL(6, hugeEntityCount);
	}
}
