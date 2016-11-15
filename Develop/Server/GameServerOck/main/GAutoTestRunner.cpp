#include "StdAfx.h"
#include "GAutoTestRunner.h"
#include "GConfig.h"
#include "GGlobal.h"
#include "GServer.h"
#include "GAIProfilingTest.h"
#include "GAIProfiler.h"
#include "GAITest.h"
#include "GQPVPTest.h"

GAutoTestRunner::GAutoTestRunner(void)
{
}

GAutoTestRunner::~GAutoTestRunner(void)
{
}

MAutoTest* GAutoTestRunner::NewTest()
{
	if (CONFIG_AUTO_TEST_AI == GConfig::m_strAutoTestType)
	{
		return new GAITest();
	}
	else if (CONFIG_AUTO_TEST_AI_PROFILE == GConfig::m_strAutoTestType)
	{
		return new GAIProfilingTest();
	}
	else if (CONFIG_AUTO_TEST_QPVPEVENT == GConfig::m_strAutoTestType)
	{
		return new GQPVPTest();
	}
	else
	{
		_ASSERT(0);
	}

	return NULL;
}

void GAutoTestRunner::OnCreate()
{
	m_pTest->GetInfo().strTestType = MLocale::ConvUTF16ToAnsi(GConfig::m_strAutoTestType.c_str());
	m_pTest->GetInfo().nRepeat = GConfig::m_nRepeat;
	m_pTest->GetInfo().dwTime = GConfig::m_dwTime;
	m_pTest->GetInfo().nSpeed = GConfig::m_nSpeed;
	m_pTest->GetInfo().fLimitFrame = GConfig::m_fLimitFrame;
}

bool GAutoTestRunner::OnUpdate(float fDelta)
{
	return gsys.pServer->Update(fDelta);
}

