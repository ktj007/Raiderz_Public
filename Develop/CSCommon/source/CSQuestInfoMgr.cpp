#include "stdafx.h"
#include "CSQuestInfoMgr.h"
#include "AStlUtil.h"
#include "CSQuestInfo.h"
#include "CSDef_Quest.h"
#include "CSStrings.h"
#include "CSQObjectiveInfo.h"
#include "MLocale.h"
#include "CSFormatString.h"

CSQuestInfoMgr::CSQuestInfoMgr()
{

}

CSQuestInfoMgr::~CSQuestInfoMgr()
{
	Clear();
}

void CSQuestInfoMgr::Clear()
{
	SAFE_DELETE_MAP(m_mapQuestInfo);
	m_multimapDestroyQObjective.clear();
	m_multimapCollectQObjective.clear();
	m_multimapScoutQObjective.clear();
	m_multimapEscortQObjective.clear();
}

CSQuestInfo* CSQuestInfoMgr::Get(int nID)
{
	MAP_QUESTINFO::iterator itor = m_mapQuestInfo.find(nID);
	if (m_mapQuestInfo.end() == itor) return NULL;

	return (*itor).second;
}

void CSQuestInfoMgr::Insert( CSQuestInfo* pQuestInfo )
{
	if (NULL == pQuestInfo) return;	

	m_mapQuestInfo.insert(MAP_QUESTINFO::value_type(pQuestInfo->nID, pQuestInfo));	 
}


bool CSQuestInfoMgr::IsExist(int nQuestID)
{
	if (NULL == Get(nQuestID)) return false;

	return true;
}

void CSQuestInfoMgr::MakeQOInfoCache_Destroy(int nParam1, CSQObjectiveInfo* pQObjectiveInfo)
{
	m_multimapDestroyQObjective.insert(MULTIMAP_QOBJECTIVEINFO::value_type(nParam1, pQObjectiveInfo));
}

void CSQuestInfoMgr::MakeQOInfoCache_Collect(int nParam1, CSQObjectiveInfo* pQObjectiveInfo)
{
	m_multimapCollectQObjective.insert(MULTIMAP_QOBJECTIVEINFO::value_type(nParam1, pQObjectiveInfo));
}

void CSQuestInfoMgr::MakeQOInfoCache_Scout(int nParam1, CSQObjectiveInfo* pQObjectiveInfo)
{
	m_multimapScoutQObjective.insert(MULTIMAP_QOBJECTIVEINFO::value_type(nParam1, pQObjectiveInfo));
}

void CSQuestInfoMgr::MakeQOInfoCache_Escort(int nParam1, CSQObjectiveInfo* pQObjectiveInfo)
{
	m_multimapEscortQObjective.insert(MULTIMAP_QOBJECTIVEINFO::value_type(nParam1, pQObjectiveInfo));
}

void CSQuestInfoMgr::Visit(IVisitor& visitor) const
{
	for each (const MAP_QUESTINFO::value_type& questPair in m_mapQuestInfo)
	{
		CSQuestInfo* questInfo(questPair.second);
		assert(NULL!=questInfo);

		bool continueVisit = true;
		visitor.OnVisit(*questInfo, continueVisit);

		if (false == continueVisit)
		{
			return;
		}
	}
}

