#include "stdafx.h"
#include "GWarpInfoCollector.h"
#include "GWarpInfoChecker.h"
#include "GDef.h"

// SENSOR_WARP_INFO 컬랙션 처리를 쉽게하기위한 약어정의 
typedef const vector<SENSOR_WARP_INFO>	vec_warp;
typedef vec_warp::const_iterator		it_warp;


GWarpInfoCollector::GWarpInfoCollector()
{
	// do nothing
}

GWarpInfoCollector::~GWarpInfoCollector()
{
	Clear();
}

GWarpInfoChecker* GWarpInfoCollector::NewWarpInfoChecker(const SENSOR_WARP_INFO& warpInfo)
{
	return new GWarpInfoChecker(warpInfo);
}

GWarpInfoCollector::Result GWarpInfoCollector::CollectAutoWarp(const SENSOR_INFO& sensorInfo, GEntityPlayer* pPlayer)
{	
	// 수집 정보 초기화
	Clear();

	// 자동진입 워프를 수집한다.
	if (!CollectAutoTrialWarp(sensorInfo, pPlayer))
	{
		CollectAutoSharedWarp(sensorInfo, pPlayer);
	}
	
	// 같은 필드의 워프끼리 그룹으로 묶는다.
	return ExtractWarpField();
}

GWarpInfoCollector::Result GWarpInfoCollector::CollectManualWarp(const SENSOR_INFO& sensorInfo, GEntityPlayer* pPlayer)
{
	// 수집 정보 초기화
	Clear();

	// 수동진입 워프를 수집한다.
	CollectManualTrialWarp(sensorInfo, pPlayer);
	CollectManualSharedWarp(sensorInfo, pPlayer);

	// 같은 필드의 워프끼리 그룹으로 묶는다.
	return ExtractWarpField();
}

bool GWarpInfoCollector::CollectAutoTrialWarp(const SENSOR_INFO& sensorInfo, GEntityPlayer* pPlayer)
{
	// 자동진입 트라이얼 필드 수집
	vec_warp& vecWarpInfo = sensorInfo.vecWarpInfos;
	for(it_warp it = vecWarpInfo.begin(); it != vecWarpInfo.end(); it++)
	{
		auto_ptr<GWarpInfoChecker> autoChecker(NewWarpInfoChecker(*it));
		
		if (!autoChecker->IsAutoTrialWarp())			continue;
		if (!autoChecker->CheckCondition(pPlayer))		continue;

		GWarpInfoChecker::Result checkResult = autoChecker->CheckQuestField(pPlayer);
		if (checkResult == GWarpInfoChecker::FAILURE)	continue;
		
		bool isFullHouse = checkResult == GWarpInfoChecker::FULLHOUSE;
		AddWarpInfoChecker(autoChecker.release(), isFullHouse);
	}

	return !m_vecWarpInfo.empty();
}

bool GWarpInfoCollector::CollectManualTrialWarp(const SENSOR_INFO& sensorInfo, GEntityPlayer* pPlayer)
{
	// 수동진입 트라이얼 필드 수집
	vec_warp& vecWarpInfo = sensorInfo.vecWarpInfos;
	for(it_warp it = vecWarpInfo.begin(); it != vecWarpInfo.end(); it++)
	{
		auto_ptr<GWarpInfoChecker> autoChecker(NewWarpInfoChecker(*it));
		
		if (!autoChecker->IsManualTrialWarp())			continue;
		if (!autoChecker->CheckCondition(pPlayer))		continue;

		GWarpInfoChecker::Result checkResult = autoChecker->CheckQuestField(pPlayer);
		if (checkResult == GWarpInfoChecker::FAILURE)	continue;
		
		bool isFullHouse = checkResult == GWarpInfoChecker::FULLHOUSE;
		AddWarpInfoChecker(autoChecker.release(), isFullHouse);
	}

	return !m_vecWarpInfo.empty();
}

