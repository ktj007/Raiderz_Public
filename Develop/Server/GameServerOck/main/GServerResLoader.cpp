#include "stdafx.h"
#include "GServerResLoader.h"
#include "GDef.h"
#include "GItemManager.h"
#include "GTalentInfoMgr.h"
#include "GBuffInfo.h"
#include "GDialogInfoMgr.h"
#include "GItemConditionsInfoMgr.h"
#include "GConditionsInfoMgr.h"
#include "GNPCShopInfoMgr.h"
#include "GLootInfoMgr.h"
#include "MFileSystem.h"
#include "GFieldInfo.h"
#include "GQuestInfoMgr.h"
#include "GNPCInfoMgr.h"
#include "GSoulBindingInfoMgr.h"
#include "GRecipeInfoMgr.h"
#include "GInnRoomInfoMgr.h"
#include "GCraftInfoMgr.h"
#include "GValidateLogger.h"
#include "GFilePath.h"
#include "GFieldInfoMgr.h"
#include "GFactionInfoMgr.h"
#include "GSQLExporter.h"
#include "GItemInfoSQLMaker.h"
#include "GTalentInfoSQLMaker.h"
#include "GQuestInfoSQLMaker.h"
#include "GRecipeInfoSQLMaker.h"
#include "GItemLearnTalentSQLMaker.h"
#include "GEmblemInfoSQLMaker.h"
#include "GFactionRelationInfoMgr.h"
#include "GCutsceneInfoMgr.h"
#include "GConfig.h"
#include "GEmblemInfoMgr.h"
#include "GChallengerQuestMgr.h"
#include "GConst.h"
#include "GQPEventInfoMgr.h"
#include "GPresetInfoMgr.h"
#include "GBPartRewardParser.h"
#include "GExportDBStringTable.h"

#define FILE_LOG(filename)	{if (m_pLisener) m_pLisener->OnLog(filename);}

GServerResLoader::Result GServerResLoader::LoadAll( GGlobalManager* pGlobalManager, const wchar_t* szRootPath /*= L"./"*/, bool bCheckDebugFile)
{
	m_pGlobalManager = pGlobalManager;
	m_strRootPath = szRootPath;

	Result result;

	wstring strPath;

	// Export DB string table - DB Export가 필요한 정보보다 먼저읽어야한다.
	if (!LoadExportDBStringTable())
	{
		return m_Result;
	}
	
	//// 아이템 정보
	if (!LoadItemInfo())
	{
		return m_Result;
	}

	// 탤런트 정보 - 탤런트 정보는 NPC 정보보다 먼저 읽어야 한다. - birdkr(2007-03-21)
	if (!LoadTalentInfo())
	{
		return m_Result;
	}

	// 버프 정보
	if (!LoadBuffInfo())
	{
		return m_Result;
	}

	// 다이얼로그 정보
	if (!LoadDialogInfo())
	{
		return m_Result;
	}

	// Conditions 정보
	if (!LoadConditionsInfo())
	{
		return m_Result;
	}

	// Conditions_Item 정보
	if (!LoadConditionsItem())
	{
		return m_Result;
	}

	// 트레이드 정보
	if (!LoadNPCShopInfo())
	{
		return m_Result;
	}

	// loot 정보
	if (!LoadLootInfo())
	{
		return m_Result;
	}

	// NPC 정보
	if (!LoadNPCInfo())
	{
		return m_Result;
	}

	// 컷씬 정보
	if (!LoadCutsceneInfo())
	{
		return m_Result;
	}

	// 필드 정보 (환경 초기화보다 먼저 해야함)
	if (!LoadFieldList(bCheckDebugFile))
	{ 
		return m_Result;
	}

	// 퀘스트 정보
	if (!LoadQuestInfo())
	{
		return m_Result;
	}

	// 바인딩 위치 정보
	if (!LoadSoulBinding())
	{
		return m_Result;
	}

	// 레시피 정보
	if (!LoadRecipeInfo())
	{
		return m_Result;
	}


	// 크래프트 정보
	if (!LoadCraftInfo())
	{
		return m_Result;
	}

	// 여관방 정보
	if (!LoadInnRoomInfo())
	{
		return m_Result;
	}


	// 팩션 정보
	if (!LoadFactionInfo())
	{
		return m_Result;
	}

	// 엠블렘 정보
	if (!LoadEmblemInfo())
	{
		return m_Result;
	}


	// 도전자퀘스트 정보
	if (!LoadChallengerQuestInfo())
	{
		return m_Result;
	}

	// QPVP 이벤트 정보
	if (!LoadQPEventInfoMgr())
	{
		return m_Result;
	}	

	// 프리셋 정보
	if (!LoadPresetInfoMgr())
	{
		return m_Result;
	}	

	CookItemInfo();
	m_pGlobalManager->pTalentInfoMgr->Cooking();

	result.bSuccess = true;
	return result;
}