vector<CSQObjectiveInfo*> CSQuestInfoMgr::GetQObjectiveByParam1( int nQObjectiveType, int nQObjectiveParam1)
{
	MULTIMAP_QOBJECTIVEINFO::iterator itor;
	MULTIMAP_QOBJECTIVEINFO::iterator lastElement;

	switch (nQObjectiveType)
	{
	case QOT_DESTROY:
		{
			itor = m_multimapDestroyQObjective.find(nQObjectiveParam1);
			if(m_multimapDestroyQObjective.end() == itor) return vector<CSQObjectiveInfo*>();

			lastElement = m_multimapDestroyQObjective.upper_bound(nQObjectiveParam1);
		}
		break;
	case QOT_COLLECT:
		{
			itor = m_multimapCollectQObjective.find(nQObjectiveParam1);
			if(m_multimapCollectQObjective.end() == itor) return vector<CSQObjectiveInfo*>();

			lastElement = m_multimapCollectQObjective.upper_bound(nQObjectiveParam1);
		}
		break;
	case QOT_SCOUT:
		{
			itor = m_multimapScoutQObjective.find(nQObjectiveParam1);
			if(m_multimapScoutQObjective.end() == itor) return vector<CSQObjectiveInfo*>();

			lastElement = m_multimapScoutQObjective.upper_bound(nQObjectiveParam1);
		}
		break;
	case QOT_ESCORT:
		{
			itor = m_multimapEscortQObjective.find(nQObjectiveParam1);
			if(m_multimapEscortQObjective.end() == itor) return vector<CSQObjectiveInfo*>();

			lastElement = m_multimapEscortQObjective.upper_bound(nQObjectiveParam1);
		}
		break;
	}

	vector<CSQObjectiveInfo*> vecQObjectiveInfo;
	for(; lastElement != itor; ++itor)
	{
		vecQObjectiveInfo.push_back((*itor).second);
	}

	return vecQObjectiveInfo;
}

vector<int> CSQuestInfoMgr::GetAllQuestID()
{
	vector<int> vecAllQuestID;
	for each (const MAP_QUESTINFO::value_type& data in m_mapQuestInfo)
	{
		 vecAllQuestID.push_back(data.first);
	}

	return vecAllQuestID;
}

bool CSQuestInfoMgr::Load(const TCHAR* szFileName)
{
	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(QUEST_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), QUEST_XML_TAG_QUEST))
		{
			int nID = 0;
			_Attribute(nID,	pElement, QUEST_XML_ATTR_ID);
			if (true == IsExist(nID)) continue;

			CSQuestInfo* pNewInfo = NewQuestInfo();
			ParseQuest(pElement, &xml, pNewInfo);
			Insert(pNewInfo);
		}
	}

	Cooking();

	return true;
}


bool CSQuestInfoMgr::Reload( const TCHAR* szFileName )
{
	Clear();
	return Load(szFileName);
}

void CSQuestInfoMgr::ParseQuest(MXmlElement* pElement, MXml* pXml, CSQuestInfo* pInfo)
{
	_Attribute(&pInfo->nID,			pElement, QUEST_XML_ATTR_ID);
	//_Attribute(pInfo->strTitle,		pElement, QUEST_XML_ATTR_TITLE, pXml);
	pInfo->strTitle = CSFormatString::Format(_T("QUEST_TITLE_{0}"), FSParam(pInfo->nID));	// 문자열테이블 키
	_Attribute(pInfo->strDesc,		pElement, QUEST_XML_ATTR_DESC, pXml);
	_Attribute(pInfo->strCategory,	pElement, QUEST_XML_ATTR_CATEGORY, pXml);
	_Attribute(pInfo->nTimeLimit,	pElement, QUEST_XML_ATTR_TIMELIMIT);
	_Attribute(pInfo->nRequiredConditionIDForChallengerQuest,	pElement, QUEST_XML_ATTR_CQ_CONDITION);

	tstring strValue;
	if (true == _Attribute(strValue,		pElement, QUEST_XML_ATTR_REPEATABLE, pXml))
	{
		pInfo->bRepeatable = CSStrings::StringToBool(strValue.c_str());
	}	
	if (true == _Attribute(strValue,		pElement, QUEST_XML_ATTR_SHARE, pXml))
	{
		pInfo->nShareType = static_cast<QUEST_SHARE_TYPE>(CSStrings::StringToQuestShareType(strValue.c_str()));
	}
	_Attribute(&pInfo->nLevel,			pElement, QUEST_XML_ATTR_LEVEL);
	_Attribute(&pInfo->nLimitPersonnel,		pElement, QUEST_XML_ATTR_PLAYERS);

	if (true == _Attribute(strValue,	pElement, QUEST_XML_ATTR_TRIALTYPE, MLocale::ConvTCHARToAnsi(CSStrings::QuestTiralTypeToString(0)).c_str(), pXml))
	{
		pInfo->nTrialType = static_cast<QUEST_TRIAL_TYPE>(CSStrings::StringToQuestTrialType(strValue.c_str()));
	}

	_Attribute(&pInfo->nGiveDialogID,	pElement, QUEST_XML_ATTR_DIALOG);
	_Attribute(&pInfo->nIncompleteDialogID,	pElement, QUEST_XML_ATTR_INCOMPLETE_DIALOG);
	_Attribute(&pInfo->nShareConditionsID,	pElement, QUEST_XML_ATTR_SHARE_CONDITION);

	_Attribute(pInfo->nTrialFieldGroupID,	pElement, QUEST_XML_ATTR_FIELD_GROUP);

	if (true == _Attribute(&pInfo->nPartyFieldID, pElement, QUEST_XML_ATTR_PARTYFIELDID))
	{
		pInfo->bAutoParty = true;
	}
	_Attribute(&pInfo->nPartySensorID, pElement, QUEST_XML_ATTR_PARTYSENSORID);
	_Attribute(&pInfo->nAutoPartyMemberLimit, pElement, QUEST_XML_ATTR_PARTY_MEMBER_COUNT);

	if (true == _Attribute(strValue,		pElement, QUEST_XML_ATTR_DESTROY_ITEM, pXml))
	{
		CSStrings::StringNumbersToIntVector(strValue, pInfo->vecDestroyItemID);
	}

	if (true == _Attribute(strValue, pElement, QUEST_XML_ATTR_PUBLIC_VAR, pXml))
	{
		pInfo->bPublicVar = CSStrings::StringToBool(strValue.c_str());
	}	

	if (true == _Attribute(strValue,		pElement, QUEST_XML_ATTR_VALIDATION, pXml))
	{
		pInfo->bValidation = CSStrings::StringToBool(strValue.c_str());
	}

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{		
		if (!_stricmp(pChild->Value(), QUEST_XML_TAG_OBJECTIVES))
		{
			ParseQObjectives(pChild, pXml, pInfo);
		}
		else if (!_stricmp(pChild->Value(), QUEST_XML_TAG_REWARDS))
		{
			ParseQRewards(pChild, pXml, pInfo);
		}
	}
}

