#include "stdafx.h"
#include "XGameInitializer.h"
#include "XStrings.h"
#include "XApplicationConfig.h"
#include "XTalentEvent.h"
#include "XGameInterface.h"
#include "XGlobalInstance.h"
#include "XPath.h"
#include "CAppHelper.h"
#include "XBaseApplication.h"
#include "XNetDebuger.h"
#include "XMotionLogic.h"
#include "XResourceManager.h"
#include "RAnimationResource.h"
#include "XFeedbackSystem.h"
#include "XEngine.h"
#include "XAlwaysResourceGrabber.h"
#include "XSoundResMgr.h"
#include "CSStringFilter.h"
#include "CLangInfo.h"
#include "XConst.h"


XGameInitializer::XGameInitializer() 
: m_bInited_Pre_CreatingWindow(false)
, m_bInited_Post_CreatingWindow(false)
, m_bInited_While_LoadingLoginCampaign(false)
, m_bInited_Pre_LoadingLoginCampaign(false)
{

}


bool XGameInitializer::Init_Pre_CreatingWindow(XGlobalInfoInstance* pGlobalInfoInstance, XGlobalInstance* pGlobalInstance)
{
	if (m_bInited_Pre_CreatingWindow) return true;

	srand(timeGetTime());

	InitConfig(pGlobalInfoInstance);

	InitRS3();

#ifndef _PUBLISH
	if (true == XCONST::CONST_MAKE_COMMAND_RESULT)
	{
		XStrings::SaveCommandResultTable();
		XStrings::SaveCommandResultSoundTable();
	}
#endif

	m_bInited_Pre_CreatingWindow = true;

	return true;
}

void XGameInitializer::InitConfig(XGlobalInfoInstance* pGlobalInfoInstance)
{
	if (false == XStrings::Init())
	{
		_ASSERT(0);
		mlog3("XStrings Init fail\n");
	}

	// 게임 설정 정보
	XApplicationConfig::Load(FILENAME_PMANG_CONFIG);

	// 게임 옵션 초기화
	XGetGameInterface().OPTIONS.LoadOption();


	// 언어 설정 읽기
	if (!pGlobalInfoInstance->lang->Load(PATH_LANG FILENAME_LANG))
	{
		mlog("Failed to open language file.\n");
		return;
	}

	// 국가별 언어 텍스트 읽기
	ReadCountryLanguages( pGlobalInfoInstance);

	// 기본 로케일 설정
	if (!XStringResMgr::GetInstance()->SetLocale(pGlobalInfoInstance->lang->GetLocale()))
	{
		mlog("Failed set default locale(%s).\n", MLocale::ConvUTF16ToAnsi(pGlobalInfoInstance->lang->GetLocale()).c_str());
	}


	// Talent Event Handler
	XTalentEvent::InitTalentEventHandler();
}

void XGameInitializer::ReadCountryLanguages(XGlobalInfoInstance* pGlobalInfoInstance)
{
	XStringResMgr::GetInstance()->Clear();


	// 언어 설정에 따른 문자열 읽기
	for each(const CLangNode& node in *pGlobalInfoInstance->lang)
	{
		// 채팅 금직어 읽기
		tstring strAbuseFileName = MLocale::ConvUTF16ToTCHAR(node.strPath.c_str()) + FILENAME_STR_ABUSE;

		if (!GetStringFilter()->LoadFromFile(strAbuseFileName.c_str()))
		{
			mlog3("CSStringFilter LoadFromFile fail (locale=%s)\n", MLocale::ConvUTF16ToAnsi(node.strLocale).c_str());
		}


		// 문자열 읽기
		vector<wstring> vecStrings;
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_INTERFACE));
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_LOCAL));	
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_STRINGS));
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_WORD));
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_SYSTEM_MSG));
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_TIP));
			
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_NAME_ITEM));
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_NAME_NPC));
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_NAME_BUFF));
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_NAME_TALENT));
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_NAME_QUEST));
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_NAME_DIALOG));
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_NAME_FIELD));
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_NAME_AREA));
		vecStrings.push_back(MLocale::ConvTCHARToUTF16(FILENAME_STR_NAME_FACTION));
		

		bool bIgnoreNonTranslationMark = XCONST::IGNORE_NON_TRANSLATION_MARK;
		if (node.strLocale != L"ko_KR") bIgnoreNonTranslationMark = false;

		XStringResMgr::GetInstance()->Init(node, vecStrings, bIgnoreNonTranslationMark);
	}
}

void XGameInitializer::InitRS3()
{
	// Path ------------
	// Video
	int w, h;
	XGetGameInterface().OPTIONS.GetScreenResolution( &w, &h);
	rs3::REngine::GetConfig().m_nWidth = w;
	rs3::REngine::GetConfig().m_nHeight = h;
	rs3::REngine::GetConfig().m_bFullScreen = XGETCFG_VIDEO_FULLSCREEN;
	rs3::REngine::GetConfig().m_nTextureReduction = XGETCFG_VIDEO_TEXTUREREDUCTION;

	rs3::REngine::GetConfig().depthFormat = (XGETCFG_VIDEO_DEPTHBITS == 24) ? RFMT_D24S8 : RFMT_D16;
	rs3::REngine::GetConfig().m_bUsingShader = XGETCFG_VIDEO_SHADER;

	// mipmap bias
	rs3::REngine::GetConfig().m_fGlobalTextureMipmapLodBias = XGETCFG_VIDEO_TEXTUREMIPMAPBIAS;

	// Debug
	rs3::REngine::GetConfig().m_bLogResource = XCONST::RESOURCE_LOADING_LOG;

	if( rs3::REngine::IsDevelopVersion() == false )
	{
		rs3::REngine::GetConfig().m_bUsingCompiledShader = XCONST::USING_COMPILED_SHADER;
		rs3::REngine::GetConfig().m_bUsingCompiledShaderCaching = true;

		if (XPath::CreateDirectoryInMyDocuments(PATH_MYDOC_SHADER_CACHING))
		{
			rs3::REngine::GetConfig().m_strCompiledShaderCachePath = MLocale::ConvUTF16ToAnsi(XPath::GetShaderCachingFileFullPath().c_str());
		}
	}
}