void GServerResLoader::SetListener(GServerResLoaderLogListener* pListener)
{
	m_pLisener = pListener;
}

bool GServerResLoader::Export( GSQLMaker* pSQLMaker, const wstring strExportFile )
{
	if (NULL == pSQLMaker)
		return false;

	GSQLMakerDeleter deleter(pSQLMaker);

	if (IsRunForTest())
	{
		if (!pSQLMaker->BuildSQL())
			return false;

		// ServerResourceValidator App에서는 gsys.pDBManager를 생성하지 않는다.
		if (gsys.pDBManager != NULL)
		{
			GSQLExporter se;
			if (GConfig::m_bExportUpdateDB 
				&& !GConfig::m_bNoDB
				&& pSQLMaker->IsExportToDB()
				&& !se.ExportToDB(pSQLMaker->GetSQLW()))
			{
				_ASSERT(0);
				return false;
			}
			if (GConfig::m_bExportWriteFile 
				&& pSQLMaker->IsExportToFile()
				&& !se.ExportToFile(strExportFile.c_str(), pSQLMaker->GetSQLW()))
			{
				_ASSERT(0);
				return false;
			}
		}
	}

	return true;
}

bool GServerResLoader::LoadItemInfo()
{
	GResLoadingTimeChecker tc(L"ItemInfo");

	FILE_LOG(FILENAME_ITEMINFO);	

	GItemManager* pItemManager = m_pGlobalManager->pItemManager;
	wstring strPath = m_strRootPath + FILENAME_ITEMINFO;	
	_VLI(strPath.c_str());
	if ( pItemManager && !pItemManager->LoadFromFile(strPath.c_str()) )
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading Item Info");
		return false;
	}

	if (!Export(new GItemInfoSQLMaker(pItemManager), L"ITEM_INFO_SQL.sql"))
	{
		m_Result = Result(false, L"Failed export Item Info.\n");	
		return false;
	}
	if (!Export(new GItemLearnTalentMaker(pItemManager), L"ITEM_LEARN_TALENT_SQL.sql"))
	{
		m_Result = Result(false, L"Failed export ItemLearnTalent Info.\n");	
		return false;
	}

	return true;
}

bool GServerResLoader::LoadTalentInfo()
{
	GResLoadingTimeChecker tc(L"TalentInfo");

	FILE_LOG(PATH_SYSTEM L"talent*.xml");

	GTalentInfoMgr*	pTalentInfoMgr = m_pGlobalManager->pTalentInfoMgr;

	if (pTalentInfoMgr && !pTalentInfoMgr->Load(m_strRootPath.c_str()))
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading Talent Info\n");
		return false;
	}

	if (!Export(new GTalentInfoSQLMaker(pTalentInfoMgr), L"TALENT_INFO_SQL.sql"))
	{
		m_Result = Result(false, L"Failed export Talent.\n");	
		return false;
	}

	return true;
}

