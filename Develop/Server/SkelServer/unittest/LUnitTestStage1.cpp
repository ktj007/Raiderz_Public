#include "stdafx.h"
#include "LUnitTestStage1.h"
#include "MUnitTestReporter.h"
#include <string>
#include "MMemoryLeak.h"
#include "MockServer.h"

void LUnitTestStage1::ZTestServerSystems::Create()
{
	LServerSystems::Create();

	if (pServer) delete pServer;
	pServer = new MockServer();
	gsys.pServer = pServer;

	//if (pDBManager) delete pDBManager;
	//pDBManager = new MockDBManager();
	//gsys.pDBManager = pDBManager;
}

void LUnitTestStage1::ZTestServerSystems::Destroy()
{
	LServerSystems::Destroy();
}

///////////////////////////////////////////////////////////////
void LUnitTestStage1::Create(MUnitTestConfig& config)
{
	m_TestSystems.Create();
	m_TestManagers.Create();

	if (gsys.pServer->Create() == false)
	{
		mlog("MockServer Create Failed\n");
	}
}

void LUnitTestStage1::Destroy()
{
	gsys.pServer->Destroy();

	m_TestManagers.Destroy();
	m_TestSystems.Destroy();
}

void LUnitTestStage1::OnTestFinish()
{
#if defined(_DEBUG)
	assert(MMemoryLeak::CheckMemory());
#endif
}