#include "MXml.h"

static vector<RAnimationResource*>	g_AniRes;

static void ReadAniFiles()
{
	vector<wstring> vecFiles;

	const char* xmlFileName = "Data/Model/Player/hm/hm.elu.animation.xml";
	MXml xml;
	bool bOk = xml.LoadFile(xmlFileName);
	if(!bOk)
		return;

	wstring strPath = MLocale::ConvAnsiToUTF16(MGetPurePath(xmlFileName).c_str());

	MXmlElement* pRootNode = xml.Doc()->FirstChildElement();

	MXmlElement* pChild = pRootNode->FirstChildElement();

	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement()) {

		char NodeName[256] = {0,};
		char IDName[256] = {0,};
		char FileName[256] = {0,};
		char MotionLoopTypeID[256] = {0,};
		AnimationLoopType MLoopType = RAniLoopType_Loop;
		float speed =1.0f;

		strcpy(NodeName, pChild->Value());

		if(strcmp(NodeName, "AddAnimation")==0) {

			_Attribute(IDName, pChild, "name");

			_Attribute(FileName, pChild, "filename");

			for(size_t i=0; i<strlen(FileName); i++)
				if(FileName[i]=='\\') FileName[i]='/';

			wstring PathFileName(strPath + MLocale::ConvAnsiToUTF16(FileName));

			vecFiles.push_back(PathFileName);

		}
	}

	for (size_t i = 0; i < vecFiles.size(); i++)
	{
		RAnimationResource* pAniRes = new RAnimationResource(MLocale::ConvUTF16ToAnsi(vecFiles[i].c_str()).c_str(), NULL);
		g_AniRes.push_back(pAniRes);

		// 
		//	pAniRes->Fill();
	}

}


bool XGameInitializer::Init_Post_CreatingWindow(XBaseApplication* pApp, XGlobalInfoInstance* pGlobalInfoInstance, XGlobalInstance* pGlobalInstance)
{
	if (m_bInited_Post_CreatingWindow) return true;

	if (pGlobalInstance->Init(pApp->GetWindowHandle()) == false)
	{
		return false;
	}

	//if (global.feedback)
	//{
	//	global.feedback->Fetch();
	//}

#ifndef _PUBLISH
	if (XCONST::USE_NET_DEBUGGER)
	{
		XNetDebuger::Init();
	}

#endif

	// 게임 옵션 설정
	XGetGameInterface().OPTIONS.UpdateSystemOption();

	//////////////////////////////////////////////////////////////////////////
	// 테스트로 애니메이션 시간 체크


	//MTimeChecker tc;
	//tc.Start();

	//ReadAniFiles();

	//

	//mlog("Load Done : %u , %u ms\n", g_AniRes.size(), tc.Check());
	//for (size_t i = 0; i < g_AniRes.size(); i++)
	//{
	//	delete g_AniRes[i];
	//}
	//g_AniRes.clear();



	m_bInited_Post_CreatingWindow = true;

	return true;
}

bool XGameInitializer::Init_Pre_LoadingLoginCampaign()
{
	if (m_bInited_Pre_LoadingLoginCampaign) return true;


	m_bInited_Pre_LoadingLoginCampaign = true;

	return true;
}

bool XGameInitializer::Init_While_LoadingLoginCampaign()
{
	if (m_bInited_While_LoadingLoginCampaign) return true;

	XGlobalInstance* pGlobalInstance = global.app->GetGlobalInstance();
	XGlobalInfoInstance* pGlobalInfoInstance = global.app->GetGlobalInfoInstance();

	if (!pGlobalInfoInstance->Init())
	{
		if (global.app) global.app->Quit();
	}

	XMotionLogic::BindMotionEvents();

	pGlobalInstance->res->InitCaching(global.smgr, FILENAME_PRELOAD);

	InitResources(pGlobalInstance);

	m_bInited_While_LoadingLoginCampaign = true;
	return true;
}

bool XGameInitializer::InitResources(XGlobalInstance* pGlobalInstance)
{
	pGlobalInstance->res->Init(new XAlwaysResourceGrabber());

	pGlobalInstance->engine->Init(REngine::GetDevicePtr(), pGlobalInstance->res);

	bool bPreloadPlayersMesh = true;
#ifdef _DEBUG
	// 개발의 편의성을 위해 디버그 모드로 돌 경우에는 메쉬 안 읽는다.
	bPreloadPlayersMesh = false;
#endif

	bool bRet = pGlobalInstance->engine->InitSharedResource(bPreloadPlayersMesh);

	if (bRet == false)
	{
		_ASSERT(bRet);
		mlog("Failed To Init Shared Resource\n");
	}

	pGlobalInstance->res->GrabAlwaysResources();

	return false;
}
