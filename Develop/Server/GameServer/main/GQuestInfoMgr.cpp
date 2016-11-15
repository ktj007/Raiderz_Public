#include "stdafx.h"
#include "GQuestInfoMgr.h"
#include "GQuestInfo.h"
#include "GDialogInfoMgr.h"
#include "GConditionsInfoMgr.h"
#include "GValidateLogger.h"
#include "GSTLUtil.h"
#include "GGlobal.h"
#include "GStrings.h"
#include "GDef.h"
#include "GScriptManager.h"
#include "GScriptGroup_Quest.h"
#include "GFieldInfoMgr.h"
#include "GItemAddRemoveInfo.h"
#include "GItemAddRemoveInfoParser.h"
#include "CSDef_Quest.h"
#include "GQObjectiveInfo.h"

GQuestInfoMgr::GQuestInfoMgr(void)
{

}

GQuestInfoMgr::~GQuestInfoMgr(void)
{
	
}

CSQuestInfo* GQuestInfoMgr::NewQuestInfo()
{
	return new GQuestInfo();
}

CSQObjectiveInfo* GQuestInfoMgr::NewQObjectiveInfo()
{
	return new GQObjectiveInfo();
}

GQuestInfo* GQuestInfoMgr::Get(int nQuestID)
{
	return static_cast<GQuestInfo*>(__super::Get(nQuestID));
}

vector<GQObjectiveInfo*> GQuestInfoMgr::GetQObjectiveByParam1(int nQObjectiveType, int nQObjectiveParam1)
{
	return ToGQObjectiveInfo(__super::GetQObjectiveByParam1(nQObjectiveType, nQObjectiveParam1));
}

bool GQuestInfoMgr::IsChallengerQuestID(int nQuestID )
{
	GQuestInfo* pQuestInfo = Get(nQuestID);
	if (NULL == pQuestInfo) return false;

	return pQuestInfo->bChallengerQuest;
}

void GQuestInfoMgr::ParseQuest(MXmlElement* pElement, MXml* pXml, CSQuestInfo* pQuestInfo)
{
	__super::ParseQuest(pElement, pXml, pQuestInfo);

	GQuestInfo* pGInfo = static_cast<GQuestInfo*>(pQuestInfo);
	 
	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), QUEST_XML_TAG_ITEM))
		{
			ParseItem(pChild, pXml, pGInfo);
		}
	}

	GetScriptQuest()->Load(pGInfo->nID);
}

void GQuestInfoMgr::ParseQRewards(MXmlElement* pElement, MXml* pXml, CSQuestInfo* pQuestInfo)
{
	__super::ParseQRewards(pElement, pXml, pQuestInfo);
	
	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(pQuestInfo->nRewardFieldID);
	if (NULL == pFieldInfo) return;
	const SPAWN_INFO_MAP& spawnInfoMap = pFieldInfo->GetSpawnInfo();

	vec3 vRewardPos = vec3::ZERO;
	for (SPAWN_INFO_MAP::const_iterator itor = spawnInfoMap.begin(); itor != spawnInfoMap.end(); ++itor)
	{
		const SPAWN_INFO& spawnInfo = (*itor).second;

		for (size_t i = 0; i < spawnInfo.vecNPCInfoList.size(); ++i)
		{
			const SPAWN_NPC_INFO& spawnNPCInfo = spawnInfo.vecNPCInfoList[i];

			if (pQuestInfo->nRewardNPCID  == spawnNPCInfo.nID)
			{
				vRewardPos = spawnInfo.vPoint;
				break;
			}
		}
	}

	if (vRewardPos == vec3::ZERO)
	{
		dlog("Quest RewardPos is ZERO (questID = %d)\n", pQuestInfo->nID);
	}
	pQuestInfo->vRewardPos = vRewardPos;
}

void GQuestInfoMgr::ParseItem( MXmlElement* pElement, MXml* pXml, GQuestInfo* pQuestInfo )
{
	pQuestInfo->pItemAddRemoveInfo = new GItemAddRemoveInfo;

	GItemAddRemoveInfoParser parser;
	parser.ParseItem(pElement, pXml, pQuestInfo->pItemAddRemoveInfo);
}

vector<GQuestInfo*> GQuestInfoMgr::GetAllQuestInfo()
{
	vector<GQuestInfo*> vecQuestInfo;
	for each (const MAP_QUESTINFO::value_type& data in m_mapQuestInfo)
	{
		vecQuestInfo.push_back(static_cast<GQuestInfo*>(data.second));
	}

	return vecQuestInfo;
}

vector<GQObjectiveInfo*> GQuestInfoMgr::ToGQObjectiveInfo(vector<CSQObjectiveInfo*> vecInfo)
{
	vector<GQObjectiveInfo*> vecGQObjectiveInfo;

	for each (CSQObjectiveInfo* pInfo in vecInfo)
	{
		vecGQObjectiveInfo.push_back(static_cast<GQObjectiveInfo*>(pInfo));
	}

	return vecGQObjectiveInfo;
}

void GQuestInfoMgr::Cooking()
{
	__super::Cooking();

	for each (const MAP_QUESTINFO::value_type& data in m_mapQuestInfo)
	{
		CSQuestInfo* pInfo = data.second;
		FIELD_GROUP_INFO* pFieldGroupInfo = gmgr.pFieldInfoMgr->FindFieldGroup(pInfo->nTrialFieldGroupID);
		if (pFieldGroupInfo)
		{
			pFieldGroupInfo->setRunningQuest.insert(pInfo->nID);
		}
	}
}