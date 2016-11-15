#include "stdafx.h"
#include "XWorld.h"

SUITE(World)
{
	TEST(TestWorldInitParam_1)
	{
		int nField = 1;
		bool bBackgroundLoading = false;

		XWorldInitParam param;
		param.SetField(nField, bBackgroundLoading);

		CHECK_EQUAL(nField, param.nFieldID);
		CHECK_EQUAL(bBackgroundLoading, param.bBackgroundLoading);
		CHECK(wstring(L"") == param.strZoneName);
		CHECK_EQUAL(true, param.bLoadPhysx);
	}
	TEST(TestWorldInitParam_2)
	{
		const wchar_t* szZoneName = L"BirdTown";
		bool bBackgroundLoading = true;
		bool bLoadPhyx = false;

		XWorldInitParam param;
		param.SetZone(szZoneName, bLoadPhyx, bBackgroundLoading);

		CHECK_EQUAL(0, param.nFieldID);
		CHECK_EQUAL(bBackgroundLoading, param.bBackgroundLoading);
		CHECK(wstring(szZoneName) == param.strZoneName);
		CHECK_EQUAL(bLoadPhyx, param.bLoadPhysx);
	}

	TEST(TestWorld_IsManagedSector)
	{
		XWorld world;

		int nSectorSize = 1;

		int my_sx = 3;
		int my_sy = 3;

		CHECK_EQUAL(true, world.IsManagedSector(2, 2, my_sx, my_sy, nSectorSize));
		CHECK_EQUAL(true, world.IsManagedSector(2, 3, my_sx, my_sy, nSectorSize));
		CHECK_EQUAL(true, world.IsManagedSector(2, 4, my_sx, my_sy, nSectorSize));
		CHECK_EQUAL(true, world.IsManagedSector(3, 2, my_sx, my_sy, nSectorSize));
		CHECK_EQUAL(true, world.IsManagedSector(3, 3, my_sx, my_sy, nSectorSize));
		CHECK_EQUAL(true, world.IsManagedSector(3, 4, my_sx, my_sy, nSectorSize));
		CHECK_EQUAL(true, world.IsManagedSector(4, 2, my_sx, my_sy, nSectorSize));
		CHECK_EQUAL(true, world.IsManagedSector(4, 3, my_sx, my_sy, nSectorSize));
		CHECK_EQUAL(true, world.IsManagedSector(4, 4, my_sx, my_sy, nSectorSize));

		CHECK_EQUAL(false, world.IsManagedSector(1, 1, my_sx, my_sy, nSectorSize));
		CHECK_EQUAL(false, world.IsManagedSector(5, 5, my_sx, my_sy, nSectorSize));
	}


}