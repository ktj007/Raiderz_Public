#include "stdafx.h"
#include "GTrialFieldValidator.h"
#include "GValidateLogger.h"
#include "GGlobal.h"
#include "GDef.h"
#include "GStrings.h"
#include "GQuestInfoMgr.h"
#include "GFieldInfoMgr.h"
#include "GFieldInfo.h"
#include "GConditionsInfoMgr.h"
#include "GConditionsInfo.h"
#include "GConditionInfo.h"
#include "GFieldInfoLoader.h"
#include "CSDef_Quest.h"
#include "GQuestInfo.h"

bool GTrialFieldValidator::Check()
{
	if (!IsCheckPossible())	return true;

	CheckQuestInfo();
	CheckWarpSensor();

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

bool GTrialFieldValidator::IsCheckPossible(void)
{
	if (gmgr.pQuestInfoMgr == NULL)		return false;
	if (gmgr.pFieldInfoMgr == NULL)		return false;
	if (gmgr.pCondtionsInfoMgr == NULL)	return false;

	return true;
}

void GTrialFieldValidator::CheckQuestInfo(void)
{
	_ASSERT(gmgr.pQuestInfoMgr != NULL);

	_VLI(FILENAME_QUEST);

	vector<GQuestInfo*> vecQuestInfo = gmgr.pQuestInfoMgr->GetAllQuestInfo();
	for each(GQuestInfo* pQuestInfo in vecQuestInfo)
	{
		if (pQuestInfo->nTrialType == QTT_PARTY)	continue;
		if (!pQuestInfo->IsTrialQuest())			continue;
		_ASSERT(pQuestInfo->nTrialFieldGroupID != 0);
		

		// 파티타입 퀘스트에서 같은 필드를 쓰는지 확인한다.
		int nQuestID = pQuestInfo->nID;
		
		CheckTrialType(nQuestID, pQuestInfo->nTrialFieldGroupID);
	}
}

void GTrialFieldValidator::CheckTrialType(int nSingleQuestID, int nFieldGroupID)
{
	_ASSERT(gmgr.pQuestInfoMgr != NULL);

	
	vector<GQuestInfo*> vecQuestInfo = gmgr.pQuestInfoMgr->GetAllQuestInfo();
	for each(GQuestInfo* pQuestInfo in vecQuestInfo)
	{
		if (pQuestInfo->nTrialType == QTT_SINGLE)	continue;
		if (!pQuestInfo->IsTrialQuest())			continue;
		_ASSERT(pQuestInfo->nTrialFieldGroupID != 0);


		_VLE(_T(QUEST_XML_TAG_QUEST));
		_VLA(_T(QUEST_XML_ATTR_ID), pQuestInfo->nID);
		_VLA(_T(QUEST_XML_ATTR_TRIALTYPE), GStrings::QuestTiralTypeToString(pQuestInfo->nTrialType));
		_VLA(_T(QUEST_XML_ATTR_FIELD_GROUP), nFieldGroupID);
		_VLP;


		// 필드를 비교한다.
		if (nFieldGroupID == pQuestInfo->nTrialFieldGroupID)
		{				
			_VLOGGER->Log(L"다른 퀘스트(%d)에서 single로 지정된 트라이얼 필드는 party_T()로 사용할 수 없습니다.\n", nSingleQuestID);
			SetFailed();
		}
	}
}

void GTrialFieldValidator::CheckWarpSensor(void)
{	
	_ASSERT(gmgr.pFieldInfoMgr != NULL);


	// 전체 필드의
	for each(GFieldInfoMgr::value_type pairFieldInfo in *gmgr.pFieldInfoMgr)
	{
		GFieldInfo* pFieldInfo = pairFieldInfo.second;
		_ASSERT(pFieldInfo);
		
		_VLI(pFieldInfo->MakeFieldFileName(PATH_FIELD).c_str());

		CheckSensorWarpInfos(pFieldInfo);

		// 모든 센서에서
		const SENSOR_INFO_MAP_BY_ID& mapSensorInfo = pFieldInfo->GetSensorInfoContainer();
		for each(const SENSOR_INFO_MAP_BY_ID::value_type& pairSensorInfo in mapSensorInfo)
		{
			const SENSOR_INFO& rSensorInfo = pairSensorInfo.second;

			_VLE(_T(FIELD_XML_TAG_SENSOR));
			_VLA(_T(FIELD_XML_ATTR_ID), rSensorInfo.nID);
			_VLP;

			
			bool isTrialSensor = false;

			// 트라이얼 필드 워프 테그를 확인
			for each(const SENSOR_WARP_INFO& infoWarp in rSensorInfo.vecWarpInfos)
			{
				if (infoWarp.nFieldGroupID == INVALID_ID)	continue;
				isTrialSensor = true;

				_VLE(_T(FIELD_XML_TAG_WARP));
				_VLA(_T(FIELD_XML_ATTR_FIELD_GROUP), infoWarp.nFieldGroupID);
				_VLA(_T(FIELD_XML_ATTR_QUEST), infoWarp.nQuestID);
				_VLP;


				int nQuestID = infoWarp.nQuestID;
				int nFieldGroupID = infoWarp.nFieldGroupID;				
				
				CheckWarpQuest(nQuestID, nFieldGroupID);
			}

			if (isTrialSensor)
			{
				int nMarkerID = rSensorInfo.nExitMarkerID;
				if (nMarkerID == INVALID_ID)
				{
					_VLOGGER->Log(L"트라이얼 필드로 연결된 센서는 %s값이 있어야합니다.\n", _T(FIELD_XML_ATTR_EXIT));
					SetFailed();
				}
				else
				{
					const MARKER_INFO* pMarkerInfo = pFieldInfo->FindMarker(nMarkerID);
					if (pMarkerInfo == NULL)
					{
						_VLOGGER->Log(L"%s가 잘못된 마커(%d)를 참조하고 있습니다.\n", _T(FIELD_XML_ATTR_EXIT), nMarkerID);
						SetFailed();
					}
				}
			}
		}
	}
}

void GTrialFieldValidator::CheckWarpQuest(int nQuestID, int nFieldGropuID)
{
	GQuestInfo* pQuestInfo = gmgr.pQuestInfoMgr->Get(nQuestID);


	// 퀘스트 있는지 확인
	if (pQuestInfo == NULL)
	{
		_VLOGGER->Log(L"%s값이 유효하지 않습니다.\n", _T(FIELD_XML_ATTR_QUEST));
		SetFailed();
		return;
	}


	// 트라이얼 퀘스트인지 확인
	if (!pQuestInfo->IsTrialQuest())
	{
		_VLOGGER->Log(L"%s에 설정된 퀘스트(%d)는 트라이얼 퀘스트가 아닙니다.\n", _T(FIELD_XML_ATTR_CONDITION), nQuestID);
		SetFailed();
		return;
	}

	// 다른 트라이얼 필드를 참조하는 퀘스트인지 확인
	_ASSERT(pQuestInfo->nTrialFieldGroupID != 0);
	if (nFieldGropuID != pQuestInfo->nTrialFieldGroupID)
	{		
		_VLOGGER->Log(L"%s과 %s에 설정된 퀘스트(%d)의 트라이얼 필드가 서로 다릅니다.\n", FIELD_XML_ATTR_CONDITION, _T(FIELD_XML_ATTR_FIELD_GROUP), nQuestID);
		SetFailed();
		return;
	}
}

void GTrialFieldValidator::CheckSensorWarpInfos(GFieldInfo* pFieldInfo)
{
	for (SENSOR_INFO_MAP_BY_ID::const_iterator itor = pFieldInfo->GetSensorInfoContainer().begin(); 
		itor != pFieldInfo->GetSensorInfoContainer().end(); itor++)
	{
		const SENSOR_INFO& refSensorInfo = (*itor).second;

		_VLE(_T(FIELD_XML_TAG_SENSOR));
		_VLA(_T(FIELD_XML_ATTR_ID), refSensorInfo.nID);
		_VLP;

		for (size_t i=0; i<refSensorInfo.vecWarpInfos.size(); i++)
		{
			const SENSOR_WARP_INFO& refWarpInfo = refSensorInfo.vecWarpInfos[i];

			if (refWarpInfo.nFieldGroupID != INVALID_ID &&
				refWarpInfo.nFieldID != INVALID_ID)
			{
				_VLOGGER->Log(L"Warp Info : 동적필드와 공유필드 중 하나만 서술해야 합니다. (Field: %d, TargetField: %d, TargetFieldGroupID: %d)\n", 
					pFieldInfo->m_nFieldID, refWarpInfo.nFieldID, refWarpInfo.nFieldGroupID);
			}

			if (refWarpInfo.eGateType == TGT_NONE)
			{
				_VLOGGER->Log(L"Warp Info : 동적필드의 Gate Type 이 유효하지 않습니다.\n");
			}
		}
	}
	
}
