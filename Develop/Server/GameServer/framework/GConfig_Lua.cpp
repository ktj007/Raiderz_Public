#include "StdAfx.h"
#include "GConfig_Lua.h"
#include "GConst.h"

GConfig_Lua::GConfig_Lua()
: bAutoTestActive(false)
, nAITestType(AITEST_NONE)
, fLimitFrame(0.0f)
, nRepeat(0)
, dwTime(0)
, nSpeed(0)
, nNPCCount(0)
, nFieldID(0)
, nQPVPEventID(0)
{

}

void GConfig_Lua::Create()
{
	bAutoTestActive = GConst::AUTO_TEST_ACTIVE;
	strAutoTestType = GConst::AUTO_TEST_TYPE;
	fLimitFrame = GConst::AUTO_TEST_LIMIT_FRAME;

	nRepeat = GConst::AUTO_TEST_REPEAT;
	dwTime = GConst::AUTO_TEST_TIME;
	nSpeed = GConst::AUTO_TEST_SPEED;

	nAITestType = static_cast<GAITestType>(GConst::AITEST_TYPE);
	nNPCCount = GConst::AUTO_TEST_NPC_COUNT;
	nFieldID = GConst::AUTO_TEST_FIELD_ID;	
	nQPVPEventID = GConst::AUTO_TEST_QPVPEVENT_ID;	
}