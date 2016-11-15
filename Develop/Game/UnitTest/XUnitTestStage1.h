#pragma once

#include "MBaseUnitTest.h"
#include "XGlobalInstance.h"

class MockGameApp;
class MockTalentInfoMgr;
class MockNPCInfoMgr;
class MockItemInfoMgr;
class MockMeshInfoMgr;
class MockBuffInfoMgr;
class MockFactionInfoMgr;
class MockQuestInfoMgr;

class XTestGlobalInstance : public XGlobalInstance
{
public:
	MockGameApp*		app;
public:
	XTestGlobalInstance();
	virtual ~XTestGlobalInstance();
	virtual bool Init(HWND nHwnd);
	virtual void Fini();
};

class XTestGlobalInfoInstance : public XGlobalInfoInstance
{
public:
	XTestGlobalInfoInstance();
	virtual ~XTestGlobalInfoInstance();
	virtual bool Init();

	MockTalentInfoMgr*	mock_talnet;
	MockNPCInfoMgr*		mock_npc;
	MockItemInfoMgr*	mock_item;
	MockMeshInfoMgr*	mock_mesh_info;
	MockBuffInfoMgr*	mock_buff_info;
	MockFactionInfoMgr*	mock_faction_info;
	MockQuestInfoMgr*	mock_quest_info;
};

#define UNITTEST_STAGE1_NAME		"Stage1"

class XUnitTestStage1 : public MBaseUnitTest
{
private:
	XTestGlobalInstance			m_GlobalInstance;
	XTestGlobalInfoInstance		m_GlobalInfoInstance;

	void InitGlobal();

public:
	XUnitTestStage1() {}
	virtual ~XUnitTestStage1() {}
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();
	virtual void OnTestStart();
	virtual void OnTestFinish() {}

	virtual const char* GetName() { return UNITTEST_STAGE1_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return UnitTest::Test::GetTestList(); }
	virtual const char* GetOutputFileName() { return "UnitTestStage1_Results.xml"; }
};