void CSQuestInfoMgr::ParseQObjectives(MXmlElement* pElement, MXml* pXml, CSQuestInfo* pQuestInfo)
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		if (!_stricmp(pChild->Value(), QUEST_XML_TAG_OBJECTIVE))
		{
			CSQObjectiveInfo* pQObjectiveInfo = NewQObjectiveInfo();
			pQObjectiveInfo->nQuestID = pQuestInfo->nID;
			ParseQObjective(pChild, pXml, pQObjectiveInfo);
			pQuestInfo->Insert(pQObjectiveInfo);
		}
	}
}


void CSQuestInfoMgr::ParseQObjective(MXmlElement* pElement, MXml* pXml, CSQObjectiveInfo* pQObjectiveInfo)
{
	_Attribute(pQObjectiveInfo->nID, pElement, QUEST_XML_ATTR_ID);
	_Attribute(pQObjectiveInfo->strDesc, pElement, QUEST_XML_ATTR_DESC, pXml);

	tstring strValue;
	if (true == _Attribute(strValue, pElement, QUEST_XML_ATTR_TYPE, pXml))
	{
		pQObjectiveInfo->nType = static_cast<QOBJECTIVE_TYPE>(CSStrings::StringToQuestObjectiveType(strValue.c_str()));
		switch (pQObjectiveInfo->nType)
		{
		case QOT_DESTROY:
			{
				if (true == _Attribute(strValue, pElement, QUEST_XML_ATTR_PARAM1, pXml))
				{
					CSStrings::StringNumbersToIntVector(strValue, pQObjectiveInfo->vecParam1);
				}
				_Attribute(&pQObjectiveInfo->nParam2, pElement, QUEST_XML_ATTR_PARAM2);
			}
			break;
		case QOT_COLLECT:
			{
				int nParam1;
				if (_Attribute(&nParam1, pElement, QUEST_XML_ATTR_PARAM1))
				{
					pQObjectiveInfo->vecParam1.push_back(nParam1);
				}
				_Attribute(&pQObjectiveInfo->nParam2, pElement, QUEST_XML_ATTR_PARAM2);
			}
			break;
		case QOT_ACT:
			{
				int nParam1;
				if (_Attribute(&nParam1, pElement, QUEST_XML_ATTR_PARAM1))
				{
					pQObjectiveInfo->vecParam1.push_back(nParam1);
				}
			}
			break;
		case QOT_SCOUT:
			{
				int nParam1;
				if (_Attribute(&nParam1, pElement, QUEST_XML_ATTR_PARAM1))
				{
					pQObjectiveInfo->vecParam1.push_back(nParam1);
				}
				_Attribute(&pQObjectiveInfo->nParam2, pElement, QUEST_XML_ATTR_PARAM2);
			}
			break;
		case QOT_INTERACT:
			{
				if (true == _Attribute(strValue, pElement, QUEST_XML_ATTR_PARAM1, pXml))
				{
					CSStrings::StringNumbersToIntVector(strValue, pQObjectiveInfo->vecParam1);
				}
				_Attribute(&pQObjectiveInfo->nParam2, pElement, QUEST_XML_ATTR_PARAM2);
			}
			break;
		case QOT_ESCORT:
			{
				int nParam1;
				if (_Attribute(&nParam1, pElement, QUEST_XML_ATTR_PARAM1))
				{
					pQObjectiveInfo->vecParam1.push_back(nParam1);
				}

				_Attribute(&pQObjectiveInfo->nParam2, pElement, QUEST_XML_ATTR_PARAM2);
			}
			break;
		}
	}

	if (true == _Attribute(strValue, pElement, QUEST_XML_ATTR_PUBLIC_PROGRESS, pXml))
	{
		int nPublicProgress = CSStrings::StringToQOPublicProgress(strValue.c_str());

		if (-1 != nPublicProgress)
		{
			pQObjectiveInfo->nPublicProgress = static_cast<QO_PUBLIC_PROGRESS>(nPublicProgress);
		}
	}

	_Attribute(&pQObjectiveInfo->nFieldID, pElement, QUEST_XML_ATTR_FIELD);
	_Attribute(&pQObjectiveInfo->nMarkerID, pElement, QUEST_XML_ATTR_MARKER);

	_Attribute(&pQObjectiveInfo->nFieldID2, pElement, QUEST_XML_ATTR_FIELD2);
	_Attribute(&pQObjectiveInfo->nMarkerID2, pElement, QUEST_XML_ATTR_MARKER2);

	switch(pQObjectiveInfo->nType)
	{
	case QOT_DESTROY:
		{
			for each (int nParam1 in pQObjectiveInfo->vecParam1)
			{
				MakeQOInfoCache_Destroy(nParam1, pQObjectiveInfo);
			}			
		}
		break;
	case QOT_COLLECT:
		{
			if (!pQObjectiveInfo->vecParam1.empty())
			{
				MakeQOInfoCache_Collect(pQObjectiveInfo->vecParam1[0], pQObjectiveInfo);
			}
		}
		break;
	case QOT_SCOUT:
		{
			if (!pQObjectiveInfo->vecParam1.empty())
			{
				MakeQOInfoCache_Scout(pQObjectiveInfo->vecParam1[0], pQObjectiveInfo);
			}
		}
		break;
	case QOT_ESCORT:
		{
			if (!pQObjectiveInfo->vecParam1.empty())
			{
				MakeQOInfoCache_Escort(pQObjectiveInfo->vecParam1[0], pQObjectiveInfo);
			}
		}
		break;
	}
}

