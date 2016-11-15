#include "stdafx.h"
#include "GTestForward.h"
#include "GFieldAttrGrid.h"

SUITE(FieldAttrGrid)
{
	struct FGrid
	{
		FGrid()
		{
			pFieldInfo = new GFieldInfo();
			pFieldInfo->SetFieldMinXY_ForTest(-1000, -1000);
			pFieldInfo->SetFieldMaxXY_ForTest(1000, 1000);
			
			pGrid = pFieldInfo->GetAttrGrid();

			pFieldInfo->Init(L"", L"");
			pFieldInfo->Cooking();

		}
		virtual ~FGrid()
		{
			SAFE_DELETE(pFieldInfo);
		}

		GFieldInfo*		pFieldInfo;
		GFieldAttrGrid* pGrid;
	};

	TEST_FIXTURE(FGrid, ConvertPos)
	{
		MPoint pt = pGrid->ConvertToGridPos(0, 0);
		CHECK_EQUAL(3, pt.x);
		CHECK_EQUAL(3, pt.y);
	}

	TEST_FIXTURE(FGrid, IsValidGridPos)
	{
		CHECK_EQUAL(false, pGrid->IsValidGridPos(-1, -1));
		CHECK_EQUAL(false, pGrid->IsValidGridPos(8, 8));
		CHECK_EQUAL(false, pGrid->IsValidGridPos(7, 7));
		CHECK_EQUAL(true, pGrid->IsValidGridPos(6, 6));
		CHECK_EQUAL(true, pGrid->IsValidGridPos(5, 5));
	}

	TEST_FIXTURE(FGrid, Cooking_PvpArea)
	{
		PVP_AREA_INFO infoPVP;
		infoPVP.rcArea = MRect(-500, -500, 500, 500);
		pFieldInfo->m_PvPAreas.push_back(infoPVP);
		pFieldInfo->Cooking();

		GFieldAttrGrid::Cell* pCell = NULL;
		pCell = pGrid->FindCell(-1000, -1000);
		TVALID(pCell);
		CHECK_EQUAL(false, pCell->bPvPEnable);

		pCell = pGrid->FindCell(-500, -500);
		TVALID(pCell);
		CHECK_EQUAL(true, pCell->bPvPEnable);

		pCell = pGrid->FindCell(0, 0);
		TVALID(pCell);
		CHECK_EQUAL(true, pCell->bPvPEnable);

		pCell = pGrid->FindCell(500, 500);
		TVALID(pCell);
		CHECK_EQUAL(true, pCell->bPvPEnable);

		pCell = pGrid->FindCell(1000, 1000);
		TVALID(pCell);
		CHECK_EQUAL(false, pCell->bPvPEnable);
	}
}