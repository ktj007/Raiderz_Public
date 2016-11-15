#include "stdafx.h"
#include "GUnitTestStage1.h"
#include "MUnitTestReporter.h"
#include "MockField.h"
#include "MockDBManager.h"
#include "MockPathFinder.h"
#include <string>
#include "MMemoryLeak.h"


void GUnitTestStage1::GTestServerSystems::Create()
{
	GServerSystems::Create();

	if (pServer) delete pServer;
	pServer = new MockServer();
	gsys.pServer = pServer;

	if (pDBManager) delete pDBManager;
	pDBManager = new MockDBManager();
	gsys.pDBManager = pDBManager;

	if (pPathFinder) delete pPathFinder;
	pPathFinder = new MockPathFinder();
	gsys.pPathFinder = pPathFinder;

}

void GUnitTestStage1::GTestServerSystems::Destroy()
{
	GServerSystems::Destroy();
}

///////////////////////////////////////////////////////////////
void GUnitTestStage1::Create(MUnitTestConfig& config)
{
	m_TestSystems.Create();
	m_TestManagers.Create();

	if (gsys.pServer->Create(L"TestStage1", false) == false)
	{
		mlog("MockServer Create Failed\n");
	}

	GConst::AI_OPTIMIZATION = false;
	GConst::INVINCIBLE_TIME_ENTER_FIELD = 0.0f;
}

void GUnitTestStage1::Destroy()
{
	DefaultMockField::Free();
	gsys.pServer->Destroy();

	m_TestManagers.Destroy();
	m_TestSystems.Destroy();
}

void GUnitTestStage1::OnTestFinish()
{
#if defined(_DEBUG)
	assert(MMemoryLeak::CheckMemory());
#endif
}