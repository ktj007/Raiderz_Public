#include "StdAfx.h"
#include "GUTHelper_QuestPVP.h"
#include "SUnitTestUtil.h"
#include "GQPEventInfoMgr.h"
#include "GQPEventInfo.h"
#include "GTestForward.h"
#include "CSDef_QuestPVP.h"


GQPEventInfo* GUTHelper_QuestPVP::NewInfo(int nID)
{
	if (nID == INVALID_ID)
	{
		nID = SUnitTestUtil::NewID();
	}

	GQPEventInfo* pInfo = gmgr.pQPEventInfoMgr->Get(nID);
	if (NULL != pInfo) return pInfo;

	pInfo = new GQPEventInfo();
	pInfo->m_nID = nID;
	gmgr.pQPEventInfoMgr->Insert(pInfo);

	return pInfo;
}

GQPEEndInfo* GUTHelper_QuestPVP::InsertEndInfo( GQPEventInfo* pEventInfo, QUESTPVP_TEAM nTeam )
{
	GQPEEndInfo* pInfo = new GQPEEndInfo();
	pInfo->m_nWinnerTeam = nTeam;

	pEventInfo->m_vecEndInfo.push_back(pInfo);	

	return pInfo;
}

std::wstring GUTHelper_QuestPVP::InsertNotice(GQPEBeginInfo& beginInfo, wstring strNotice)
{
	if (L"" == strNotice)
	{
		strNotice = SUnitTestUtil::NewString();
	}

	beginInfo.m_vecStrNotice.push_back(strNotice);

	return strNotice;
}

std::wstring GUTHelper_QuestPVP::InsertNotice( GQPEEndInfo* pEndInfo, wstring strNotice)
{
	if (L"" == strNotice)
	{
		strNotice = SUnitTestUtil::NewString();
	}

	pEndInfo->m_vecStrNotice.push_back(strNotice);

	return strNotice;
}

GQPESpawnInfo* GUTHelper_QuestPVP::InsertSpawnInfo(GQPEBeginInfo& beginInfo, const GFieldInfo* pFieldInfo, int nNPCID, float fDelayTime, float fDespawnTime)
{
	SPAWN_INFO* pSpawnInfo = test.field.InsertNewSpawnInfo(pFieldInfo, 0, nNPCID);

	GQPESpawnInfo* pInfo = new GQPESpawnInfo();
	pInfo->m_nSpawnID = pSpawnInfo->nID;
	pInfo->m_fDelayTime = fDelayTime;
	pInfo->m_fDespawnTime = fDespawnTime;

	beginInfo.m_vecSpawnInfo.push_back(pInfo);
	return pInfo;
}

GQPESpawnInfo* GUTHelper_QuestPVP::InsertSpawnInfo( GQPEEndInfo* pEndInfo, const GFieldInfo* pFieldInfo, int nNPCID, float fDelayTime, float fDespawnTime)
{
	SPAWN_INFO* pSpawnInfo = test.field.InsertNewSpawnInfo(pFieldInfo, 0, nNPCID);

	GQPESpawnInfo* pInfo = new GQPESpawnInfo();
	pInfo->m_nSpawnID = pSpawnInfo->nID;
	pInfo->m_fDelayTime = fDelayTime;
	pInfo->m_fDespawnTime = fDespawnTime;

	pEndInfo->m_vecSpawnInfo.push_back(pInfo);
	return pInfo;
}

GQPEItemInfo* GUTHelper_QuestPVP::InsertItemInfo( GQPEEndInfo* pEndInfo, int nItemID, int nAmount)
{
	GItemData* pItemData = test.item.NewItemData(nItemID);

	GQPEItemInfo* pInfo = new GQPEItemInfo();
	pInfo->m_nItemID = nItemID;
	pInfo->m_nAmount = nAmount;
	pEndInfo->m_vecItemInfo.push_back(pInfo);

	return pInfo;
}

void GUTHelper_QuestPVP::InsertDespawnNPCID( GQPEEndInfo* pEndInfo, int nNPCID )
{
	pEndInfo->m_vecDespawnNPCID.push_back(nNPCID);
}
