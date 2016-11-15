#pragma once

#include "MBaseUnitTest.h"
#include "XUnitTestRunner.h"
#include "XGlobalInstance.h"

class MockGameApp;
class MockTalentInfoMgr;
class MockNPCInfoMgr;
class MockItemInfoMgr;
class MockMeshInfoMgr;

class XTestGlobalInstance2 : public XGlobalInstance
{
public:
	MockGameApp*		app;

public:
	XTestGlobalInstance2();
	virtual ~XTestGlobalInstance2();
	virtual bool Init(HWND nHwnd);
	virtual void Fini();
};

class XTestGlobalInfoInstance2 : public XGlobalInfoInstance
{
public:
	XTestGlobalInfoInstance2();
	virtual ~XTestGlobalInfoInstance2();
	virtual bool Init();

	MockTalentInfoMgr*	mock_talnet;
	MockNPCInfoMgr*		mock_npc;
	MockItemInfoMgr*	mock_item;
	MockMeshInfoMgr*	mock_mesh_info;
};

class XCharacter;

#define UNITTEST_STAGE2_NAME		"Stage2"

class XUnitTestStage2 : public MBaseUnitTest
{
private:
	XTestGlobalInstance2			m_GlobalInstance;
	XTestGlobalInfoInstance2		m_GlobalInfoInstance;
	vector<XCharacter*>				m_Actors;

	void InitGlobal();
	void InitPath();
	void InitConfig();
	void LoadModels();

	void LoadModel( const char* szModelName );
	void ReleaseModels();
public:
	XUnitTestStage2() {}
	virtual ~XUnitTestStage2() {}
	virtual void Create(MUnitTestConfig& config);

	void InitEngine();
	virtual void Destroy();


	virtual const char* GetName() { return UNITTEST_STAGE2_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return GetStage2TestList(); }
	virtual const char* GetOutputFileName() { return "UnitTestStage2_Results.xml"; }
};
