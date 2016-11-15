#include "StdAfx.h"
#include "GChallengerQuestMgr.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GEntityPlayer.h"
#include "GConst.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"
#include "GPlayerQuests.h"
#include "GQuestSystem.h"
#include "GQuestInfoMgr.h"
#include "GValidateLogger.h"
#include "GFieldInfoMgr.h"
#include "GQuestInfo.h"
#include "GCommandCenter.h"
#include "GFieldMgr.h"
#include "GDynamicFieldMaster.h"
#include "GQuestGiver.h"
#include "GQuestFailer.h"


GChallengerQuestMgr::Tier::Tier()
: nMinLevel(0)
, nMaxLevel(0)
, nDailyQuestCount(1)
{}

GChallengerQuestMgr::GChallengerQuestMgr(void)
: m_nRandomSeed(0)
{
	m_rgrCheckTick.SetTickTime(600); // 10분
	m_rgrCheckTick.Start();
}

GChallengerQuestMgr::~GChallengerQuestMgr(void)
{
}

GChallengerQuestMgr::Tier* GChallengerQuestMgr::Get( int nTier )
{
	if (nTier < 0)
		return NULL;
	
	if (nTier >= (int)m_vecSourceTiers.size())
		return NULL;

	return &m_vecSourceTiers[nTier];
}

void GChallengerQuestMgr::Clear()
{
	m_vecSourceTiers.clear();
	m_vecDailyQuests.clear();
}

bool GChallengerQuestMgr::Load( const wchar_t* szFileName )
{
	VALID_RET(szFileName, false);

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(CQ_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), CQ_XML_TAG_CQ))
		{
			Parse_ChallengerQuest(pElement, &xml);
		}
	}

	OnLoadCompleted();
	
	return true;
}

void GChallengerQuestMgr::Parse_ChallengerQuest( MXmlElement* pElement, MXml* pXml )
{
	m_vecSourceTiers.push_back(Tier());
	Tier& tier = m_vecSourceTiers.back();
	_Attribute(&tier.nMinLevel, pElement, CQ_XML_ATTR_MIN_LEVEL);
	_Attribute(&tier.nMaxLevel, pElement, CQ_XML_ATTR_MAX_LEVEL);
	_Attribute(&tier.nDailyQuestCount, pElement, CQ_XML_ATTR_MAX_COUNT);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), CQ_XML_TAG_QUEST))
		{
			Parse_Quest(tier, pChild, pXml);
		}
	}
}

void GChallengerQuestMgr::Parse_Quest( Tier& tier, MXmlElement* pElement, MXml* pXml )
{
	int nID;
	_Attribute(&nID, pElement, CQ_XML_ATTR_ID);

	_VLE(_T(CQ_XML_TAG_QUEST));	
	_VLA(_T(CQ_XML_ATTR_ID), nID);
	_VLP;

	for each (const GQuestInfo* each in tier.vecSourceQuests)
	{
		if (each->nID == nID)
		{
			_VLOGGER->Log(_T(CQ_XML_ATTR_ID) IS_ALREADY_EXIST);
			return;
		}
	}

	GQuestInfo* pInfo = gmgr.pQuestInfoMgr->Get(nID);
	if (!pInfo)
		return;

	tier.vecSourceQuests.push_back(pInfo);
}

void GChallengerQuestMgr::RouteQuestList( GEntityPlayer* pPlayer ) const
{
	vector<TD_CHALLENGER_QUEST> vecTD;
	CheckQuestEnable(pPlayer, vecTD);
	
	MCommand* pNewCommand = MakeNewCommand(MC_CHALLENGER_QUEST_LIST,
		1,
		NEW_BLOB(vecTD));
	pPlayer->RouteToMe(pNewCommand);
}

void GChallengerQuestMgr::RouteQuestListRefresh( GEntityPlayer* pPlayer ) const
{
	vector<TD_CHALLENGER_QUEST> vecTD;
	CheckQuestEnable(pPlayer, vecTD);

	MCommand* pNewCommand = MakeNewCommand(MC_CHALLENGER_QUEST_LIST_REFRESH,
		1,
		NEW_BLOB(vecTD));
	pPlayer->RouteToMe(pNewCommand);
}


bool GChallengerQuestMgr::IsTodayQuest(const GQuestInfo* pInfo) const
{
	return std::find(m_vecDailyQuests.begin(), m_vecDailyQuests.end(), pInfo) != m_vecDailyQuests.end();
}

void GChallengerQuestMgr::CheckQuestEnable(GEntityPlayer* pPlayer, vector<TD_CHALLENGER_QUEST>& outvecTD) const
{
	outvecTD.clear();

	for each (const Tier& tier in m_vecSourceTiers)
	{
		const vector<const GQuestInfo*>& vecSourceQuests = tier.vecSourceQuests;
		for each (const GQuestInfo* pInfo in vecSourceQuests)
		{
			const int nQuestID = pInfo->nID;

			if (pPlayer->GetQuests().Has(nQuestID))
				continue; // 퀘스트를 이미 가지고 있음

			if (pPlayer->GetQuests().IsDoneQuest(nQuestID))
				continue; // 퀘스트를 이미 깼었음

			if (pPlayer->GetLevel() < pInfo->nRequiredMinLevelForChallengerQuest)
				continue; // 최소레벨보다 작음

			if (pPlayer->GetLevel() > pInfo->nRequiredMaxLevelForChallengerQuest)
				continue; // 최대레벨보다 작음

			ChallengerQuestUnableType nUnableType = CQUT_OK;
			if (!IsTodayQuest(pInfo))
			{
				// 오늘의 퀘스트가 아님
				nUnableType = CQUT_FAIL_QUEST_NOT_TODAY;
			}
			else if (!gsys.pQuestSystem->GetGiver().CheckForChallengerQuest(pPlayer, nQuestID, pInfo))
			{
				// 퀘스트 컨디션 조건 불총족
				nUnableType = CQUT_FAIL_QUEST_CONDITION;
			}
			
			TD_CHALLENGER_QUEST td;
			td.nQuestID = nQuestID;
			td.nUnableType = nUnableType;
			outvecTD.push_back(td);
		}
	}
}

