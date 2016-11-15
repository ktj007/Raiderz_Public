#include "stdafx.h"
#include "CSHelper.h"
#include "MUID.h"

SUITE(Helper)
{
	TEST(TestMUIDGenerator)
	{
		MUIDGenerator gen;
		CHECK_EQUAL(MUID(0,1), gen.Generate());
		CHECK_EQUAL(MUID(0,2), gen.Generate());
	}

	TEST(TestMUIDGenerator_Seed)
	{
		MUIDGenerator gen(MUID(1000, 0));
		CHECK_EQUAL(MUID(1000,1), gen.Generate());
		CHECK_EQUAL(MUID(1000,2), gen.Generate());

		MUIDGenerator gen2(MUID(100, 5000));
		CHECK_EQUAL(MUID(100,5001), gen2.Generate());
		CHECK_EQUAL(MUID(100,5002), gen2.Generate());

		MUIDGenerator gen3;
		gen3.SetSeed(MUID(10, 10));
		CHECK_EQUAL(MUID(10,11), gen3.Generate());
		CHECK_EQUAL(MUID(10,12), gen3.Generate());

	}

// 	TEST(TestMUIDRandomGenerator)
// 	{
// 		MUIDRandomGenerator uidGen(10);
// 
// 		set<MUID>	setUID;
// 		
// 		// 한 10만번 돌려도 같은 UID가 없다.
// 		const int MAX_COUNT = 100000;
// 
// 		for (int i = 0; i < MAX_COUNT; i++)
// 		{
// 			MUID uidNew = uidGen.Generate();
// 
// 			setUID.insert(uidNew);
// 		}
// 
// 		CHECK_EQUAL(MAX_COUNT, setUID.size());
// 	}

}