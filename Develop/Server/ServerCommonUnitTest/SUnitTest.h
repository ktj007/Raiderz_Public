#pragma once

#include "ServerCommonUnitTestLib.h"
#include "MTest.h"

UnitTest::TestList& GetStage2TestList();
#define _TS2		GetStage2TestList()


UnitTest::TestList& GetStage3TestList();
#define _TS3		GetStage3TestList()

UnitTest::TestList& GetStage4TestList();
#define _TS4		GetStage4TestList()

UnitTest::TestList& GetStageAccountDBTestList();
#define _TS_ACCDB	GetStageAccountDBTestList()

UnitTest::TestList& GetStageGameDBTestList();
#define _TS_GAMEDB	GetStageGameDBTestList()

UnitTest::TestList& GetStageLogDBTestList();
#define _TS_LOGDB	GetStageLogDBTestList()


#define UNITTEST_STAGE1_NAME			"Stage1"
#define UNITTEST_STAGE2_NAME			"Stage2"
#define UNITTEST_STAGE3_NAME			"stage3"
#define UNITTEST_STAGE4_NAME			"stage4"
#define UNITTEST_STAGE_ACCDB_NAME		"accountdb"
#define UNITTEST_STAGE_GAMEDB_NAME		"gamedb"
#define UNITTEST_STAGE_LOGDB_NAME		"logdb"


#define TEST2(Name)							TEST_EX(Name, _TS2)
#define TEST_FIXTURE2(Fixture, Name)		TEST_FIXTURE_EX(Fixture, Name, _TS2)
	
#define TEST3(Name)							TEST_EX(Name, _TS3)
#define TEST_FIXTURE3(Fixture, Name)		TEST_FIXTURE_EX(Fixture, Name, _TS3)
	
#define TEST4(Name)							TEST_EX(Name, _TS4)
#define TEST_FIXTURE4(Fixture, Name)		TEST_FIXTURE_EX(Fixture, Name, _TS4)

#define ACCDB_TEST(Name)					TEST_EX(Name, _TS_ACCDB)
#define ACCDB_TEST_FIXTURE(Fixture, Name)	TEST_FIXTURE_EX(Fixture, Name, _TS_ACCDB)

#define GAMEDB_TEST(Name)					TEST_EX(Name, _TS_GAMEDB)
#define GAMEDB_TEST_FIXTURE(Fixture, Name)	TEST_FIXTURE_EX(Fixture, Name, _TS_GAMEDB)

#define LOGDB_TEST(Name)					TEST_EX(Name, _TS_LOGDB)
#define LOGDB_TEST_FIXTURE(Fixture, Name)	TEST_FIXTURE_EX(Fixture, Name, _TS_LOGDB)