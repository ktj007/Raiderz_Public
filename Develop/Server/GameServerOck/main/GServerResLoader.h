#pragma once

#include "GGlobal.h"

class GSQLMaker;

class GServerResLoaderLogListener
{
public:
	virtual void OnLog(const wchar_t* filename) = 0;
};

/// 게임 서버에서 필요한 각종 xml 등을 읽는 로더 클래스
class GServerResLoader
{
public:
	struct Result
	{
		bool	bSuccess;
		wstring	strErrorLog;
		Result() : bSuccess(true) {}
		Result(bool success) : bSuccess(success) {}
		Result(bool success, wstring errorLog) : bSuccess(success), strErrorLog(errorLog) {}
	};
private:
	class GResLoadingTimeChecker
	{
	private:
		MTimeChecker2	m_TimeChecker;
		wstring			m_strResName;
	public:
		GResLoadingTimeChecker(wstring strResName);
		~GResLoadingTimeChecker();
	};

	bool LoadItemInfo();
	bool LoadTalentInfo();
	bool LoadNPCShopInfo();
	bool LoadConditionsItem();
	bool LoadConditionsInfo();
	bool LoadDialogInfo();
	bool LoadBuffInfo();
	bool LoadChallengerQuestInfo();
	bool LoadEmblemInfo();
	bool LoadCutsceneInfo();
	bool LoadFactionInfo();
	bool LoadInnRoomInfo();
	bool LoadCraftInfo();
	bool LoadRecipeInfo();
	bool LoadSoulBinding();
	bool LoadQuestInfo();
	bool LoadFieldList( bool bCheckDebugFile );
	bool LoadNPCInfo();
	bool LoadLootInfo();
	bool LoadQPEventInfoMgr();
	bool LoadPresetInfoMgr();
	bool LoadExportDBStringTable();
	void CookItemInfo();

public:
	GServerResLoader(): m_pLisener(NULL), m_pGlobalManager(NULL) {}
	Result LoadAll(GGlobalManager* pGlobalManager, const wchar_t* szRootPath = L"./", bool bCheckDebugFile=true);

	void SetListener(GServerResLoaderLogListener* pListener);

	bool Export(GSQLMaker* pSQLMaker, const wstring strExportFile);

private:
	GServerResLoaderLogListener* m_pLisener;

	GGlobalManager* m_pGlobalManager;
	wstring			m_strRootPath;

	Result			m_Result;
};
