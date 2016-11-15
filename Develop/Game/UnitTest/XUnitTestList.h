#pragma once

#include "UnitTest++.h"

UnitTest::TestList& GetStage2TestList();
UnitTest::TestList& GetStage3TestList();

#define _TS2		GetStage2TestList()
#define _TS3		GetStage3TestList()

#define TEST2(Name)							TEST_EX(Name, _TS2)
#define TEST_FIXTURE2(Fixture, Name)		TEST_FIXTURE_EX(Fixture, Name, _TS2)

#define TEST3(Name)							TEST_EX(Name, _TS3)
#define TEST_FIXTURE3(Fixture, Name)		TEST_FIXTURE_EX(Fixture, Name, _TS3)