void CSQuestInfoMgr::ParseQRewards(MXmlElement* pElement, MXml* pXml, CSQuestInfo* pQuestInfo)
{
	_Attribute(&pQuestInfo->nRewardDialogID, pElement, QUEST_XML_ATTR_DIALOG);

	tstring strValue;
	if (true == _Attribute(strValue, pElement, QUEST_XML_ATTR_REWARDER, pXml))
	{
		CSStrings::StringToFieldIDAndNPCID(strValue.c_str(), pQuestInfo->nRewardFieldID, pQuestInfo->nRewardNPCID);
	}

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		if (!_stricmp(pChild->Value(), QUEST_XML_TAG_REWARD))
		{
			ParseQReward(pChild, pXml, pQuestInfo);
		}
	}
}

void CSQuestInfoMgr::ParseQReward(MXmlElement* pElement, MXml* pXml, CSQuestInfo* pQuestInfo)
{
	QUEST_REWARD_TYPE nType;

	tstring strValue;
	if (true == _Attribute(strValue, pElement, QUEST_XML_ATTR_TYPE, pXml))
	{
		nType = static_cast<QUEST_REWARD_TYPE>(CSStrings::StringToQuestRewardType(strValue.c_str()));
	}

	switch (nType)
	{
	case QRT_MONEY:
	case QRT_XP:
	case QRT_TITLE:
	case QRT_FACTION:
		{
			QUEST_REWARD questReward;
			questReward.nType = nType;
			_Attribute(&questReward.nParam1, pElement, QUEST_XML_ATTR_PARAM1);
			_Attribute(&questReward.nParam2, pElement, QUEST_XML_ATTR_PARAM2);
			pQuestInfo->vecReward.push_back(questReward);
		}
		break;
	case QRT_ITEM:
		{
			vector<int> vecItemID;
			vector<int> vecItemQuantity;
			if (true == _Attribute(strValue, pElement, QUEST_XML_ATTR_PARAM1, pXml))
			{				
				CSStrings::StringNumbersToIntVector(strValue.c_str(), vecItemID);

				_ASSERT(false == vecItemID.empty());
				if (true == vecItemID.empty()) return;
			}

			if (true == _Attribute(strValue, pElement, QUEST_XML_ATTR_PARAM2, pXml))
			{				
				CSStrings::StringNumbersToIntVector(strValue.c_str(), vecItemQuantity);

				_ASSERT(false == vecItemQuantity.empty());
				if (true == vecItemQuantity.empty()) return;
			}

			size_t nItemIDSize = vecItemID.size();
			size_t nItemQuantitySize = vecItemQuantity.size();

			_ASSERT(nItemIDSize == nItemQuantitySize);
			if (nItemIDSize != nItemQuantitySize) return;

			if (1 < nItemIDSize && 1 < nItemQuantitySize)
			{
				for (size_t i = 0; nItemIDSize > i; i++)
				{
					QUEST_REWARD questReward;
					questReward.nType = nType;
					questReward.nParam1 = vecItemID[i];
					questReward.nParam2 = vecItemQuantity[i];

					pQuestInfo->vecSelectableReward.push_back(questReward);
				}				
			}
			else
			{
				QUEST_REWARD questReward;
				questReward.nType = nType;
				questReward.nParam1 = vecItemID[0];
				questReward.nParam2 = vecItemQuantity[0];
				pQuestInfo->vecReward.push_back(questReward);
			}
		}
		break;
	default:
		{
			_ASSERT(0);
		}
	}
}

