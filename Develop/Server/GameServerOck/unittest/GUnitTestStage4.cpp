#include "stdafx.h"
#include "GUnitTestStage4.h"
#include "MockField.h"
#include "GServer.h"
#include "NetworkMockServer.h"
#include "MockDBManager.h"
#include "GConst.h"

void GUnitTestStage4::GTestServerSystems::Create()
{
	GServerSystems::Create();

	if (pServer) delete pServer;
	pServer = new NetworkMockServer();
	gsys.pServer = pServer;

	if (pDBManager) delete pDBManager;
	pDBManager = new MockDBManager();
	gsys.pDBManager = pDBManager;
}

void GUnitTestStage4::GTestServerSystems::Destroy()
{
	GServerSystems::Destroy();
}

///////////////////////////////////////////////////////////////

GUnitTestStage4::GUnitTestStage4()
{

}

GUnitTestStage4::~GUnitTestStage4()
{

}

void GUnitTestStage4::Create(MUnitTestConfig& config)
{
	m_TestSystems.Create();
	m_TestManagers.Create();

	if (gsys.pServer->Create(L"TestStage4") == false)
	{
		mlog("UnitTestStage4: Server Create Failed\n");
	}
}

void GUnitTestStage4::Destroy()
{
	DefaultMockField::Free();

	gsys.pServer->Destroy();

	m_TestManagers.Destroy();
	m_TestSystems.Destroy();
}