bool GServerResLoader::LoadBuffInfo()
{
	GResLoadingTimeChecker tc(L"BuffInfo");

	FILE_LOG(FILENAME_BUFFINFO);

	GBuffInfoMgr* pBuffInfoMgr = m_pGlobalManager->pBuffInfoMgr;
	wstring strPath = m_strRootPath + FILENAME_BUFFINFO;	

	if (pBuffInfoMgr && pBuffInfoMgr->Load(FILENAME_BUFFINFO) == false) 
	{
		_ASSERT(0);
		m_Result.bSuccess = false;
		m_Result.strErrorLog = L"Failed Loading Buff Info : " FILENAME_BUFFINFO L"\n";
		return false;
	}

	return true;
}

bool GServerResLoader::LoadDialogInfo()
{
	GResLoadingTimeChecker tc(L"DialogInfo");

	FILE_LOG(FILENAME_DIALOG);

	wstring strPath = m_strRootPath + FILENAME_DIALOG;
	GDialogInfoMgr* pDialogInfoMgr = m_pGlobalManager->pDialogInfoMgr;

	if (pDialogInfoMgr && pDialogInfoMgr->Load(strPath.c_str()) == false) 
	{
		_ASSERT(0);
		m_Result.bSuccess = false;
		m_Result.strErrorLog = L"Failed Loading Dialog Info : " FILENAME_DIALOG L"\n";
		return false;
	}

	return true;
}

bool GServerResLoader::LoadConditionsInfo()
{
	GResLoadingTimeChecker tc(L"ConditionsInfo");

	FILE_LOG(FILENAME_CONDITIONS);

	wstring strPath = m_strRootPath + FILENAME_CONDITIONS;
	GConditionsInfoMgr* pCondtionsInfoMgr = m_pGlobalManager->pCondtionsInfoMgr;

	if (pCondtionsInfoMgr && pCondtionsInfoMgr->Load(strPath.c_str()) == false) 
	{
		_ASSERT(0);
		m_Result.bSuccess = false;
		m_Result.strErrorLog = L"Failed Loading Condtions Info : " FILENAME_CONDITIONS L"\n";
		return false;
	}

	return true;
}

bool GServerResLoader::LoadConditionsItem()
{
	GResLoadingTimeChecker tc(L"ConditionsItem");

	FILE_LOG(FILENAME_CONDITIONS_ITEM);

	wstring strPath = m_strRootPath + FILENAME_CONDITIONS_ITEM;
	GItemConditionsInfoMgr* pItemConditionsInfoMgr = m_pGlobalManager->pItemConditionsInfoMgr;

	if (pItemConditionsInfoMgr && pItemConditionsInfoMgr->Load(strPath.c_str()) == false) 
	{
		_ASSERT(0);
		m_Result.bSuccess = false;
		m_Result.strErrorLog = L"Failed Loading Condtions_Item Info : " FILENAME_CONDITIONS_ITEM L"\n";
		return false;
	}

	return true;
}

bool GServerResLoader::LoadNPCShopInfo()
{
	GResLoadingTimeChecker tc(L"NPCShopInfo");

	FILE_LOG(FILENAME_NPCSHOP);

	wstring strPath = m_strRootPath + FILENAME_NPCSHOP;
	GNPCShopInfoMgr* pNPCShopInfoMgr = m_pGlobalManager->pNPCShopInfoMgr;

	if (pNPCShopInfoMgr && pNPCShopInfoMgr->Load(strPath.c_str()) == false) 
	{
		_ASSERT(0);
		m_Result.bSuccess = false;
		m_Result.strErrorLog = L"Failed Loading Trade Info : " FILENAME_NPCSHOP L"\n";
		return false;
	}

	return true;
}

bool GServerResLoader::LoadLootInfo()
{
	GResLoadingTimeChecker tc(L"LootInfo");

	FILE_LOG(FILENAME_LOOT_INFO);

	wstring strPath = m_strRootPath + FILENAME_LOOT_INFO;
	GLootInfoMgr* pLootInfoMgr = m_pGlobalManager->pLootInfoMgr;

	if (NULL != pLootInfoMgr && pLootInfoMgr->Load(strPath.c_str()) == false)
	{
		_ASSERT(0);
		m_Result.bSuccess = false;
		m_Result.strErrorLog = L"Failed Loading Loot Info : " FILENAME_LOOT_INFO L"\n";
		return false;
	}

	return true;
}

