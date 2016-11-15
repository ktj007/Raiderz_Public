#include "stdafx.h"
#include "GUnitTestStage2.h"
#include "MUnitTestReporter.h"
#include "MockField.h"
#include "MockDBManager.h"
#include <string>
#include "GNPCInfoMgr.h"
#include "GFieldInfoMgr.h"

void GUnitTestStage2::GTestServerSystems2::Create()
{
	GServerSystems::Create();

	if (pServer) delete pServer;
	pServer = new MockServer();
	gsys.pServer = pServer;

	if (pDBManager) delete pDBManager;
	pDBManager = new MockDBManager();
	gsys.pDBManager = pDBManager;	
}

void GUnitTestStage2::GTestServerSystems2::Destroy()
{
	GServerSystems::Destroy();
}

///////////////////////////////////////////////////////////////
void GUnitTestStage2::Create(MUnitTestConfig& config)
{
	m_TestSystems.Create();
	m_TestManagers.Create();

	if (gsys.pServer->Create(L"TestStage2", false) == false)
	{
		mlog("MockServer Create Failed\n");
	}
	gsys.pScriptManager->Init();

	if(gmgr.pFieldInfoMgr->Load(L"unittest_res/zonelist.xml", 
		L"unittest_res/fieldlist.xml", 
		L"unittest_res/", 
		L"unittest_res/",
		true) == false)
	{
		mlog("Failed Loading Map Info : (%s, %s)\n", "unittest_res/zonelist.xml", "unittest_res/fieldlist.xml");
	}

	// NPC Á¤º¸
	gmgr.pNPCInfoMgr->Load(L"unittest_res/npc.xml");
}

void GUnitTestStage2::Destroy()
{
	DefaultMockField::Free();
	gsys.pServer->Destroy();
	gsys.pScriptManager->Fini();

	m_TestManagers.Destroy();
	m_TestSystems.Destroy();

}