bool GWarpInfoCollector::CollectAutoSharedWarp(const SENSOR_INFO& sensorInfo, GEntityPlayer* pPlayer)
{
	// 공유 필드 수집
	vec_warp& vecWarpInfo = sensorInfo.vecWarpInfos;
	for(it_warp it = vecWarpInfo.begin(); it != vecWarpInfo.end(); it++)
	{
		auto_ptr<GWarpInfoChecker> autoChecker(NewWarpInfoChecker(*it));
		
		if (!autoChecker->IsAutoSharedWarp())			continue;
		if (!autoChecker->CheckCondition(pPlayer))		continue;

		AddWarpInfoChecker(autoChecker.release(), false);
	}

	// 디폴트 워프 추가 검사
	const SENSOR_WARP_INFO& warpInfo = sensorInfo.DefaultWarpInfo;
	auto_ptr<GWarpInfoChecker> autoChecker(NewWarpInfoChecker(warpInfo));

	if (autoChecker->IsAutoSharedWarp())
	{
		AddWarpInfoChecker(autoChecker.release(), false);
	}

	
	return !m_vecWarpInfo.empty();
}

bool GWarpInfoCollector::CollectManualSharedWarp(const SENSOR_INFO& sensorInfo, GEntityPlayer* pPlayer)
{
	// 공유 필드 수집
	vec_warp& vecWarpInfo = sensorInfo.vecWarpInfos;
	for(it_warp it = vecWarpInfo.begin(); it != vecWarpInfo.end(); it++)
	{
		auto_ptr<GWarpInfoChecker> autoChecker(NewWarpInfoChecker(*it));
		
		if (!autoChecker->IsManualSharedWarp())			continue;
		if (!autoChecker->CheckCondition(pPlayer))		continue;

		AddWarpInfoChecker(autoChecker.release(), false);
	}

	return !m_vecWarpInfo.empty();
}

GWarpInfoCollector::Result GWarpInfoCollector::ExtractWarpField(void)
{
	// WarpInfo를 필드별로 추출
	m_vecExtract.clear();

	for(int i = 0; i < (int)m_vecWarpInfo.size(); i++)
	{
		AddExtractField(i);
	}
	
	// 추출된 필드 결과 전달
	return GetExtractResult();
}

void GWarpInfoCollector::AddWarpInfoChecker(GWarpInfoChecker* pWarpInfoChecker, bool isFullHouse)
{
	VALID(pWarpInfoChecker != NULL);
	m_vecWarpInfo.push_back(pair_warpinfo(pWarpInfoChecker, isFullHouse));
}

void GWarpInfoCollector::Clear(void)
{
	for each(pair_warpinfo warpInfo in m_vecWarpInfo)
	{
		SAFE_DELETE(warpInfo.first);
	}
	
	m_vecWarpInfo.clear();
	m_vecExtract.clear();
}

int GWarpInfoCollector::GetWarpCount(void) const
{
	return m_vecExtract.size();
}

int GWarpInfoCollector::GetFieldGroupID(int nIndex) const
{
	const GWarpInfoChecker* pWarpInfoChecker = GetWarpInfoChecker(nIndex);
	VALID_RET(pWarpInfoChecker != NULL, INVALID_ID);

	return pWarpInfoChecker->GetFieldGroupID();
}

int GWarpInfoCollector::GetFieldID(int nIndex) const
{
	const GWarpInfoChecker* pWarpInfoChecker = GetWarpInfoChecker(nIndex);
	VALID_RET(pWarpInfoChecker != NULL, INVALID_ID);

	return pWarpInfoChecker->GetFieldID();
}

int GWarpInfoCollector::GetMarkerID(int nIndex) const
{	
	const GWarpInfoChecker* pWarpInfoChecker = GetWarpInfoChecker(nIndex);
	VALID_RET(pWarpInfoChecker != NULL, INVALID_ID);

	return pWarpInfoChecker->GetMarkerID();
}