void GChallengerQuestMgr::ResetDailyQuest()
{
	m_vecDailyQuests.clear();

	SaveCurrentTime();
	InitRandomSeed();
	
	for each (const Tier& tier in m_vecSourceTiers)
	{
		vector<const GQuestInfo*> vecSourceQuests = tier.vecSourceQuests;
		if (vecSourceQuests.empty())
			continue;

		std::random_shuffle(vecSourceQuests.begin(), vecSourceQuests.end());
		size_t nSize = min(vecSourceQuests.size(), (size_t)tier.nDailyQuestCount);
		for (size_t i=0; i<nSize; ++i)
		{
			m_vecDailyQuests.push_back(vecSourceQuests[i]);
		}
	}
}

void GChallengerQuestMgr::InitRandomSeed()
{
	unsigned int nSeed = 0;
	if (m_nRandomSeed != 0)
	{
		// 지정된 랜덤시드
		nSeed = m_nRandomSeed;
	}
	else
	{
		// 날짜 기반 랜덤시드
		tm timeCurrent = GetCurrentTime();
		nSeed = timeCurrent.tm_year * timeCurrent.tm_mon * timeCurrent.tm_mday;
	}

	srand(nSeed);
}

void GChallengerQuestMgr::SetRandomSeed( unsigned int nRandomSeed )
{
	m_nRandomSeed = nRandomSeed;
}

void GChallengerQuestMgr::Update( float fDelta )
{
	if (!m_rgrCheckTick.IsReady(fDelta))
		return;

	if (CheckExpiredTime())
	{
		OnQuestExpired();
	}
}

void GChallengerQuestMgr::SaveCurrentTime()
{
	m_timeLast = GetCurrentTime();
}

bool GChallengerQuestMgr::CheckExpiredTime()
{
	tm timeCurrent = GetCurrentTime();
	if (m_timeLast.tm_mday == timeCurrent.tm_mday)
		return false; // 같은 날

	if (GConst::CQ_EXPIRED_HOUR != timeCurrent.tm_hour)
		return false; // 초기화 시간 아님

	return true;
}

void GChallengerQuestMgr::OnQuestExpired()
{
	AllQuestFail();
	ResetDailyQuest();
}

void GChallengerQuestMgr::AllQuestFail()
{
	GPlayerObjectManager::GPlayerObjectMap& player_objects = 
		gmgr.pPlayerObjectManager->GetObjects();

	for each (const GPlayerObjectManager::GPlayerObjectMap::value_type& data in player_objects)
	{
		GPlayerObject* pObj = data.second;
		DCHECK(pObj);
		GEntityPlayer* pEntity = pObj->GetEntity();
		DCHECK(pEntity);

		for each (const GQuestInfo* pQuestInfo in m_vecDailyQuests)
		{
			DCHECK(pQuestInfo);
			int nQuestID = pQuestInfo->nID;
			if (pEntity->GetQuests().Has(nQuestID))
			{
				gsys.pQuestSystem->GetFailer().Fail(pEntity, nQuestID);
			}
		}
	}
}

void GChallengerQuestMgr::OnLoadCompleted()
{
	Cooking();
	ResetDailyQuest();
}

void GChallengerQuestMgr::Cooking()
{
	for each (const Tier& tier in m_vecSourceTiers)
	{
		for each (const GQuestInfo* each in tier.vecSourceQuests)
		{
			GQuestInfo* pInfo = gmgr.pQuestInfoMgr->Get(each->nID);
			if (!pInfo)
				continue;

			pInfo->bChallengerQuest = true;
			pInfo->bCancelable = false;
			pInfo->nRequiredMaxLevelForChallengerQuest = tier.nMaxLevel;
			pInfo->nRequiredMinLevelForChallengerQuest = tier.nMinLevel;

			FIELD_GROUP_INFO* pFieldGroupInfo = gmgr.pFieldInfoMgr->FindFieldGroup(pInfo->nTrialFieldGroupID);
			if (!pFieldGroupInfo)
				continue;

			pFieldGroupInfo->bChallengerQuest = true;
		}
	}
}

size_t GChallengerQuestMgr::GetTierSize()
{
	return m_vecSourceTiers.size();
}

void GChallengerQuestMgr::OnChallengerQuestAccept( GEntityPlayer* pPlayer, const GQuestInfo* pQuestInfo ) const
{
	return RouteQuestListRefresh(pPlayer);
}

void GChallengerQuestMgr::AllQuestFailForTest()
{
	AllQuestFail();
}

void GChallengerQuestMgr::OnChallengerQuestComplete( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);
	GDynamicFieldMaster* pFieldGroup = gmgr.pFieldMgr->GetDynamicFieldMaster(pPlayer->GetPlayerField().GetDynamicFieldGroupUID());
	if (!pFieldGroup)
		return; // 동적 필드에 있지 않음

	if (!pFieldGroup->IsChallengerQuestField())
		return; // 도전자퀘스트가 없는 필드

	MCommand* pNewCmdMe = gsys.pCommandCenter->NewCommand(MC_CHALLENGER_QUEST_COMPLETE, 
		0);
	pPlayer->RouteToMe(pNewCmdMe);
}


tm GChallengerQuestMgr::GetCurrentTime()
{
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	return *timeinfo;
}