#include "stdafx.h"
#include "XUnitTestStage1.h"

#include <Shlwapi.h>
#include "RUnitTestRunner.h"
#include "MockScript.h"
#include "MockGameApp.h"
#include "MockNetClient.h"
#include "MockTalentInfo.h"
#include "MockItemInfo.h"
#include "MockNPCInfo.h"
#include "MockMeshInfo.h"
#include "MockBuffInfoMgr.h"
#include "MockQuestInfo.h"
#include "XSystem.h"
#include "XMotion.h"
#include "XStrings.h"
#include "XTalentEvent.h"
#include "MockEngine.h"
#include "MockSystem.h"
#include "XNetwork.h"
#include "MockFactionInfoMgr.h"
#include "XMotionMgr.h"

XTestGlobalInstance::XTestGlobalInstance() : XGlobalInstance()
{
	app = new MockGameApp();

	if (system) delete system;
	system = new MockSystem();

	if (script) delete script;
	script = new MockScript();

	if (engine) delete engine;
	engine = new MockEngine();

	if (net) delete net;

	net = new MockNetwork();
	net->Create();
}

XTestGlobalInstance::~XTestGlobalInstance()
{
	SAFE_DELETE(app);
}

bool XTestGlobalInstance::Init(HWND nHwnd)
{
	script->Init();
	if ( !script->Create() )
	{
		return false;
	}

	return true;
}

void XTestGlobalInstance::Fini()
{
	script->Destroy();
}

XTestGlobalInfoInstance::XTestGlobalInfoInstance() : XGlobalInfoInstance()
{
	mock_talnet = new MockTalentInfoMgr();
	mock_npc = new MockNPCInfoMgr();
	mock_item = new MockItemInfoMgr();
	mock_mesh_info = new MockMeshInfoMgr();
	mock_buff_info = new MockBuffInfoMgr();
	mock_faction_info = new MockFactionInfoMgr();
	mock_quest_info = new MockQuestInfoMgr();
}

XTestGlobalInfoInstance::~XTestGlobalInfoInstance()
{
	SAFE_DELETE( mock_item );
	SAFE_DELETE( mock_npc );
	SAFE_DELETE( mock_talnet );
	SAFE_DELETE( mock_mesh_info );
	SAFE_DELETE( mock_buff_info );
	SAFE_DELETE( mock_faction_info );
	SAFE_DELETE( mock_quest_info );
}

bool XTestGlobalInfoInstance::Init()
{
	// Motion
	motion->Init();

	return true;
}


/////////////////////////////////////////////////////////////////////
void XUnitTestStage1::InitGlobal()
{
	m_GlobalInstance.Init(0);
	m_GlobalInfoInstance.Init();

	global.app = m_GlobalInstance.app;
	global.device = (rs3::RDeviceD3D*)(REngine::GetDevicePtr());
	global.smgr = REngine::GetSceneManagerPtr();
	global.gfmgr = NULL;

	global.system = m_GlobalInstance.system;
	global.sound = m_GlobalInstance.sound;
	global.script = m_GlobalInstance.script;
	global.ui = m_GlobalInstance.ui;
	global.mint = m_GlobalInstance.ui->GetMint();
	global.net = m_GlobalInstance.net;
	global.engine = m_GlobalInstance.engine;
	global.history = m_GlobalInstance.history;
	global.camera = m_GlobalInstance.camera;
	global.emgr = m_GlobalInstance.emgr;

	// info

	//info.item = m_GlobalInfoInstance.item;
	info.item = m_GlobalInfoInstance.mock_item;
	//info.npc = m_GlobalInfoInstance.npc;
	info.npc = m_GlobalInfoInstance.mock_npc;
	info.field = m_GlobalInfoInstance.field;
	info.quest = m_GlobalInfoInstance.mock_quest_info;
	//info.talent = m_GlobalInfoInstance.talent;
	info.talent = m_GlobalInfoInstance.mock_talnet;
	info.talent_effect = m_GlobalInfoInstance.talent_effect;
	info.recipe = m_GlobalInfoInstance.recipe;
	info.mesh_info = m_GlobalInfoInstance.mock_mesh_info;
	info.buff = m_GlobalInfoInstance.mock_buff_info;
	info.motion = m_GlobalInfoInstance.motion;
	info.buff = m_GlobalInfoInstance.buff;
	info.colorTable = m_GlobalInfoInstance.colorTable;
	info.faction = m_GlobalInfoInstance.mock_faction_info;
}

void XUnitTestStage1::Create(MUnitTestConfig& config)
{
	XStrings::Init();

	// 엔진 초기화
	RUnitTestRunner::GetInstance().InitMockEngine();

	// Game Global Instance 초기화
	InitGlobal();

	global.system->SetResourceLoading(false);

	// Talent Event Handler
	XTalentEvent::InitTalentEventHandler();
}

void XUnitTestStage1::Destroy()
{
	global.ui->ShowMouse(true);

	m_GlobalInstance.Fini();

	global.SetNULL();
	gg.SetNull();
	info.SetNull();

	// 엔진 종료
#ifdef _UNITTEST
	RUnitTestRunner::GetInstance().HaltMockEngine(false);
#else
	RUnitTestRunner::GetInstance().HaltMockEngine(true);
#endif
}

void XUnitTestStage1::OnTestStart()
{
	// gvar는 항상 새로 교체한다.
	delete m_GlobalInstance.var;
	m_GlobalInstance.var = new GlobalVar();
}