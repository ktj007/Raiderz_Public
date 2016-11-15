#include "StdAfx.h"
#include "GQuestInfoValidator.h"
#include "GFieldInfo.h"
#include "GQuestInfo.h"
#include "GQuestInfoMgr.h"
#include "GDialogInfoMgr.h"
#include "GConditionsInfoMgr.h"
#include "GNPCInfoMgr.h"
#include "GItemManager.h"
#include "GTalentInfo.h"
#include "GValidateLogger.h"
#include "GServerSystems.h"
#include "GStrings.h"
#include "GItemAddRemoveInfo.h"
#include "GItemAddRemoveInfoValidator.h"
#include "GQObjectiveInfo.h"
#include "GFieldInfoMgr.h"
#include "AStlUtil.h"
#include "MLocale.h"

bool GQuestInfoValidator::Check()
{
	if (gmgr.pQuestInfoMgr == NULL)	return true;

	_VLI(FILENAME_QUEST);

	vector<GQuestInfo*> vecQuestInfo = gmgr.pQuestInfoMgr->GetAllQuestInfo();
	for each(GQuestInfo* pQuestInfo in vecQuestInfo)
	{
		if (!pQuestInfo->bValidation) continue;	// 유효성검사 무시 플래그

		CheckQuest(pQuestInfo);
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GQuestInfoValidator::CheckQuest(GQuestInfo* pQuestInfo)
{
	_ASSERT(NULL != pQuestInfo);
	if (NULL == pQuestInfo) return;

	_VLE(_T(QUEST_XML_TAG_QUEST));	
	_VLA(_T(QUEST_XML_ATTR_ID), pQuestInfo->nID);
	_VLA(_T(QUEST_XML_ATTR_TITLE), pQuestInfo->strTitle);
	_VLP;

	// id
	_VLOGGER->ValidateID(pQuestInfo->nID);
	
	// title
	_VLOGGER->ValidateEmptyText(pQuestInfo->strTitle, _T(QUEST_XML_ATTR_TITLE));
	_VLOGGER->ValidateLengthText(pQuestInfo->strTitle, QUEST_TITLE_LEN, _T(QUEST_XML_ATTR_TITLE));

	// desc
	_VLOGGER->ValidateEmptyText(pQuestInfo->strDesc, _T(QUEST_XML_ATTR_DESC));
	_VLOGGER->ValidateLengthText(pQuestInfo->strDesc, QUEST_DESC_LEN, _T(QUEST_XML_ATTR_DESC));

	// category
	_VLOGGER->ValidateEmptyText(pQuestInfo->strCategory, _T(QUEST_XML_ATTR_CATEGORY));
	_VLOGGER->ValidateLengthText(pQuestInfo->strCategory, QUEST_CATEGORY_LEN, _T(QUEST_XML_ATTR_CATEGORY));	

	// timelimit
	_VLOGGER->ValidateMinus(pQuestInfo->nTimeLimit, _T(QUEST_XML_ATTR_TIMELIMIT));

	// players
	_VLOGGER->ValidateMinusORZero(pQuestInfo->nLimitPersonnel, _T(QUEST_XML_ATTR_PLAYERS));

	// repeatable - 제약조건 없음	
	
	// shareable - 제약조건 없음

	// level
	_VLOGGER->ValidateMinusORZero(pQuestInfo->nLevel, _T(QUEST_XML_ATTR_LEVEL));
	
	// destroy_item
	for each(int nItemId in pQuestInfo->vecDestroyItemID)
	{
		_VLOGGER->ValidateItemID(nItemId, _T(QUEST_XML_ATTR_DESTROY_ITEM));
	}

	// field_group	
	if (0 != pQuestInfo->nTrialFieldGroupID)
	{
		FIELD_GROUP_INFO* pFieldGroup = gmgr.pFieldInfoMgr->FindFieldGroup(pQuestInfo->nTrialFieldGroupID);
		if (NULL == pFieldGroup)
		{
			_VLOGGER->Log(_T(QUEST_XML_ATTR_FIELD_GROUP) IS_NOT_IN_FIELDLIST);
		}		
	}

	// dialog
	if (0 != pQuestInfo->nGiveDialogID && false == gmgr.pDialogInfoMgr->IsExist(pQuestInfo->nGiveDialogID))
	{
		_VLOGGER->Log(_T(QUEST_XML_ATTR_DIALOG) IS_NOT_IN_DIALOG);
	}

	// incomplete_dialog
	if (0 != pQuestInfo->nIncompleteDialogID && false == gmgr.pDialogInfoMgr->IsExist(pQuestInfo->nIncompleteDialogID))
	{
		_VLOGGER->Log(_T(QUEST_XML_ATTR_INCOMPLETE_DIALOG) IS_NOT_IN_DIALOG);
	}

	// share_condition
	if (0 != pQuestInfo->nShareConditionsID && false == gmgr.pCondtionsInfoMgr->IsExist(pQuestInfo->nShareConditionsID))
	{
		_VLOGGER->Log(_T(QUEST_XML_ATTR_SHARE_CONDITION) IS_NOT_IN_CONDITIONS);
	}

	// trialtype
	_VLOGGER->ValidateQuestTrialType(pQuestInfo->nTrialType, _T(QUEST_XML_ATTR_TRIALTYPE));

	// OBJECTIVES	
	CheckQObjectives(pQuestInfo);

	vector<GQObjectiveInfo*> vecQObjectiveInfo = pQuestInfo->GetAllQObjectiveInfo();
	for each (GQObjectiveInfo* pQObjectiveInfo in vecQObjectiveInfo)
	{
		CheckQObjective(pQObjectiveInfo);
	}

	// REWARDS
	CheckRewards(pQuestInfo);

	// 도전자퀘스트
	CheckChallengerQuest(pQuestInfo);
}

void GQuestInfoValidator::CheckItem(GItemAddRemoveInfo* pItemInfo)
{
	VALID(pItemInfo);

	_VLE(_T(QUEST_XML_TAG_ITEM));
	_VLP;	

	GItemAddRemoveInfoValidator itemValidator;
	itemValidator.Check(pItemInfo);	
}

void GQuestInfoValidator::CheckChallengerQuest(GQuestInfo* pQuestInfo)
{
	if (!pQuestInfo->bChallengerQuest)
		return; // 도전자퀘스트가 아님

	if (pQuestInfo->GetQObejctiveInfoCount() != 1)
	{
		_VLOGGER->Log(L"도전자 퀘스트는 오브젝티브가 한개여야만 합니다.");
	}
}


void GQuestInfoValidator::CheckQObjectives(GQuestInfo* pQuestInfo)
{
	int nCount = pQuestInfo->GetQObejctiveInfoCount();
	_VLOGGER->ValidateBiggerThan(nCount, MAX_PLAYERQOBJECTIVE, L"퀘스트의 목적개수", L"퀘스트의 최대목적개수");

	vector<GQObjectiveInfo*> vecQObjectiveInfo = pQuestInfo->GetAllQObjectiveInfo();

	set<int> setCollectItemID;
	for each (GQObjectiveInfo* pQObjectiveInfo in vecQObjectiveInfo)
	{
		if (NULL == pQObjectiveInfo)
			continue;

		_VLE(_T(QUEST_XML_TAG_OBJECTIVE));	
		_VLA(_T(QUEST_XML_ATTR_ID), pQObjectiveInfo->nID);
		_VLP;

		_VLOGGER->ValidateMinMax(_T(QUEST_XML_ATTR_ID), pQObjectiveInfo->nID, QOBJECTIVE_MIN_ID, QOBJECTIVE_MAX_ID);

		if (QOT_COLLECT == pQObjectiveInfo->nType)
		{
			if (true == IsExist(setCollectItemID, pQObjectiveInfo->vecParam1[0]))
			{
				_VLOGGER->Log(L"한 퀘스트에 동일한 종류의 아이템을 수집하는 목적은 한개만 있어야 합니다.");
			}
			else
			{
				setCollectItemID.insert(pQObjectiveInfo->vecParam1[0]);
			}
		}
	}
}

void GQuestInfoValidator::CheckQObjective(GQObjectiveInfo* pQObjectiveInfo)
{
	_ASSERT(NULL != pQObjectiveInfo);
	if (NULL == pQObjectiveInfo) return;

	_VLE(_T(QUEST_XML_TAG_OBJECTIVE));	
	_VLA(_T(QUEST_XML_ATTR_ID), pQObjectiveInfo->nID);
	_VLP;

	
	// desc
	_VLOGGER->ValidateEmptyText(pQObjectiveInfo->strDesc, _T(QUEST_XML_ATTR_DESC));
	_VLOGGER->ValidateLengthText(pQObjectiveInfo->strDesc, QUEST_OBJECTIVE_DESC_LEN, _T(QUEST_XML_ATTR_DESC));
	

	switch (pQObjectiveInfo->nType)
	{
	case QOT_DESTROY :
		{
			if (pQObjectiveInfo->vecParam1.empty())
			{
				_VLOGGER->Log(_T(QUEST_XML_ATTR_PARAM1) IS_EMPTY);
			}
			else
			{
				if (MAX_QOBJECTIVE_DESTROY_TARGET < pQObjectiveInfo->vecParam1.size())
				{
					_VLOGGER->ValidateMuchThan(static_cast<int>(pQObjectiveInfo->vecParam1.size()), MAX_QOBJECTIVE_DESTROY_TARGET, _T(QUEST_XML_ATTR_PARAM1), L"최대 destroy 대상");
				}

				for each (int nParam1 in pQObjectiveInfo->vecParam1)
				{
					_VLOGGER->ValidateNPCID(nParam1, _T(QUEST_XML_ATTR_PARAM1));
				}
			}
			
			_VLOGGER->ValidateMinusORZero(pQObjectiveInfo->nParam2, _T(QUEST_XML_ATTR_PARAM2));
		}
		break;
	case QOT_COLLECT :
		{
			if (pQObjectiveInfo->vecParam1.empty())
			{
				_VLOGGER->Log(_T(QUEST_XML_ATTR_PARAM1) IS_EMPTY);
			}
			else
			{
				_VLOGGER->ValidateItemID(pQObjectiveInfo->vecParam1[0], _T(QUEST_XML_ATTR_PARAM1));
			}

			_VLOGGER->ValidateMinusORZero(pQObjectiveInfo->nParam2, _T(QUEST_XML_ATTR_PARAM2));
		}
		break;
	case QOT_ACT :
		{
			if (pQObjectiveInfo->vecParam1.empty())
			{
				_VLOGGER->Log(_T(QUEST_XML_ATTR_PARAM1) IS_EMPTY);
			}
			else
			{
				_VLOGGER->ValidateMinusORZero(pQObjectiveInfo->vecParam1[0], _T(QUEST_XML_ATTR_PARAM1));
			}
		}
		break;
	case QOT_SCOUT :
		{
			if (pQObjectiveInfo->vecParam1.empty())
			{
				_VLOGGER->Log(_T(QUEST_XML_ATTR_PARAM1) IS_EMPTY);
			}
			else
			{
				_VLOGGER->ValidateFieldID(pQObjectiveInfo->vecParam1[0], _T(QUEST_XML_ATTR_PARAM1));
				_VLOGGER->ValidateSensorID(pQObjectiveInfo->vecParam1[0], pQObjectiveInfo->nParam2, _T(QUEST_XML_ATTR_PARAM2));
			}
		}
		break;
	case QOT_INTERACT :
		{
			if (pQObjectiveInfo->vecParam1.empty())
			{
				_VLOGGER->Log(_T(QUEST_XML_ATTR_PARAM1) IS_EMPTY);
			}
			else
			{
				_VLOGGER->ValidateNPCID(pQObjectiveInfo->vecParam1[0], _T(QUEST_XML_ATTR_PARAM1));
			}

			_VLOGGER->ValidateMinusORZero(pQObjectiveInfo->nParam2, _T(QUEST_XML_ATTR_PARAM2));
		}
		break;
	case QOT_ESCORT :
		{
			if (pQObjectiveInfo->vecParam1.empty())
			{
				_VLOGGER->Log(_T(QUEST_XML_ATTR_PARAM1) IS_EMPTY);
			}
			else
			{
				_VLOGGER->ValidateNPCID(pQObjectiveInfo->vecParam1[0], _T(QUEST_XML_ATTR_PARAM1));
			}
			
			_VLOGGER->ValidateSensorID(pQObjectiveInfo->vecParam1[0], pQObjectiveInfo->nParam2, _T(QUEST_XML_ATTR_PARAM2));
		}
		break;
	default :
		{
			_VLOGGER->Log(_T(QUEST_XML_ATTR_TYPE) IS_INVALID_TYPE);
		}
	}

	if (0 != pQObjectiveInfo->nFieldID)
	{
		GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(pQObjectiveInfo->nFieldID);
		if (NULL == pFieldInfo)
		{
			_VLOGGER->Log(_T(QUEST_XML_ATTR_FIELD) FILED_OR_MARKER_OF IS_NOT_IN_FIELDLIST);						
		}
		else
		{
			if (0 != pQObjectiveInfo->nMarkerID)
			{
				const MARKER_INFO* pMarkerInfo = pFieldInfo->FindMarker(static_cast<unsigned long>(pQObjectiveInfo->nMarkerID));
				if (NULL == pMarkerInfo)
				{
					_VLOGGER->Log(_T(QUEST_XML_ATTR_MARKER) IS_NOT_IN_FIELDLIST);
				}
			}			
		}		
	}
}

void GQuestInfoValidator::CheckRewards(GQuestInfo* pQuestInfo)
{
	_ASSERT(NULL != pQuestInfo);
	if (NULL == pQuestInfo) return;

	_VLE(_T(QUEST_XML_TAG_REWARDS));
	_VLP;

	if (!pQuestInfo->bChallengerQuest && 0 == pQuestInfo->nRewardDialogID)
	{
		_VLOGGER->Log(_T(QUEST_XML_ATTR_DIALOG) IS_NOT_IN_DIALOG);
	}

	for each (const QUEST_REWARD& questReward in pQuestInfo->vecReward)
	{
		CheckReward(questReward, pQuestInfo->bRepeatable);
	}

	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(pQuestInfo->nRewardFieldID);
	if (NULL == pFieldInfo)
	{
		_VLOGGER->Log(_T(QUEST_XML_ATTR_REWARDER) FILED_OR_MARKER_OF IS_NOT_IN_FIELDLIST);		
	}

	if (vec3::ZERO == pQuestInfo->vRewardPos)
	{
		_VLOGGER->Log(_T(QUEST_XML_ATTR_REWARDER) NPCID_OF IS_NOT_IN_FIELDLIST);
	}

	CheckSelectableReward(pQuestInfo->vecSelectableReward);
}

void GQuestInfoValidator::CheckReward(const QUEST_REWARD& questReward, bool bRepeatableQuest)
{
	_VLE(_T(QUEST_XML_TAG_REWARD));
	_VLA(_T(QUEST_XML_ATTR_TYPE), GStrings::QuestRewardTypeToString(questReward.nType));
	_VLP;

	_VLOGGER->ValidateQuestRewardType(questReward.nType, _T(QUEST_XML_ATTR_TYPE));

	switch (questReward.nType)
	{
	case QRT_MONEY :
		{
			_VLOGGER->ValidateMinusORZero(questReward.nParam1, _T(QUEST_XML_ATTR_PARAM1));			
			
			if (questReward.nParam2 != 0)
			{				
				_VLOGGER->Log(L"%s 는 " _T(QUEST_XML_ATTR_PARAM2) L"를 사용하지 않습니다.", MLocale::ConvTCHARToAnsi(GStrings::QuestRewardTypeToString(QRT_MONEY)).c_str());
			}
		}
		break;
	case QRT_XP :
		{
			_VLOGGER->ValidateMinusORZero(questReward.nParam1, _T(QUEST_XML_ATTR_PARAM1));

			if (questReward.nParam2 != 0)
			{				
				_VLOGGER->Log(L"%s 는 " _T(QUEST_XML_ATTR_PARAM2) L"를 사용하지 않습니다.", MLocale::ConvTCHARToAnsi(GStrings::QuestRewardTypeToString(QRT_XP)).c_str());
			}

			if (true == bRepeatableQuest)
			{
				_VLOGGER->Log(L"반복 퀘스트는 보상으로 경험치를 주면 안됩니다.");
			}
		}
		break;
	case QRT_TITLE :
		{			
			if (questReward.nParam2 != 0)
			{				
				_VLOGGER->Log(L"%s 는 " _T(QUEST_XML_ATTR_PARAM2) L"를 사용하지 않습니다.", MLocale::ConvTCHARToAnsi(GStrings::QuestRewardTypeToString(QRT_TITLE)).c_str());
			}
		}
		break;
	case QRT_ITEM :
		{
			_VLOGGER->ValidateItemID(questReward.nParam1, _T(QUEST_XML_ATTR_PARAM1));
			_VLOGGER->ValidateMinusORZero(questReward.nParam2, _T(QUEST_XML_ATTR_PARAM2));
		}
		break;
	case QRT_FACTION :
		{
		}
		break;
	default:
		{			
		}
	}
}

void GQuestInfoValidator::CheckSelectableReward(const vector<QUEST_REWARD>& vecSelectableReward)
{
	if (true == vecSelectableReward.empty()) return;

	_VLE(_T(QUEST_XML_TAG_REWARD));
	_VLA(_T(QUEST_XML_ATTR_TYPE), L"선택 가능한 아이템 보상");
	_VLP;

	for each (const QUEST_REWARD& selectableQuestReward in vecSelectableReward)
	{
		if (QRT_ITEM != selectableQuestReward.nType)
		{
			_VLOGGER->Log(_T(QUEST_XML_ATTR_TYPE) IS_INVALID_TYPE);
		}

		_VLOGGER->ValidateItemID(selectableQuestReward.nParam1, _T(QUEST_XML_ATTR_PARAM1));
		_VLOGGER->ValidateMinusORZero(selectableQuestReward.nParam2, _T(QUEST_XML_ATTR_PARAM2));
	}	
}