bool GWarpInfoCollector::IsTrialWarp(int nIndex) const
{
	const GWarpInfoChecker* pWarpInfoChecker = GetWarpInfoChecker(nIndex);
	VALID_RET(pWarpInfoChecker != NULL, false);

	return pWarpInfoChecker->IsTrialWarp();
}

bool GWarpInfoCollector::IsSharedWarp(int nIndex) const
{
	const GWarpInfoChecker* pWarpInfoChecker = GetWarpInfoChecker(nIndex);
	VALID_RET(pWarpInfoChecker != NULL, false);

	return pWarpInfoChecker->IsSharedWarp();
}

bool GWarpInfoCollector::IsManualTrialWarp(int nIndex) const
{
	const GWarpInfoChecker* pWarpInfoChecker = GetWarpInfoChecker(nIndex);
	VALID_RET(pWarpInfoChecker != NULL, false);

	return pWarpInfoChecker->IsManualTrialWarp();
}

bool GWarpInfoCollector::IsSingleQuestWarp(int nIndex) const
{
	const GWarpInfoChecker* pWarpInfoChecker = GetWarpInfoChecker(nIndex);
	VALID_RET(pWarpInfoChecker != NULL, false);

	return pWarpInfoChecker->IsSingleQuestWarp();
}

const SENSOR_WARP_INFO* GWarpInfoCollector::GetSensorWarpInfo(int nIndex) const
{
	const GWarpInfoChecker* pWarpInfoChecker = GetWarpInfoChecker(nIndex);
	VALID_RET(pWarpInfoChecker != NULL, NULL);

	return pWarpInfoChecker->GetSensorWarpInfo();
}

GWarpInfoCollector::Result GWarpInfoCollector::RecheckWarp(int nIndex, GEntityPlayer* pPlayer) const
{
	Result recheckResult = NOTHING;

	// 선택된 워프 확인
	const GWarpInfoChecker* pWarpInfoChecker = GetWarpInfoChecker(nIndex);
	VALID_RET(pWarpInfoChecker != NULL, NOTHING);

	// 같은 대상 Field를 가진 워프 재검사
	for(vec_warpinfo::const_iterator it = m_vecWarpInfo.begin(); it != m_vecWarpInfo.end(); it++)
	{
		const GWarpInfoChecker* pChecker = it->first;

		if (pWarpInfoChecker->SameFieldWarp(pChecker))
		{
			// 컨디션 검사
			if (!pWarpInfoChecker->CheckCondition(pPlayer))	continue;			
			
			// 트라이얼 필드가 아니면 성공
			if (!pWarpInfoChecker->IsTrialWarp())	return SINGLE;
			
			// 트라이얼 필드는 퀘스트 검사
			GWarpInfoChecker::Result checkResult = pWarpInfoChecker->CheckQuestField(pPlayer);
			if (checkResult == GWarpInfoChecker::SUCCESS)	return SINGLE;

			// 만원의 경우는 계속 검사.
			if (checkResult == GWarpInfoChecker::FULLHOUSE)
			{
				recheckResult = FULLHOUSE;
				continue;
			}
		}
	}
	
	// 검사 통과된 워프 없음
	return recheckResult;
}

int GWarpInfoCollector::GetAllowFollowWarpCount(int nIndex, GEntityPlayer* pPlayer) const
{
	const GWarpInfoChecker* pWarpInfoChecker = GetWarpInfoChecker(nIndex);
	VALID_RET(pWarpInfoChecker != NULL, 0);

	if (!pWarpInfoChecker->IsManualTrialWarp())	return 0;


	// 입장 능한 퀘스트의 인원제한 확인
	int nLargestAllowCount = 0;
	for(vec_warpinfo::const_iterator it = m_vecWarpInfo.begin(); it != m_vecWarpInfo.end(); it++)
	{
		const GWarpInfoChecker* pChecker = it->first;

		if (!pWarpInfoChecker->SameFieldWarp(pChecker))	continue;

		// 컨디션 검사
		if (!pWarpInfoChecker->CheckCondition(pPlayer))	continue;
					
		// 트라이얼 필드는 퀘스트 검사
		GWarpInfoChecker::Result checkResult = pWarpInfoChecker->CheckQuestField(pPlayer);
		if (checkResult != GWarpInfoChecker::SUCCESS)	continue;

		// 가장 큰 여유 인원수를 구한다. 따라올 수 있는 수는 입장 가능 수에서 자기자신을 뺀수
		int nEnterAllowCount = pChecker->GetAllowEnterPlayerCount(pPlayer);		
		nLargestAllowCount = max(nLargestAllowCount, nEnterAllowCount - 1);
	}

	return nLargestAllowCount;
}