bool CSQuestInfoMgr::IsCollectItem(int nItemID)
{
	if (m_multimapCollectQObjective.end() == m_multimapCollectQObjective.find(nItemID)) return false;

	return true;
}

bool CSQuestInfoMgr::IsCollectItem( const vector<int>& vecQuestID, int nItemID )
{
	for each (int nQuestID in vecQuestID)
	{
		CSQuestInfo* pQuestInfo = Get(nQuestID);
		if (NULL == pQuestInfo) continue;

		vector<CSQObjectiveInfo*> vecQObjectiveInfo = pQuestInfo->GetCollectQObjectiveInfo();
		for each (CSQObjectiveInfo* pQObjectiveInfo in vecQObjectiveInfo)
		{
			if (nItemID == pQObjectiveInfo->vecParam1.front()) return true;
		}
	}

	return false;
}

bool CSQuestInfoMgr::IsDestroyNPC(int nNPCID)
{
	if (m_multimapDestroyQObjective.end() == m_multimapDestroyQObjective.find(nNPCID)) return false;

	return true;
}

bool CSQuestInfoMgr::IsExistIncompleteDialog( int nQusetID )
{
	CSQuestInfo* pQuestInfo = Get(nQusetID);
	if (NULL == pQuestInfo) return false;
	if (0 == pQuestInfo->nIncompleteDialogID) return false;

	return true;
}