bool GServerResLoader::LoadNPCInfo()
{
	GResLoadingTimeChecker tc(L"NPCInfo");

	FILE_LOG(FILENAME_NPCINFO);

	{
		wstring strPath = m_strRootPath + FILENAME_NPCINFO;
		GNPCInfoMgr* pNPCInfoMgr = m_pGlobalManager->pNPCInfoMgr;

		if (pNPCInfoMgr && pNPCInfoMgr->Load(strPath.c_str()) == false) 
		{
			_ASSERT(0);
			m_Result.bSuccess = false;
			m_Result.strErrorLog = L"Failed Loading NPC Info : " FILENAME_NPCINFO L"\n";
			return false;
		}
	}
	

	{
		wstring strPath = m_strRootPath + FILENAME_BPART_REWARD;
		GBPartRewardParser bpart_reward_parser;
		if (!bpart_reward_parser.Parse(strPath.c_str()))
		{
			_ASSERT(0);
			m_Result.bSuccess = false;
			m_Result.strErrorLog = L"Failed Loading BPart Reward Info : " FILENAME_BPART_REWARD L"\n";
			return false;
		}
	}
	
	return true;
}

bool GServerResLoader::LoadFieldList( bool bCheckDebugFile )
{
	if (!m_pGlobalManager->pFieldInfoMgr->empty())
		return true; // 이미 로딩되어 있음

	GResLoadingTimeChecker tc(L"FieldList");

	FILE_LOG(PATH_SYSTEM L"fieldlist*.xml");

	wstring strSystemPath = m_strRootPath + wstring(PATH_SYSTEM);

	wstring strFieldListFileName = GFilePath::MakeFieldList(strSystemPath.c_str(), bCheckDebugFile);
	wstring strZoneListFileName = GFilePath::MakeZoneList(strSystemPath.c_str());

	wstring strZonePath = m_strRootPath + PATH_ZONE;
	wstring strFieldPath = m_strRootPath + PATH_FIELD;

	// 오토테스트 처리
	if (!GConfig::m_strAutoTestFieldlistFileName.empty())
	{
		strFieldListFileName = strSystemPath + GConfig::m_strAutoTestFieldlistFileName;
	}


	// 개발중인 필드도 로딩할 지 여부
	bool bIncludeDevFieldList = false;

	if ( GConst::RUN_FOR_TEST && GConst::INCLUDE_DEV_FIELD_LIST )
	{
		bIncludeDevFieldList = true;
	}

	if(m_pGlobalManager->pFieldInfoMgr && m_pGlobalManager->pFieldInfoMgr->Load(strZoneListFileName.c_str(), 
																			strFieldListFileName.c_str(), 
																			strZonePath.c_str(), 
																			strFieldPath.c_str(),
																			bIncludeDevFieldList ) == false)
	{
		_ASSERT(0);
		m_Result.bSuccess = false;
		m_Result.strErrorLog = wstring(L"Failed Loading Map Info : (") + wstring(FILENAME_LOOT_INFO) +
			wstring (L", ") + strFieldListFileName + wstring(L")\n");
		return false;
	}

	return true;
}

bool GServerResLoader::LoadQuestInfo()
{
	GResLoadingTimeChecker tc(L"QuestInfo");

	FILE_LOG(FILENAME_QUEST);

	wstring strPath = m_strRootPath + FILENAME_QUEST;
	GQuestInfoMgr* pQuestInfoMgr = m_pGlobalManager->pQuestInfoMgr;

	if (pQuestInfoMgr && pQuestInfoMgr->Load(strPath.c_str()) == false)
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading QuestInfo\n");
		return false;
	}

	if (!Export(new GQuestInfoSQLMaker(pQuestInfoMgr), L"QUEST_INFO_SQL.sql"))
	{
		m_Result = Result(false, L"Failed export Quest.\n");
		return false;
	}

	return true;
}