void GWarpInfoCollector::AddExtractField(int nWarpInfoIndex)
{
	VALID(0 <= nWarpInfoIndex);
	VALID(nWarpInfoIndex < (int)m_vecWarpInfo.size());

	// 같은 필드가 있는 확인한다.
	for(vec_extract::iterator it = m_vecExtract.begin(); it != m_vecExtract.end(); it++)
	{		
		int nIndex = *it;
		VALID(0 <= nIndex);
		VALID(nIndex < (int)m_vecWarpInfo.size());

		if (SameFieldChecker(nWarpInfoIndex, nIndex))
		{
			// 같은 필드가 만원이면, 기존정보 교체
			bool isFullHouse = m_vecWarpInfo[nIndex].second;
			if (isFullHouse)
			{
				*it = nWarpInfoIndex;
			}

			// 같은 필드는 추가하지 않는다.
			return;
		}
	}

	// 같은 필드가 없으면 추가
	m_vecExtract.push_back(nWarpInfoIndex);
}

bool GWarpInfoCollector::SameFieldChecker(int nWarpInfoIndex1, int nWarpInfoIndex2) const
{
	VALID_RET(0 <= nWarpInfoIndex1, false);
	VALID_RET(0 <= nWarpInfoIndex2, false);
	VALID_RET(nWarpInfoIndex1 < (int)m_vecWarpInfo.size(), false);
	VALID_RET(nWarpInfoIndex2 < (int)m_vecWarpInfo.size(), false);

	GWarpInfoChecker* pWarpInfo1 = m_vecWarpInfo[nWarpInfoIndex1].first;
	GWarpInfoChecker* pWarpInfo2 = m_vecWarpInfo[nWarpInfoIndex2].first;
	VALID_RET(pWarpInfo1 != NULL, false);
	VALID_RET(pWarpInfo2 != NULL, false);

	return pWarpInfo1->SameFieldWarp(pWarpInfo2);
}

GWarpInfoCollector::Result GWarpInfoCollector::GetExtractResult(void) const
{
	int nCount = m_vecExtract.size();

	// 필드가 두개 이상이면 선택
	if (2 <= nCount)
	{
		return MULTY;
	}

	// 필드가 한개이면 만원인지 구분
	if (1 == nCount)
	{
		int nIndex = m_vecExtract.front();
		VALID_RET(0 <= nIndex, NOTHING);
		VALID_RET(nIndex < (int)m_vecWarpInfo.size(), NOTHING);

		bool isFullHouse = m_vecWarpInfo[nIndex].second;		
		return isFullHouse ? FULLHOUSE : SINGLE;
	}

	// 필드 없다
	return NOTHING;
}

const GWarpInfoChecker* GWarpInfoCollector::GetWarpInfoChecker(int nWarpInfoIndex) const
{
	VALID_RET(0 <= nWarpInfoIndex, NULL);
	VALID_RET(nWarpInfoIndex < (int)m_vecWarpInfo.size(), NULL);

	GWarpInfoChecker* pWarpInfoChecker = m_vecWarpInfo[nWarpInfoIndex].first;
	VALID_RET(pWarpInfoChecker != NULL, NULL);

	return pWarpInfoChecker;
}
