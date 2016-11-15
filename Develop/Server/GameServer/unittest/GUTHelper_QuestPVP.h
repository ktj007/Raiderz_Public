#pragma once

class GQPEventInfo;
class GQPEEndInfo;
class GQPESpawnInfo;
class GQPEItemInfo;
class GQPEBeginInfo;

enum QUESTPVP_TEAM;

class GUTHelper_QuestPVP
{
public:
	static GQPEventInfo* NewInfo(int nID = INVALID_ID);
	static GQPEEndInfo* InsertEndInfo(GQPEventInfo* pInfo, QUESTPVP_TEAM nTeam);

	static wstring InsertNotice(GQPEBeginInfo& beginInfo, wstring strNotice =L"");
	static wstring InsertNotice(GQPEEndInfo* pEndInfo, wstring strNotice =L"");

	static GQPESpawnInfo* InsertSpawnInfo(GQPEBeginInfo& beginInfo, const GFieldInfo* pFieldInfo, int nNPCID = INVALID_ID, float fDelayTime=0.0f, float fDespawnTime=0.0f);
	static GQPESpawnInfo* InsertSpawnInfo(GQPEEndInfo* pEndInfo, const GFieldInfo* pFieldInfo, int nNPCID = INVALID_ID, float fDelayTime=0.0f, float fDespawnTime=0.0f);

	static GQPEItemInfo* InsertItemInfo(GQPEEndInfo* pEndInfo, int nItemID = INVALID_ID, int nAmount = 1);	

	static void InsertDespawnNPCID(GQPEEndInfo* pEndInfo, int nNPCID);
};
