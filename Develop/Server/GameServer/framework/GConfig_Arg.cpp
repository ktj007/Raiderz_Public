#include "StdAfx.h"
#include "GConfig_Arg.h"
#include "MCommandLine.h"

#define CONFIG_ARG_SPACE			L" "

#define CONFIG_ARG_AUTO_TEST_TYPE	L"-att"
#define CONFIG_ARG_AUTO_TEST_FRAME	L"-frame"

#define CONFIG_ARG_AUTO_TEST_REPEAT	L"-repeat"
#define CONFIG_ARG_AUTO_TEST_TIME	L"-time"
#define CONFIG_ARG_AUTO_TEST_SPEED	L"-speed"
#define CONFIG_ARG_AUTO_TEST_NPC_COUNT	L"-npc"
#define CONFIG_ARG_AUTOTEST_FIELD_ID	L"-field"
#define CONFIG_ARG_AUTOTEST_QPVPEVENT_ID	L"-qpvpeventid"


#define CONFIG_ARG_AI_TEST_TYPE			L"-ait"
#define CONFIG_ARG_COMMAND_RECORDING_TYPE	L"-cmdrec"


#define CONFIG_ARG_AUTO_START_COMMAND_PROFILE	L"-cmdprofile"
#define CONFIG_ARG_SERVER_RUN_SECOND			L"-runtime"


GConfig_Arg::GConfig_Arg()
: bAutoTestActive(false)
, nAITestType(AITEST_NONE)
, nCommandRecordingType(COMMANDREC_NONE)
, fLimitFrame(0.0f)
, nRepeat(0)
, dwTime(0)
, nSpeed(0)
, nNPCCount(0)
, nFieldID(0)
, bAutoStartCommandProfile(false)
, bLimitRunTime(false)
, dwRunTime(0)
, nQPVPEventID(0)
{
	
}

void GConfig_Arg::Create(const wchar_t* szCmd)
{
	if (NULL == szCmd) return;

	wchar_t szTemp[512];

	swprintf_s(szTemp, 512, szCmd);
	wchar_t* szAutoTestType = wcsstr(szTemp, CONFIG_ARG_AUTO_TEST_TYPE);
	if (NULL != szAutoTestType)
	{
		wchar_t* nextToken;		
		szAutoTestType = wcstok_s(szAutoTestType, CONFIG_ARG_SPACE, &nextToken);
		szAutoTestType = wcstok_s(NULL, CONFIG_ARG_SPACE, &nextToken);

		strAutoTestType = szAutoTestType;

		bAutoTestActive = true;
	}

	swprintf_s(szTemp, 512, szCmd);
	wchar_t* szFrame = wcsstr(szTemp, CONFIG_ARG_AUTO_TEST_FRAME);
	if (NULL != szFrame)
	{
		wchar_t* nextToken;
		szFrame = wcstok_s(szFrame, CONFIG_ARG_SPACE, &nextToken);
		szFrame = wcstok_s(NULL, CONFIG_ARG_SPACE, &nextToken);
		fLimitFrame = (float) _wtof(szFrame);
	}

	swprintf_s(szTemp, 512, szCmd);
	wchar_t* szRepeat = wcsstr(szTemp, CONFIG_ARG_AUTO_TEST_REPEAT);
	if (NULL != szRepeat)
	{
		wchar_t* nextToken;
		szRepeat = wcstok_s(szRepeat, CONFIG_ARG_SPACE, &nextToken);
		szRepeat = wcstok_s(NULL, CONFIG_ARG_SPACE, &nextToken);
		nRepeat = _wtoi(szRepeat);
	}

	swprintf_s(szTemp, 512, szCmd);
	wchar_t* szTime = wcsstr(szTemp, CONFIG_ARG_AUTO_TEST_TIME);
	if (NULL != szTime)
	{
		wchar_t* nextToken;
		szTime = wcstok_s(szTime, CONFIG_ARG_SPACE, &nextToken);
		szTime = wcstok_s(NULL, CONFIG_ARG_SPACE, &nextToken);
		dwTime = _wtoi(szTime);
	}

	swprintf_s(szTemp, 512, szCmd);
	wchar_t* szSpeed = wcsstr(szTemp, CONFIG_ARG_AUTO_TEST_SPEED);
	if (NULL != szSpeed)
	{
		wchar_t* nextToken;
		szSpeed = wcstok_s(szSpeed, CONFIG_ARG_SPACE, &nextToken);
		szSpeed = wcstok_s(NULL, CONFIG_ARG_SPACE, &nextToken);
		nSpeed = _wtoi(szSpeed);
	}

	swprintf_s(szTemp, 512, szCmd);
	wchar_t* szAITestType = wcsstr(szTemp, CONFIG_ARG_AI_TEST_TYPE);
	if (NULL != szAITestType)
	{
		wchar_t* nextToken;		
		szAITestType = wcstok_s(szAITestType, CONFIG_ARG_SPACE, &nextToken);
		szAITestType = wcstok_s(NULL, CONFIG_ARG_SPACE, &nextToken);

		nAITestType = (GAITestType) _wtoi(szAITestType);
	}

	swprintf_s(szTemp, 512, szCmd);
	wchar_t* szRecordingType = wcsstr(szTemp, CONFIG_ARG_COMMAND_RECORDING_TYPE);
	if (NULL != szRecordingType)
	{
		wchar_t* nextToken;		
		szRecordingType = wcstok_s(szRecordingType, CONFIG_ARG_SPACE, &nextToken);
		szRecordingType = wcstok_s(NULL, CONFIG_ARG_SPACE, &nextToken);

		nCommandRecordingType = (GCommandRecordingType) _wtoi(szRecordingType);
	}

	swprintf_s(szTemp, 512, szCmd);
	wchar_t* szNPC = wcsstr(szTemp, CONFIG_ARG_AUTO_TEST_NPC_COUNT);
	if (NULL != szNPC)
	{
		wchar_t* nextToken;
		szNPC = wcstok_s(szNPC, CONFIG_ARG_SPACE, &nextToken);
		szNPC = wcstok_s(NULL, CONFIG_ARG_SPACE, &nextToken);
		nNPCCount = _wtoi(szNPC);
	}

	swprintf_s(szTemp, 512, szCmd);
	wchar_t* szFieldID = wcsstr(szTemp, CONFIG_ARG_AUTOTEST_FIELD_ID);
	if (NULL != szFieldID)
	{
		wchar_t* nextToken;
		szFieldID = wcstok_s(szFieldID, CONFIG_ARG_SPACE, &nextToken);
		szFieldID = wcstok_s(NULL, CONFIG_ARG_SPACE, &nextToken);
		nFieldID = _wtoi(szFieldID);
	}

	swprintf_s(szTemp, 512, szCmd);
	wchar_t* szQPVPEventID = wcsstr(szTemp, CONFIG_ARG_AUTOTEST_QPVPEVENT_ID);
	if (NULL != szNPC)
	{
		wchar_t* nextToken;
		szQPVPEventID = wcstok_s(szFieldID, CONFIG_ARG_SPACE, &nextToken);
		szQPVPEventID = wcstok_s(NULL, CONFIG_ARG_SPACE, &nextToken);
		nQPVPEventID = _wtoi(szQPVPEventID);
	}


	cml2::MCommandLine cl;
	cl.ParseFromCommandLine();

	if (cl.HasSwitch("cmdprofile"))
	{
		bAutoStartCommandProfile = true;
	}

	dwRunTime = cl.GetSwitchValueInt("runtime");

	if (0 < dwRunTime)
	{
		bLimitRunTime = true;
	}
}