bool GServerResLoader::LoadSoulBinding()
{
	GResLoadingTimeChecker tc(L"SoulBindingInfo");

	FILE_LOG(FILENAME_SOULBINDING);

	wstring strPath = m_strRootPath + FILENAME_SOULBINDING;
	GSoulBindingInfoMgr* pSoulBindingInfoMgr = m_pGlobalManager->pSoulBindingInfoMgr;

	if (pSoulBindingInfoMgr && pSoulBindingInfoMgr->Load(strPath.c_str()) == false)
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading SoulBindingInfo : soulbinding.xml\n");
		return false;
	}

	return true;
}

bool GServerResLoader::LoadRecipeInfo()
{
	GResLoadingTimeChecker tc(L"RecipeInfo");

	FILE_LOG(FILENAME_RECIPE);

	wstring strPath = m_strRootPath + FILENAME_RECIPE;
	GRecipeInfoMgr* pRecipeInfoMgr = m_pGlobalManager->pRecipeInfoMgr;

	if (pRecipeInfoMgr && pRecipeInfoMgr->Load(strPath.c_str()) == false)
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading RecipeInfo : recipe.xml\n");
		return false;
	}

	if (!Export(new GRecipeInfoSQLMaker(pRecipeInfoMgr), L"RECIPE_INFO_SQL.sql"))
	{
		m_Result = Result(false, L"Failed export Recipe.\n");	
		return false;
	}

	return true;
}

bool GServerResLoader::LoadCraftInfo()
{
	GResLoadingTimeChecker tc(L"CraftInfo");

	FILE_LOG(FILENAME_CRAFT);
	
	wstring strPath = m_strRootPath + FILENAME_CRAFT;
	GCraftInfoMgr* pCraftInfoMgr = m_pGlobalManager->pCraftInfoMgr;

	if (pCraftInfoMgr && pCraftInfoMgr->Load(strPath.c_str()) == false)
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading CraftInfo : craft.xml\n");
		return false;
	}

	return true;
}

bool GServerResLoader::LoadInnRoomInfo()
{
	GResLoadingTimeChecker tc(L"InnRoomInfo");

	FILE_LOG(FILENAME_INNROOM);

	wstring strPath = m_strRootPath + FILENAME_INNROOM;
	GInnRoomInfoMgr* pInnRoomInfoMgr = m_pGlobalManager->pInnRoomInfoMgr;

	if (pInnRoomInfoMgr && pInnRoomInfoMgr->Load(strPath.c_str()) == false)
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading InnRoomInfo : innroom.xml\n");
		return false;
	}

	return true;
}

bool GServerResLoader::LoadFactionInfo()
{
	GResLoadingTimeChecker tc(L"FactionInfo");

	FILE_LOG(FILENAME_FACTION);

	wstring strPath = m_strRootPath + FILENAME_FACTION;
	GFactionInfoMgr* pFactionInfoMgr = m_pGlobalManager->pFactionInfoMgr;

	if (pFactionInfoMgr && pFactionInfoMgr->Load(strPath.c_str()) == false)
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading FactionInfo : faction.xml\n");
		return false;
	}

	// 팩션 관계 정보
	FILE_LOG(FILENAME_FACTION_RELATION);

	strPath = m_strRootPath + FILENAME_FACTION_RELATION;
	GFactionRelationInfoMgr* pFactionRelationInfoMgr = m_pGlobalManager->pFactionRelationInfoMgr;

	if (pFactionRelationInfoMgr && pFactionRelationInfoMgr->Load(strPath.c_str()) == false)
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading FactionRelationInfo : faction_relation.xml\n");
		return false;
	}

	return true;
}

bool GServerResLoader::LoadCutsceneInfo()
{
	GResLoadingTimeChecker tc(L"CutSceneInfo");

	FILE_LOG(FILENAME_CUTSCENES);

	wstring strPath = m_strRootPath + FILENAME_CUTSCENES;
	GCutsceneInfoMgr* pCutsceneInfoMgr = m_pGlobalManager->pCutsceneInfoMgr;

	if (pCutsceneInfoMgr && pCutsceneInfoMgr->Load(strPath.c_str()) == false)
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading CutsceneInfo : cutscenes.xml");
		return false;
	}

	return true;
}

