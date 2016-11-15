#include "stdafx.h"
#include "ServerValidator.h"
#include "GGlobal.h"
#include "GValidateLogger.h"
#include "GItemManager.h"
#include "GNPCInfoMgr.h"
#include "GTalentInfo.h"
#include "GTalentInfoMgr.h"
#include "GQuestInfoMgr.h"
#include "GSoulBindingInfoMgr.h"
#include "GConditionsInfoMgr.h"
#include "GItemConditionsInfoMgr.h"
#include "GDialogInfoMgr.h"
#include "GLootInfoMgr.h"
#include "GNPCShopInfoMgr.h"
#include "GValidatorSilo.h"
#include "CSBaseValidator.h"
#include "GStrings.h"
#include "GScriptManager.h"
#include "GBuffInfo.h"
#include "GInnRoomInfoMgr.h"
#include "GPathFinder.h"
#include "GFieldMgr.h"
#include "GServer.h"
#include "MMaietXmlTestReporter.h"
#include "MMaietTestResult.h"
#include "AddConsoleReporter.h"
#include "GServerResLoader.h"
#include "GFieldInfoMgr.h"
#include "GRecipeInfoMgr.h"
#include "GCraftInfoMgr.h"
#include "GFactionInfoMgr.h"
#include "GConst.h"
#include "GFactionRelationInfoMgr.h"
#include "GCutsceneInfoMgr.h"
#include "GEmblemInfoMgr.h"
#include "GChallengerQuestMgr.h"
#include "GAIRoomMgr.h"
#include "GExportDBStringTable.h"

ServerValidator::ServerValidator()
: m_ofStream("../ResourceValidator/ServerResourceValidatorResult.xml")
{
	m_reporter = new AddConsoleReporter(m_ofStream);
}

ServerValidator::~ServerValidator()
{
	delete m_reporter;
}

bool ServerValidator::Create(void)
{
	_InitializeDependence();
	_InitalizeConst();

	if (!_InitializeIo())
	{
		return false;
	}

	return _LoadResource();
}

ServerValidator::ReturnValue ServerValidator::Check( void )
{
	if (Create() == false)
	{
		return VALIDATOR_INITIALIZE_FAILED;
	}

	GValidatorSilo silo;
	CSBaseValidator* pValidator = silo.GetAllCheckValidator();

	bool bResult = pValidator->Check();
	m_reporter->Report();
	
	Destroy();

	if (bResult == false)
	{
		return VALIDATOR_FAILED;
	}

	return VALIDATOR_SUCCESS;
}

void ServerValidator::Destroy(void)
{
	_FinalizeIo();
	_FinalizeDependence();
}

bool ServerValidator::Log(const char *pFormat, ...)
{
	if (m_reporter == NULL)
	{
		return false;
	}

	char temp[8192];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	m_reporter->OnLog(temp);
	
	return true;
}

void ServerValidator::_InitalizeConst( void )
{
	GConst::RUN_FOR_TEST = true;
	GConst::INCLUDE_DEV_FIELD_LIST = true;
}

void ServerValidator::_InitializeDependence(void)
{
	// 시스템
	gsys.pValidateLogger = new GValidateLogger;
	gsys.pScriptManager = new GScriptManager;
	gsys.pPathFinder = new GPathFinder;
	gsys.pServer = new GServer;

	// 매니저
	gmgr.pFieldMgr = new GFieldMgr;
	gmgr.pNPCInfoMgr = new GNPCInfoMgr;
	gmgr.pTalentInfoMgr = new GTalentInfoMgr;	
	gmgr.pCondtionsInfoMgr = new GConditionsInfoMgr;
	gmgr.pLootInfoMgr = new GLootInfoMgr;
	gmgr.pQuestInfoMgr = new GQuestInfoMgr;
	gmgr.pDialogInfoMgr = new GDialogInfoMgr;
	gmgr.pNPCShopInfoMgr = new GNPCShopInfoMgr;
	gmgr.pSoulBindingInfoMgr = new GSoulBindingInfoMgr;
	gmgr.pItemManager = new GItemManager;
	gmgr.pItemConditionsInfoMgr = new GItemConditionsInfoMgr;	
	gmgr.pFieldInfoMgr = new GFieldInfoMgr;	
	gmgr.pBuffInfoMgr = new GBuffInfoMgr;
	gmgr.pInnRoomInfoMgr = new GInnRoomInfoMgr;
	gmgr.pRecipeInfoMgr = new GRecipeInfoMgr;
	gmgr.pCraftInfoMgr = new GCraftInfoMgr;
	gmgr.pFactionInfoMgr = new GFactionInfoMgr;
	gmgr.pFactionRelationInfoMgr = new GFactionRelationInfoMgr;
	gmgr.pCutsceneInfoMgr = new GCutsceneInfoMgr;
	gmgr.pEmblemInfoMgr = new GEmblemInfoMgr;
	gmgr.pChallengerQuestMgr = new GChallengerQuestMgr;
	gmgr.pAIRoomMgr	= new GAIRoomMgr();
	gmgr.pExportDBStringTable = new GExportDBStringTable();
}

