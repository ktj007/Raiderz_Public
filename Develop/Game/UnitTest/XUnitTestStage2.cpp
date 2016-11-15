#include "stdafx.h"
#include "XUnitTestStage2.h"
#include <Shlwapi.h>
#include "RUnitTestRunner.h"
#include "MockScript.h"
#include "MockGameApp.h"
#include "MockNetClient.h"
#include "MockTalentInfo.h"
#include "MockItemInfo.h"
#include "MockNPCInfo.h"
#include "MockMeshInfo.h"
#include "XSystem.h"
#include "XMotion.h"
#include "XStrings.h"
#include "XRecipeInfoMgr.h"
#include "CSMeshInfo.h"
#include "CSMeshInfoLoader.h"
#include "XTalentEvent.h"
#include "XFieldinfo.h"
#include "XTalentEffectMgr.h"
#include "XCharacter.h"
#include "XNetwork.h"
#include "XMotionMgr.h"
#include "XPath.h"


XTestGlobalInstance2::XTestGlobalInstance2() : XGlobalInstance()
{
	app = new MockGameApp();

	if (script) delete script;
	script = new MockScript();

	if (net) delete net;

	net = new MockNetwork();
	net->Create();
}

XTestGlobalInstance2::~XTestGlobalInstance2()
{
	SAFE_DELETE(app);
}

bool XTestGlobalInstance2::Init(HWND nHwnd)
{
	script->Init();
	if ( !script->Create() )
	{
		return false;
	}

	return true;
}

void XTestGlobalInstance2::Fini()
{
	script->Destroy();
}

XTestGlobalInfoInstance2::XTestGlobalInfoInstance2() : XGlobalInfoInstance()
{
	mock_talnet = new MockTalentInfoMgr();
	mock_npc = new MockNPCInfoMgr();
	mock_item = new MockItemInfoMgr();
	mock_mesh_info = new MockMeshInfoMgr();
}

XTestGlobalInfoInstance2::~XTestGlobalInfoInstance2()
{
	SAFE_DELETE( mock_item );
	SAFE_DELETE( mock_npc );
	SAFE_DELETE( mock_talnet );
	SAFE_DELETE( mock_mesh_info );
}

bool XTestGlobalInfoInstance2::Init()
{
	// 메쉬 정보 로드
	CSMeshInfoLoader meshInfoLoader;
	meshInfoLoader.LoadMeshInfo(mock_mesh_info, FILENAME_MESH_INFO);

	// npc
	npc->Load(FILENAME_NPC, FILENAME_NPC_CHANGEMTRL, mock_mesh_info);	

	// Field
	field->Load(FILENAME_ZONELIST, FILENAME_FIELDLIST);

	// quest 
	quest->Load(FILENAME_QUEST);

	// Talent
	talent->Load(FILENAME_TALENT, FILENAME_TALENT_EXT, FILENAME_TALENTHITINFO);

	// Talent Effect
	talent_effect->Load(FILENAME_EFFECTINFO);

	// Motion
	motion->Init();

	// recipe
	recipe->Load(FILENAME_RECIPE);

	// Buff
	buff->Load(FILENAME_BUFFINFO, FILENAME_BUFF_EFFECTINFO, FILENAME_ENCHANT_BUFF_EFFECTINFO);

	return true;
}


/////////////////////////////////////////////////////////////////////
void XUnitTestStage2::InitGlobal()
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
	global.history = m_GlobalInstance.history;
	global.camera = m_GlobalInstance.camera;
	global.emgr = m_GlobalInstance.emgr;

	// info

	info.item = m_GlobalInfoInstance.mock_item;
	info.npc = m_GlobalInfoInstance.mock_npc;
	info.field = m_GlobalInfoInstance.field;
	info.quest = m_GlobalInfoInstance.quest;
	info.talent = m_GlobalInfoInstance.mock_talnet;
	info.talent_effect = m_GlobalInfoInstance.talent_effect;
	info.recipe = m_GlobalInfoInstance.recipe;
	info.mesh_info = m_GlobalInfoInstance.mock_mesh_info;
	info.motion = m_GlobalInfoInstance.motion;
	info.buff = m_GlobalInfoInstance.buff;
}

