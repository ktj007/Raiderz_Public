#include "stdafx.h"
#include "PUnitTestStage1.h"
#include "MUnitTestReporter.h"
#include <string>
#include "MMemoryLeak.h"
#include "MockServer.h"

void PUnitTestStage1::ZTestServerSystems::Create()
{
	PServerSystems::Create();

	if (pServer) delete pServer;
	pServer = new MockServer();
	gsys.pServer = pServer;

	//if (pDBManager) delete pDBManager;
	//pDBManager = new MockDBManager();
	//gsys.pDBManager = pDBManager;
}

void PUnitTestStage1::ZTestServerSystems::Destroy()
{
	PServerSystems::Destroy();
}

///////////////////////////////////////////////////////////////
void PUnitTestStage1::Create(MUnitTestConfig& config)
{
	m_TestSystems.Create();
	m_TestManagers.Create();

	if (gsys.pServer->Create() == false)
	{
		mlog("MockServer Create Failed\n");
	}
}

void PUnitTestStage1::Destroy()
{
	gsys.pServer->Destroy();

	m_TestManagers.Destroy();
	m_TestSystems.Destroy();
}

void PUnitTestStage1::OnTestFinish()
{
#if defined(_DEBUG)
	assert(MMemoryLeak::CheckMemory());
#endif
}