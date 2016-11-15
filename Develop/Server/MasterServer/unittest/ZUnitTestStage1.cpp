#include "stdafx.h"
#include "ZUnitTestStage1.h"
#include "MUnitTestReporter.h"
#include <string>
#include "MMemoryLeak.h"
#include "MockServer.h"
#include "MockDBManager.h"

void ZUnitTestStage1::ZTestServerSystems::Create()
{
	ZServerSystems::Create();

	if (pServer) delete pServer;
	pServer = new MockServer();
	gsys.pServer = pServer;

	if (pDBManager) delete pDBManager;
	pDBManager = new MockDBManager();
	gsys.pDBManager = pDBManager;
}

void ZUnitTestStage1::ZTestServerSystems::Destroy()
{
	ZServerSystems::Destroy();
}

///////////////////////////////////////////////////////////////
void ZUnitTestStage1::Create(MUnitTestConfig& config)
{
	m_TestSystems.Create();
	m_TestManagers.Create();

	if (gsys.pServer->Create() == false)
	{
		mlog("MockServer Create Failed\n");
	}
}

void ZUnitTestStage1::Destroy()
{
	gsys.pServer->Destroy();

	m_TestManagers.Destroy();
	m_TestSystems.Destroy();
}

void ZUnitTestStage1::OnTestFinish()
{
#if defined(_DEBUG)
	assert(MMemoryLeak::CheckMemory());
#endif
}