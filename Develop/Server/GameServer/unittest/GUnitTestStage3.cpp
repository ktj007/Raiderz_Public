#include "stdafx.h"
#include "GUnitTestStage3.h"
#include "MockField.h"
#include "GServer.h"
#include "GConst.h"

GUnitTestStage3::GUnitTestStage3()
{

}

GUnitTestStage3::~GUnitTestStage3()
{

}

void GUnitTestStage3::Create(MUnitTestConfig& config)
{
	m_TestSystems.Create();
	m_TestManagers.Create();

	if (gsys.pServer->Create(L"TestStage3") == false)
	{
		mlog("UnitTestStage3: Server Create Failed\n");
	}
}

void GUnitTestStage3::Destroy()
{
	DefaultMockField::Free();

	gsys.pServer->Destroy();

	m_TestManagers.Destroy();
	m_TestSystems.Destroy();
}
