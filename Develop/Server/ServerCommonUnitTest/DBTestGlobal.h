#pragma once


#define DB_UNIT_TEST_PATH L"..\\db\\script\\UnitTest\\"
#define DB_SCRIPT_PATH L"..\\db\\script\\create_db_script\\"

#define USED_DB_NAME_COUNT		5
static const wstring USED_DB_NAME[USED_DB_NAME_COUNT] = {L"SoulHuntDB", 
														L"LogDB", 
														L"UserDB", 
														L"SoulHuntStableDB", 
														L"SoulHuntBackupDB"};

#include "UTestDatabase.h"
#include "UTestDatabaseContainer.h"
#include "DBTestHelper.h"