void ServerValidator::_FinalizeDependence()
{
	// 매니저
	SAFE_DELETE(gmgr.pExportDBStringTable);
	SAFE_DELETE(gmgr.pNPCInfoMgr);
	SAFE_DELETE(gmgr.pTalentInfoMgr);
	SAFE_DELETE(gmgr.pCondtionsInfoMgr);
	SAFE_DELETE(gmgr.pLootInfoMgr);
	SAFE_DELETE(gmgr.pQuestInfoMgr);
	SAFE_DELETE(gmgr.pDialogInfoMgr);
	SAFE_DELETE(gmgr.pNPCShopInfoMgr);
	SAFE_DELETE(gmgr.pSoulBindingInfoMgr);
	SAFE_DELETE(gmgr.pItemManager);
	SAFE_DELETE(gmgr.pItemConditionsInfoMgr);
	SAFE_DELETE(gmgr.pFieldInfoMgr);
	SAFE_DELETE(gmgr.pBuffInfoMgr);
	SAFE_DELETE(gmgr.pInnRoomInfoMgr);
	SAFE_DELETE(gmgr.pFieldMgr);
	SAFE_DELETE(gmgr.pRecipeInfoMgr);
	SAFE_DELETE(gmgr.pCraftInfoMgr);
	SAFE_DELETE(gmgr.pFactionInfoMgr);
	SAFE_DELETE(gmgr.pFactionRelationInfoMgr);
	SAFE_DELETE(gmgr.pCutsceneInfoMgr);
	SAFE_DELETE(gmgr.pEmblemInfoMgr);
	SAFE_DELETE(gmgr.pChallengerQuestMgr);
	SAFE_DELETE(gmgr.pAIRoomMgr);

	// 시스템
	SAFE_DELETE(gsys.pValidateLogger);
	SAFE_DELETE(gsys.pScriptManager);
	SAFE_DELETE(gsys.pPathFinder);
	SAFE_DELETE(gsys.pServer);
}

bool ServerValidator::_InitializeIo(void)
{
	// 스크립트 문자열 초기화
	gsys.pValidateLogger->SetListener(m_reporter);
	gsys.pScriptManager->Init();	


	// 파일 읽기
	if (GStrings::Init() == false)
	{
		Log("Failed InitStrings\n");
		return false;
	}

	return true;
}

void ServerValidator::_FinalizeIo(void)
{
	gsys.pScriptManager->Fini();
	gsys.pValidateLogger->SetListener(NULL);
}

bool ServerValidator::_LoadResource(void)
{
	wstring strRootPath = L"../GameServer";

	struct ConsolLog: public GServerResLoaderLogListener { 	 
		virtual void OnLog(const wchar_t* filename)	{ wcout << L"> " << filename << endl;	}
	} consolLog;


	GServerResLoader resLoader;	
	resLoader.SetListener(&consolLog);
	GServerResLoader::Result result = resLoader.LoadAll(&gmgr, strRootPath.c_str(), false);

	if (result.bSuccess == false)
	{
		Log("%s", result.strErrorLog.c_str());
		return false;
	}

	return true;
}