bool GServerResLoader::LoadEmblemInfo()
{
	GResLoadingTimeChecker tc(L"EmblemInfo");

	FILE_LOG(FILENAME_EMBLEM);

	wstring strPath = m_strRootPath + FILENAME_EMBLEM;
	GEmblemInfoMgr*	pEmblemInfoMgr = m_pGlobalManager->pEmblemInfoMgr;

	if (pEmblemInfoMgr && pEmblemInfoMgr->Load(strPath.c_str()) == false)
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading EmblemInfo : emblem.xml");
		return false;
	}

	if (!Export(new GEmblemInfoSQLMaker(strPath), L"EMBLEM_INFO_SQL.sql"))
	{
		m_Result = Result(false, L"Failed export EmblemInfo.\n");	
		return false;
	}


	return true;
}

bool GServerResLoader::LoadChallengerQuestInfo()
{
	GResLoadingTimeChecker tc(L"ChallengerQuestInfo");

	FILE_LOG(FILENAME_CHALLENGER_QUEST);

	wstring strPath = m_strRootPath + FILENAME_CHALLENGER_QUEST;
	GChallengerQuestMgr* pChallengerQuestMgr = m_pGlobalManager->pChallengerQuestMgr;

	if (pChallengerQuestMgr && pChallengerQuestMgr->Load(strPath.c_str()) == false)
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading ChallengerQuestInfo : challenger.xml");
		return false;
	}



	return true;
}

bool GServerResLoader::LoadQPEventInfoMgr()
{
	GResLoadingTimeChecker tc(L"QPEventInfo");

	FILE_LOG(FILENAME_QPEVENT);

	wstring strPath = m_strRootPath + FILENAME_QPEVENT;
	GQPEventInfoMgr* pQPEventInfoMgr = m_pGlobalManager->pQPEventInfoMgr;

	if (pQPEventInfoMgr && pQPEventInfoMgr->Load(strPath.c_str()) == false)
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading QPEventInfo : qpevent.xml\n");
		return false;
	}

	return true;
}

bool GServerResLoader::LoadPresetInfoMgr()
{
	GResLoadingTimeChecker tc(L"PresetInfo");

	FILE_LOG(FILENAME_PRESET);

	wstring strPath = m_strRootPath + FILENAME_PRESET;
	GPresetInfoMgr* pPresetInfoMgr = m_pGlobalManager->pPresetInfoMgr;

	if (pPresetInfoMgr && pPresetInfoMgr->Load(strPath.c_str()) == false)
	{
		_ASSERT(0);
		m_Result = Result(false, L"Failed Loading PresetInfo : preset.xml\n");
		return false;
	}

	return true;
}

bool GServerResLoader::LoadExportDBStringTable()
{
	// Export 설정이 있어야 사용
	if (!GConfig::m_bExportUpdateDB && !GConfig::m_bExportWriteFile)
	{
		return true;
	}


	// 문자열 테이블 열기 - Runtime 클라이언트 폴더 참조
	GResLoadingTimeChecker tc(L"ExportDBStringTable");
	FILE_LOG(L"..\\..\\Data\\system\\lang\\*.xml");

	
	GExportDBStringTable* pTable = m_pGlobalManager->pExportDBStringTable;	
	if (m_pGlobalManager == NULL)	return false;
	
	if (!pTable->Load(GConfig::m_strExportLocale))
	{
		m_Result = Result(false, L"Failed to loading 'export db string table'\n");
		return false;
	}

	return true;
}

void GServerResLoader::CookItemInfo()
{
	GItemManager* pItemManager = m_pGlobalManager->pItemManager;
	if (pItemManager)
	{
		pItemManager->Cooking();
	}
}

GServerResLoader::GResLoadingTimeChecker::GResLoadingTimeChecker( wstring strResName ) : m_strResName(strResName)
{
	m_TimeChecker.Start();
}

GServerResLoader::GResLoadingTimeChecker::~GResLoadingTimeChecker()
{
	dlog(L"%s Loaded (elapsed: %.1f seconds)\n", m_strResName.c_str(), m_TimeChecker.Check() * 0.001f);
}