void XUnitTestStage2::Create(MUnitTestConfig& config)
{
	XStrings::Init();

	InitPath();
	InitConfig();

	InitEngine();

	// Game Global Instance 초기화
	InitGlobal();

	LoadModels();

	global.system->SetResourceLoading(false);
}

void XUnitTestStage2::Destroy()
{
	ReleaseModels();

	global.ui->ShowMouse(true);

	m_GlobalInstance.Fini();

	global.SetNULL();
	gg.SetNull();
	info.SetNull();

	// 엔진 종료
	// 엔진 종료
#ifdef _UNITTEST
	RUnitTestRunner::GetInstance().HaltEngine(false);
#else
	RUnitTestRunner::GetInstance().HaltEngine(true);
#endif
}

void XUnitTestStage2::InitPath()
{
	USES_CONVERSION_EX;

	// Current Directory를 맞춘다.
	wchar_t szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);


	// Path ------------
	// Video
	rs3::REngine::GetConfig().m_bFullScreen = XGETCFG_VIDEO_FULLSCREEN;
	rs3::REngine::GetConfig().m_nWidth = XGETCFG_VIDEO_WIDTH;
	rs3::REngine::GetConfig().m_nHeight = XGETCFG_VIDEO_HEIGHT;
	rs3::REngine::GetConfig().m_nTextureReduction = XGETCFG_VIDEO_TEXTUREREDUCTION;

	rs3::REngine::GetConfig().depthFormat = (XGETCFG_VIDEO_DEPTHBITS == 24) ? RFMT_D24X8 : RFMT_D16;
	rs3::REngine::GetConfig().m_fGlobalTextureMipmapLodBias = XGETCFG_VIDEO_TEXTUREMIPMAPBIAS;

	// path
	if( rs3::REngine::IsDevelopVersion() )
		rs3::REngine::GetConfig().SetDataPath(W2A_EX(PATH_SHADER_DEV, 0));
	else
		rs3::REngine::GetConfig().SetDataPath(W2A_EX(PATH_SHADER, 0));

	rs3::REngine::GetConfig().SetDataPath(W2A_EX(PATH_TREE, 0));
	rs3::REngine::GetConfig().SetDataPath(W2A_EX(PATH_MODELS, 0));
	rs3::REngine::GetConfig().SetDataPath(W2A_EX(XCONST::PATH_TEXTURE, 0));
	rs3::REngine::GetConfig().SetDataPath(W2A_EX(PATH_SFX, 0));
}

void XUnitTestStage2::InitConfig()
{
	USES_CONVERSION_EX;

	XStrings::Init();

	// 설정파일을 읽는다.
	XGetConfig()->LoadXmlFile( XPath::GetConfigFileFullPath().c_str());

	// TODO: 읽어온 설정파일로 어플리케이션에 적용해야 한다.
	CLangNode node;
	node.strName = L"한국어";
	node.strCountry = L"대한민국";
	node.strLocale = L"ko_KR";
	node.strPath = PATH_LANG_KOREAN;
	
	XStringResMgr::GetInstance()->Init(node);


	// Talent Event Handler
	XTalentEvent::InitTalentEventHandler();
}

void XUnitTestStage2::LoadModels()
{
	USES_CONVERSION_EX;

	LoadModel(W2A_EX(XCONST::PlayerModel.Info[SEX_MALE].MODEL_NAME.c_str(), 0));
	LoadModel("data/model/Monster/goblin_mage/goblin_mage.elu");
}

void XUnitTestStage2::ReleaseModels()
{
	for (size_t i = 0; i < m_Actors.size(); ++i)
	{
		m_Actors[i]->Destroy();
		delete m_Actors[i];
	}
	m_Actors.clear();
}

void XUnitTestStage2::LoadModel( const char* szModelName )
{
	USES_CONVERSION_EX;

	XCharacter* pNewActor = new XCharacter();
	pNewActor->Create(A2W_EX(szModelName, 0), false);
	m_Actors.push_back(pNewActor);
}

void XUnitTestStage2::InitEngine()
{
	HWND nHwnd = GetDesktopWindow();
	rs3::RUnitTestRunner::GetInstance().InitEngine(nHwnd, false, false);
}